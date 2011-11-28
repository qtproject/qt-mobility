
/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60mediaplayercontrol.h"
#include "s60mediaplayersession.h"
#include "s60mmtrace.h"

#include <QtCore/qdir.h>
#include <QtCore/qurl.h>
#include <QtCore/qdebug.h>

/*!
    Constructs a new media player control with the given \a parent.
*/

S60MediaPlayerControl::S60MediaPlayerControl(S60MediaPlayerService *service)
    : QMediaPlayerControl(service),
      m_service(service),
      m_session(NULL),
      m_stream(NULL)
{
    TRACE("S60MediaPlayerControl::S60MediaPlayerControl" << qtThisPtr());
}

/*!
    Destroys a media player control.
*/

S60MediaPlayerControl::~S60MediaPlayerControl()
{
    TRACE("S60MediaPlayerControl::~S60MediaPlayerControl" << qtThisPtr());
}

/*!
    \return the current playback position in milliseconds.
*/

qint64 S60MediaPlayerControl::position() const
{
    if (m_session)
        return m_session->position();
    return 0;
}

/*!
   \return the duration of the current media in milliseconds.
*/

qint64 S60MediaPlayerControl::duration() const
{
    if (m_session)
        return m_session->duration();
    return -1;
}

/*!
    \return the state of a player control.
*/

QMediaPlayer::State S60MediaPlayerControl::state() const
{
    if (m_session)
        return m_session->state();
    return QMediaPlayer::StoppedState;
}

/*!
    \return the status of the current media.
*/

QMediaPlayer::MediaStatus S60MediaPlayerControl::mediaStatus() const
{
    if (m_session)
        return m_session->mediaStatus();
    return m_mediaSettings.mediaStatus();
}

/*!
    \return the buffering progress of the current media.  Progress is measured in the percentage
    of the buffer filled.
*/

int S60MediaPlayerControl::bufferStatus() const
{
    if (m_session)
        return m_session->bufferStatus();
    return 0;
}

/*!
    \return the audio volume of a player control.
*/

int S60MediaPlayerControl::volume() const
{
    if (m_session)
        return m_session->volume();
    return m_mediaSettings.volume();
}

/*!
    \return the mute state of a player control.
*/

bool S60MediaPlayerControl::isMuted() const
{
   if (m_session)
       return  m_session->isMuted();
   return m_mediaSettings.isMuted();
}

/*!
    Identifies if the current media is seekable.

    \return true if it possible to seek within the current media, and false otherwise.
*/

bool S60MediaPlayerControl::isSeekable() const
{
    if (m_session)
       return  m_session->isSeekable();
    return false;
}

/*!
    \return a range of times in milliseconds that can be played back.

    Usually for local files this is a continuous interval equal to [0..duration()]
    or an empty time range if seeking is not supported, but for network sources
    it refers to the buffered parts of the media.
*/

QMediaTimeRange S60MediaPlayerControl::availablePlaybackRanges() const
{
    QMediaTimeRange ranges;

    if(m_session && m_session->isSeekable())
        ranges.addInterval(0, m_session->duration());

    return ranges;
}

/*!
    \return the rate of playback.
*/

qreal S60MediaPlayerControl::playbackRate() const
{
    return m_mediaSettings.playbackRate();
}

/*!
    Sets the \a rate of playback.
*/

void S60MediaPlayerControl::setPlaybackRate(qreal rate)
{
    TRACE("S60MediaPlayerControl::setPlaybackRate" << qtThisPtr() << "rate" << rate);

    //getting the current playbackrate
    qreal currentPBrate = m_mediaSettings.playbackRate();
    //checking if we need to change the Playback rate
    if (!qFuzzyCompare(currentPBrate,rate))
    {
        if(m_session)
            m_session->setPlaybackRate(rate);

        m_mediaSettings.setPlaybackRate(rate);
    }
}

/*!
    Sets the playback \a pos of the current media.  This will initiate a seek and it may take
    some time for playback to reach the position set.
*/

void S60MediaPlayerControl::setPosition(qint64 pos)
{
    TRACE("S60MediaPlayerControl::setPosition" << qtThisPtr() << "pos" << pos);

    if (m_session)
        m_session->setPosition(pos);
}

/*!
    Starts playback of the current media.

    If successful the player control will immediately enter the \l {QMediaPlayer::PlayingState}
    {playing} state.
*/

void S60MediaPlayerControl::play()
{
    TRACE("S60MediaPlayerControl::play" << qtThisPtr());

    if (m_session)
        m_session->play();
}

/*!
    Pauses playback of the current media.

    If sucessful the player control will immediately enter the \l {QMediaPlayer::PausedState}
    {paused} state.
*/

void S60MediaPlayerControl::pause()
{
    TRACE("S60MediaPlayerControl::pause" << qtThisPtr());

    if (m_session)
        m_session->pause();
}

/*!
    Stops playback of the current media.

    If successful the player control will immediately enter the \l {QMediaPlayer::StoppedState}
    {stopped} state.
*/

void S60MediaPlayerControl::stop()
{
    TRACE("S60MediaPlayerControl::stop" << qtThisPtr());

    if (m_session)
        m_session->stop();
}

/*!
    Sets the audio \a volume of a player control.
*/

void S60MediaPlayerControl::setVolume(int volume)
{
    int boundVolume = qBound(0, volume, 100);
    if (boundVolume == m_mediaSettings.volume())
        return;

    TRACE("S60MediaPlayerControl::setVolume" << qtThisPtr()
          << "volume" << volume << "boundVolume" << boundVolume);

    m_mediaSettings.setVolume(boundVolume);

    if (m_session)
        m_session->setVolume(boundVolume);
}

/*!
    Sets the \a muted state of a player control.
*/

void S60MediaPlayerControl::setMuted(bool muted)
{
    if (m_mediaSettings.isMuted() == muted)
        return;

    TRACE("S60MediaPlayerControl::setMuted" << qtThisPtr() << "muted" << muted);

    m_mediaSettings.setMuted(muted);

    if (m_session)
        m_session->setMuted(muted);
}

/*!
 * \return the current media source.
*/

QMediaContent S60MediaPlayerControl::media() const
{
    return m_media;
}

/*!
    \return the current media stream. This is only a valid if a stream was passed to setMedia().

    \sa setMedia()
*/

const QIODevice *S60MediaPlayerControl::mediaStream() const
{
    return m_stream;
}

/*!
     Sets the current \a source media source.  If a \a stream is supplied; data will be read from that
    instead of attempting to resolve the media source.  The media source may still be used to
    supply media information such as mime type.

    Setting the media to a null QMediaContent will cause the control to discard all
    information relating to the current media source and to cease all I/O operations related
    to that media.
*/

void S60MediaPlayerControl::setMedia(const QMediaContent &source, QIODevice *stream)
{
    TRACE("S60MediaPlayerControl::setMedia" << qtThisPtr()
          << "source" << source.canonicalUrl().toString());
    Q_UNUSED(stream)
    if (m_session && (m_media == source)) {
        if (m_session->isStreaming())
            m_session->load(source);
    } else {
        delete m_session;
        m_session = 0;
        m_session = m_service->createPlayerSession(source);
        QMediaPlayer::MediaStatus mediaStatus = QMediaPlayer::NoMedia;
        if (m_session && !source.canonicalUrl().isEmpty()) {
            m_session->load(source);
            mediaStatus = m_session->mediaStatus();
        }
        m_mediaSettings.setMediaStatus(mediaStatus);
    }
    emit stateChanged(QMediaPlayer::StoppedState);
    if (m_media != source) {
        m_media = source;
        emit mediaChanged(m_media);
    }
}

/*!
 * \return media player session.
*/
S60MediaPlayerSession* S60MediaPlayerControl::session()
{
    return m_session;
}

/*!
 *   Sets \a output as a VideoOutput.
*/

void S60MediaPlayerControl::setVideoOutput(QObject *output)
{
    TRACE("S60MediaPlayerControl::setVideoOutput" << qtThisPtr()
          << "output" << output);
    m_mediaSettings.setVideoOutput(output);
    if (m_session)
        m_session->setVideoRenderer(output);
}

/*!
 * \return TRUE if Audio available or else FALSE.
*/

bool S60MediaPlayerControl::isAudioAvailable() const
{
    if (m_session)
        return m_session->isAudioAvailable(); 
    return false;
}

/*!
 * \return TRUE if Video available or else FALSE.
*/

bool S60MediaPlayerControl::isVideoAvailable() const
{
    if (m_session)
        return m_session->isVideoAvailable();
    return false;
}

/*!
 * \return media settings.
 *
 * Media Settings include volume, muted, playbackRate, mediaStatus, audioEndpoint.
*/
const S60MediaSettings& S60MediaPlayerControl::mediaControlSettings() const
{
    return m_mediaSettings;
}

/*!
 * Set the audio endpoint to \a name.
*/

void S60MediaPlayerControl::setAudioEndpoint(const QString& name)
{
    TRACE("S60MediaPlayerControl::setAudioEndpoint" << qtThisPtr()
          << "name" << name);

    m_mediaSettings.setAudioEndpoint(name);
}

/*!
 *  Sets media type \a type as Unknown, Video, Audio, Data.
*/

void S60MediaPlayerControl::setMediaType(S60MediaSettings::TMediaType type)
{
    TRACE("S60MediaPlayerControl::setMediaType" << qtThisPtr()
          << "type" << type);

    m_mediaSettings.setMediaType(type);
}
