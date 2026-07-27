#ifndef DATABASE_H
#define DATABASE_H
#include <QSqlDatabase>
#include <QList>
#include "user.h"
#include "tour.h"
#include "booking.h"
class DatabaseManager {
public:
    static DatabaseManager* getInstance();
    bool openDatabase(const QString&path);
    void closeDatabase();
    // Users
    bool  addUser(User*u);
    User* getUserByEmail(const QString&email);
    QList<User*> getAllUsers();
    // Tours
    bool  addTour(Tour*t);
    bool  updateTour(Tour*t);
    bool  deleteTour(int id);
    Tour* getTourById(int id);
    QList<Tour*> getAllTours(bool activeOnly=true);
    QList<Tour*> searchTours(const QString&from,const QString&to,const QDate&date,const QString&cls);
    bool  updateSeats(int tourId,int delta);
    // Bookings
    bool  addBooking(TourBooking*b);
    bool  updateBookingStatus(int id,const QString&status);
    QList<TourBooking*> getBookingsByUser(int uid);
    QList<TourBooking*> getAllBookings();
    TourBooking* getBookingById(int id);
private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&)=delete;
    DatabaseManager&operator=(const DatabaseManager&)=delete;
    void createTables();
    void seedDefaultData();
    static DatabaseManager* m_inst;
    QSqlDatabase m_db;
};
#endif
