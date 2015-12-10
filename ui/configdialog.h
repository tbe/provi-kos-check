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

#ifndef PKOS_CONFIGDIALOG_H
#define PKOS_CONFIGDIALOG_H

#include <QtGui/QDialog>
#include <phonon/AudioOutputDevice>
#include <phonon/Path>

class QLabel;
class QSlider;
class QTabWidget;
class QCheckBox;
class QHBoxLayout;
class QLineEdit;
class QFrame;
class QComboBox;
class QSpacerItem;
class QDialogButtonBox;
class QVBoxLayout;


namespace pkos {

class Audio;

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    ConfigDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~ConfigDialog();

private slots:
    void audio_useInternalToogled(int state);
    void findFile();
    void playButtonTriggered();

    void audio_useInternalHDToogled(int state);
    void findFile_hd();
    void playButtonHDTriggered();

    void audioDone();
    void volumeSliderMoved(int newValue);
    void upstreamVolumeChanged(qreal volume);
    void accept();
    void reject();

    void system_wineDefaultsToogled(int state);
    void findWineFolder();

private:
    void getCurrentConfig();
    void setup_audiotab();
    void setup_systemtab();


    QVBoxLayout             *centralLayout;
    QTabWidget              *tabs;
    QWidget                 *audiotab,*systemtab;
    QDialogButtonBox        *buttons;

    // audio tab
    void setVolumeText(int volume);

    QStringList             audioFileExtensions;
    QVBoxLayout             *audiotab_layout;
    QLabel                  *audio_label,*hd_label;
    QCheckBox               *useInternal,*useInternal_hd;
    QHBoxLayout             *fileinput_layout,*fileinput_layout_hd;
    QLineEdit               *fileinput,*fileinput_hd;
    QPushButton             *fileinput_button,*fileinput_button_hd;
    QPushButton             *play_button,*play_button_hd;
    QFrame                  *line1,*line1_hd;
    QLabel                  *output_label;
    QComboBox               *output_device;
    QFrame                  *line2;
    QLabel                  *volume_label;
    QHBoxLayout             *volSlider_layout;
    QLabel                  *volPixmap;
    QSlider                 *volSlider;
    QLabel                  *volSlider_label;
    QSpacerItem             *verticalSpacer;

    // systems tab
    QVBoxLayout             *systemtab_layout;
    QLabel                  *winedoc_label,*winedoc_warning;
    QCheckBox               *customWineDoc;
    QHBoxLayout             *winedoc_layout;
    QLineEdit               *winedoc_input;
    QPushButton             *winedoc_button;
    QSpacerItem             *systemtab_spacer;

    bool                    playing;
    Audio                   *audio;
    int                     oldDeviceIndex;
    qreal                   oldVolume;
    bool                    changingVolume;
};
}

#endif // PKOS_CONFIGDIALOG_H
