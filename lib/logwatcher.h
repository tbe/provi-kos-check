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

#ifndef LOGWATCHER_H
#define LOGWATCHER_H

#include <QtCore/QObject>
#include <QtCore/QMap>

class QFile;
class QFileSystemWatcher;

namespace pkos {

/**
 * @class logWatcher
 * @brief basic logfile watcher and parser
 */
class logWatcher : public QObject
{
    Q_OBJECT

public:
    logWatcher(QObject* parent);
    /**
     * @brief adds a logfile to the watchlist
     * @param logfile path to the logfile to monitor
     * @returns true on success, false if an error occured
     */
    bool addLog(const QString& logfile);

    ///@brief resets the curren watchlist
    void reset();

signals:
    /**
     * @brief emitted if a new list of units to check was found
     * @param player name of the requesting player
     * @param units list of units to check
     */
    void newCheck(const QString& player, const QStringList& units);
#ifdef Q_WS_WIN
    /**
     * @brief emitted if a file has new data
     * @param fpath path to the file with new data
     */
    void fileHasData(QString fpath);
#endif

private slots:
    /**
     * @brief gets called of a file was changed
     * @param fpath path to the file with new data
     */
    void fileChanged(QString fpath);
#ifdef Q_WS_WIN
    /**
     * @brief checks all files for new data
     */
    void checkFiles();
#endif

private:
#ifndef Q_WS_WIN
    QFileSystemWatcher *watcher;
#endif
    QMap<QString,QFile*> files;
};
}
#endif // LOGWATCHER_H
