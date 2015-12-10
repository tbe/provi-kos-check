/*
 * PilotDatabase - Database for Pilot Informations
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

#ifndef PDB_CLIENT_H
#define PDB_CLIENT_H

#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>
#include "PilotDatabaseTypes.h"

namespace pdb {

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QTcpSocket* socket, QObject* parent = 0);
    ~Client();

    bool getVersion(pdb::pdb_version_response& response);
    bool getClientVersion(QString& version);
    bool getPilotInformations(pdb::pdb_pilotdb_response& pilotdb);
    bool getPilotInformationsUpdate(const QDateTime &since, pdb::pdb_pilotdb_response& pilotdb);
    bool updatePilotInformation(const pdb::pdb_pilotinformation& pilot);

private:
    void prepareReq(const pdb_request_type &request);
    bool sendReq();

    bool readData();

    QTcpSocket  *socket;
    QByteArray  *buffer;
    QDataStream *stream;
    quint16     hdr_size;
};
}

#endif // PDB_CLIENT_H
