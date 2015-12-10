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

#include "mainwindow.h"

#include "aboutbox.h"
#include "koslistseperator.h"
#include "koslistentrycompact.h"
#include "configdialog.h"
#include "../lib/pdb/client.h"


#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtGui/QAction>
#include <QtGui/QCloseEvent>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>




using namespace pkos;

MainWindow::MainWindow(Network* net, QWidget* parent, Qt::WindowFlags flags)
    :QMainWindow(parent,flags),net(net)
{
    this->setWindowTitle(QString("KOS Checker for Providence"));
    this->setWindowIcon(QIcon(":/icon.png"));

    readSettings();

    central = new QWidget(this);
    layout = new QGridLayout(central);

    menu = new QMenuBar(this);
    lst = new QListWidget(central);

    layout->addWidget(lst , 0, 0, 1, 1);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    lst->setMinimumWidth(300);

    this->setCentralWidget(central);
    this->setMenuBar(menu);

    file = new QMenu(menu);
    file->setTitle("&File");
    menu->addMenu(file);

    reloadf = new QAction(file);
    reloadf->setText("&Reload Logs");
    file->addAction(reloadf);

    configd = new QAction(file);
    configd->setText("&Configuration");
    file->addAction(configd);

    quit = new QAction(file);
    quit->setText("&Quit");
    file->addAction(quit);

    help = new QMenu(menu);
    help->setTitle("&Help");
    menu->addMenu(help);

    about = new QAction(help);
    about->setText("&About");
    help->addAction(about);



    connect(reloadf,SIGNAL(triggered(bool)),this,SLOT(reloadLogfilesTriggered()));
    connect(configd,SIGNAL(triggered(bool)),this,SLOT(displayConfig()));
    connect(about,SIGNAL(triggered(bool)),this,SLOT(displayAbout()));
    connect(quit,SIGNAL(triggered(bool)),this,SLOT(close()));

}

KOSListEntryCompact* MainWindow::addKosEntry(const QString& name)
{
    QListWidgetItem *itm = new QListWidgetItem(lst);
    lst->insertItem(0,itm);

    KOSListEntryCompact* itmw = new KOSListEntryCompact(this,itm);
    itmw->setName(name);

    lst->setItemWidget(itm,itmw);
    itm->setSizeHint(itmw->sizeHint());
    //lst->scrollToItem(itm);
    return itmw;
}

void MainWindow::scrollToEntry(KOSListEntryCompact* entry)
{
    lst->scrollToItem(entry->getItem());
}


void MainWindow::addSeperator(const QString& pilot)
{
    QListWidgetItem *itm = new QListWidgetItem(lst);
    lst->insertItem(0,itm);
    pkos::KOSListSeperator* itmw = new KOSListSeperator(lst);
    lst->setItemWidget(itm,itmw);
    itm->setSizeHint(itmw->sizeHint());
    itmw->setPilot(pilot);
    lst->scrollToItem(itm);
}


void MainWindow::displayAbout()
{
    AboutBox about(this);
    about.exec();
}

void MainWindow::displayConfig()
{
    emit disableSound(true);
    ConfigDialog cfgd(this);
    connect(&cfgd,SIGNAL(destroyed(QObject*)),this,SLOT(configClosed()));
    cfgd.exec();
}

void MainWindow::configClosed()
{
    emit disableSound(false);
}

void MainWindow::reloadLogfilesTriggered()
{
    emit reloadLogfiles();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    writeSettings();
    QWidget::closeEvent(event);
}

void MainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(322,800)).toSize());
    if ( settings.contains("pos") )
        move(settings.value("pos").toPoint());
    settings.endGroup();
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}


#include "mainwindow.moc"
