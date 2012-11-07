/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef S60EGLEXTENSIONS_H
#define S60EGLEXTENSIONS_H

#include <QtCore/QObject>
#include <egl/eglext.h>

#ifndef QT_NO_OPENGL
#include <QtOpenGL/qgl.h>
#include <GLES/glext.h>
#endif

#ifndef QT_NO_OPENVG
#include <QtOpenVG/qvg.h>
#endif

QT_USE_NAMESPACE

class S60EglExtensions : public QObject
    {
public:
    static S60EglExtensions *create(QObject *parent = 0);
    ~S60EglExtensions();

    // EGL_NOK_image_endpoint
    EGLEndpointNOK createEndpoint(EGLDisplay dpy, EGLenum type, EGLenum source_type, EGLEndpointSourceNOK source, const EGLint *attrib_list) const;
    EGLBoolean destroyEndpoint(EGLDisplay dpy, EGLEndpointNOK endpoint) const;
    EGLint getEndpointAttrib(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint attrib) const;
    EGLBoolean setEndpointAttrib(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint attrib, EGLint value) const;
    EGLImageKHR acquireImage(EGLDisplay dpy, EGLEndpointNOK endpoint) const;
    EGLBoolean releaseImage(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLImageKHR image, EGLenum api) const;
    EGLint getEndpointDirtyArea(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLint *rects, EGLint start_rect, EGLint max_rects, EGLBoolean collapse) const;
    EGLBoolean endpointRequestNotification(EGLDisplay dpy, EGLEndpointNOK endpoint, EGLTRequestStatusNOK sync) const;
    EGLBoolean endpointCancelNotification(EGLDisplay dpy, EGLEndpointNOK endpoint) const;
    EGLBoolean destroyImage(EGLDisplay dpy, EGLImageKHR image) const;

#ifndef QT_NO_OPENGL
    // EGL_KHR_gl_texture_2D_image
    void glEglImageTargetTexture(GLenum target, GLeglImageOES image);
#endif // !QT_NO_OPENGL

#ifndef QT_NO_OPENVG
    // EGL_KHR_vg_parent_image
    VGImage vgCreateEglImageTarget(EGLImageKHR image);
#endif // !QT_NO_OPENVG

private:
    S60EglExtensions(QObject *parent = 0);
    bool initialize();
    template <typename FuncPtr> static bool getProcAddress(const char *procName, FuncPtr &funcPtr);

private:
    // EGL_NOK_image_endpoint
    PFNEGLCREATEENDPOINTNOKPROC m_eglCreateEndpointNOK;
    PFNEGLDESTROYENDPOINTNOKPROC m_eglDestroyEndpointNOK;
    PFNEGLGETENDPOINTATTRIBNOKPROC m_eglGetEndpointAttribNOK;
    PFNEGLSETENDPOINTATTRIBNOKPROC m_eglSetEndpointAttribNOK;
    PFNEGLACQUIREIMAGENOKPROC m_eglAcquireImageNOK;
    PFNEGLRELEASEIMAGENOKPROC m_eglReleaseImageNOK;
    PFNEGLGETENDPOINTDIRTYAREANOKPROC m_eglGetEndpointDirtyAreaNOK;
    PFNEGLENDPOINTREQUESTNOTIFICATIONNOKPROC m_eglEndpointRequestNotificationNOK;
    PFNEGLENDPOINTCANCELNOTIFICATIONNOKPROC m_eglEndpointCancelNotificationNOK;
    PFNEGLDESTROYIMAGEKHRPROC m_eglDestroyImageKHR;

#ifndef QT_NO_OPENGL
    // EGL_KHR_gl_texture_2D_image
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC m_glEGLImageTargetTexture2DOES;
#endif // !QT_NO_OPENGL

#ifndef QT_NO_OPENVG
    // EGL_KHR_vg_parent_image
    typedef VGImage (*PFNCREATEEGLIMAGETARGETKHRPROC)(EGLImageKHR);
    PFNCREATEEGLIMAGETARGETKHRPROC m_vgCreateEGLImageTargetKHR;
#endif // !QT_NO_OPENVG
    };

#endif // S60EGLEXTENSIONS_H

