/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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

#include <multimedia/qmediaslideshow.h>

#include <multimedia/qabstractmediaobject_p.h>
#include <multimedia/qmediaslideshowservice_p.h>

#include <multimedia/qmediaplaylist.h>
#include <multimedia/qmediasource.h>
#include <multimedia/qmediaresource.h>

#include <QtCore/qcoreevent.h>
#include <QtCore/qtextstream.h>

class QMediaSlideShowPrivate : public QAbstractMediaObjectPrivate
{
    Q_DECLARE_PUBLIC(QMediaSlideShow)
public:
    QMediaSlideShowPrivate()
        : service(0)
        , slideControl(0)
        , playlist(0)
        , state(QMediaSlideShow::StoppedState)
        , timeout(3000)
    {
    }

    void _q_playlistMediaChanged(const QMediaSource &source);
    void _q_playlistDestroyed(QObject *playlist);

    QMediaSlideShowService *service;
    QMediaSlideShowControl *slideControl;
    QMediaPlaylist *playlist;
    QMediaSlideShow::State state;
    int timeout;
    QBasicTimer timer;
    QMediaSource media;
};

void QMediaSlideShowPrivate::_q_playlistMediaChanged(const QMediaSource &source)
{
    media = source;

    slideControl->setMedia(media);

    timer.start(timeout, q_func());
}

void QMediaSlideShowPrivate::_q_playlistDestroyed(QObject *object)
{
    if (object == playlist) {
        playlist = 0;
        timer.stop();

        if (state != QMediaSlideShow::StoppedState)
            emit q_func()->stateChanged(state = QMediaSlideShow::StoppedState);
    }
}

/*!
    \class QMediaSlideShow
    \brief The QMediaSlideShow class presents a slide show of images from a playlist.
    \preliminary

    A slide show playlist may be composed of a variety of media.  If a playlist item itself isn't
    an image a representive image may be displayed instead, for music this may be the album cover
    art or for video a poster image.
*/

/*!
    \enum QMediaSlideShow::State

    \value StoppedState The slide show is not progressing, and has been reset.
    \value PlayingState The slide show is progressing.
    \value PausedState The slide show is not progressing.
*/

/*!
    \enum QMediaSlideShow::MediaStatus

    \value NoMedia  There is no current media.
    \value LoadingMedia The slide show is loading the current media.
    \value LoadedMedia The slide show has loaded the current media.
    \value InvalidMedia The current media cannot be loaded.
*/

/*!
    Constructs a new slide show with the given \a parent.
*/
QMediaSlideShow::QMediaSlideShow(QObject *parent)
    : QAbstractMediaObject(*new QMediaSlideShowPrivate, parent)
{
    Q_D(QMediaSlideShow);

    d->service = new QMediaSlideShowService;

    d->slideControl = qobject_cast<QMediaSlideShowControl*>(
            d->service->control(QMediaSlideShowControl_iid));
}

/*!
    Destroys a slide show.
*/
QMediaSlideShow::~QMediaSlideShow()
{
    Q_D(QMediaSlideShow);

    delete d->service;
}

/*!
    \reimp
*/
QAbstractMediaService *QMediaSlideShow::service() const
{
    return d_func()->service;
}

/*!
    \reimp
*/
bool QMediaSlideShow::isValid() const
{
    return d_func()->service != 0;
}

/*!
    \property QMediaSlideShow::state
    \brief the playback state of a slide show.
*/

QMediaSlideShow::State QMediaSlideShow::state() const
{
    return d_func()->state;
}

/*!
    \fn QMediaSlideShow::stateChanged(QMediaSlideShow::State state)

    Signals that the playback \a state of a slide show has changed.
*/

/*!
    \property QMediaSlideShow::mediaStatus
    \brief the status of the current media.
*/

QMediaSlideShow::MediaStatus QMediaSlideShow::mediaStatus() const
{
    Q_D(const QMediaSlideShow);

    return d->slideControl
            ? d->slideControl->mediaStatus()
            : NoMedia;
}

/*!
    \fn QMediaSlideShow::mediaStatusChanged(QMediaSlideShow::MediaStatus status)

    Signals the the \a status of the current media has changed.
*/

/*!
    \property QMediaSlideShow::media
    \brief the media a slide show is presenting.

    This is the media used to initially populate the playlist, and may not be representive of the
    currently displayed media.

    \sa currentMedia
*/

QMediaSource QMediaSlideShow::media() const
{
    Q_D(const QMediaSlideShow);

    return d->media;
}

void QMediaSlideShow::setMedia(const QMediaSource &media)
{
    Q_D(QMediaSlideShow);

    d->media = media;
    d->timer.stop();

    if (d->state != QMediaSlideShow::StoppedState)
        emit stateChanged(d->state = QMediaSlideShow::StoppedState);

    emit mediaChanged(d->media);
}

/*!
    \fn QMediaSlideShow::mediaChanged(const QMediaSource &media)

    Signals that the \a media a slide show is presenting.
*/

/*!
    \property QMediaSlideShow::currentMedia
    \brief the actual media resource the slide show is currently displaying.
*/

QMediaResource QMediaSlideShow::currentMedia() const
{
    Q_D(const QMediaSlideShow);

    return d->slideControl
            ? d->slideControl->currentMedia()
            : QMediaResource();
}

/*!
    \fn QMediaSlideShow::currentMediaChanged(const QMediaResource &media)

    Signals that the \a media resource a slide show is presenting has changed.
*/

/*!
    \property QMediaSlideShow::timeout
    \brief the amount of time an image is displayed for before moving to the next image.
*/

int QMediaSlideShow::timeout() const
{
    return d_func()->timeout;
}

void QMediaSlideShow::setTimeout(int timeout)
{
    Q_D(QMediaSlideShow);

    d->timeout = timeout;
}

/*!
    \reimp
*/
void QMediaSlideShow::bind(QObject *object)
{
    Q_D(QMediaSlideShow);

    if (QMediaPlaylist *playlist = qobject_cast<QMediaPlaylist *>(object)) {
        if (d->playlist) {
            qWarning("QMediaSlideShow::bind(): already bound to a playlist");
        } else {
            d->playlist = playlist;

            connect(d->playlist, SIGNAL(currentMediaChanged(QMediaSource)),
                    this, SLOT(_q_playlistMediaChanged(QMediaSource)));
            connect(d->playlist, SIGNAL(destroyed(QObject*)),
                    this, SLOT(_q_playlistDestroyed(QObject *)));
        }
    }
}

/*!
    Starts a slide show.

    If there is no current index set this will start at the beginning of the playlist, otherwise it
    will resume from the current index.
*/
void QMediaSlideShow::play()
{
    Q_D(QMediaSlideShow);

    if (d->playlist && d->playlist->size() > 0 && d->state != PlayingState) {
        bool advance = d->state == StoppedState && d->playlist->currentPosition() < 0;

        if (d->timeout >= 0)
            d->timer.start(d->timeout, this);

        emit stateChanged(d->state = PlayingState);

        if (advance)
            d->playlist->advance();
    }
}

/*!
    Pauses a slide show.

    The current index is retained.
*/
void QMediaSlideShow::pause()
{
    Q_D(QMediaSlideShow);

    if (d->state == PlayingState) {
        d->timer.stop();

        emit stateChanged(d->state = PausedState);
    }
}

/*!
    Stops a slide show.

    This will reset the current index and media to null states.
*/
void QMediaSlideShow::stop()
{
    Q_D(QMediaSlideShow);

    switch (d->state) {
    case PlayingState:
        d->timer.stop();
        // fall through.
    case PausedState:
        d->state = QMediaSlideShow::StoppedState;

        if (d->playlist)
            d->playlist->setCurrentPosition(-1);

        emit stateChanged(d->state);
        break;
    case StoppedState:
        break;
    }
}

/*!
    \reimp
*/
void QMediaSlideShow::timerEvent(QTimerEvent *event)
{
    Q_D(QMediaSlideShow);

    if (event->timerId() == d->timer.timerId()) {
        Q_ASSERT(d->playlist);

        d->playlist->advance();

        if (d->playlist->currentPosition() < 0) {
            d->timer.stop();

            emit stateChanged(d->state = StoppedState);
        }
    } else {
        QAbstractMediaObject::timerEvent(event);
    }
}

#include "moc_qmediaslideshow.cpp"
