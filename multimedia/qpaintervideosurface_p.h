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

#ifndef QPAINTERVIDEOSURFACE_P_H
#define QPAINTERVIDEOSURFACE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QT_NO_MULTIMEDIA

#include <QtCore/qsize.h>
#include <QtGui/qimage.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qpaintengine.h>
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideoframe.h>

#include <qmultimediaglobal.h>

#ifndef QT_NO_OPENGL
# include <QtOpenGL/qgl.h>
# ifndef APIENTRY
#  define APIENTRY
#  define QT_TMP_APIENTRY
# endif
#endif

class Q_MEDIA_EXPORT QPainterVideoSurface : public QAbstractVideoSurface
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

    int brightness() const { return m_brightness; }
    void setBrightness(int brightness);

    int contrast() const { return m_contrast; }
    void setContrast(int contrast);

    int hue() const { return m_hue; }
    void setHue(int hue);

    int saturation() const { return m_saturation; }
    void setSaturation(int saturation);

    bool isReady() const { return m_ready; }
    void setReady(bool ready);

    void paint(QPainter *painter, const QRect &rect);

Q_SIGNALS:
    void frameChanged();

private:
#ifndef QT_NO_OPENGL
protected:
    explicit QPainterVideoSurface(const QGLContext *context, QObject *parent = 0);

    virtual void makeCurrent() {}
    virtual void doneCurrent() {}

private:
    void initRgbTextureInfo(GLenum internalFormat, GLuint format, const QSize &size);
    void initYuv420PTextureInfo(const QSize &size);
    void initYv12TextureInfo(const QSize &size);

    void updateColorMatrix();

    typedef void (APIENTRY *_glProgramStringARB) (GLenum, GLenum, GLsizei, const GLvoid *);
    typedef void (APIENTRY *_glBindProgramARB) (GLenum, GLuint);
    typedef void (APIENTRY *_glDeleteProgramsARB) (GLsizei, const GLuint *);
    typedef void (APIENTRY *_glGenProgramsARB) (GLsizei, GLuint *);
    typedef void (APIENTRY *_glProgramLocalParameter4fARB) (
            GLenum, GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
    typedef void (APIENTRY *_glActiveTexture) (GLenum);

    _glProgramStringARB glProgramStringARB;
    _glBindProgramARB glBindProgramARB;
    _glDeleteProgramsARB glDeleteProgramsARB;
    _glGenProgramsARB glGenProgramsARB;
    _glProgramLocalParameter4fARB glProgramLocalParameter4fARB;
    _glActiveTexture glActiveTexture;

    enum ShaderSupport
    {
        ShadersUnsupported,
        ShadersSupported
    };

    ShaderSupport m_shaderSupport;

    GLenum m_textureFormat;
    GLuint m_textureInternalFormat;
    int m_textureCount;
    GLuint m_textureIds[3];
    int m_textureWidths[3];
    int m_textureHeights[3];
    int m_textureOffsets[3];
    GLuint m_shaderId;
#endif

    int m_brightness;
    int m_contrast;
    int m_hue;
    int m_saturation;

    QVideoFrame m_frame;
    QAbstractVideoBuffer::HandleType m_handleType;
    QVideoFrame::PixelFormat m_pixelFormat;
    QImage::Format m_imageFormat;
    QSize m_imageSize;
    QRect m_sourceRect;
    QMatrix4x4 m_colorMatrix;
    bool m_colorMatrixDirty;
    bool m_ready;
};


#ifndef QT_NO_OPENGL
# ifdef QT_TMP_APIENTRY
#  undef APIENTRY
#  undef QT_TMP_APIENTRY
# endif
#endif

#endif

#endif
