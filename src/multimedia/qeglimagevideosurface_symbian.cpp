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

#include <QtCore/QDebug>
#include <QtCore/QVariant>
#include <QtGui/QPainter>
#include <qeglimagevideosurface_symbian_p.h>
#include <qvideosurfaceformat.h>

#ifndef QT_NO_OPENGL
#include <GLES/glext.h>
#include <QtOpenGL/QGLShaderProgram>
#endif

QT_BEGIN_NAMESPACE

//-----------------------------------------------------------------------------
// QEglImageVideoSurface
//-----------------------------------------------------------------------------

QEglImageVideoSurface::QEglImageVideoSurface(QObject *parent)
:   QAbstractVideoSurface(parent)
,   m_frameIsNew(false)
,   m_ready(false)
{

}

QEglImageVideoSurface::~QEglImageVideoSurface()
{
    if (isActive())
        stop();
}

QList<QVideoFrame::PixelFormat> QEglImageVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    TRACE("QEglImageVideoSurface::supportedPixelFormats" << qtThisPtr()
          << "handleType" << handleType);
    return QList<QVideoFrame::PixelFormat>()
               << QVideoFrame::Format_RGB32
               << QVideoFrame::Format_ARGB32;
}

bool QEglImageVideoSurface::start(const QVideoSurfaceFormat &format)
{
    TRACE("QEglImageVideoSurface::start" << qtThisPtr());
    bool result = false;
    if (isActive())
        stop();
    if (format.frameSize().isEmpty())
        setError(UnsupportedFormatError);
    else
        doStart(format);
    if (error() == QAbstractVideoSurface::NoError) {
        m_format = format;
        m_ready = true;
        result = QAbstractVideoSurface::start(format);
    } else {
        QAbstractVideoSurface::stop();
    }
    return result;
}

void QEglImageVideoSurface::stop()
{
    TRACE("QEglImageVideoSurface::stop" << qtThisPtr());
    if (isActive()) {
        doStop();
        m_format = QVideoSurfaceFormat();
        m_frame = QVideoFrame();
        m_ready = false;
        QAbstractVideoSurface::stop();
    }
}

bool QEglImageVideoSurface::present(const QVideoFrame &frame)
{
    VERBOSE_TRACE("QEglImageVideoSurface::present" << qtThisPtr()
                  << "active" << isActive()
                  << "ready" << m_ready
                  << "handleType" << frame.handleType()
                  << "handle" << frame.handle().value<uint>()
                  << "frame.isValid" << frame.isValid()
                  << "format.pixelFormat" << m_format.pixelFormat()
                  << "format.frameSize" << m_format.frameSize()
                  << "frame.pixelFormat" << frame.pixelFormat()
                  << "frame.Size" << frame.size());
    bool result = false;
    m_frame = frame;
    m_frameIsNew = true;
    if (!m_ready) {
        if (!isActive())
            setError(StoppedError);
    } else if (frame.isValid()
               && (frame.pixelFormat() != m_format.pixelFormat()
                   || frame.size() != m_format.frameSize())) {
        setError(IncorrectFormatError);
        qWarning() << "QEglImageVideoSurface::present received frame of incorrect format, stopping the surface";
        stop();
    } else {
        m_ready = false;
        emit frameChanged();
        result = true;
    }
    return result;
}

void QEglImageVideoSurface::viewportDestroyed()
{
    TRACE("QEglImageVideoSurface::viewportDestroyed" << qtThisPtr());
    onViewportDestroyed();
    doStop();
}

void QEglImageVideoSurface::setReady(bool ready)
{
    if (m_ready != ready) {
        VERBOSE_TRACE("QEglImageVideoSurface::setReady" << qtThisPtr()
                      << "ready" << ready
                      << "frame.isValid" << m_frame.isValid());
        m_ready = ready;
    }
}

void QEglImageVideoSurface::frameDisplayed()
{
    m_frameIsNew = false;
}

void QEglImageVideoSurface::onViewportDestroyed()
{
    // Default implementation does nothing
}

void QEglImageVideoSurface::doStart(const QVideoSurfaceFormat &format)
{
    TRACE("QEglImageVideoSurface::doStart" << qtThisPtr());
    Q_UNUSED(format)
    // Default implementation does nothing
}


//-----------------------------------------------------------------------------
// QEglImageGLVideoSurface
//-----------------------------------------------------------------------------

#ifndef QT_NO_OPENGL

static const char *qt_QEglImageGLVideoSurface_VertexShaderProgram =
    "attribute highp vec4 vertexCoordArray;\n"
    "attribute mediump vec2 textureCoordArray;\n"
    "uniform highp mat4 positionMatrix;\n"
    "varying mediump vec2 textureCoord;\n"
    "void main (void)\n"
    "{\n"
    "   gl_Position = positionMatrix * vertexCoordArray;\n"
    "   textureCoord = textureCoordArray;\n"
    "}";

static const char* qt_QEglImageGLVideoSurface_FragmentShaderProgram =
    "uniform sampler2D texRgb;\n"
    "varying mediump vec2 textureCoord;\n"
    "void main (void)\n"
    "{\n"
    "    gl_FragColor = texture2D(texRgb, textureCoord);\n"
    "}";

QEglImageGLVideoSurface::QEglImageGLVideoSurface(QObject *parent)
:   QEglImageVideoSurface(parent)
,   m_context(const_cast<QGLContext *>(QGLContext::currentContext()))
{
    TRACE("QEglImageGLVideoSurface::QEglImageGLVideoSurface" << qtThisPtr());
    Q_ASSERT(m_context);
}

QEglImageGLVideoSurface::~QEglImageGLVideoSurface()
{
    TRACE("QEglImageGLVideoSurface::~QEglImageGLVideoSurface" << qtThisPtr();)
}

void QEglImageGLVideoSurface::paint(QPainter *painter, const QRectF &sourceRect,
                                    const QRectF &targetRect)
{
    VERBOSE_TRACE("QEglImageGLVideoSurface::paint" << qtThisPtr()
                  << "sourceRect" << sourceRect
                  << "targetRect" << targetRect
                  << "isActive" << isActive()
                  << "frameIsValid" << m_frame.isValid());
    if (!isActive() || !m_frame.isValid()) {
        painter->fillRect(targetRect, QBrush(Qt::black));
    } else {
        const QRectF viewport = m_format.viewport();
        const QRectF source(viewport.x() + viewport.width() * sourceRect.x(),
                            viewport.y() + viewport.height() * sourceRect.y(),
                            viewport.width() * sourceRect.width(),
                            viewport.height() * sourceRect.height());

        const bool stencilTestEnabled = glIsEnabled(GL_STENCIL_TEST);
        const bool scissorTestEnabled = glIsEnabled(GL_SCISSOR_TEST);

        painter->beginNativePainting();

        if (stencilTestEnabled)
            glEnable(GL_STENCIL_TEST);
        if (scissorTestEnabled)
            glEnable(GL_SCISSOR_TEST);

        const int width = m_context->device()->width();
        const int height = m_context->device()->height();
        const GLfloat wfactor = 2.0 / width;
        const GLfloat hfactor = -2.0 / height;

        const QTransform transform = painter->deviceTransform();
        const GLfloat positionMatrix[4][4] =
        {
            {
                /*(0,0)*/ GLfloat(wfactor * transform.m11() - transform.m13()),
                /*(0,1)*/ GLfloat(hfactor * transform.m12() + transform.m13()),
                /*(0,2)*/ 0.0,
                /*(0,3)*/ GLfloat(transform.m13())
            }, {
                /*(1,0)*/ GLfloat(wfactor * transform.m21() - transform.m23()),
                /*(1,1)*/ GLfloat(hfactor * transform.m22() + transform.m23()),
                /*(1,2)*/ 0.0,
                /*(1,3)*/ GLfloat(transform.m23())
            }, {
                /*(2,0)*/ 0.0,
                /*(2,1)*/ 0.0,
                /*(2,2)*/ -1.0,
                /*(2,3)*/ 0.0
            }, {
                /*(3,0)*/ GLfloat(wfactor * transform.dx() - transform.m33()),
                /*(3,1)*/ GLfloat(hfactor * transform.dy() + transform.m33()),
                /*(3,2)*/ 0.0,
                /*(3,3)*/ GLfloat(transform.m33())
            }
        };

        QVideoSurfaceFormat::Direction scanLineDirection = m_format.scanLineDirection();
        const GLfloat vTop = targetRect.top();
        const GLfloat vBottom = targetRect.bottom() + 1;

        const GLfloat vertexCoordArray[] =
        {
            GLfloat(targetRect.left())     , GLfloat(vBottom),
            GLfloat(targetRect.right() + 1), GLfloat(vBottom),
            GLfloat(targetRect.left())     , GLfloat(vTop),
            GLfloat(targetRect.right() + 1), GLfloat(vTop)
        };

        const QSize frameSize = m_format.frameSize();
        GLfloat txLeft = sourceRect.left();
        GLfloat txRight = sourceRect.right();
        GLfloat txTop = scanLineDirection == QVideoSurfaceFormat::TopToBottom
                ? sourceRect.top()
                : sourceRect.bottom();
        GLfloat txBottom = scanLineDirection == QVideoSurfaceFormat::TopToBottom
                ? sourceRect.bottom()
                : sourceRect.top();

        const GLfloat textureCoordArray[] =
        {
            txLeft , txBottom,
            txRight, txBottom,
            txLeft , txTop,
            txRight, txTop
        };

        m_shaderProgram->bind();
        m_shaderProgram->enableAttributeArray("vertexCoordArray");
        m_shaderProgram->enableAttributeArray("textureCoordArray");
        m_shaderProgram->setAttributeArray("vertexCoordArray", vertexCoordArray, 2);
        m_shaderProgram->setAttributeArray("textureCoordArray", textureCoordArray, 2);
        m_shaderProgram->setUniformValue("positionMatrix", positionMatrix);
        m_shaderProgram->setUniformValue("texRgb", 0);

        glEnable(GL_TEXTURE_2D);
        m_frame.map(QAbstractVideoBuffer::ReadOnly);
        bindTexture();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        unbindTexture();
        m_frame.unmap();
        frameDisplayed();

        m_shaderProgram->release();
        painter->endNativePainting();
    }
}

void QEglImageGLVideoSurface::onViewportDestroyed()
{
    m_context = 0;
}

void QEglImageGLVideoSurface::doStart(const QVideoSurfaceFormat &format)
{
    TRACE("QEglImageGLVideoSurface::doStart" << qtThisPtr());
    m_context->makeCurrent();
    m_shaderProgram = new QGLShaderProgram(m_context, this);
    if (!m_shaderProgram->addShaderFromSourceCode(QGLShader::Vertex,
                                                  qt_QEglImageGLVideoSurface_VertexShaderProgram)) {
        qWarning("QEglImageGLVideoSurface::doStart vertex shader compile error %s",
                 qPrintable(m_shaderProgram->log()));
        setError(QAbstractVideoSurface::ResourceError);
    } else if (!m_shaderProgram->addShaderFromSourceCode(QGLShader::Fragment,
                                                         qt_QEglImageGLVideoSurface_FragmentShaderProgram)) {
        qWarning("QEglImageGLVideoSurface::doStart vertex shader compile error %s",
                 qPrintable(m_shaderProgram->log()));
        setError(QAbstractVideoSurface::ResourceError);
    } else {
        m_shaderProgram->bindAttributeLocation("textureCoordArray", 1);
        if (!m_shaderProgram->link()) {
            qWarning("QEglImageGLVideoSurface::doStart shader link error %s",
                     qPrintable(m_shaderProgram->log()));
            m_shaderProgram->removeAllShaders();
            setError(QAbstractVideoSurface::ResourceError);
        }
    }
    if (error() != QAbstractVideoSurface::NoError) {
        delete m_shaderProgram;
        m_shaderProgram = 0;
    }
}

void QEglImageGLVideoSurface::doStop()
{
    if (m_context) {
        m_context->makeCurrent();
        m_shaderProgram->removeAllShaders();
    }
    delete m_shaderProgram;
    m_shaderProgram = 0;
}

void QEglImageGLVideoSurface::bindTexture()
{
    const GLuint texture = m_frame.handle().value<GLuint>();
    VERBOSE_TRACE("QEglImageGLVideoSurface::bindTexture" << qtThisPtr()
                  << "texture" << texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void QEglImageGLVideoSurface::unbindTexture()
{
    VERBOSE_TRACE("QEglImageGLVideoSurface::unbindTexture" << qtThisPtr());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

#endif // !QT_NO_OPENGL


//-----------------------------------------------------------------------------
// QEglImageVGVideoSurface
//-----------------------------------------------------------------------------

#ifndef QT_NO_OPENVG

QEglImageVGVideoSurface::QEglImageVGVideoSurface(QObject *parent)
:   QEglImageVideoSurface(parent)
{
    TRACE("QEglImageVGVideoSurface::QEglImageVGVideoSurface" << qtThisPtr());
}

QEglImageVGVideoSurface::~QEglImageVGVideoSurface()
{
    TRACE("QEglImageVGVideoSurface::~QEglImageVGVideoSurface" << qtThisPtr());
}

void QEglImageVGVideoSurface::paint(QPainter *painter, const QRectF &sourceRect,
                                    const QRectF &targetRect)
{
    VERBOSE_TRACE("QEglImageVGVideoSurface::paint" << qtThisPtr()
                  << "sourceRect" << sourceRect
                  << "targetRect" << targetRect
                  << "isActive" << isActive());
    if (!isActive() || !m_frame.isValid()) {
        painter->fillRect(targetRect, QBrush(Qt::black));
    } else {
        VERBOSE_TRACE("QEglImageVGVideoSurface::paint" << qtThisPtr());
        m_frame.map(QAbstractVideoBuffer::ReadOnly);
        const VGImage image = m_frame.handle().value<VGImage>();
        const int imageWidth = vgGetParameteri(image, VG_IMAGE_WIDTH);
        const int imageHeight = vgGetParameteri(image, VG_IMAGE_WIDTH);
        const QRectF subImageRect(sourceRect.left() * imageWidth,
                                  sourceRect.top() * imageHeight,
                                  sourceRect.width() * imageWidth,
                                  sourceRect.height() * imageHeight);
        const VGImage subImage = vgChildImage(image, subImageRect.left(),
                                                     subImageRect.top(),
                                                     subImageRect.width(),
                                                     subImageRect.height());
        const qreal scaleX = targetRect.width() / subImageRect.width();
        const qreal scaleY = targetRect.height() / subImageRect.height();
        const VGint scissor = vgGeti(VG_SCISSORING);
        painter->save();
        if (QVideoSurfaceFormat::BottomToTop == m_format.scanLineDirection()) {
            painter->translate(targetRect.bottomLeft());
            painter->scale(scaleX, -scaleY);
        } else {
            painter->translate(targetRect.topLeft());
            painter->scale(scaleX, scaleY);
        }
        painter->beginNativePainting();
        vgSeti(VG_SCISSORING, scissor);
        vgDrawImage(subImage);
        vgDestroyImage(subImage);
        m_frame.unmap();
        frameDisplayed();
        painter->endNativePainting();
        painter->restore();
    }
}

void QEglImageVGVideoSurface::doStop()
{
    // TODO: release resources
}

#endif // !QT_NO_OPENVG

QT_END_NAMESPACE
