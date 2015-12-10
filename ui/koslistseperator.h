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

#ifndef KOSLISTSEPERATOR_H
#define KOSLISTSEPERATOR_H

#include <QtGui/QWidget>


class QFrame;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class QSpacerItem;
class QVBoxLayout;

namespace pkos {

class KOSListSeperator : public QWidget
{
    Q_OBJECT

public:
    KOSListSeperator(QWidget* parent);


    const QSize sizeHint();
    const QSize minimumSizeHint();
    const QSize maximumSizeHint();

    void setPilot(const QString& pilot);
private:
    QGridLayout* cLayout;
    QVBoxLayout* mLayout;
    QHBoxLayout* tLayout;

    QLabel*      pilot;
    QSpacerItem  *sp1,*sp2;

    QFrame*      hline;


};

}
#endif // KOSLISTSEPERATOR_H
