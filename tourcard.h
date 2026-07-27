#ifndef TOURCARD_H
#define TOURCARD_H
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include "tour.h"
// Виджет-карточка рейса (как на Aviasales)
class TourCard : public QFrame {
    Q_OBJECT
public:
    explicit TourCard(Tour *tour, QWidget *parent = nullptr);
    int getTourId() const;
signals:
    void buyClicked(int tourId);
private:
    void setupUI();
    Tour *m_tour;
    QLabel *m_lblRoute, *m_lblTimes, *m_lblDuration,
           *m_lblAirline, *m_lblSeats, *m_lblPrice, *m_lblClass;
    QPushButton *m_btnBuy;
};
#endif
