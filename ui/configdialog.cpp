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

#include "configdialog.h"

#include <QtCore/QSettings>
#include <QtGui/QDesktopServices>
#include <QtGui/QFileDialog>
#include <QtGui/QVBoxLayout>
#include <QtGui/QTabWidget>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QCheckBox>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QSpacerItem>
#include <QtGui/QSlider>


#include <phonon/BackendCapabilities>
#include <phonon/MediaObject>
#include <phonon/AudioOutput>

#include <QtCore/QDebug>

#include "mimetypes.h"

#include "../lib/audio.h"

using namespace pkos;

ConfigDialog::ConfigDialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    this->resize(380, 280);
    this->setWindowTitle("Configuration");


    centralLayout   = new QVBoxLayout(this);
    tabs            = new QTabWidget(this);

    audiotab        = new QWidget(tabs);
    setup_audiotab();
    tabs->addTab(audiotab, QString());
    tabs->setTabText(tabs->indexOf(audiotab),"Audio");

#ifdef Q_WS_X11
    systemtab       = new QWidget(tabs);
    setup_systemtab();
    tabs->addTab(systemtab,QString());
    tabs->setTabText(tabs->indexOf(systemtab),"System");
#endif
    centralLayout->addWidget(tabs);

    buttons = new QDialogButtonBox(this);
    buttons->setOrientation(Qt::Horizontal);
    buttons->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Save);

    centralLayout->addWidget(buttons);


    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    tabs->setCurrentIndex(0);
    this->getCurrentConfig();
}

ConfigDialog::~ConfigDialog()
{
    audio->stop();
}


void ConfigDialog::setup_audiotab()
{
    audio               = Audio::getInstance();
    audiotab_layout     = new QVBoxLayout(audiotab);

    audio_label         = new QLabel(audiotab);
    audio_label->setText(tr("alarm sound"));
    audio_label->setAlignment(Qt::AlignCenter);
    audiotab_layout->addWidget(audio_label);

    useInternal         = new QCheckBox(audiotab);
    useInternal->setText("use internal sound");
    audiotab_layout->addWidget(useInternal);

    fileinput_layout    = new QHBoxLayout();
    audiotab_layout->addLayout(fileinput_layout);

    fileinput           = new QLineEdit(audiotab);
    fileinput->setEnabled(false);
    fileinput_layout->addWidget(fileinput);

    fileinput_button    = new QPushButton(audiotab);
    fileinput_button->setText("select file");
    fileinput_layout->addWidget(fileinput_button);

    play_button         = new QPushButton(audiotab);
    play_button->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    audiotab_layout->addWidget(play_button);

    line1 = new QFrame(audiotab);
    line1->setFrameShape(QFrame::HLine);
    line1->setFrameShadow(QFrame::Sunken);
    audiotab_layout->addWidget(line1);

    hd_label         = new QLabel(audiotab);
    hd_label->setText(tr("hotdropper sound"));
    hd_label->setAlignment(Qt::AlignCenter);
    audiotab_layout->addWidget(hd_label);

    useInternal_hd   = new QCheckBox(audiotab);
    useInternal_hd->setText("use internal sound");
    audiotab_layout->addWidget(useInternal_hd);

    fileinput_layout_hd  = new QHBoxLayout();
    audiotab_layout->addLayout(fileinput_layout_hd);

    fileinput_hd         = new QLineEdit(audiotab);
    fileinput_hd->setEnabled(false);
    fileinput_layout_hd->addWidget(fileinput_hd);

    fileinput_button_hd  = new QPushButton(audiotab);
    fileinput_button_hd->setText("select file");
    fileinput_layout_hd->addWidget(fileinput_button_hd);

    play_button_hd       = new QPushButton(audiotab);
    play_button_hd->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    audiotab_layout->addWidget(play_button_hd);

    line1_hd = new QFrame(audiotab);
    line1_hd->setFrameShape(QFrame::HLine);
    line1_hd->setFrameShadow(QFrame::Sunken);
    audiotab_layout->addWidget(line1_hd);

    output_label = new QLabel(audiotab);
    output_label->setText("Output device");
    audiotab_layout->addWidget(output_label);

    output_device = new QComboBox(audiotab);
    audiotab_layout->addWidget(output_device);

    line2 = new QFrame(audiotab);
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    audiotab_layout->addWidget(line2);

    volume_label = new QLabel(audiotab);
    volume_label->setText("alert volume");
    audiotab_layout->addWidget(volume_label);

    volSlider_layout = new QHBoxLayout(audiotab);
    audiotab_layout->addLayout(volSlider_layout);

    volPixmap = new QLabel(audiotab);
    volPixmap->setPixmap(style()->standardIcon(QStyle::SP_MediaVolume).pixmap(16));
    volSlider_layout->addWidget(volPixmap);

    volSlider = new QSlider(audiotab);
    volSlider->setOrientation(Qt::Horizontal);
    volSlider->setMinimum(0);
    if ( audio->hasVolumeEffect() )
        volSlider->setMaximum(100);
    else
        volSlider->setMaximum(150);
    volSlider->setValue(audio->volume()*100);
    volSlider_layout->addWidget(volSlider);

    volSlider_label = new QLabel();
    setVolumeText(volSlider->value());
    volSlider_layout->addWidget(volSlider_label);

    verticalSpacer = new QSpacerItem(20, 43, QSizePolicy::Minimum, QSizePolicy::Expanding);
    audiotab_layout->addItem(verticalSpacer);

    useInternal->setCheckState(Qt::Checked);
    this->audio_useInternalToogled(Qt::Checked);

    useInternal_hd->setCheckState(Qt::Checked);
    this->audio_useInternalHDToogled(Qt::Checked);

    playing = false;
    //audioOutput->setVolume(settings->value("audio/volume",0.25).toReal());

    oldDeviceIndex = audio->outputDeviceIndex();
    oldVolume      = audio->volume();
    changingVolume = false;

    connect(useInternal,SIGNAL(stateChanged(int)),this,SLOT(audio_useInternalToogled(int)));
    connect(useInternal_hd,SIGNAL(stateChanged(int)),this,SLOT(audio_useInternalHDToogled(int)));
    connect(play_button,SIGNAL(clicked(bool)),this,SLOT(playButtonTriggered()));

    connect(fileinput_button,SIGNAL(clicked(bool)),this,SLOT(findFile()));
    connect(fileinput_button_hd,SIGNAL(clicked(bool)),this,SLOT(findFile_hd()));
    connect(play_button_hd,SIGNAL(clicked(bool)),this,SLOT(playButtonHDTriggered()));


    connect(volSlider,SIGNAL(valueChanged(int)),this,SLOT(volumeSliderMoved(int)));
    connect(audio,SIGNAL(finished()),this,SLOT(audioDone()));
    connect(audio,SIGNAL(volumeChanged(qreal)),this,SLOT(upstreamVolumeChanged(qreal)));
}


void ConfigDialog::setup_systemtab()
{
    systemtab_layout = new QVBoxLayout(systemtab);
#ifdef Q_WS_X11
    winedoc_label   = new QLabel(systemtab);
    winedoc_label->setText("wine document folder");
    winedoc_label->setAlignment(Qt::AlignCenter);
    systemtab_layout->addWidget(winedoc_label);

    customWineDoc   = new QCheckBox(systemtab);
    customWineDoc->setText("use default wine settings");
    systemtab_layout->addWidget(customWineDoc);

    winedoc_layout  = new QHBoxLayout();
    systemtab_layout->addLayout(winedoc_layout);

    winedoc_input           = new QLineEdit(systemtab);
    winedoc_input->setEnabled(false);
    winedoc_layout->addWidget(winedoc_input);

    winedoc_button  = new QPushButton(systemtab);
    winedoc_button->setText("select dir");
    winedoc_layout->addWidget(winedoc_button);

    winedoc_warning = new QLabel(systemtab);
    systemtab_layout->addWidget(winedoc_warning);

    systemtab_spacer = new QSpacerItem(20, 43, QSizePolicy::Minimum, QSizePolicy::Expanding);
    systemtab_layout->addItem(systemtab_spacer);

    customWineDoc->setCheckState(Qt::Checked);
    this->system_wineDefaultsToogled(Qt::Checked);

    connect(customWineDoc,SIGNAL(stateChanged(int)),this,SLOT(system_wineDefaultsToogled(int)));
    connect(winedoc_button,SIGNAL(clicked(bool)),this,SLOT(findWineFolder()));
#endif
}

void ConfigDialog::getCurrentConfig()
{
    QSettings settings;
    settings.beginGroup("audio");
    // get all needed data for audio output settings
    QStringList mimeTypes   = Phonon::BackendCapabilities::availableMimeTypes();

    // add all audio devices to the drop down list
    Q_FOREACH(Phonon::AudioOutputDevice dev,audio->outputDevices()) {
        if(dev.isValid())
            output_device->addItem(dev.name(),QVariant::fromValue(dev.index()));
    }
    // get the current device
    output_device->setCurrentIndex(output_device->findData(settings.value("current_device").toInt()));

    // set the audio file
    if(settings.contains("file")) {
        useInternal->setCheckState(Qt::Unchecked);
        audio_useInternalToogled(Qt::Unchecked);
        fileinput->setText(settings.value("file").toString());
    }

    if (settings.contains("hotdropper")) {
        useInternal_hd->setCheckState(Qt::Unchecked);
        audio_useInternalHDToogled(Qt::Unchecked);
        fileinput_hd->setText(settings.value("hotdropper").toString());
    }

    // init the mimetype lookup map
    init_mimetypes();
    QStringList extensions;
    QMap<QString,QStringList>::iterator it;
    Q_FOREACH(QString mime,mimeTypes) {
        it = mimeTypeExtensions.find(mime);
        if ( it != mimeTypeExtensions.end() )
            Q_FOREACH(QString s,it.value()) {
            extensions <<  "*." + s;
        }
    }
    QSet<QString> extensionSet = QSet<QString>::fromList(extensions);
    audioFileExtensions = QStringList::fromSet(extensionSet);
    audioFileExtensions.sort();
    settings.endGroup();

#ifdef Q_WS_X11
    settings.beginGroup("wine");
    if(settings.contains("docpath")) {
        customWineDoc->setCheckState(Qt::Unchecked);
        system_wineDefaultsToogled(Qt::Unchecked);
        winedoc_input->setText(settings.value("docpath").toString());
    } else
        winedoc_input->setText(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
    settings.endGroup();
#endif
}


void ConfigDialog::audio_useInternalToogled(int state)
{
    if ( state == Qt::Checked )
        fileinput_button->setEnabled(false);
    else
        fileinput_button->setEnabled(true);
}

void ConfigDialog::findFile()
{
    QString filter = "Audio files (%1)";
    fileinput->setText(
        QFileDialog::getOpenFileName(this,tr("Select File"),QDesktopServices::storageLocation(QDesktopServices::MusicLocation),tr(filter.toStdString().c_str()).arg(audioFileExtensions.join(" ")))
    );
}

void ConfigDialog::playButtonTriggered()
{
    audio->stop();
    if ( !playing ) {
        play_button_hd->setEnabled(false);
        play_button->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
        if ( useInternal->checkState() == Qt::Checked || fileinput->text().size() == 0 ) {
            QSettings settings;
            audio->play(settings.value("audio/file",":/alarm.wav").toString());
        } else
            audio->play(fileinput->text());
    } else {
        play_button_hd->setEnabled(true);
        play_button->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
    playing = !playing;
}

void ConfigDialog::audioDone()
{
    play_button->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    play_button->setEnabled(true);
    play_button_hd->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    play_button_hd->setEnabled(true);
    playing = false;
}


void ConfigDialog::audio_useInternalHDToogled(int state)
{
    if ( state == Qt::Checked )
        fileinput_button_hd->setEnabled(false);
    else
        fileinput_button_hd->setEnabled(true);
}

void ConfigDialog::findFile_hd()
{
    QString filter = "Audio files (%1)";
    fileinput_hd->setText(
        QFileDialog::getOpenFileName(this,tr("Select File"),QDesktopServices::storageLocation(QDesktopServices::MusicLocation),tr(filter.toStdString().c_str()).arg(audioFileExtensions.join(" ")))
    );
}

void ConfigDialog::playButtonHDTriggered() {
    audio->stop();
    if ( !playing ) {
        play_button->setEnabled(false);
        play_button_hd->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
        if ( useInternal_hd->checkState() == Qt::Checked || fileinput_hd->text().size() == 0 ) {
            QSettings settings;
            audio->play(settings.value("audio/hotdropper",":/hotdropper.wav").toString());
        } else
            audio->play(fileinput_hd->text());
    } else {
        play_button->setEnabled(true);
        play_button_hd->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
    playing = !playing;
}


void ConfigDialog::volumeSliderMoved(int newValue)
{
    qreal volume = (static_cast<qreal>(newValue)) * 0.01;
    if ( !changingVolume && volume != audio->volume() ) {
        changingVolume = true;
        setVolumeText(newValue);
        QMetaObject::invokeMethod(audio,"setVolume",Q_ARG(const qreal,volume));
    }
    changingVolume = false;
}

void ConfigDialog::upstreamVolumeChanged(qreal volume)
{
    int value = qRound(100 * volume);
    if ( !changingVolume && volSlider->value() != value ) {
        changingVolume = true;
        volSlider->setValue(volume*100);
        setVolumeText(volume*100);
    }
    changingVolume = false;
}

void ConfigDialog::system_wineDefaultsToogled(int state)
{
    if ( state == Qt::Checked )
        winedoc_button->setEnabled(false);
    else
        winedoc_button->setEnabled(true);
}


void ConfigDialog::findWineFolder() {
    QString winedocdir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), winedoc_input->text(),
                         QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    qDebug() << "choosen dir:" << winedocdir;
    winedoc_input->setText(
        winedocdir
    );
    QString logdir = winedocdir;
    logdir.append("/EVE/logs");
    if ( !QDir(logdir).isReadable() )
        winedoc_warning->setText("<p style=\"color:red;\"><b>EVE Log directory not found</b></p>");
    else
        winedoc_warning->setText("");
}

void ConfigDialog::accept()
{
    QSettings settings;
    settings.beginGroup("audio");

    if ( useInternal->checkState() == Qt::Checked ) {
        if ( settings.contains("file") )
            settings.remove("file");
    }
    else if (fileinput->text().size() > 0 && settings.value("file","").toString() != fileinput->text())
        settings.setValue("file",fileinput->text());

    if ( useInternal_hd->checkState() == Qt::Checked ) {
        if ( settings.contains("hotdropper") )
            settings.remove("hotdropper");
    }
    else if (fileinput_hd->text().size() > 0 && settings.value("hotdropper","").toString() != fileinput_hd->text())
        settings.setValue("hotdropper",fileinput_hd->text());

    if ( output_device->itemData(output_device->currentIndex()) != oldDeviceIndex )
        settings.setValue("default_device",output_device->itemData(output_device->currentIndex()));

    if ( volSlider->value() != oldVolume*100 ) {
        qreal vol =  (qreal)volSlider->value()/100;
        settings.setValue("volume",vol);
    }
    settings.endGroup();

#ifdef Q_WS_X11
    settings.beginGroup("wine");
    if ( customWineDoc->checkState() == Qt::Checked ) {
        if ( settings.contains("docpath") )
            settings.remove("docpath");
    } else if ( winedoc_input->text().size() > 0 && settings.value("docpath","").toString() != winedoc_input->text())
        settings.setValue("docpath",winedoc_input->text());
    settings.endGroup();
#endif
    QDialog::accept();
}

void ConfigDialog::reject()
{
    QSettings settings;
    audio->setOutputDeviceIndex(oldDeviceIndex);
    audio->setVolume(oldVolume);
    QDialog::reject();
}

void ConfigDialog::setVolumeText(int volume)
{
    volSlider_label->setText(QString("%1 %").arg(volume));
}


#include "configdialog.moc"
