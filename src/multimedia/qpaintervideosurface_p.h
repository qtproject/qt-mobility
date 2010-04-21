/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPAINTERVIDEOSURFACE_P_H
#define QPAINTERVIDEOSURFACE_P_H

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

#include <qmobilityglobal.h>
#include <QtCore/qsize.h>
#include <QtGui/qimage.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qpaintengine.h>
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideoframe.h>

QT_BEGIN_NAMESPACE
class QGLContext;
QT_END_NAMESPACE

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

class QVideoSurfacePainter;
class Q_AUTOTEST_EXPORT QPainterVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit QPainterVideoSurface(QObject *parent = 0);
    ~QPainterVideoSurface();

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;

    bool isFormatSupported(
            const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar = 0) const;

    bool start(const QVideoSurfaceFormat &format);
    void stop();

    bool present(const QVideoFrame &frame);

    int brightness() const;
    void setBrightness(int brightness);

    int contrast() const;
    void setContrast(int contrast);

    int hue() const;
    void setHue(int hue);

    int saturation() const;
    void setSaturation(int saturation);

    bool isReady() const;
    void setReady(bool ready);

    void paint(QPainter *painter, const QRectF &target, const QRectF &source = QRectF(0, 0, 1, 1));

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
    const QGLContext *glContext() const;
    void setGLContext(QGLContext *context);

    enum ShaderType
    {
        NoShaders = 0x00,
        FragmentProgramShader = 0x01,
        GlslShader = 0x02
    };

    Q_DECLARE_FLAGS(ShaderTypes, ShaderType)

    ShaderTypes supportedShaderTypes() const;

    ShaderType shaderType() const;
    void setShaderType(ShaderType type);
#endif

Q_SIGNALS:
    void frameChanged();

private:
    void createPainter();

    QVideoSurfacePainter *m_painter;
#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
    QGLContext *m_glContext;
    ShaderTypes m_shaderTypes;
    ShaderType m_shaderType;
#endif
    int m_brightness;
    int m_contrast;
    int m_hue;
    int m_saturation;

    QVideoFrame::PixelFormat m_pixelFormat;
    QSize m_frameSize;
    QRect m_sourceRect;
    bool m_colorsDirty;
    bool m_ready;
};

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
Q_DECLARE_OPERATORS_FOR_FLAGS(QPainterVideoSurface::ShaderTypes)
#endif

QT_END_NAMESPACE

#endif
