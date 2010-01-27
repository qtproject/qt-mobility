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
#include <qpaintervideosurface_p.h>
#include <qvideooutputcontrol.h>
#include <qvideorenderercontrol.h>

// #define QMLGRAPHICSVIDEO_SHADERS

QML_DEFINE_TYPE(Qt,4,6,Video,QTM_PREPEND_NAMESPACE(QmlGraphicsVideo));

QTM_BEGIN_NAMESPACE

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
    , m_videoOutputControl(0)
    , m_videoRendererControl(0)
    , m_videoSurface(0)
    , m_fillMode(QmlGraphicsVideo::PreserveAspectFit)
    , m_updatePaintDevice(true)
{
    setObject(this);

    if (m_mediaService) {
        m_videoOutputControl = qobject_cast<QVideoOutputControl *>(
                m_mediaService->control(QVideoOutputControl_iid));
        m_videoRendererControl = qobject_cast<QVideoRendererControl *>(
                m_mediaService->control(QVideoRendererControl_iid));

        connect(m_playerControl, SIGNAL(videoAvailableChanged(bool)),
                this, SIGNAL(hasVideoChanged()));

        if (m_videoOutputControl && m_videoRendererControl) {
            m_videoSurface = new QPainterVideoSurface;
            m_videoRendererControl->setSurface(m_videoSurface);
            m_videoOutputControl->setOutput(QVideoOutputControl::RendererOutput);

            connect(m_videoSurface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)),
                    this, SLOT(_q_videoSurfaceFormatChanged(QVideoSurfaceFormat)));
            connect(m_videoSurface, SIGNAL(frameChanged()), this, SLOT(_q_frameChanged()));
        }
    }
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QmlGraphicsVideo::~QmlGraphicsVideo()
{
    shutdown();

    delete m_videoSurface;
}

/*!
    \qmlproperty url Video::source

    This property holds the source URL of the video.
*/

/*!
    \qmlproperty bool Video::playing

    This property holds whether the video is playing.
*/

/*!
    \qmlproperty bool Video::paused

    This property holds whether the video is paused.
*/

/*!
    \qmlproperty enum Video::status

    This property holds the status of video loading. It can be one of:

    \list
    \o NoMedia - no video has been set.
    \o Loading - the video is currently being loaded.
    \o Loaded - the video has been loaded.
    \o Buffering - the video is buffering data.
    \o Stalled - playback has been interrupted while the video is buffering data.
    \o Buffered - the video has buffered data.
    \o EndOfMedia - the video has played to the end.
    \o InvalidMedia - the video cannot be played.
    \o UnknownStatus - the status of the video is unknown.
    \endlist
*/

QmlGraphicsVideo::Status QmlGraphicsVideo::status() const
{
    return Status(m_status);
}

/*!
    \qmlproperty int Video::duration

    This property holds the duration of the video in milliseconds.

    If the video doesn't have a fixed duration (a live stream for example) this will be 0.
*/

/*!
    \qmlproperty int Video::position

    This property holds the current playback position in milliseconds.
*/

/*!
    \qmlproperty qreal Video::volume

    This property holds the volume of the audio output, from 0.0 (silent) to 1.0 (maximum volume).
*/

/*!
    \qmlproperty bool Video::muted

    This property holds whether the audio output is muted.
*/

/*!
    \qmlproperty bool Video::hasVideo

    This property holds whether the source contains video.
*/

bool QmlGraphicsVideo::hasVideo() const
{
    return m_playerControl->isVideoAvailable();
}

/*!
    \qmlproperty qreal Video::bufferProgress

    This property holds how much of the data buffer is currently filled, from 0.0 (empty) to 1.0
    (full).
*/

/*!
    \qmlproperty bool Video::seekable

    This property holds whether position of the video can be changed.
*/

/*!
    \qmlproperty qreal playbackRate

    This property holds the rate at which video is played at as a multiple of the normal rate.
*/

/*!
    \qmlproperty enum Video::error

    This property holds the error state of the video.  It can be one of:

    \list
    \o NoError - there is no current error.
    \o ResourceError - the video cannot be played due to a problem allocating resources.
    \o FormatError - the video format is not supported.
    \o NetworkError - the video cannot be played due to network issues.
    \o AccessDenied - the video cannot be played due to insufficient permissions.
    \o ServiceMissing -  the video cannot be played because the media service could not be
    instantiated.
    \endlist
*/


QmlGraphicsVideo::Error QmlGraphicsVideo::error() const
{
    return Error(m_error);
}

/*!
    \qmlproperty enum Video::errorString

    This property holds a string describing the current error condition in more detail.
*/

/*!
    \qmlproperty enum Video::FillMode

    Set this property to define how the video is scaled to fit the target area.

    \list
    \o Stretch - the video is scaled to fit.
    \o PreserveAspectFit - the image is scaled uniformly to fit without cropping
    \o PreserveAspectCrop - the image is scaled uniformly to fill, cropping if necessary
    \endlist
*/

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

/*!
    \qmlmethod Video::play()

    Starts playback of the video.
*/

void QmlGraphicsVideo::play()
{
    m_playerControl->play();
}

/*!
    \qmlmethod Video::pause()

    Pauses playback of the video.
*/

void QmlGraphicsVideo::pause()
{
    m_playerControl->pause();
}

/*!
    \qmlmethod Video::stop()

    Stops playback of the video.
*/

void QmlGraphicsVideo::stop()
{
    m_playerControl->stop();
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

#include "moc_qmlgraphicsvideo_p.cpp"

QTM_END_NAMESPACE
