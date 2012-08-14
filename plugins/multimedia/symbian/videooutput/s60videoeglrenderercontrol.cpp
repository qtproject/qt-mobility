/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60eglendpoint.h"
#include "s60mmtrace.h"
#include "s60videobuffer.h"
#include "s60videoeglrenderercontrol.h"
#include <EGL/egl.h>
#include <qabstractvideosurface.h>
#include <qvideoframe.h>
#include <qvideosurfaceformat.h>

#ifdef VIDEOOUTPUT_MEASURE_FRAMERATE
#include "s60videoframerate.h"
#endif

S60VideoEglRendererControl::S60VideoEglRendererControl(S60EglExtensions *extensions, QObject *parent)
    : QVideoRendererControl(parent)
    , m_nativeSurface(TSurfaceId::CreateNullId())
    , m_doesProduceEglImages(false)
    , m_surface(0)
    , m_eglExtensions(extensions)
    , m_eglEndpoint(0)
    , m_buffer(0)
    , m_frameRate(0)
{
    TRACE("S60VideoEglRendererControl::S60VideoEglRendererControl" << qtThisPtr());
    Q_ASSERT(m_eglExtensions);
#ifdef VIDEOOUTPUT_MEASURE_FRAMERATE
    m_frameRate = new S60VideoFrameRate(this);
#endif
}

S60VideoEglRendererControl::~S60VideoEglRendererControl()
{
    TRACE("S60VideoEglRendererControl::~S60VideoEglRendererControl" << qtThisPtr());
}

QAbstractVideoSurface *S60VideoEglRendererControl::surface() const
{
    return m_surface;
}

void S60VideoEglRendererControl::setSurface(QAbstractVideoSurface *surface)
{
    if (surface != m_surface) {
        TRACE("S60VideoEglRendererControl::setSurface" << qtThisPtr()
              << "surface" << surface);
        m_surface = surface;
        if (!m_surface)
            destroyEndpoint();
        else if (!m_nativeSurface.IsNull())
            createEndpoint();
        emit surfaceChanged();
    }
}

void S60VideoEglRendererControl::setNativeSurface(TSurfaceId surface)
{
    if (surface != m_nativeSurface) {
        TRACE("S60VideoEglRendererControl::setNativeSurface" << qtThisPtr()
              << "surface" << (void*)surface.iInternal[3]);
        m_nativeSurface = surface;
        if (m_nativeSurface.IsNull())
            destroyEndpoint();
        else if (m_surface)
            createEndpoint();
    }
}

const QSize &S60VideoEglRendererControl::nativeSize() const
{
    return m_nativeSize;
}

bool S60VideoEglRendererControl::doesProduceEglImages() const
{
    return m_doesProduceEglImages;
}

void S60VideoEglRendererControl::setNativeSize(QSize size)
{
    if (size != m_nativeSize) {
        TRACE("S60VideoEglRendererControl::setNativeSize" << qtThisPtr()
              << "size" << size);
        m_nativeSize = size;
        emit nativeSizeChanged();
    }
}

void S60VideoEglRendererControl::setDoesProduceEglImages(bool enabled)
{
    if (enabled != m_doesProduceEglImages) {
        TRACE("S60VideoEglRendererControl::setDoesProduceEglImages" << qtThisPtr()
              << "enabled" << enabled);
        m_doesProduceEglImages = enabled;
    }
}

// Helper function
EGLenum currentEglApi()
{
    EGLenum api = 0;
    if (EGL_NO_CONTEXT != eglGetCurrentContext())
        api = eglQueryAPI();
    return api;
}

void S60VideoEglRendererControl::imageAvailable()
{
    const EGLenum api = currentEglApi();
    VERBOSE_TRACE("S60VideoEglRendererControl::imageAvailable" << qtThisPtr()
                  << "api" << api);
    Q_ASSERT(!m_buffer);

    if (!api)
        return;

    if (m_doesProduceEglImages) {
        m_buffer = new S60EglImageVideoBuffer(this, m_eglEndpoint);
    } else {
        switch (api) {
#ifndef QT_NO_OPENGL
        case EGL_OPENGL_ES_API:
            m_buffer = new S60GlTextureVideoBuffer(this, m_eglEndpoint, m_eglExtensions);
            break;
#endif
#ifndef QT_NO_OPENVG
        case EGL_OPENVG_API:
            m_buffer = new S60VgImageVideoBuffer(this, m_eglEndpoint, m_eglExtensions);
            break;
#endif
        }
    }
    Q_ASSERT(m_buffer);
    QVideoSurfaceFormat format(m_nativeSize,
                               QVideoFrame::Format_RGB32,
                               m_buffer->handleType());
    format.setScanLineDirection(QVideoSurfaceFormat::BottomToTop);
    if (!m_surface->isActive())
        m_surface->start(format);
    const QVideoFrame frame(m_buffer, format.frameSize(), format.pixelFormat());
    m_surface->present(frame);
#ifdef VIDEOOUTPUT_MEASURE_FRAMERATE
    m_frameRate->notify();
#endif
}

EGLImageKHR S60VideoEglRendererControl::acquireEglImage(S60EglEndpoint *endpoint) const
{
    VERBOSE_TRACE("S60VideoEglRendererControl::acquireEglImage" << qtThisPtr());
    EGLImageKHR image = EGL_NO_IMAGE_KHR;
    if (m_eglEndpoint == endpoint)
        image = m_eglEndpoint->acquireImage();
    return image;
}

void S60VideoEglRendererControl::releaseEglImage(S60EglEndpoint *endpoint, qint64 delay)
{
    VERBOSE_TRACE("S60VideoEglRendererControl::releaseEglImage" << qtThisPtr()
                  << "delay" << delay);
    if (m_eglEndpoint == endpoint) {
        if (delay)
            m_eglEndpoint->setDelay(delay);
        m_eglEndpoint->releaseImage();
    }
    m_buffer = 0;
}

void S60VideoEglRendererControl::destroyEndpoint()
{
    if (m_eglEndpoint) {
        delete m_eglEndpoint;
        m_eglEndpoint = 0;
    }
}

void S60VideoEglRendererControl::createEndpoint()
{
    Q_ASSERT(m_surface);
    Q_ASSERT(!m_nativeSurface.IsNull());
    Q_ASSERT(!m_eglEndpoint);
    m_eglEndpoint = new S60EglEndpoint(m_nativeSurface, m_eglExtensions, this);
    if (m_eglEndpoint->isValid())
        connect(m_eglEndpoint, SIGNAL(imageAvailable()),
                this, SLOT(imageAvailable()));
    else
        emit error();
}
