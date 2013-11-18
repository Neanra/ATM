#include "mainwindow.h"
#include <QApplication>
#include <QtSql>
#include "ATM.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    ATM atm(&w, &w);

    return a.exec();
}
