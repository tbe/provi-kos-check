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

#ifndef KOSLISTENTRYCOMPACT_H
#define KOSLISTENTRYCOMPACT_H

#include <QtCore/QObject>
#include <QtGui/QWidget>

#include "../lib/network.h"
#include "../include/pkostypes.h"

class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QListWidgetItem;
class QMovie;
class QMenu;
class QUrl;

class QNetworkReply;

namespace pkos {
class KOSListEntryCompact : public QWidget {
    Q_OBJECT

public:
    KOSListEntryCompact(QWidget* parent, QListWidgetItem* itm,Network* net = 0);

    void setName(const QString& name);
    void setData(const list_entry_data_t& data);

    const QSize sizeHint();
    const QSize minimumSizeHint();
    const QSize maximumSizeHint();

    QListWidgetItem* getItem() {
        return itm;
    }

private slots:
    void drawImage();
    void openKillboard();
    void updateHotdropper(bool hd);
    void openCVA();

private:
    void contextMenuEvent(QContextMenuEvent *event);

    Network *net;
    QHBoxLayout *cLayout;
    QVBoxLayout *vLayout;
    QGridLayout *gLayout;
    QLabel *l_avatar;
    QMovie *l_avaMovie;
    QLabel *l_name;
    QLabel *l_kos;
    QLabel *l_kosby;
    QLabel *l_hd;


    bool fallback_icon;

    QNetworkReply *reply;
    QListWidgetItem* itm;

    // context menu
    QMenu   *contextMenu,*markAsMenu;
    QAction *hotdropper_m,*killboard_m,*cva_m;

    QUrl *killboard_link,*cva_link;
    list_entry_data_t data;
    bool no_HDupdate;
};

}
#endif // KOSLISTENTRYCOMPACT_H
