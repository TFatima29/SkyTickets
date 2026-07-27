#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H
#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QLabel>
#include <QTabWidget>
#include "user.h"
#include "database.h"
class ClientWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit ClientWindow(User *user, QWidget *parent=nullptr);
    ~ClientWindow() override;
private slots:
    void onSearch();
    void onBuyClicked(int tourId);
    void onCancelBooking();
    void onRefreshBookings();
    void onLogout();
private:
    void setupUI();
    QWidget *makeSearchTab();
    QWidget *makeBookingsTab();
    QWidget *makeProfileTab();
    void loadResults(const QList<Tour*>&tours);
    void loadMyBookings();
    User *m_user;
    DatabaseManager *m_db;
    QTabWidget *m_tabs;
    QLineEdit *m_leFrom, *m_leTo;
    QDateEdit *m_dateEdit;
    QComboBox *m_cmbClass;
    QVBoxLayout *m_resultsLayout;
    QWidget    *m_resultsWidget;
    QLabel     *m_lblCount;
    QTableWidget *m_tblBookings;
};
#endif
