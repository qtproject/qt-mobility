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

#include "s60eglextensions.h"
#include "s60mmtrace.h"
#include "s60videoeglrenderercontrol.h"
#include "s60videobuffer.h"
#include <QtCore/QElapsedTimer>
#include <QtCore/QVariant>

#ifndef QT_NO_OPENGL
#include <GLES/glext.h>
#endif

// TODO: get rid of these magic numbers
const QAbstractVideoBuffer::HandleType EGLImageTextureHandle =
      QAbstractVideoBuffer::HandleType(QAbstractVideoBuffer::UserHandle + 128);
const QAbstractVideoBuffer::HandleType VGImageTextureHandle =
      QAbstractVideoBuffer::HandleType(QAbstractVideoBuffer::UserHandle + 129);

// Dummy pointer returned by map() - see comments below
static uchar* const DummyMapReturnValue = reinterpret_cast<uchar *>(1);

//-----------------------------------------------------------------------------
// S60EglImageVideoBuffer
//-----------------------------------------------------------------------------

S60EglImageVideoBuffer::S60EglImageVideoBuffer(S60VideoEglRendererControl *control,
                                               S60EglEndpoint *endpoint)
    : QAbstractVideoBuffer(EGLImageTextureHandle)
    , m_timer(new QElapsedTimer)
    , m_control(control)
    , m_endpoint(endpoint)
    , m_delay(0)
{
    VERBOSE_TRACE("S60EglImageVideoBuffer::S60EglImageVideoBuffer" << qtThisPtr());
}

S60EglImageVideoBuffer::S60EglImageVideoBuffer(S60VideoEglRendererControl *control,
                                               S60EglEndpoint *endpoint,
                                               QAbstractVideoBuffer::HandleType type)
    : QAbstractVideoBuffer(type)
    , m_timer(new QElapsedTimer)
    , m_control(control)
    , m_endpoint(endpoint)
    , m_delay(0)
{

}

S60EglImageVideoBuffer::~S60EglImageVideoBuffer()
{
    VERBOSE_TRACE("S60EglImageVideoBuffer::~S60EglImageVideoBuffer" << qtThisPtr());
    unmap();
}

QAbstractVideoBuffer::MapMode S60EglImageVideoBuffer::mapMode() const
{
    return NotMapped;
}

// Note: the use of map/unmap to indicate when the frame has been displayed
// by QGraphicsVideoItem is something of a hack, but in QtMobility 1.2,
// there is no suitable QVideoFrame API, so we must do it this way.

uchar *S60EglImageVideoBuffer::map(MapMode mode, int *numBytes, int *bytesPerLine)
{
    Q_UNUSED(mode);
    Q_UNUSED(numBytes);
    Q_UNUSED(bytesPerLine);
    VERBOSE_TRACE("S60EglImageVideoBuffer::map" << qtThisPtr());
    acquireEglImage();
    return DummyMapReturnValue;
}

void S60EglImageVideoBuffer::unmap()
{
    VERBOSE_TRACE("S60EglImageVideoBuffer::unmap" << qtThisPtr());
    releaseEglImage();
}

QVariant S60EglImageVideoBuffer::handle() const
{
    return m_eglImage;
}

void S60EglImageVideoBuffer::acquireEglImage()
{
    Q_ASSERT(EGL_NO_IMAGE_KHR == m_eglImage);
    m_timer->start();
    m_eglImage = m_control->acquireEglImage(m_endpoint);
}

void S60EglImageVideoBuffer::releaseEglImage()
{
    if (EGL_NO_IMAGE_KHR != m_eglImage) {
        VERBOSE_TRACE("S60EglImageVideoBuffer::releaseEglImage" << qtThisPtr());
        // Here we need to calculate the time interval between acquiring an image
        // from the endpoint, and that image being rendered.  This is passed back
        // to the producer, which may use it to correct AV sync.
        // We assume that unmap() was called by the client shortly after rendering
        // the image.
        m_delay = 1000 * m_timer->elapsed();
        m_control->releaseEglImage(m_endpoint, m_delay);
        m_eglImage = EGL_NO_IMAGE_KHR;
    }
}


//-----------------------------------------------------------------------------
// S60GlTextureVideoBuffer
//-----------------------------------------------------------------------------

#ifndef QT_NO_OPENGL

S60GlTextureVideoBuffer::S60GlTextureVideoBuffer(S60VideoEglRendererControl *control,
                                                 S60EglEndpoint *endpoint,
                                                 S60EglExtensions *extensions)
    : S60EglImageVideoBuffer(control, endpoint, QAbstractVideoBuffer::GLTextureHandle)
    , m_extensions(extensions)
    , m_texture(0)
{

}

S60GlTextureVideoBuffer::~S60GlTextureVideoBuffer()
{
    VERBOSE_TRACE("S60GlTextureVideoBuffer::~S60GlTextureVideoBuffer" << qtThisPtr());
    unmap();
}

uchar *S60GlTextureVideoBuffer::map(MapMode mode, int *numBytes, int *bytesPerLine)
{
    Q_UNUSED(mode);
    Q_UNUSED(numBytes);
    Q_UNUSED(bytesPerLine);
    VERBOSE_TRACE("S60GlTextureVideoBuffer::map" << qtThisPtr());
    acquireEglImage();
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    const GLeglImageOES glImage = reinterpret_cast<GLeglImageOES>(m_eglImage);
    m_extensions->glEglImageTargetTexture(GL_TEXTURE_2D, glImage);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    return DummyMapReturnValue;
}

void S60GlTextureVideoBuffer::unmap()
{
    VERBOSE_TRACE("S60GlTextureVideoBuffer::unmap" << qtThisPtr());
    if (m_texture)
        glDeleteTextures(1, &m_texture);
    releaseEglImage();
}

QVariant S60GlTextureVideoBuffer::handle() const
{
    return m_texture;
}

#endif // !QT_NO_OPENGL


//-----------------------------------------------------------------------------
// S60VGImageVideoBuffer
//-----------------------------------------------------------------------------

#ifndef QT_NO_OPENGL

S60VgImageVideoBuffer::S60VgImageVideoBuffer(S60VideoEglRendererControl *control,
                                             S60EglEndpoint *endpoint,
                                             S60EglExtensions *extensions)
    : S60EglImageVideoBuffer(control, endpoint, VGImageTextureHandle)
    , m_extensions(extensions)
{
    VERBOSE_TRACE("S60VgImageVideoBuffer::S60VgImageVideoBuffer" << qtThisPtr());
}

S60VgImageVideoBuffer::~S60VgImageVideoBuffer()
{
    VERBOSE_TRACE("S60VgImageVideoBuffer::~S60VgImageVideoBuffer" << qtThisPtr());
    unmap();
}

uchar *S60VgImageVideoBuffer::map(MapMode mode, int *numBytes, int *bytesPerLine)
{
    Q_UNUSED(mode);
    Q_UNUSED(numBytes);
    Q_UNUSED(bytesPerLine);
    VERBOSE_TRACE("S60GlTextureVideoBuffer::map" << qtThisPtr());
    acquireEglImage();
    m_vgImage = m_extensions->vgCreateEglImageTarget(m_eglImage);
    return DummyMapReturnValue;
}

void S60VgImageVideoBuffer::unmap()
{
    VERBOSE_TRACE("S60VgImageVideoBuffer::unmap" << qtThisPtr());
    if (VG_INVALID_HANDLE != m_vgImage)
        vgDestroyImage(m_vgImage);
    releaseEglImage();
}

QVariant S60VgImageVideoBuffer::handle() const
{
    return static_cast<quint32>(m_vgImage);
}

#endif // !QT_NO_OPENGL

