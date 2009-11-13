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

#include "s60audioplayersession.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>

#include <QWidget>
#include <QDir>
#include <QVariant>

S60AudioPlayerSession::S60AudioPlayerSession(QObject *parent)
    : S60MediaPlayerSession(parent),
      m_numberOfMetaDataEntries(0)
{    
    //TODO: Error checking somehow...
    TRAP_IGNORE(m_player = CMdaAudioPlayerUtility::NewL(*this);)
}

S60AudioPlayerSession::~S60AudioPlayerSession()
{
    delete m_player;
}

qint64 S60AudioPlayerSession::duration() const
{
    // TODO:
    return -1;
}

qint64 S60AudioPlayerSession::position() const
{
    //TODO:
    return 0;
}

qreal S60AudioPlayerSession::playbackRate() const
{
    return m_playbackRate;
}

void S60AudioPlayerSession::setPlaybackRate(qreal rate)
{
    //TODO: set playbackrate 
    m_playbackRate = rate;
}

bool S60AudioPlayerSession::isBuffering() const
{
    return false;
}

int S60AudioPlayerSession::bufferingProgress() const
{
    return 0;
}

int S60AudioPlayerSession::volume() const
{
    //TODO:
    return -1;
}

bool S60AudioPlayerSession::isMuted() const
{
    //TODO:
    return true;
}

bool S60AudioPlayerSession::isVideoAvailable() const
{
    return false;
}

bool S60AudioPlayerSession::isSeekable() const
{
    return false;
}

void S60AudioPlayerSession::play()
{
    if (m_state == QMediaPlayer::PausedState) {
        m_player->Play();
        m_state = QMediaPlayer::PlayingState;
        emit stateChanged(m_state);
    } else {
        QString fileName = QDir::toNativeSeparators(m_url.toString());
        TPtrC str(reinterpret_cast<const TUint16*>(fileName.utf16()));
        TRAP_IGNORE(m_player->OpenFileL(str);)  // TODO: Error handling...
    }
}

void S60AudioPlayerSession::pause()
{
    m_player->Pause();
    m_state = QMediaPlayer::PausedState;
    emit stateChanged(QMediaPlayer::PausedState);
}

void S60AudioPlayerSession::stop()
{
    // TODO: Stop()
    m_player->Stop();
    m_state = QMediaPlayer::StoppedState;
    emit stateChanged(QMediaPlayer::StoppedState);
}

void S60AudioPlayerSession::seek(qint64 ms)
{
    // TODO:
}

void S60AudioPlayerSession::setVolume(int volume)
{
    // TODO:
}

void S60AudioPlayerSession::setMuted(bool muted)
{
    // TODO:
}

void S60AudioPlayerSession::setMediaStatus(QMediaPlayer::MediaStatus status)
{
    /*if (m_mediaStatus != status) {
        m_mediaStatus = status;
        emit mediaStatusChanged(status);
    }*/
}

void S60AudioPlayerSession::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
{
    if (!aError) {
        m_player->Play();   
        emit stateChanged(QMediaPlayer::PlayingState);
    }
}

void S60AudioPlayerSession::MapcPlayComplete(TInt aError)
{
    m_state = QMediaPlayer::StoppedState;
    emit stateChanged(m_state);
}

bool S60AudioPlayerSession::isMetadataAvailable()
{
    return (m_numberOfMetaDataEntries > 0);
}

QVariant S60AudioPlayerSession::metaData(QtMedia::MetaData key)
{
    return "Test metadata";
}
