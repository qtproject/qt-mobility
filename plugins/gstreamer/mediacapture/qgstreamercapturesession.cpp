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
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgstreamercapturesession.h"
#include "qgstreamerplayersession.h"
#include "qgstreameraudioencode.h"
#include "qgstreamerbushelper.h"
#include "qmediastreams.h"
#include "qmediacapture.h"

#include <QDebug>

QGstreamerCaptureSession::QGstreamerCaptureSession(QObject *parent)
    :QMediaCaptureControl(parent),
     m_state(QMediaCapture::StoppedState)
{
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        gst_init(NULL, NULL);
    }

    m_audioEncodeControl = new QGstreamerAudioEncode(this);

    m_audiosrc = gst_element_factory_make("osssrc", "audiosrc");
    m_tee = gst_element_factory_make("tee", "tee");
    m_audioconvert1 = gst_element_factory_make("audioconvert", "audioconvert1");
    m_volume = gst_element_factory_make("volume", "volume");
    m_encoder = m_audioEncodeControl->encoder();// gst_element_factory_make("vorbisenc", "encoder");
    //m_muxer = gst_element_factory_make("oggmux", "muxer");
    m_filesink = gst_element_factory_make("filesink", "filesink");

    //m_audioconvert2 = gst_element_factory_make("audioconvert", "audioconvert2");
    //m_fakesink = gst_element_factory_make("fakesink", "fakesink");

    m_pipeline = gst_pipeline_new("audio-capture-pipeline");

    if ( m_audiosrc && m_audioconvert1 && m_volume && m_tee && m_encoder && m_filesink && m_pipeline ) {
        gst_bin_add_many(GST_BIN(m_pipeline), m_audiosrc, m_audioconvert1, m_tee, m_volume,
                         m_encoder, m_filesink,  NULL);

        gst_element_link_many(m_audiosrc, m_audioconvert1, m_volume,
                              m_encoder, m_filesink, NULL);

        m_bus = gst_element_get_bus(m_pipeline);
        m_busHelper = new QGstreamerBusHelper(m_bus, this);
        connect(m_busHelper, SIGNAL(message(QGstreamerMessage)), SLOT(busMessage(QGstreamerMessage)));

        g_object_set(G_OBJECT(m_volume), "volume", 10.0, NULL);
    }
}


QGstreamerCaptureSession::~QGstreamerCaptureSession()
{
    stop();

    if (m_pipeline) {
        delete m_busHelper;
        gst_object_unref(GST_OBJECT(m_bus));
        gst_object_unref(GST_OBJECT(m_pipeline));
        gst_object_unref(GST_OBJECT(m_audiosrc));
        gst_object_unref(GST_OBJECT(m_audioconvert1));
        gst_object_unref(GST_OBJECT(m_tee));
        gst_object_unref(GST_OBJECT(m_audioconvert2));
        //gst_object_unref(GST_OBJECT(m_encoder));
        //gst_object_unref(GST_OBJECT(m_muxer));
        gst_object_unref(GST_OBJECT(m_filesink));
    }
}

QMediaSink QGstreamerCaptureSession::sink() const
{
    return m_sink;
}

bool QGstreamerCaptureSession::setSink(const QMediaSink& sink)
{
    m_sink = sink;
    QUrl url = sink.dataLocation().toUrl();
    g_object_set(G_OBJECT(m_filesink), "location", url.toString().toLocal8Bit().constData(), NULL);
    return true;
}

int QGstreamerCaptureSession::state() const
{
    return int(m_state);
}

qint64 QGstreamerCaptureSession::position() const
{
    return 0;
}

void QGstreamerCaptureSession::setPositionUpdatePeriod(int ms)
{
    Q_UNUSED(ms);
}

void QGstreamerCaptureSession::record()
{
    if (m_pipeline) {
        if (gst_element_set_state(m_pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
            m_state = QMediaCapture::StoppedState;
            emit stateChanged(m_state);
        }
    }
}

void QGstreamerCaptureSession::pause()
{
    if (m_pipeline)
        gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
}

void QGstreamerCaptureSession::stop()
{
    if (m_pipeline)
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
}

void QGstreamerCaptureSession::setCaptureDevice(const QString &deviceName)
{
    g_object_set(G_OBJECT(m_audiosrc), "device", deviceName.toLocal8Bit().constData(), NULL);
}

void QGstreamerCaptureSession::busMessage(const QGstreamerMessage &message)
{
    GstMessage* gm = message.rawMessage();

    if (gm) {
        if (GST_MESSAGE_SRC(gm) == GST_OBJECT_CAST(m_pipeline)) {
            switch (GST_MESSAGE_TYPE(gm))  {
            case GST_MESSAGE_DURATION:
                break;

            case GST_MESSAGE_STATE_CHANGED:
                {
                    GstState    oldState;
                    GstState    newState;
                    GstState    pending;

                    gst_message_parse_state_changed(gm, &oldState, &newState, &pending);

                    switch (newState) {
                    case GST_STATE_VOID_PENDING:
                    case GST_STATE_NULL:
                    case GST_STATE_READY:
                        if (m_state != QMediaCapture::StoppedState) {
                            emit stateChanged(m_state = QMediaCapture::StoppedState);
                        }
                        break;
                    case GST_STATE_PAUSED:
                        if (m_state != QMediaCapture::PausedState)
                            emit stateChanged(m_state = QMediaCapture::PausedState);
                        break;
                    case GST_STATE_PLAYING:                        
                        if (m_state != QMediaCapture::RecordingState)
                            emit stateChanged(m_state = QMediaCapture::RecordingState);
                        break;
                    }
                }
                break;
            case GST_MESSAGE_ERROR:
                {
                    GError *err;
                    gchar *debug;
                    gst_message_parse_error (gm, &err, &debug);
                    emit error(int(QMediaCapture::ResourceError),QString::fromUtf8(err->message));
                    g_error_free (err);
                    g_free (debug);
                }
                break;


            default:
                break;
            }
        }
    }
}
