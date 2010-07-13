/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgstreamercapturesession.h"
#include "qgstreamerrecordercontrol.h"
#include "qgstreamermediacontainercontrol.h"
#include "qgstreameraudioencode.h"
#include "qgstreamervideoencode.h"
#include "qgstreamerbushelper.h"
#include <qmediarecorder.h>

#include <gst/gsttagsetter.h>
#include <gst/gstversion.h>

#include <QtCore/qdebug.h>
#include <QtCore/qurl.h>
#include <QtCore/qset.h>
#include <QCoreApplication>
#include <QtCore/qmetaobject.h>
#include <QtCore/qfile.h>

#include <QtGui/qimage.h>

#define gstRef(element) { gst_object_ref(GST_OBJECT(element)); gst_object_sink(GST_OBJECT(element)); }
#define gstUnref(element) { if (element) { gst_object_unref(GST_OBJECT(element)); element = 0; } }

QGstreamerCaptureSession::QGstreamerCaptureSession(QGstreamerCaptureSession::CaptureMode captureMode, QObject *parent)
    :QObject(parent),
     m_state(StoppedState),
     m_pendingState(StoppedState),
     m_waitingForEos(false),
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
     m_imageCaptureBin(0),
     m_encodeBin(0),
     m_passImage(false),
     m_passPrerollImage(false)
{
    m_pipeline = gst_pipeline_new("media-capture-pipeline");
    gstRef(m_pipeline);

    m_bus = gst_element_get_bus(m_pipeline);
    m_busHelper = new QGstreamerBusHelper(m_bus, this);
    m_busHelper->installSyncEventFilter(this);
    connect(m_busHelper, SIGNAL(message(QGstreamerMessage)), SLOT(busMessage(QGstreamerMessage)));
    m_audioEncodeControl = new QGstreamerAudioEncode(this);
    m_videoEncodeControl = new QGstreamerVideoEncode(this);    
    m_recorderControl = new QGstreamerRecorderControl(this);
    m_mediaContainerControl = new QGstreamerMediaContainerControl(this);

    setState(StoppedState);
}

QGstreamerCaptureSession::~QGstreamerCaptureSession()
{
    setState(StoppedState);
    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(m_pipeline));
}

GstElement *QGstreamerCaptureSession::buildEncodeBin()
{
    bool ok = true;

    GstElement *encodeBin = gst_bin_new("encode-bin");

    GstElement *muxer = gst_element_factory_make( m_mediaContainerControl->formatElementName().constData(), "muxer");
    if (!muxer) {
        gst_object_unref(encodeBin);
        encodeBin = 0;
        return 0;
    }

    GstElement *fileSink = gst_element_factory_make("filesink", "filesink");

    g_object_set(G_OBJECT(fileSink), "location", m_sink.toString().toLocal8Bit().constData(), NULL);

    gst_bin_add_many(GST_BIN(encodeBin), muxer, fileSink,  NULL);
    ok &= gst_element_link(muxer, fileSink);

    if (m_captureMode & Audio) {
        GstElement *audioConvert = gst_element_factory_make("audioconvert", "audioconvert");
        GstElement *audioQueue = gst_element_factory_make("queue", "audio-encode-queue");
        GstElement *volume = gst_element_factory_make("volume", "volume");
        GstElement *audioEncoder = m_audioEncodeControl->createEncoder();

        ok &= audioEncoder != 0;

        gst_bin_add_many(GST_BIN(encodeBin), audioConvert, audioQueue, volume, audioEncoder,  NULL);

        ok &= gst_element_link_many(audioConvert, audioQueue, volume, audioEncoder, muxer, NULL);
        //g_object_set(G_OBJECT(volume), "volume", 10.0, NULL);

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

        ok &= videoEncoder != 0;

        gst_bin_add_many(GST_BIN(encodeBin), videoQueue, colorspace, videoscale, videoEncoder, NULL);
        ok &= gst_element_link_many(videoQueue, colorspace, videoscale, videoEncoder, muxer, NULL);

        // add ghostpads
        GstPad *pad = gst_element_get_static_pad(videoQueue, "sink");
        Q_ASSERT(pad);
        gst_element_add_pad(GST_ELEMENT(encodeBin), gst_ghost_pad_new("videosink", pad));
        gst_object_unref(GST_OBJECT(pad));
    }

    if (!ok) {
        gst_object_unref(encodeBin);
        encodeBin = 0;
    }

    return encodeBin;
}

GstElement *QGstreamerCaptureSession::buildAudioSrc()
{
    GstElement *audioSrc = 0;
    if (m_audioInputFactory)
        audioSrc = m_audioInputFactory->buildElement();
    else {

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
        audioSrc = gst_element_factory_make("pulsesrc", "audio_src");
#elif defined(QT_QWS_N810)
        audioSrc = gst_element_factory_make("dsppcmsrc", "audio_src");
#else
        QString elementName = "alsasrc";
        QString device;

        if (m_captureDevice.startsWith("alsa:")) {
            device = m_captureDevice.mid(QString("alsa:").length());
        } else if (m_captureDevice.startsWith("oss:")) {
            elementName = "osssrc";
            device = m_captureDevice.mid(QString("oss:").length());
        } else if (m_captureDevice.startsWith("pulseaudio:")) {
            elementName = "pulsesrc";
        } else {
            elementName = "autoaudiosrc";
        }

        audioSrc = gst_element_factory_make(elementName.toAscii().constData(), "audio_src");
        if (audioSrc && !device.isEmpty())
            g_object_set(G_OBJECT(audioSrc), "device", device.toLocal8Bit().constData(), NULL);
#endif
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
        GstElement *bin = gst_bin_new("video-preview-bin");
        GstElement *colorspace = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace-preview");
        GstElement *capsFilter = gst_element_factory_make("capsfilter", "capsfilter-video-preview");
        GstElement *preview = m_videoPreviewFactory->buildElement();

        gst_bin_add_many(GST_BIN(bin), colorspace, capsFilter, preview,  NULL);
        gst_element_link(colorspace,capsFilter);
        gst_element_link(capsFilter,preview);

        QVideoEncoderSettings videoSettings = m_videoEncodeControl->videoSettings();
        if (!videoSettings.resolution().isEmpty() || videoSettings.frameRate() > 0.001) {
            QSize resolution = videoSettings.resolution();
            qreal frameRate = videoSettings.frameRate();

            GstCaps *caps = gst_caps_new_empty();
            QStringList structureTypes;
            structureTypes << "video/x-raw-yuv" << "video/x-raw-rgb";

            foreach(const QString &structureType, structureTypes) {
                GstStructure *structure = gst_structure_new(structureType.toAscii().constData(), NULL);

                if (!resolution.isEmpty()) {
                    gst_structure_set(structure, "width", G_TYPE_INT, resolution.width(), NULL);
                    gst_structure_set(structure, "height", G_TYPE_INT, resolution.height(), NULL);
                }

                if (frameRate > 0.001) {
                    QPair<int,int> rate = m_videoEncodeControl->rateAsRational();

                    //qDebug() << "frame rate:" << num << denum;

                    gst_structure_set(structure, "framerate", GST_TYPE_FRACTION, rate.first, rate.second, NULL);
                }

                gst_caps_append_structure(caps,structure);
            }

            //qDebug() << "set video preview caps filter:" << gst_caps_to_string(caps);

            g_object_set(G_OBJECT(capsFilter), "caps", caps, NULL);

        }

        // add ghostpads
        GstPad *pad = gst_element_get_static_pad(colorspace, "sink");
        Q_ASSERT(pad);
        gst_element_add_pad(GST_ELEMENT(bin), gst_ghost_pad_new("videosink", pad));
        gst_object_unref(GST_OBJECT(pad));

        previewElement = bin;
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

bool QGstreamerCaptureSession::rebuildGraph(QGstreamerCaptureSession::PipelineMode newMode)
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

                ok &= m_audioSrc && m_audioPreview;

                if (ok) {
                    gst_bin_add_many(GST_BIN(m_pipeline), m_audioSrc, m_audioPreview, NULL);
                    ok &= gst_element_link(m_audioSrc, m_audioPreview);
                }
            }
            if (m_captureMode & Video) {
                m_videoSrc = buildVideoSrc();
                m_videoTee = gst_element_factory_make("tee", "video-preview-tee");
                m_videoPreviewQueue = gst_element_factory_make("queue", "video-preview-queue");
                m_videoPreview = buildVideoPreview();                

                ok &= m_videoSrc && m_videoTee && m_videoPreviewQueue && m_videoPreview;

                if (ok) {
                    gst_bin_add_many(GST_BIN(m_pipeline), m_videoSrc, m_videoTee,
                                     m_videoPreviewQueue, m_videoPreview, NULL);

                    ok &= gst_element_link(m_videoSrc, m_videoTee);
                    ok &= gst_element_link(m_videoTee, m_videoPreviewQueue);
                    ok &= gst_element_link(m_videoPreviewQueue, m_videoPreview);
                }
            }
            break;
        case RecordingPipeline:
            m_encodeBin = buildEncodeBin();
            gst_bin_add(GST_BIN(m_pipeline), m_encodeBin);

            if (m_captureMode & Audio) {
                m_audioSrc = buildAudioSrc();
                ok &= m_audioSrc != 0;

                gst_bin_add(GST_BIN(m_pipeline), m_audioSrc);
                ok &= gst_element_link(m_audioSrc, m_encodeBin);
            }

            if (m_captureMode & Video) {
                m_videoSrc = buildVideoSrc();
                ok &= m_videoSrc != 0;

                gst_bin_add(GST_BIN(m_pipeline), m_videoSrc);
                ok &= gst_element_link(m_videoSrc, m_encodeBin);
            }

            if (!m_metaData.isEmpty())
                setMetaData(m_metaData);

            break;
        case PreviewAndRecordingPipeline:
            m_encodeBin = buildEncodeBin();
            if (m_encodeBin)
                gst_bin_add(GST_BIN(m_pipeline), m_encodeBin);

            ok &= m_encodeBin != 0;

            if (ok && m_captureMode & Audio) {
                m_audioSrc = buildAudioSrc();
                m_audioPreview = buildAudioPreview();
                m_audioTee = gst_element_factory_make("tee", NULL);
                m_audioPreviewQueue = gst_element_factory_make("queue", NULL);

                ok &= m_audioSrc && m_audioPreview && m_audioTee && m_audioPreviewQueue;

                if (ok) {
                    gst_bin_add_many(GST_BIN(m_pipeline), m_audioSrc, m_audioTee,
                                     m_audioPreviewQueue, m_audioPreview, NULL);
                    ok &= gst_element_link(m_audioSrc, m_audioTee);
                    ok &= gst_element_link(m_audioTee, m_audioPreviewQueue);
                    ok &= gst_element_link(m_audioPreviewQueue, m_audioPreview);
                    ok &= gst_element_link(m_audioTee, m_encodeBin);
                }
            }

            if (ok && (m_captureMode & Video)) {
                m_videoSrc = buildVideoSrc();
                m_videoPreview = buildVideoPreview();
                m_videoTee = gst_element_factory_make("tee", NULL);
                m_videoPreviewQueue = gst_element_factory_make("queue", NULL);

                ok &= m_videoSrc && m_videoPreview && m_videoTee && m_videoPreviewQueue;

                if (ok) {
                    gst_bin_add_many(GST_BIN(m_pipeline), m_videoSrc, m_videoTee,
                                 m_videoPreviewQueue, m_videoPreview, NULL);
                    ok &= gst_element_link(m_videoSrc, m_videoTee);
                    ok &= gst_element_link(m_videoTee, m_videoPreviewQueue);
                    ok &= gst_element_link(m_videoPreviewQueue, m_videoPreview);
                    ok &= gst_element_link(m_videoTee, m_encodeBin);
                }
            }

            if (!m_metaData.isEmpty())
                setMetaData(m_metaData);

            break;
    }

    if (!ok) {
        emit error(int(QMediaRecorder::FormatError),tr("Failed to build media capture pipeline."));
    }

    dumpGraph( QString("rebuild_graph_%1_%2").arg(m_pipelineMode).arg(newMode) );
    if (m_encodeBin) {
        QString fileName = QString("rebuild_graph_encode_%1_%2").arg(m_pipelineMode).arg(newMode);
#if !(GST_DISABLE_GST_DEBUG) && (GST_VERSION_MAJOR >= 0) && (GST_VERSION_MINOR >= 10) && (GST_VERSION_MICRO >= 19)
        _gst_debug_bin_to_dot_file(GST_BIN(m_encodeBin), GST_DEBUG_GRAPH_SHOW_ALL, fileName.toAscii());
#endif
    }

    if (ok) {
        m_pipelineMode = newMode;
    } else {
        m_pipelineMode = EmptyPipeline;

        REMOVE_ELEMENT(m_audioSrc);
        REMOVE_ELEMENT(m_audioPreview);
        REMOVE_ELEMENT(m_audioPreviewQueue);
        REMOVE_ELEMENT(m_audioTee);
        REMOVE_ELEMENT(m_videoSrc);
        REMOVE_ELEMENT(m_videoPreview);
        REMOVE_ELEMENT(m_videoPreviewQueue);
        REMOVE_ELEMENT(m_videoTee);
        REMOVE_ELEMENT(m_encodeBin);
    }

    return ok;
}

void QGstreamerCaptureSession::dumpGraph(const QString &fileName)
{
#if !(GST_DISABLE_GST_DEBUG) && (GST_VERSION_MAJOR >= 0) && (GST_VERSION_MINOR >= 10) && (GST_VERSION_MICRO >= 19)
    _gst_debug_bin_to_dot_file(GST_BIN(m_pipeline),
                               GstDebugGraphDetails(/*GST_DEBUG_GRAPH_SHOW_ALL |*/ GST_DEBUG_GRAPH_SHOW_MEDIA_TYPE | GST_DEBUG_GRAPH_SHOW_NON_DEFAULT_PARAMS | GST_DEBUG_GRAPH_SHOW_STATES),
                               fileName.toAscii());
#endif
}

QUrl QGstreamerCaptureSession::outputLocation() const
{
    return m_sink;
}

bool QGstreamerCaptureSession::setOutputLocation(const QUrl& sink)
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

void QGstreamerCaptureSession::setVideoInput(QGstreamerVideoInput *videoInput)
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

    while (state != GST_STATE_NULL) {
        qApp->processEvents();
        gst_element_get_state(m_pipeline, &state, 0, 0);
    }
}

void QGstreamerCaptureSession::setState(QGstreamerCaptureSession::State newState)
{
    if (newState == m_pendingState && !m_waitingForEos)
        return;

    m_pendingState = newState;

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
        if (m_pipelineMode == PreviewAndRecordingPipeline) {
            if (!m_waitingForEos) {
                m_waitingForEos = true;
                //qDebug() << "Waiting for EOS";
                //with live sources it's necessary to send EOS even to pipeline
                //before going to STOPPED state
                gst_element_send_event(m_pipeline, gst_event_new_eos());
                return;
            } else {
                m_waitingForEos = false;
                //qDebug() << "EOS received";
            }
        }

        //select suitable default codecs/containers, if necessary
        m_recorderControl->applySettings();

        gst_element_set_state(m_pipeline, GST_STATE_NULL);

        //It would be better to do this async. but
        //gstreamer doesn't notify about pipeline went to NULL state
        waitForStopped();
        if (!rebuildGraph(newMode)) {
            m_pendingState = StoppedState;
            m_state = StoppedState;
            emit stateChanged(StoppedState);

            return;
        }
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

    //we have to do it here, since gstreamer will not emit bus messages any more
    if (newState == StoppedState) {
        m_state = StoppedState;
        emit stateChanged(StoppedState);
    }
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

void QGstreamerCaptureSession::setMetaData(const QMap<QByteArray, QVariant> &data)
{
    //qDebug() << "QGstreamerCaptureSession::setMetaData" << data;
    m_metaData = data;

    if (m_encodeBin) {
        GstIterator *elements = gst_bin_iterate_all_by_interface(GST_BIN(m_encodeBin), GST_TYPE_TAG_SETTER);
        GstElement *element = 0;
        while (gst_iterator_next(elements, (void**)&element) == GST_ITERATOR_OK) {
            //qDebug() << "found element with tag setter interface:" << gst_element_get_name(element);
            QMapIterator<QByteArray, QVariant> it(data);
            while (it.hasNext()) {
                it.next();
                const QString tagName = it.key();
                const QVariant tagValue = it.value();


                switch(tagValue.type()) {
                    case QVariant::String:
                        gst_tag_setter_add_tags(GST_TAG_SETTER(element),
                            GST_TAG_MERGE_REPLACE_ALL,
                            tagName.toUtf8().constData(),
                            tagValue.toString().toUtf8().constData(),
                            NULL);
                        break;
                    case QVariant::Int:
                    case QVariant::LongLong:
                        gst_tag_setter_add_tags(GST_TAG_SETTER(element),
                            GST_TAG_MERGE_REPLACE_ALL,
                            tagName.toUtf8().constData(),
                            tagValue.toInt(),
                            NULL);
                        break;
                    case QVariant::Double:
                        gst_tag_setter_add_tags(GST_TAG_SETTER(element),
                            GST_TAG_MERGE_REPLACE_ALL,
                            tagName.toUtf8().constData(),
                            tagValue.toDouble(),
                            NULL);
                        break;
                    default:
                        break;
                }

            }

        }
    }
}

bool QGstreamerCaptureSession::processSyncMessage(const QGstreamerMessage &message)
{
    GstMessage* gm = message.rawMessage();

    if (gm && GST_MESSAGE_TYPE(gm) == GST_MESSAGE_ELEMENT &&
            gst_structure_has_name(gm->structure, "prepare-xwindow-id"))
        {
            if (m_audioPreviewFactory)
                m_audioPreviewFactory->prepareWinId();

            if (m_videoPreviewFactory)
                m_videoPreviewFactory->prepareWinId();

            return true;
        }

    return false;
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

        if (GST_MESSAGE_SRC(gm) == GST_OBJECT_CAST(m_pipeline)) {
            switch (GST_MESSAGE_TYPE(gm))  {
            case GST_MESSAGE_DURATION:
                break;

            case GST_MESSAGE_EOS:
                if (m_waitingForEos)
                    setState(m_pendingState);
                break;

            case GST_MESSAGE_STATE_CHANGED:
                {

                    GstState    oldState;
                    GstState    newState;
                    GstState    pending;

                    gst_message_parse_state_changed(gm, &oldState, &newState, &pending);

                    QStringList states;
                    states << "GST_STATE_VOID_PENDING" <<  "GST_STATE_NULL" << "GST_STATE_READY" << "GST_STATE_PAUSED" << "GST_STATE_PLAYING";

                    /*
                    qDebug() << QString("state changed: old: %1  new: %2  pending: %3") \
                            .arg(states[oldState]) \
                           .arg(states[newState]) \
                            .arg(states[pending]);

                    #define ENUM_NAME(c,e,v) (c::staticMetaObject.enumerator(c::staticMetaObject.indexOfEnumerator(e)).valueToKey((v)))

                    qDebug() << "Current session state:" << ENUM_NAME(QGstreamerCaptureSession,"State",m_state);
                    qDebug() << "Pending session state:" << ENUM_NAME(QGstreamerCaptureSession,"State",m_pendingState);
                    */

                    switch (newState) {
                    case GST_STATE_VOID_PENDING:
                    case GST_STATE_NULL:
                    case GST_STATE_READY:
                        if (m_state != StoppedState && m_pendingState == StoppedState) {
                            emit stateChanged(m_state = StoppedState);
                            dumpGraph("stopped");
                        }
                        break;
                    case GST_STATE_PAUSED:
                        if (m_state != PausedState && m_pendingState == PausedState)
                            emit stateChanged(m_state = PausedState);
                        dumpGraph("paused");

                        if (m_pipelineMode == RecordingPipeline && !m_metaData.isEmpty())
                            setMetaData(m_metaData);
                        break;
                    case GST_STATE_PLAYING:
                        {
                            if ((m_pendingState == PreviewState || m_pendingState == RecordingState) &&
                                m_state != m_pendingState)
                            {
                                m_state = m_pendingState;
                                emit stateChanged(m_state);
                            }

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
            //qDebug() << "New session state:" << ENUM_NAME(QGstreamerCaptureSession,"State",m_state);
        }
    }
}

