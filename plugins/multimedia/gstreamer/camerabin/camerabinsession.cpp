/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "camerabinsession.h"
#include "camerabinrecorder.h"
#include "camerabincontainer.h"
#include "camerabinaudioencoder.h"
#include "camerabinvideoencoder.h"
#include "camerabinimageencoder.h"
#include "camerabinexposure.h"
#include "camerabinfocus.h"
#include "camerabinimageprocessing.h"
#include "camerabinlocks.h"
#include "qgstreamerbushelper.h"
#include <qmediarecorder.h>
#include <gst/interfaces/photography.h>
#include <gst/gsttagsetter.h>
#include <gst/gstversion.h>

#include <QtCore/qdebug.h>
#include <QCoreApplication>
#include <QtCore/qmetaobject.h>

#include <QtGui/qimage.h>

#define CAMERABIN_DEBUG 1

#ifdef Q_WS_MAEMO_5
#define FILENAME_PROPERTY "filename"
#define MODE_PROPERTY "mode"
#define MUTE_PROPERTY "mute"
#define ZOOM_PROPERTY "zoom"
#define IMAGE_PP_PROPERTY "imagepp"
#define IMAGE_ENCODER_PROPERTY "imageenc"
#define VIDEO_PP_PROPERTY "videopp"
#define VIDEO_ENCODER_PROPERTY "videoenc"
#define AUDIO_ENCODER_PROPERTY "audioenc"
#define VIDEO_MUXER_PROPERTY "videomux"
#define VIEWFINDER_SINK_PROPERTY "vfsink"
#define VIDEO_SOURCE_PROPERTY "videosrc"
#define AUDIO_SOURCE_PROPERTY "audiosrc"
#define VIDEO_SOURCE_CAPS_PROPERTY "inputcaps"
#define FILTER_CAPS_PROPERTY "filter-caps"
#define PREVIEW_CAPS_PROPERTY "preview-caps"

#define IMAGE_DONE_SIGNAL "img-done"
#define CAPTURE_START "user-start"
#define CAPTURE_STOP "user-stop"
#define CAPTURE_PAUSE "user-pause"
#define SET_VIDEO_RESOLUTION_FPS "user-res-fps"
#define SET_IMAGE_RESOLUTION "user-image-res"

#else

#define FILENAME_PROPERTY "filename"
#define MODE_PROPERTY "mode"
#define MUTE_PROPERTY "mute"
#define ZOOM_PROPERTY "zoom"
#define IMAGE_PP_PROPERTY "image-post-processing"
#define IMAGE_ENCODER_PROPERTY "image-encoder"
#define VIDEO_PP_PROPERTY "video-post-processing"
#define VIDEO_ENCODER_PROPERTY "video-encoder"
#define AUDIO_ENCODER_PROPERTY "audio-encoder"
#define VIDEO_MUXER_PROPERTY "video-muxer"
#define VIEWFINDER_SINK_PROPERTY "viewfinder-sink"
#define VIDEO_SOURCE_PROPERTY "video-source"
#define AUDIO_SOURCE_PROPERTY "audio-source"
#define VIDEO_SOURCE_CAPS_PROPERTY "video-source-caps"
#define FILTER_CAPS_PROPERTY "filter-caps"
#define PREVIEW_CAPS_PROPERTY "preview-caps"

#define IMAGE_DONE_SIGNAL "image-done"
#define CAPTURE_START "capture-start"
#define CAPTURE_STOP "capture-stop"
#define CAPTURE_PAUSE "capture-pause"
#define SET_VIDEO_RESOLUTION_FPS "set-video-resolution-fps"
#define SET_IMAGE_RESOLUTION "set-image-resolution"
#endif

#define CAMERABIN_IMAGE_MODE 0
#define CAMERABIN_VIDEO_MODE 1

#define gstRef(element) { gst_object_ref(GST_OBJECT(element)); gst_object_sink(GST_OBJECT(element)); }
#define gstUnref(element) { if (element) { gst_object_unref(GST_OBJECT(element)); element = 0; } }

#define PREVIEW_CAPS \
    "video/x-raw-rgb, width = (int) 640, height = (int) 480"

// Function prototypes
static gboolean imgCaptured(GstElement *camera, const gchar *filename, gpointer user_data);

QT_BEGIN_NAMESPACE
static inline uint qHash(const QSize& key) { return uint(key.width()*256+key.height()); }

static bool operator<(const QSize &s1, const QSize s2)
{
    return s1.width()*s1.height() < s2.width()*s2.height();
}
QT_END_NAMESPACE


CameraBinSession::CameraBinSession(QObject *parent)
    :QObject(parent),
     m_state(QCamera::StoppedState),
     m_pendingState(QCamera::StoppedState),
     m_muted(false),
     m_captureMode(QCamera::CaptureDisabled),
     m_audioInputFactory(0),
     m_videoInputFactory(0),
     m_viewfinderFactory(0),
     m_pipeline(0),
     m_videoSrc(0),
     m_viewfinderFactoryHasChanged(false),
     m_videoInputHasChanged(false),
     m_sourceCaps(0),
     m_audioSrc(0),
     m_audioConvert(0),
     m_capsFilter(0),
     m_fileSink(0),
     m_audioEncoder(0),
     m_muxer(0)
{
    m_pipeline = gst_element_factory_make("camerabin", "camerabin");

    g_signal_connect(G_OBJECT(m_pipeline), IMAGE_DONE_SIGNAL, G_CALLBACK(imgCaptured), this);

    gstRef(m_pipeline);

    m_bus = gst_element_get_bus(m_pipeline);

    m_busHelper = new QGstreamerBusHelper(m_bus, this);
    m_busHelper->installSyncEventFilter(this);
    connect(m_busHelper, SIGNAL(message(QGstreamerMessage)), SLOT(busMessage(QGstreamerMessage)));
    m_audioEncodeControl = new CameraBinAudioEncoder(this);
    m_videoEncodeControl = new CameraBinVideoEncoder(this);
    m_imageEncodeControl = new CameraBinImageEncoder(this);
    m_recorderControl = new CameraBinRecorder(this);
    m_mediaContainerControl = new CameraBinContainer(this);
    m_cameraExposureControl = new CameraBinExposure(*m_pipeline, this);
    m_cameraFocusControl = new CameraBinFocus(*m_pipeline, this);
    m_imageProcessingControl = new CameraBinImageProcessing(*m_pipeline, this);
    m_cameraLocksControl = new CameraBinLocks(*m_pipeline, this);
}

CameraBinSession::~CameraBinSession()
{
    if (m_pipeline) {
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
        gst_element_get_state(m_pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
        gstUnref(m_pipeline);
    }
}

bool CameraBinSession::setupCameraBin()
{
    if (m_captureMode == QCamera::CaptureStillImage) {
        g_object_set(m_pipeline, MODE_PROPERTY, CAMERABIN_IMAGE_MODE, NULL);
    }

    if (m_captureMode == QCamera::CaptureVideo) {
        g_object_set(m_pipeline, MODE_PROPERTY, CAMERABIN_VIDEO_MODE, NULL);

        if (!m_recorderControl->findCodecs())
            return false;

        g_object_set(m_pipeline, VIDEO_ENCODER_PROPERTY, m_videoEncodeControl->createEncoder(), NULL);
        g_object_set(m_pipeline, AUDIO_ENCODER_PROPERTY, m_audioEncodeControl->createEncoder(), NULL);
        g_object_set(m_pipeline, VIDEO_MUXER_PROPERTY,
                     gst_element_factory_make(m_mediaContainerControl->formatElementName().constData(), NULL), NULL);
    }

    if (m_videoInputHasChanged) {
        g_object_set(m_pipeline, VIDEO_SOURCE_PROPERTY, buildVideoSrc(), NULL);
        updateVideoSourceCaps();
        m_videoInputHasChanged = false;
    }

    if (m_viewfinderFactory) {
        //temporary disabled for the current camerabin due to gstreamer bug 623802
#ifdef Q_WS_MAEMO_5
        GstElement *preview = m_viewfinderFactory->buildElement();
        g_object_set(G_OBJECT(m_pipeline), VIEWFINDER_SINK_PROPERTY, preview, NULL);
#endif
    }

    GstCaps *previewCaps = gst_caps_from_string(PREVIEW_CAPS);
    g_object_set(G_OBJECT(m_pipeline), PREVIEW_CAPS_PROPERTY, previewCaps, NULL);
    gst_caps_unref(previewCaps);

    return true;
}

void CameraBinSession::updateVideoSourceCaps()
{
    if (m_sourceCaps) {
        gst_caps_unref(m_sourceCaps);
        m_sourceCaps = 0;
    }

    g_object_get(G_OBJECT(m_pipeline), VIDEO_SOURCE_CAPS_PROPERTY, &m_sourceCaps, NULL);

    qDebug() << "Video source caps:" << gst_caps_to_string(m_sourceCaps);
}

void CameraBinSession::setupCaptureResolution()
{
    if (m_captureMode == QCamera::CaptureStillImage) {
        QSize resolution = m_imageEncodeControl->imageSettings().resolution();
        if (!resolution.isEmpty()) {
            qDebug() << "image resolution" << resolution;
            g_signal_emit_by_name(G_OBJECT(m_pipeline), SET_IMAGE_RESOLUTION, resolution.width(), resolution.height(), NULL);
        }
    }

    if (m_captureMode == QCamera::CaptureVideo) {
        QSize resolution = m_videoEncodeControl->videoSettings().resolution();
        qreal framerate = m_videoEncodeControl->videoSettings().frameRate();
        if (!resolution.isEmpty() || framerate > 0) {
            qDebug() << "image resolution" << resolution;
            g_signal_emit_by_name(G_OBJECT(m_pipeline),
                                  SET_VIDEO_RESOLUTION_FPS,
                                  resolution.width(),
                                  resolution.height(),
                                  0, //framerate nom == max rate
                                  0, // framerate denom == max rate
                                  NULL);
        }
    }
}

#define REMOVE_ELEMENT(element) { if (element) {gst_bin_remove(GST_BIN(m_pipeline), element); element = 0;} }


GstElement *CameraBinSession::buildVideoSrc()
{
    GstElement *videoSrc = 0;
    if (m_videoInputFactory) {
        videoSrc = m_videoInputFactory->buildElement();
    } else {
#ifndef Q_WS_MAEMO_5
        videoSrc = gst_element_factory_make("v4l2src", "camera_source");
#else
        videoSrc = gst_element_factory_make("v4l2camsrc", "camera_source");
#endif
    if (videoSrc && !m_inputDevice.isEmpty() )
        g_object_set(G_OBJECT(videoSrc), "device", m_inputDevice.toLocal8Bit().constData(), NULL);
    }

    return videoSrc;
}

void CameraBinSession::captureImage(int requestId, const QString &fileName)
{
    QString actualFileName = fileName;
    if (actualFileName.isEmpty()) {
        actualFileName = generateFileName("img_", defaultDir(QCamera::CaptureStillImage), "jpg");
    }

    m_requestId = requestId;

    QSize resolution = m_imageEncodeControl->imageSettings().resolution();
    if (!resolution.isEmpty()) {
        qDebug() << "Set image resolution" << resolution;
        g_signal_emit_by_name(G_OBJECT(m_pipeline), SET_IMAGE_RESOLUTION, resolution.width(), resolution.height(), NULL);
    }

    g_object_set(G_OBJECT(m_pipeline), FILENAME_PROPERTY, actualFileName.toLocal8Bit().constData(), NULL);

    g_signal_emit_by_name(G_OBJECT(m_pipeline), CAPTURE_START, NULL);

    m_imageFileName = actualFileName;
}

QUrl CameraBinSession::outputLocation() const
{
    //return the location service wrote data to, not one set by user, it can be empty.
    return m_actualSink;
}

bool CameraBinSession::setOutputLocation(const QUrl& sink)
{
    m_sink = m_actualSink = sink;
    return true;
}

QDir CameraBinSession::defaultDir(QCamera::CaptureMode mode) const
{
    QStringList dirCandidates;

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    if (mode == QCamera::CaptureVideo)
        dirCandidates << QLatin1String("/home/user/MyDocs/.videos");
    else
        dirCandidates << QLatin1String("/home/user/MyDocs/.images");

    dirCandidates << QLatin1String("/home/user/MyDocs/");
#endif

    if (mode == QCamera::CaptureVideo) {
        dirCandidates << QDir::home().filePath("Documents/Video");
        dirCandidates << QDir::home().filePath("Documents/Videos");
    } else {
        dirCandidates << QDir::home().filePath("Documents/Photo");
        dirCandidates << QDir::home().filePath("Documents/Photos");
        dirCandidates << QDir::home().filePath("Documents/photo");
        dirCandidates << QDir::home().filePath("Documents/photos");
        dirCandidates << QDir::home().filePath("Documents/Images");
    }

    dirCandidates << QDir::home().filePath("Documents");
    dirCandidates << QDir::home().filePath("My Documents");
    dirCandidates << QDir::homePath();
    dirCandidates << QDir::currentPath();
    dirCandidates << QDir::tempPath();

    foreach (const QString &path, dirCandidates) {
        if (QFileInfo(path).isWritable())
            return QDir(path);
    }

    return QDir();
}

QString CameraBinSession::generateFileName(const QString &prefix, const QDir &dir, const QString &ext) const
{
    int lastClip = 0;
    foreach(QString fileName, dir.entryList(QStringList() << QString("%1*.%2").arg(prefix).arg(ext))) {
        int imgNumber = fileName.mid(prefix.length(), fileName.size()-prefix.length()-ext.length()-1).toInt();
        lastClip = qMax(lastClip, imgNumber);
    }

    QString name = QString("%1%2.%3").arg(prefix)
                                     .arg(lastClip+1,
                                     4, //fieldWidth
                                     10,
                                     QLatin1Char('0'))
                                     .arg(ext);

    return dir.absoluteFilePath(name);
}

void CameraBinSession::setDevice(const QString &device)
{
    if (m_inputDevice != device) {
        m_inputDevice = device;
        m_videoInputHasChanged = true;
    }
}

void CameraBinSession::setAudioInput(QGstreamerElementFactory *audioInput)
{
    m_audioInputFactory = audioInput;
}

void CameraBinSession::setVideoInput(QGstreamerVideoInput *videoInput)
{
    m_videoInputFactory = videoInput;
    m_videoInputHasChanged = true;
}

void CameraBinSession::setViewfinder(QGstreamerElementFactory *viewfinder)
{
    m_viewfinderFactory = viewfinder;
    m_viewfinderFactoryHasChanged = true;
}

QCamera::State CameraBinSession::state() const
{
    return m_state;
}

void CameraBinSession::setState(QCamera::State newState)
{
    if (newState == m_pendingState)
        return;

    m_pendingState = newState;

    qDebug() << Q_FUNC_INFO << newState;

    switch (newState) {
    case QCamera::StoppedState:
    case QCamera::IdleState:
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
        m_state = newState;
        if (m_state == QCamera::IdleState && m_videoInputHasChanged) {
            g_object_set(m_pipeline, VIDEO_SOURCE_PROPERTY, buildVideoSrc(), NULL);
            updateVideoSourceCaps();
            m_videoInputHasChanged = false;
        }
        emit stateChanged(m_state);
        break;
    case QCamera::ActiveState:
        if (setupCameraBin())
            gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

        bool continuous = false;
        supportedFrameRates(QSize(), &continuous);
        supportedResolutions(QPair<int,int>(0,0), &continuous);
        break;
    }
}

qint64 CameraBinSession::duration() const
{
    GstFormat   format = GST_FORMAT_TIME;
    gint64      duration = 0;

    if ( m_pipeline && gst_element_query_position(m_pipeline, &format, &duration))
        return duration / 1000000;
    else
        return 0;
}

bool CameraBinSession::isMuted() const
{
    return m_muted;
}

void CameraBinSession::setMuted(bool muted)
{
    if (m_muted != muted) {
        m_muted = muted;

        if (m_pipeline)
            g_object_set(G_OBJECT(m_pipeline), MUTE_PROPERTY, m_muted, NULL);
        emit mutedChanged(m_muted);
    }
}

void CameraBinSession::setCaptureDevice(const QString &deviceName)
{
    m_captureDevice = deviceName;
}

void CameraBinSession::setMetaData(const QMap<QByteArray, QVariant> &data)
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

bool CameraBinSession::processSyncMessage(const QGstreamerMessage &message)
{
    GstMessage* gm = message.rawMessage();
    const GstStructure *st;
    const GValue *image;
    GstBuffer *buffer = NULL;

    if (gm && GST_MESSAGE_TYPE(gm) == GST_MESSAGE_ELEMENT) {
        if (gst_structure_has_name(gm->structure, "preview-image"))
        {
            st = gst_message_get_structure(gm);
            if (gst_structure_has_field_typed(st, "buffer", GST_TYPE_BUFFER)) {
                image = gst_structure_get_value(st, "buffer");
                if (image) {
                    buffer = gst_value_get_buffer(image);

                    QImage img;

                    GstCaps *caps = gst_buffer_get_caps(buffer);
                    if (caps) {
                        GstStructure *structure = gst_caps_get_structure(caps, 0);
                        gint width = 0;
                        gint height = 0;

                        if (structure &&
                            gst_structure_get_int(structure, "width", &width) &&
                            gst_structure_get_int(structure, "height", &height) &&
                            width > 0 && height > 0) {
                            if (qstrcmp(gst_structure_get_name(structure), "video/x-raw-rgb") == 0) {
                                QImage::Format format = QImage::Format_Invalid;
                                int bpp = 0;
                                gst_structure_get_int(structure, "bpp", &bpp);

                                if (bpp == 24)
                                    format = QImage::Format_RGB888;
                                else if (bpp == 32)
                                    format = QImage::Format_RGB32;

                                if (format != QImage::Format_Invalid) {
                                    img = QImage((const uchar *)buffer->data, width, height, format);
                                    img.bits(); //detach
                                 }
                            }
                        }
                        gst_caps_unref(caps);

                        emit imageExposed(m_requestId);
                        emit imageCaptured(m_requestId, img);
                    }

                }
                return true;
            }
        }

        if (gst_structure_has_name(gm->structure, "prepare-xwindow-id")) {
            if (m_viewfinderFactory)
                m_viewfinderFactory->prepareWinId();

            return true;
        }

        if (gst_structure_has_name(gm->structure, GST_PHOTOGRAPHY_AUTOFOCUS_DONE)) {
            gint status = GST_PHOTOGRAPHY_FOCUS_STATUS_NONE;
            gst_structure_get_int (gm->structure, "status", &status);
            switch (status) {
                case GST_PHOTOGRAPHY_FOCUS_STATUS_FAIL:
                emit focusStatusChanged(QCamera::Unlocked, QCamera::LockFailed);
                    break;
                case GST_PHOTOGRAPHY_FOCUS_STATUS_SUCCESS:
                    emit focusStatusChanged(QCamera::Locked, QCamera::UserRequest);
                    break;
                case GST_PHOTOGRAPHY_FOCUS_STATUS_NONE:
                    emit focusStatusChanged(QCamera::Unlocked, QCamera::UserRequest);
                    break;
                case GST_PHOTOGRAPHY_FOCUS_STATUS_RUNNING:
                    emit focusStatusChanged(QCamera::Searching, QCamera::UserRequest);
                    break;
                default:
                    break;
            }
        }
    }

    return false;
}

void CameraBinSession::busMessage(const QGstreamerMessage &message)
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

            case GST_MESSAGE_STATE_CHANGED:
                {

                    GstState    oldState;
                    GstState    newState;
                    GstState    pending;

                    gst_message_parse_state_changed(gm, &oldState, &newState, &pending);


#if CAMERABIN_DEBUG
                    QStringList states;
                    states << "GST_STATE_VOID_PENDING" <<  "GST_STATE_NULL" << "GST_STATE_READY" << "GST_STATE_PAUSED" << "GST_STATE_PLAYING";


                    qDebug() << QString("state changed: old: %1  new: %2  pending: %3") \
                            .arg(states[oldState]) \
                           .arg(states[newState]) \
                            .arg(states[pending]);
#endif

                    switch (newState) {
                    case GST_STATE_VOID_PENDING:
                    case GST_STATE_NULL:
                    case GST_STATE_READY:
                        {
                            QCamera::State state = m_captureMode == QCamera::CaptureDisabled ?
                                                   QCamera::StoppedState : QCamera::IdleState;
                            if (m_state != state)
                                emit stateChanged(m_state = state);
                        }
                        break;
                    case GST_STATE_PAUSED:                        
                        setupCaptureResolution();
                        break;
                    case GST_STATE_PLAYING:
                        emit stateChanged(m_state = QCamera::ActiveState);
                        break;
                    }
                }
                break;
            default:
                break;
            }
            //qDebug() << "New session state:" << ENUM_NAME(CameraBinSession,"State",m_state);
        }
    }
}

void CameraBinSession::processSavedImage(const QString &filename)
{
    emit imageSaved(m_requestId, filename);
    emit focusStatusChanged(QCamera::Unlocked, QCamera::LockLost);
}

static gboolean imgCaptured(GstElement *camera,
                        const gchar *filename,
                        gpointer user_data)
{
    qDebug() << "Image done" << filename;
    Q_UNUSED(camera);
    CameraBinSession *session = (CameraBinSession *)user_data;
    session->processSavedImage(QString::fromUtf8(filename));
    return true;
}


void CameraBinSession::recordVideo()
{
    m_actualSink = m_sink;
    if (m_actualSink.isEmpty()) {
        QString ext = m_mediaContainerControl->containerMimeType();
        m_actualSink = generateFileName("clip_", defaultDir(QCamera::CaptureVideo), ext);
    }

    setupCaptureResolution();

    g_object_set(G_OBJECT(m_pipeline), FILENAME_PROPERTY, m_actualSink.toEncoded().constData(), NULL);

    g_signal_emit_by_name(G_OBJECT(m_pipeline), CAPTURE_START, NULL);
}

void CameraBinSession::resumeVideoRecording()
{
    g_signal_emit_by_name(G_OBJECT(m_pipeline), CAPTURE_START, NULL);
}


void CameraBinSession::pauseVideoRecording()
{
    g_signal_emit_by_name(G_OBJECT(m_pipeline), CAPTURE_PAUSE, NULL);
}

void CameraBinSession::stopVideoRecording()
{
    g_signal_emit_by_name(G_OBJECT(m_pipeline), CAPTURE_STOP, NULL);
}

static void readValue(const GValue *value, QList< QPair<int,int> > *res, bool *continuous)
{
    if (GST_VALUE_HOLDS_FRACTION(value)) {
        int num = gst_value_get_fraction_numerator(value);
        int denum = gst_value_get_fraction_denominator(value);

        *res << QPair<int,int>(num, denum);
    } else if (GST_VALUE_HOLDS_FRACTION_RANGE(value)) {
        const GValue *rateValueMin = gst_value_get_fraction_range_min(value);
        const GValue *rateValueMax = gst_value_get_fraction_range_max(value);

        if (continuous)
            *continuous = true;

        readValue(rateValueMin, res, continuous);
        readValue(rateValueMax, res, continuous);
    } else if (GST_VALUE_HOLDS_LIST(value)) {
        for (uint i=0; i<gst_value_list_get_size(value); i++) {
            readValue(gst_value_list_get_value(value, i), res, continuous);
        }
    }
}

QList< QPair<int,int> > CameraBinSession::supportedFrameRates(const QSize &frameSize, bool *continuous) const
{
    QList< QPair<int,int> > res;

    if (!m_sourceCaps)
        return res;

    GstCaps *caps = 0;

    if (frameSize.isEmpty()) {
        caps = gst_caps_copy(m_sourceCaps);
    } else {
        GstCaps *filter = gst_caps_new_full(
                gst_structure_new(
                        "video/x-raw-rgb",
                        "width"     , G_TYPE_INT , frameSize.width(),
                        "height"    , G_TYPE_INT, frameSize.height(), NULL),
                gst_structure_new(
                        "video/x-raw-yuv",
                        "width"     , G_TYPE_INT, frameSize.width(),
                        "height"    , G_TYPE_INT, frameSize.height(), NULL),
                gst_structure_new(
                        "image/jpeg",
                        "width"     , G_TYPE_INT, frameSize.width(),
                        "height"    , G_TYPE_INT, frameSize.height(), NULL),
                NULL);

        caps = gst_caps_intersect(m_sourceCaps, filter);
        gst_caps_unref(filter);
    }

    //simplify to the list of rates only:
    gst_caps_make_writable(caps);
    for (uint i=0; i<gst_caps_get_size(caps); i++) {
        GstStructure *structure = gst_caps_get_structure(caps, i);
        gst_structure_set_name(structure, "video/x-raw-yuv");
        const GValue *oldRate = gst_structure_get_value(structure, "framerate");
        GValue rate = {0};
        g_value_init(&rate, G_VALUE_TYPE(oldRate));
        g_value_copy(oldRate, &rate);
        gst_structure_remove_all_fields(structure);
        gst_structure_set_value(structure, "framerate", &rate);
    }
    gst_caps_do_simplify(caps);


    for (uint i=0; i<gst_caps_get_size(caps); i++) {
        GstStructure *structure = gst_caps_get_structure(caps, i);
        const GValue *rateValue = gst_structure_get_value(structure, "framerate");
        readValue(rateValue, &res, continuous);
    }

    //qDebug() << "Supported rates:" << gst_caps_to_string(caps);
    //qDebug() << res;

    gst_caps_unref(caps);

    return res;
}

QList<QSize> CameraBinSession::supportedResolutions(QPair<int,int> rate, bool *continuous) const
{
    QList<QSize> res;

    if (continuous)
        *continuous = false;

    if (!m_sourceCaps)
        return res;

    GstCaps *caps = 0;

    if (rate.first <= 0 || rate.second <= 0) {
        caps = gst_caps_copy(m_sourceCaps);
    } else {
        GstCaps *filter = gst_caps_new_full(
                gst_structure_new(
                        "video/x-raw-rgb",
                        "framerate"     , GST_TYPE_FRACTION , rate.first, rate.second, NULL),
                gst_structure_new(
                        "video/x-raw-yuv",
                        "framerate"     , GST_TYPE_FRACTION , rate.first, rate.second, NULL),
                gst_structure_new(
                        "image/jpeg",
                        "framerate"     , GST_TYPE_FRACTION , rate.first, rate.second, NULL),
                NULL);

        caps = gst_caps_intersect(m_sourceCaps, filter);
        gst_caps_unref(filter);
    }

    //simplify to the list of resolutions only:
    gst_caps_make_writable(caps);
    for (uint i=0; i<gst_caps_get_size(caps); i++) {
        GstStructure *structure = gst_caps_get_structure(caps, i);
        gst_structure_set_name(structure, "video/x-raw-yuv");
        const GValue *oldW = gst_structure_get_value(structure, "width");
        const GValue *oldH = gst_structure_get_value(structure, "height");
        GValue w = {0};
        GValue h = {0};
        g_value_init(&w, G_VALUE_TYPE(oldW));
        g_value_init(&h, G_VALUE_TYPE(oldH));
        g_value_copy(oldW, &w);
        g_value_copy(oldH, &h);
        gst_structure_remove_all_fields(structure);
        gst_structure_set_value(structure, "width", &w);
        gst_structure_set_value(structure, "height", &h);
    }
    gst_caps_do_simplify(caps);

    for (uint i=0; i<gst_caps_get_size(caps); i++) {
        GstStructure *structure = gst_caps_get_structure(caps, i);
        const GValue *wValue = gst_structure_get_value(structure, "width");
        const GValue *hValue = gst_structure_get_value(structure, "height");

        if (g_value_type_compatible(G_VALUE_TYPE(wValue), G_TYPE_INT) &&
            g_value_type_compatible(G_VALUE_TYPE(hValue), G_TYPE_INT)) {
            int w = g_value_get_int(wValue);
            int h = g_value_get_int(hValue);
            res << QSize(w,h);
        } else if (GST_VALUE_HOLDS_INT_RANGE(wValue)) {
            int wMin = gst_value_get_int_range_min(wValue);
            int wMax = gst_value_get_int_range_max(wValue);
            int hMin = gst_value_get_int_range_min(hValue);
            int hMax = gst_value_get_int_range_max(hValue);

            if (continuous)
                *continuous = true;

            res << QSize(wMin,hMin);
            res << QSize(wMax,hMax);
        } else if (GST_VALUE_HOLDS_LIST(wValue)) {
        }
    }

    //qDebug() << "Supported resolutions:" << gst_caps_to_string(caps);
    //qDebug() << res;

    gst_caps_unref(caps);

    return res;
}
