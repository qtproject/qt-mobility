/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qurl.h>
#include <qmediaresource.h>

#include "qt7widget.h"
#include "qt7movie.h"

#import <CoreFoundation/CoreFoundation.h>
#import <QTKit/QTMovie.h>


class Qt7MoviePrivate
{
public:
    QTMovie*    movie;
    Qt7Widget*  widget;
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

qint64 Qt7Movie::duration() const
{
    QTTime  t = [d->movie duration];

    if (t.timeScale == 0)
        return t.timeValue * 1000;
    return t.timeValue * 1000 / t.timeScale;
}

qint64 Qt7Movie::position() const
{
    QTTime t = [d->movie currentTime];
    return t.timeValue * 1000 / t.timeScale;
}

void Qt7Movie::setPosition(qint64 position)
{
    [d->movie setCurrentTime:QTMakeTime(position, 1000)];
}

int Qt7Movie::volume() const
{
    return [d->movie volume] * 100;
}

void Qt7Movie::setVolume(int v)
{
    [d->movie setVolume:float(v) / 100];
}

bool Qt7Movie::isMuted() const
{
    return [d->movie muted] == TRUE;
}

void Qt7Movie::setMuted(bool muted)
{
    [d->movie setMuted:BOOL(muted)];
}

float Qt7Movie::rate() const
{
    return [d->movie rate];
}

void Qt7Movie::setRate(float r)
{
    [d->movie setRate:r];
}

bool Qt7Movie::isVideoAvailable() const
{
    NSDictionary *attributes = [d->movie movieAttributes];

    return [(NSNumber*)[attributes objectForKey:QTMovieHasVideoAttribute] boolValue];
}

bool Qt7Movie::isSeekable() const
{
    return false;
}

void Qt7Movie::play()
{
    [d->movie play];
    d->widget->setMovie(d->movie);
}

void Qt7Movie::pause()
{
    [d->movie setRate:0];
}

void Qt7Movie::stop()
{
    [d->movie stop];
}

int Qt7Movie::state() const
{
    if (d->movie == nil)
        return 3;

    if (IsMovieDone([d->movie quickTimeMovie]))
        return 3;
    else if (rate() == 0)
        return 2;

    return 1;
}

void Qt7Movie::setVideoWidget(Qt7Widget *video)
{
    d->widget = video;
}

void Qt7Movie::setSource(QMediaSource const &source)
{
    if (d->movie != nil)
        [d->movie release];

    QUrl qurl = source.contentUri();
    d->movie = [[QTMovie movieWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:qurl.toEncoded().constData()]]
                                error:nil] retain];
}

