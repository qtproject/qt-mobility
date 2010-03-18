/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgstreamercapturesession_maemo.h"
#include "qgstreamerrecordercontrol_maemo.h"
#include "qgstreamermediacontainercontrol_maemo.h"
#include "qgstreameraudioencode_maemo.h"
#include "qgstreamervideoencode_maemo.h"
#include "qgstreamerbushelper.h"
#include <qmediarecorder.h>
#include <gst/gsttagsetter.h>
#include <gst/gstversion.h>

#include <QtCore/qdebug.h>
#include <QCoreApplication>
#include <QtCore/qmetaobject.h>

#include <QtGui/qimage.h>

#define gstRef(element) { gst_object_ref(GST_OBJECT(element)); gst_object_sink(GST_OBJECT(element)); }
#define gstUnref(element) { if (element) { gst_object_unref(GST_OBJECT(element)); element = 0; } }

#define PREVIEW_CAPS \
    "video/x-raw-rgb, width = (int) 640, height = (int) 480"

// Function prototypes
static gboolean imgCaptured(GstElement *camera, const gchar *filename, gpointer user_data);

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
     m_pipeline(0),
     m_videoSrc(0),
     m_videoPreviewFactoryHasChanged(false),
     m_audioSrc(0),
     m_audioConvert(0),
     m_capsFilter(0),
     m_fileSink(0),
     m_audioEncoder(0)
{
    if (m_captureMode == AudioAndVideo) {
        m_pipeline = gst_element_factory_make("camerabin", "camerabin");
    } else if (m_captureMode & Audio) {
        m_pipeline = gst_pipeline_new("audio-capture-pipeline");
        m_audioSrc = gst_element_factory_make("pulsesrc", "pulsesrc");
        m_audioConvert = gst_element_factory_make("audioconvert", "audioconvert");
        m_capsFilter = gst_element_factory_make("capsfilter", "capsfilter-audio");
        m_fileSink = gst_element_factory_make("filesink", "filesink");

        if (!m_audioSrc || !m_audioConvert || !m_fileSink)
            emit error(int(QMediaRecorder::ResourceError), QString("Element creation failed."));

        gst_bin_add_many(GST_BIN(m_pipeline), m_audioSrc, m_audioConvert, m_fileSink, NULL);

        if (!gst_element_link(m_audioSrc, m_audioConvert))
            emit error(int(QMediaRecorder::ResourceError), QString("Element linking failed."));

    }

    gstRef(m_pipeline);

    m_bus = gst_element_get_bus(m_pipeline);

    m_busHelper = new QGstreamerBusHelper(m_bus, this);
    m_busHelper->installSyncEventFilter(this);
    connect(m_busHelper, SIGNAL(message(QGstreamerMessage)), SLOT(busMessage(QGstreamerMessage)));
    m_audioEncodeControl = new QGstreamerAudioEncode(this);
    m_videoEncodeControl = new QGstreamerVideoEncode(this);
    m_recorderControl = new QGstreamerRecorderControl(this);
    m_mediaContainerControl = new QGstreamerMediaContainerControl(this);
}

QGstreamerCaptureSession::~QGstreamerCaptureSession()
{
    if (m_pipeline) {
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
        gst_element_get_state(m_pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
        gstUnref(m_pipeline);
    }
}

void QGstreamerCaptureSession::setupCameraBin()
{
    GstState currentState = GST_STATE_PLAYING;
    gst_element_get_state(m_pipeline, &currentState, 0, 0);
    GstState previousState = currentState;

    if (currentState != GST_STATE_NULL) {
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
        gst_element_get_state(m_pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
    }

    m_recorderControl->applySettings();

    g_object_set(m_pipeline, "videosrc", buildVideoSrc(), NULL);
    g_object_set(m_pipeline, "videoenc", m_videoEncodeControl->createEncoder(), NULL);
    g_object_set(m_pipeline, "audioenc", m_audioEncodeControl->createEncoder(), NULL);
    g_object_set(m_pipeline, "videomux",
                 gst_element_factory_make(m_mediaContainerControl->formatElementName().constData(), NULL), NULL);

    if (m_videoPreviewFactory) {
        GstElement *preview = m_videoPreviewFactory->buildElement();
        g_object_set(G_OBJECT(m_pipeline), "vfsink", preview, NULL);
    }

    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
}

#define REMOVE_ELEMENT(element) { if (element) {gst_bin_remove(GST_BIN(m_pipeline), element); element = 0;} }

void QGstreamerCaptureSession::buildAudioEncodeBin()
{    
    REMOVE_ELEMENT(m_audioEncoder);

    m_audioEncoder = m_audioEncodeControl->createEncoder();
    if (!m_audioEncoder)
       emit error(int(QMediaRecorder::ResourceError), QString("Element creation failed"));

    gst_bin_add(GST_BIN(m_pipeline), m_audioEncoder);

    if (!gst_element_link_many(m_audioConvert, m_audioEncoder, m_fileSink, NULL))
        emit error(int(QMediaRecorder::ResourceError), QString("Element linking failed"));

    g_object_set(G_OBJECT(m_fileSink), "location", m_sink.toString().toLocal8Bit().constData(), NULL);
}

GstElement *QGstreamerCaptureSession::buildVideoSrc()
{
    GstElement *videoSrc = 0;
    if (m_videoInputFactory) {
        videoSrc = m_videoInputFactory->buildElement();
    } else {
        videoSrc = gst_element_factory_make("videotestsrc", "video_test_src");
    }

    return videoSrc;
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
    m_videoPreviewFactoryHasChanged = true;
}

QGstreamerCaptureSession::State QGstreamerCaptureSession::state() const
{
    return m_state;
}

void QGstreamerCaptureSession::setState(QGstreamerCaptureSession::State newState)
{

    if (newState == m_state)
        return;

    //qDebug() << "NewState: " << newState;
    //qDebug() << "CurrentState: " << m_state;

    switch (m_state) {
        case PreviewState:
            if (newState == StoppedState) {
                m_state = StoppedState;
                gst_element_set_state(m_pipeline, GST_STATE_NULL);
                gst_element_get_state(m_pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
                emit stateChanged(StoppedState);
            }
            g_object_set(G_OBJECT(m_pipeline), "mode", 1, NULL);
            m_state = StoppedState;
            if (newState == RecordingState) {
                setState(newState);
            }
            break;
        case StoppedState:
            m_state = newState;
            if (newState == PreviewState) {

                if (m_videoPreviewFactory && m_videoPreviewFactoryHasChanged) {
                    m_videoPreviewFactoryHasChanged = false;
                    GstElement *preview = m_videoPreviewFactory->buildElement();
                    g_object_set(G_OBJECT(m_pipeline), "vfsink", preview, NULL);
                }
                gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
                g_object_set(G_OBJECT(m_pipeline), "mode", 0, NULL);
            } else {
                if (m_captureMode == AudioAndVideo) {
                    setupCameraBin();
                    g_object_set(G_OBJECT(m_pipeline), "filename", m_sink.toString().toLocal8Bit().constData(), NULL);
                    g_object_set(G_OBJECT(m_pipeline), "mode", 1, NULL);
                    g_signal_emit_by_name(m_pipeline, "user-start", 0);
                } else if (m_captureMode & Audio) {
                    buildAudioEncodeBin();
                    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
                }
            }
            break;
        case PausedState:
        case RecordingState:
            if (newState == PausedState) {
                g_signal_emit_by_name(m_pipeline, "user-pause", 0);
                m_state = PausedState;
            } else {
                if (m_captureMode == AudioAndVideo)
                    g_signal_emit_by_name(m_pipeline, "user-stop", 0);
                else if (m_captureMode & Audio) {
                    gst_element_send_event(m_pipeline, gst_event_new_eos());
                    gst_element_set_state(m_pipeline, GST_STATE_NULL);
                }

                m_state = StoppedState;
                if (newState == PreviewState)
                    setState(newState);
            }
            break;
    }
}


qint64 QGstreamerCaptureSession::duration() const
{
    GstFormat   format = GST_FORMAT_TIME;
    gint64      duration = 0;

    if ( m_pipeline && gst_element_query_position(m_pipeline, &format, &duration))
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
    m_metaData = data;

    if (m_pipeline) {
        GstIterator *elements = gst_bin_iterate_all_by_interface(GST_BIN(m_pipeline), GST_TYPE_TAG_SETTER);
        GstElement *element = 0;
        while (gst_iterator_next(elements, (void**)&element) == GST_ITERATOR_OK) {
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
    const GstStructure *st;
    const GValue *image;
    GstBuffer *buffer = NULL;

    if (gm && GST_MESSAGE_TYPE(gm) == GST_MESSAGE_ELEMENT) {
        if (gst_structure_has_name(gm->structure, "prepare-xwindow-id")) {
            if (m_audioPreviewFactory)
                m_audioPreviewFactory->prepareWinId();

            if (m_videoPreviewFactory)
                m_videoPreviewFactory->prepareWinId();

            return true;
        }
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


                    //qDebug() << QString("state changed: old: %1  new: %2  pending: %3") \
                    //        .arg(states[oldState]) \
                    //       .arg(states[newState]) \
                    //        .arg(states[pending]);

                    #define ENUM_NAME(c,e,v) (c::staticMetaObject.enumerator(c::staticMetaObject.indexOfEnumerator(e)).valueToKey((v)))

                    //qDebug() << "Current session state:" << ENUM_NAME(QGstreamerCaptureSession,"State",m_state);
                    //qDebug() << "Pending session state:" << ENUM_NAME(QGstreamerCaptureSession,"State",m_pendingState);


                    switch (newState) {
                    case GST_STATE_VOID_PENDING:
                    case GST_STATE_NULL:
                    case GST_STATE_READY:
                        /*if (m_state != StoppedState && m_pendingState == StoppedState) {
                            emit stateChanged(m_state = StoppedState);
                        }*/
                        break;
                    case GST_STATE_PAUSED:
                        if (m_state != PausedState && m_pendingState == PausedState)
                            emit stateChanged(m_state = PausedState);

                        if (m_pipelineMode == RecordingPipeline && !m_metaData.isEmpty())
                            setMetaData(m_metaData);
                        break;
                    case GST_STATE_PLAYING:
                        {
                            if (m_state == PreviewState || m_state == RecordingState)
                            {
                                emit stateChanged(m_state);
                            }

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
