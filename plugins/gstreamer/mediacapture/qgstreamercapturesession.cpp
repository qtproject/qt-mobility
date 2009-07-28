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
#include "qgstreamercapturecontrol.h"
#include "qgstreameraudioencode.h"
#include "qgstreamervideoencode.h"
#include "qgstreamerbushelper.h"
#include "qmediastreams.h"
#include "qmediacapture.h"

#include <QDebug>
#include <QUrl>
#include <QSet>
#include <QCoreApplication>

#define gstRef(element) { gst_object_ref(GST_OBJECT(element)); gst_object_sink(GST_OBJECT(element)); }
#define gstUnref(element) gst_object_unref(GST_OBJECT(element))

QGstreamerCaptureSession::QGstreamerCaptureSession(QGstreamerCaptureSession::CaptureMode captureMode, QObject *parent)
    :QObject(parent),
     m_state(StoppedState),
     m_pipelineMode(EmptyPipeline),
     m_captureMode(captureMode)
{
    m_pipeline = gst_pipeline_new("media-capture-pipeline");
    gstRef(m_pipeline);

    m_bus = gst_element_get_bus(m_pipeline);
    m_busHelper = new QGstreamerBusHelper(m_bus, this);
    connect(m_busHelper, SIGNAL(message(QGstreamerMessage)), SLOT(busMessage(QGstreamerMessage)));

    buildEncodeBin();

    if (m_captureMode & Audio) {
        m_audioSrc = gst_element_factory_make("osssrc", "audio_src");
        gstRef(m_audioSrc);

        m_audioTee = gst_element_factory_make("tee", "audio_tee");
        gstRef(m_audioTee);
        m_audioPreview = gst_element_factory_make("fakesink", "audio-fake-sink");
        gstRef(m_audioPreview);

        m_audioPreviewPad = gst_element_get_request_pad(m_audioTee, "src%d");
        gstRef(m_audioPreviewPad);
        m_audioCapturePad = gst_element_get_request_pad(m_audioTee, "src%d");
        gstRef(m_audioCapturePad);

        m_audioPreviewQueue = gst_element_factory_make("queue", "audio_preview_queue");
        gstRef(m_audioPreviewQueue);

        m_audioEncodeQueue = gst_element_factory_make("queue", "audio_encode_queue");
        gstRef(m_audioEncodeQueue);
    }

    m_captureControl = new QGstreamerCaptureControl(this);

    setState(PreviewState);
}

QGstreamerCaptureSession::~QGstreamerCaptureSession()
{
    if (m_encodeBin) {
        delete m_busHelper;
        gst_object_unref(GST_OBJECT(m_encodeBin));
        gst_object_unref(GST_OBJECT(m_audioConvert));
        gst_object_unref(GST_OBJECT(m_muxer));
        gst_object_unref(GST_OBJECT(m_fileSink));
    }
}

void QGstreamerCaptureSession::buildEncodeBin()
{
    m_encodeBin = gst_bin_new("encode-bin");
    gstRef(m_encodeBin);

    m_muxer = gst_element_factory_make("matroskamux", "muxer");
    gstRef(m_muxer);
    m_fileSink = gst_element_factory_make("filesink", "filesink");
    gstRef(m_fileSink);
    g_object_set(G_OBJECT(m_fileSink), "location", "tmp.mkv", NULL);

    m_audioEncodeControl = 0;
    m_videoEncodeControl = 0;

    gst_bin_add_many(GST_BIN(m_encodeBin), m_muxer, m_fileSink,  NULL);
    gst_element_link(m_muxer, m_fileSink);

    if (m_captureMode & Audio) {
        m_audioConvert = gst_element_factory_make("audioconvert", "audioconvert");
        gstRef(m_audioConvert);
        m_audioQueue = gst_element_factory_make("queue", "audio-queue");
        gstRef(m_audioQueue);
        m_volume = gst_element_factory_make("volume", "volume");
        gstRef(m_volume);
        m_audioEncodeControl = new QGstreamerAudioEncode(this);

        gst_bin_add_many(GST_BIN(m_encodeBin), m_audioConvert, /*m_audioQueue,*/ m_volume, m_audioEncodeControl->encoder(),  NULL);

        gst_element_link_many(m_audioConvert, /*m_audioQueue,*/ m_volume, m_audioEncodeControl->encoder(), m_muxer, NULL);
        g_object_set(G_OBJECT(m_volume), "volume", 10.0, NULL);


        // add ghostpads
        GstPad *pad = gst_element_get_static_pad(m_audioConvert, "sink");
        Q_ASSERT(pad);
        gst_element_add_pad(GST_ELEMENT(m_encodeBin), gst_ghost_pad_new("audiosink", pad));
        gst_object_unref(GST_OBJECT(pad));
    }

    /*if (m_captureMode & Video) {
        m_videoqueue = gst_element_factory_make("queue", "video-queue");
        m_fakevideosink = gst_element_factory_make("fakesink", "video-fake-sink");

        m_colorconvert = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace");
        m_videoEncodeControl = new QGstreamerVideoEncode(this);

        gst_bin_add_many(GST_BIN(m_encodeBin), m_colorconvert, m_videoqueue, m_videoEncodeControl->encoder(),  NULL);
        gst_element_link_many(m_colorconvert, m_videoqueue, m_videoEncodeControl->encoder(), m_muxer, NULL);

        // add ghostpads
        GstPad *pad = gst_element_get_static_pad(m_colorconvert, "sink");
        gst_element_add_pad(GST_ELEMENT(m_encodeBin), gst_ghost_pad_new("videosink", pad));
        gst_object_unref(GST_OBJECT(pad));
    }*/
}

void QGstreamerCaptureSession::rebuildGraph(QGstreamerCaptureSession::PipelineMode newMode)
{
    switch (m_pipelineMode) {
        case EmptyPipeline:
            break;
        case PreviewPipeline:
            gst_element_unlink(m_audioSrc, m_audioPreview);
            gst_bin_remove_many(GST_BIN(m_pipeline), m_audioSrc, m_audioPreview, NULL);
            break;
        case RecordingPipeline:
            gst_element_unlink(m_audioSrc, m_encodeBin);
            gst_bin_remove_many(GST_BIN(m_pipeline), m_audioSrc, m_encodeBin, NULL);
            break;
        case PreviewAndRecordingPipeline:
            gst_element_unlink(m_audioSrc, m_audioTee);
            gst_element_unlink(m_audioTee, m_audioPreviewQueue);
            gst_element_unlink(m_audioPreviewQueue, m_audioPreview);
            gst_element_unlink(m_audioTee, m_audioEncodeQueue);
            gst_element_unlink(m_audioEncodeQueue, m_encodeBin);
            gst_bin_remove_many(GST_BIN(m_pipeline), m_audioSrc, m_audioTee, m_audioPreviewQueue,
                                m_audioPreview, m_audioEncodeQueue, m_encodeBin, NULL);
            break;
    }

    gstUnref(m_audioSrc);
    m_audioSrc = gst_element_factory_make("osssrc", "audio_src");
    gstRef(m_audioSrc);

    switch (newMode) {
        case EmptyPipeline:
            break;
        case PreviewPipeline:
            gst_bin_add_many(GST_BIN(m_pipeline), m_audioSrc, m_audioPreview, NULL);
            gst_element_link(m_audioSrc, m_audioPreview);
            break;
        case RecordingPipeline:
            gst_bin_add_many(GST_BIN(m_pipeline), m_audioSrc, m_encodeBin, NULL);
            m_audioEncodeControl->applyOptions();
            gst_element_link(m_audioSrc, m_encodeBin);
            break;
        case PreviewAndRecordingPipeline:
            gst_bin_add_many(GST_BIN(m_pipeline), m_audioSrc, m_audioTee, m_audioPreviewQueue,
                                m_audioPreview, m_audioEncodeQueue, m_encodeBin, NULL);
            m_audioEncodeControl->applyOptions();
            gst_element_link(m_audioSrc, m_audioTee);
            gst_element_link(m_audioTee, m_audioPreviewQueue);
            gst_element_link(m_audioPreviewQueue, m_audioPreview);
            gst_element_link(m_audioTee, m_audioEncodeQueue);
            gst_element_link(m_audioEncodeQueue, m_encodeBin);
            break;
    }

    dumpGraph( QString("rebuild_graph_%1_%2").arg(m_pipelineMode).arg(newMode) );

    m_pipelineMode = newMode;
}

void QGstreamerCaptureSession::dumpGraph(const QString &fileName)
{
    _gst_debug_bin_to_dot_file(GST_BIN(m_pipeline), GST_DEBUG_GRAPH_SHOW_ALL, fileName.toAscii());
}

QMediaSink QGstreamerCaptureSession::sink() const
{
    return m_sink;
}

bool QGstreamerCaptureSession::setSink(const QMediaSink& sink)
{
    m_sink = sink;
    QUrl url = sink.dataLocation().toUrl();
    g_object_set(G_OBJECT(m_fileSink), "location", url.toString().toLocal8Bit().constData(), NULL);
    return true;
}

QGstreamerCaptureSession::State QGstreamerCaptureSession::state() const
{
    return m_state;
}

void QGstreamerCaptureSession::waitForStopped()
{
    GstState state = GST_STATE_PLAYING;
    gst_element_get_state(m_pipeline, &state, 0, 0);

    while (state!= GST_STATE_NULL) {
        qApp->processEvents();
        gst_element_get_state(m_pipeline, &state, 0, 0);
    }
}

void QGstreamerCaptureSession::setState(QGstreamerCaptureSession::State newState)
{
    if (newState == m_state)
        return;

    PipelineMode newMode = EmptyPipeline;

    switch (newState) {
        case PausedState:
        case RecordingState:
            newMode = PreviewAndRecordingPipeline;
            break;
        case PreviewState:
            newMode = PreviewPipeline;
            break;
        case StoppedState:
            newMode = EmptyPipeline;
            break;
    }

    if (newMode != m_pipelineMode) {
        gst_element_set_state(m_pipeline, GST_STATE_NULL);

        waitForStopped();
        rebuildGraph(newMode);
    }

    switch (newState) {
        case PausedState:
            gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
            break;
        case RecordingState:
        case PreviewState:
            gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
            break;
        case StoppedState:
            gst_element_set_state(m_pipeline, GST_STATE_NULL);
    }
    m_state = newState;
}


qint64 QGstreamerCaptureSession::position() const
{
    GstFormat   format = GST_FORMAT_TIME;
    gint64      position = 0;

    if ( m_encodeBin && gst_element_query_position(m_encodeBin, &format, &position))
        return position / 1000000;
    else
        return 0;
}

void QGstreamerCaptureSession::setCaptureDevice(const QString &deviceName)
{
    g_object_set(G_OBJECT(m_audioSrc), "device", deviceName.toLocal8Bit().constData(), NULL);
}

void QGstreamerCaptureSession::busMessage(const QGstreamerMessage &message)
{
    GstMessage* gm = message.rawMessage();

    if (gm) {
        if (GST_MESSAGE_SRC(gm) == GST_OBJECT_CAST(m_encodeBin)) {
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
                        if (m_state != StoppedState) {
                            emit stateChanged(m_state = StoppedState);
                            dumpGraph("stopped");
                        }
                        break;
                    case GST_STATE_PAUSED:
                        if (m_state != PausedState)
                            emit stateChanged(m_state = PausedState);
                        dumpGraph("paused");
                        break;
                    case GST_STATE_PLAYING:
                        {
                            State newState = m_pipelineMode == PreviewPipeline ? PreviewState : RecordingState;

                            if (m_state != newState)
                                emit stateChanged(m_state = newState);

                            if (m_pipelineMode == PreviewPipeline)
                                dumpGraph("preview");
                            else
                                dumpGraph("recording");
                        }
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
