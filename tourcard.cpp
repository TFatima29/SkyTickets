#include "tourcard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
TourCard::TourCard(Tour *tour, QWidget *parent)
    : QFrame(parent), m_tour(tour) {
    setObjectName("card");
    setupUI();
}
int TourCard::getTourId() const { return m_tour ? m_tour->getId() : -1; }
void TourCard::setupUI() {
    setFixedHeight(110);
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(12); shadow->setOffset(0, 2);
    shadow->setColor(QColor(0,0,0,30));
    setGraphicsEffect(shadow);
    QHBoxLayout *main = new QHBoxLayout(this);
    main->setContentsMargins(16,12,16,12);
    main->setSpacing(0);
    // --- Авиакомпания ---
    QVBoxLayout *airCol = new QVBoxLayout;
    airCol->setSpacing(2);
    m_lblAirline = new QLabel(m_tour->getAirline(), this);
    m_lblAirline->setStyleSheet("color:#1565C0;font-weight:bold;font-size:12px;");
    QLabel *fn = new QLabel(m_tour->getFlightNum(), this);
    fn->setStyleSheet("color:#9E9E9E;font-size:11px;");
    m_lblClass = new QLabel(m_tour->getTourClass(), this);
    m_lblClass->setObjectName(m_tour->getTourClass()=="Бизнес"?"lblBadgeOrange":"lblBadge");
    m_lblClass->setAlignment(Qt::AlignCenter);
    airCol->addWidget(m_lblAirline);
    airCol->addWidget(fn);
    airCol->addWidget(m_lblClass);
    airCol->addStretch();
    // --- Время вылета ---
    QVBoxLayout *depCol = new QVBoxLayout;
    depCol->setSpacing(2);
    QLabel *depTime = new QLabel(m_tour->getDeparture().toString("hh:mm"), this);
    depTime->setStyleSheet("font-size:22px;font-weight:bold;color:#212121;");
    QLabel *depDate = new QLabel(m_tour->getDeparture().toString("dd.MM.yyyy"), this);
    depDate->setStyleSheet("color:#757575;font-size:11px;");
    QLabel *depCity = new QLabel(m_tour->getFromCity(), this);
    depCity->setStyleSheet("color:#1565C0;font-size:12px;font-weight:bold;");
    depCol->addWidget(depTime); depCol->addWidget(depDate); depCol->addWidget(depCity);
    depCol->addStretch();
    // --- Длительность ---
    QVBoxLayout *durCol = new QVBoxLayout;
    durCol->setAlignment(Qt::AlignCenter);
    QLabel *line = new QLabel("────✈────", this);
    line->setAlignment(Qt::AlignCenter);
    line->setStyleSheet("color:#BDBDBD;font-size:14px;");
    QLabel *dur = new QLabel(m_tour->flightDurationStr(), this);
    dur->setAlignment(Qt::AlignCenter);
    dur->setStyleSheet("color:#9E9E9E;font-size:11px;");
    QLabel *direct = new QLabel("прямой", this);
    direct->setAlignment(Qt::AlignCenter);
    direct->setStyleSheet("color:#2E7D32;font-size:11px;");
    durCol->addStretch(); durCol->addWidget(line);
    durCol->addWidget(dur); durCol->addWidget(direct); durCol->addStretch();
    // --- Время прилёта ---
    QVBoxLayout *arrCol = new QVBoxLayout;
    arrCol->setSpacing(2);
    QLabel *arrTime = new QLabel(m_tour->getArrival().toString("hh:mm"), this);
    arrTime->setStyleSheet("font-size:22px;font-weight:bold;color:#212121;");
    QLabel *arrDate = new QLabel(m_tour->getArrival().toString("dd.MM.yyyy"), this);
    arrDate->setStyleSheet("color:#757575;font-size:11px;");
    QLabel *arrCity = new QLabel(m_tour->getToCity(), this);
    arrCity->setStyleSheet("color:#1565C0;font-size:12px;font-weight:bold;");
    arrCol->addWidget(arrTime); arrCol->addWidget(arrDate); arrCol->addWidget(arrCity);
    arrCol->addStretch();
    // --- Места ---
    QVBoxLayout *seatsCol = new QVBoxLayout;
    seatsCol->setAlignment(Qt::AlignCenter);
    m_lblSeats = new QLabel(QString("Мест: %1").arg(m_tour->getAvailSeats()), this);
    m_lblSeats->setAlignment(Qt::AlignCenter);
    m_lblSeats->setStyleSheet("color:#757575;font-size:11px;");
    seatsCol->addStretch(); seatsCol->addWidget(m_lblSeats); seatsCol->addStretch();
    // --- Цена + кнопка ---
    QVBoxLayout *priceCol = new QVBoxLayout;
    priceCol->setAlignment(Qt::AlignCenter);
    m_lblPrice = new QLabel(QString("$%1").arg(m_tour->getPrice(),0,'f',0), this);
    m_lblPrice->setObjectName("lblPrice");
    m_lblPrice->setAlignment(Qt::AlignCenter);
    m_btnBuy = new QPushButton("Купить", this);
    m_btnBuy->setFixedWidth(90);
    priceCol->addStretch();
    priceCol->addWidget(m_lblPrice);
    priceCol->addWidget(m_btnBuy);
    priceCol->addStretch();
    main->addLayout(airCol, 1);
    main->addSpacing(20);
    main->addLayout(depCol, 1);
    main->addSpacing(10);
    main->addLayout(durCol, 2);
    main->addSpacing(10);
    main->addLayout(arrCol, 1);
    main->addSpacing(20);
    main->addLayout(seatsCol, 1);
    main->addSpacing(20);
    main->addLayout(priceCol, 1);
    connect(m_btnBuy, &QPushButton::clicked, this, [this](){
        emit buyClicked(m_tour->getId());
    });
}
