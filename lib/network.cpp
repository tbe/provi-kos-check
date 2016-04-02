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

#include "network.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QEventLoop>
#include <QtCore/QDebug>

#ifndef USERAGENT
#define USERAGENT "ProvidenceKOSLookup 2.0 / YF [TN-NT] Rounon Dax"
#endif//USERAGENT

using namespace pkos;

Network::Network(QObject* parent):
    QObject(parent)
{
    this->networkManager = new QNetworkAccessManager(this);
}

QNetworkReply* Network::httpAsyncRequest(const QString& url, const QMap<QString,QString>& params)
{
    QUrl requrl(url);
    QMapIterator<QString,QString> it(params);
    while(it.hasNext()) {
        it.next();
        requrl.addQueryItem(it.key(),it.value());
    }
    QNetworkRequest request;
    request.setUrl(requrl);
    request.setRawHeader("User-Agent",USERAGENT);
    return this->networkManager->get(request);
}

QNetworkReply* Network::httpSyncRequest(const QString& url, const QMap< QString, QString >& params)
{
    QEventLoop loop;
    QNetworkReply* reply = this->httpAsyncRequest(url,params);
    connect(reply,SIGNAL(finished()),&loop,SLOT(quit()));
    loop.exec();
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error in" << reply->url() << ":" << reply->errorString();
        delete reply;
        return 0;
    }
    return reply;
}


#include "network.moc"
