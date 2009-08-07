#include "qgstvideobuffer.h"

#ifndef QT_NO_VIDEOSURFACE

QGstVideoBuffer::QGstVideoBuffer(GstBuffer *buffer, int bytesPerLine)
    : QAbstractVideoBuffer(NoHandle)
    , m_buffer(buffer)
    , m_bytesPerLine(bytesPerLine)
    , m_mode(NotMapped)
{
    gst_buffer_ref(m_buffer);
}

QGstVideoBuffer::~QGstVideoBuffer()
{
    gst_buffer_unref(m_buffer);
}


QAbstractVideoBuffer::MapMode QGstVideoBuffer::mapMode() const
{
    return m_mode;
}

uchar *QGstVideoBuffer::map(MapMode mode, int *numBytes, int *bytesPerLine)
{
    if (mode != NotMapped && m_mode == NotMapped) {
        if (numBytes)
            *numBytes = m_buffer->size;

        if (bytesPerLine)
            *bytesPerLine = m_bytesPerLine;

        m_mode = mode;

        return m_buffer->data;
    } else {
        return 0;
    }
}
void QGstVideoBuffer::unmap()
{
    m_mode = NotMapped;
}

#endif
