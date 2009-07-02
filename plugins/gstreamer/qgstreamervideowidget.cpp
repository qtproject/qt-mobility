#include "qgstreamervideowidget.h"

#include <QEvent>
#include <QApplication>

#include <X11/Xlib.h>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <gst/interfaces/propertyprobe.h>

QGstreamerVideoWidget::QGstreamerVideoWidget(QWidget *parent)
    :QMediaWidgetEndpoint(parent),m_videoSink(0)
{
    m_videoSink = gst_element_factory_make ("xvimagesink", NULL);
    if (m_videoSink) {
        // Check if the xv sink is usable
        if (gst_element_set_state(m_videoSink, GST_STATE_READY) != GST_STATE_CHANGE_SUCCESS) {
            gst_object_unref(GST_OBJECT(m_videoSink));
            m_videoSink = 0;
        } else {
            // Note that this should not really be neccessary as these are
            // default values, though under certain conditions values are retained
            // even between application instances. (reproducible on 0.10.16/Gutsy)
            g_object_set(G_OBJECT(m_videoSink), "brightness", 0, (const char*)NULL);
            g_object_set(G_OBJECT(m_videoSink), "contrast", 0, (const char*)NULL);
            g_object_set(G_OBJECT(m_videoSink), "hue", 0, (const char*)NULL);
            g_object_set(G_OBJECT(m_videoSink), "saturation", 0, (const char*)NULL);
        }
    }

    if (!m_videoSink)
        m_videoSink = gst_element_factory_make ("ximagesink", NULL);

    gst_object_ref (GST_OBJECT (m_videoSink)); //Take ownership
    gst_object_sink (GST_OBJECT (m_videoSink));

    setOverlay();
}

QGstreamerVideoWidget::~QGstreamerVideoWidget()
{
}

GstElement *QGstreamerVideoWidget::videoSink()
{
    return m_videoSink;
}

bool QGstreamerVideoWidget::event(QEvent *e)
{
    if (e->type() == QEvent::Show) {
        // Setting these values ensures smooth resizing since it
        // will prevent the system from clearing the background
        setAttribute(Qt::WA_NoSystemBackground, true);
        setAttribute(Qt::WA_PaintOnScreen, true);
        setOverlay();
    } else if (e->type() == QEvent::Resize) {
        // This is a workaround for missing background repaints
        // when reducing window size
        windowExposed();
    }

    return QMediaWidgetEndpoint::event(e);
}

void QGstreamerVideoWidget::setOverlay()
{
    if (m_videoSink && GST_IS_X_OVERLAY(m_videoSink)) {
        WId windowId = winId();
        // Even if we have created a winId at this point, other X applications
        // need to be aware of it.
        QApplication::syncX();
        gst_x_overlay_set_xwindow_id ( GST_X_OVERLAY(m_videoSink) ,  windowId );
    }
    windowExposed();    
}

void QGstreamerVideoWidget::windowExposed()
{
    QApplication::syncX();
    if (m_videoSink && GST_IS_X_OVERLAY(m_videoSink))
        gst_x_overlay_expose(GST_X_OVERLAY(m_videoSink));
}

