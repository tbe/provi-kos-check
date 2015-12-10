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

#ifndef PKOS_EVEAPI_H
#define PKOS_EVEAPI_H

#include <QtCore/QObject>
#include "network.h"

namespace pkos {
namespace eveapi {

typedef struct {
    qint32 id;
    qint32 corporationID;
    QString startDate;
} employmentHistory_t;

typedef struct {
    qint32 id;
    QString name;
    QString race;
    QString bloodline;
    qint32  corporationID;
    QString corporation;
    QString corporationDate;
    float   securityStatus;
    QList<employmentHistory_t> employmentHistory;
} characterInfo_t;

typedef struct {
    qint32 id;
    QString name;
    qint32  allianceID;
    QString allianceName;
} corpSheet_t;

}
/** @class EveAPI
 *  @brief API Wrapper for the official EVE API
 */
class EveAPI : public QObject
{
    Q_OBJECT

public:
    EveAPI(QObject* parent, Network* net = 0);
    ///@brief get the character ID for the given Name
    qint32                   getCharacterID(const QString& name);
    ///@brief get the character Info for the given ID
    eveapi::characterInfo_t  getCharacterInfo(qint32 &charid);
    ///@brief get the coro Info for the given ID
    eveapi::corpSheet_t      getCorpSheet(qint32 &corpid);

private:
    QNetworkReply* apiCall(const QString& endpoint, const QMap< QString, QString >& params);

    Network *net;

};


}

#endif // PKOS_EVEAPI_H
