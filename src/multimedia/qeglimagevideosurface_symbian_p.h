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

#ifndef QEGLIMAGESURFACE_SYMBIAN_P_H
#define QEGLIMAGESURFACE_SYMBIAN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qabstractvideosurface.h>
#include <qvideoframe.h>
#include <qvideosurfaceformat.h>

#if !defined(QT_NO_OPENGL) || !defined(QT_NO_OPENVG)
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif

#ifndef QT_NO_OPENGL
#include <QtOpenGL/qgl.h>
#endif

#ifndef QT_NO_OPENVG
#include <QtOpenVG/qvg.h>
#endif

#ifdef _DEBUG
#define QTMMK_ENABLE_TRACE
#endif

//#define QTMMK_ENABLE_VERBOSE_TRACE

#ifdef QTMMK_ENABLE_TRACE

#   include <QtCore/QDebug>
#   define TRACE(args) qDebug() << "[QtMultimediaKit]" << args
#   ifdef QTMMK_ENABLE_VERBOSE_TRACE
#       define VERBOSE_TRACE(args) TRACE(args)
#   else // QTMMK_ENABLE_VERBOSE_TRACE
#       define VERBOSE_TRACE(args)
#   endif // QTMMK_ENABLE_VERBOSE_TRACE

    template <typename T>
    inline const void *qtVoidPtr(T *ptr)
    { return reinterpret_cast<const void *>(ptr); }

#   define qtThisPtr() qtVoidPtr(this)

#else // QTMMK_ENABLE_TRACE
#   define TRACE(args)
#   define VERBOSE_TRACE(args)
#endif

QT_BEGIN_NAMESPACE

class QEglImageVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    ~QEglImageVideoSurface();

    // QAbstractVideoSurface
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
    bool start(const QVideoSurfaceFormat &format);
    void stop();
    bool present(const QVideoFrame &frame);

    void setReady(bool ready);
    virtual void paint(QPainter *painter,
                       const QRectF &sourceRect, const QRectF &targetRect) = 0;

signals:
    void frameChanged();

public slots:
    void viewportDestroyed();

protected:
    QEglImageVideoSurface(QObject *parent);
    void frameDisplayed();

private:
    virtual void onViewportDestroyed();
    virtual void doStart(const QVideoSurfaceFormat &format);
    virtual void doStop() = 0;

protected:
    QVideoFrame m_frame;
    QVideoSurfaceFormat m_format;

private:
    bool m_frameIsNew;
    bool m_ready;
};

#ifndef QT_NO_OPENGL

class QGLShaderProgram;

class QEglImageGLVideoSurface : public QEglImageVideoSurface
{
    Q_OBJECT
public:
    QEglImageGLVideoSurface(QObject *parent = 0);
    ~QEglImageGLVideoSurface();

    // QEglImageVideoSurface
    void paint(QPainter *painter,
               const QRectF &sourceRect, const QRectF &targetRect);

private:
    void onViewportDestroyed();
    void doStart(const QVideoSurfaceFormat &format);
    void doStop();
    void bindTexture();
    void unbindTexture();

private:
    QGLContext *m_context;
    QGLShaderProgram *m_shaderProgram;
};

#endif // !QT_NO_OPENGL

#ifndef QT_NO_OPENVG

class QEglImageVGVideoSurface : public QEglImageVideoSurface
{
    Q_OBJECT
public:
    QEglImageVGVideoSurface(QObject *parent = 0);
    ~QEglImageVGVideoSurface();

    // QEglImageVideoSurface
    void paint(QPainter *painter,
               const QRectF &sourceRect, const QRectF &targetRect);

private:
    void doStop();
};

#endif // !QT_NO_OPENVG

QT_END_NAMESPACE

#endif
