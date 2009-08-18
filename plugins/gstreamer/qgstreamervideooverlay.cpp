/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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

#include "qgstreamervideooverlay.h"
#include "qvideosurfacegstsink.h"

#include <QtMultimedia/qvideosurfaceformat.h>

#include "qx11videosurface.h"

QGstreamerVideoOverlay::QGstreamerVideoOverlay(QObject *parent)
    : QVideoWindowControl(parent)
    , m_surface(new QX11VideoSurface)
    , m_videoSink(reinterpret_cast<GstElement*>(QVideoSurfaceGstSink::createSink(m_surface)))
{
    if (m_videoSink) {
        gst_object_ref(GST_OBJECT(m_videoSink)); //Take ownership
        gst_object_sink(GST_OBJECT(m_videoSink));
    }

    connect(m_surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)),
            this, SLOT(surfaceFormatChanged()));
}

QGstreamerVideoOverlay::~QGstreamerVideoOverlay()
{
    if (m_videoSink)
        gst_object_unref(GST_OBJECT(m_videoSink));

    delete m_surface;
}

void QGstreamerVideoOverlay::setWinId(WId id)
{
    m_surface->setWinId(id);

    QVideoWindowControl::setWinId(id);
}

void QGstreamerVideoOverlay::setDisplayRect(const QRect &rect)
{
    m_surface->setDisplayRect(rect);

    QVideoWindowControl::setDisplayRect(rect);
}

void QGstreamerVideoOverlay::setBrightness(int brightness)
{
    m_surface->setBrightness(brightness);

    QVideoWindowControl::setBrightness(brightness);
}

void QGstreamerVideoOverlay::setContrast(int contrast)
{
    m_surface->setContrast(contrast);

    QVideoWindowControl::setContrast(contrast);
}

void QGstreamerVideoOverlay::setHue(int hue)
{
    m_surface->setHue(hue);

    QVideoWindowControl::setHue(hue);
}

void QGstreamerVideoOverlay::setSaturation(int saturation)
{
    m_surface->setSaturation(saturation);

    QVideoWindowControl::setSaturation(saturation);
}

void QGstreamerVideoOverlay::setFullscreen(bool fullscreen)
{
    QVideoWindowControl::setFullscreen(fullscreen);
}

QSize QGstreamerVideoOverlay::nativeSize() const
{
    return m_surface->surfaceFormat().sizeHint();
}

QAbstractVideoSurface *QGstreamerVideoOverlay::surface() const
{
    return m_surface;
}

GstElement *QGstreamerVideoOverlay::videoSink()
{
    return m_videoSink;
}

void QGstreamerVideoOverlay::surfaceFormatChanged()
{
    QVideoWindowControl::setBrightness(m_surface->brightness());
    QVideoWindowControl::setContrast(m_surface->contrast());
    QVideoWindowControl::setHue(m_surface->hue());
    QVideoWindowControl::setSaturation(m_surface->saturation());

    emit nativeSizeChanged();
}
