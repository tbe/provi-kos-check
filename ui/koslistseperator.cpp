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

#include "koslistseperator.h"
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>


using namespace pkos;

KOSListSeperator::KOSListSeperator(QWidget* parent): QWidget()
{

    this->setMinimumSize(QSize(300, 36));
    this->setMaximumSize(QSize(16777215, 36));

    cLayout = new QGridLayout(this);

    mLayout = new QVBoxLayout();
    cLayout->addLayout(mLayout, 1, 0, 1, 1);

    tLayout = new QHBoxLayout();
    mLayout->addLayout(tLayout);

    sp1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    tLayout->addItem(sp1);

    pilot = new QLabel();
    tLayout->addWidget(pilot);

    sp2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    tLayout->addItem(sp2);

    hline = new QFrame();
    hline->setFrameShape(QFrame::HLine);
    hline->setFrameShadow(QFrame::Sunken);

    mLayout->addWidget(hline);

}

void KOSListSeperator::setPilot(const QString& pilot)
{
    this->pilot->setText(pilot);
}


const QSize KOSListSeperator::sizeHint()
{
    return QSize(300,36);
}

const QSize KOSListSeperator::maximumSizeHint()
{
    return sizeHint();
}

const QSize KOSListSeperator::minimumSizeHint()
{
    return sizeHint();
}
