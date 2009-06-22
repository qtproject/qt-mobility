#ifndef QPAINTERVIDEOSURFACE_P_H
#define QPAINTERVIDEOSURFACE_P_H

#ifndef QT_NO_VIDEOSURFACE

#include <QtCore/qsize.h>
#include <QtGui/qimage.h>
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideoframe.h>

class QPainterVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    QPainterVideoSurface(QObject *parent = 0);
    ~QPainterVideoSurface();

    bool isFormatSupported(const QVideoFormat &format, QVideoFormat *similar = 0);

    bool start(const QVideoFormat &format);
    void stop();

    bool present(const QVideoFrame &frame);

    bool isReady() const;
    void setReady(bool ready);

    void paint(QPainter *painter, const QRect &rect);

Q_SIGNALS:
    void frameChanged();

private:
    QVideoFrame m_frame;
    QVideoFrame::Type m_frameType;
    QImage::Format m_imageFormat;
    QSize m_imageSize;
    QRect m_sourceRect;
    bool m_ready;
};

#endif

#endif
