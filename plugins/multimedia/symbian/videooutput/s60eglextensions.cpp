/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60eglextensions.h"
#include "s60mmtrace.h"
#include <e32base.h>
#include <egl/egl.h>
#include <graphics/surface.h>
#include <QtCore/QDebug>
#include <QtCore/QTime>

S60EglExtensions *S60EglExtensions::create(QObject *parent)
{
    S60EglExtensions *self = new S60EglExtensions(parent);
    if (!self->initialize()) {
        delete self;
        self = 0;
    }
    return self;
}

S60EglExtensions::S60EglExtensions(QObject *parent)
    : QObject(parent)
    , m_eglCreateEndpointNOK(0)
    , m_eglDestroyEndpointNOK(0)
    , m_eglGetEndpointAttribNOK(0)
    , m_eglSetEndpointAttribNOK(0)
    , m_eglAcquireImageNOK(0)
    , m_eglReleaseImageNOK(0)
    , m_eglGetEndpointDirtyAreaNOK(0)
    , m_eglEndpointRequestNotificationNOK(0)
    , m_eglEndpointCancelNotificationNOK(0)
    , m_eglDestroyImageKHR(0)
#ifndef QT_NO_OPENGL
    , m_glEGLImageTargetTexture2DOES(0)
#endif
#ifndef QT_NO_OPENVG
    , m_vgCreateEGLImageTargetKHR(0)
#endif
{

}

S60EglExtensions::~S60EglExtensions()
{

}

bool S60EglExtensions::initialize()
{
    const bool endpoint =
                getProcAddress("eglCreateEndpointNOK", m_eglCreateEndpointNOK)
             && getProcAddress("eglDestroyEndpointNOK", m_eglDestroyEndpointNOK)
             && getProcAddress("eglGetEndpointAttribNOK", m_eglGetEndpointAttribNOK)
             && getProcAddress("eglSetEndpointAttribNOK", m_eglSetEndpointAttribNOK)
             && getProcAddress("eglAcquireImageNOK", m_eglAcquireImageNOK)
             && getProcAddress("eglReleaseImageNOK", m_eglReleaseImageNOK)
             && getProcAddress("eglGetEndpointDirtyAreaNOK", m_eglGetEndpointDirtyAreaNOK)
             && getProcAddress("eglEndpointRequestNotificationNOK", m_eglEndpointRequestNotificationNOK)
             && getProcAddress("eglEndpointCancelNotificationNOK", m_eglEndpointCancelNotificationNOK)
             && getProcAddress("eglDestroyImageKHR", m_eglDestroyImageKHR);

    bool gl = true;
#ifndef QT_NO_OPENGL
    gl = getProcAddress("glEGLImageTargetTexture2DOES", m_glEGLImageTargetTexture2DOES);
#endif

    bool vg = true;
#ifndef QT_NO_OPENVG
    vg = getProcAddress("vgCreateEGLImageTargetKHR", m_vgCreateEGLImageTargetKHR);
#endif

    return (endpoint && gl && vg);
}

template <typename FuncPtr>
bool S60EglExtensions::getProcAddress(const char *procName, FuncPtr &funcPtr)
{
    funcPtr = reinterpret_cast<FuncPtr>(eglGetProcAddress(procName));
    if (!funcPtr)
        qWarning() << "S60EglEndpointFunctions::getProcAddress" << procName << "not found";
    return (funcPtr != 0);
}

EGLEndpointNOK S60EglExtensions::createEndpoint(EGLDisplay dpy, EGLenum type, EGLenum source_type,
                                                       EGLEndpointSourceNOK source, const EGLint *attrib_list) const
{
    return m_eglCreateEndpointNOK(dpy, type, source_type, source, attrib_list);
}

EGLBoolean S60EglExtensions::destroyEndpoint(EGLDisplay dpy, EGLEndpointNOK endpoint) const
{
    return m_eglDestroyEndpointNOK(dpy, endpoint);
}

EGLint S60EglExtensions::getEndpointAttrib(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint attrib) const
{
    return m_eglGetEndpointAttribNOK(dpy, endpoint, attrib);
}

EGLBoolean S60EglExtensions::setEndpointAttrib(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint attrib, EGLint value) const
{
    return m_eglSetEndpointAttribNOK(dpy, endpoint, attrib, value);
}

EGLImageKHR S60EglExtensions::acquireImage(EGLDisplay dpy, EGLEndpointNOK endpoint) const
{
    return m_eglAcquireImageNOK(dpy, endpoint);
}

EGLBoolean S60EglExtensions::releaseImage(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLImageKHR image, EGLenum api) const
{
    return m_eglReleaseImageNOK(dpy, endpoint, image, api);
}

EGLint S60EglExtensions::getEndpointDirtyArea(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint* rects,
                                              EGLint start_rect, EGLint max_rects, EGLBoolean collapse) const
{
    return m_eglGetEndpointDirtyAreaNOK(dpy, endpoint, rects, start_rect, max_rects, collapse);
}

EGLBoolean S60EglExtensions::endpointRequestNotification(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLTRequestStatusNOK sync) const
{
    return m_eglEndpointRequestNotificationNOK(dpy, endpoint, sync);
}

EGLBoolean S60EglExtensions::endpointCancelNotification(EGLDisplay dpy, EGLEndpointNOK endpoint) const
{
    return m_eglEndpointCancelNotificationNOK(dpy, endpoint);
}

EGLBoolean S60EglExtensions::destroyImage(EGLDisplay dpy, EGLImageKHR image) const
{
    return m_eglDestroyImageKHR(dpy, image);
}

#ifndef QT_NO_OPENGL
void S60EglExtensions::glEglImageTargetTexture(GLenum target, GLeglImageOES image)
{
    return m_glEGLImageTargetTexture2DOES(target, image);
}
#endif

#ifndef QT_NO_OPENVG
VGImage S60EglExtensions::vgCreateEglImageTarget(EGLImageKHR image)
{
    return m_vgCreateEGLImageTargetKHR(image);
}
#endif // !QT_NO_OPENVG

