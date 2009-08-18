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

#include "qpaintervideosurface_p.h"

#include <qpainter.h>
#include <qvideosurfaceformat.h>

/*!
    \class QPainterVideoSurface
    \internal
*/

/*!
*/
QPainterVideoSurface::QPainterVideoSurface(QObject *parent)
    : QAbstractVideoSurface(parent)
    , m_pixelFormat(QVideoFrame::Format_Invalid)
    , m_imageFormat(QImage::Format_Invalid)
    , m_ready(false)
{
}

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
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
            << QVideoFrame::Format_RGB32
            << QVideoFrame::Format_ARGB32
            << QVideoFrame::Format_ARGB32_Premultiplied
            << QVideoFrame::Format_RGB565
            << QVideoFrame::Format_RGB555;
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}

/*!
*/
bool QPainterVideoSurface::isFormatSupported(
        const QVideoSurfaceFormat &format, QVideoSurfaceFormat *similar)
{
    Q_UNUSED(similar);

    QImage::Format imageFormat = QVideoFrame::equivalentImageFormat(format.pixelFormat());
    QSize imageSize = format.frameSize();

    return imageFormat != QImage::Format_Invalid && !imageSize.isEmpty();
}

/*!
*/
bool QPainterVideoSurface::start(const QVideoSurfaceFormat &format)
{
    QImage::Format imageFormat = QVideoFrame::equivalentImageFormat(format.pixelFormat());
    QSize imageSize = format.frameSize();

    if (imageFormat == QImage::Format_Invalid || imageSize.isEmpty()) {
        setError(UnsupportedFormatError);

        return false;
    } else {
        m_frame = QVideoFrame();
        m_pixelFormat = format.pixelFormat();
        m_imageFormat = imageFormat;
        m_imageSize = imageSize;
        m_sourceRect = format.viewport();
        m_ready = true;

        return QAbstractVideoSurface::start(format);
    }
}

/*!
*/
void QPainterVideoSurface::stop()
{
    m_frame = QVideoFrame();
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
        m_frame = frame;
        m_ready = false;

       emit frameChanged();

       return true;
    }
}

/*!
*/
bool QPainterVideoSurface::isReady() const
{
    return m_ready;
}

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

