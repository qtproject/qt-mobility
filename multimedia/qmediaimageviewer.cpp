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

#include <multimedia/qmediaimageviewer.h>

#include <multimedia/qmediaobject_p.h>
#include <multimedia/qmediaimageviewerservice_p.h>

#include <multimedia/qmediaplaylist.h>
#include <multimedia/qmediacontent.h>
#include <multimedia/qmediaresource.h>

#include <QtCore/qcoreevent.h>
#include <QtCore/qtextstream.h>

class QMediaImageViewerPrivate : public QMediaObjectPrivate
{
    Q_DECLARE_PUBLIC(QMediaImageViewer)
public:
    QMediaImageViewerPrivate():
        viewerControl(0), playlist(0),
        state(QMediaImageViewer::StoppedState), timeout(3000)
    {
    }

    void _q_playlistMediaChanged(const QMediaContent &content);
    void _q_playlistDestroyed(QObject *playlist);

    QMediaImageViewerControl *viewerControl;
    QMediaPlaylist *playlist;
    QMediaImageViewer::State state;
    int timeout;
    QBasicTimer timer;
    QMediaContent media;
};

void QMediaImageViewerPrivate::_q_playlistMediaChanged(const QMediaContent &content)
{
    media = content;

    viewerControl->showMedia(media);

    emit q_func()->mediaChanged(media);
}

void QMediaImageViewerPrivate::_q_playlistDestroyed(QObject *object)
{
    if (object == playlist) {
        playlist = 0;
        timer.stop();

        if (state != QMediaImageViewer::StoppedState)
            emit q_func()->stateChanged(state = QMediaImageViewer::StoppedState);
    }
}

/*!
    \class QMediaImageViewer
    \brief The QMediaImageViewer class presents a slide show of images from a playlist.
    \preliminary

    A slide show playlist may be composed of a variety of media.  If a playlist item itself isn't
    an image a representive image may be displayed instead, for music this may be the album cover
    art or for video a poster image.
*/

/*!
    \enum QMediaImageViewer::State

    \value StoppedState The slide show is not progressing, and has been reset.
    \value PlayingState The slide show is progressing.
    \value PausedState The slide show is not progressing.
*/

/*!
    \enum QMediaImageViewer::MediaStatus

    \value NoMedia  There is no current media.
    \value LoadingMedia The slide show is loading the current media.
    \value LoadedMedia The slide show has loaded the current media.
    \value InvalidMedia The current media cannot be loaded.
*/

/*!
    Constructs a new slide show with the given \a parent.
*/
QMediaImageViewer::QMediaImageViewer(QObject *parent)
    : QMediaObject(*new QMediaImageViewerPrivate, parent, new QMediaImageViewerService)
{
    Q_D(QMediaImageViewer);

    d->viewerControl = qobject_cast<QMediaImageViewerControl*>(
            d->service->control(QMediaImageViewerControl_iid));

    connect(d->viewerControl, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            this, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)));
    connect(d->viewerControl, SIGNAL(currentMediaChanged(QMediaResource)),
            this, SIGNAL(currentMediaChanged(QMediaResource)));
}

/*!
    Destroys a slide show.
*/
QMediaImageViewer::~QMediaImageViewer()
{
    Q_D(QMediaImageViewer);

    delete d->service;
}

/*!
    \property QMediaImageViewer::state
    \brief the playback state of a slide show.
*/

QMediaImageViewer::State QMediaImageViewer::state() const
{
    return d_func()->state;
}

/*!
    \fn QMediaImageViewer::stateChanged(QMediaImageViewer::State state)

    Signals that the playback \a state of a slide show has changed.
*/

/*!
    \property QMediaImageViewer::mediaStatus
    \brief the status of the current media.
*/

QMediaImageViewer::MediaStatus QMediaImageViewer::mediaStatus() const
{
    return d_func()->viewerControl->mediaStatus();
}

/*!
    \fn QMediaImageViewer::mediaStatusChanged(QMediaImageViewer::MediaStatus status)

    Signals the the \a status of the current media has changed.
*/

/*!
    \property QMediaImageViewer::media
    \brief the media a slide show is presenting.

    This is the media used to initially populate the playlist, and may not be representive of the
    currently displayed media.

    \sa currentMedia
*/

QMediaContent QMediaImageViewer::media() const
{
    Q_D(const QMediaImageViewer);

    return d->media;
}

void QMediaImageViewer::setMedia(const QMediaContent &media)
{
    Q_D(QMediaImageViewer);

    d->media = media;
    d->timer.stop();

    if (d->state != QMediaImageViewer::StoppedState)
        emit stateChanged(d->state = QMediaImageViewer::StoppedState);

    d->viewerControl->showMedia(d->media);

    emit mediaChanged(d->media);
}

/*!
    \fn QMediaImageViewer::mediaChanged(const QMediaContent &media)

    Signals that the \a media a slide show is presenting.
*/

/*!
    \property QMediaImageViewer::currentMedia
    \brief the actual media resource the slide show is currently displaying.
*/

QMediaResource QMediaImageViewer::currentMedia() const
{
    return d_func()->viewerControl->currentMedia();
}

/*!
    \fn QMediaImageViewer::currentMediaChanged(const QMediaResource &media)

    Signals that the \a media resource a slide show is presenting has changed.
*/

/*!
    \property QMediaImageViewer::timeout
    \brief the amount of time an image is displayed for before moving to the next image.
*/

int QMediaImageViewer::timeout() const
{
    return d_func()->timeout;
}

void QMediaImageViewer::setTimeout(int timeout)
{
    Q_D(QMediaImageViewer);

    d->timeout = qMax(0, timeout);
}

/*!
    \internal
*/
void QMediaImageViewer::bind(QObject *object)
{
    Q_D(QMediaImageViewer);

    if (QMediaPlaylist *playlist = qobject_cast<QMediaPlaylist *>(object)) {
        if (d->playlist) {
            qWarning("QMediaImageViewer::bind(): already bound to a playlist");
        } else {
            d->playlist = playlist;

            connect(d->playlist, SIGNAL(currentMediaChanged(QMediaContent)),
                    this, SLOT(_q_playlistMediaChanged(QMediaContent)));
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
void QMediaImageViewer::play()
{
    Q_D(QMediaImageViewer);

    if (d->playlist && d->playlist->size() > 0 && d->state != PlayingState) {
        if (d->timeout >= 0)
            d->timer.start(d->timeout, this);

        d->state = PlayingState;
        d->playlist->next();

        if (d->state == QMediaImageViewer::PlayingState)
            emit stateChanged(d->state);
    }
}

/*!
    Pauses a slide show.

    The current index is retained.
*/
void QMediaImageViewer::pause()
{
    Q_D(QMediaImageViewer);

    if (d->state == PlayingState) {
        d->timer.stop();

        emit stateChanged(d->state = PausedState);
    }
}

/*!
    Stops a slide show.

    This will reset the current index and media to null states.
*/
void QMediaImageViewer::stop()
{
    Q_D(QMediaImageViewer);

    switch (d->state) {
    case PlayingState:
        d->timer.stop();
        // fall through.
    case PausedState:
        d->state = QMediaImageViewer::StoppedState;

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
void QMediaImageViewer::timerEvent(QTimerEvent *event)
{
    Q_D(QMediaImageViewer);

    if (event->timerId() == d->timer.timerId()) {
        Q_ASSERT(d->playlist);

        d->playlist->next();

        if (d->playlist->currentPosition() < 0) {
            d->timer.stop();

            emit stateChanged(d->state = StoppedState);
        }
    } else {
        QMediaObject::timerEvent(event);
    }
}

#include "moc_qmediaimageviewer.cpp"
