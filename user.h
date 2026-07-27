#ifndef USER_H
#define USER_H
#include "person.h"
class User : public Person {
public:
    User(int id,const QString&name,const QString&email,
         const QString&password,const QString&role);
    virtual ~User();
    bool checkPassword(const QString&input) const;
    QString getPassword() const;
    QString getRole() const override;
    static QString hashPassword(const QString&p);
protected:
    QString m_password, m_role;
};
class Client : public User {
public:
    Client(int id,const QString&name,const QString&email,const QString&password);
    ~Client() override;
    QString getRole() const override;
};
class Admin : public User {
public:
    Admin(int id,const QString&name,const QString&email,const QString&password);
    ~Admin() override;
    QString getRole() const override;
};
#endif
