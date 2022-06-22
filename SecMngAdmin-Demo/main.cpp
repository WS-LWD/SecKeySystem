#include<QDebug>
#include "mainwindow.h"
#include <QApplication>
#include <QSqlDatabase>
#include "jsonparse.h"
#include "databaseop.h"
#include "dbinfoset.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    qDebug() << QSqlDatabase::drivers();

    JsonParse json;
    DatabaseOP db;
    bool bl = db.openDatabase(json.getHostName(), json.getDBName(),
                              json.getUserName(), json.getPassWd(), json.getDBPort());
    if(bl == false)
    {
        DBInfoSet infowg;
        int ret = infowg.exec();
        if(ret == QDialog::Rejected)
        {
            return 0;
        }
    }

    w.show();
    return a.exec();
}
