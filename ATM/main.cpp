#include "mainwindow.h"
#include <QApplication>
#include <QtSql>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("blabladb");

    MainWindow w;
    w.connectionSuccessful = db.open();
    w.show();

    return a.exec();
}
