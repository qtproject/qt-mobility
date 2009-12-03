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

#include "s60videoplayersession.h"

#include <QtCore/qdebug.h>

#include <coemain.h>    // For CCoeEnv
#include <coecntrl.h>
#include <w32std.h>

#include <QWidget>
#include <QDir>

#include <mmf/common/mmfcontrollerframeworkbase.h>

S60VideoPlayerSession::S60VideoPlayerSession(QObject *parent)
    : S60MediaPlayerSession(parent),
      m_wsSession(0),
      m_screenDevice(0),
      m_window(0),
      m_videoWidgetControl(0)
{    
    m_dummyWidget = new QWidget();
    
    const TInt priority = 0;
    const TMdaPriorityPreference preference = EMdaPriorityPreferenceNone;
    
    nativeHandles();
    TRAPD(err, 
        m_player = CVideoPlayerUtility::NewL(*this, 
                                  priority, 
                                  preference, 
                                  *m_wsSession, 
                                  *m_screenDevice, 
                                  *m_window, 
                                  m_windowRect, 
                                  m_clipRect)
         );
}

S60VideoPlayerSession::~S60VideoPlayerSession()
{
    delete m_player;
    delete m_dummyWidget;
}
void S60VideoPlayerSession::setVolume(int volume)
{
    S60MediaPlayerSession::setVolume(volume);
    if (currentMediaStatus() == QMediaPlayer::LoadedMedia && !isMuted()) {
        TRAP_IGNORE(m_player->SetVolumeL((this->volume()/100)* m_player->MaxVolume()));
    }
}

void S60VideoPlayerSession::load(const QUrl &url)
{
    setMediaStatus(QMediaPlayer::LoadingMedia);
    m_url = url.toLocalFile();
    QString fileName = QDir::toNativeSeparators(m_url.toString());
    TPtrC str(reinterpret_cast<const TUint16*>(fileName.utf16()));
    TRAP_IGNORE(m_player->OpenFileL(str);) // TODO: Error handling...
    emit positionChanged(position());
    emit durationChanged(duration());
}

qint64 S60VideoPlayerSession::duration() const
{         
    Int64 duration;
    TRAP_IGNORE(duration = m_player->DurationL().Int64() / 1000;)
    return duration;   
}

qint64 S60VideoPlayerSession::position() const
{
    Int64 position;
    TRAP_IGNORE(position = m_player->PositionL().Int64() / 1000;)
    return position; 
}

void S60VideoPlayerSession::setPlaybackRate(qreal rate)
{
    S60MediaPlayerSession::setPlaybackRate(rate);
    // TODO: Set the playback rate for the player
}

void S60VideoPlayerSession::setVideoRenderer(QObject *videoOutput)
{
    m_videoWidgetControl = qobject_cast<S60VideoWidgetControl*>(videoOutput);
    
    nativeHandles();   
      
    m_player->SetDisplayWindowL(*m_wsSession, 
         *m_screenDevice, 
         *m_window, 
         m_windowRect, 
         m_clipRect);
}

void S60VideoPlayerSession::nativeHandles()
{
    if (!m_videoWidgetControl) {
        control =  m_dummyWidget->winId(); 
    } else {
        control =  m_videoWidgetControl->videoWidget()->winId();
        control->MakeVisible(ETrue);
        control->SetPosition(TPoint(0,0));
    }
    
    CCoeEnv* const coeEnv = control->ControlEnv();
    m_wsSession = &(coeEnv->WsSession());
    m_screenDevice = coeEnv->ScreenDevice();
    m_window = control->DrawableWindow();

    m_windowRect = TRect(
        control->DrawableWindow()->AbsPosition(),
        control->DrawableWindow()->Size());
    m_clipRect = m_windowRect;
}

bool S60VideoPlayerSession::isVideoAvailable() const
{
    return m_videoAvailable;
}

void S60VideoPlayerSession::play()
{   
    TRAP_IGNORE(
    m_player->SetDisplayWindowL(*m_wsSession, 
                                  *m_screenDevice, 
                                  *m_window, 
                                  m_windowRect, 
                                  m_clipRect);  
    control->ActivateL();
    );
    control->DrawNow();
    if (currentState() != QMediaPlayer::PlayingState && currentMediaStatus() != QMediaPlayer::LoadingMedia ) {
        startTimer();
        m_player->Play();
        setState(QMediaPlayer::PlayingState);
    }
}

void S60VideoPlayerSession::pause()
{
    TRAP_IGNORE(m_player->PauseL();)
    setState(QMediaPlayer::PausedState);
}

void S60VideoPlayerSession::stop()
{
    m_player->Stop();
    stopTimer();
    control->DrawDeferred();
    setState(QMediaPlayer::StoppedState);
    emit positionChanged(position());
}

void S60VideoPlayerSession::setPosition(qint64 ms)
{
    TRAP_IGNORE(
        m_player->SetPositionL(milliSecondsToMicroSeconds(ms));
        emit positionChanged(position());
    )  
}

void S60VideoPlayerSession::setMuted(bool muted)
{
    S60MediaPlayerSession::setMuted(muted);
    if (isMuted() == true) {
        TRAP_IGNORE(m_player->SetVolumeL(0);)
    } else {
        TRAP_IGNORE(m_player->SetVolumeL(volume());)
    }
}

void S60VideoPlayerSession::MvpuoOpenComplete(TInt aError)
{
    if (aError == KErrNone) {
        m_player->Prepare();
        setMediaStatus(QMediaPlayer::LoadedMedia);
    } else {
        setMediaStatus(QMediaPlayer::NoMedia);
    }
}

void S60VideoPlayerSession::MvpuoPrepareComplete(TInt aError)
{    
    if (aError == KErrNone) {
        updateMetaDataEntries();
        setVolume(volume());
        emit durationChanged(duration());
    } else {
        setMediaStatus(QMediaPlayer::NoMedia);
    }
}

void S60VideoPlayerSession::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
{
    Q_UNUSED(aFrame);
    Q_UNUSED(aError);
}

void S60VideoPlayerSession::MvpuoPlayComplete(TInt aError)
{
    Q_UNUSED(aError)
    control->DrawDeferred();
    stopTimer();
    
    emit positionChanged(position());
    
    setState(QMediaPlayer::StoppedState);
    setMediaStatus(QMediaPlayer::EndOfMedia);
}

void S60VideoPlayerSession::MvpuoEvent(const TMMFEvent &aEvent)
{
    Q_UNUSED(aEvent);
}

void S60VideoPlayerSession::updateMetaDataEntries()
{
    m_metaDataMap.clear();
    int numberOfMetaDataEntries = 0;
    
    TRAP_IGNORE(numberOfMetaDataEntries = m_player->NumberOfMetaDataEntriesL();)
    
    for (int i = 0; i < numberOfMetaDataEntries; i++) {
        CMMFMetaDataEntry *entry = NULL;
        TRAPD(err, entry = m_player->MetaDataEntryL(i));
        
        if (err == KErrNone) {
           m_metaDataMap.insert(QString::fromUtf16(entry->Name().Ptr(), entry->Name().Length()), QString::fromUtf16(entry->Value().Ptr(), entry->Value().Length()));
        }
        delete entry;
    }
    
    emit metaDataChanged();
}
