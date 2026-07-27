#include "loginwindow.h"
#include "appstyle.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
LoginWindow::LoginWindow(QWidget *parent)
    : QDialog(parent), m_db(DatabaseManager::getInstance()), m_user(nullptr) {
    setWindowFlags(Qt::Window|Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(900,560);
    setupUI();
}
LoginWindow::~LoginWindow(){}
User* LoginWindow::getUser()const{return m_user;}
void LoginWindow::setupUI(){
    // Фоновый виджет
    QWidget *bg=new QWidget(this);
    bg->setObjectName("loginBg");
    bg->setStyleSheet(AppStyle::loginStyle());
    bg->setFixedSize(900,560);
    QHBoxLayout *root=new QHBoxLayout(bg);
    root->setContentsMargins(0,0,0,0);
    // Левая панель — логотип
    QWidget *left=new QWidget(bg);
    left->setFixedWidth(420);
    left->setStyleSheet("background:transparent;");
    QVBoxLayout *lv=new QVBoxLayout(left);
    lv->setAlignment(Qt::AlignCenter);
    QLabel *logo=new QLabel("✈", left);
    logo->setAlignment(Qt::AlignCenter);
    logo->setStyleSheet("font-size:72px;color:white;");
    QLabel *lname=new QLabel("SkyTickets", left);
    lname->setAlignment(Qt::AlignCenter);
    lname->setStyleSheet("color:white;font-size:32px;font-weight:bold;");
    QLabel *lsub=new QLabel("Бронирование авиабилетов\nбыстро и удобно", left);
    lsub->setAlignment(Qt::AlignCenter);
    lsub->setStyleSheet("color:rgba(255,255,255,0.75);font-size:14px;");
    lv->addWidget(logo); lv->addWidget(lname); lv->addWidget(lsub);
    // Правая панель — форма
    QFrame *card=new QFrame(bg);
    card->setObjectName("loginCard");
    card->setFixedSize(440,480);
    card->setStyleSheet(AppStyle::loginStyle());
    auto *shadow=new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(30); shadow->setOffset(0,8);
    shadow->setColor(QColor(0,0,0,60));
    card->setGraphicsEffect(shadow);
    QVBoxLayout *cv=new QVBoxLayout(card);
    cv->setContentsMargins(40,36,40,36);
    m_stack=new QStackedWidget(card);
    m_stack->addWidget(makePgLogin());
    m_stack->addWidget(makePgRegister());
    cv->addWidget(m_stack);
    QWidget *rightWrap=new QWidget(bg);
    rightWrap->setStyleSheet("background:transparent;");
    QHBoxLayout *rh=new QHBoxLayout(rightWrap);
    rh->setAlignment(Qt::AlignCenter);
    rh->addWidget(card);
    root->addWidget(left);
    root->addWidget(rightWrap,1);
}
QWidget* LoginWindow::makePgLogin(){
    QWidget *pg=new QWidget;
    QVBoxLayout *v=new QVBoxLayout(pg);
    v->setSpacing(14);
    QLabel *title=new QLabel("Войти в аккаунт",pg);
    title->setObjectName("loginTitle");
    QLabel *sub=new QLabel("Введите e-mail и пароль",pg);
    sub->setObjectName("loginSub");
    m_leEmail=new QLineEdit(pg); m_leEmail->setPlaceholderText("E-mail");
    m_lePass =new QLineEdit(pg); m_lePass->setPlaceholderText("Пароль");
    m_lePass->setEchoMode(QLineEdit::Password);
    QPushButton *btn=new QPushButton("Войти",pg);
    btn->setObjectName("btnLogin");
    QPushButton *reg=new QPushButton("Нет аккаунта? Зарегистрироваться",pg);
    reg->setObjectName("btnRegLink");
    v->addWidget(title); v->addWidget(sub); v->addSpacing(10);
    v->addWidget(new QLabel("E-mail",pg));
    v->addWidget(m_leEmail);
    v->addWidget(new QLabel("Пароль",pg));
    v->addWidget(m_lePass);
    v->addSpacing(6); v->addWidget(btn); v->addWidget(reg,0,Qt::AlignCenter);
    connect(btn,&QPushButton::clicked,this,&LoginWindow::onLogin);
    connect(reg,&QPushButton::clicked,this,&LoginWindow::showRegister);
    return pg;
}
QWidget* LoginWindow::makePgRegister(){
    QWidget *pg=new QWidget;
    QVBoxLayout *v=new QVBoxLayout(pg);
    v->setSpacing(10);
    QLabel *title=new QLabel("Создать аккаунт",pg);
    title->setObjectName("loginTitle");
    m_reName=new QLineEdit(pg); m_reName->setPlaceholderText("Ваше имя");
    m_reEmail=new QLineEdit(pg); m_reEmail->setPlaceholderText("E-mail");
    m_rePass=new QLineEdit(pg); m_rePass->setPlaceholderText("Пароль (мин. 6)");
    m_rePass->setEchoMode(QLineEdit::Password);
    m_reConf=new QLineEdit(pg); m_reConf->setPlaceholderText("Повторите пароль");
    m_reConf->setEchoMode(QLineEdit::Password);
    QPushButton *btn=new QPushButton("Зарегистрироваться",pg);
    btn->setObjectName("btnLogin");
    QPushButton *back=new QPushButton("Уже есть аккаунт? Войти",pg);
    back->setObjectName("btnRegLink");
    v->addWidget(title); v->addSpacing(6);
    v->addWidget(new QLabel("Имя",pg)); v->addWidget(m_reName);
    v->addWidget(new QLabel("E-mail",pg)); v->addWidget(m_reEmail);
    v->addWidget(new QLabel("Пароль",pg)); v->addWidget(m_rePass);
    v->addWidget(new QLabel("Подтверждение",pg)); v->addWidget(m_reConf);
    v->addSpacing(4); v->addWidget(btn); v->addWidget(back,0,Qt::AlignCenter);
    connect(btn,&QPushButton::clicked,this,&LoginWindow::onRegister);
    connect(back,&QPushButton::clicked,this,&LoginWindow::showLogin);
    return pg;
}
void LoginWindow::showLogin()   {m_stack->setCurrentIndex(0);}
void LoginWindow::showRegister(){m_stack->setCurrentIndex(1);}
void LoginWindow::onLogin(){
    if(m_leEmail->text().trimmed().isEmpty()||m_lePass->text().isEmpty()){
        QMessageBox::warning(this,"","Заполните все поля."); return;
    }
    try{
        User*u=m_db->getUserByEmail(m_leEmail->text().trimmed());
        if(!u){QMessageBox::warning(this,"","Пользователь не найден.");return;}
        if(!u->checkPassword(m_lePass->text())){
            delete u; QMessageBox::warning(this,"","Неверный пароль."); return;
        }
        m_user=u; accept();
    }catch(const DatabaseException&ex){
        QMessageBox::critical(this,"Ошибка БД",ex.getMessage());
    }
}
void LoginWindow::onRegister(){
    if(m_reName->text().trimmed().isEmpty()||
       m_reEmail->text().trimmed().isEmpty()||
       m_rePass->text().isEmpty()){
        QMessageBox::warning(this,"","Заполните все поля."); return;
    }
    if(m_rePass->text()!=m_reConf->text()){
        QMessageBox::warning(this,"","Пароли не совпадают."); return;
    }
    if(m_rePass->text().length()<6){
        QMessageBox::warning(this,"","Пароль не менее 6 символов."); return;
    }
    try{
        User*ex=m_db->getUserByEmail(m_reEmail->text().trimmed());
        if(ex){delete ex; QMessageBox::warning(this,"","E-mail уже используется."); return;}
        QString h=User::hashPassword(m_rePass->text());
        Client *c=new Client(0,m_reName->text().trimmed(),
                              m_reEmail->text().trimmed(),h);
        m_db->addUser(c); delete c;
        m_user=m_db->getUserByEmail(m_reEmail->text().trimmed());
        QMessageBox::information(this,"","Регистрация успешна!");
        accept();
    }catch(const DatabaseException&ex){
        QMessageBox::critical(this,"Ошибка БД",ex.getMessage());
    }
}
