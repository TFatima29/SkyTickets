#include "booking.h"
BookingException::BookingException(const QString&m):m_msg(m){}
const char* BookingException::what()const noexcept{m_buf=m_msg.toStdString();return m_buf.c_str();}
QString BookingException::getMessage()const{return m_msg;}
DatabaseException::DatabaseException(const QString&m):m_msg(m){}
const char* DatabaseException::what()const noexcept{m_buf=m_msg.toStdString();return m_buf.c_str();}
QString DatabaseException::getMessage()const{return m_msg;}
const QString Booking::S_NEW="new";
const QString Booking::S_PAID="paid";
const QString Booking::S_CANCELLED="cancelled";
Booking::Booking(int u,int t,int s,double tot)
    :m_id(0),m_userId(u),m_tourId(t),m_seats(s),m_total(tot),
     m_date(QDateTime::currentDateTime()),m_status(S_NEW){}
Booking::~Booking(){}
int Booking::getId()const{return m_id;}
int Booking::getUserId()const{return m_userId;}
int Booking::getTourId()const{return m_tourId;}
int Booking::getSeats()const{return m_seats;}
double Booking::getTotal()const{return m_total;}
QDateTime Booking::getDate()const{return m_date;}
QString Booking::getStatus()const{return m_status;}
void Booking::setId(int v){m_id=v;}
void Booking::setStatus(const QString&v){m_status=v;}
void Booking::setDate(const QDateTime&v){m_date=v;}
TourBooking::TourBooking(int u,int t,int s,double tot):Booking(u,t,s,tot){}
TourBooking::~TourBooking(){}
QString TourBooking::getSummary()const{
    return QString("Бронь #%1 | Тур %2 | %3 мест | %4 $ | %5")
        .arg(m_id).arg(m_tourId).arg(m_seats).arg(m_total,0,'f',2).arg(m_status);
}
bool TourBooking::pay(){
    if(m_status==S_NEW){m_status=S_PAID;return true;}
    return false;
}
bool TourBooking::cancel(){
    if(m_status!=S_CANCELLED){m_status=S_CANCELLED;return true;}
    return false;
}
