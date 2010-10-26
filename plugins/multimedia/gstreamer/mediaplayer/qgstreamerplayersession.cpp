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

#include "qgstreamerplayersession.h"
#include "qgstreamerbushelper.h"

#include "qgstreamervideorendererinterface.h"

#include <gst/gstvalue.h>

#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>
#include <QtCore/qsize.h>
#include <QtCore/qdebug.h>

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6) || (GST_VERSION_MICRO > 20)
#define USE_PLAYBIN2
#endif

//#define DEBUG_PLAYBIN
//#define DEBUG_VO_BIN_DUMP

static gboolean new_segment_probe(GstObject *pad, GstEvent *event, guint * session)
{
    Q_UNUSED(pad);
    if (GST_EVENT_TYPE(event) == GST_EVENT_NEWSEGMENT)
        reinterpret_cast<QGstreamerPlayerSession*>(session)->processNewSegment(event);
    return TRUE;
}

static gboolean new_buffer_probe(GstObject *pad, GstBuffer *buffer, guint * session)
{
    Q_UNUSED(pad);
    reinterpret_cast<QGstreamerPlayerSession*>(session)->processNewBuffer(buffer);
    return TRUE;
}


typedef enum {
    GST_PLAY_FLAG_VIDEO         = 0x00000001,
    GST_PLAY_FLAG_AUDIO         = 0x00000002,
    GST_PLAY_FLAG_TEXT          = 0x00000004,
    GST_PLAY_FLAG_VIS           = 0x00000008,
    GST_PLAY_FLAG_SOFT_VOLUME   = 0x00000010,
    GST_PLAY_FLAG_NATIVE_AUDIO  = 0x00000020,
    GST_PLAY_FLAG_NATIVE_VIDEO  = 0x00000040,
    GST_PLAY_FLAG_DOWNLOAD      = 0x00000080,
    GST_PLAY_FLAG_BUFFERING     = 0x000000100
} GstPlayFlags;

QGstreamerPlayerSession::QGstreamerPlayerSession(QObject *parent)
    :QObject(parent),
     m_state(QMediaPlayer::StoppedState),
     m_pendingState(QMediaPlayer::StoppedState),
     m_busHelper(0),
     m_playbin(0),
     m_usePlaybin2(false),
     m_videoSink(0),
     m_pendingVideoSink(0),
     m_nullVideoSink(0),
     m_bus(0),
     m_videoOutput(0),
     m_renderer(0),
     m_volume(100),
     m_playbackRate(1.0),
     m_muted(false),
     m_audioAvailable(false),
     m_videoAvailable(false),
     m_seekable(false),
     m_lastPosition(0),
     m_duration(-1),
     m_resourceSet(0),
     m_audioResource(0),
     m_resourceState(NoResourceState)
{
#ifdef USE_PLAYBIN2
    m_playbin = gst_element_factory_make("playbin2", NULL);
#endif

    if (m_playbin) {
        m_usePlaybin2 = true;

        //GST_PLAY_FLAG_NATIVE_VIDEO omits configuration of ffmpegcolorspace and videoscale,
        //since those elements are included in the video output bin.
        int flags = 0;
        g_object_get(G_OBJECT(m_playbin), "flags", &flags, NULL);
        flags |= GST_PLAY_FLAG_NATIVE_VIDEO;
        g_object_set(G_OBJECT(m_playbin), "flags", flags, NULL);
    } else {
        m_usePlaybin2 = false;
        m_playbin = gst_element_factory_make("playbin", NULL);
    }

    m_videoOutputBin = gst_bin_new("video-output-bin");
    gst_object_ref(GST_OBJECT(m_videoOutputBin));

    m_videoIdentity = gst_element_factory_make("identity", "identity-vo");
    m_colorSpace = gst_element_factory_make("ffmpegcolorspace", "ffmpegcolorspace-vo");
    m_videoScale = gst_element_factory_make("videoscale","videoscale-vo");
    m_nullVideoSink = gst_element_factory_make("fakesink", NULL);
    gst_object_ref(GST_OBJECT(m_nullVideoSink));
    gst_bin_add_many(GST_BIN(m_videoOutputBin), m_videoIdentity, m_colorSpace, m_videoScale, m_nullVideoSink, NULL);
    gst_element_link_many(m_videoIdentity, m_colorSpace, m_videoScale, m_nullVideoSink, NULL);

    //add an event probe before video output to save and repost segment events
    {
        gst_segment_init (&m_segment, GST_FORMAT_TIME);

        GstPad *srcPad = gst_element_get_static_pad(m_videoIdentity, "src");
        gst_pad_add_event_probe(srcPad, G_CALLBACK(new_segment_probe), this);
        gst_pad_add_buffer_probe(srcPad, G_CALLBACK(new_buffer_probe), this);
        gst_object_unref(GST_OBJECT(srcPad));
    }


    m_videoSink = m_nullVideoSink;

    // add ghostpads
    GstPad *pad = gst_element_get_static_pad(m_videoIdentity,"sink");
    gst_element_add_pad(GST_ELEMENT(m_videoOutputBin), gst_ghost_pad_new("videosink", pad));
    gst_object_unref(GST_OBJECT(pad));

    if (m_playbin != 0) {
        // Sort out messages
        m_bus = gst_element_get_bus(m_playbin);
        m_busHelper = new QGstreamerBusHelper(m_bus, this);
        connect(m_busHelper, SIGNAL(message(QGstreamerMessage)), SLOT(busMessage(QGstreamerMessage)));
        m_busHelper->installSyncEventFilter(this);

        g_object_set(G_OBJECT(m_playbin), "video-sink", m_videoOutputBin, NULL);

        g_signal_connect(G_OBJECT(m_playbin), "notify::source", G_CALLBACK(playbinNotifySource), this);

        // Initial volume
        double volume = 1.0;
        g_object_get(G_OBJECT(m_playbin), "volume", &volume, NULL);
        m_volume = int(volume*100);
    }

    // resource policy awareness
    m_resourceSet = new ResourcePolicy::ResourceSet("player", this);
    m_resourceSet->setAlwaysReply();

    m_audioResource = new ResourcePolicy::AudioResource("player");
    m_audioResource->setProcessID(QCoreApplication::applicationPid());
    m_audioResource->setStreamTag("media.name", "*");
    m_resourceSet->addResourceObject(m_audioResource);

    connect(m_resourceSet, SIGNAL(resourcesGranted(const QList<ResourcePolicy::ResourceType>&)),
            this, SLOT(resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>&)));
    connect(m_resourceSet, SIGNAL(lostResources()), this, SLOT(resourceLostHandler()));
    connect(m_resourceSet, SIGNAL(resourcesReleased()), this, SLOT(resourceReleasedHandler()));
}

QGstreamerPlayerSession::~QGstreamerPlayerSession()
{
    if (m_playbin) {
        stop();

        delete m_busHelper;
        gst_object_unref(GST_OBJECT(m_bus));
        gst_object_unref(GST_OBJECT(m_playbin));
        gst_object_unref(GST_OBJECT(m_nullVideoSink));
        gst_object_unref(GST_OBJECT(m_videoOutputBin));
    }
}

void QGstreamerPlayerSession::load(const QNetworkRequest &request)
{
    m_request = request;

    if (m_playbin) {
        m_tags.clear();
        emit tagsChanged();

        g_object_set(G_OBJECT(m_playbin), "uri", m_request.url().toEncoded().constData(), NULL);

        if (!m_streamTypes.isEmpty()) {
            m_streamProperties.clear();
            m_streamTypes.clear();

            emit streamsChanged();
        }
    }
}

qint64 QGstreamerPlayerSession::duration() const
{
    return m_duration;
}

qint64 QGstreamerPlayerSession::position() const
{
    GstFormat   format = GST_FORMAT_TIME;
    gint64      position = 0;

    if ( m_playbin && gst_element_query_position(m_playbin, &format, &position))
        return position / 1000000;
    else
        return 0;
}

qreal QGstreamerPlayerSession::playbackRate() const
{
    return m_playbackRate;
}

void QGstreamerPlayerSession::setPlaybackRate(qreal rate)
{
    if (!qFuzzyCompare(m_playbackRate, rate)) {
        m_playbackRate = rate;
        if (m_playbin) {
            gst_element_seek(m_playbin, rate, GST_FORMAT_TIME,
                             GstSeekFlags(GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH),
                             GST_SEEK_TYPE_NONE,0,
                             GST_SEEK_TYPE_NONE,0 );
        }
    }
}

int QGstreamerPlayerSession::activeStream(QMediaStreamsControl::StreamType streamType) const
{
    int streamNumber = -1;
    if (m_playbin) {
        switch (streamType) {
        case QMediaStreamsControl::AudioStream:
            g_object_get(G_OBJECT(m_playbin), "current-audio", streamNumber, NULL);
            break;
        case QMediaStreamsControl::VideoStream:
            g_object_get(G_OBJECT(m_playbin), "current-video", streamNumber, NULL);
            break;
        case QMediaStreamsControl::SubPictureStream:
            g_object_get(G_OBJECT(m_playbin), "current-text", streamNumber, NULL);
            break;
        default:
            break;
        }
    }

    if (m_usePlaybin2 && streamNumber >= 0)
        streamNumber += m_playbin2StreamOffset.value(streamType,0);

    return streamNumber;
}

void QGstreamerPlayerSession::setActiveStream(QMediaStreamsControl::StreamType streamType, int streamNumber)
{

    if (m_usePlaybin2 && streamNumber >= 0)
        streamNumber -= m_playbin2StreamOffset.value(streamType,0);

    if (m_playbin) {
        switch (streamType) {
        case QMediaStreamsControl::AudioStream:
            g_object_set(G_OBJECT(m_playbin), "current-audio", &streamNumber, NULL);
            break;
        case QMediaStreamsControl::VideoStream:
            g_object_set(G_OBJECT(m_playbin), "current-video", &streamNumber, NULL);
            break;
        case QMediaStreamsControl::SubPictureStream:
            g_object_set(G_OBJECT(m_playbin), "current-text", &streamNumber, NULL);
            break;
        default:
            break;
        }
    }
}


bool QGstreamerPlayerSession::isBuffering() const
{
    return false;
}

int QGstreamerPlayerSession::bufferingProgress() const
{
    return 0;
}

int QGstreamerPlayerSession::volume() const
{
    return m_volume;
}

bool QGstreamerPlayerSession::isMuted() const
{
    return m_muted;
}

bool QGstreamerPlayerSession::isAudioAvailable() const
{
    return m_audioAvailable;
}

static void block_pad_cb(GstPad *pad, gboolean blocked, gpointer user_data)
{
    Q_UNUSED(pad);
#ifdef DEBUG_PLAYBIN
    qDebug() << "block_pad_cb, blocked:" << blocked;
#endif

    if (blocked && user_data) {
        QGstreamerPlayerSession *session = reinterpret_cast<QGstreamerPlayerSession*>(user_data);
        QMetaObject::invokeMethod(session, "finishVideoOutputChange", Qt::QueuedConnection);
    }
}

#ifdef DEBUG_VO_BIN_DUMP
    static int dumpNum = 0;
#endif

void QGstreamerPlayerSession::updateVideoRenderer()
{
#ifdef DEBUG_PLAYBIN
    qDebug() << "Video sink has chaged, reload video output";
#endif

    if (m_videoOutput)
        setVideoRenderer(m_videoOutput);
}

void QGstreamerPlayerSession::setVideoRenderer(QObject *videoOutput)
{
    if (m_videoOutput != videoOutput) {
        if (m_videoOutput) {
            disconnect(m_videoOutput, SIGNAL(sinkChanged()),
                       this, SLOT(updateVideoRenderer()));
            disconnect(m_videoOutput, SIGNAL(readyChanged(bool)),
                   this, SLOT(updateVideoRenderer()));
        }

        if (videoOutput) {
            connect(videoOutput, SIGNAL(sinkChanged()),
                    this, SLOT(updateVideoRenderer()));
            connect(videoOutput, SIGNAL(readyChanged(bool)),
                   this, SLOT(updateVideoRenderer()));
        }

        m_videoOutput = videoOutput;
    }

    QGstreamerVideoRendererInterface* renderer = qobject_cast<QGstreamerVideoRendererInterface*>(videoOutput);   

    m_renderer = renderer;

#ifdef DEBUG_VO_BIN_DUMP
    dumpNum++;

    _gst_debug_bin_to_dot_file(GST_BIN(m_playbin),
                                  GstDebugGraphDetails(GST_DEBUG_GRAPH_SHOW_ALL /* GST_DEBUG_GRAPH_SHOW_MEDIA_TYPE | GST_DEBUG_GRAPH_SHOW_NON_DEFAULT_PARAMS | GST_DEBUG_GRAPH_SHOW_STATES*/),
                                  QString("playbin_%1_set").arg(dumpNum).toAscii().constData());
#endif

    GstElement *videoSink = m_renderer ? m_renderer->videoSink() : m_nullVideoSink;
    if (!videoSink)
        videoSink = m_nullVideoSink;

#ifdef DEBUG_PLAYBIN
    qDebug() << "Set video output:" << videoOutput;
    qDebug() << "Current sink:" << (m_videoSink ? GST_ELEMENT_NAME(m_videoSink) : "") <<  m_videoSink
             << "pending:" << (m_pendingVideoSink ? GST_ELEMENT_NAME(m_pendingVideoSink) : "") << m_pendingVideoSink
             << "new sink:" << (videoSink ? GST_ELEMENT_NAME(videoSink) : "") << videoSink;
#endif

    if (m_pendingVideoSink == videoSink ||
        (m_pendingVideoSink == 0 && m_videoSink == videoSink)) {
#ifdef DEBUG_PLAYBIN
        qDebug() << "Video sink has not changed, skip video output reconfiguration";
#endif
        return;
    }

#ifdef DEBUG_PLAYBIN
    qDebug() << "Reconfigure video output";
#endif

    if (m_state == QMediaPlayer::StoppedState) {
#ifdef DEBUG_PLAYBIN
        qDebug() << "The pipeline has not started yet, pending state:" << m_pendingState;
#endif
        //the pipeline has not started yet
        m_pendingVideoSink = 0;        
        gst_element_set_state(m_videoSink, GST_STATE_NULL);
        gst_element_set_state(m_playbin, GST_STATE_NULL);
        gst_element_unlink(m_videoScale, m_videoSink);

        gst_bin_remove(GST_BIN(m_videoOutputBin), m_videoSink);

        m_videoSink = videoSink;

        gst_bin_add(GST_BIN(m_videoOutputBin), m_videoSink);
        gst_element_link(m_videoScale, m_videoSink);

        switch (m_pendingState) {
        case QMediaPlayer::PausedState:
            gst_element_set_state(m_playbin, GST_STATE_PAUSED);
            break;
        case QMediaPlayer::PlayingState:
            gst_element_set_state(m_playbin, GST_STATE_PLAYING);
            break;
        default:
            break;
        }
    } else {
        if (m_pendingVideoSink) {
#ifdef DEBUG_PLAYBIN
            qDebug() << "already waiting for pad to be blocked, just change the pending sink";
#endif
            m_pendingVideoSink = videoSink;
            return;
        }

        m_pendingVideoSink = videoSink;

#ifdef DEBUG_PLAYBIN
        qDebug() << "Blocking the video output pad...";
#endif

        {
#ifdef DEBUG_PLAYBIN
            qDebug() << "send the last new segment event to the video output...";
#endif
            GstEvent *event = gst_event_new_new_segment(TRUE,
                                                        m_segment.rate,
                                                        m_segment.format,
                                                        m_segment.last_stop, //start
                                                        m_segment.stop,
                                                        m_segment.last_stop);//position

            GstPad *pad = gst_element_get_static_pad(videoSink, "sink");
            //gst_pad_send_event(pad, m_lastSegmentEvent);
            gst_pad_send_event(pad, event);
            gst_object_unref(GST_OBJECT(pad));
        }

        //block pads, async to avoid locking in paused state
        GstPad *srcPad = gst_element_get_static_pad(m_videoIdentity, "src");
        gst_pad_set_blocked_async(srcPad, true, &block_pad_cb, this);
        gst_object_unref(GST_OBJECT(srcPad));
    }
}

void QGstreamerPlayerSession::finishVideoOutputChange()
{
    if (!m_pendingVideoSink)
        return;

#ifdef DEBUG_PLAYBIN
    qDebug() << "finishVideoOutputChange" << m_pendingVideoSink;
#endif

    GstPad *srcPad = gst_element_get_static_pad(m_videoIdentity, "src");

    if (!gst_pad_is_blocked(srcPad)) {
        //pad is not blocked, it's possible to swap outputs only in the null state
        qWarning() << "Pad is not blocked yet, could not switch video sink";
        GstState identityElementState = GST_STATE_NULL;
        gst_element_get_state(m_videoIdentity, &identityElementState, NULL, GST_CLOCK_TIME_NONE);
        if (identityElementState != GST_STATE_NULL) {
            gst_object_unref(GST_OBJECT(srcPad));
            return; //can't change vo yet, received async call from the previous change
        }

    }

    if (m_pendingVideoSink == m_videoSink) {
        //video output was change back to the current one,
        //no need to torment the pipeline, just unblock the pad
        if (gst_pad_is_blocked(srcPad))
            gst_pad_set_blocked_async(srcPad, false, &block_pad_cb, 0);

        m_pendingVideoSink = 0;
        gst_object_unref(GST_OBJECT(srcPad));
        return;
    }  

    gst_element_set_state(m_colorSpace, GST_STATE_NULL);
    gst_element_set_state(m_videoScale, GST_STATE_NULL);
    gst_element_set_state(m_videoSink, GST_STATE_NULL);

    gst_element_unlink(m_videoScale, m_videoSink);

    gst_bin_remove(GST_BIN(m_videoOutputBin), m_videoSink);

    m_videoSink = m_pendingVideoSink;
    m_pendingVideoSink = 0;

    gst_bin_add(GST_BIN(m_videoOutputBin), m_videoSink);
    if (!gst_element_link(m_videoScale, m_videoSink))
        qWarning() << "Linking video output element failed";

    GstState state;

    switch (m_pendingState) {
    case QMediaPlayer::StoppedState:
        state = GST_STATE_NULL;
        break;
    case QMediaPlayer::PausedState:
        state = GST_STATE_PAUSED;
        break;
    case QMediaPlayer::PlayingState:
        state = GST_STATE_PLAYING;
        break;
    }

    gst_element_set_state(m_colorSpace, state);
    gst_element_set_state(m_videoScale, state);
    gst_element_set_state(m_videoSink, state);    

    // Set state change that was deferred due the video output
    // change being pending
    gst_element_set_state(m_playbin, state);

    //don't have to wait here, it will unblock eventually
    if (gst_pad_is_blocked(srcPad))
        gst_pad_set_blocked_async(srcPad, false, &block_pad_cb, 0);
    gst_object_unref(GST_OBJECT(srcPad));

#ifdef DEBUG_VO_BIN_DUMP
    dumpNum++;
    _gst_debug_bin_to_dot_file(GST_BIN(m_playbin),
                                  GstDebugGraphDetails(/*GST_DEBUG_GRAPH_SHOW_ALL */ GST_DEBUG_GRAPH_SHOW_MEDIA_TYPE | GST_DEBUG_GRAPH_SHOW_NON_DEFAULT_PARAMS | GST_DEBUG_GRAPH_SHOW_STATES),
                                  QString("playbin_%1_finish").arg(dumpNum).toAscii().constData());
#endif
}

void QGstreamerPlayerSession::processNewSegment(GstEvent *event)
{
    gboolean update;
    GstFormat format;
    gdouble rate, arate;
    gint64 start, stop, time;

    gst_event_parse_new_segment_full(event, &update, &rate, &arate, &format,
                                      &start, &stop, &time);

    gst_segment_set_newsegment_full(&m_segment, update,
              rate, arate, format, start, stop, time);
}

void QGstreamerPlayerSession::processNewBuffer(GstBuffer *buf)
{
    GstClockTime last_stop, duration;
    last_stop = GST_BUFFER_TIMESTAMP (buf);
    if (GST_CLOCK_TIME_IS_VALID (last_stop)) {
        duration = GST_BUFFER_DURATION (buf);
        if (GST_CLOCK_TIME_IS_VALID (duration)) {
            last_stop += duration;
        }
        gst_segment_set_last_stop(&m_segment, m_segment.format, last_stop);
    }
}

bool QGstreamerPlayerSession::isVideoAvailable() const
{
    return m_videoAvailable;
}

bool QGstreamerPlayerSession::isSeekable() const
{
    return m_seekable;
}

bool QGstreamerPlayerSession::play()
{
    if (m_playbin) {
        if (m_resourceState == NoResourceState) {
            m_resourceState = PendingResourceState;
            m_resourceSet->acquire();
            return true;
        }
        else if (m_resourceState == HasResourceState) {
            return doPlay();
        }
    }

    return false;
}

bool QGstreamerPlayerSession::pause()
{
    if (m_playbin) {
        m_pendingState = QMediaPlayer::PausedState;
        if (m_pendingVideoSink != 0) {
            if (m_resourceState == HasResourceState)
                m_resourceSet->release();
            return true;
        }

        if (gst_element_set_state(m_playbin, GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE) {
            qWarning() << "GStreamer; Unable to play -" << m_request.url().toString();
            m_pendingState = m_state = QMediaPlayer::StoppedState;

            emit stateChanged(m_state);
            emit error(int(QMediaPlayer::ResourceError), tr("Unable to play %1").arg(m_request.url().path()));
        } else {
            if (m_resourceState == HasResourceState)
                m_resourceSet->release();
            return true;
        }
    }

    return false;
}

void QGstreamerPlayerSession::stop()
{
    if (m_playbin) {
        gst_element_set_state(m_playbin, GST_STATE_NULL);

        QMediaPlayer::State oldState = QMediaPlayer::StoppedState;
        m_pendingState = m_state = QMediaPlayer::StoppedState;

        finishVideoOutputChange();

        //we have to do it here, since gstreamer will not emit bus messages any more
        if (oldState != m_state)
            emit stateChanged(m_state);

        // release the resource
        if (m_resourceState != NoResourceState)
            m_resourceSet->release();
    }
}

bool QGstreamerPlayerSession::seek(qint64 ms)
{
    //seek locks when the video output sink is changing and pad is blocked
    if (m_playbin && !m_pendingVideoSink && m_state != QMediaPlayer::StoppedState) {
        gint64  position = qMax(ms,qint64(0)) * 1000000;
        return gst_element_seek(m_playbin,
                                m_playbackRate,
                                GST_FORMAT_TIME,
                                GstSeekFlags(GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH),
                                GST_SEEK_TYPE_SET,
                                position,
                                GST_SEEK_TYPE_NONE,
                                0);
    }

    return false;
}

void QGstreamerPlayerSession::setVolume(int volume)
{
    if (m_volume != volume) {
        m_volume = volume;

        if (m_playbin) {
            //playbin2 allows to set volume and muted independently,
            //with playbin1 it's necessary to keep volume at 0 while muted
            if (!m_muted || m_usePlaybin2)
                g_object_set(G_OBJECT(m_playbin), "volume", m_volume/100.0, NULL);
        }

        emit volumeChanged(m_volume);
    }
}

void QGstreamerPlayerSession::setMuted(bool muted)
{
    if (m_muted != muted) {
        m_muted = muted;

        if (m_usePlaybin2)
            g_object_set(G_OBJECT(m_playbin), "mute", m_muted, NULL);
        else
            g_object_set(G_OBJECT(m_playbin), "volume", (m_muted ? 0 : m_volume/100.0), NULL);

        emit mutedStateChanged(m_muted);
    }
}

static void addTagToMap(const GstTagList *list,
                        const gchar *tag,
                        gpointer user_data)
{
    QMap<QByteArray, QVariant> *map = reinterpret_cast<QMap<QByteArray, QVariant>* >(user_data);

    GValue val;
    val.g_type = 0;
    gst_tag_list_copy_value(&val,list,tag);

    switch( G_VALUE_TYPE(&val) ) {
        case G_TYPE_STRING:
        {
            const gchar *str_value = g_value_get_string(&val);
            map->insert(QByteArray(tag), QString::fromUtf8(str_value));
            break;
        }
        case G_TYPE_INT:
            map->insert(QByteArray(tag), g_value_get_int(&val));
            break;
        case G_TYPE_UINT:
            map->insert(QByteArray(tag), g_value_get_uint(&val));
            break;
        case G_TYPE_LONG:
            map->insert(QByteArray(tag), qint64(g_value_get_long(&val)));
            break;
        case G_TYPE_BOOLEAN:
            map->insert(QByteArray(tag), g_value_get_boolean(&val));
            break;
        case G_TYPE_CHAR:
            map->insert(QByteArray(tag), g_value_get_char(&val));
            break;
        case G_TYPE_DOUBLE:
            map->insert(QByteArray(tag), g_value_get_double(&val));
            break;
        default:
            // GST_TYPE_DATE is a function, not a constant, so pull it out of the switch
            if (G_VALUE_TYPE(&val) == GST_TYPE_DATE) {
                const GDate *date = gst_value_get_date(&val);
                if (g_date_valid(date)) {
                    int year = g_date_get_year(date);
                    int month = g_date_get_month(date);
                    int day = g_date_get_day(date);
                    map->insert(QByteArray(tag), QDate(year,month,day));
                    if (!map->contains("year"))
                        map->insert("year", year);
                }
            } else if (G_VALUE_TYPE(&val) == GST_TYPE_FRACTION) {
                int nom = gst_value_get_fraction_numerator(&val);
                int denom = gst_value_get_fraction_denominator(&val);

                if (denom > 0) {
                    map->insert(QByteArray(tag), double(nom)/denom);
                }
            }
            break;
    }

    g_value_unset(&val);
}

void QGstreamerPlayerSession::setSeekable(bool seekable)
{
    if (seekable != m_seekable) {
        m_seekable = seekable;
        emit seekableChanged(m_seekable);
    }
}

bool QGstreamerPlayerSession::processSyncMessage(const QGstreamerMessage &message)
{
    GstMessage* gm = message.rawMessage();

    if (gm &&
        GST_MESSAGE_TYPE(gm) == GST_MESSAGE_ELEMENT &&
        gst_structure_has_name(gm->structure, "prepare-xwindow-id"))
    {
        if (m_renderer)
            m_renderer->precessNewStream();
        return true;
    }

    return false;
}

void QGstreamerPlayerSession::busMessage(const QGstreamerMessage &message)
{
    GstMessage* gm = message.rawMessage();

    if (gm == 0) {
        // Null message, query current position
        quint32 newPos = position();

        if (newPos/1000 != m_lastPosition) {
            m_lastPosition = newPos/1000;
            emit positionChanged(newPos);
        }

        double volume = 1.0;
        g_object_get(G_OBJECT(m_playbin), "volume", &volume, NULL);
        if (m_volume != int(volume*100)) {
            m_volume = int(volume*100);
            emit volumeChanged(m_volume);
        }

    } else {
#ifdef DEBUG_PLAYBIN
        qDebug() << "Object:" << GST_OBJECT_NAME((GST_MESSAGE_SRC(gm))) << "has sent Message:" << gst_message_type_get_name(GST_MESSAGE_TYPE(gm));
#endif
        //tag message comes from elements inside playbin, not from playbin itself
        if (GST_MESSAGE_TYPE(gm) == GST_MESSAGE_TAG) {
            //qDebug() << "tag message";
            GstTagList *tag_list;
            gst_message_parse_tag(gm, &tag_list);
            gst_tag_list_foreach(tag_list, addTagToMap, &m_tags);

            //qDebug() << m_tags;

            emit tagsChanged();
        }

        if (GST_MESSAGE_SRC(gm) == GST_OBJECT_CAST(m_playbin)) {
            switch (GST_MESSAGE_TYPE(gm))  {
            case GST_MESSAGE_STATE_CHANGED:
                {
                    GstState    oldState;
                    GstState    newState;
                    GstState    pending;

                    gst_message_parse_state_changed(gm, &oldState, &newState, &pending);

#ifdef DEBUG_PLAYBIN
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
                        setSeekable(false);
                        finishVideoOutputChange();
                        if (m_state != QMediaPlayer::StoppedState)
                            emit stateChanged(m_state = QMediaPlayer::StoppedState);
                        break;
                    case GST_STATE_READY:
                        setSeekable(false);
                        if (m_state != QMediaPlayer::StoppedState)
                            emit stateChanged(m_state = QMediaPlayer::StoppedState);
                        break;
                    case GST_STATE_PAUSED:
                    {
                        QMediaPlayer::State prevState = m_state;
                        m_state = QMediaPlayer::PausedState;

                        //check for seekable
                        if (oldState == GST_STATE_READY) {
                            getStreamsInfo();
                            updateVideoResolutionTag();

                            /*
                                //gst_element_seek_simple doesn't work reliably here, have to find a better solution

                                GstFormat   format = GST_FORMAT_TIME;
                                gint64      position = 0;
                                bool seekable = false;
                                if (gst_element_query_position(m_playbin, &format, &position)) {
                                    seekable = gst_element_seek_simple(m_playbin, format, GST_SEEK_FLAG_NONE, position);
                                }

                                setSeekable(seekable);
                                */

                            setSeekable(true);

                            if (!qFuzzyCompare(m_playbackRate, qreal(1.0))) {
                                qreal rate = m_playbackRate;
                                m_playbackRate = 1.0;
                                setPlaybackRate(rate);
                            }
                        }

                        if (m_state != prevState)
                            emit stateChanged(m_state);

                        break;
                    }
                    case GST_STATE_PLAYING:
                        if (m_state != QMediaPlayer::PlayingState)
                            emit stateChanged(m_state = QMediaPlayer::PlayingState);

                        break;
                    }
                }
                break;

            case GST_MESSAGE_EOS:
                emit playbackFinished();
                break;

            case GST_MESSAGE_TAG:
            case GST_MESSAGE_STREAM_STATUS:
            case GST_MESSAGE_UNKNOWN:
                break;
            case GST_MESSAGE_ERROR: {
                    GError *err;
                    gchar *debug;
                    gst_message_parse_error(gm, &err, &debug);
                    if (err->code == GST_STREAM_ERROR_CODEC_NOT_FOUND)
                        emit error(int(QMediaPlayer::FormatError), QString::fromLatin1("Cannot play stream of type: <unknown>"));
                    else
                        emit error(int(QMediaPlayer::ResourceError), QString::fromUtf8(err->message));
#ifdef DEBUG_PLAYBIN
                    qDebug() << "Error:" << QString::fromUtf8(err->message);
#endif
                    g_error_free(err);
                    g_free(debug);
                    stop();
                }
                break;
            case GST_MESSAGE_WARNING:
                {
                    GError *err;
                    gchar *debug;
                    gst_message_parse_warning (gm, &err, &debug);
                    qWarning() << "Warning:" << QString::fromUtf8(err->message);
                    g_error_free (err);
                    g_free (debug);
                }
                break;
            case GST_MESSAGE_INFO:
#ifdef DEBUG_PLAYBIN
                {
                    GError *err;
                    gchar *debug;
                    gst_message_parse_info (gm, &err, &debug);
                    qDebug() << "Info:" << QString::fromUtf8(err->message);
                    g_error_free (err);
                    g_free (debug);
                }
#endif
                break;
            case GST_MESSAGE_BUFFERING:
                {
                    int progress = 0;
                    gst_message_parse_buffering(gm, &progress);
                    emit bufferingProgressChanged(progress);
                }
                break;
            case GST_MESSAGE_STATE_DIRTY:
            case GST_MESSAGE_STEP_DONE:
            case GST_MESSAGE_CLOCK_PROVIDE:
            case GST_MESSAGE_CLOCK_LOST:
            case GST_MESSAGE_NEW_CLOCK:
            case GST_MESSAGE_STRUCTURE_CHANGE:
            case GST_MESSAGE_APPLICATION:
            case GST_MESSAGE_ELEMENT:
                break;
            case GST_MESSAGE_SEGMENT_START:
                {
                    const GstStructure *structure = gst_message_get_structure(gm);
                    qint64 position = g_value_get_int64(gst_structure_get_value(structure, "position"));
                    position /= 1000000;
                    m_lastPosition = position;
                    emit positionChanged(position);
                }
                break;
            case GST_MESSAGE_SEGMENT_DONE:
                break;
            case GST_MESSAGE_DURATION:
                {
                    GstFormat   format = GST_FORMAT_TIME;
                    gint64      duration = 0;

                    if (gst_element_query_duration(m_playbin, &format, &duration)) {
                        int newDuration = duration / 1000000;
                        if (m_duration != newDuration) {
                            m_duration = newDuration;
                            emit durationChanged(m_duration);
                        }
                    }
                }
                break;
            case GST_MESSAGE_LATENCY:
#if (GST_VERSION_MAJOR >= 0) &&  (GST_VERSION_MINOR >= 10) && (GST_VERSION_MICRO >= 13)
            case GST_MESSAGE_ASYNC_START:
            case GST_MESSAGE_ASYNC_DONE:
#if GST_VERSION_MICRO >= 23
            case GST_MESSAGE_REQUEST_STATE:
#endif
#endif
            case GST_MESSAGE_ANY:
                break;
            default:
                break;
            }
        } else if (m_videoSink
                   && m_renderer
                   && GST_MESSAGE_SRC(gm) == GST_OBJECT_CAST(m_videoSink)
                   && GST_MESSAGE_TYPE(gm) == GST_MESSAGE_STATE_CHANGED) {
            GstState oldState;
            GstState newState;
            gst_message_parse_state_changed(gm, &oldState, &newState, 0);

            if (oldState == GST_STATE_READY && newState == GST_STATE_PAUSED)
                m_renderer->precessNewStream();
        } else if (GST_MESSAGE_TYPE(gm) == GST_MESSAGE_ERROR) {
            // If the source has given up, so do we.
            if (qstrcmp(GST_OBJECT_NAME(GST_MESSAGE_SRC(gm)), "source") == 0) {
                GError *err;
                gchar *debug;
                gst_message_parse_error(gm, &err, &debug);
                emit error(int(QMediaPlayer::ResourceError), QString::fromUtf8(err->message));
#ifdef DEBUG_PLAYBIN
                qDebug() << "Error:" << QString::fromUtf8(err->message);
#endif
                g_error_free(err);
                g_free(debug);
                stop();
            }
        } else if (m_usePlaybin2 && GST_MESSAGE_TYPE(gm) == GST_MESSAGE_WARNING) {
            GError *err;
            gchar *debug;
            gst_message_parse_warning(gm, &err, &debug);
            if (err->code == GST_STREAM_ERROR_CODEC_NOT_FOUND)
                emit error(int(QMediaPlayer::FormatError), QString::fromLatin1("Cannot play stream of type: <unknown>"));
#ifdef DEBUG_PLAYBIN
            qDebug() << "Warning:" << QString::fromUtf8(err->message);
#endif
            g_error_free(err);
            g_free(debug);
        }
    }
}

void QGstreamerPlayerSession::getStreamsInfo()
{
    GstFormat   format = GST_FORMAT_TIME;
    gint64      duration = 0;

    if (gst_element_query_duration(m_playbin, &format, &duration)) {
        int newDuration = duration / 1000000;
        if (m_duration != newDuration) {
            m_duration = newDuration;
            emit durationChanged(m_duration);
        }
    }

    //check if video is available:
    bool haveAudio = false;
    bool haveVideo = false;
    m_streamProperties.clear();
    m_streamTypes.clear();

    if (m_usePlaybin2) {
        gint audioStreamsCount = 0;
        gint videoStreamsCount = 0;
        gint textStreamsCount = 0;

        g_object_get(G_OBJECT(m_playbin), "n-audio", &audioStreamsCount, NULL);
        g_object_get(G_OBJECT(m_playbin), "n-video", &videoStreamsCount, NULL);
        g_object_get(G_OBJECT(m_playbin), "n-text", &textStreamsCount, NULL);

        haveAudio = audioStreamsCount > 0;
        haveVideo = videoStreamsCount > 0;

        m_playbin2StreamOffset[QMediaStreamsControl::AudioStream] = 0;
        m_playbin2StreamOffset[QMediaStreamsControl::VideoStream] = audioStreamsCount;
        m_playbin2StreamOffset[QMediaStreamsControl::SubPictureStream] = audioStreamsCount+videoStreamsCount;

        for (int i=0; i<audioStreamsCount; i++)
            m_streamTypes.append(QMediaStreamsControl::AudioStream);

        for (int i=0; i<videoStreamsCount; i++)
            m_streamTypes.append(QMediaStreamsControl::VideoStream);

        for (int i=0; i<textStreamsCount; i++)
            m_streamTypes.append(QMediaStreamsControl::SubPictureStream);

        for (int i=0; i<m_streamTypes.count(); i++) {
            QMediaStreamsControl::StreamType streamType = m_streamTypes[i];
            QMap<QtMultimediaKit::MetaData, QVariant> streamProperties;

            int streamIndex = i - m_playbin2StreamOffset[streamType];

            GstTagList *tags = 0;
            switch (streamType) {
            case QMediaStreamsControl::AudioStream:
                g_signal_emit_by_name(G_OBJECT(m_playbin), "get-audio-tags", streamIndex, &tags);
                break;
            case QMediaStreamsControl::VideoStream:
                g_signal_emit_by_name(G_OBJECT(m_playbin), "get-video-tags", streamIndex, &tags);
                break;
            case QMediaStreamsControl::SubPictureStream:
                g_signal_emit_by_name(G_OBJECT(m_playbin), "get-text-tags", streamIndex, &tags);
                break;
            default:
                break;
            }

            if (tags && gst_is_tag_list(tags)) {
                gchar *languageCode = 0;
                if (gst_tag_list_get_string(tags, GST_TAG_LANGUAGE_CODE, &languageCode))
                    streamProperties[QtMultimediaKit::Language] = QString::fromUtf8(languageCode);

                //qDebug() << "language for setream" << i << QString::fromUtf8(languageCode);
                g_free (languageCode);
            }

            m_streamProperties.append(streamProperties);
        }
    } else { // PlayBin 1
        enum {
            GST_STREAM_TYPE_UNKNOWN,
            GST_STREAM_TYPE_AUDIO,
            GST_STREAM_TYPE_VIDEO,
            GST_STREAM_TYPE_TEXT,
            GST_STREAM_TYPE_SUBPICTURE,
            GST_STREAM_TYPE_ELEMENT
        };

        GList*      streamInfoList;
        g_object_get(G_OBJECT(m_playbin), "stream-info", &streamInfoList, NULL);

        for (; streamInfoList != 0; streamInfoList = g_list_next(streamInfoList)) {
            gint        type;
            gchar *languageCode = 0;

            GObject*    streamInfo = G_OBJECT(streamInfoList->data);

            g_object_get(streamInfo, "type", &type, NULL);
            g_object_get(streamInfo, "language-code", &languageCode, NULL);

            QMediaStreamsControl::StreamType streamType = QMediaStreamsControl::UnknownStream;

            switch (type) {
            case GST_STREAM_TYPE_VIDEO:
                streamType = QMediaStreamsControl::VideoStream;
                haveVideo = true;
                break;
            case GST_STREAM_TYPE_AUDIO:
                streamType = QMediaStreamsControl::AudioStream;
                haveAudio = true;
                break;
            case GST_STREAM_TYPE_SUBPICTURE:
                streamType = QMediaStreamsControl::SubPictureStream;
                break;
            case GST_STREAM_TYPE_UNKNOWN: {
                GstCaps *caps = 0;
                g_object_get(streamInfo, "caps", &caps, NULL);
                const GstStructure *structure = gst_caps_get_structure(caps, 0);
                const gchar *media_type = gst_structure_get_name(structure);
                emit error(int(QMediaPlayer::FormatError), QString::fromLatin1("Cannot play stream of type: %1").arg(QString::fromUtf8(media_type)));
#ifdef DEBUG_PLAYBIN
                qDebug() << "Encountered unknown stream type";
#endif
                gst_caps_unref(caps);
            }
            default:
                streamType = QMediaStreamsControl::UnknownStream;
                break;
            }

            QMap<QtMultimediaKit::MetaData, QVariant> streamProperties;
            streamProperties[QtMultimediaKit::Language] = QString::fromUtf8(languageCode);

            m_streamProperties.append(streamProperties);
            m_streamTypes.append(streamType);
        }
    }


    if (haveAudio != m_audioAvailable) {
        m_audioAvailable = haveAudio;
        emit audioAvailableChanged(m_audioAvailable);
    }
    if (haveVideo != m_videoAvailable) {
        m_videoAvailable = haveVideo;
        emit videoAvailableChanged(m_videoAvailable);
    }

    emit streamsChanged();
}

void QGstreamerPlayerSession::updateVideoResolutionTag()
{
    QSize size;
    QSize aspectRatio;

    GstPad *pad = gst_element_get_static_pad(m_videoIdentity, "src");
    GstCaps *caps = gst_pad_get_negotiated_caps(pad);

    if (caps) {
        const GstStructure *structure = gst_caps_get_structure(caps, 0);
        gst_structure_get_int(structure, "width", &size.rwidth());
        gst_structure_get_int(structure, "height", &size.rheight());

        gint aspectNum = 0;
        gint aspectDenum = 0;
        if (!size.isEmpty() && gst_structure_get_fraction(
                    structure, "pixel-aspect-ratio", &aspectNum, &aspectDenum)) {
            if (aspectDenum > 0)
                aspectRatio = QSize(aspectNum, aspectDenum);
        }
        gst_caps_unref(caps);
    }

    gst_object_unref(GST_OBJECT(pad));

    QSize currentSize = m_tags.value("resolution").toSize();
    QSize currentAspectRatio = m_tags.value("pixel-aspect-ratio").toSize();

    if (currentSize != size || currentAspectRatio != aspectRatio) {
        if (aspectRatio.isEmpty())
            m_tags.remove("pixel-aspect-ratio");

        if (size.isEmpty()) {
            m_tags.remove("resolution");
        } else {
            m_tags.insert("resolution", QVariant(size));
            if (!aspectRatio.isEmpty())
                m_tags.insert("pixel-aspect-ratio", QVariant(aspectRatio));
        }

        emit tagsChanged();
    }
}

bool QGstreamerPlayerSession::doPlay()
{
    if (m_playbin && m_resourceState == HasResourceState) {
        m_pendingState = QMediaPlayer::PlayingState;
        if (gst_element_set_state(m_playbin, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
            qWarning() << "GStreamer; Unable to play -" << m_request.url().toString();
            m_pendingState = m_state = QMediaPlayer::StoppedState;

            emit stateChanged(m_state);
            emit error(int(QMediaPlayer::ResourceError), tr("Unable to play %1").arg(m_request.url().path()));
        } else
            return true;
    }

    return false;
}

void QGstreamerPlayerSession::resourceAcquiredHandler(const QList<ResourcePolicy::ResourceType>&
                                                      /*grantedOptionalResList*/)
{
    if (m_resourceState == PendingResourceState) {
        m_resourceState = HasResourceState;
        doPlay();
    }
}

void QGstreamerPlayerSession::resourceReleasedHandler()
{
    m_resourceState = NoResourceState;
}

void QGstreamerPlayerSession::resourceLostHandler()
{
    if (m_resourceState != NoResourceState) {
        m_resourceState = PendingResourceState;
        pause();
    }
}

void QGstreamerPlayerSession::playbinNotifySource(GObject *o, GParamSpec *p, gpointer d)
{
    Q_UNUSED(p);

    GstElement *source = 0;
    g_object_get(o, "source", &source, NULL);
    if (source == 0)
        return;

    // Turn off icecast metadata request, will be re-set if in QNetworkRequest
    // (souphttpsrc docs say is false by default, but header appears in request
    // @version 0.10.21)
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(source), "iradio-mode") != 0)
        g_object_set(G_OBJECT(source), "iradio-mode", FALSE, NULL);


    // Set Headers
    const QByteArray userAgentString("User-Agent");

    QGstreamerPlayerSession *self = reinterpret_cast<QGstreamerPlayerSession *>(d);

    // User-Agent - special case, souphhtpsrc will always set something, even if
    // defined in extra-headers
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(source), "user-agent") != 0) {
        g_object_set(G_OBJECT(source), "user-agent",
                     self->m_request.rawHeader(userAgentString).constData(), NULL);
    }

    // The rest
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(source), "extra-headers") != 0) {
        GstStructure *extras = gst_structure_empty_new("extras");

        foreach (const QByteArray &rawHeader, self->m_request.rawHeaderList()) {
            if (rawHeader == userAgentString) // Filter User-Agent
                continue;
            else {
                GValue headerValue;

                memset(&headerValue, 0, sizeof(GValue));
                g_value_init(&headerValue, G_TYPE_STRING);

                g_value_set_string(&headerValue,
                                   self->m_request.rawHeader(rawHeader).constData());

                gst_structure_set_value(extras, rawHeader.constData(), &headerValue);
            }
        }

        if (gst_structure_n_fields(extras) > 0)
            g_object_set(G_OBJECT(source), "extra-headers", extras, NULL);

        gst_structure_free(extras);
    }

    gst_object_unref(source);

    // NOTE: code assumes souphttpsrc, but written so anything with "extra-headers"
    // should be functional.
}

