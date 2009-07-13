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

#include <qmediasource.h>

#include "qt7widget.h"
#include "qt7movie.h"
#import <CoreFoundation/CoreFoundation.h>
#import <QTKit/QTMovie.h>


class Qt7MoviePrivate
{
public:
    QTMovie*    movie;
};


Qt7Movie::Qt7Movie(QObject *parent):
    QObject(parent),
    d(new Qt7MoviePrivate)
{
    d->movie = nil;
}

Qt7Movie::~Qt7Movie()
{
    [d->movie release];
}

void Qt7Movie::setSource(QMediaSource const &source)
{
    if (d->movie != nil)
        [d->movie release];

    NSURL* url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:source.dataLocation().toString().toUtf8()]];
    d->movie = [[QTMovie movieWithURL:url error:nil] retain];
}

qint64 Qt7Movie::duration() const
{
    QTTime  t = [d->movie duration];

    return 0;
}

qint64 Qt7Movie::position() const
{
    QTTime t = [d->movie currentTime];
    return 0;
}

void Qt7Movie::setPosition(qint64 position)
{
}

int Qt7Movie::volume() const
{
    return [d->movie volume] * 100;
}

void Qt7Movie::setVolume(int v)
{
}

bool Qt7Movie::isMuted() const
{
    return [d->movie muted] == TRUE;
}

void Qt7Movie::setMuted(bool muted)
{
}

bool Qt7Movie::isVideoAvailable() const
{
    return false;
}

bool Qt7Movie::isSeekable() const
{
    return false;
}

void Qt7Movie::play()
{
    [d->movie play];
}

void Qt7Movie::pause()
{
}

void Qt7Movie::stop()
{
    [d->movie stop];
}

void Qt7Movie::setVideoOutput(Qt7Widget *outpout)
{
}

