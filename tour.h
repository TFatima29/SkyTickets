#ifndef TOUR_H
#define TOUR_H
#include <QString>
#include <QDateTime>
// Тур / авиабилет
class Tour {
public:
    Tour();
    ~Tour();
    // Геттеры
    int      getId()          const;
    QString  getFromCity()    const;
    QString  getToCity()      const;
    QString  getAirline()     const;
    QString  getFlightNum()   const;
    QDateTime getDeparture()  const;
    QDateTime getArrival()    const;
    double   getPrice()       const;
    int      getTotalSeats()  const;
    int      getAvailSeats()  const;
    QString  getTourClass()   const;  // Economy / Business
    QString  getDescription() const;
    bool     isActive()       const;
    // Сеттеры
    void setId(int v);
    void setFromCity(const QString&v);
    void setToCity(const QString&v);
    void setAirline(const QString&v);
    void setFlightNum(const QString&v);
    void setDeparture(const QDateTime&v);
    void setArrival(const QDateTime&v);
    void setPrice(double v);
    void setTotalSeats(int v);
    void setAvailSeats(int v);
    void setTourClass(const QString&v);
    void setDescription(const QString&v);
    void setActive(bool v);
    // Бизнес-методы
    bool   isAvailable()           const;
    int    flightDurationMin()     const;
    QString flightDurationStr()    const;
    double calcTotal(int n)        const;
    void   bookSeats(int n);
    void   releaseSeats(int n);
private:
    int      m_id;
    QString  m_fromCity, m_toCity, m_airline, m_flightNum;
    QDateTime m_departure, m_arrival;
    double   m_price;
    int      m_totalSeats, m_availSeats;
    QString  m_class, m_description;
    bool     m_active;
};
#endif
