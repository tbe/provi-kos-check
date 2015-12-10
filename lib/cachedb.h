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

#ifndef PKOS_CACHEDB_H
#define PKOS_CACHEDB_H

#include <QObject>
#include <QtSql/QSqlDatabase>

class QSqlQuery;
namespace pkos {

class CacheDB : public QObject
{
    Q_OBJECT

public:
    enum unitType {
        unknown   = 0,
        pilot     = 1,
        corp      = 2,
        alliance  = 3
    };
    typedef struct {
        qint32      eveID;
        QString     name;
        unitType    type;
        int         kos;
        qint32      parentID;
        QString     parentName;
    } cacheEntry_t;

    CacheDB(QObject* parent = 0);
    virtual ~CacheDB();
    inline bool isOpen() {
        return dbOpen;
    }

    cacheEntry_t* getByID(qint32 id);
    cacheEntry_t* getByName(QString name, unitType type = unknown);

    void insert(const qint32& eveID, const QString& name, const pkos::CacheDB::unitType& type, const int& kos, const qint32& parentID = 0, const QString& parentName = QString());

private:
    bool createDatabase();
    bool prepareQueries();
    void cleanDB();

    cacheEntry_t* checkEntry(QSqlQuery* q);

    QSqlDatabase    db;
    QString         dbfile;

    bool            dbOpen;

    QSqlQuery       *byID_q,*byName_q,*byNameType_q,*insert_q,*clean_q;
};
}

#endif // PKOS_CACHEDB_H
