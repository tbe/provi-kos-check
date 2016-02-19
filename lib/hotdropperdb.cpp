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

#include "hotdropperdb.h"

#define DB_URL "http://koscheck.terra-nanotech.de/kospilotdb.sqlite"
#define APIURL "http://koscheck.terra-nanotech.de/kos-pilot-information.php"

#define PDB_SERVER "terra-nanotech.de"

#ifndef USERAGENT
#define USERAGENT "ProvidenceKOSLookup 2.0"
#endif//USERAGENT

#include <QtCore/QAtomicInt>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtGui/QDesktopServices>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>


using namespace pkos;

QAtomicInt atomicInt;
void spinLock()
{
    while (atomicInt.fetchAndStoreAcquire(1))
        ;
}

void spinUnlock()
{
    atomicInt.fetchAndStoreRelease(0);
}


HotDropperDB::HotDropperDB(QObject* parent)
    :QObject(parent),ready(false)
{
    db = QSqlDatabase::addDatabase("QSQLITE","hotdropper");
    QString dbdir = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
    dbfile = dbdir + QDir::separator() + "kospilotdb.sqlite3";
    if (! QDir().mkpath(dbdir) ) {
        qWarning() << "failed to create database directory";
        return;
    }
    db.setDatabaseName(dbfile);
    ready = false;
    if (db.open()) {
        qDebug() << "  database open";
        QSqlQuery query("SELECT \"name\" FROM sqlite_master WHERE name = \"KOSPilotInformation\"",db);
        if (!query.next()) {
            qDebug() << "  the database is empty, creating schema";
            if ( !createDatabase() ) {
                qWarning() << "failed to create database schema:" << db.lastError().databaseText();
                return;
            }
        }
        ready = prepareQueries();
    }
    if ( ready ) {
        updateTimer = new QTimer(this);
        connect(updateTimer,SIGNAL(timeout()),this,SLOT(updateDB()));
        updateDB();
    }
}

bool HotDropperDB::isHotdropper(qint32 id)
{
    if ( !ready )
        return false;
    bool isDropper = false;
    spinLock();
    select_q->bindValue(":id",id);
    select_q->exec();
    if ( select_q->next() )
        isDropper = select_q->value(2).toBool();
    spinUnlock();
    return isDropper;
}


void HotDropperDB::insert(qint32 id, const QString& name, bool isHotdropper)
{
    pdb::pdb_pilotinformation info;
    info.ID = id;
    info.name = name;
    info.isHotdropper = isHotdropper;
    socket.connectToHost(PDB_SERVER,4711);
    socket.waitForConnected();
    client = new pdb::Client(&socket);
    client->updatePilotInformation(info);
    delete client;
    socket.close();

    if (!ready)
        return;
    spinLock();
    select_q->bindValue(":id",id);
    select_q->exec();
    if ( select_q->next() ) {
        update_q->bindValue(":id",id);
        update_q->bindValue(":isHotdropper",(int)isHotdropper);
        update_q->exec();
    } else {
        insert_q->bindValue(":id",id);
        insert_q->bindValue(":name",name);
        insert_q->bindValue(":isHotdropper",(int)isHotdropper);
        insert_q->exec();
    }
    spinUnlock();
}


void HotDropperDB::updateDB()
{
    socket.connectToHost(PDB_SERVER,4711);
    socket.waitForConnected();
    client = new pdb::Client(&socket);
    spinLock();
    pdb::pdb_pilotdb_response resp;
    pdb::pdb_pilotinformation_table::iterator it;
    qDebug() << "current update date" << lastUpdate;
    if (!lastUpdate.isValid()) {
        QSqlQuery q(db);
        q.exec("DELETE FROM KOSPilotInformation");
        client->getPilotInformations(resp);
        for ( it = resp.table.begin(); it != resp.table.end(); ++it) {
            insert_q->bindValue(":id",it->ID);
            insert_q->bindValue(":name",it->name);
            insert_q->bindValue(":isHotdropper",it->isHotdropper);
            insert_q->exec();
        }
    } else {
        client->getPilotInformationsUpdate(lastUpdate,resp);
        for ( it = resp.table.begin(); it != resp.table.end(); ++it) {
            select_q->bindValue(":id",it->ID);
            select_q->exec();
            if ( select_q->next() ) {
                update_all_q->bindValue(":id",it->ID);
                update_all_q->bindValue(":name",it->name);
                update_all_q->bindValue(":isHotdropper",it->isHotdropper);
                update_all_q->exec();
            } else {
                insert_q->bindValue(":id",it->ID);
                insert_q->bindValue(":name",it->name);
                insert_q->bindValue(":isHotdropper",it->isHotdropper);
                insert_q->exec();
            }
        }
    }
    lastUpdate = resp.change_time;
    qDebug() << "new update date" << lastUpdate;
    spinUnlock();
    delete client;
    socket.close();
    updateTimer->start(900000);
}


#define SAFE_PREPARE(q,sql) q = new QSqlQuery(db); if (!q->prepare(sql) ) { qDebug() << "prepare failed, stmt:" << sql << "error:" << q->lastError().databaseText(); return false; };
bool HotDropperDB::prepareQueries()
{
    SAFE_PREPARE(select_q,"SELECT * FROM KOSPilotInformation WHERE characterID = :id")
    SAFE_PREPARE(insert_q,"INSERT INTO KOSPilotInformation VALUES (:id, :name, :isHotdropper)");
    SAFE_PREPARE(update_q,"UPDATE KOSPilotInformation SET isHotdropper = :isHotdropper WHERE characterID = :id");
    SAFE_PREPARE(update_all_q,"UPDATE KOSPilotInformation SET name = :name, isHotdropper = :isHotdropper WHERE characterID = :id");
    return true;
}
#define SAFE_STATEMENT(sql) if (!q.exec(sql)) return false;
bool HotDropperDB::createDatabase()
{
    QSqlQuery q(db);
    SAFE_STATEMENT("CREATE TABLE KOSPilotInformation( characterID INTEGER PRIMARY KEY NOT NULL, "
                   "name TEXT NOT NULL, isHotdropper INTEGER NOT NULL)");
    SAFE_STATEMENT("CREATE UNIQUE INDEX \"characterID_idx\"  on KOSPilotInformation (unitName characterID)")
    return true;
}
#undef SAFE_STATEMENT

#include "hotdropperdb.moc"
