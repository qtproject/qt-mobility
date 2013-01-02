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

#ifndef S60VIDEOEGLRENDERERCONTROL_H
#define S60VIDEOEGLRENDERERCONTROL_H

#include <QtCore/QSize>
#include <qvideorenderercontrol.h>
#include <egl/eglext.h>
#include <graphics/surface.h>

QT_USE_NAMESPACE

class QTime;
class QVideoFrame;
class S60EglEndpoint;
class S60EglExtensions;
class S60EglImageVideoBuffer;
class S60VideoFrameRate;

class S60VideoEglRendererControl : public QVideoRendererControl
{
    Q_OBJECT
    Q_PROPERTY(QSize nativeSize READ nativeSize WRITE setNativeSize NOTIFY nativeSizeChanged)

    /**
     * If set, the returned QVideoFrame handle type is EGLImageKHR.
     * If not set, the returned handle type depends on the current EGL rendering
     * API:
     *     for OpenGLES, GL texture handles
     *     for OpenVG, VG image handles
     * The default value is false.
     */
    Q_PROPERTY(bool doesProduceEglImages READ doesProduceEglImages WRITE setDoesProduceEglImages)

public:
    S60VideoEglRendererControl(S60EglExtensions *extensions, QObject *parent);
    ~S60VideoEglRendererControl();

public:
    // QVideoRendererControl
    QAbstractVideoSurface *surface() const;
    void setSurface(QAbstractVideoSurface *surface);

    const QSize &nativeSize() const;
    bool doesProduceEglImages() const;

public slots:
    void setNativeSurface(TSurfaceId surface);
    void setNativeSize(QSize size);
    void setDoesProduceEglImages(bool enabled);

signals:
    void surfaceChanged();
    void nativeSizeChanged();
    void error();

private slots:
    void imageAvailable();

private:
    void destroyEndpoint();
    void createEndpoint();

private:
    // Called by S60EglImageVideoBuffer
    EGLImageKHR acquireEglImage(S60EglEndpoint *endpoint) const;
    void releaseEglImage(S60EglEndpoint *endpoint, qint64 delay);

private:
    friend class S60EglImageVideoBuffer;
    TSurfaceId m_nativeSurface;
    QSize m_nativeSize;
    bool m_doesProduceEglImages;
    QAbstractVideoSurface *m_surface;
    S60EglExtensions *m_eglExtensions;
    S60EglEndpoint *m_eglEndpoint;
    S60EglImageVideoBuffer *m_buffer;
    S60VideoFrameRate *m_frameRate;
};

#endif // S60VIDEOEGLRENDERERCONTROL_H

