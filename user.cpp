#include "user.h"
#include <QCryptographicHash>
User::User(int id,const QString&n,const QString&e,const QString&p,const QString&r)
    :Person(id,n,e),m_password(p),m_role(r){}
User::~User(){}
bool User::checkPassword(const QString&input)const{return m_password==hashPassword(input);}
QString User::getPassword()const{return m_password;}
QString User::getRole()const{return m_role;}
QString User::hashPassword(const QString&p){
    return QString(QCryptographicHash::hash(p.toUtf8(),QCryptographicHash::Md5).toHex());
}
Client::Client(int id,const QString&n,const QString&e,const QString&p)
    :User(id,n,e,p,"client"){}
Client::~Client(){}
QString Client::getRole()const{return "client";}
Admin::Admin(int id,const QString&n,const QString&e,const QString&p)
    :User(id,n,e,p,"admin"){}
Admin::~Admin(){}
QString Admin::getRole()const{return "admin";}
