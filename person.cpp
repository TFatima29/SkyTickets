#include "person.h"
Person::Person(int id,const QString&n,const QString&e):m_id(id),m_name(n),m_email(e){}
Person::~Person(){}
int Person::getId()const{return m_id;}
QString Person::getName()const{return m_name;}
QString Person::getEmail()const{return m_email;}
void Person::setName(const QString&v){m_name=v;}
void Person::setEmail(const QString&v){m_email=v;}
