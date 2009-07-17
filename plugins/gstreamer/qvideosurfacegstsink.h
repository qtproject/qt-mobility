/*  This file is part of the KDE project.

    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

    This library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 2.1 or 3 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VIDEOSURFACEGSTSINK_H
#define VIDEOSURFACEGSTSINK_H

#ifndef QT_NO_VIDEOSURFACE

#include <gst/video/gstvideosink.h>

#include <QtCore/qlist.h>
#include <QtCore/qmutex.h>
#include <QtCore/qpointer.h>
#include <QtCore/qwaitcondition.h>
#include <QtMultimedia/qvideoformat.h>
#include <QtMultimedia/qvideoframe.h>

QT_BEGIN_NAMESPACE
class QAbstractVideoSurface;

class QVideoSurfaceGstDelegate : public QObject
{
    Q_OBJECT
public:
    QVideoSurfaceGstDelegate(QAbstractVideoSurface *surface);

    bool start(const QVideoFormat &format, int bytesPerLine);
    void stop();

    GstFlowReturn render(GstBuffer *buffer);

private slots:
    void queuedStart();
    void queuedStop();
    void queuedRender();

private:
    QPointer<QAbstractVideoSurface> m_surface;
    QMutex m_mutex;
    QWaitCondition m_setupCondition;
    QWaitCondition m_renderCondition;
    QVideoFormat m_format;
    QVideoFrame m_frame;
    GstFlowReturn m_renderReturn;
    int m_bytesPerLine;
    bool m_started;
};

class QVideoSurfaceGstSink
{
public:
    GstVideoSink parent;

    static QVideoSurfaceGstSink *createSink(QAbstractVideoSurface *surface);

private:
    static GType get_type();
    static void class_init(gpointer g_class, gpointer class_data);
    static void base_init(gpointer g_class);
    static void instance_init(GTypeInstance *instance, gpointer g_class);

    static void finalize(GObject *object);

    static GstStateChangeReturn change_state(GstElement *element, GstStateChange transition);

    static GstCaps *get_caps(GstBaseSink *sink);
    static gboolean set_caps(GstBaseSink *sink, GstCaps *caps);

    static GstFlowReturn buffer_alloc(
            GstBaseSink *sink, guint64 offset, guint size, GstCaps *caps, GstBuffer **buffer);

    static gboolean start(GstBaseSink *sink);
    static gboolean stop(GstBaseSink *sink);

    static gboolean unlock(GstBaseSink *sink);

    static gboolean event(GstBaseSink *sink, GstEvent *event);
    static GstFlowReturn preroll(GstBaseSink *sink, GstBuffer *buffer);
    static GstFlowReturn render(GstBaseSink *sink, GstBuffer *buffer);

private:
    QVideoSurfaceGstDelegate *delegate;
    GstCaps *supportedCaps;
};


class QVideoSurfaceGstSinkClass
{
public:
    GstVideoSinkClass parent_class;
};


QT_END_NAMESPACE

#endif

#endif
