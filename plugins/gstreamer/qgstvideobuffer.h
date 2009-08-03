#ifndef QGSTVIDEOBUFFER_H
#define QGSTVIDEOBUFFER_H

#ifndef QT_NO_VIDEOSURFACE

#include <QtMultimedia/QAbstractVideoBuffer>

#include <gst/gst.h>

QT_BEGIN_NAMESPACE

class QGstVideoBuffer : public QAbstractVideoBuffer
{
public:
    QGstVideoBuffer(GstBuffer *buffer, int bytesPerLine);
    ~QGstVideoBuffer();

    MapMode mapMode() const;

    uchar *map(MapMode mode, int *numBytes, int *bytesPerLine);
    void unmap();

private:
    GstBuffer *m_buffer;
    int m_bytesPerLine;
    MapMode m_mode;
};


QT_END_NAMESPACE

#endif

#endif
