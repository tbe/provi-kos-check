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

#include "koslistentrycompact.h"
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QMovie>
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QDesktopServices>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QDebug>

#include "../ProvidenceKOSLookup.h"

using namespace pkos;

const QString killboard_pilot("https://zkillboard.com/character/%1/");
const QString killboard_corp("https://zkillboard.com/corporation/%1/");
const QString killboard_ally("https://zkillboard.com/alliance/%1/");

KOSListEntryCompact::KOSListEntryCompact(QWidget* parent, QListWidgetItem* itm, Network* net)
    :QWidget(parent),itm(itm),net(net)
{

    if ( this->net == 0 )
        this->net = new Network(this);

    l_avatar = new QLabel(this);
    l_avatar->setObjectName(QString::fromUtf8("uniticon"));
    l_avatar->setGeometry(QRect(2, 2, 32, 32));
    l_name = new QLabel(this);
    l_name->setObjectName(QString::fromUtf8("unitname"));
    l_name->setGeometry(QRect(40, 2, 260, 16));
    l_kosby = new QLabel(this);
    l_kosby->setObjectName(QString::fromUtf8("kosby"));
    l_kosby->setGeometry(QRect(40, 18, 260, 16));

    contextMenu = new QMenu(this);

    markAsMenu  = new QMenu(tr("&mark as"),contextMenu);
    hotdropper_m  = new QAction(tr("&hotdropper"),markAsMenu);
    hotdropper_m->setCheckable(true);


    killboard_m   = new QAction(tr("show &killboard"),contextMenu);
    cva_m         = new QAction(tr("show &CVA database"),contextMenu);

    fallback_icon = false;
}

void KOSListEntryCompact::setName(const QString& name)
{
    l_name->setText(name);
    l_kosby->setText(tr("not checked"));
    l_name->setStyleSheet("color: grey; font-weight: bold;");
    QMovie *movie = new QMovie(":/listentry/loader.gif");
    movie->setScaledSize(QSize(32,32));
    l_avatar->setMovie(movie);
    movie->start();
}



void KOSListEntryCompact::setData(const list_entry_data_t& data)
{
    this->data = data;
    l_name->setText(data.name);
    QString urlstring(data.icon);
    if ( urlstring.size() == 0 )
        urlstring = QString("https://image.zkillboard.com/Alliance/99003651_32.png");

    QMap<QString,QString> dummy;
    reply = this->net->httpAsyncRequest(urlstring,dummy);
    connect(reply,SIGNAL(finished()),this,SLOT(drawImage()));

    if ( data.kos > 0 ) {
        qDebug() << "setting red";
        l_name->setStyleSheet("color: red; font-weight: bold;");
        l_kosby->setText(data.kosby);
    } else if ( data.kos == 0 ) {
        qDebug() << "setting green";
        l_name->setStyleSheet("color: green; font-weight: bold;");
        l_kosby->setText("Not KOS");
    } else  {
        qDebug() << "setting gray";
        l_kosby->setText("Not listed - be carefull");
        l_kosby->setStyleSheet("color: grey; font-weight: bold;");
    }
    if ( data.unit_type == 1) {
        contextMenu->addMenu(markAsMenu);
        markAsMenu->addAction(hotdropper_m);
        l_hd = new QLabel(this);
        connect(hotdropper_m,SIGNAL(toggled(bool)),this,SLOT(updateHotdropper(bool)));
        no_HDupdate = true;
        hotdropper_m->setChecked(data.hotdropper);
        no_HDupdate = false;
    }
    this->setContextMenuPolicy(Qt::DefaultContextMenu);
    if ( data.unit_type > 0 ) {
        contextMenu->addAction(killboard_m);
        connect(killboard_m,SIGNAL(triggered(bool)),this,SLOT(openKillboard()));
        switch ( data.unit_type ) {
        case 1:
            killboard_link = new QUrl(killboard_pilot.arg(data.id));
            break;
        case 2:
            killboard_link = new QUrl(killboard_corp.arg(data.id));
            break;
        case 3:
            killboard_link = new QUrl(killboard_ally.arg(data.id));
            break;
        }
    }
    contextMenu->addAction(cva_m);
    cva_link = new QUrl("http://kos.cva-eve.org/");
    cva_link->addQueryItem("q",data.name);
    connect(cva_m,SIGNAL(triggered(bool)),this,SLOT(openCVA()));
}

const QSize KOSListEntryCompact::sizeHint()
{
    return QSize(300,36);
}

const QSize KOSListEntryCompact::maximumSizeHint()
{
    return sizeHint();
}

const QSize KOSListEntryCompact::minimumSizeHint()
{
    return sizeHint();
}

QUrl redirectUrl(const QUrl& possibleRedirectUrl, const QUrl& oldRedirectUrl) {
    QUrl redirectUrl;
    if(!possibleRedirectUrl.isEmpty() && possibleRedirectUrl != oldRedirectUrl) {
        redirectUrl = QUrl(QString("http://image.eveonline.com") + possibleRedirectUrl.toString());

    }
    return redirectUrl;
}

void KOSListEntryCompact::drawImage()
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Error in" << reply->url() << ":" << reply->errorString();
        reply->deleteLater();
        return;
    }
    QVariant redUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QUrl urlRedirectedTo = redirectUrl(redUrl.toUrl(), reply->url());

    if(!urlRedirectedTo.isEmpty()) {
        reply->deleteLater();
        QMap<QString,QString> dummy;
        reply = this->net->httpSyncRequest(urlRedirectedTo.toString(),dummy);
    }
    QByteArray pngData = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(pngData);
    l_avatar->setPixmap(pixmap);
    reply->deleteLater();
    //delete l_avaMovie;
}

void KOSListEntryCompact::openKillboard()
{
    QDesktopServices::openUrl(*killboard_link);
}

void KOSListEntryCompact::openCVA()
{
    QDesktopServices::openUrl(*cva_link);
}


void KOSListEntryCompact::updateHotdropper(bool hd)
{
    if (!no_HDupdate)
        data.updateHandler->updateHD(data.id,data.name,hd);
    if (hd) {
        int x_pos = l_name->minimumSizeHint().width() + 40 + 5;
        QRect newpos(x_pos,2,16,16);
        qDebug() << " showing hotdropper icon at" << newpos;
        QPixmap pixmap;
        qDebug() << " loading hotdropper image ..." << pixmap.load(":/listentry/hotdropper.png");
        l_hd->setPixmap(pixmap);
        l_hd->setGeometry(newpos);
        l_hd->show();
    } else {
        l_hd->hide();
    }
}


void KOSListEntryCompact::contextMenuEvent(QContextMenuEvent* event)
{
    qDebug() << "opening context menu";
    contextMenu->exec(event->globalPos());
}


#include "koslistentrycompact.moc"

