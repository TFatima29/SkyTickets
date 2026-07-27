#include "tour.h"
#include <stdexcept>
Tour::Tour():m_id(0),m_price(0),m_totalSeats(0),m_availSeats(0),m_active(true){}
Tour::~Tour(){}
int      Tour::getId()         const{return m_id;}
QString  Tour::getFromCity()   const{return m_fromCity;}
QString  Tour::getToCity()     const{return m_toCity;}
QString  Tour::getAirline()    const{return m_airline;}
QString  Tour::getFlightNum()  const{return m_flightNum;}
QDateTime Tour::getDeparture() const{return m_departure;}
QDateTime Tour::getArrival()   const{return m_arrival;}
double   Tour::getPrice()      const{return m_price;}
int      Tour::getTotalSeats() const{return m_totalSeats;}
int      Tour::getAvailSeats() const{return m_availSeats;}
QString  Tour::getTourClass()  const{return m_class;}
QString  Tour::getDescription()const{return m_description;}
bool     Tour::isActive()      const{return m_active;}
void Tour::setId(int v)               {m_id=v;}
void Tour::setFromCity(const QString&v){m_fromCity=v;}
void Tour::setToCity(const QString&v)  {m_toCity=v;}
void Tour::setAirline(const QString&v) {m_airline=v;}
void Tour::setFlightNum(const QString&v){m_flightNum=v;}
void Tour::setDeparture(const QDateTime&v){m_departure=v;}
void Tour::setArrival(const QDateTime&v)  {m_arrival=v;}
void Tour::setPrice(double v)          {m_price=v;}
void Tour::setTotalSeats(int v)        {m_totalSeats=v;}
void Tour::setAvailSeats(int v)        {m_availSeats=v;}
void Tour::setTourClass(const QString&v){m_class=v;}
void Tour::setDescription(const QString&v){m_description=v;}
void Tour::setActive(bool v)           {m_active=v;}
bool Tour::isAvailable()const{return m_active&&m_availSeats>0;}
int  Tour::flightDurationMin()const{return (int)m_departure.secsTo(m_arrival)/60;}
QString Tour::flightDurationStr()const{
    int mins=flightDurationMin();
    if(mins<=0)return "—";
    return QString("%1ч %2м").arg(mins/60).arg(mins%60);
}
double Tour::calcTotal(int n)const{return m_price*n;}
void Tour::bookSeats(int n){
    if(n>m_availSeats)throw std::runtime_error("Недостаточно мест");
    m_availSeats-=n;
}
void Tour::releaseSeats(int n){
    m_availSeats+=n;
    if(m_availSeats>m_totalSeats)m_availSeats=m_totalSeats;
}
