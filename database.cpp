#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
DatabaseManager* DatabaseManager::m_inst=nullptr;
DatabaseManager* DatabaseManager::getInstance(){
    if(!m_inst)m_inst=new DatabaseManager();
    return m_inst;
}
DatabaseManager::DatabaseManager(){}
DatabaseManager::~DatabaseManager(){closeDatabase();}
bool DatabaseManager::openDatabase(const QString&path){
    m_db=QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    if(!m_db.open())throw DatabaseException("Не удалось открыть БД: "+m_db.lastError().text());
    createTables();
    seedDefaultData();
    return true;
}
void DatabaseManager::closeDatabase(){if(m_db.isOpen())m_db.close();}
void DatabaseManager::createTables(){
    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS users("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "name TEXT NOT NULL,email TEXT NOT NULL UNIQUE,"
           "password TEXT NOT NULL,role TEXT DEFAULT 'client',"
           "created_at TEXT)");
    q.exec("CREATE TABLE IF NOT EXISTS tours("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "from_city TEXT,to_city TEXT,airline TEXT,flight_num TEXT,"
           "departure TEXT,arrival TEXT,price REAL,total_seats INTEGER,"
           "avail_seats INTEGER,class TEXT,description TEXT,active INTEGER DEFAULT 1)");
    q.exec("CREATE TABLE IF NOT EXISTS bookings("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "user_id INTEGER,tour_id INTEGER,seats INTEGER,"
           "total REAL,booking_date TEXT,status TEXT DEFAULT 'new',"
           "FOREIGN KEY(user_id)REFERENCES users(id),"
           "FOREIGN KEY(tour_id)REFERENCES tours(id))");
}
void DatabaseManager::seedDefaultData(){
    QSqlQuery q;
    q.exec("SELECT COUNT(*)FROM users WHERE role='admin'");
    if(q.next()&&q.value(0).toInt()==0){
        QSqlQuery i;
        i.prepare("INSERT INTO users(name,email,password,role,created_at)VALUES(?,?,?,?,?)");
        i.addBindValue("Администратор");
        i.addBindValue("admin@sky.com");
        i.addBindValue(User::hashPassword("admin123"));
        i.addBindValue("admin");
        i.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
        i.exec();
    }
    q.exec("SELECT COUNT(*)FROM tours");
    if(q.next()&&q.value(0).toInt()==0){
        // Добавляем несколько туров-примеров
        struct TData{const char*from,*to,*air,*fn,*dep,*arr;double price;int seats;const char*cls;};
        TData tours[]={
            {"Алматы","Дубай","Emirates","EK392","2024-07-10 06:00","2024-07-10 09:30",350,120,"Эконом"},
            {"Алматы","Стамбул","Turkish Airlines","TK352","2024-07-15 14:00","2024-07-15 18:45",290,150,"Эконом"},
            {"Алматы","Москва","Аэрофлот","SU1852","2024-07-20 08:00","2024-07-20 11:00",180,200,"Эконом"},
            {"Алматы","Дубай","Emirates","EK393","2024-07-25 22:00","2024-07-26 01:30",550,40,"Бизнес"},
            {"Алматы","Бангкок","Thai Airways","TG320","2024-08-01 01:00","2024-08-01 09:00",420,100,"Эконом"},
        };
        for(auto&t:tours){
            QSqlQuery ins;
            ins.prepare("INSERT INTO tours(from_city,to_city,airline,flight_num,departure,arrival,"
                        "price,total_seats,avail_seats,class,description,active)VALUES(?,?,?,?,?,?,?,?,?,?,?,1)");
            ins.addBindValue(QString(t.from));
            ins.addBindValue(QString(t.to));
            ins.addBindValue(QString(t.air));
            ins.addBindValue(QString(t.fn));
            ins.addBindValue(QString(t.dep));
            ins.addBindValue(QString(t.arr));
            ins.addBindValue(t.price);
            ins.addBindValue(t.seats);
            ins.addBindValue(t.seats);
            ins.addBindValue(QString(t.cls));
            ins.addBindValue(QString("Прямой рейс"));
            ins.exec();
        }
    }
}
// ---- Users ----
bool DatabaseManager::addUser(User*u){
    if(!u)return false;
    QSqlQuery q;
    q.prepare("INSERT INTO users(name,email,password,role,created_at)VALUES(?,?,?,?,?)");
    q.addBindValue(u->getName()); q.addBindValue(u->getEmail());
    q.addBindValue(u->getPassword()); q.addBindValue(u->getRole());
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    if(!q.exec())throw DatabaseException(q.lastError().text());
    return true;
}
User* DatabaseManager::getUserByEmail(const QString&email){
    QSqlQuery q;
    q.prepare("SELECT id,name,email,password,role FROM users WHERE email=?");
    q.addBindValue(email); q.exec();
    if(!q.next())return nullptr;
    int id=q.value(0).toInt();
    QString n=q.value(1).toString(),e=q.value(2).toString(),
            p=q.value(3).toString(),r=q.value(4).toString();
    if(r=="admin")return new Admin(id,n,e,p);
    return new Client(id,n,e,p);
}
QList<User*> DatabaseManager::getAllUsers(){
    QList<User*>list;
    QSqlQuery q("SELECT id,name,email,password,role FROM users");
    while(q.next()){
        int id=q.value(0).toInt();
        QString n=q.value(1).toString(),e=q.value(2).toString(),
                p=q.value(3).toString(),r=q.value(4).toString();
        if(r=="admin")list<<new Admin(id,n,e,p);
        else list<<new Client(id,n,e,p);
    }
    return list;
}
// ---- Tours ----
static Tour* rowToTour(QSqlQuery&q){
    Tour*t=new Tour();
    t->setId(q.value(0).toInt());
    t->setFromCity(q.value(1).toString());
    t->setToCity(q.value(2).toString());
    t->setAirline(q.value(3).toString());
    t->setFlightNum(q.value(4).toString());
    t->setDeparture(QDateTime::fromString(q.value(5).toString(),"yyyy-MM-dd hh:mm"));
    t->setArrival(QDateTime::fromString(q.value(6).toString(),"yyyy-MM-dd hh:mm"));
    t->setPrice(q.value(7).toDouble());
    t->setTotalSeats(q.value(8).toInt());
    t->setAvailSeats(q.value(9).toInt());
    t->setTourClass(q.value(10).toString());
    t->setDescription(q.value(11).toString());
    t->setActive(q.value(12).toInt()==1);
    return t;
}
bool DatabaseManager::addTour(Tour*t){
    if(!t)return false;
    QSqlQuery q;
    q.prepare("INSERT INTO tours(from_city,to_city,airline,flight_num,departure,arrival,"
              "price,total_seats,avail_seats,class,description,active)VALUES(?,?,?,?,?,?,?,?,?,?,?,1)");
    q.addBindValue(t->getFromCity()); q.addBindValue(t->getToCity());
    q.addBindValue(t->getAirline()); q.addBindValue(t->getFlightNum());
    q.addBindValue(t->getDeparture().toString("yyyy-MM-dd hh:mm"));
    q.addBindValue(t->getArrival().toString("yyyy-MM-dd hh:mm"));
    q.addBindValue(t->getPrice()); q.addBindValue(t->getTotalSeats());
    q.addBindValue(t->getAvailSeats()); q.addBindValue(t->getTourClass());
    q.addBindValue(t->getDescription());
    if(!q.exec())throw DatabaseException(q.lastError().text());
    t->setId(q.lastInsertId().toInt());
    return true;
}
bool DatabaseManager::updateTour(Tour*t){
    if(!t)return false;
    QSqlQuery q;
    q.prepare("UPDATE tours SET from_city=?,to_city=?,airline=?,flight_num=?,"
              "departure=?,arrival=?,price=?,total_seats=?,avail_seats=?,"
              "class=?,description=?,active=? WHERE id=?");
    q.addBindValue(t->getFromCity()); q.addBindValue(t->getToCity());
    q.addBindValue(t->getAirline()); q.addBindValue(t->getFlightNum());
    q.addBindValue(t->getDeparture().toString("yyyy-MM-dd hh:mm"));
    q.addBindValue(t->getArrival().toString("yyyy-MM-dd hh:mm"));
    q.addBindValue(t->getPrice()); q.addBindValue(t->getTotalSeats());
    q.addBindValue(t->getAvailSeats()); q.addBindValue(t->getTourClass());
    q.addBindValue(t->getDescription()); q.addBindValue(t->isActive()?1:0);
    q.addBindValue(t->getId());
    return q.exec();
}
bool DatabaseManager::deleteTour(int id){
    QSqlQuery q;
    q.prepare("UPDATE tours SET active=0 WHERE id=?");
    q.addBindValue(id); return q.exec();
}
Tour* DatabaseManager::getTourById(int id){
    QSqlQuery q;
    q.prepare("SELECT id,from_city,to_city,airline,flight_num,departure,arrival,"
              "price,total_seats,avail_seats,class,description,active FROM tours WHERE id=?");
    q.addBindValue(id); q.exec();
    if(!q.next())return nullptr;
    return rowToTour(q);
}
QList<Tour*> DatabaseManager::getAllTours(bool activeOnly){
    QList<Tour*>list;
    QString sql="SELECT id,from_city,to_city,airline,flight_num,departure,arrival,"
                "price,total_seats,avail_seats,class,description,active FROM tours";
    if(activeOnly)sql+=" WHERE active=1";
    sql+=" ORDER BY departure";
    QSqlQuery q(sql);
    while(q.next())list<<rowToTour(q);
    return list;
}
QList<Tour*> DatabaseManager::searchTours(const QString&from,const QString&to,
                                            const QDate&date,const QString&cls){
    QList<Tour*>list;
    QSqlQuery q;
    QString sql="SELECT id,from_city,to_city,airline,flight_num,departure,arrival,"
                "price,total_seats,avail_seats,class,description,active FROM tours"
                " WHERE active=1 AND avail_seats>0";
    if(!from.isEmpty()) sql+=" AND from_city LIKE '%" + from + "%'";
    if(!to.isEmpty())   sql+=" AND to_city LIKE '%" + to + "%'";
    if(date.isValid())  sql+=" AND date(departure)='"+date.toString("yyyy-MM-dd")+"'";
    if(!cls.isEmpty()&&cls!="Все") sql+=" AND class='"+cls+"'";
    sql+=" ORDER BY price";
    q.exec(sql);
    while(q.next())list<<rowToTour(q);
    return list;
}
bool DatabaseManager::updateSeats(int tourId,int delta){
    QSqlQuery q;
    q.prepare("UPDATE tours SET avail_seats=avail_seats+? WHERE id=?");
    q.addBindValue(delta); q.addBindValue(tourId);
    return q.exec();
}
// ---- Bookings ----
bool DatabaseManager::addBooking(TourBooking*b){
    if(!b)return false;
    m_db.transaction();
    try{
        QSqlQuery q;
        q.prepare("INSERT INTO bookings(user_id,tour_id,seats,total,booking_date,status)"
                  "VALUES(?,?,?,?,?,?)");
        q.addBindValue(b->getUserId()); q.addBindValue(b->getTourId());
        q.addBindValue(b->getSeats()); q.addBindValue(b->getTotal());
        q.addBindValue(b->getDate().toString(Qt::ISODate));
        q.addBindValue(b->getStatus());
        if(!q.exec())throw DatabaseException(q.lastError().text());
        b->setId(q.lastInsertId().toInt());
        if(!updateSeats(b->getTourId(),-b->getSeats()))
            throw DatabaseException("Ошибка обновления мест");
        m_db.commit();
    }catch(...){m_db.rollback();throw;}
    return true;
}
bool DatabaseManager::updateBookingStatus(int id,const QString&status){
    QSqlQuery q;
    q.prepare("UPDATE bookings SET status=? WHERE id=?");
    q.addBindValue(status); q.addBindValue(id);
    return q.exec();
}
static TourBooking* rowToBooking(QSqlQuery&q){
    TourBooking*b=new TourBooking(q.value(1).toInt(),q.value(2).toInt(),
                                   q.value(3).toInt(),q.value(4).toDouble());
    b->setId(q.value(0).toInt());
    b->setDate(QDateTime::fromString(q.value(5).toString(),Qt::ISODate));
    b->setStatus(q.value(6).toString());
    return b;
}
QList<TourBooking*> DatabaseManager::getBookingsByUser(int uid){
    QList<TourBooking*>list;
    QSqlQuery q;
    q.prepare("SELECT id,user_id,tour_id,seats,total,booking_date,status"
              " FROM bookings WHERE user_id=? ORDER BY id DESC");
    q.addBindValue(uid); q.exec();
    while(q.next())list<<rowToBooking(q);
    return list;
}
QList<TourBooking*> DatabaseManager::getAllBookings(){
    QList<TourBooking*>list;
    QSqlQuery q("SELECT id,user_id,tour_id,seats,total,booking_date,status"
                " FROM bookings ORDER BY id DESC");
    while(q.next())list<<rowToBooking(q);
    return list;
}
TourBooking* DatabaseManager::getBookingById(int id){
    QSqlQuery q;
    q.prepare("SELECT id,user_id,tour_id,seats,total,booking_date,status"
              " FROM bookings WHERE id=?");
    q.addBindValue(id); q.exec();
    if(!q.next())return nullptr;
    return rowToBooking(q);
}
