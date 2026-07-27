#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H
#include <QMainWindow>
#include <QTableWidget>
#include <QTabWidget>
#include <QLabel>
#include "user.h"
#include "database.h"
class AdminWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit AdminWindow(User *user, QWidget *parent=nullptr);
    ~AdminWindow() override;
private slots:
    void onAddTour();
    void onEditTour();
    void onDeleteTour();
    void onConfirmBooking();
    void onCancelBooking();
    void onRefreshAll();
    void onLogout();
private:
    void setupUI();
    QWidget *makeToursTab();
    QWidget *makeBookingsTab();
    QWidget *makeStatsTab();
    QWidget *makeProfileTab();
    void loadTours();
    void loadBookings();
    void loadStats();
    Tour* showTourDialog(Tour *existing=nullptr);
    User *m_user;
    DatabaseManager *m_db;
    QTabWidget   *m_tabs;
    QTableWidget *m_tblTours;
    QTableWidget *m_tblBookings;
    QLabel       *m_lblStats;
};
#endif
