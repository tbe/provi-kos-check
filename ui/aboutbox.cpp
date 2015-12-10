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

#include "aboutbox.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtCore/QDirIterator>

#include "../pkosversion.h"

using namespace pkos;
AboutBox::AboutBox(QWidget* parent):QDialog(parent)
{
    grid = new QGridLayout(this);
    content = new QLabel(this);
    content->setTextFormat(Qt::RichText);
    content->setOpenExternalLinks(true);
    grid->addWidget(content,0,0,1,1);
    QFile file(":/about/about.html");
    QString VERSION("%1.%2.%3");
    if (file.open(QFile::ReadOnly)) {
        QTextStream in(&file);
        QString data(in.readAll());
        data.replace("$VERSION",VERSION.arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_PATCH));
        content->setText(data);
    }
}

#include "aboutbox.moc"
