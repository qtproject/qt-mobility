#include "qgstreamervideorenderer.h"
#include "qvideosurfacegstsink.h"

#ifndef QT_NO_VIDEOSURFACE

#include <QEvent>
#include <QApplication>

#include <gst/gst.h>

QGstreamerVideoRenderer::QGstreamerVideoRenderer(QObject *parent)
    :QVideoRendererControl(parent),m_videoSink(0)
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
        m_videoSink = reinterpret_cast<GstElement*>(QVideoSurfaceGstSink::createSink(m_surface));
        gst_object_ref(GST_OBJECT(m_videoSink)); //Take ownership
        gst_object_sink(GST_OBJECT(m_videoSink));
    }

    return m_videoSink;
}


QAbstractVideoSurface *QGstreamerVideoRenderer::surface() const
{
    return m_surface;
}

void QGstreamerVideoRenderer::setSurface(QAbstractVideoSurface *surface)
{
    m_surface = surface;
}


#endif
