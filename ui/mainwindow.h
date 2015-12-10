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

#ifndef PKOS_MAINWINDOW_H
#define PKOS_MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "../include/pkostypes.h"
//#include "../ProvidenceKOSLookup.h"

namespace Phonon {
class AudioOutput;
}

class QAction;
class QGridLayout;
class QListWidget;
class QMenu;
class QMenuBar;


namespace pkos {

class Network;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Network* net,QWidget* parent = 0, Qt::WindowFlags flags = Qt::Window);

    KOSListEntryCompact* addKosEntry(const QString& name);
    void scrollToEntry(KOSListEntryCompact* entry);
    
public slots:
    void addSeperator(const QString& pilot);

private slots:
    void displayAbout();
    void displayConfig();
    void configClosed();

    void reloadLogfilesTriggered();

signals:
    void reloadLogfiles();
    void disableSound(bool disable);

private:
    void closeEvent(QCloseEvent *event);
    void readSettings();
    void writeSettings();

    QWidget     *central;
    QGridLayout *layout;
    QMenuBar    *menu;
    QMenu       *file,*help;
    QAction     *reloadf,*configd,*quit,*about;
    QListWidget *lst;

    Network     *net;
};
}

#endif // PKOS_MAINWINDOW_H

