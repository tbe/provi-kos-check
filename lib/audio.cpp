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

#include "audio.h"
#include <phonon/BackendCapabilities>
#include <phonon/AudioOutput>
#include <phonon/Effect>
#include <phonon/EffectParameter>
#include <phonon/MediaObject>
#include <QtCore/QSettings>
#include <QtCore/QDebug>

using namespace pkos;

Audio* Audio::_this = 0;


Audio* Audio::getInstance()
{
    if ( _this == 0 )
        _this = new Audio;
    return _this;
}


Audio::Audio()
{
    settings      = new QSettings(this);
    settings->beginGroup("audio");
    sound    = new Phonon::MediaObject(this);
    audioOutput   = new Phonon::AudioOutput(Phonon::CommunicationCategory, this);
    audioPath     = Phonon::createPath(sound, audioOutput);

    // check if there is an alternative default device
    if(settings->contains("audio/default_device")) {
        QList<Phonon::AudioOutputDevice> outputDevices = Phonon::BackendCapabilities::availableAudioOutputDevices();
        Q_FOREACH(Phonon::AudioOutputDevice d,outputDevices) {
            if ( d.index() == settings->value("audio/default_device").toInt() && d.isValid()) {
                audioOutput->setOutputDevice(d);
                break;
            }
        }
    }
    deviceIndex = audioOutput->outputDevice().index();

    checkVolumeSupport();
    if ( useVolumeEffect ) {
        volumeEffect->setParameterValue(volumeEffect->parameters().at(0),settings->value("volume",0.25).toReal());
    } else {
        audioOutput->setVolume(settings->value("volume",0.25).toReal());
        connect(audioOutput,SIGNAL(volumeChanged(qreal)),this,SLOT(handleVolumeChanged(qreal)));
    }

    audioOutputDevices = Phonon::BackendCapabilities::availableAudioOutputDevices();

    connect(sound,SIGNAL(finished()),this,SLOT(handleFinished()));
}

Audio::~Audio()
{

}

qreal Audio::volume()
{
    qreal vol;
    if ( useVolumeEffect )
        vol = volumeEffect->parameterValue(volumeEffect->parameters().at(0)).toReal();
    else
        vol = audioOutput->volume();
    qDebug() << "current volume:" << vol;
    return vol;
}

bool Audio::hasVolumeEffect()
{
    return useVolumeEffect;
}

int Audio::outputDeviceIndex()
{
    return deviceIndex;
}

QList< Phonon::AudioOutputDevice >& Audio::outputDevices()
{
    return audioOutputDevices;
}


void Audio::setOutputDeviceIndex(int idx)
{
    Q_FOREACH(Phonon::AudioOutputDevice dev, audioOutputDevices) {
        if ( dev.index() == idx ) {
            audioOutput->setOutputDevice(dev);
            break;
        }
    }
}


void Audio::play(const QString& file)
{
#ifdef Q_WS_WIN
    // on windows, we have to stop and revind the sound
    sound->stop();
    sound->seek(0);
#endif
    // set the mediasource to our soundfile
    sound->setCurrentSource(file);
    //play the sound
    sound->play();
}

void Audio::stop()
{
    sound->stop();
}

void Audio::setVolume(qreal newVolume)
{
    if ( useVolumeEffect )
        volumeEffect->setParameterValue(volumeEffect->parameters().at(0),newVolume);
    else
        audioOutput->setVolume(newVolume);
}

void Audio::handleFinished()
{
    emit finished();
}

void Audio::handleVolumeChanged(qreal newVolume)
{
    emit volumeChanged(newVolume);
}


void Audio::checkVolumeSupport()
{
    // TODO find the real effects we need
    useVolumeEffect = false;
    QList<Phonon::EffectDescription> effects = Phonon::BackendCapabilities::availableAudioEffects();
    Q_FOREACH (Phonon::EffectDescription effect, effects) {
        qDebug() << "Effect - name:" << effect.name() << "description:" << effect.description();
        if ( effect.name() == "volume")  {
            qDebug() << "found volume effect for sound output";
            useVolumeEffect = true;
            volumeEffect = new Phonon::Effect(effect, this);
            audioPath.insertEffect(volumeEffect);
            break;
        }
    }
}

#include "audio.moc"
