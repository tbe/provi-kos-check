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

#include "eveapi.h"
#include "eveapixmlhandler.h"
#include <QtNetwork/QNetworkReply>
#include <QtXml/QXmlSimpleReader>

#define EVEAPI "http://eve-api.terra-nanotech.de:8080/%1.xml.aspx"
/* #define EVEAPI "https://api.eveonline.com/%1.xml.aspx" */
const QString urlptrn(EVEAPI);


using namespace pkos;

EveAPI::EveAPI(QObject* parent, Network* net)
    :QObject(parent),net(net)
{
    if ( this->net == 0 )
        net = new Network(this);

}

QNetworkReply* EveAPI::apiCall(const QString& endpoint, const QMap< QString, QString >& params)
{
    return this->net->httpSyncRequest(urlptrn.arg(endpoint),params);
}

qint32 EveAPI::getCharacterID(const QString& name)
{
    QMap<QString,QString> params;
    qint32 characterID = 0;

    params["names"] = name;
    QNetworkReply* reply = this->apiCall("eve/CharacterID",params);
    if ( reply == 0 ) {
        characterID = -1;
        return characterID;
    }

    QXmlSimpleReader rdr;
    eveapi::EveAPIPlayerIDHandler handler(name,characterID);
    rdr.setContentHandler(&handler);
    rdr.parse(QXmlInputSource(reply));
    delete reply;
    return characterID;
}


eveapi::characterInfo_t EveAPI::getCharacterInfo(qint32& charid)
{
    QMap<QString,QString> params;
    eveapi::characterInfo_t charinfo;

    params["characterID"] = QString::number(charid);
    QNetworkReply* reply = this->apiCall("eve/CharacterInfo",params);
    if ( reply == 0 ) {
        charinfo.id = -1;
        return charinfo;
    }

    QXmlSimpleReader rdr;
    eveapi::EveAPICharacterInfoHandler handler(&charinfo);
    rdr.setContentHandler(&handler);
    rdr.parse(QXmlInputSource(reply));
    delete reply;
    return charinfo;
}

eveapi::corpSheet_t EveAPI::getCorpSheet(qint32& corpid)
{
    QMap<QString,QString> params;
    eveapi::corpSheet_t corpsheet;

    params["corporationID"] = QString::number(corpid);
    QNetworkReply* reply = this->apiCall("corp/CorporationSheet",params);
    if ( reply == 0 ) {
        corpsheet.id = -1;
        return corpsheet;
    }

    QXmlSimpleReader rdr;
    eveapi::EveAPICorpSheetHandler handler(&corpsheet);
    rdr.setContentHandler(&handler);
    rdr.parse(QXmlInputSource(reply));
    delete reply;
    return corpsheet;
}


#include "eveapi.moc"
