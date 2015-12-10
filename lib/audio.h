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

#ifndef PKOS_AUDIO_H
#define PKOS_AUDIO_H

#include <QtCore/QObject>
#include <phonon/Path>

class QSettings;

namespace Phonon {
class MediaObject;
class AudioOutput;
class Effect;
}

namespace pkos {

class Audio : public QObject
{
    Q_OBJECT
public:
    static Audio* getInstance();
    ~Audio();

    qreal volume();
    bool  hasVolumeEffect();

    int   outputDeviceIndex();
    void  setOutputDeviceIndex(int idx);

    QList<Phonon::AudioOutputDevice>& outputDevices();

signals:
    void finished();
    void volumeChanged(qreal newVolume);

public slots:
    void play(const QString &file);
    void stop();
    void setVolume(qreal newVolume);

private slots:
    void handleFinished();
    void handleVolumeChanged(qreal newVolume);

private:
    Audio();
    void checkVolumeSupport();

    static Audio* _this;

    QSettings *settings;

    QList<Phonon::AudioOutputDevice>    audioOutputDevices;

    bool                    useVolumeEffect;
    int                     deviceIndex;
    Phonon::MediaObject*    sound;
    Phonon::AudioOutput*    audioOutput;
    Phonon::Path            audioPath;
    Phonon::Effect*         volumeEffect;
};
}

#endif // PKOS_AUDIO_H
