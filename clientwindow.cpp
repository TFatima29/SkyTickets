#include "clientwindow.h"
#include "tourcard.h"
#include "appstyle.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QHeaderView>
#include <QStatusBar>
#include <QScrollArea>
#include <QFrame>
ClientWindow::ClientWindow(User *user, QWidget *parent)
    :QMainWindow(parent),m_user(user),m_db(DatabaseManager::getInstance()){
    setStyleSheet(AppStyle::mainStyle());
    setupUI();
    setWindowTitle("SkyTickets");
    resize(1100,700);
    statusBar()->showMessage("  ✈  Добро пожаловать, "+user->getName()+"!");
}
ClientWindow::~ClientWindow(){delete m_user;}
void ClientWindow::setupUI(){
    m_tabs=new QTabWidget(this);
    m_tabs->setTabPosition(QTabWidget::North);
    m_tabs->addTab(makeSearchTab(),   "  ✈  Поиск билетов  ");
    m_tabs->addTab(makeBookingsTab(), "  📋  Мои билеты  ");
    m_tabs->addTab(makeProfileTab(),  "  👤  Профиль  ");
    setCentralWidget(m_tabs);
    connect(m_tabs,&QTabWidget::currentChanged,this,[this](int i){
        if(i==1)loadMyBookings();
    });
}
QWidget* ClientWindow::makeSearchTab(){
    QWidget *tab=new QWidget;
    QVBoxLayout *v=new QVBoxLayout(tab);
    v->setContentsMargins(20,20,20,20); v->setSpacing(16);
    // --- Шапка ---
    QFrame *searchBox=new QFrame(tab);
    searchBox->setObjectName("card");
    searchBox->setStyleSheet("QFrame#card{background:white;border-radius:12px;border:1px solid #E0E0E0;}");
    QVBoxLayout *sv=new QVBoxLayout(searchBox);
    sv->setContentsMargins(20,16,20,16);
    QLabel *heading=new QLabel("Куда летим?",searchBox);
    heading->setStyleSheet("font-size:20px;font-weight:bold;color:#0D47A1;");
    sv->addWidget(heading);
    QHBoxLayout *row=new QHBoxLayout;
    row->setSpacing(12);
    auto mkField=[&](const QString&ph)->QLineEdit*{
        auto *le=new QLineEdit(searchBox);
        le->setPlaceholderText(ph);
        le->setMinimumWidth(130);
        return le;
    };
    m_leFrom=mkField("Откуда");
    m_leTo  =mkField("Куда");
    m_dateEdit=new QDateEdit(QDate::currentDate(),searchBox);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("dd.MM.yyyy");
    m_dateEdit->setMinimumWidth(120);
    m_cmbClass=new QComboBox(searchBox);
    m_cmbClass->addItems({"Все","Эконом","Бизнес"});
    m_cmbClass->setMinimumWidth(110);
    QPushButton *btnSearch=new QPushButton("  🔍  Найти",searchBox);
    btnSearch->setFixedHeight(38);
    QPushButton *btnAll=new QPushButton("Все рейсы",searchBox);
    btnAll->setObjectName("btnGhost");
    btnAll->setFixedHeight(38);
    QLabel *lFrom=new QLabel("Откуда",searchBox);
    lFrom->setStyleSheet("color:#757575;font-size:11px;");
    QLabel *lTo=new QLabel("Куда",searchBox);
    lTo->setStyleSheet("color:#757575;font-size:11px;");
    QLabel *lDate=new QLabel("Дата вылета",searchBox);
    lDate->setStyleSheet("color:#757575;font-size:11px;");
    QLabel *lCls=new QLabel("Класс",searchBox);
    lCls->setStyleSheet("color:#757575;font-size:11px;");
    QVBoxLayout *colFrom=new QVBoxLayout; colFrom->setSpacing(2);
    colFrom->addWidget(lFrom); colFrom->addWidget(m_leFrom);
    QVBoxLayout *colTo=new QVBoxLayout; colTo->setSpacing(2);
    colTo->addWidget(lTo); colTo->addWidget(m_leTo);
    QVBoxLayout *colDate=new QVBoxLayout; colDate->setSpacing(2);
    colDate->addWidget(lDate); colDate->addWidget(m_dateEdit);
    QVBoxLayout *colCls=new QVBoxLayout; colCls->setSpacing(2);
    colCls->addWidget(lCls); colCls->addWidget(m_cmbClass);
    row->addLayout(colFrom,2); row->addLayout(colTo,2);
    row->addLayout(colDate,2); row->addLayout(colCls,1);
    row->addWidget(btnSearch,1,Qt::AlignBottom);
    row->addWidget(btnAll,1,Qt::AlignBottom);
    sv->addLayout(row);
    v->addWidget(searchBox);
    // --- Счётчик результатов ---
    m_lblCount=new QLabel("",tab);
    m_lblCount->setStyleSheet("color:#757575;font-size:12px;padding:0 4px;");
    v->addWidget(m_lblCount);
    // --- Результаты (скроллируемая зона) ---
    QScrollArea *scroll=new QScrollArea(tab);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_resultsWidget=new QWidget;
    m_resultsLayout=new QVBoxLayout(m_resultsWidget);
    m_resultsLayout->setSpacing(10);
    m_resultsLayout->setContentsMargins(0,0,0,0);
    m_resultsLayout->addStretch();
    scroll->setWidget(m_resultsWidget);
    v->addWidget(scroll,1);
    connect(btnSearch,&QPushButton::clicked,this,&ClientWindow::onSearch);
    connect(btnAll,&QPushButton::clicked,this,[this](){
        m_leFrom->clear(); m_leTo->clear();
        m_cmbClass->setCurrentIndex(0);
        loadResults(m_db->getAllTours(true));
    });
    // Загрузить все рейсы сразу
    loadResults(m_db->getAllTours(true));
    return tab;
}
QWidget* ClientWindow::makeBookingsTab(){
    QWidget *tab=new QWidget;
    QVBoxLayout *v=new QVBoxLayout(tab);
    v->setContentsMargins(20,20,20,20); v->setSpacing(12);
    QLabel *h=new QLabel("Мои билеты",tab);
    h->setStyleSheet("font-size:18px;font-weight:bold;color:#0D47A1;");
    v->addWidget(h);
    m_tblBookings=new QTableWidget(0,7,tab);
    m_tblBookings->setHorizontalHeaderLabels(
        {"#","Рейс","Откуда","Куда","Дата вылета","Мест","Сумма ($)","Статус"});
    m_tblBookings->setColumnCount(8);
    m_tblBookings->setHorizontalHeaderLabels(
        {"#","Рейс","Откуда","Куда","Дата вылета","Мест","Сумма ($)","Статус"});
    m_tblBookings->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tblBookings->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tblBookings->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tblBookings->setAlternatingRowColors(true);
    v->addWidget(m_tblBookings,1);
    QHBoxLayout *bh=new QHBoxLayout;
    QPushButton *btnCancel=new QPushButton("Отменить билет",tab);
    btnCancel->setObjectName("btnDanger");
    QPushButton *btnRefresh=new QPushButton("Обновить",tab);
    btnRefresh->setObjectName("btnSecondary");
    bh->addWidget(btnCancel); bh->addStretch(); bh->addWidget(btnRefresh);
    v->addLayout(bh);
    connect(btnCancel, &QPushButton::clicked,this,&ClientWindow::onCancelBooking);
    connect(btnRefresh,&QPushButton::clicked,this,&ClientWindow::onRefreshBookings);
    loadMyBookings();
    return tab;
}
QWidget* ClientWindow::makeProfileTab(){
    QWidget *tab=new QWidget;
    QVBoxLayout *v=new QVBoxLayout(tab);
    v->setAlignment(Qt::AlignCenter);
    QFrame *card=new QFrame(tab);
    card->setObjectName("card");
    card->setFixedWidth(380);
    QVBoxLayout *cv=new QVBoxLayout(card);
    cv->setContentsMargins(30,30,30,30); cv->setSpacing(12);
    QLabel *ico=new QLabel("👤",card);
    ico->setAlignment(Qt::AlignCenter);
    ico->setStyleSheet("font-size:48px;");
    QLabel *name=new QLabel(m_user->getName(),card);
    name->setAlignment(Qt::AlignCenter);
    name->setStyleSheet("font-size:20px;font-weight:bold;color:#0D47A1;");
    QLabel *email=new QLabel(m_user->getEmail(),card);
    email->setAlignment(Qt::AlignCenter);
    email->setStyleSheet("color:#757575;");
    QLabel *role=new QLabel("Роль: Клиент",card);
    role->setAlignment(Qt::AlignCenter);
    role->setObjectName("lblBadge");
    QPushButton *btnOut=new QPushButton("Выйти из системы",card);
    btnOut->setObjectName("btnDanger");
    cv->addWidget(ico); cv->addWidget(name); cv->addWidget(email);
    cv->addWidget(role,0,Qt::AlignCenter); cv->addSpacing(16); cv->addWidget(btnOut);
    v->addWidget(card);
    connect(btnOut,&QPushButton::clicked,this,&ClientWindow::onLogout);
    return tab;
}
void ClientWindow::loadResults(const QList<Tour*>&tours){
    // Удалить все карточки кроме stretch
    QLayoutItem *item;
    while(m_resultsLayout->count()>1){
        item=m_resultsLayout->takeAt(0);
        if(item->widget())delete item->widget();
        delete item;
    }
    if(tours.isEmpty()){
        QLabel *lbl=new QLabel("Рейсы не найдены. Попробуйте изменить параметры поиска.",m_resultsWidget);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet("color:#9E9E9E;font-size:14px;padding:40px;");
        m_resultsLayout->insertWidget(0,lbl);
        m_lblCount->setText("Найдено 0 рейсов");
        return;
    }
    m_lblCount->setText(QString("Найдено %1 рейсов").arg(tours.size()));
    for(Tour *t:tours){
        TourCard *card=new TourCard(t,m_resultsWidget);
        m_resultsLayout->insertWidget(m_resultsLayout->count()-1,card);
        connect(card,&TourCard::buyClicked,this,&ClientWindow::onBuyClicked);
    }
}
void ClientWindow::loadMyBookings(){
    auto list=m_db->getBookingsByUser(m_user->getId());
    m_tblBookings->setRowCount(list.size());
    for(int i=0;i<list.size();++i){
        TourBooking*b=list[i];
        Tour*t=m_db->getTourById(b->getTourId());
        m_tblBookings->setItem(i,0,new QTableWidgetItem(QString::number(b->getId())));
        m_tblBookings->setItem(i,1,new QTableWidgetItem(t?t->getAirline()+" "+t->getFlightNum():"—"));
        m_tblBookings->setItem(i,2,new QTableWidgetItem(t?t->getFromCity():"—"));
        m_tblBookings->setItem(i,3,new QTableWidgetItem(t?t->getToCity():"—"));
        m_tblBookings->setItem(i,4,new QTableWidgetItem(t?t->getDeparture().toString("dd.MM.yyyy hh:mm"):"—"));
        m_tblBookings->setItem(i,5,new QTableWidgetItem(QString::number(b->getSeats())));
        m_tblBookings->setItem(i,6,new QTableWidgetItem(QString::number(b->getTotal(),'f',2)));
        auto *statusItem=new QTableWidgetItem(b->getStatus());
        if(b->getStatus()=="paid")statusItem->setForeground(QColor("#2E7D32"));
        else if(b->getStatus()=="cancelled")statusItem->setForeground(QColor("#C62828"));
        else statusItem->setForeground(QColor("#E65100"));
        m_tblBookings->setItem(i,7,statusItem);
        delete b; delete t;
    }
}
void ClientWindow::onSearch(){
    QString from=m_leFrom->text().trimmed();
    QString to  =m_leTo->text().trimmed();
    QDate   date; // не фильтруем по дате если поле "сегодня" — пустим без даты
    QString cls =m_cmbClass->currentIndex()==0?"":m_cmbClass->currentText();
    auto tours=m_db->searchTours(from,to,date,cls);
    loadResults(tours);
}
void ClientWindow::onBuyClicked(int tourId){
    Tour *t=m_db->getTourById(tourId);
    if(!t||!t->isAvailable()){
        QMessageBox::warning(this,"","На этот рейс мест нет.");
        delete t; return;
    }
    // Диалог покупки
    QDialog dlg(this);
    dlg.setWindowTitle("Оформление билета");
    dlg.setMinimumWidth(400);
    dlg.setStyleSheet(AppStyle::mainStyle());
    QVBoxLayout *dv=new QVBoxLayout(&dlg);
    dv->setContentsMargins(24,24,24,24); dv->setSpacing(12);
    QLabel *title=new QLabel(QString("✈ %1 → %2").arg(t->getFromCity()).arg(t->getToCity()),&dlg);
    title->setStyleSheet("font-size:16px;font-weight:bold;color:#0D47A1;");
    QLabel *info=new QLabel(QString("%1  %2 — %3  (%4)")
        .arg(t->getAirline()+" "+t->getFlightNum())
        .arg(t->getDeparture().toString("dd.MM.yyyy hh:mm"))
        .arg(t->getArrival().toString("hh:mm"))
        .arg(t->flightDurationStr()),&dlg);
    info->setStyleSheet("color:#757575;");
    QFormLayout *form=new QFormLayout;
    QSpinBox *spin=new QSpinBox(&dlg);
    spin->setRange(1,qMin(9,t->getAvailSeats()));
    spin->setValue(1);
    QLabel *lblTotal=new QLabel(QString("$%1").arg(t->getPrice(),0,'f',2),&dlg);
    lblTotal->setStyleSheet("font-size:18px;font-weight:bold;color:#FF6D00;");
    form->addRow("Количество мест:",spin);
    form->addRow("Итого:",lblTotal);
    QHBoxLayout *bh=new QHBoxLayout;
    QPushButton *btnBuy=new QPushButton("Оплатить",&dlg);
    QPushButton *btnCancel=new QPushButton("Отмена",&dlg);
    btnCancel->setObjectName("btnGhost");
    bh->addWidget(btnBuy); bh->addWidget(btnCancel);
    dv->addWidget(title); dv->addWidget(info);
    dv->addLayout(form); dv->addLayout(bh);
    connect(spin,QOverload<int>::of(&QSpinBox::valueChanged),&dlg,[&](int n){
        lblTotal->setText(QString("$%1").arg(t->calcTotal(n),0,'f',2));
    });
    connect(btnBuy,&QPushButton::clicked,&dlg,[&](){
        int n=spin->value();
        double total=t->calcTotal(n);
        TourBooking *b=new TourBooking(m_user->getId(),t->getId(),n,total);
        try{
            m_db->addBooking(b);
            b->pay();
            m_db->updateBookingStatus(b->getId(),"paid");
            QMessageBox::information(&dlg,"Готово!",
                QString("Билет оформлен!\nБронь #%1\n%2 → %3\n%4 мест\nСумма: $%5")
                .arg(b->getId()).arg(t->getFromCity()).arg(t->getToCity())
                .arg(n).arg(total,0,'f',2));
            delete b;
            dlg.accept();
        }catch(const DatabaseException&ex){
            delete b;
            QMessageBox::critical(&dlg,"Ошибка",ex.getMessage());
        }
    });
    connect(btnCancel,&QPushButton::clicked,&dlg,&QDialog::reject);
    if(dlg.exec()==QDialog::Accepted){
        loadResults(m_db->getAllTours(true));
        loadMyBookings();
        m_tabs->setCurrentIndex(1);
    }
    delete t;
}
void ClientWindow::onCancelBooking(){
    int row=m_tblBookings->currentRow();
    if(row<0){QMessageBox::information(this,"","Выберите билет.");return;}
    int id=m_tblBookings->item(row,0)->text().toInt();
    QString st=m_tblBookings->item(row,7)->text();
    if(st=="cancelled"){QMessageBox::warning(this,"","Билет уже отменён.");return;}
    if(QMessageBox::question(this,"Отмена","Отменить билет #"+QString::number(id)+"?",
        QMessageBox::Yes|QMessageBox::No)!=QMessageBox::Yes)return;
    try{
        TourBooking*b=m_db->getBookingById(id);
        if(b){
            m_db->updateBookingStatus(id,"cancelled");
            m_db->updateSeats(b->getTourId(),b->getSeats());
            delete b;
        }
        loadMyBookings();
        loadResults(m_db->getAllTours(true));
        statusBar()->showMessage("Билет отменён.");
    }catch(const DatabaseException&ex){
        QMessageBox::critical(this,"Ошибка",ex.getMessage());
    }
}
void ClientWindow::onRefreshBookings(){loadMyBookings();}
void ClientWindow::onLogout(){
    if(QMessageBox::question(this,"Выход","Выйти?",
        QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes) close();
}
