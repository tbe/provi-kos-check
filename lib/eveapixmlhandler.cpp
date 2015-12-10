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


#include <QtCore/QDebug>
#include "eveapixmlhandler.h"
using namespace pkos::eveapi;

EveAPIXMLHandler::EveAPIXMLHandler()
    :QXmlDefaultHandler(),inResult(false),inTag(false)
{
}

bool EveAPIXMLHandler::startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts)
{
    if (this->inResult) {
        this->cur_element = localName;
        if (localName.compare("rowset") != 0)
            this->inTag = true;
        //qDebug() << qName;
    }

    if ( localName.compare("result") == 0 )
        this->inResult = true;
    return QXmlDefaultHandler::startElement(namespaceURI, localName, qName, atts);
}

bool EveAPIXMLHandler::endElement(const QString& namespaceURI, const QString& localName, const QString& qName)
{
    if (this->inResult)
        this->inTag = false;


    if ( localName.compare("result") == 0 )
        this->inResult = false;


    return QXmlDefaultHandler::endElement(namespaceURI, localName, qName);
}


bool EveAPIXMLHandler::characters(const QString& ch)
{
    if ( this->inTag ) {
        this->handleText(ch);
    }
    return QXmlDefaultHandler::characters(ch);
}

void EveAPIXMLHandler::handleText(const QString& ch)
{
    qDebug() << ch;
}



EveAPICharacterInfoHandler::EveAPICharacterInfoHandler(characterInfo_t* charinfo)
    :EveAPIXMLHandler(),charinfo(charinfo)
{
    tags << "characterID" << "characterName" << "race" << "bloodline" << "corporationID"
         << "corporation" << "corporationDate" << "securityStatus";
}



void EveAPICharacterInfoHandler::handleText(const QString& ch)
{
    switch(tags.indexOf(this->cur_element)) {
    case 0:
        this->charinfo->id = ch.toInt();
        break;
    case 1:
        this->charinfo->name = ch;
        break;
    case 2:
        this->charinfo->race = ch;
        break;
    case 3:
        this->charinfo->bloodline = ch;
        break;
    case 4:
        this->charinfo->corporationID = ch.toInt();
        break;
    case 5:
        this->charinfo->corporation = ch;
        break;
    case 6:
        this->charinfo->corporationDate = ch;
        break;
    case 7:
        this->charinfo->securityStatus = ch.toFloat();
        break;
    default:
        break;
    }
}

bool EveAPICharacterInfoHandler::startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts)
{
    if ( this->cur_element.compare("rowset") == 0 ) {
        if ( localName.compare("row") == 0 ) {
            employmentHistory_t record;
            record.id = atts.value("recordID").toInt();
            record.corporationID = atts.value("corporationID").toInt();
            record.startDate = atts.value("startDate");
            this->charinfo->employmentHistory.append(record);
            return true;
        }
        return false;
    } else {
        return EveAPIXMLHandler::startElement(namespaceURI, localName, qName, atts);
    }
}



EveAPIPlayerIDHandler::EveAPIPlayerIDHandler(const QString& wanted,qint32& playerid)
    :EveAPIXMLHandler(),playerid(&playerid),wanted(wanted)
{
}

bool EveAPIPlayerIDHandler::startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts)
{
    if ( this->cur_element.compare("rowset") == 0 ) {
        if ( localName.compare("row") == 0 ) {
            employmentHistory_t record;
            if ( atts.value("name").compare(wanted) == 0)
                *playerid = atts.value("characterID").toInt();
            return true;
        }
        return false;
    } else {
        return EveAPIXMLHandler::startElement(namespaceURI, localName, qName, atts);
    }
}


EveAPICorpSheetHandler::EveAPICorpSheetHandler(corpSheet_t* corpinfo):
    EveAPIXMLHandler(),corpinfo(corpinfo)
{
    tags << "corporationID" << "corporationName" << "allianceName" << "allianceID";
}


void EveAPICorpSheetHandler::handleText(const QString& ch)
{
    switch(tags.indexOf(this->cur_element)) {
    case 0:
        this->corpinfo->id = ch.toInt();
        break;
    case 1:
        this->corpinfo->name = ch;
        break;
    case 2:
        this->corpinfo->allianceName = ch;
        break;
    case 3:
        this->corpinfo->allianceID = ch.toInt();
        break;
    default:
        break;
    }
}
