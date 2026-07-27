#ifndef PERSON_H
#define PERSON_H
#include <QString>
class Person {
public:
    Person(int id, const QString &name, const QString &email);
    virtual ~Person();
    int getId() const;
    QString getName() const;
    QString getEmail() const;
    void setName(const QString &v);
    void setEmail(const QString &v);
    virtual QString getRole() const = 0;
protected:
    int m_id;
    QString m_name, m_email;
};
#endif
