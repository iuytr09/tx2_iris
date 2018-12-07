#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include "dbconnection.h"

bool createConnection(QWidget* parent)
{
    //TO DO
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath("../database/personinfo.db");

    if(!QFile::exists(dbPath))
    {
        db.setDatabaseName(dbPath);
        if(!db.open())
        {
            QMessageBox::critical(parent, QString::fromUtf8("连接数据库"), QString::fromUtf8("数据库打开失败！"), QMessageBox::Ok);
            return false;
        }
        QSqlQuery query;
        query.exec(QString("CREATE TABLE person (uid INTEGER  primary key AUTOINCREMENT not NULL, name varchar, gender varchar,"
                           "idnum varchar, departid int, iris varchar, card varchar, note varchar)"));

        if(!query.isActive())
        {
            QMessageBox::warning(parent, QString::fromUtf8("创建数据库"), query.lastError().text());
            return false;
        }

        query.exec(QString("CREATE TABLE depart (id INTEGER  primary key AUTOINCREMENT not NULL, name varchar)"));
        if(!query.isActive())
        {
            QMessageBox::warning(parent, QString::fromUtf8("创建数据库"), query.lastError().text());
            return false;
        }
        query.exec(QString("CREATE TABLE irisdata (id INTEGER  primary key AUTOINCREMENT not NULL, uid int not NULL,name varchar, depart_name varchar,l_1_tc BLOB,l_1_quality double,l_2_tc BLOB,l_2_quality double,l_3_tc BLOB,l_3_quality double,r_1_tc BLOB,r_1_quality double,r_2_tc BLOB,r_2_quality double,r_3_tc BLOB,r_3_quality double)"));
        if(!query.isActive())
        {
            QMessageBox::warning(parent, QString::fromUtf8("创建数据库"), query.lastError().text());
            return false;
        }

        query.exec(QString("CREATE TABLE facedata (id INTEGER  primary key AUTOINCREMENT not NULL, uid int not NULL,name varchar, depart_name varchar,feat BLOB)"));
        if(!query.isActive())
        {
            QMessageBox::warning(parent, QString::fromUtf8("创建数据库"), query.lastError().text());
            return false;
        }


    }else{
        db.setDatabaseName(dbPath);
        if(!db.open())
        {
            QMessageBox::critical(parent, QString::fromUtf8("连接数据库"), QString::fromUtf8("数据库打开失败！"), QMessageBox::Ok);
            return false;
        }

    }

    return true;
}
