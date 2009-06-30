#include "qpaintervideosurface_p.h"

#ifndef QT_NO_VIDEOSURFACE

#include <qpainter.h>

/*!
    \class QPainterVideoSurface
    \internal
*/

/*!
*/
QPainterVideoSurface::QPainterVideoSurface(QObject *parent)
    : QAbstractVideoSurface(parent)
    , m_frameType(QVideoFrame::InvalidType)
    , m_imageFormat(QImage::Format_Invalid)
    ,  m_ready(false)
{
    setSupportedTypes(QList<QVideoFrame::Type>()
            << QVideoFrame::Image_RGB32
            << QVideoFrame::Image_ARGB32
            << QVideoFrame::Image_ARGB32_Premultiplied
            << QVideoFrame::Image_RGB565
            << QVideoFrame::Image_RGB555);
}

/*!
*/
QPainterVideoSurface::~QPainterVideoSurface()
{
}

/*!
*/
bool QPainterVideoSurface::isFormatSupported(const QVideoFormat &format, QVideoFormat *similar)
{
    Q_UNUSED(similar);

    QImage::Format imageFormat = QVideoFrame::imageFormatFromType(format.frameType());
    QSize imageSize = format.frameSize();

    return imageFormat != QImage::Format_Invalid && !imageSize.isEmpty();
}

/*!
*/
bool QPainterVideoSurface::start(const QVideoFormat &format)
{
    QImage::Format imageFormat = QVideoFrame::imageFormatFromType(format.frameType());
    QSize imageSize = format.frameSize();

    if (imageFormat == QImage::Format_Invalid || imageSize.isEmpty()) {
        setError(UnsupportedFormatError);

        return false;
    } else {
        m_frame = QVideoFrame();
        m_imageFormat = imageFormat;
        m_imageSize = imageSize;
        m_sourceRect = format.viewport();
        m_ready = true;

        setFormat(format);
        setStarted(true);

        return true;
    }
}

/*!
*/
void QPainterVideoSurface::stop()
{
    m_frame = QVideoFrame();
    m_imageFormat = QImage::Format_Invalid;
    m_imageSize = QSize();
    m_sourceRect = QRect();
    m_ready = false;

    setFormat(QVideoFormat());
    setStarted(false);
}

/*!
*/
bool QPainterVideoSurface::present(const QVideoFrame &frame)
{
    if (!m_ready) {
        setError(isStarted() ? StoppedError : NotReadyError);

        return false;
    } else if (frame.type() != m_frameType || frame.size() != m_imageSize) {
        setError(IncorrectFormatError);

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
    if (!m_frame.isNull()) {
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

#endif
