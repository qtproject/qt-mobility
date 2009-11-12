/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "streamplayer.h"

#include <qmediaplayer.h>

#include <QtGui>

StreamPlayer::StreamPlayer(QWidget *parent)
    : QWidget(parent)
    , player(0)
    , progress(0)
{
    player = new QMediaPlayer;
    connect(player, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));

    progress = new QProgressBar;
    progress->setRange(0, 0);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(progress);

    setLayout(layout);
}

StreamPlayer::~StreamPlayer()
{
    delete player;
}

void StreamPlayer::setFileName(const QString &fileName)
{
    file.setFileName(fileName);

    if (file.open(QIODevice::ReadOnly)) {
        player->setMedia(QMediaContent(), &file);
    }
}

void StreamPlayer::play()
{
    player->play();
}

void StreamPlayer::durationChanged(qint64 duration)
{
    progress->setMaximum(duration / 1000);
}

void StreamPlayer::positionChanged(qint64 position)
{
    progress->setValue(position / 1000);
}

void StreamPlayer::metaDataChanged()
{
    setWindowTitle(player->metaData(QtMedia::Title).toString());
}
