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

#include "qgstreamercapturesession.h"
#include "qgstreamerrecordercontrol.h"
#include "qgstreamermediaformatcontrol.h"
#include "qgstreameraudioencode.h"
#include "qgstreamervideoencode.h"
#include "qgstreamerbushelper.h"
#include "qmediastreams.h"
#include "qmediarecorder.h"

#include <QDebug>
#include <QUrl>
#include <QSet>
#include <QCoreApplication>

#define gstRef(element) { gst_object_ref(GST_OBJECT(element)); gst_object_sink(GST_OBJECT(element)); }
#define gstUnref(element) { if (element) { gst_object_unref(GST_OBJECT(element)); element = 0; } }

QGstreamerCaptureSession::QGstreamerCaptureSession(QGstreamerCaptureSession::CaptureMode captureMode, QObject *parent)
    :QObject(parent),
     m_state(StoppedState),
     m_pipelineMode(EmptyPipeline),
     m_captureMode(captureMode),
     m_audioInputFactory(0),
     m_audioPreviewFactory(0),
     m_videoInputFactory(0),
     m_videoPreviewFactory(0),
     m_audioSrc(0),
     m_audioTee(0),
     m_audioPreviewQueue(0),
     m_audioPreview(0),
     m_videoSrc(0),
     m_videoTee(0),
     m_videoPreviewQueue(0),
     m_videoPreview(0),
     m_encodeBin(0)
{
    m_pipeline = gst_pipeline_new("media-capture-pipeline");
    gstRef(m_pipeline);

    m_bus = gst_element_get_bus(m_pipeline);
    m_busHelper = new QGstreamerBusHelper(m_bus, this);
    connect(m_busHelper, SIGNAL(message(QGstreamerMessage)), SLOT(busMessage(QGstreamerMessage)));
    m_audioEncodeControl = new QGstreamerAudioEncode(this);
    m_videoEncodeControl = new QGstreamerVideoEncode(this);
    m_recorderControl = new QGstreamerRecorderControl(this);
    m_mediaFormatControl = new QGstreamerMediaFormatControl(this);

    setState(StoppedState);
}

QGstreamerCaptureSession::~QGstreamerCaptureSession()
{
    gst_object_unref(GST_OBJECT(m_pipeline));
}


GstElement *QGstreamerCaptureSession::buildEncodeBin()
{
    GstElement *encodeBin = gst_bin_new("encode-bin");

    GstElement *muxer = gst_element_factory_make( m_mediaFormatControl->format().toAscii(), "muxer");
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

    if (m_captureMode & Video) {
        GstElement *videoQueue = gst_element_factory_make("queue", "video-encode-queue");
        GstElement *colorspace = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace-encoder");
        GstElement *videoscale = gst_element_factory_make("videoscale","videoscale-encoder");

        GstElement *videoEncoder = m_videoEncodeControl->createEncoder();

        gst_bin_add_many(GST_BIN(encodeBin), videoQueue, colorspace, videoscale, videoEncoder, NULL);
        gst_element_link_many(videoQueue, colorspace, videoscale, videoEncoder, muxer, NULL);

        // add ghostpads
        GstPad *pad = gst_element_get_static_pad(videoQueue, "sink");
        Q_ASSERT(pad);
        gst_element_add_pad(GST_ELEMENT(encodeBin), gst_ghost_pad_new("videosink", pad));
        gst_object_unref(GST_OBJECT(pad));
    }

    return encodeBin;
}

GstElement *QGstreamerCaptureSession::buildAudioSrc()
{
    GstElement *audioSrc = 0;
    if (m_audioInputFactory)
        audioSrc = m_audioInputFactory->buildElement();
    else {
        QString elementName = "alsasrc";
        QString device;

        if (m_captureDevice.startsWith("alsa:")) {
            device = m_captureDevice.mid(QString("alsa:").length());
        } else if (m_captureDevice.startsWith("oss:")) {
            elementName = "osssrc";
            device = m_captureDevice.mid(QString("oss:").length());
        }

        audioSrc = gst_element_factory_make(elementName.toAscii().constData(), "audio_src");
        if (audioSrc && !device.isEmpty())
            g_object_set(G_OBJECT(audioSrc), "device", device.toLocal8Bit().constData(), NULL);
    }

    if (!audioSrc) {
        emit error(int(QMediaRecorder::ResourceError), tr("Could not create an audio source element"));
        audioSrc = gst_element_factory_make("fakesrc", NULL);
    }

    return audioSrc;
}

GstElement *QGstreamerCaptureSession::buildAudioPreview()
{
    GstElement *previewElement = 0;

    if (m_audioPreviewFactory) {
        previewElement = m_audioPreviewFactory->buildElement();
    } else {


#if 1
        previewElement = gst_element_factory_make("fakesink", "audio-preview");
#else
        GstElement *bin = gst_bin_new("audio-preview-bin");
        GstElement *visual = gst_element_factory_make("libvisual_lv_scope", "audio-preview");
        GstElement *sink = gst_element_factory_make("ximagesink", NULL);
        gst_bin_add_many(GST_BIN(bin), visual, sink,  NULL);
        gst_element_link_many(visual,sink, NULL);


        // add ghostpads
        GstPad *pad = gst_element_get_static_pad(visual, "sink");
        Q_ASSERT(pad);
        gst_element_add_pad(GST_ELEMENT(bin), gst_ghost_pad_new("audiosink", pad));
        gst_object_unref(GST_OBJECT(pad));

        previewElement = bin;
#endif
    }

    return previewElement;
}

GstElement *QGstreamerCaptureSession::buildVideoSrc()
{
    GstElement *videoSrc = 0;
    if (m_videoInputFactory) {
        videoSrc = m_videoInputFactory->buildElement();
    } else {
        videoSrc = gst_element_factory_make("videotestsrc", "video_test_src");
        //videoSrc = gst_element_factory_make("v4l2src", "video_test_src");
    }

    return videoSrc;
}

GstElement *QGstreamerCaptureSession::buildVideoPreview()
{
    GstElement *previewElement = 0;

    if (m_videoPreviewFactory) {
        previewElement = m_videoPreviewFactory->buildElement();
    } else {
#if 1
        previewElement = gst_element_factory_make("fakesink", "video-preview");
#else
        GstElement *bin = gst_bin_new("video-preview-bin");
        GstElement *colorspace = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace-preview");
        GstElement *preview = gst_element_factory_make("ximagesink", "video-preview");
        gst_bin_add_many(GST_BIN(bin), colorspace, preview,  NULL);
        gst_element_link(colorspace,preview);

        // add ghostpads
        GstPad *pad = gst_element_get_static_pad(colorspace, "sink");
        Q_ASSERT(pad);
        gst_element_add_pad(GST_ELEMENT(bin), gst_ghost_pad_new("videosink", pad));
        gst_object_unref(GST_OBJECT(pad));

        previewElement = bin;
#endif
    }

    return previewElement;
}

#define REMOVE_ELEMENT(element) { if (element) {gst_bin_remove(GST_BIN(m_pipeline), element); element = 0;} }

void QGstreamerCaptureSession::rebuildGraph(QGstreamerCaptureSession::PipelineMode newMode)
{
    REMOVE_ELEMENT(m_audioSrc);
    REMOVE_ELEMENT(m_audioPreview);
    REMOVE_ELEMENT(m_audioPreviewQueue);
    REMOVE_ELEMENT(m_audioTee);
    REMOVE_ELEMENT(m_videoSrc);
    REMOVE_ELEMENT(m_videoPreview);
    REMOVE_ELEMENT(m_videoPreviewQueue);
    REMOVE_ELEMENT(m_videoTee);
    REMOVE_ELEMENT(m_encodeBin);

    bool ok = true;

    switch (newMode) {
        case EmptyPipeline:
            break;
        case PreviewPipeline:
            if (m_captureMode & Audio) {
                m_audioSrc = buildAudioSrc();
                m_audioPreview = buildAudioPreview();
                if (m_audioSrc && m_audioPreview) {
                    gst_bin_add_many(GST_BIN(m_pipeline), m_audioSrc, m_audioPreview, NULL);
                    ok &= gst_element_link(m_audioSrc, m_audioPreview);
                }
            }
            if (m_captureMode & Video) {
                m_videoSrc = buildVideoSrc();
                m_videoPreview = buildVideoPreview();
                if (m_videoSrc && m_videoPreview) {
                    gst_bin_add_many(GST_BIN(m_pipeline), m_videoSrc, m_videoPreview, NULL);
                    ok &= gst_element_link(m_videoSrc, m_videoPreview);
                }
            }
            break;
        case RecordingPipeline:
            m_encodeBin = buildEncodeBin();
            gst_bin_add(GST_BIN(m_pipeline), m_encodeBin);

            if (m_captureMode & Audio) {
                m_audioSrc = buildAudioSrc();
                gst_bin_add(GST_BIN(m_pipeline), m_audioSrc);
                ok &= gst_element_link(m_audioSrc, m_encodeBin);
            }

            if (m_captureMode & Video) {
                m_videoSrc = buildVideoSrc();
                gst_bin_add(GST_BIN(m_pipeline), m_videoSrc);
                ok &= gst_element_link(m_videoSrc, m_encodeBin);
            }

            break;
        case PreviewAndRecordingPipeline:
            m_encodeBin = buildEncodeBin();
            gst_bin_add(GST_BIN(m_pipeline), m_encodeBin);

            if (m_captureMode & Audio) {
                m_audioSrc = buildAudioSrc();
                m_audioPreview = buildAudioPreview();
                m_audioTee = gst_element_factory_make("tee", NULL);
                m_audioPreviewQueue = gst_element_factory_make("queue", NULL);
                gst_bin_add_many(GST_BIN(m_pipeline), m_audioSrc, m_audioTee,
                                 m_audioPreviewQueue, m_audioPreview, NULL);
                ok &= gst_element_link(m_audioSrc, m_audioTee);
                ok &= gst_element_link(m_audioTee, m_audioPreviewQueue);
                ok &= gst_element_link(m_audioPreviewQueue, m_audioPreview);
                ok &= gst_element_link(m_audioTee, m_encodeBin);
            }

            if (m_captureMode & Video) {
                m_videoSrc = buildVideoSrc();
                m_videoPreview = buildVideoPreview();
                m_videoTee = gst_element_factory_make("tee", NULL);
                m_videoPreviewQueue = gst_element_factory_make("queue", NULL);
                gst_bin_add_many(GST_BIN(m_pipeline), m_videoSrc, m_videoTee,
                                 m_videoPreviewQueue, m_videoPreview, NULL);
                ok &= gst_element_link(m_videoSrc, m_videoTee);
                ok &= gst_element_link(m_videoTee, m_videoPreviewQueue);
                ok &= gst_element_link(m_videoPreviewQueue, m_videoPreview);
                ok &= gst_element_link(m_videoTee, m_encodeBin);
            }

            break;
    }

    if (!ok) {
        emit error(int(QMediaRecorder::FormatError),tr("Failed to build media capture pipeline."));
    }

    dumpGraph( QString("rebuild_graph_%1_%2").arg(m_pipelineMode).arg(newMode) );
    if (m_encodeBin) {
        QString fileName = QString("rebuild_graph_encode_%1_%2").arg(m_pipelineMode).arg(newMode);
        _gst_debug_bin_to_dot_file(GST_BIN(m_encodeBin), GST_DEBUG_GRAPH_SHOW_ALL, fileName.toAscii());
    }

    m_pipelineMode = newMode;
}

void QGstreamerCaptureSession::dumpGraph(const QString &fileName)
{
    _gst_debug_bin_to_dot_file(GST_BIN(m_pipeline),
                               GstDebugGraphDetails(/*GST_DEBUG_GRAPH_SHOW_ALL |*/ GST_DEBUG_GRAPH_SHOW_MEDIA_TYPE | GST_DEBUG_GRAPH_SHOW_NON_DEFAULT_PARAMS | GST_DEBUG_GRAPH_SHOW_STATES),
                               fileName.toAscii());
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

void QGstreamerCaptureSession::setAudioInput(QGstreamerElementFactory *audioInput)
{
    m_audioInputFactory = audioInput;
}

void QGstreamerCaptureSession::setAudioPreview(QGstreamerElementFactory *audioPreview)
{
    m_audioPreviewFactory = audioPreview;
}

void QGstreamerCaptureSession::setVideoInput(QGstreamerElementFactory *videoInput)
{
    m_videoInputFactory = videoInput;
}

void QGstreamerCaptureSession::setVideoPreview(QGstreamerElementFactory *videoPreview)
{
    m_videoPreviewFactory = videoPreview;
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

        //TODO: get rid of this, process async.
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


qint64 QGstreamerCaptureSession::duration() const
{
    GstFormat   format = GST_FORMAT_TIME;
    gint64      duration = 0;

    if ( m_encodeBin && gst_element_query_position(m_encodeBin, &format, &duration))
        return duration / 1000000;
    else
        return 0;
}

void QGstreamerCaptureSession::setCaptureDevice(const QString &deviceName)
{
    m_captureDevice = deviceName;
}

void QGstreamerCaptureSession::enablePreview(bool enabled)
{
    m_previewEnabled = enabled;
    if (enabled) {
        if (state() == StoppedState)
            setState(PreviewState);
    } else {
        if (state() == PreviewState)
            setState(StoppedState);
    }
}

void QGstreamerCaptureSession::busMessage(const QGstreamerMessage &message)
{
    GstMessage* gm = message.rawMessage();

    if (gm) {
        if (GST_MESSAGE_TYPE(gm) == GST_MESSAGE_ERROR) {
            GError *err;
            gchar *debug;
            gst_message_parse_error (gm, &err, &debug);
            emit error(int(QMediaRecorder::ResourceError),QString::fromUtf8(err->message));
            g_error_free (err);
            g_free (debug);
        }

        if (GST_MESSAGE_TYPE(gm) == GST_MESSAGE_ELEMENT &&
            gst_structure_has_name(gm->structure, "prepare-xwindow-id"))
        {
            if (m_audioPreviewFactory)
                m_audioPreviewFactory->prepareWinId();

            if (m_videoPreviewFactory)
                m_videoPreviewFactory->prepareWinId();
        }

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
            default:
                break;
            }
        }
    }
}
