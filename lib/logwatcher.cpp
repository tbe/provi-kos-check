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

#include "logwatcher.h"
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtCore/QFile>

using namespace pkos;

logWatcher::logWatcher(QObject* parent)
{
#ifdef Q_WS_WIN
    connect(this,SIGNAL(fileHasData(QString)),this,SLOT(fileChanged(QString)));
#else
    watcher = new QFileSystemWatcher(this);
    connect(watcher,SIGNAL(fileChanged(QString)),this,SLOT(fileChanged(QString))) ;
#endif

#ifdef Q_WS_WIN
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkFiles()));
    timer->start(500);
#endif
}

bool logWatcher::addLog(const QString& logfile)
{
    QFile* file = new QFile(this);
    file->setFileName(logfile);
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug() << file->errorString();
        return false;
    }
    file->seek(file->size());
    files[logfile] = file;
#ifndef Q_WS_WIN
    watcher->addPath(logfile);
#endif
    return true;
}

void logWatcher::reset()
{
    QMapIterator<QString,QFile*> it(files);
    while (it.hasNext()) {
        it.next();
#ifndef Q_WS_WIN
        watcher->removePath(it.key());
#endif
        it.value()->close();
        delete it.value();
    }
    files.clear();
}


#ifdef Q_WS_WIN
void logWatcher::checkFiles()
{
    QMapIterator<QString,QFile*> it(files);
    while(it.hasNext()) {
        it.next();
        if (!it.value()->atEnd() )
            emit fileHasData(it.key());
    }
}
#endif

void logWatcher::fileChanged(QString fpath)
{
    qDebug() << "file changed: " + fpath;
    QFile* file = files[fpath];
    QString line_tmp;
    QTextStream strm(file);

    while (!strm.atEnd()) {
        QString line_tmp = strm.readLine();
        QString line;
        Q_FOREACH (QChar c, line_tmp) {
            if (c.isPrint() && c != '\n' && c != '\r')
                line.append(c);
        }
        qDebug() << line;
        QRegExp re("\\] (.*) > xxx +([^ ].*)\\s*$");
        QRegExp re_dots("\\.\\.\\.\\s*$");
        int idx = re.indexIn(line);
        if (idx >= 0 ) {
            QString player = re.cap(1);
            QString names  = re.cap(2).replace(re_dots,"");
            QStringList namelist = names.split("  ");

            emit newCheck(player,namelist);
        } else  {
            qDebug() << "regex unmatched!";
            qDebug() << line;
        }
    }
}


#include "moc_logwatcher.cpp"
