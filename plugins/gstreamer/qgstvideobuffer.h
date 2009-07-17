#ifndef QGSTVIDEOBUFFER_H
#define QGSTVIDEOBUFFER_H

#ifndef QT_NO_VIDEOSURFACE

#include <QtMultimedia/QAbstractVideoBuffer>

#include <gst/gst.h>

QT_BEGIN_NAMESPACE

class QGstVideoBuffer : public QAbstractVideoBuffer
{
public:
    QGstVideoBuffer(GstBuffer *buffer);
    ~QGstVideoBuffer();

    uchar *data() const;

private:
    GstBuffer *m_buffer;
};


QT_END_NAMESPACE

#endif

#endif
