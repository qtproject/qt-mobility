/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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

#include "qgstreamerplayersession.h"
#include "qgstreamerbushelper.h"

#include <QDebug>

QGstreamerVideoRendererInterface::~QGstreamerVideoRendererInterface()
{
}

QGstreamerPlayerSession::QGstreamerPlayerSession(QObject *parent)
    :QObject(parent),
     m_state(QMediaPlayer::StoppedState),
     m_mediaStatus(QMediaPlayer::UnknownMediaStatus),
     m_busHelper(0),
     m_playbin(0),
     m_bus(0),
     m_renderer(0),
     m_volume(100),
     m_playbackRate(1.0),
     m_muted(false),
     m_videoAvailable(false),
     m_seekable(false),
     m_lastPosition(0),
     m_duration(-1)
{
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        gst_init(NULL, NULL);
    }


    m_playbin = gst_element_factory_make("playbin", NULL);
    if (m_playbin != 0) {
        // Sort out messages
        m_bus = gst_element_get_bus(m_playbin);
        m_busHelper = new QGstreamerBusHelper(m_bus, this);
        connect(m_busHelper, SIGNAL(message(QGstreamerMessage)), SLOT(busMessage(QGstreamerMessage)));

        // Initial volume
        double volume = 1.0;
        g_object_get(G_OBJECT(m_playbin), "volume", &volume, NULL);
        m_volume = int(volume*100);
    }
}

QGstreamerPlayerSession::~QGstreamerPlayerSession()
{
    if (m_playbin) {
        stop();

        delete m_busHelper;
        gst_object_unref(GST_OBJECT(m_bus));
        gst_object_unref(GST_OBJECT(m_playbin));
    }
}

void QGstreamerPlayerSession::load(const QUrl &url)
{
    m_url = url;
    if (m_playbin) {
        m_tags.clear();
        emit tagsChanged();

        g_object_set(G_OBJECT(m_playbin), "uri", m_url.toString().toLocal8Bit().constData(), NULL);

        if (m_renderer)
            m_renderer->precessNewStream();
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

float QGstreamerPlayerSession::playbackRate() const
{
    return m_playbackRate;
}

void QGstreamerPlayerSession::setPlaybackRate(float rate)
{
    m_playbackRate = rate;

    if (m_playbin) {
        gst_element_seek(m_playbin, rate, GST_FORMAT_TIME,
                         GstSeekFlags(GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT),
                         GST_SEEK_TYPE_NONE,0,
                         GST_SEEK_TYPE_NONE,0 );
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

void QGstreamerPlayerSession::setVideoRenderer(QObject *videoOutput)
{
    m_renderer = qobject_cast<QGstreamerVideoRendererInterface*>(videoOutput);
    if (m_renderer)
        g_object_set(G_OBJECT(m_playbin), "video-sink", m_renderer->videoSink(), NULL);
    else
        g_object_set(G_OBJECT(m_playbin), "video-sink", 0, NULL);
}

bool QGstreamerPlayerSession::isVideoAvailable() const
{
    return m_videoAvailable;
}

bool QGstreamerPlayerSession::isSeekable() const
{
    return m_seekable;
}

void QGstreamerPlayerSession::play()
{
    if (m_playbin) {
        if (gst_element_set_state(m_playbin, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
            qWarning() << "GStreamer; Unable to play -" << m_url.toString();
            m_state = QMediaPlayer::StoppedState;
            m_mediaStatus = QMediaPlayer::InvalidMedia;

            emit stateChanged(m_state);
            emit mediaStatusChanged(m_mediaStatus);
        }
    }
}

void QGstreamerPlayerSession::pause()
{
    if (m_playbin)
        gst_element_set_state(m_playbin, GST_STATE_PAUSED);
}

void QGstreamerPlayerSession::stop()
{
    if (m_playbin)
        gst_element_set_state(m_playbin, GST_STATE_NULL);
}

void QGstreamerPlayerSession::seek(qint64 ms)
{
    if (m_playbin) {
        gint64  position = (gint64)ms * 1000000;
        gst_element_seek_simple(m_playbin, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, position);
    }
}

void QGstreamerPlayerSession::setVolume(int volume)
{
    m_volume = volume;
    emit volumeChanged(m_volume);

    if (!m_muted && m_playbin)
        g_object_set(G_OBJECT(m_playbin), "volume", m_volume/100.0, NULL);

}

void QGstreamerPlayerSession::setMuted(bool muted)
{
    m_muted = muted;
    g_object_set(G_OBJECT(m_playbin), "volume", (m_muted ? 0 : m_volume/100.0), NULL);
}

static void addTagToMap(const GstTagList *list,
                        const gchar *tag,
                        gpointer user_data)
{
    QMap<QByteArray, QVariant> *map = reinterpret_cast<QMap<QByteArray, QVariant>* >(user_data);

    GValue val;
    val.g_type = 0;
    //g_value_init(&val,G_TYPE_INVALID);
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

void QGstreamerPlayerSession::setMediaStatus(QMediaPlayer::MediaStatus status)
{
    if (m_mediaStatus != status) {
        m_mediaStatus = status;
        emit mediaStatusChanged(status);
    }
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

    } else {
        //tag message comes from elements inside playbin, not from playbin itself
        if (GST_MESSAGE_TYPE(gm) == GST_MESSAGE_TAG) {
            qDebug() << "tag message";
            GstTagList *tag_list;
            gst_message_parse_tag(gm, &tag_list);
            gst_tag_list_foreach(tag_list, addTagToMap, &m_tags);

            qDebug() << m_tags;

            emit tagsChanged();
        }

        if (GST_MESSAGE_SRC(gm) == GST_OBJECT_CAST(m_playbin)) {
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
                        setMediaStatus(QMediaPlayer::UnknownMediaStatus);
                        setSeekable(false);
                        break;
                    case GST_STATE_READY:
                        setMediaStatus(QMediaPlayer::LoadedMedia);
                        setSeekable(false);
                        break;
                    case GST_STATE_PAUSED:
                        if (m_state != QMediaPlayer::PausedState)
                            emit stateChanged(m_state = QMediaPlayer::PausedState);

                        setMediaStatus(QMediaPlayer::LoadedMedia);

                        //check for seekable
                        if (oldState == GST_STATE_READY) {
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

                            if (!qFuzzyCompare(m_playbackRate, float(1.0)))
                                setPlaybackRate(m_playbackRate);

                        }

                        break;
                    case GST_STATE_PLAYING:
                        if (oldState == GST_STATE_PAUSED)
                            getStreamsInfo();

                        if (m_state != QMediaPlayer::PlayingState)
                            emit stateChanged(m_state = QMediaPlayer::PlayingState);
                        break;
                    }
                }
                break;

            case GST_MESSAGE_EOS:
                if (m_state != QMediaPlayer::StoppedState)
                    emit stateChanged(m_state = QMediaPlayer::StoppedState);

                setMediaStatus(QMediaPlayer::EndOfMedia);

                emit playbackFinished();
                break;

            case GST_MESSAGE_TAG:
            case GST_MESSAGE_STREAM_STATUS:
            case GST_MESSAGE_UNKNOWN:
            case GST_MESSAGE_ERROR:
            case GST_MESSAGE_WARNING:
            case GST_MESSAGE_INFO:
                break;
            case GST_MESSAGE_BUFFERING:
                setMediaStatus(QMediaPlayer::BufferingMedia);
                break;
            case GST_MESSAGE_STATE_DIRTY:
            case GST_MESSAGE_STEP_DONE:
            case GST_MESSAGE_CLOCK_PROVIDE:
            case GST_MESSAGE_CLOCK_LOST:
            case GST_MESSAGE_NEW_CLOCK:
            case GST_MESSAGE_STRUCTURE_CHANGE:
            case GST_MESSAGE_APPLICATION:
            case GST_MESSAGE_ELEMENT:
            case GST_MESSAGE_SEGMENT_START:
            case GST_MESSAGE_SEGMENT_DONE:
            case GST_MESSAGE_LATENCY:
#if (GST_VERSION_MAJOR >= 0) &&  (GST_VERSION_MINOR >= 10) && (GST_VERSION_MICRO >= 13)
            case GST_MESSAGE_ASYNC_START:
            case GST_MESSAGE_ASYNC_DONE:
#endif
            case GST_MESSAGE_ANY:
                break;
            }
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
    enum {
        GST_STREAM_TYPE_UNKNOWN,
        GST_STREAM_TYPE_AUDIO,
        GST_STREAM_TYPE_VIDEO,
        GST_STREAM_TYPE_TEXT,
        GST_STREAM_TYPE_SUBPICTURE,
        GST_STREAM_TYPE_ELEMENT
    };

    GList*      streamInfo;
    g_object_get(G_OBJECT(m_playbin), "stream-info", &streamInfo, NULL);

    bool haveVideo = false;

    for (; streamInfo != 0; streamInfo = g_list_next(streamInfo)) {
        gint        type;
        GObject*    obj = G_OBJECT(streamInfo->data);

        g_object_get(obj, "type", &type, NULL);

        if (type == GST_STREAM_TYPE_VIDEO)
            haveVideo = true;
    }

    if (haveVideo != m_videoAvailable) {
        m_videoAvailable = haveVideo;
        emit videoAvailabilityChanged(m_videoAvailable);
    }
}
