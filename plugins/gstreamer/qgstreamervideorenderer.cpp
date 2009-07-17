#include "qgstreamervideorenderer.h"
#include "qvideosurfacegstsink.h"

#ifndef QT_NO_VIDEOSURFACE

#include <QEvent>
#include <QApplication>

#include <gst/gst.h>

QGstreamerVideoRenderer::QGstreamerVideoRenderer(QObject *parent)
    :QVideoRendererEndpoint(parent),m_videoSink(0)
{


}

QGstreamerVideoRenderer::~QGstreamerVideoRenderer()
{
    if (m_videoSink)
        gst_object_unref(GST_OBJECT(m_videoSink));
}

GstElement *QGstreamerVideoRenderer::videoSink()
{
    if (!m_videoSink) {
        m_videoSink = reinterpret_cast<GstElement*>(QVideoSurfaceGstSink::createSink(surface()));
        gst_object_ref(GST_OBJECT(m_videoSink)); //Take ownership
        gst_object_sink(GST_OBJECT(m_videoSink));
    }

    return m_videoSink;
}

#endif
