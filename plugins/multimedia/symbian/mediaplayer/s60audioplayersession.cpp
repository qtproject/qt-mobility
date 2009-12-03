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
    TRAPD(err, m_player = CMdaAudioPlayerUtility::NewL(*this, 0, EMdaPriorityPreferenceNone));
    //TODO: Error handlind if creating audio player fails
}

S60AudioPlayerSession::~S60AudioPlayerSession()
{
    delete m_player;
}

void S60AudioPlayerSession::load(const QUrl &url)
{
    // we should not load if the player is already loading media
    // this is the case if we have already loaded same media and we set media again
    if (mediaStatus() != QMediaPlayer::LoadingMedia) {
        setMediaStatus(QMediaPlayer::LoadingMedia);
        setFilePath(url.toLocalFile());
        TPtrC str(reinterpret_cast<const TUint16*>(filePath().utf16()));
        TRAPD(err, m_player->OpenFileL(str));
        emit positionChanged(position());
        emit durationChanged(duration());
        if (err) {
            setMediaStatus(QMediaPlayer::NoMedia);
        }
    }
}

qint64 S60AudioPlayerSession::duration() const
{
    return m_player->Duration().Int64() / 1000;
}

qint64 S60AudioPlayerSession::position() const
{
    TTimeIntervalMicroSeconds ms = 0;
    m_player->GetPosition(ms);
    return microSecondsToMilliSeconds(ms.Int64());
}

void S60AudioPlayerSession::setPlaybackRate(qreal rate)
{
    S60MediaPlayerSession::setPlaybackRate(rate);
    // TODO: Set the playback rate for the player
}

bool S60AudioPlayerSession::isVideoAvailable() const
{
    return false;
}

void S60AudioPlayerSession::play()
{
    if (state() != QMediaPlayer::PlayingState && mediaStatus() != QMediaPlayer::LoadingMedia ) {
        startTimer();
        m_player->Play();
        setState(QMediaPlayer::PlayingState);
    }
}

void S60AudioPlayerSession::pause()
{
    m_player->Pause();
    stopTimer();
    setState(QMediaPlayer::PausedState);
}

void S60AudioPlayerSession::stop()
{
    m_player->Stop();
    stopTimer();
    setState(QMediaPlayer::StoppedState);
    emit positionChanged(position());
}

void S60AudioPlayerSession::setVolume(int volume)
{    
    S60MediaPlayerSession::setVolume(volume);
    if (mediaStatus() == QMediaPlayer::LoadedMedia && !isMuted()) {
        m_player->SetVolume((this->volume() / 100)* m_player->MaxVolume());
    }
}

void S60AudioPlayerSession::setPosition(qint64 ms)
{   
    if (state() == QMediaPlayer::PlayingState)
        m_player->Pause();

    m_player->SetPosition(milliSecondsToMicroSeconds(ms));
    emit positionChanged(position());
    
    if (state() == QMediaPlayer::PlayingState)
        m_player->Play();
}

void S60AudioPlayerSession::setMuted(bool muted)
{    
    S60MediaPlayerSession::setMuted(muted);
    if (isMuted() == true) {
        m_player->SetVolume(0); 
    } else {
        m_player->SetVolume(volume());
    }
}

void S60AudioPlayerSession::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
{
    Q_UNUSED(aDuration);
    
    if (aError == KErrNone) {
        setMediaStatus(QMediaPlayer::LoadedMedia);
        updateMetaDataEntries();
        setVolume(volume());
        emit durationChanged(duration());
    } else {
        setMediaStatus(QMediaPlayer::NoMedia);
    }
}

void S60AudioPlayerSession::updateMetaDataEntries()
{
    metaDataEntries().clear();
    int numberOfMetaDataEntries = 0;
    
    m_player->GetNumberOfMetaDataEntries(numberOfMetaDataEntries);
    
    for (int i = 0; i < numberOfMetaDataEntries; i++) {
        CMMFMetaDataEntry *entry = NULL;
        TRAPD(err, entry = m_player->GetMetaDataEntryL(i));
    
        if (err == KErrNone) {
            metaDataEntries().insert(QString::fromUtf16(entry->Name().Ptr(), entry->Name().Length()), QString::fromUtf16(entry->Value().Ptr(), entry->Value().Length()));
        }
        delete entry;
    }
    emit metaDataChanged();
}

void S60AudioPlayerSession::MapcPlayComplete(TInt aError)
{
    Q_UNUSED(aError)
    stopTimer();
    setState(QMediaPlayer::StoppedState);
    setMediaStatus(QMediaPlayer::EndOfMedia);
    m_player->Close();
    emit positionChanged(position());
}

