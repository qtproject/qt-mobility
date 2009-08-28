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
*/

/*!
    \enum QMediaSlideShow::State

    \value StoppedState
    \value PlayingState
    \value PausedState
*/

/*!
    \enum QMediaSlideShow::MediaStatus

    \value NoMedia
    \value LoadingMedia
    \value LoadedMedia
    \value InvalidMedia
*/

/*!
*/
QMediaSlideShow::QMediaSlideShow(QObject *parent)
    : QAbstractMediaObject(*new QMediaSlideShowPrivate, parent)
{
    Q_D(QMediaSlideShow);

    d->network = new QNetworkAccessManager(this);
    d->playlist = new QLocalMediaPlaylistProvider;

    d->service = new QMediaSlideShowService;
    d->service->setNetworkManager(d->network);

    d->slideControl = d->service->QAbstractMediaService::control<QMediaSlideShowControl *>();
}

/*!
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
*/

QMediaSlideShow::State QMediaSlideShow::state() const
{
    return d_func()->state;
}

/*!
    \fn QMediaSlideShow::stateChanged(QMediaSlideShow::State state);
*/

/*!
    \property QMediaSlideShow::mediaStatus
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
*/

/*!
    \property QMediaSlideShow::media
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
    \fn QMediaSlideShow::mediaChanged(const QMediaSource &media);
*/

/*!
    \property QMediaSlideShow::currentMedia
*/

QMediaResource QMediaSlideShow::currentMedia() const
{
    Q_D(const QMediaSlideShow);

    return d->slideControl
            ? d->slideControl->currentMedia()
            : QMediaResource();
}

/*!
    \fn QMediaSlideShow::currentMediaChanged(const QMediaResource &media);
*/

/*!
    \property QMediaSlideShow::currentIndex
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
    \fn QMediaSlideShow::currentIndexChanged(int index);
*/

/*!
    \property QMediaSlideShow::playlist
*/

QMediaPlaylistProvider *QMediaSlideShow::playlist() const
{
    return d_func()->playlist;
}

/*!
    \property QMediaSlideShow::timeout
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
*/
void QMediaSlideShow::next()
{
    Q_D(QMediaSlideShow);

    setCurrentIndex(d->currentIndex + 1);
}

/*!
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
