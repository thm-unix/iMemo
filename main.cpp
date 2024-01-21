#include "mainmenu.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    MainMenu* mainMenu = new MainMenu(nullptr, &a);
    mainMenu->show();
    return a.exec();
}
