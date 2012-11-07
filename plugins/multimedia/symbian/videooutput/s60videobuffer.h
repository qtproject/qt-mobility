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

#ifndef S60VIDEOBUFFER_H
#define S60VIDEOBUFFER_H

#include <qabstractvideobuffer.h>
#include <egl/eglext.h>

#ifndef QT_NO_OPENGL
#include <QtOpenGL/qgl.h>
#endif

#ifndef QT_NO_OPENVG
#include <QtOpenVG/qvg.h>
#endif

QT_USE_NAMESPACE

class QElapsedTimer;
class S60EglEndpoint;
class S60EglExtensions;
class S60VideoEglRendererControl;

class S60EglImageVideoBuffer : public QAbstractVideoBuffer
{
public:
    S60EglImageVideoBuffer(S60VideoEglRendererControl *control,
                           S60EglEndpoint *endpoint);
    ~S60EglImageVideoBuffer();

    // QAbstractVideoBuffer
    MapMode mapMode() const;
    uchar *map(MapMode mode, int *numBytes, int *bytesPerLine);
    void unmap();
    QVariant handle() const;

protected:
    S60EglImageVideoBuffer(S60VideoEglRendererControl *control,
                           S60EglEndpoint *endpoint,
                           QAbstractVideoBuffer::HandleType type);
    void acquireEglImage();
    void releaseEglImage();

protected:
    EGLImageKHR m_eglImage;

private:
    QScopedPointer<QElapsedTimer> m_timer;
    S60VideoEglRendererControl *m_control;
    S60EglEndpoint *m_endpoint;
    qint64 m_delay;
};

#ifndef QT_NO_OPENGL
class S60GlTextureVideoBuffer : public S60EglImageVideoBuffer
{
public:
    S60GlTextureVideoBuffer(S60VideoEglRendererControl *control,
                            S60EglEndpoint *endpoint,
                            S60EglExtensions *extensions);
    ~S60GlTextureVideoBuffer();

    // QAbstractVideoBuffer
    uchar *map(MapMode mode, int *numBytes, int *bytesPerLine);
    void unmap();
    QVariant handle() const;

private:
    S60EglExtensions *m_extensions;
    GLuint m_texture;
};
#endif // !QT_NO_OPENGL

#ifndef QT_NO_OPENVG
class S60VgImageVideoBuffer : public S60EglImageVideoBuffer
{
public:
    S60VgImageVideoBuffer(S60VideoEglRendererControl *control,
                          S60EglEndpoint *endpoint,
                          S60EglExtensions *extensions);
    ~S60VgImageVideoBuffer();

    // QAbstractVideoBuffer
    uchar *map(MapMode mode, int *numBytes, int *bytesPerLine);
    void unmap();
    QVariant handle() const;

private:
    S60EglExtensions *m_extensions;
    VGImage m_vgImage;
};
#endif // !QT_NO_OPENVG

#endif // S60VIDEOBUFFER_H

