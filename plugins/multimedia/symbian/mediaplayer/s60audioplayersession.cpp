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
#include <QTimer>

#include <MdaAudioSamplePlayer.h>

S60AudioPlayerSession::S60AudioPlayerSession(QObject *parent)
    : S60MediaPlayerSession(parent)
{    
    TRAPD(err, m_player = CMdaAudioPlayerUtility::NewL(*this));
    //TODO: Error handlind if creating audio player fails
}

S60AudioPlayerSession::~S60AudioPlayerSession()
{
    delete m_timer;
    delete m_player;
}

void S60AudioPlayerSession::load(const QUrl &url)
{
    m_audioAvailable = false;

    // we should not load if the player is already loading media
    // this is the case if we have already loaded same media and we set media again
    if (m_mediaStatus != QMediaPlayer::LoadingMedia) {
        m_mediaStatus = QMediaPlayer::LoadingMedia;
        m_url = url.toLocalFile();
        QString fileName = QDir::toNativeSeparators(m_url.toString());
        TPtrC str(reinterpret_cast<const TUint16*>(fileName.utf16()));
        TRAPD(err, m_player->OpenFileL(str));
        emit positionChanged(position());
        emit durationChanged(duration());
        if (err) {
            m_mediaStatus = QMediaPlayer::NoMedia;
        }
        emit mediaStatusChanged(m_mediaStatus);

    }
}

qint64 S60AudioPlayerSession::duration() const
{
    return m_player->Duration().Int64() / 1000;
}

qint64 S60AudioPlayerSession::position() const
{
    TTimeIntervalMicroSeconds ms;
    m_player->GetPosition(ms);
    qDebug() << ms.Int64() / 1000 ;
    return ms.Int64() / 1000;
}
void S60AudioPlayerSession::setPlaybackRate(qreal rate)
{
    //TODO: set playbackrate 
    m_playbackRate = rate;
}

bool S60AudioPlayerSession::isVideoAvailable() const
{
    return false;
}

void S60AudioPlayerSession::play()
{
    if (m_state != QMediaPlayer::PlayingState && m_mediaStatus != QMediaPlayer::LoadingMedia ) {
        startTimer();
        m_player->Play();
        m_state = QMediaPlayer::PlayingState;
        emit stateChanged(m_state);
    }
}

void S60AudioPlayerSession::pause()
{
    m_player->Pause();
    m_state = QMediaPlayer::PausedState;
    stopTimer();
    emit stateChanged(m_state);
}

void S60AudioPlayerSession::stop()
{
    m_player->Stop();
    stopTimer();
    m_state = QMediaPlayer::StoppedState;
    emit stateChanged(m_state);
}

void S60AudioPlayerSession::setVolume(int volume)
{
    if (m_volume != volume)
        emit volumeChanged(m_volume);
    m_volume = volume;
    m_player->SetVolume(m_volume);

}
void S60AudioPlayerSession::setPosition(qint64 ms)
{   
    m_player->Pause();
    m_player->SetPosition(ms*1000);
    emit positionChanged(position());
    m_player->Play();
}

void S60AudioPlayerSession::setMuted(bool muted)
{
    if (muted == true) {
        m_player->SetVolume(0);
        m_muted = true;
    } else {
        m_player->SetVolume(m_volume);
        m_muted = false;
    }
    
    emit mutedStateChaned(muted);
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
    int numberOfMetaDataEntries;
    Q_UNUSED(aDuration);
    if (!aError) {
        m_mediaStatus = QMediaPlayer::LoadedMedia;
        m_audioAvailable = true;

        m_player->GetNumberOfMetaDataEntries(numberOfMetaDataEntries);
        // we need to check volume here
        if (m_volume == -1) {
            // get default volume from s60 player
            m_player->GetVolume(m_volume);
            emit volumeChanged(m_volume);
        }
        else {
            setVolume(m_volume);
        }
        
    }
    else
        m_mediaStatus = QMediaPlayer::NoMedia;

    for (int i=0; i < numberOfMetaDataEntries; i++) {
        CMMFMetaDataEntry *entry = NULL;
        TRAPD(err, entry = m_player->GetMetaDataEntryL(i));

        if (err == KErrNone) {
            m_metaDataMap.insert(QString::fromUtf16(entry->Name().Ptr(), entry->Name().Length()), QString::fromUtf16(entry->Value().Ptr(), entry->Value().Length()));
        }
        delete entry;
    }
    emit mediaStatusChanged(m_mediaStatus);
    emit audioAvailableChanged(m_audioAvailable);
    emit durationChanged(duration());
    emit metaDataChanged();
}

void S60AudioPlayerSession::MapcPlayComplete(TInt aError)
{
    Q_UNUSED(aError) // TODO: Error handling...
    stopTimer();
    m_state = QMediaPlayer::StoppedState;
    m_mediaStatus = QMediaPlayer::EndOfMedia;
    emit stateChanged(m_state);
    emit positionChanged(position());
    emit mediaStatusChanged(m_mediaStatus);
}

