#ifndef BOOKING_H
#define BOOKING_H
#include <QString>
#include <QDateTime>
#include <stdexcept>
// Исключения
class BookingException : public std::exception {
public:
    explicit BookingException(const QString&msg);
    const char* what()const noexcept override;
    QString getMessage()const;
private:
    QString m_msg;
    mutable std::string m_buf;
};
class DatabaseException : public std::exception {
public:
    explicit DatabaseException(const QString&msg);
    const char* what()const noexcept override;
    QString getMessage()const;
private:
    QString m_msg;
    mutable std::string m_buf;
};
// Абстрактный базовый класс бронирования
class Booking {
public:
    static const QString S_NEW, S_PAID, S_CANCELLED;
    Booking(int userId,int tourId,int seats,double total);
    virtual ~Booking();
    int getId()const;        int getUserId()const;
    int getTourId()const;    int getSeats()const;
    double getTotal()const;  QDateTime getDate()const;
    QString getStatus()const;
    void setId(int v);       void setStatus(const QString&v);
    void setDate(const QDateTime&v);
    virtual QString getSummary()const=0;
protected:
    int m_id,m_userId,m_tourId,m_seats;
    double m_total;
    QDateTime m_date;
    QString m_status;
};
// Конкретное бронирование тура
class TourBooking : public Booking {
public:
    TourBooking(int userId,int tourId,int seats,double total);
    ~TourBooking() override;
    QString getSummary()const override;
    bool pay();
    bool cancel();
};
#endif
