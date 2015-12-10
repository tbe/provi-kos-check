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

#ifndef PKOS_NETWORK_H
#define PKOS_NETWORK_H

#include <QtCore/QObject>
#include <QtCore/QMap>

class QNetworkAccessManager;
class QNetworkReply;

namespace pkos {
/** @class Network
 *  @brief simple wrapper around QNetworkAccessManager
 */
class Network : public QObject
{
    Q_OBJECT

public:
    Network(QObject* parent);
    /**
     * @brief do a sync HTTP Request
     * @param url URL for the Request
     * @param params Map with key->value pairs for the url paramterers
     *
     * @returns the QNetworkReply with the answer from the HTTP Request. May be null on error
     * @warning the QNetworkReply must be deleted after use!
     */
    QNetworkReply* httpSyncRequest (const QString& url, const QMap< QString, QString >& params);

    /**
     * @brief do a async HTTP Request
     * @param url URL for the Request
     * @param params Map with key->value pairs for the url paramterers
     *
     * @returns the QNetworkReply for the ongoing request.
     * @warning the QNetworkReply must be delted after use!
     */
    QNetworkReply* httpAsyncRequest(const QString& url, const QMap< QString, QString >& params);

private:
    QNetworkAccessManager *networkManager;

};
}

#endif // PKOS_NETWORK_H
