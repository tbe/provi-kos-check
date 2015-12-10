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

#ifndef PKOS_EVEAPI_EVEAPIXMLHANDLER_H
#define PKOS_EVEAPI_EVEAPIXMLHANDLER_H

#include <QtXml/QXmlDefaultHandler>
#include "eveapi.h"


/** @file eveapixmlhandler.h
 *  @brief contains the internaly used XML Handlers for QXmlParser
 */

namespace pkos {
namespace eveapi {

/** @class EveAPIXMLHandler
 *  @brief basic XML Handler for the EveAPI
 */
class EveAPIXMLHandler : public QXmlDefaultHandler {
public:
    EveAPIXMLHandler();
    /// @brief handles opening tags for elements
    virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts);
    /// @brief handles the closing tags for elements
    virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
    /// @brief handles the contents of a tag
    virtual bool characters(const QString& ch);
    /// @brief handles the contents of a tag stateful
    virtual void handleText(const QString& ch);

protected:
    QString cur_element;
    bool    inResult;
    bool    inTag;

};

/** @class EveAPICharacterInfoHandler
 *  @brief XML Handler for the EveAPI Character Info Structure
 */
class EveAPICharacterInfoHandler : public EveAPIXMLHandler {
public:
    EveAPICharacterInfoHandler(characterInfo_t* charinfo);
    bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts);
    void handleText(const QString& ch);

private:
    characterInfo_t* charinfo;
    QStringList     tags;
};

/** @class EveAPIPlayerIDHandler
 *  @brief XML Handler for the EveAPI Player ID Structure
 */
class EveAPIPlayerIDHandler : public EveAPIXMLHandler {
public:
    EveAPIPlayerIDHandler(const QString& wanted, qint32& playerid);

    bool startElement(const QString& namespaceURI, const QString& localName,
                      const QString& qName, const QXmlAttributes& atts);


private:
    qint32*         playerid;
    QString         wanted;
};

/** @class EveAPICorpInfoHandler
 *  @brief XML Handler for the EveAPI Corp Info Structure
 */
class EveAPICorpSheetHandler : public EveAPIXMLHandler {
public:
    EveAPICorpSheetHandler(corpSheet_t* corpinfo);
    void handleText(const QString& ch);

private:
    corpSheet_t* corpinfo;
    QStringList     tags;
};

}
}

#endif // PKOS_EVEAPI_EVEAPIXMLHANDLER_H
