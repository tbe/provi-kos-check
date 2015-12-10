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

#ifndef PKOS_HOTDROPPER_H
#define PKOS_HOTDROPPER_H

#include <QtCore/QObject>
#include <QtSql/QSqlDatabase>

#include "pdb/client.h"


class QTimer;
class QNetworkReply;
class QNetworkAccessManager;
class QSqlQuery;

namespace pkos {

class HotDropperDB : public QObject
{
    Q_OBJECT

public:
    HotDropperDB(QObject* parent);
    bool isHotdropper(qint32 id);
    void insert(qint32 id, const QString& name, bool isHotdropper);

private slots:
    void updateDB();
    bool createDatabase();


private:
    bool prepareQueries();


    bool                    ready;
    QTimer                  *updateTimer;
    QNetworkAccessManager   *manager;
    QSqlDatabase            db;
    QString                 dbfile;

    QSqlQuery               *select_q,*insert_q,*update_q,*update_all_q;

    QTcpSocket              socket;
    pdb::Client             *client;
    QDateTime               lastUpdate;
};
}

#endif // PKOS_HOTDROPPER_H
