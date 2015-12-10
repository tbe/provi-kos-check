/*
 * ProvidenceKOSLookup - a KOS Checker
 * Copyright (C) 2014 Terra Nanotech <info@terra-nanotech.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cachedb.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtGui/QDesktopServices>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

#define SCHEMA_VERSION 1


using namespace pkos;

CacheDB::CacheDB(QObject* parent)
    :QObject(parent),dbOpen(false),byID_q(0),byName_q(0),byNameType_q(0),insert_q(0),clean_q(0)
{
    db = QSqlDatabase::addDatabase("QSQLITE","cachedb");
    QString dbdir = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
    dbfile = dbdir + QDir::separator() + "pkoschace.sqlite3";
    if (! QDir().mkpath(dbdir) ) {
        qWarning() << "failed to create database directory";
        return;
    }
    qDebug() << "using cache database" << dbfile;
    db.setDatabaseName(dbfile);
    if (db.open()) {
        qDebug() << "  database open";
        // check if the database does contain our schema already
        QSqlQuery query("SELECT \"name\" FROM sqlite_master WHERE name = \"schema_version\"",db);
        if (!query.next()) {
            qDebug() << "  the database is empty, creating schema";
            if ( !createDatabase() ) {
                qWarning() << "failed to create database schema:" << db.lastError().databaseText();
                return;
            }
        }
        dbOpen = prepareQueries();
        if (dbOpen)
            cleanDB();
    } else
        qWarning() << "failed to open database:" << db.lastError().driverText();
}

CacheDB::~CacheDB()
{
    delete clean_q;
    delete insert_q;
    delete byNameType_q;
    delete byName_q;
    delete byID_q;
}


CacheDB::cacheEntry_t* CacheDB::getByID(qint32 id)
{
    byID_q->bindValue(":eveID",id);
    return checkEntry(byID_q);
}

CacheDB::cacheEntry_t* CacheDB::getByName(QString name, CacheDB::unitType type)
{
    QSqlQuery *q;
    if ( type == unknown ) {
        q = byName_q;
        q->bindValue(":unitName",name);
    } else {
        q = byNameType_q;
        q->bindValue(":unitName",name);
        q->bindValue(":unitType",type);
    }
    return checkEntry(q);
}


void CacheDB::insert(const qint32& eveID, const QString& name, const CacheDB::unitType& type, const int& kos, const qint32& parentID, const QString& parentName)
{
    clean_q->exec();
    insert_q->bindValue(":eveID",eveID);
    insert_q->bindValue(":unitName",name);
    insert_q->bindValue(":unitType",type);
    insert_q->bindValue(":kosState",kos);
    if ( parentID > 0 ) {
        insert_q->bindValue(":parentID",parentID);
        insert_q->bindValue(":parentName",parentName);
    } else {
        insert_q->bindValue(":parentID",QVariant(QVariant::Int));
        insert_q->bindValue(":parentName",QVariant(QVariant::String));
    }
    insert_q->exec();
}


#define SAFE_STATEMENT(sql) if (!q.exec(sql)) return false;
bool CacheDB::createDatabase()
{
    QSqlQuery q(db);
    SAFE_STATEMENT("CREATE TABLE schema_version ( version INTEGER )")
    SAFE_STATEMENT(QString("INSERT INTO schema_version VALUES ( %1 )").arg(SCHEMA_VERSION))
    SAFE_STATEMENT("CREATE TABLE koscache ( eveID INTEGER PRIMARY KEY NOT NULL, unitName TEXT NOT NULL, "
                   "unitType INTEGER NOT NULL, kosState INTEGER NOT NULL, parentID INTEGER, parentName TEXT, expireDate INTEGER )")
    SAFE_STATEMENT("CREATE INDEX \"expireDate_idx\" on koscache (expireDate DESC)")
    SAFE_STATEMENT("CREATE UNIQUE INDEX \"eveID_idx\" on koscache (eveID ASC)")
    SAFE_STATEMENT("CREATE UNIQUE INDEX \"unitName_idx\"  on koscache (unitName ASC)")
    return true;
}
#undef SAFE_STATEMENT

#define SAFE_PREPARE(q,sql) q = new QSqlQuery(db); if (!q->prepare(sql) ) { qDebug() << "prepare failed, stmt:" << sql << "error:" << q->lastError().databaseText(); return false; };
bool CacheDB::prepareQueries()
{
    SAFE_PREPARE(byID_q,        "SELECT * FROM koscache WHERE eveID = :eveID AND expireDate > strftime('%s','now')")
    SAFE_PREPARE(byName_q,      "SELECT * FROM koscache WHERE unitName = :unitName AND expireDate > strftime('%s','now')")
    SAFE_PREPARE(byNameType_q,  "SELECT * FROM koscache WHERE unitName = :unitName AND unitType = :unitType AND expireDate > strftime('%s','now')")
    SAFE_PREPARE(insert_q,      "INSERT INTO koscache VALUES ( :eveID, :unitName, :unitType, :kosState, :parentID, :parentName, strftime('%s','now') + 3600 )")
    SAFE_PREPARE(clean_q,       "DELETE FROM koscache WHERE expireDate <= strftime('%s','now')")
    return true;
}


void CacheDB::cleanDB()
{
    qDebug() << "purging expired cache entries";
    clean_q->exec();
}

QString getLastExecutedQuery(const QSqlQuery& query)
{
  QString str = query.lastQuery();
  QMapIterator<QString, QVariant> it(query.boundValues());
  while (it.hasNext())
  {
    it.next();
    if (it.value().type() == QVariant::String)
      str.replace(it.key(),"\""+it.value().toString()+"\"");
    else
      str.replace(it.key(),it.value().toString());
  }
  return str;
}


CacheDB::cacheEntry_t* CacheDB::checkEntry(QSqlQuery* q)
{
    cacheEntry_t* entry = 0;
    bool ok = q->exec();
    if (!ok)
      qWarning() << "SQL ERROR: " << q->lastError().databaseText();
    if ( q->next() ) {
        entry = new cacheEntry_t;
        entry->eveID = q->value(0).toInt();
        entry->name  = q->value(1).toString();
        entry->type  = (unitType)q->value(2).toInt();
        entry->kos   = q->value(3).toInt();
        if ( q->value(4).isNull() || q->value(4).toInt() == 0 )
            entry->parentID = 0;
        else {
            entry->parentID   = q->value(4).toInt();
            entry->parentName = q->value(5).toString();
        }
    }
    return entry;
}


#include "cachedb.moc"
