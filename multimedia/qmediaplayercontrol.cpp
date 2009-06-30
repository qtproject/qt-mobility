/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmediaplayercontrol.h"
#include "qmediasource.h"

Q_DECLARE_METATYPE(QMediaPlaylist*)

QMediaPlayerControl::~QMediaPlayerControl()
{
}

int QMediaPlayerControl::state() const
{
    return property("state").toInt();
}

void QMediaPlayerControl::setState(int state)
{
    changePropertyValue("state", state);
}

QMediaPlaylist* QMediaPlayerControl::mediaPlaylist() const
{
    return property("mediaPlaylist").value<QMediaPlaylist*>();
}

void QMediaPlayerControl::setMediaPlaylist(QMediaPlaylist *mediaPlaylist)
{
    changePropertyValue("mediaPlaylist", qVariantFromValue(mediaPlaylist));
}

qint64 QMediaPlayerControl::duration() const
{
    return property("duration").toLongLong();
}

void QMediaPlayerControl::setDuration(qint64 duration)
{
    changePropertyValue("duration", duration);
}

qint64 QMediaPlayerControl::position() const
{
    return property("position").toLongLong();
}

void QMediaPlayerControl::setPosition(qint64 position)
{
    changePropertyValue("position", position);
}

int QMediaPlayerControl::volume() const
{
    return property("volume").toInt();
}

void QMediaPlayerControl::setVolume(int volume)
{
    changePropertyValue("volume", volume);
}

bool QMediaPlayerControl::isMuted() const
{
    return property("muted").toBool();
}

void QMediaPlayerControl::setMuted(bool muted)
{
    changePropertyValue("muted", muted);
}

bool QMediaPlayerControl::isBuffering() const
{
    return property("buffering").toBool();
}

void QMediaPlayerControl::setBuffering(bool buffering)
{
    changePropertyValue("buffering", buffering);
}

int QMediaPlayerControl::bufferStatus() const
{
    return property("bufferStatus").toInt();
}

void QMediaPlayerControl::setBufferState(int bufferStatus)
{
    changePropertyValue("bufferStatus", bufferStatus);
}

bool QMediaPlayerControl::isVideoAvailable() const
{
    return property("videoAvailable").toBool();
}

void QMediaPlayerControl::setVideoAvailable(bool videoAvailable)
{
    changePropertyValue("videoAvailable", videoAvailable);
}


QMediaPlayerControl::QMediaPlayerControl(QObject *parent):
    QAbstractMediaControl(parent)
{
    addPropertyWatch("position");
}
