#include "adminwindow.h"
#include "appstyle.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QTextEdit>
#include <QDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QStatusBar>
#include <QFrame>
#include <QScrollArea>
AdminWindow::AdminWindow(User *user, QWidget *parent)
    :QMainWindow(parent),m_user(user),m_db(DatabaseManager::getInstance()){
    setStyleSheet(AppStyle::mainStyle());
    setupUI();
    setWindowTitle("SkyTickets — Панель администратора");
    resize(1150,700);
    statusBar()->showMessage("  ⚙  Панель администратора | "+user->getName());
}
AdminWindow::~AdminWindow(){delete m_user;}
void AdminWindow::setupUI(){
    m_tabs=new QTabWidget(this);
    m_tabs->addTab(makeToursTab(),   "  ✈  Рейсы  ");
    m_tabs->addTab(makeBookingsTab(),"  📋  Бронирования  ");
    m_tabs->addTab(makeStatsTab(),   "  📊  Статистика  ");
    m_tabs->addTab(makeProfileTab(), "  👤  Профиль  ");
    setCentralWidget(m_tabs);
    connect(m_tabs,&QTabWidget::currentChanged,this,[this](int i){
        if(i==1)loadBookings();
        if(i==2)loadStats();
    });
}

// ========== ВКЛАДКА РЕЙСЫ ==========
QWidget* AdminWindow::makeToursTab(){
    QWidget *tab=new QWidget;
    QVBoxLayout *v=new QVBoxLayout(tab);
    v->setContentsMargins(20,20,20,10); v->setSpacing(10);
    QLabel *h=new QLabel("Управление рейсами",tab);
    h->setStyleSheet("font-size:18px;font-weight:bold;color:#0D47A1;");
    v->addWidget(h);
    m_tblTours=new QTableWidget(0,9,tab);
    m_tblTours->setHorizontalHeaderLabels(
        {"ID","Откуда","Куда","Авиакомпания","Рейс","Вылет","Прилёт","Цена ($)","Мест"});
    m_tblTours->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tblTours->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tblTours->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tblTours->setAlternatingRowColors(true);
    v->addWidget(m_tblTours,1);
    QHBoxLayout *bh=new QHBoxLayout;
    QPushButton *btnAdd =new QPushButton("+ Добавить рейс",tab);
    btnAdd->setObjectName("btnGreen");
    QPushButton *btnEdit=new QPushButton("✏ Редактировать",tab);
    btnEdit->setObjectName("btnSecondary");
    QPushButton *btnDel =new QPushButton("🗑 Удалить",tab);
    btnDel->setObjectName("btnDanger");
    QPushButton *btnRef =new QPushButton("⟳ Обновить",tab);
    bh->addWidget(btnAdd); bh->addWidget(btnEdit); bh->addWidget(btnDel);
    bh->addStretch(); bh->addWidget(btnRef);
    v->addLayout(bh);
    connect(btnAdd, &QPushButton::clicked,this,&AdminWindow::onAddTour);
    connect(btnEdit,&QPushButton::clicked,this,&AdminWindow::onEditTour);
    connect(btnDel, &QPushButton::clicked,this,&AdminWindow::onDeleteTour);
    connect(btnRef, &QPushButton::clicked,this,[this](){loadTours();});
    loadTours();
    return tab;
}
void AdminWindow::loadTours(){
    auto tours=m_db->getAllTours(false);
    m_tblTours->setRowCount(tours.size());
    for(int i=0;i<tours.size();++i){
        Tour *t=tours[i];
        m_tblTours->setItem(i,0,new QTableWidgetItem(QString::number(t->getId())));
        m_tblTours->setItem(i,1,new QTableWidgetItem(t->getFromCity()));
        m_tblTours->setItem(i,2,new QTableWidgetItem(t->getToCity()));
        m_tblTours->setItem(i,3,new QTableWidgetItem(t->getAirline()));
        m_tblTours->setItem(i,4,new QTableWidgetItem(t->getFlightNum()));
        m_tblTours->setItem(i,5,new QTableWidgetItem(t->getDeparture().toString("dd.MM.yyyy hh:mm")));
        m_tblTours->setItem(i,6,new QTableWidgetItem(t->getArrival().toString("dd.MM.yyyy hh:mm")));
        m_tblTours->setItem(i,7,new QTableWidgetItem(QString::number(t->getPrice(),'f',2)));
        auto *seatsItem=new QTableWidgetItem(
            QString("%1 / %2").arg(t->getAvailSeats()).arg(t->getTotalSeats()));
        seatsItem->setForeground(t->getAvailSeats()==0?QColor("#C62828"):QColor("#212121"));
        m_tblTours->setItem(i,8,seatsItem);
        if(!t->isActive()){
            for(int c=0;c<9;++c)
                if(m_tblTours->item(i,c))
                    m_tblTours->item(i,c)->setForeground(QColor("#BDBDBD"));
        }
        delete t;
    }
}

// ========== ДИАЛОГ ТУРА ==========
Tour* AdminWindow::showTourDialog(Tour *existing){
    QDialog dlg(this);
    dlg.setWindowTitle(existing?"Редактировать рейс":"Добавить рейс");
    dlg.setMinimumWidth(500);
    dlg.setStyleSheet(AppStyle::mainStyle());
    QVBoxLayout *dv=new QVBoxLayout(&dlg);
    dv->setContentsMargins(24,24,24,16); dv->setSpacing(0);
    QLabel *title=new QLabel(existing?"✏ Редактировать рейс":"✈ Новый рейс",&dlg);
    title->setStyleSheet("font-size:16px;font-weight:bold;color:#0D47A1;margin-bottom:12px;");
    dv->addWidget(title);
    QFormLayout *form=new QFormLayout;
    form->setSpacing(10); form->setContentsMargins(0,0,0,12);
    auto mkEdit=[&](const QString&ph)->QLineEdit*{
        auto*e=new QLineEdit(&dlg); e->setPlaceholderText(ph); return e;
    };
    QLineEdit *leFrom   =mkEdit("Алматы");
    QLineEdit *leTo     =mkEdit("Дубай");
    QLineEdit *leAirline=mkEdit("Emirates");
    QLineEdit *leFlight =mkEdit("EK392");
    QDateTimeEdit *dtDep=new QDateTimeEdit(QDateTime::currentDateTime(),&dlg);
    dtDep->setDisplayFormat("dd.MM.yyyy  hh:mm");
    dtDep->setCalendarPopup(true);
    QDateTimeEdit *dtArr=new QDateTimeEdit(QDateTime::currentDateTime().addSecs(3*3600),&dlg);
    dtArr->setDisplayFormat("dd.MM.yyyy  hh:mm");
    dtArr->setCalendarPopup(true);
    QDoubleSpinBox *spPrice=new QDoubleSpinBox(&dlg);
    spPrice->setRange(1,99999); spPrice->setDecimals(2); spPrice->setSuffix(" $");
    spPrice->setValue(200);
    QSpinBox *spSeats=new QSpinBox(&dlg);
    spSeats->setRange(1,999); spSeats->setValue(100);
    QComboBox *cmbClass=new QComboBox(&dlg);
    cmbClass->addItems({"Эконом","Бизнес"});
    QTextEdit *teDesc=new QTextEdit(&dlg);
    teDesc->setPlaceholderText("Описание рейса...");
    teDesc->setMaximumHeight(70);
    form->addRow("Откуда *:",leFrom);
    form->addRow("Куда *:",leTo);
    form->addRow("Авиакомпания *:",leAirline);
    form->addRow("Номер рейса *:",leFlight);
    form->addRow("Дата и время вылета *:",dtDep);
    form->addRow("Дата и время прилёта *:",dtArr);
    form->addRow("Цена за билет *:",spPrice);
    form->addRow("Количество мест *:",spSeats);
    form->addRow("Класс:",cmbClass);
    form->addRow("Описание:",teDesc);
    dv->addLayout(form);
    if(existing){
        leFrom->setText(existing->getFromCity());
        leTo->setText(existing->getToCity());
        leAirline->setText(existing->getAirline());
        leFlight->setText(existing->getFlightNum());
        dtDep->setDateTime(existing->getDeparture());
        dtArr->setDateTime(existing->getArrival());
        spPrice->setValue(existing->getPrice());
        spSeats->setValue(existing->getTotalSeats());
        cmbClass->setCurrentText(existing->getTourClass());
        teDesc->setPlainText(existing->getDescription());
    }
    QHBoxLayout *bh=new QHBoxLayout;
    QPushButton *btnSave=new QPushButton("Сохранить",&dlg);
    btnSave->setObjectName("btnGreen");
    QPushButton *btnCan =new QPushButton("Отмена",&dlg);
    btnCan->setObjectName("btnGhost");
    bh->addWidget(btnSave); bh->addWidget(btnCan);
    dv->addLayout(bh);
    Tour *result=nullptr;
    connect(btnSave,&QPushButton::clicked,&dlg,[&](){
        if(leFrom->text().trimmed().isEmpty()||leTo->text().trimmed().isEmpty()||
           leAirline->text().trimmed().isEmpty()||leFlight->text().trimmed().isEmpty()){
            QMessageBox::warning(&dlg,"","Заполните обязательные поля (*).");return;
        }
        if(dtArr->dateTime()<=dtDep->dateTime()){
            QMessageBox::warning(&dlg,"","Время прилёта должно быть позже вылета.");return;
        }
        Tour *t=existing?existing:new Tour();
        t->setFromCity(leFrom->text().trimmed());
        t->setToCity(leTo->text().trimmed());
        t->setAirline(leAirline->text().trimmed());
        t->setFlightNum(leFlight->text().trimmed());
        t->setDeparture(dtDep->dateTime());
        t->setArrival(dtArr->dateTime());
        t->setPrice(spPrice->value());
        if(!existing){t->setTotalSeats(spSeats->value());t->setAvailSeats(spSeats->value());}
        t->setTourClass(cmbClass->currentText());
        t->setDescription(teDesc->toPlainText().trimmed());
        t->setActive(true);
        result=t;
        dlg.accept();
    });
    connect(btnCan,&QPushButton::clicked,&dlg,&QDialog::reject);
    if(dlg.exec()!=QDialog::Accepted)return nullptr;
    return result;
}
void AdminWindow::onAddTour(){
    Tour *t=showTourDialog(nullptr);
    if(!t)return;
    try{
        m_db->addTour(t);
        QMessageBox::information(this,"Готово",
            QString("Рейс добавлен: %1 → %2 (#%3)")
            .arg(t->getFromCity()).arg(t->getToCity()).arg(t->getId()));
        loadTours();
        statusBar()->showMessage("Рейс добавлен: "+t->getFlightNum());
    }catch(const DatabaseException&ex){
        QMessageBox::critical(this,"Ошибка БД",ex.getMessage());
    }
    delete t;
}
void AdminWindow::onEditTour(){
    int row=m_tblTours->currentRow();
    if(row<0){QMessageBox::information(this,"","Выберите рейс.");return;}
    int id=m_tblTours->item(row,0)->text().toInt();
    Tour *t=m_db->getTourById(id);
    if(!t)return;
    Tour *updated=showTourDialog(t);
    if(updated){
        try{
            m_db->updateTour(updated);
            QMessageBox::information(this,"Готово","Рейс обновлён.");
            loadTours();
        }catch(const DatabaseException&ex){
            QMessageBox::critical(this,"Ошибка БД",ex.getMessage());
        }
    }
    delete t;
}
void AdminWindow::onDeleteTour(){
    int row=m_tblTours->currentRow();
    if(row<0){QMessageBox::information(this,"","Выберите рейс.");return;}
    int id=m_tblTours->item(row,0)->text().toInt();
    QString from=m_tblTours->item(row,1)->text();
    QString to  =m_tblTours->item(row,2)->text();
    if(QMessageBox::question(this,"Удалить рейс",
        QString("Переместить рейс %1 → %2 в архив?").arg(from).arg(to),
        QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes)return;
    m_db->deleteTour(id);
    loadTours();
    statusBar()->showMessage("Рейс перемещён в архив.");
}

// ========== ВКЛАДКА БРОНИРОВАНИЯ ==========
QWidget* AdminWindow::makeBookingsTab(){
    QWidget *tab=new QWidget;
    QVBoxLayout *v=new QVBoxLayout(tab);
    v->setContentsMargins(20,20,20,10); v->setSpacing(10);
    QLabel *h=new QLabel("Все бронирования",tab);
    h->setStyleSheet("font-size:18px;font-weight:bold;color:#0D47A1;");
    v->addWidget(h);
    m_tblBookings=new QTableWidget(0,8,tab);
    m_tblBookings->setHorizontalHeaderLabels(
        {"#","Клиент ID","Рейс","Откуда","Куда","Мест","Сумма ($)","Статус"});
    m_tblBookings->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tblBookings->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tblBookings->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tblBookings->setAlternatingRowColors(true);
    v->addWidget(m_tblBookings,1);
    QHBoxLayout *bh=new QHBoxLayout;
    QPushButton *btnConf=new QPushButton("✔ Подтвердить",tab);
    btnConf->setObjectName("btnGreen");
    QPushButton *btnCan =new QPushButton("✘ Отменить",tab);
    btnCan->setObjectName("btnDanger");
    QPushButton *btnRef =new QPushButton("⟳ Обновить",tab);
    bh->addWidget(btnConf); bh->addWidget(btnCan);
    bh->addStretch(); bh->addWidget(btnRef);
    v->addLayout(bh);
    connect(btnConf,&QPushButton::clicked,this,&AdminWindow::onConfirmBooking);
    connect(btnCan, &QPushButton::clicked,this,&AdminWindow::onCancelBooking);
    connect(btnRef, &QPushButton::clicked,this,&AdminWindow::onRefreshAll);
    loadBookings();
    return tab;
}
void AdminWindow::loadBookings(){
    auto list=m_db->getAllBookings();
    m_tblBookings->setRowCount(list.size());
    for(int i=0;i<list.size();++i){
        TourBooking*b=list[i];
        Tour*t=m_db->getTourById(b->getTourId());
        m_tblBookings->setItem(i,0,new QTableWidgetItem(QString::number(b->getId())));
        m_tblBookings->setItem(i,1,new QTableWidgetItem(QString::number(b->getUserId())));
        m_tblBookings->setItem(i,2,new QTableWidgetItem(t?t->getAirline()+" "+t->getFlightNum():"—"));
        m_tblBookings->setItem(i,3,new QTableWidgetItem(t?t->getFromCity():"—"));
        m_tblBookings->setItem(i,4,new QTableWidgetItem(t?t->getToCity():"—"));
        m_tblBookings->setItem(i,5,new QTableWidgetItem(QString::number(b->getSeats())));
        m_tblBookings->setItem(i,6,new QTableWidgetItem(QString::number(b->getTotal(),'f',2)));
        auto *st=new QTableWidgetItem(b->getStatus());
        if(b->getStatus()=="paid")     st->setForeground(QColor("#2E7D32"));
        else if(b->getStatus()=="cancelled")st->setForeground(QColor("#C62828"));
        else                           st->setForeground(QColor("#E65100"));
        m_tblBookings->setItem(i,7,st);
        delete b; delete t;
    }
}
void AdminWindow::onConfirmBooking(){
    int row=m_tblBookings->currentRow();
    if(row<0){QMessageBox::information(this,"","Выберите бронирование.");return;}
    int id=m_tblBookings->item(row,0)->text().toInt();
    m_db->updateBookingStatus(id,"paid");
    loadBookings();
    statusBar()->showMessage("Бронирование #"+QString::number(id)+" подтверждено.");
}
void AdminWindow::onCancelBooking(){
    int row=m_tblBookings->currentRow();
    if(row<0){QMessageBox::information(this,"","Выберите бронирование.");return;}
    int id=m_tblBookings->item(row,0)->text().toInt();
    TourBooking*b=m_db->getBookingById(id);
    if(b){
        m_db->updateBookingStatus(id,"cancelled");
        m_db->updateSeats(b->getTourId(),b->getSeats());
        delete b;
    }
    loadBookings(); loadTours();
    statusBar()->showMessage("Бронирование #"+QString::number(id)+" отменено.");
}
void AdminWindow::onRefreshAll(){loadBookings();}

// ========== ВКЛАДКА СТАТИСТИКА ==========
QWidget* AdminWindow::makeStatsTab(){
    QWidget *tab=new QWidget;
    QVBoxLayout *v=new QVBoxLayout(tab);
    v->setContentsMargins(20,20,20,20); v->setSpacing(16);
    QLabel *h=new QLabel("Статистика",tab);
    h->setStyleSheet("font-size:18px;font-weight:bold;color:#0D47A1;");
    v->addWidget(h);
    QScrollArea *sc=new QScrollArea(tab);
    sc->setWidgetResizable(true);
    sc->setFrameShape(QFrame::NoFrame);
    QWidget *inner=new QWidget;
    QVBoxLayout *iv=new QVBoxLayout(inner);
    iv->setSpacing(12);
    m_lblStats=new QLabel("Загрузка...",inner);
    m_lblStats->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    m_lblStats->setStyleSheet("background:white;border-radius:10px;padding:20px;font-size:14px;");
    m_lblStats->setWordWrap(true);
    iv->addWidget(m_lblStats);
    QPushButton *btnRef=new QPushButton("⟳ Обновить статистику",inner);
    btnRef->setObjectName("btnSecondary");
    btnRef->setMaximumWidth(220);
    iv->addWidget(btnRef,0,Qt::AlignLeft);
    iv->addStretch();
    sc->setWidget(inner);
    v->addWidget(sc,1);
    connect(btnRef,&QPushButton::clicked,this,&AdminWindow::loadStats);
    loadStats();
    return tab;
}
void AdminWindow::loadStats(){
    auto tours=m_db->getAllTours(true);
    auto allTours=m_db->getAllTours(false);
    auto bookings=m_db->getAllBookings();
    int paid=0,cancelled=0,pending=0;
    double revenue=0;
    for(TourBooking*b:bookings){
        if(b->getStatus()=="paid"){paid++;revenue+=b->getTotal();}
        else if(b->getStatus()=="cancelled")cancelled++;
        else pending++;
        delete b;
    }
    int totalSeats=0,availSeats=0;
    for(Tour*t:allTours){totalSeats+=t->getTotalSeats();delete t;}
    for(Tour*t:tours){availSeats+=t->getAvailSeats();delete t;}
    m_lblStats->setText(
        QString("<table cellpadding='10' cellspacing='0' width='100%'>"
        "<tr><td colspan='2' style='font-size:16px;font-weight:bold;color:#0D47A1;padding-bottom:16px;'>"
        "Сводная статистика системы</td></tr>"
        "<tr bgcolor='#F5F5F5'><td style='color:#757575;'>✈ Активных рейсов</td>"
        "<td style='font-weight:bold;font-size:16px;'>%1</td></tr>"
        "<tr><td style='color:#757575;'>📋 Всего бронирований</td>"
        "<td style='font-weight:bold;font-size:16px;'>%2</td></tr>"
        "<tr bgcolor='#F5F5F5'><td style='color:#2E7D32;'>✔ Оплачено</td>"
        "<td style='font-weight:bold;color:#2E7D32;font-size:16px;'>%3</td></tr>"
        "<tr><td style='color:#E65100;'>⏳ Ожидают</td>"
        "<td style='font-weight:bold;color:#E65100;font-size:16px;'>%4</td></tr>"
        "<tr bgcolor='#F5F5F5'><td style='color:#C62828;'>✘ Отменено</td>"
        "<td style='font-weight:bold;color:#C62828;font-size:16px;'>%5</td></tr>"
        "<tr><td style='color:#757575;'>💺 Мест занято / всего</td>"
        "<td style='font-weight:bold;font-size:16px;'>%6 / %7</td></tr>"
        "<tr bgcolor='#F5F5F5'><td style='color:#757575;'>💵 Суммарная выручка</td>"
        "<td style='font-weight:bold;font-size:18px;color:#FF6D00;'>$%8</td></tr>"
        "</table>")
        .arg(tours.size()).arg(paid+cancelled+pending)
        .arg(paid).arg(pending).arg(cancelled)
        .arg(totalSeats-availSeats).arg(totalSeats)
        .arg(revenue,0,'f',2));
}

// ========== ПРОФИЛЬ ==========
QWidget* AdminWindow::makeProfileTab(){
    QWidget *tab=new QWidget;
    QVBoxLayout *v=new QVBoxLayout(tab);
    v->setAlignment(Qt::AlignCenter);
    QFrame *card=new QFrame(tab);
    card->setObjectName("card");
    card->setFixedWidth(380);
    QVBoxLayout *cv=new QVBoxLayout(card);
    cv->setContentsMargins(30,30,30,30); cv->setSpacing(12);
    QLabel *ico=new QLabel("⚙",card);
    ico->setAlignment(Qt::AlignCenter);
    ico->setStyleSheet("font-size:48px;");
    QLabel *name=new QLabel(m_user->getName(),card);
    name->setAlignment(Qt::AlignCenter);
    name->setStyleSheet("font-size:20px;font-weight:bold;color:#0D47A1;");
    QLabel *email=new QLabel(m_user->getEmail(),card);
    email->setAlignment(Qt::AlignCenter);
    email->setStyleSheet("color:#757575;");
    QLabel *role=new QLabel("Роль: Администратор",card);
    role->setAlignment(Qt::AlignCenter);
    role->setObjectName("lblBadgeOrange");
    QPushButton *btnOut=new QPushButton("Выйти из системы",card);
    btnOut->setObjectName("btnDanger");
    cv->addWidget(ico); cv->addWidget(name); cv->addWidget(email);
    cv->addWidget(role,0,Qt::AlignCenter); cv->addSpacing(16); cv->addWidget(btnOut);
    v->addWidget(card);
    connect(btnOut,&QPushButton::clicked,this,&AdminWindow::onLogout);
    return tab;
}
void AdminWindow::onLogout(){
    if(QMessageBox::question(this,"Выход","Выйти из системы?",
        QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes) close();
}
