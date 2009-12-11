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
    m_playerControl->stop();

    if (m_videoRendererControl)
        m_videoRendererControl->setSurface(0);

    delete m_videoSurface;
}

/*!
    \qmlproperty url Video::source
*/

/*!
    \qmlproperty enum Video::state
*/

/*!
    \qmlproperty enum Video::status
*/

/*!
    \qmlproperty real Video::duration
*/

/*!
    \qmlproperty real Video::position
*/


/*!
    \qmlproperty int Video::volume
*/

/*!
    \qmlproperty bool Video::muted
*/

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

/*!
    \qmlproperty bool Video::seekable
*/

/*!
    \qmlproperty qreal playbackRate
*/

/*!
    \qmlproperty enum Video::error
*/

/*!
    \qmlproperty enum Video::errorString
*/

/*!
    \qmlproperty enum Video::FillMode
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
*/

void QmlGraphicsVideo::play()
{
    m_playerControl->play();
}

/*!
    \qmlmethod Video::pause()
*/

void QmlGraphicsVideo::pause()
{
    m_playerControl->pause();
}

/*!
    \qmlmethod Video::stop()
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
