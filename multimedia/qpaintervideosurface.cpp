/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <multimedia/qpaintervideosurface_p.h>

#include <qpainter.h>
#include <qvariant.h>
#include <qvideosurfaceformat.h>
#include <qmath.h>

#ifndef QT_NO_OPENGL
# ifndef GL_FRAGMENT_PROGRAM_ARB
#  define GL_FRAGMENT_PROGRAM_ARB           0x8804
#  define GL_PROGRAM_FORMAT_ASCII_ARB       0x8875
# endif
# ifndef GL_TEXTURE0
#  define GL_TEXTURE0    0x84C0
#  define GL_TEXTURE1    0x84C1
#  define GL_TEXTURE2    0x84C2
# endif
# ifndef GL_PROGRAM_ERROR_STRING_ARB
#  define GL_PROGRAM_ERROR_STRING_ARB       0x8874
#endif
#endif

#ifndef GL_UNSIGNED_SHORT_5_6_5
#define GL_UNSIGNED_SHORT_5_6_5 33635
#endif

// Paints an RGB32 frame
static const char *qt_xrgbShaderProgram =
    "!!ARBfp1.0\n"
    "PARAM matrix[4] = { program.local[0..2],"
    "{ 0.0, 0.0, 0.0, 1.0 } };\n"
    "TEMP xrgb;\n"
    "TEX xrgb.xyz, fragment.texcoord[0], texture[0], 2D;\n"
    "MOV xrgb.w, matrix[3].w;\n"
    "DP4 result.color.x, xrgb.zyxw, matrix[0];\n"
    "DP4 result.color.y, xrgb.zyxw, matrix[1];\n"
    "DP4 result.color.z, xrgb.zyxw, matrix[2];\n"
    "END";

// Paints an ARGB frame.
static const char *qt_argbShaderProgram =
    "!!ARBfp1.0\n"
    "PARAM matrix[4] = { program.local[0..2],"
    "{ 0.0, 0.0, 0.0, 1.0 } };\n"
    "TEMP argb;\n"
    "TEX argb, fragment.texcoord[0], texture[0], 2D;\n"
    "MOV argb.w, matrix[3].w;\n"
    "DP4 result.color.x, argb.zyxw, matrix[0];\n"
    "DP4 result.color.y, argb.zyxw, matrix[1];\n"
    "DP4 result.color.z, argb.zyxw, matrix[2];\n"
    "TEX result.color.w, fragment.texcoord[0], texture, 2D;\n"
    "END";

// Paints an RGB(A) frame.
static const char *qt_rgbShaderProgram =
    "!!ARBfp1.0\n"
    "PARAM matrix[4] = { program.local[0..2],"
    "{ 0.0, 0.0, 0.0, 1.0 } };\n"
    "TEMP rgb;\n"
    "TEX rgb, fragment.texcoord[0], texture[0], 2D;\n"
    "MOV rgb.w, matrix[3].w;\n"
    "DP4 result.color.x, rgb, matrix[0];\n"
    "DP4 result.color.y, rgb, matrix[1];\n"
    "DP4 result.color.z, rgb, matrix[2];\n"
    "END";

// Paints a YUV420P or YV12 frame.
static const char *qt_yuvPlanarShaderProgram =
    "!!ARBfp1.0\n"
    "PARAM matrix[4] = { program.local[0..2],"
    "{ 0.0, 0.0, 0.0, 1.0 } };\n"
    "TEMP yuv;\n"
    "TEX yuv.x, fragment.texcoord[0], texture[0], 2D;\n"
    "TEX yuv.y, fragment.texcoord[0], texture[1], 2D;\n"
    "TEX yuv.z, fragment.texcoord[0], texture[2], 2D;\n"
    "MOV yuv.w, matrix[3].w;\n"
    "DP4 result.color.x, yuv, matrix[0];\n"
    "DP4 result.color.y, yuv, matrix[1];\n"
    "DP4 result.color.z, yuv, matrix[2];\n"
    "END";

/*!
    \class QPainterVideoSurface
    \internal
*/

/*!
*/
QPainterVideoSurface::QPainterVideoSurface(QObject *parent)
    : QAbstractVideoSurface(parent)
#ifndef QT_NO_OPENGL
    , m_shaderSupport(ShadersUnsupported)
    , m_textureFormat(0)
    , m_textureInternalFormat(0)
    , m_textureCount(0)
    , m_shaderId(0)
#endif
    , m_handleType(QAbstractVideoBuffer::NoHandle)
    , m_pixelFormat(QVideoFrame::Format_Invalid)
    , m_imageFormat(QImage::Format_Invalid)
    , m_colorMatrixDirty(true)
    , m_ready(false)
{
}

#ifndef QT_NO_OPENGL
QPainterVideoSurface::QPainterVideoSurface(const QGLContext *context, QObject *parent)
    : QAbstractVideoSurface(parent)
    , m_shaderSupport(ShadersUnsupported)
    , m_textureFormat(0)
    , m_textureInternalFormat(0)
    , m_textureCount(0)
    , m_shaderId(0)
    , m_handleType(QAbstractVideoBuffer::NoHandle)
    , m_pixelFormat(QVideoFrame::Format_Invalid)
    , m_imageFormat(QImage::Format_Invalid)
    , m_colorMatrixDirty(true)
    , m_ready(false)
{
    if (context) {
        const_cast<QGLContext *>(context)->makeCurrent();

        glProgramStringARB = (_glProgramStringARB) context->getProcAddress(
                QLatin1String("glProgramStringARB"));
        glBindProgramARB = (_glBindProgramARB) context->getProcAddress(
                QLatin1String("glBindProgramARB"));
        glDeleteProgramsARB = (_glDeleteProgramsARB) context->getProcAddress(
                QLatin1String("glDeleteProgramsARB"));
        glGenProgramsARB = (_glGenProgramsARB) context->getProcAddress(
                QLatin1String("glGenProgramsARB"));
        glProgramLocalParameter4fARB = (_glProgramLocalParameter4fARB) context->getProcAddress(
                QLatin1String("glProgramLocalParameter4fARB"));
        glActiveTexture = (_glActiveTexture) context->getProcAddress(
                QLatin1String("glActiveTexture"));

        if (glProgramStringARB
                && glBindProgramARB
                && glDeleteProgramsARB
                && glGenProgramsARB
                && glProgramLocalParameter4fARB
                && glActiveTexture) {
            m_shaderSupport = ShadersSupported;
        }
        const_cast<QGLContext *>(context)->doneCurrent();
    }
}
#endif

/*!
*/
QPainterVideoSurface::~QPainterVideoSurface()
{
}

/*!
*/
QList<QVideoFrame::PixelFormat> QPainterVideoSurface::supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType) const
{
    QList<QVideoFrame::PixelFormat> formats;

    if (handleType == QAbstractVideoBuffer::NoHandle) {
        formats << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_RGB24
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_ARGB32_Premultiplied
                << QVideoFrame::Format_RGB565
                << QVideoFrame::Format_RGB555;
#ifndef QT_NO_OPENGL
        if (m_shaderSupport == ShadersSupported) {
            formats << QVideoFrame::Format_YV12
                    << QVideoFrame::Format_YUV420P;
        }
    } else if (handleType == QAbstractVideoBuffer::GLTextureHandle
            && m_shaderSupport == ShadersSupported) {
        formats << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_RGB565;
#endif
    }

    return formats;
}

/*!
*/
bool QPainterVideoSurface::isFormatSupported(
        const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar) const
{
    Q_UNUSED(similar);

    return !format.frameSize().isEmpty()
            && supportedPixelFormats(format.handleType()).contains(format.pixelFormat());
}

/*!
*/
bool QPainterVideoSurface::start(const QVideoSurfaceFormat &format)
{
    m_frame = QVideoFrame();

    const QSize imageSize = format.frameSize();

    if (imageSize.isEmpty()) {
        setError(UnsupportedFormatError);
    } else {
#ifndef QT_NO_OPENGL
        if (m_shaderSupport == ShadersSupported) {
            makeCurrent();

            if (m_textureCount > 0) {
                if (m_handleType != QAbstractVideoBuffer::GLTextureHandle)
                    glDeleteTextures(m_textureCount, m_textureIds);
                glDeleteProgramsARB(1, &m_shaderId);

                m_textureCount = 0;
                m_shaderId = 0;
            }

            const char *program = 0;

            if (format.handleType() == QAbstractVideoBuffer::NoHandle) {
                switch (format.pixelFormat()) {
                case QVideoFrame::Format_RGB32:
                    initRgbTextureInfo(GL_RGBA, GL_RGBA, imageSize);
                    program = qt_xrgbShaderProgram;
                    break;
                case QVideoFrame::Format_ARGB32:
                    initRgbTextureInfo(GL_RGBA, GL_RGBA, imageSize);
                    program = qt_argbShaderProgram;
                    break;
                case QVideoFrame::Format_RGB565:
#ifdef QT_OPENGL_ES
                    initRgbTextureInfo(GL_UNSIGNED_SHORT_5_6_5, GL_RGB, imageSize);
#else
                    initRgbTextureInfo(GL_RGB16, GL_RGB, imageSize);
#endif
                    program = qt_rgbShaderProgram;
                    break;
                case QVideoFrame::Format_YV12:
                    initYv12TextureInfo(imageSize);
                    program = qt_yuvPlanarShaderProgram;
                    break;
                case QVideoFrame::Format_YUV420P:
                    initYuv420PTextureInfo(imageSize);
                    program = qt_yuvPlanarShaderProgram;
                    break;
                default:
                    break;
                }
            } else if (format.handleType() == QAbstractVideoBuffer::GLTextureHandle) {
                switch (format.pixelFormat()) {
                case QVideoFrame::Format_RGB32:
                case QVideoFrame::Format_RGB565:
                    m_textureCount = 1;
                    program = qt_rgbShaderProgram;
                    break;
                case QVideoFrame::Format_ARGB32:
                    m_textureCount = 1;
                    program = qt_rgbShaderProgram;
                    break;
                default:
                    break;
                }
            }

            if (m_textureCount > 0) {
                Q_ASSERT(program);

                glGenProgramsARB(1, &m_shaderId);
                glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, m_shaderId);
                glProgramStringARB(
                        GL_FRAGMENT_PROGRAM_ARB,
                        GL_PROGRAM_FORMAT_ASCII_ARB,
                        qstrlen(program),
                        reinterpret_cast<const GLvoid *>(program));

                if (glGetError() == GL_NO_ERROR) {
                    if (format.handleType() == QAbstractVideoBuffer::NoHandle)
                        glGenTextures(m_textureCount, m_textureIds);

                    m_handleType = format.handleType();
                    m_pixelFormat = format.pixelFormat();
                    m_imageFormat = QImage::Format_Invalid;
                    m_imageSize = imageSize;
                    m_sourceRect = format.viewport();
                    m_colorMatrixDirty = true;
                    m_ready = true;

                    return QAbstractVideoSurface::start(format);
                } else {
                    const GLubyte* errorString = glGetString(GL_PROGRAM_ERROR_STRING_ARB);

                    qWarning("QPainterVideoSurface: Shader compile error %s",
                             reinterpret_cast<const char *>(errorString));
                    glDeleteProgramsARB(1, &m_shaderId);

                    m_textureCount = 0;
                    m_shaderId = 0;

                    setError(ResourceError);
                }
            }
            doneCurrent();
        }
    #endif
        const QImage::Format imageFormat = QVideoFrame::equivalentImageFormat(format.pixelFormat());

        if (imageFormat == QImage::Format_Invalid
                || format.handleType() != QAbstractVideoBuffer::NoHandle) {
            setError(UnsupportedFormatError);
        } else {
            m_handleType = format.handleType();
            m_pixelFormat = format.pixelFormat();
            m_imageFormat = imageFormat;
            m_imageSize = imageSize;
            m_sourceRect = format.viewport();
            m_ready = true;

            return QAbstractVideoSurface::start(format);
        }
    }
    if (isStarted()) {
        m_handleType = QAbstractVideoBuffer::NoHandle;
        m_pixelFormat = QVideoFrame::Format_Invalid;
        m_imageFormat = QImage::Format_Invalid;
        m_imageSize = QSize();
        m_sourceRect = QRect();
        m_ready = false;

        QAbstractVideoSurface::stop();
    }
    return false;
}

/*!
*/
void QPainterVideoSurface::stop()
{
#ifndef QT_NO_OPENGL
    if (m_textureCount > 0) {
        makeCurrent();

        if (m_handleType != QAbstractVideoBuffer::GLTextureHandle)
            glDeleteTextures(m_textureCount, m_textureIds);
        glDeleteProgramsARB(1, &m_shaderId);

        m_textureCount = 0;
        m_shaderId = 0;

        doneCurrent();
    }
#endif

    m_frame = QVideoFrame();
    m_handleType = QAbstractVideoBuffer::NoHandle;
    m_pixelFormat = QVideoFrame::Format_Invalid;
    m_imageFormat = QImage::Format_Invalid;
    m_imageSize = QSize();
    m_sourceRect = QRect();
    m_ready = false;

    QAbstractVideoSurface::stop();
}

/*!
*/
bool QPainterVideoSurface::present(const QVideoFrame &frame)
{
    if (!m_ready) {
        if (!isStarted())
            setError(StoppedError);

        return false;
    } else if (frame.pixelFormat() != m_pixelFormat || frame.size() != m_imageSize) {
        setError(IncorrectFormatError);
        stop();

        return false;
    } else {
#ifndef QT_NO_OPENGL
        if (m_textureCount > 0) {          
            QVideoFrame frameCopy(frame);

            if (m_handleType == QAbstractVideoBuffer::GLTextureHandle) {
                m_textureIds[0] = frame.handle().toInt();
            } else if (frameCopy.map(QAbstractVideoBuffer::ReadOnly)) {
                makeCurrent();

                for (int i = 0; i < m_textureCount; ++i) {
                    glBindTexture(GL_TEXTURE_2D, m_textureIds[i]);
                    glTexImage2D(
                            GL_TEXTURE_2D,
                            0,
                            m_textureInternalFormat,
                            m_textureWidths[i],
                            m_textureHeights[i],
                            0,
                            m_textureFormat,
                            GL_UNSIGNED_BYTE,
                            frame.bits() + m_textureOffsets[i]);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                }
                frameCopy.unmap();

                doneCurrent();
            } else {
                setError(ResourceError);
                stop();

                return false;
            }
        }
#endif
        m_frame = frame;
        m_ready = false;

       emit frameChanged();

       return true;
    }
}

/*!
    \fn QPainterVideoSurface::brightness() const
*/

/*!
*/
void QPainterVideoSurface::setBrightness(int brightness)
{
    m_brightness = brightness;

    m_colorMatrixDirty = true;
}

/*!
    \fn QPainterVideoSurface::contrast() const
*/

/*!
*/
void QPainterVideoSurface::setContrast(int contrast)
{
    m_contrast = contrast;

    m_colorMatrixDirty = true;
}

/*!
    \fn QPainterVideoSurface::hue() const
*/

/*!
*/
void QPainterVideoSurface::setHue(int hue)
{
    m_hue = hue;

    m_colorMatrixDirty = true;
}

/*!
    \fn QPainterVideoSurface::saturation() const
*/

/*!
*/
void QPainterVideoSurface::setSaturation(int saturation)
{
    m_saturation = saturation;

    m_colorMatrixDirty = true;
}

/*!
    \fn QPainterVideoSurface::isReady() const
*/

/*!
*/
void QPainterVideoSurface::setReady(bool ready)
{
    m_ready = ready;
}

/*!
*/
void QPainterVideoSurface::paint(QPainter *painter, const QRect &rect)
{
#ifndef QT_NO_OPENGL
    if (m_textureCount > 0 && m_frame.isValid()) {
        painter->beginNativePainting();

        if (m_colorMatrixDirty)
            updateColorMatrix();

        float txLeft = float(m_sourceRect.left()) / float(m_imageSize.width());
        float txRight = float(m_sourceRect.right()) / float(m_imageSize.width());
        float txTop = float(m_sourceRect.top()) / float(m_imageSize.height());
        float txBottom = float(m_sourceRect.bottom()) / float(m_imageSize.height());

        const float tx_array[] =
        {
            txLeft , txBottom,
            txRight, txBottom,
            txLeft , txTop,
            txRight, txTop
        };
        const float v_array[] =
        {
            rect.left()     , rect.bottom() + 1,
            rect.right() + 1, rect.bottom() + 1,
            rect.left()     , rect.top(),
            rect.right() + 1, rect.top()
        };

        glEnable(GL_FRAGMENT_PROGRAM_ARB);
        glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, m_shaderId);

        glProgramLocalParameter4fARB(
                GL_FRAGMENT_PROGRAM_ARB,
                0,
                m_colorMatrix(0, 0),
                m_colorMatrix(0, 1),
                m_colorMatrix(0, 2),
                m_colorMatrix(0, 3));
        glProgramLocalParameter4fARB(
                GL_FRAGMENT_PROGRAM_ARB,
                1,
                m_colorMatrix(1, 0),
                m_colorMatrix(1, 1),
                m_colorMatrix(1, 2),
                m_colorMatrix(1, 3));
        glProgramLocalParameter4fARB(
                GL_FRAGMENT_PROGRAM_ARB,
                2,
                m_colorMatrix(2, 0),
                m_colorMatrix(2, 1),
                m_colorMatrix(2, 2),
                m_colorMatrix(2, 3));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);

        if (m_textureCount > 1) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_textureIds[1]);

            if (m_textureCount > 2) {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, m_textureIds[2]);
            }
            glActiveTexture(GL_TEXTURE0);
        }

        glVertexPointer(2, GL_FLOAT, 0, v_array);
        glTexCoordPointer(2, GL_FLOAT, 0, tx_array);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisable(GL_FRAGMENT_PROGRAM_ARB);

        painter->endNativePainting();
    } else
#endif
    if (m_frame.map(QAbstractVideoBuffer::ReadOnly)) {
        QImage image(
                m_frame.bits(),
                m_imageSize.width(),
                m_imageSize.height(),
                m_frame.bytesPerLine(),
                m_imageFormat);

        painter->drawImage(rect, image, m_sourceRect);
    }
}

/*!
    \fn QPainterVideoSurface::frameChanged()
*/

#ifndef QT_NO_OPENGL

void QPainterVideoSurface::initRgbTextureInfo(
        GLenum internalFormat, GLuint format, const QSize &size)
{
    m_textureInternalFormat = internalFormat;
    m_textureFormat = format;
    m_textureCount = 1;
    m_textureWidths[0] = size.width();
    m_textureHeights[0] = size.height();
    m_textureOffsets[0] = 0;
}

void QPainterVideoSurface::initYuv420PTextureInfo(const QSize &size)
{
    m_textureInternalFormat = GL_LUMINANCE;
    m_textureFormat = GL_LUMINANCE;
    m_textureCount = 3;
    m_textureWidths[0] = size.width();
    m_textureHeights[0] = size.height();
    m_textureOffsets[0] = 0;
    m_textureWidths[1] = size.width() / 2;
    m_textureHeights[1] = size.height() / 2;
    m_textureOffsets[1] = size.width() * size.height();
    m_textureWidths[2] = size.width() / 2;
    m_textureHeights[2] = size.height() / 2;
    m_textureOffsets[2] = size.width() * size.height() * 5 / 4;
}

void QPainterVideoSurface::initYv12TextureInfo(const QSize &size)
{
    m_textureInternalFormat = GL_LUMINANCE;
    m_textureFormat = GL_LUMINANCE;
    m_textureCount = 3;
    m_textureWidths[0] = size.width();
    m_textureHeights[0] = size.height();
    m_textureOffsets[0] = 0;
    m_textureWidths[1] = size.width() / 2;
    m_textureHeights[1] = size.height() / 2;
    m_textureOffsets[1] = size.width() * size.height() * 5 / 4;
    m_textureWidths[2] = size.width() / 2;
    m_textureHeights[2] = size.height() / 2;
    m_textureOffsets[2] = size.width() * size.height();
}

void QPainterVideoSurface::updateColorMatrix()
{
    const qreal b = m_brightness / 200.0;
    const qreal c = m_contrast / 100.0 + 1.0;
    const qreal h = m_hue / 200.0;
    const qreal s = m_saturation / 100.0 + 1.0;

    const qreal cosH = qCos(M_PI * h);
    const qreal sinH = qSin(M_PI * h);

    const qreal h11 = -0.4728 * cosH + 0.7954 * sinH + 1.4728;
    const qreal h21 = -0.9253 * cosH - 0.0118 * sinH + 0.9523;
    const qreal h31 =  0.4525 * cosH + 0.8072 * sinH - 0.4524;

    const qreal h12 =  1.4728 * cosH - 1.3728 * sinH - 1.4728;
    const qreal h22 =  1.9253 * cosH + 0.5891 * sinH - 0.9253;
    const qreal h32 = -0.4525 * cosH - 1.9619 * sinH + 0.4525;

    const qreal h13 =  1.4728 * cosH - 0.2181 * sinH - 1.4728;
    const qreal h23 =  0.9253 * cosH + 1.1665 * sinH - 0.9253;
    const qreal h33 =  0.5475 * cosH - 1.3846 * sinH + 0.4525;

    const qreal sr = (1.0 - s) * 0.3086;
    const qreal sg = (1.0 - s) * 0.6094;
    const qreal sb = (1.0 - s) * 0.0820;

    const qreal sr_s = sr + s;
    const qreal sg_s = sg + s;
    const qreal sb_s = sr + s;

    const float m4 = (s + sr + sg + sb) * (0.5 - 0.5 * c + b);

    m_colorMatrix(0, 0) = c * (sr_s * h11 + sg * h21 + sb * h31);
    m_colorMatrix(0, 1) = c * (sr_s * h12 + sg * h22 + sb * h32);
    m_colorMatrix(0, 2) = c * (sr_s * h13 + sg * h23 + sb * h33);
    m_colorMatrix(0, 3) = m4;

    m_colorMatrix(1, 0) = c * (sr * h11 + sg_s * h21 + sb * h31);
    m_colorMatrix(1, 1) = c * (sr * h12 + sg_s * h22 + sb * h32);
    m_colorMatrix(1, 2) = c * (sr * h13 + sg_s * h23 + sb * h33);
    m_colorMatrix(1, 3) = m4;

    m_colorMatrix(2, 0) = c * (sr * h11 + sg * h21 + sb_s * h31);
    m_colorMatrix(2, 1) = c * (sr * h12 + sg * h22 + sb_s * h32);
    m_colorMatrix(2, 2) = c * (sr * h13 + sg * h23 + sb_s * h33);
    m_colorMatrix(2, 3) = m4;

    m_colorMatrix(3, 0) = 0.0;
    m_colorMatrix(3, 1) = 0.0;
    m_colorMatrix(3, 2) = 0.0;
    m_colorMatrix(3, 3) = 1.0;

    switch (m_pixelFormat) {
    case QVideoFrame::Format_YUV420P:
    case QVideoFrame::Format_YV12:
        m_colorMatrix = m_colorMatrix * QMatrix4x4(
                1.0,  0.000,  1.140, -0.5700,
                1.0, -0.394, -0.581,  0.4875,
                1.0,  2.028,  0.000, -1.0140,
                0.0,  0.000,  0.000,  1.0000);
        break;
    default:
        break;
    }

    m_colorMatrixDirty = false;
}

#endif
