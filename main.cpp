// =============================================================
// main.cpp — точка входа SkyTickets
// =============================================================
#include <QApplication>
#include <QMessageBox>
#include "loginwindow.h"
#include "clientwindow.h"
#include "adminwindow.h"
#include "database.h"
#include "booking.h"

int main(int argc, char *argv[]){
    QApplication app(argc,argv);
    app.setApplicationName("SkyTickets");
    app.setOrganizationName("SkyTickets 2024");
    app.setStyle("Fusion");
    // Открыть БД
    try{
        DatabaseManager::getInstance()->openDatabase("skytickets.db");
    }catch(const DatabaseException &ex){
        QMessageBox::critical(nullptr,"Ошибка БД",
            "Не удалось открыть базу данных:\n"+ex.getMessage());
        return 1;
    }
    // Цикл: показывать LoginWindow, затем нужное главное окно
    while(true){
        LoginWindow login;
        if(login.exec()!=QDialog::Accepted) break;
        User *user=login.getUser();
        if(!user) continue;
        if(user->getRole()=="admin"){
            AdminWindow w(user); w.show(); app.exec();
        } else {
            ClientWindow w(user); w.show(); app.exec();
        }
    }
    DatabaseManager::getInstance()->closeDatabase();
    return 0;
}
