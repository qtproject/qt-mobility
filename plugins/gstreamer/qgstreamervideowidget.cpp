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

#include "qgstreamervideowidget.h"

#include <QEvent>
#include <QApplication>

#include <X11/Xlib.h>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <gst/interfaces/propertyprobe.h>

QGstreamerVideoWidgetControl::QGstreamerVideoWidgetControl(QObject *parent)
    : QVideoWidgetControl(parent)
    , m_videoSink(0)
    , m_widget(new QWidget)
{   
    m_widget->installEventFilter(this);

    m_videoSink = gst_element_factory_make ("xvimagesink", NULL);
    if (m_videoSink) {
        // Check if the xv sink is usable
        if (gst_element_set_state(m_videoSink, GST_STATE_READY) != GST_STATE_CHANGE_SUCCESS) {
            gst_object_unref(GST_OBJECT(m_videoSink));
            m_videoSink = 0;
        } else {
            gst_element_set_state(m_videoSink, GST_STATE_NULL);

            g_object_set(G_OBJECT(m_videoSink), "force-aspect-ratio", 1, (const char*)NULL);
        }
    }

    if (!m_videoSink)
        m_videoSink = gst_element_factory_make ("ximagesink", NULL);

    gst_object_ref (GST_OBJECT (m_videoSink)); //Take ownership
    gst_object_sink (GST_OBJECT (m_videoSink));

    setOverlay();
}

QGstreamerVideoWidgetControl::~QGstreamerVideoWidgetControl()
{
    if (m_videoSink)
        gst_object_unref(GST_OBJECT(m_videoSink));

    delete m_widget;
}

GstElement *QGstreamerVideoWidgetControl::videoSink()
{
    return m_videoSink;
}

bool QGstreamerVideoWidgetControl::eventFilter(QObject *object, QEvent *e)
{
    if (object == m_widget) {
        if (e->type() == QEvent::Show) {
            // Setting these values ensures smooth resizing since it
            // will prevent the system from clearing the background
            //setAttribute(Qt::WA_NoSystemBackground, true);
            m_widget->setAttribute(Qt::WA_PaintOnScreen, true);
            setOverlay();
        } else if (e->type() == QEvent::Resize) {
            // This is a workaround for missing background repaints
            // when reducing window size
            windowExposed();
        }
    }

    return false;
}

void QGstreamerVideoWidgetControl::setOverlay()
{
    if (m_videoSink && GST_IS_X_OVERLAY(m_videoSink)) {
        WId windowId = m_widget->winId();
        // Even if we have created a winId at this point, other X applications
        // need to be aware of it.
        QApplication::syncX();
        gst_x_overlay_set_xwindow_id ( GST_X_OVERLAY(m_videoSink) ,  windowId );
    }
    windowExposed();
}

void QGstreamerVideoWidgetControl::windowExposed()
{
    QApplication::syncX();
    if (m_videoSink && GST_IS_X_OVERLAY(m_videoSink))
        gst_x_overlay_expose(GST_X_OVERLAY(m_videoSink));
}

QWidget *QGstreamerVideoWidgetControl::videoWidget()
{
    return m_widget;
}

QVideoWidget::AspectRatio QGstreamerVideoWidgetControl::aspectRatio() const
{
    return m_aspectRatioMode;
}

QSize QGstreamerVideoWidgetControl::customAspectRatio() const
{
    return m_customAspectRatio;
}

void QGstreamerVideoWidgetControl::setAspectRatio(QVideoWidget::AspectRatio ratio)
{
    if (m_videoSink) {
        g_object_set(G_OBJECT(m_videoSink),
                     "force-aspect-ratio",
                     (ratio == QVideoWidget::AspectRatioAuto),
                     (const char*)NULL);
    }

    m_aspectRatioMode = ratio;
}

void QGstreamerVideoWidgetControl::setCustomAspectRatio(const QSize &ratio)
{
    m_customAspectRatio = ratio;
}

bool QGstreamerVideoWidgetControl::isFullscreen() const
{
    return false;
}

void QGstreamerVideoWidgetControl::setFullscreen(bool fullscreen)
{
}

int QGstreamerVideoWidgetControl::brightness() const
{
    int brightness = 0;

    g_object_get(G_OBJECT(m_videoSink), "brightness", &brightness, NULL);

    return brightness * 10;
}

void QGstreamerVideoWidgetControl::setBrightness(int brightness)
{
    g_object_set(G_OBJECT(m_videoSink), "brightness", brightness * 10, NULL);

    emit brightnessChanged(brightness);
}

int QGstreamerVideoWidgetControl::contrast() const
{
    int contrast = 0;

    g_object_get(G_OBJECT(m_videoSink), "contrast", &contrast, NULL);

    return contrast * 10;
}

void QGstreamerVideoWidgetControl::setContrast(int contrast)
{
    g_object_set(G_OBJECT(m_videoSink), "contrast", contrast * 10, NULL);

    emit contrastChanged(contrast);
}

int QGstreamerVideoWidgetControl::hue() const
{
    int hue = 0;

    g_object_get(G_OBJECT(m_videoSink), "hue", &hue, NULL);

    return hue * 10;
}

void QGstreamerVideoWidgetControl::setHue(int hue)
{
    g_object_set(G_OBJECT(m_videoSink), "hue", hue * 10, NULL);

    emit hueChanged(hue);
}

int QGstreamerVideoWidgetControl::saturation() const
{
    int saturation = 0;

    g_object_get(G_OBJECT(m_videoSink), "saturation", &saturation, NULL);

    return saturation * 10;
}

void QGstreamerVideoWidgetControl::setSaturation(int saturation)
{
    g_object_set(G_OBJECT(m_videoSink), "saturation", saturation * 10, NULL);

    emit saturationChanged(saturation);
}
