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
    m_audioEncodeControl = new QGstreamerAudioEncode(this);
    m_videoEncodeControl = new QGstreamerVideoEncode(this);
    m_captureControl = new QGstreamerCaptureControl(this);

    setState(PreviewState);
}

QGstreamerCaptureSession::~QGstreamerCaptureSession()
{
    gst_object_unref(GST_OBJECT(m_pipeline));
}


GstElement *QGstreamerCaptureSession::buildEncodeBin()
{
    GstElement *encodeBin = gst_bin_new("encode-bin");

    //m_muxer = gst_element_factory_make("matroskamux", "muxer");
    GstElement *muxer = gst_element_factory_make("oggmux", "muxer");
    GstElement *fileSink = gst_element_factory_make("filesink", "filesink");

    QUrl url = m_sink.dataLocation().toUrl();
    g_object_set(G_OBJECT(fileSink), "location", url.toString().toLocal8Bit().constData(), NULL);

    gst_bin_add_many(GST_BIN(encodeBin), muxer, fileSink,  NULL);
    gst_element_link(muxer, fileSink);

    if (m_captureMode & Audio) {
        GstElement *audioConvert = gst_element_factory_make("audioconvert", "audioconvert");
        GstElement *audioQueue = gst_element_factory_make("queue", "audio-encode-queue");
        GstElement *volume = gst_element_factory_make("volume", "volume");
        GstElement *audioEncoder = m_audioEncodeControl->createEncoder();

        gst_bin_add_many(GST_BIN(encodeBin), audioConvert, audioQueue, volume, audioEncoder,  NULL);

        gst_element_link_many(audioConvert, audioQueue, volume, audioEncoder, muxer, NULL);
        g_object_set(G_OBJECT(volume), "volume", 10.0, NULL);

        // add ghostpads
        GstPad *pad = gst_element_get_static_pad(audioConvert, "sink");
        Q_ASSERT(pad);
        gst_element_add_pad(GST_ELEMENT(encodeBin), gst_ghost_pad_new("audiosink", pad));
        gst_object_unref(GST_OBJECT(pad));
    }

    return encodeBin;
}

GstElement *QGstreamerCaptureSession::buildAudioSrcBin()
{
    //GstBin *bin = gst_bin_new("audio-src-bin");
    GstElement *audioSrc = gst_element_factory_make("osssrc", "audio_src");

    if (!m_captureDevice.isEmpty())
        g_object_set(G_OBJECT(audioSrc), "device", m_captureDevice.toLocal8Bit().constData(), NULL);

    return audioSrc;
}

GstElement *QGstreamerCaptureSession::buildAudioPreviewBin()
{
    GstElement *bin = gst_bin_new("audio-preview-bin");
    GstElement *audioQueue = gst_element_factory_make("queue", "audio-preview-queue");
    GstElement *preview = gst_element_factory_make("fakesink", "audio-preview");
    gst_bin_add_many(GST_BIN(bin), audioQueue, preview,  NULL);
    gst_element_link(audioQueue,preview);

    // add ghostpads
    GstPad *pad = gst_element_get_static_pad(audioQueue, "sink");
    Q_ASSERT(pad);
    gst_element_add_pad(GST_ELEMENT(bin), gst_ghost_pad_new("audiosink", pad));
    gst_object_unref(GST_OBJECT(pad));

    return bin;
}


void QGstreamerCaptureSession::rebuildGraph(QGstreamerCaptureSession::PipelineMode newMode)
{
    switch (m_pipelineMode) {
        case EmptyPipeline:
            break;
        case PreviewPipeline:
            gst_element_unlink(m_audioSrc, m_audioPreview);
            gst_bin_remove_many(GST_BIN(m_pipeline), m_audioSrc, m_audioPreview, NULL);
            gstUnref(m_audioSrc);
            gstUnref(m_audioPreview);
            break;
        case RecordingPipeline:
            gst_element_unlink(m_audioSrc, m_encodeBin);
            gst_bin_remove_many(GST_BIN(m_pipeline), m_audioSrc, m_encodeBin, NULL);
            gstUnref(m_audioSrc);
            gstUnref(m_encodeBin);
            break;
        case PreviewAndRecordingPipeline:
            gst_element_unlink(m_audioSrc, m_audioTee);
            gst_element_unlink(m_audioTee, m_audioPreview);
            gst_element_unlink(m_audioTee, m_encodeBin);
            gst_bin_remove_many(GST_BIN(m_pipeline), m_audioSrc, m_audioTee,
                                m_audioPreview, m_encodeBin, NULL);
            gstUnref(m_audioSrc);
            gstUnref(m_encodeBin);
            gstUnref(m_audioTee);
            gstUnref(m_audioPreview);
            break;
    }

    bool ok = true;

    switch (newMode) {
        case EmptyPipeline:
            break;
        case PreviewPipeline:
            m_audioSrc = buildAudioSrcBin();
            m_audioPreview = buildAudioPreviewBin();
            gst_bin_add_many(GST_BIN(m_pipeline), m_audioSrc, m_audioPreview, NULL);
            ok &= gst_element_link(m_audioSrc, m_audioPreview);
            break;
        case RecordingPipeline:
            m_audioSrc = buildAudioSrcBin();
            m_encodeBin = buildEncodeBin();
            gst_bin_add_many(GST_BIN(m_pipeline), m_audioSrc, m_encodeBin, NULL);
            ok &= gst_element_link(m_audioSrc, m_encodeBin);
            break;
        case PreviewAndRecordingPipeline:
            m_audioSrc = buildAudioSrcBin();
            m_encodeBin = buildEncodeBin();
            m_audioPreview = buildAudioPreviewBin();
            m_audioTee = gst_element_factory_make("tee", NULL);
            gst_bin_add_many(GST_BIN(m_pipeline), m_audioSrc, m_audioTee,
                                m_audioPreview, m_encodeBin, NULL);
            ok &= gst_element_link(m_audioSrc, m_audioTee);
            ok &= gst_element_link(m_audioTee, m_audioPreview);
            ok &= gst_element_link(m_audioTee, m_encodeBin);
            break;
    }

    if (!ok) {
        emit error(int(QMediaCapture::FormatError),tr("Failed to build media capture pipeline."));
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
    m_captureDevice = deviceName;
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
