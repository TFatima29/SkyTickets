#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H
#include <QDialog>
#include <QLineEdit>
#include <QStackedWidget>
#include "user.h"
#include "database.h"
class LoginWindow : public QDialog {
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent=nullptr);
    ~LoginWindow() override;
    User* getUser() const;
private slots:
    void onLogin();
    void onRegister();
    void showLogin();
    void showRegister();
private:
    void setupUI();
    QWidget *makePgLogin();
    QWidget *makePgRegister();
    DatabaseManager *m_db;
    User *m_user;
    QStackedWidget *m_stack;
    QLineEdit *m_leEmail, *m_lePass;
    QLineEdit *m_reName, *m_reEmail, *m_rePass, *m_reConf;
};
#endif
