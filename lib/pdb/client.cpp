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

#include "client.h"

using namespace pdb;

QDataStream &operator>> ( QDataStream& in, pdb_response_hdr& hdr )
{
    in >> hdr.packetsize;
    return in;
}


QDataStream &operator<< ( QDataStream& out, const pdb_request_hdr& hdr )
{
    out <<  hdr.packetsize << hdr.version << ( quint32 ) hdr.request;
    return out;
}

QDataStream &operator>> ( QDataStream& in, pdb_version_response& resp )
{
    in >> resp.version >> resp.name;
    return in;
}

QDataStream &operator>> ( QDataStream& in, pdb_pilotinformation& info )
{
    in >> info.ID >> info.name >> info.isHotdropper;
    return in;
}

QDataStream &operator>> ( QDataStream& in, pdb_pilotdb_response& response )
{
    in >> response.table >> response.change_id >> response.change_time;
    return in;
}

QDataStream &operator<< ( QDataStream& out, const pdb_pilotinformation& info )
{
    out << info.ID << info.name << info.isHotdropper;
    return out;
}

QDataStream &operator<< ( QDataStream&out, const pdb_update_request& req)
{
    out << req.since;
    return out;
}

Client::Client ( QTcpSocket* socket, QObject* parent )
    :QObject ( parent ),socket ( socket )
{
}

Client::~Client()
{
    prepareReq ( BYE );
    sendReq();
}

bool Client::getVersion ( pdb_version_response& response )
{
    prepareReq ( GET_API_VERSION );
    if ( !sendReq() ) return false;
    if ( !readData() ) return false;
    QDataStream in ( *buffer );
    in >> response;
    return true;
}

bool Client::getClientVersion ( QString& version )
{
    prepareReq ( GET_CLIENT_VERSION );
    if ( !sendReq() ) return false;
    if ( !readData() ) return false;
    QDataStream in ( *buffer );
    in >> version;
    return true;
}

bool Client::getPilotInformations ( pdb_pilotdb_response& pilotdb )
{
    prepareReq ( GET_PILOTINFORMATIONS );
    if ( !sendReq() ) return false;
    if ( !readData() ) return false;
    QDataStream in ( *buffer );
    in >> pilotdb;
    return true;
}


bool Client::updatePilotInformation ( const pdb_pilotinformation  &pilot )
{
    prepareReq ( SEND_UPDATE );
    qDebug() << pilot.ID << pilot.name << pilot.isHotdropper;
    *stream << pilot;
    if ( !sendReq() ) return false;
    if ( !readData() ) return false;
    QDataStream in ( *buffer );
    quint32 resp;
    in >> resp;
    if ( ( pdb_request_type ) resp != STATUS_OK ) return false;
    return true;
}

bool Client::getPilotInformationsUpdate(const QDateTime &since, pdb_pilotdb_response& pilotdb)
{
    prepareReq ( GET_UPDATES );
    pdb_update_request req;
    req.since = since;
    *stream << req;
    if ( !sendReq() ) return false;
    if ( !readData() ) return false;
    QDataStream in ( *buffer );
    in >> pilotdb;
    return true;
}

void Client::prepareReq ( const pdb_request_type& request )
{
    buffer = new QByteArray;
    stream = new QDataStream ( buffer,QIODevice::WriteOnly );
    pdb_request_hdr hdr;
    hdr.version = API_VERSION;
    hdr.request = request;
    hdr.packetsize = 0; // we fill this later
    *stream << hdr;
    hdr_size = buffer->size();
}

bool Client::sendReq()
{
    quint16 datasize = buffer->size();
    stream->device()->seek ( 0 );
    *stream << datasize;
    qDebug() << buffer->toHex();
    bool writeOk = socket->write ( *buffer ) >= buffer->size();
    socket->flush();
    delete stream;
    delete buffer;
    return writeOk;
}

bool Client::readData()
{
    pdb_response_hdr hdr;
    while ( socket->bytesAvailable() < sizeof ( hdr ) )
        if ( !socket->waitForReadyRead() )
            return false;
    QDataStream in ( socket );
    in >> hdr;
    buffer = new QByteArray;
    qDebug() << "reading respons of" << hdr.packetsize << "bytes";
    while ( hdr.packetsize > 0 ) {
        while ( socket->bytesAvailable() < ( ( hdr.packetsize < 64 ) ? hdr.packetsize : 64 ) )
            if ( !socket->waitForReadyRead() )
                return false;
        QByteArray data = socket->readAll();
        buffer->append ( data );
        hdr.packetsize -= data.size();
    }
    return true;
}


#include "client.moc"
