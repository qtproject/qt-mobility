#include "qgstvideobuffer.h"

#ifndef QT_NO_VIDEOSURFACE

QGstVideoBuffer::QGstVideoBuffer(GstBuffer *buffer)
    : m_buffer(buffer)
{
    setCapacity(m_buffer->size);
    setSize(m_buffer->size);

    gst_buffer_ref(m_buffer);
}

QGstVideoBuffer::~QGstVideoBuffer()
{
    gst_buffer_unref(m_buffer);
}

uchar *QGstVideoBuffer::data() const
{
    return m_buffer->data;
}

#endif
