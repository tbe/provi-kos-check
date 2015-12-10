#ifndef PILOTDATABASETYPES_H
#define PILOTDATABASETYPES_H

#include <QtCore/QtGlobal>
#include <QtCore/QString>
#include <QtCore/QDataStream>
#include <QtCore/QDebug>
#include <QtCore/QDateTime>

#define API_VERSION 1

namespace pdb
{


typedef quint8 pdb_version;
typedef quint16 pdb_packetsize;
typedef enum {
    GET_API_VERSION       = 1,
    GET_CLIENT_VERSION    = 2,
    GET_PILOTINFORMATIONS = 3,
    GET_UPDATES           = 4,
    SEND_UPDATE           = 10,
    STATUS_OK             = 50,
    STATUS_ERROR          = 51,
    BYE                   = 99
} pdb_request_type;


typedef struct {
    pdb_packetsize packetsize;
} pdb_response_hdr;

typedef struct {
    pdb_packetsize      packetsize;
    pdb_version         version;
    pdb_request_type    request;
} pdb_request_hdr;

typedef struct {
    QDateTime     since;
} pdb_update_request;

typedef struct {
    pdb_version version;
    QString     name;
} pdb_version_response;

typedef QString pdb_client_version;

typedef struct {
    quint32 ID;
    QString name;
    bool    isHotdropper;
} pdb_pilotinformation;

typedef QList<pdb_pilotinformation> pdb_pilotinformation_table;

typedef struct {
    pdb_pilotinformation_table table;
    quint32   change_id;
    QDateTime change_time;
} pdb_pilotdb_response;


}

#endif//PILOTDATABASETYPES_H

