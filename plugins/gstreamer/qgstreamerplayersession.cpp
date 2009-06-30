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

#include "qgstreamerplayersession.h"
#include "qgstreamerbushelper.h"

#include <QDebug>

QGstreamerPlayerSession::QGstreamerPlayerSession(QObject *parent)
    :QObject(parent),     
     m_state(QGstreamerPlayerControl::Stopped),
     m_busHelper(0),
     m_playbin(0),
     m_bus(0),
     m_videoOutput(0),
     m_volume(1.0),
     m_muted(false),
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
        g_object_get(G_OBJECT(m_playbin), "volume", &m_volume, NULL);
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
        g_object_set(G_OBJECT(m_playbin), "uri", m_url.toString().toLocal8Bit().constData(), NULL);
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

bool QGstreamerPlayerSession::isBuffering() const
{
    return false;
}

int QGstreamerPlayerSession::bufferingProgress() const
{
    return 0;
}

double QGstreamerPlayerSession::volume() const
{
    return m_volume;
}

bool QGstreamerPlayerSession::isMuted() const
{
    return m_muted;
}

QObject *QGstreamerPlayerSession::videoOutput() const
{
    return m_videoOutput;
}

bool QGstreamerPlayerSession::setVideoOutput(QObject *videoOutput)
{
    m_videoOutput = videoOutput;

    //g_object_set(G_OBJECT(d->playbin), "video-sink", d->sinkWidget->element(), NULL);
    return true;
}

bool QGstreamerPlayerSession::isVideoAvailable() const
{
    return true;
}

void QGstreamerPlayerSession::play()
{
    if (m_playbin) {
        if (gst_element_set_state(m_playbin, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
            qWarning() << "GStreamer; Unable to play -" << m_url.toString();
            m_state = QMediaPlayerControl::Error;
            emit stateChanged(m_state);
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

void QGstreamerPlayerSession::setVolume(double volume)
{
    m_volume = volume;
    emit volumeChanged(m_volume);

    if (!m_muted && m_playbin)
        g_object_set(G_OBJECT(m_playbin), "volume", m_volume, NULL);

}

void QGstreamerPlayerSession::setMuted(bool muted)
{
    m_muted = muted;
    g_object_set(G_OBJECT(m_playbin), "volume", (m_muted ? 0 : m_volume), NULL);
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

    } else if (GST_MESSAGE_SRC(gm) == GST_OBJECT_CAST(m_playbin)) {
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
                    break;
                case GST_STATE_PAUSED:
                    if ( m_state != QGstreamerPlayerControl::Paused )
                        emit stateChanged(m_state = QGstreamerPlayerControl::Paused);
                    break;
                case GST_STATE_PLAYING:
                    //if (oldState == GST_STATE_PAUSED)
                    //    getStreamsInfo();

                    if (m_state != QGstreamerPlayerControl::Playing)
                        emit stateChanged(m_state = QGstreamerPlayerControl::Playing);
                    break;
                }
            }
            break;

        case GST_MESSAGE_EOS:
            if (m_state != QGstreamerPlayerControl::Stopped && m_state != QGstreamerPlayerControl::Finished) {
                emit stateChanged(m_state = QGstreamerPlayerControl::Finished);
                emit playbackFinished();
            }
            break;

        case GST_MESSAGE_STREAM_STATUS:
        case GST_MESSAGE_UNKNOWN:
        case GST_MESSAGE_ERROR:
        case GST_MESSAGE_WARNING:
        case GST_MESSAGE_INFO:
        case GST_MESSAGE_TAG:
        case GST_MESSAGE_BUFFERING:
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

void QGstreamerPlayerSession::getStreamInfo()
{    // Get Length if have not before.
    if (m_duration == -1)
    {
        GstFormat   format = GST_FORMAT_TIME;
        gint64      duration = 0;

        if (gst_element_query_duration(m_playbin, &format, &duration))
        {
            m_duration = duration / 1000000;
            emit durationChanged(m_duration);
        }
    }

    /*

    // Audio/Video
    if (!d->haveStreamInfo)
    {
        enum {
            GST_STREAM_TYPE_UNKNOWN,
            GST_STREAM_TYPE_AUDIO,
            GST_STREAM_TYPE_VIDEO,
            GST_STREAM_TYPE_TEXT,
            GST_STREAM_TYPE_SUBPICTURE,
            GST_STREAM_TYPE_ELEMENT
        };

        GList*      streamInfo;

        g_object_get(G_OBJECT(d->playbin), "stream-info", &streamInfo, NULL);

        for (; streamInfo != 0; streamInfo = g_list_next(streamInfo))
        {
            gint        type;
            GObject*    obj = G_OBJECT(streamInfo->data);

            g_object_get(obj, "type", &type, NULL);

            switch (type)
            {
            case GST_STREAM_TYPE_AUDIO:
                break;

            case GST_STREAM_TYPE_VIDEO:
                {
                    d->videoControlServer = new QMediaVideoControlServer( d->id,
                                                                          0, //target
                                                                          this );

                    d->videoControlServer->setRenderTarget(d->sinkWidget->windowId());
                    d->interfaces << "Video";

                    QVideoSurface *surface = d->sinkWidget->videoSurface();
                    surface->setRotation(d->videoControlServer->videoRotation());
                    surface->setScaleMode(d->videoControlServer->videoScaleMode());
                    connect( d->videoControlServer, SIGNAL(rotationChanged(QtopiaVideo::VideoRotation)),
                             surface, SLOT(setRotation(QtopiaVideo::VideoRotation)) );
                    connect( d->videoControlServer, SIGNAL(scaleModeChanged(QtopiaVideo::VideoScaleMode)),
                             surface, SLOT(setScaleMode(QtopiaVideo::VideoScaleMode)) );

                    emit interfaceAvailable("Video");
                }
                break;

            case GST_STREAM_TYPE_UNKNOWN:
                break;

            case GST_STREAM_TYPE_TEXT:
                break;

            case GST_STREAM_TYPE_SUBPICTURE:
                break;

            case GST_STREAM_TYPE_ELEMENT:
                break;
            }
        }

        d->haveStreamInfo = true;
    }
    */

}
