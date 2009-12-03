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

#include "qmlgraphicsvideo_p.h"

#include <QtCore/qcoreevent.h>
#include <QtMultimedia/qvideosurfaceformat.h>
#include <QtOpenGL/qgl.h>

#include <qmediaplayercontrol.h>
#include <qmediaservice.h>
#include <qmediaserviceprovider.h>
#include <qmetadatacontrol.h>
#include <qmetadatacontrolmetaobject_p.h>
#include <qpaintervideosurface_p.h>
#include <qvideooutputcontrol.h>
#include <qvideorenderercontrol.h>

QML_DEFINE_TYPE(Qt,4,6,Video,QTM_PREPEND_NAMESPACE(QmlGraphicsVideo));

QTM_BEGIN_NAMESPACE

namespace
{
    class QmlGraphicsVideoPlayerControl : public QMediaPlayerControl
    {
    public:
        QmlGraphicsVideoPlayerControl(QObject *parent)
            : QMediaPlayerControl(parent)
        {
        }

        QMediaPlayer::State state() const { return QMediaPlayer::StoppedState; }
        QMediaPlayer::MediaStatus mediaStatus() const { return QMediaPlayer::NoMedia; }

        qint64 duration() const { return 0; }
        qint64 position() const { return 0; }
        void setPosition(qint64) {}
        int volume() const { return 100; }
        void setVolume(int) {}
        bool isMuted() const { return false; }
        void setMuted(bool) {}
        int bufferStatus() const { return 0; }
        bool isVideoAvailable() const { return false; }
        bool isSeekable() const { return false; }
        QPair<qint64, qint64> seekRange() const { return QPair<qint64, qint64>(); }
        qreal playbackRate() const { return 1; }
        void setPlaybackRate(qreal) {}
        QMediaContent media() const { return QMediaContent(); }
        const QIODevice *mediaStream() const { return 0; }
        void setMedia(const QMediaContent &, QIODevice *) {}

        void play() {}
        void pause() {}
        void stop() {}
    };
}

void QmlGraphicsVideo::updateVideoRect(const QRectF &geometry, const QSize &videoSize)
{
    if (m_fillMode == Stretch) {
        m_scaledRect = geometry;
    } else {
        QSizeF size = videoSize;

        if (m_fillMode == PreserveAspectFit)
            size.scale(geometry.size(), Qt::KeepAspectRatio);
        else
            size.scale(geometry.size(), Qt::KeepAspectRatioByExpanding);

        m_scaledRect = QRectF(QPointF(), size);
        m_scaledRect.moveCenter(geometry.center());
    }
}

void QmlGraphicsVideo::_q_stateChanged(QMediaPlayer::State state)
{
    if (State(state) != m_state) {
        if (state == QMediaPlayer::PlayingState)
            m_notifyTimer.start(1000, this);
        else
            m_notifyTimer.stop();

        emit stateChanged(m_state = State(state));
    }
}

void QmlGraphicsVideo::_q_mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (Status(status) != m_status)
        emit statusChanged(m_status = Status(status));
}

void QmlGraphicsVideo::_q_mediaChanged(const QMediaContent &media)
{
    emit sourceChanged(media.canonicalUri());
}

void QmlGraphicsVideo::_q_durationChanged(qint64 duration)
{
    emit durationChanged(duration);
}

void QmlGraphicsVideo::_q_positionChanged(qint64 position)
{
    qint64 duration = m_playerControl->duration();

    if (duration != 0)
        emit positionChanged(qreal(position) / qreal(duration));
    else
        emit positionChanged(0);
}

void QmlGraphicsVideo::_q_volumeChanged(int volume)
{
    emit volumeChanged(qreal(volume) / 100);
}

void QmlGraphicsVideo::_q_error(QMediaPlayer::Error error, const QString &errorString)
{
    m_error = Error(error);
    m_errorString = errorString;

    emit errorChanged(m_error);
    emit errorStringChanged(m_errorString);
}

void QmlGraphicsVideo::_q_metaDataChanged()
{
    m_metaObject->metaDataChanged();
}

void QmlGraphicsVideo::_q_videoSurfaceFormatChanged(const QVideoSurfaceFormat &format)
{
    const QSize implicitSize = format.sizeHint();

    setImplicitWidth(qMax(0, implicitSize.width()));
    setImplicitHeight(qMax(0, implicitSize.height()));

    updateVideoRect(boundingRect(), format.sizeHint());
}

void QmlGraphicsVideo::_q_frameChanged()
{
    if (isObscured()) {
        update();
        m_videoSurface->setReady(true);
    } else {
        update();
    }
}

/*!
    \qmlclass Video QmlGraphicsVideo
    \brief The Video element allows you to add videos to a scene.
    \inherits Item
*/

/*!
    \internal
    \class QmlGraphicsVideo
    \brief The QmlGraphicsVideo class provides a video item that you can add to a QmlView.
*/

QmlGraphicsVideo::QmlGraphicsVideo(QmlGraphicsItem *parent)
    : QmlGraphicsItem(parent)
    , m_mediaProvider(0)
    , m_mediaService(0)
    , m_playerControl(0)
    , m_metaDataControl(0)
    , m_videoOutputControl(0)
    , m_videoRendererControl(0)
    , m_videoSurface(0)
    , m_metaObject(0)
    , m_state(QmlGraphicsVideo::Stopped)
    , m_status(QmlGraphicsVideo::NoMedia)
    , m_error(QmlGraphicsVideo::NoError)
    , m_fillMode(QmlGraphicsVideo::PreserveAspectFit)
    , m_updatePaintDevice(true)
{
    if ((m_mediaProvider = QMediaServiceProvider::defaultServiceProvider())) {
        if ((m_mediaService = m_mediaProvider->requestService(Q_MEDIASERVICE_MEDIAPLAYER))) {
            m_playerControl = qobject_cast<QMediaPlayerControl *>(
                    m_mediaService->control(QMediaPlayerControl_iid));
            m_metaDataControl = qobject_cast<QMetaDataControl *>(
                    m_mediaService->control(QMetaDataControl_iid));
            m_videoOutputControl = qobject_cast<QVideoOutputControl *>(
                    m_mediaService->control(QVideoOutputControl_iid));
            m_videoRendererControl = qobject_cast<QVideoRendererControl *>(
                    m_mediaService->control(QVideoRendererControl_iid));
        }
    }

    if (m_playerControl) {
        if (m_metaDataControl) {
            m_metaObject = new QMetaDataControlMetaObject(m_metaDataControl, this);

            connect(m_metaDataControl, SIGNAL(metaDataChanged()),
                    this, SLOT(_q_metaDataChanged()));
        }
        if (m_videoOutputControl && m_videoRendererControl) {
            m_videoSurface = new QPainterVideoSurface;
            m_videoRendererControl->setSurface(m_videoSurface);
            m_videoOutputControl->setOutput(QVideoOutputControl::RendererOutput);

            connect(m_videoSurface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)),
                    this, SLOT(_q_videoSurfaceFormatChanged(QVideoSurfaceFormat)));
            connect(m_videoSurface, SIGNAL(frameChanged()), this, SLOT(_q_frameChanged()));
        }
        connect(m_playerControl, SIGNAL(mediaChanged(QMediaContent)),
                this, SLOT(_q_mediaChanged(QMediaContent)));
        connect(m_playerControl, SIGNAL(stateChanged(QMediaPlayer::State)),
                this, SLOT(_q_stateChanged(QMediaPlayer::State)));
        connect(m_playerControl, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
                this, SLOT(_q_mediaStatusChanged(QMediaPlayer::MediaStatus)));
        connect(m_playerControl, SIGNAL(durationChanged(qint64)),
                this, SLOT(_q_durationChanged(qint64)));
        connect(m_playerControl, SIGNAL(positionChanged(qint64)),
                this, SLOT(_q_positionChanged(qint64)));
        connect(m_playerControl, SIGNAL(volumeChanged(int)),
                this, SLOT(_q_volumeChanged(int)));
        connect(m_playerControl, SIGNAL(mutedChanged(bool)),
                this, SIGNAL(mutedChanged(bool)));
    } else {
        m_playerControl = new QmlGraphicsVideoPlayerControl(this);
    }
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QmlGraphicsVideo::~QmlGraphicsVideo()
{
    m_notifyTimer.stop();

    if (m_mediaProvider)
        m_mediaProvider->releaseService(m_mediaService);

    delete m_videoSurface;
}

/*!
    \qmlproperty url Video::source
*/

QUrl QmlGraphicsVideo::source() const
{
    return m_playerControl->media().canonicalUri();
}

void QmlGraphicsVideo::setSource(const QUrl &url)
{
    m_playerControl->setMedia(QMediaContent(url), 0);
}

/*!
    \qmlproperty enum Video::state
*/
QmlGraphicsVideo::State QmlGraphicsVideo::state() const
{
    return m_state;
}

void QmlGraphicsVideo::setState(State state)
{
    switch (state) {
    case Stopped:
        m_playerControl->stop();
        break;
    case Playing:
        m_playerControl->play();
        break;
    case Paused:
        m_playerControl->pause();
        break;
    }
}

/*!
    \qmlproperty enum Video::status
*/

QmlGraphicsVideo::Status QmlGraphicsVideo::status() const
{
    return m_status;
}

/*!
    \qmlproperty real Video::duration
*/

qreal QmlGraphicsVideo::duration() const
{
    return m_playerControl->duration();
}

/*!
    \qmlproperty real Video::position
*/
qreal QmlGraphicsVideo::position() const
{
    qint64 duration = m_playerControl->duration();

    if (duration != 0)
        return qreal(m_playerControl->position()) / qreal(duration);
    else
        return 0;
}

void QmlGraphicsVideo::setPosition(qreal position)
{
    qint64 duration = m_playerControl->duration();

    if (duration != 0)
        m_playerControl->setPosition(position * duration);
}

/*!
    \qmlproperty int Video::volume
*/

qreal QmlGraphicsVideo::volume() const
{
    return qreal(m_playerControl->volume()) / 100;
}

void QmlGraphicsVideo::setVolume(qreal volume)
{
    m_playerControl->setVolume(volume * 100);
}

/*!
    \qmlproperty bool Video::muted
*/

bool QmlGraphicsVideo::isMuted() const
{
    return m_playerControl->isMuted();
}

void QmlGraphicsVideo::setMuted(bool muted)
{
    m_playerControl->setMuted(muted);
}

/*!
    \qmlproperty bool Video::videoAvailable
*/

bool QmlGraphicsVideo::isVideoAvailable() const
{
    return m_playerControl->isVideoAvailable();
}

/*!
    \qmlproperty int Video::bufferStatus
*/

int QmlGraphicsVideo::bufferStatus() const
{
    return m_playerControl->bufferStatus();
}


/*!
    \qmlproperty bool Video::seekable
*/

bool QmlGraphicsVideo::isSeekable() const
{
    return m_playerControl->isSeekable();
}

/*!
    \qmlproperty qreal playbackRate
*/

qreal QmlGraphicsVideo::playbackRate() const
{
    return m_playerControl->playbackRate();
}

void QmlGraphicsVideo::setPlaybackRate(qreal rate)
{
    m_playerControl->setPlaybackRate(rate);
}

/*!
    \qmlproperty enum Video::error
*/

QmlGraphicsVideo::Error QmlGraphicsVideo::error() const
{
    return m_error;
}

/*!
    \qmlproperty enum Video::errorString
*/

QString QmlGraphicsVideo::errorString() const
{
    return m_errorString;
}

QmlGraphicsVideo::FillMode QmlGraphicsVideo::fillMode() const
{
    return m_fillMode;
}

void QmlGraphicsVideo::setFillMode(FillMode mode)
{
    m_fillMode = mode;

    if (m_videoSurface)
            updateVideoRect(boundingRect(), m_videoSurface->surfaceFormat().sizeHint());

    update();
}

void QmlGraphicsVideo::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (m_videoSurface && m_videoSurface->isActive()) {
        m_videoSurface->paint(painter, QRect(
                m_scaledRect.x(), m_scaledRect.y(), m_scaledRect.width(), m_scaledRect.height()));
        m_videoSurface->setReady(true);
#ifndef QMLGRAPHICSVIDEO_SHADERS    // Flickers
    }
#else
    } else if (m_updatePaintDevice && (painter->paintEngine()->type() == QPaintEngine::OpenGL
            || painter->paintEngine()->type() == QPaintEngine::OpenGL2)) {
        m_updatePaintDevice = false;

        m_videoSurface->setGLContext(const_cast<QGLContext *>(QGLContext::currentContext()));
        if (m_videoSurface->supportedShaderTypes() & QPainterVideoSurface::GlslShader) {
            m_videoSurface->setShaderType(QPainterVideoSurface::GlslShader);
        } else {
            m_videoSurface->setShaderType(QPainterVideoSurface::FragmentProgramShader);
        }
    }
#endif
}

void QmlGraphicsVideo::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (m_videoSurface)
        updateVideoRect(newGeometry, m_videoSurface->surfaceFormat().sizeHint());

    QmlGraphicsItem::geometryChanged(newGeometry, oldGeometry);
}

void QmlGraphicsVideo::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_notifyTimer.timerId())
        emit positionChanged(position());
    else
        QmlGraphicsItem::timerEvent(event);
}

#include "moc_qmlgraphicsvideo_p.cpp"

QTM_END_NAMESPACE
