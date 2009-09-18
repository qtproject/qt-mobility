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

#include "qmediaslideshow.h"

#include "qabstractmediaobject_p.h"
#include "qmediaslideshowservice_p.h"

#include "qlocalmediaplaylistprovider.h"
#include "qmediasource.h"
#include "qmediaresource.h"

#include <QtCore/qcoreevent.h>
#include <QtCore/qtextstream.h>
#include <QtNetwork/qnetworkreply.h>

class QMediaSlideShowPrivate : public QAbstractMediaObjectPrivate
{
    Q_DECLARE_PUBLIC(QMediaSlideShow)
public:
    QMediaSlideShowPrivate()
        : service(0)
        , slideControl(0)
        , playlist(0)
        , network(0)
        , headReply(0)
        , getReply(0)
        , state(QMediaSlideShow::StoppedState)
        , currentIndex(-1)
        , timeout(3000)
    {
    }

    bool isPlaylistType(const QUrl &url, const QString &mimeType) const;

    void requestHeaders(const QUrl &uri);
    void requestPlaylist(const QUrl &uri);
    void cancelRequests();

    void _q_headFinished();
    void _q_getFinished();

    QMediaSlideShowService *service;
    QMediaSlideShowControl *slideControl;
    QMediaPlaylistProvider *playlist;
    QNetworkAccessManager *network;
    QNetworkReply *headReply;
    QNetworkReply *getReply;
    QMediaSlideShow::State state;
    int currentIndex;
    int timeout;
    QBasicTimer timer;
    QMediaSource media;
};

bool QMediaSlideShowPrivate::isPlaylistType(const QUrl &url, const QString &mimeType) const
{
    if (!mimeType.isEmpty()) {
        return mimeType == QLatin1String("audio/x-mpegurl")
                || mimeType == QLatin1String("audio/mpeg-url")
                || mimeType == QLatin1String("application/x-winamp-playlist")
                || mimeType == QLatin1String("audio/scpls")
                || mimeType == QLatin1String("audio/x-scpls")
                || mimeType == QLatin1String("video/x-ms-asf")
                || mimeType == QLatin1String("application/xsfp+xml");
    } else if (url.scheme() == QLatin1String("file")) {
        QString path = url.path();

        return path.endsWith(QLatin1String(".m3u"), Qt::CaseInsensitive)
                || path.endsWith(QLatin1String(".pls"), Qt::CaseInsensitive)
                || path.endsWith(QLatin1String(".asx"), Qt::CaseInsensitive);
    } else {
        return false;
    }
}

void QMediaSlideShowPrivate::requestHeaders(const QUrl &uri)
{
    Q_ASSERT(!headReply);

    headReply = network->head(QNetworkRequest(uri));

    QObject::connect(headReply, SIGNAL(readChannelFinished()), q_func(), SLOT(_q_headFinished()));
}

void QMediaSlideShowPrivate::requestPlaylist(const QUrl &uri)
{
    Q_ASSERT(!getReply);

    getReply = network->get(QNetworkRequest(uri));

    QObject::connect(getReply, SIGNAL(readChannelFinished()), q_func(), SLOT(_q_getFinished()));
}

void QMediaSlideShowPrivate::cancelRequests()
{
    if (getReply) {
        getReply->abort();
        getReply->deleteLater();
        getReply = 0;
    }

    if (headReply) {
        headReply->abort();
        headReply->deleteLater();
        headReply = 0;
    }
}

void QMediaSlideShowPrivate::_q_headFinished()
{
    QString mimeType = headReply->header(QNetworkRequest::ContentTypeHeader).toString();
    QUrl uri = headReply->url();

    headReply->deleteLater();
    headReply = 0;

    if (!mimeType.isEmpty()) {
        mimeType = mimeType.section(QLatin1Char(';'), 0, 0);

        if (isPlaylistType(uri, mimeType)) {
            getReply = network->get(QNetworkRequest(uri));

            QObject::connect(
                    getReply, SIGNAL(readChannelFinished()), q_func(), SLOT(_q_getFinished()));
        } else {
            playlist->appendItem(media);
            currentIndex = 0;
            slideControl->setMedia(media);

            emit q_func()->currentIndexChanged(currentIndex);
        }
    }
}

void QMediaSlideShowPrivate::_q_getFinished()
{
    QTextStream stream(getReply);

    while (!stream.atEnd()) {
        QString line = stream.readLine();

        if (!line.startsWith(QLatin1Char('#'))) {
            QUrl url(line);
            if (url.isValid())
                playlist->appendItem(QMediaSource(url));
        }
    }

    getReply->deleteLater();
    getReply = 0;
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
    : QAbstractMediaObject(*new QMediaSlideShowPrivate, parent, new QMediaSlideShowService)
{
    Q_D(QMediaSlideShow);

    d->network = new QNetworkAccessManager(this);
    d->playlist = new QLocalMediaPlaylistProvider;

    d->service->setNetworkManager(d->network);

    d->slideControl = qobject_cast<QMediaSlideShowControl*>(d->service->control(QMediaSlideShowControl_iid));
}

/*!
    Destroys a slide show.
*/
QMediaSlideShow::~QMediaSlideShow()
{
    Q_D(QMediaSlideShow);

    d->cancelRequests();

    delete d->service;
    delete d->network;
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

    d->cancelRequests();

    d->media = media;

    d->currentIndex = -1;
    d->playlist->clear();

    if (d->media.isNull()) {
        d->slideControl->setMedia(d->media);

        emit currentIndexChanged(-1);
    } else {
        QUrl uri = d->media.contentUri();
        QString mimeType = d->media.contentResource().mimeType();

        if (d->isPlaylistType(uri, mimeType)) {
            d->requestPlaylist(uri);
        } else if (mimeType.isEmpty() && uri.scheme() != QLatin1String("file")) {
            d->requestHeaders(uri);
        } else {
            d->playlist->appendItem(media);
            d->currentIndex = 0;
            d->slideControl->setMedia(d->media);
        }
    }
    emit currentIndexChanged(d->currentIndex);
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
    \property QMediaSlideShow::currentIndex
    \brief the index in the playlist of the current media.
*/

int QMediaSlideShow::currentIndex() const
{
    return d_func()->currentIndex;
}

void QMediaSlideShow::setCurrentIndex(int index)
{
    Q_D(QMediaSlideShow);

    if (d->playlist->size() > 0) {
        if (index >= 0 && index < d->playlist->size()) {
            d->currentIndex = index;

            d->slideControl->setMedia(d->playlist->media(d->currentIndex));
        } else {
            d->currentIndex = -1;
            d->slideControl->setMedia(QMediaSource());

        }

        emit currentIndexChanged(d->currentIndex);
    }
}

/*!
    \fn QMediaSlideShow::currentIndexChanged(int index)

    Signals that the \a index in the playlist of the current media has changed.
*/

/*!
    \property QMediaSlideShow::playlist
    \brief a playlist containing the media displayed in a slide show.
*/

QMediaPlaylistProvider *QMediaSlideShow::playlist() const
{
    return d_func()->playlist;
}

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
    Starts a slide show.

    If there is no current index set this will start at the beginning of the playlist, otherwise it
    will resume from the current index.
*/
void QMediaSlideShow::play()
{
    Q_D(QMediaSlideShow);

    switch (d->state) {
    case StoppedState:
        if (d->currentIndex < 0 || d->currentIndex >= d->playlist->size()) {
            d->currentIndex = 0;
            d->slideControl->setMedia(d->playlist->media(d->currentIndex));
        }
        // fall through.
    case PausedState:
        if (d->timeout < 0) {
            qWarning("negative slide show timeout");
        } else if (d->playlist->size() > 0) {
            d->timer.start(d->timeout, this);

            emit stateChanged(d->state = PlayingState);
        }
        break;
    case PlayingState:
        break;
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
    case QMediaSlideShow::PlayingState:
        d->timer.stop();
        // fall through.
    case QMediaSlideShow::PausedState:
        d->state = QMediaSlideShow::StoppedState;

        d->currentIndex = -1;
        d->slideControl->setMedia(QMediaSource());

        emit stateChanged(d->state);
        break;
    case QMediaSlideShow::StoppedState:
        break;
    }
}

/*!
    Moves the slide show to the next image.

    If there is no current index this will display the first item in the playlist.
*/
void QMediaSlideShow::next()
{
    Q_D(QMediaSlideShow);

    setCurrentIndex(d->currentIndex + 1);
}

/*!
    Moves the sldie show to the previous image.

    If there is no current index this will display the last item in the playlist.
*/
void QMediaSlideShow::previous()
{
    Q_D(QMediaSlideShow);

    setCurrentIndex(d->currentIndex - 1);
}

/*!
    \reimp
*/
void QMediaSlideShow::timerEvent(QTimerEvent *event)
{
    Q_D(QMediaSlideShow);

    if (event->timerId() == d->timer.timerId()) {
        next();
    } else {
        QAbstractMediaObject::timerEvent(event);
    }
}

#include "moc_qmediaslideshow.cpp"
