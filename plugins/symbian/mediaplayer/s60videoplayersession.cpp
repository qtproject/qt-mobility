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

#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>

#include <coemain.h>    // For CCoeEnv
#include <coecntrl.h>
#include <w32std.h>

#include <QWidget>
#include <QDir>

S60VideoPlayerSession::S60VideoPlayerSession(QObject *parent)
    : S60MediaPlayerSession(parent),
      m_wsSession(0),
      m_screenDevice(0),
      m_window(0),
      m_numberOfMetaDataEntries(0)
{    
    m_frameSize = QSize(320,240);
}

S60VideoPlayerSession::~S60VideoPlayerSession()
{
    delete m_player;
}

void S60VideoPlayerSession::load(const QUrl &url)
{
    m_url = url.toLocalFile();
    QString fileName = QDir::toNativeSeparators(m_url.toString());
    TPtrC str(reinterpret_cast<const TUint16*>(fileName.utf16()));
    m_player->OpenFileL(str);
    qDebug() << m_url;
}

qint64 S60VideoPlayerSession::duration() const
{         
    return  m_player->DurationL().Int64() / 1000; 
}

qint64 S60VideoPlayerSession::position() const
{
    return m_player->PositionL().Int64() / 1000;
}

qreal S60VideoPlayerSession::playbackRate() const
{
    return m_playbackRate;
}

void S60VideoPlayerSession::setPlaybackRate(qreal rate)
{
    m_playbackRate = rate;
}

bool S60VideoPlayerSession::isBuffering() const
{
    return false;
}

int S60VideoPlayerSession::bufferingProgress() const
{
    return 0;
}

int S60VideoPlayerSession::volume() const
{
    return m_volume;
}

bool S60VideoPlayerSession::isMuted() const
{
    return m_muted;
}

void S60VideoPlayerSession::setVideoRenderer(QObject *videoOutput)
{
    m_testWidget = qobject_cast<S60VideoWidgetControl*>(videoOutput);

    const TInt priority = 0;
    const TMdaPriorityPreference preference = EMdaPriorityPreferenceNone;
    
    getNativeHandles();   
      
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
    // TODO: m_player->SetDisplayWindowL();
}

void S60VideoPlayerSession::getNativeHandles()
{
    CCoeControl* const control =  m_testWidget->videoWidget()->winId();
    control->MakeVisible(ETrue);
    qDebug() << "h: " << m_testWidget->videoWidget()->winId()->Size().iHeight <<  "v: " << m_testWidget->videoWidget()->winId()->Size().iWidth;
    qDebug() << "h: " << m_testWidget->videoWidget()->winId()->Size().iHeight <<  "v: " << m_testWidget->videoWidget()->winId()->Size().iWidth;
    //control->SetSize(TSize(240,245));
    //control->SetPosition(TPoint(0,-50));
    qDebug() << "x: " <<control->Position().iX  << "y: " << control->Position().iY;
    control->ActivateL();

    CCoeEnv* const coeEnv = control->ControlEnv();
    m_wsSession = &(coeEnv->WsSession());
    m_screenDevice = coeEnv->ScreenDevice();
    m_window = control->DrawableWindow();

    m_windowRect = TRect(
        control->DrawableWindow()->AbsPosition(),
        control->DrawableWindow()->Size());
    m_clipRect = m_windowRect;
    
    qDebug() << "h: " << m_clipRect.Height() << "w: " << m_clipRect.Width();     
}

bool S60VideoPlayerSession::isVideoAvailable() const
{
    return m_videoAvailable;
}

bool S60VideoPlayerSession::isSeekable() const
{
    return m_seekable;
}

void S60VideoPlayerSession::play()
{
    startTimer();
    m_player->Play();
    m_state = QMediaPlayer::PlayingState;
    emit stateChanged(QMediaPlayer::PlayingState); 
}

void S60VideoPlayerSession::pause()
{
    m_player->PauseL();
    m_state = QMediaPlayer::PausedState;
    emit stateChanged(QMediaPlayer::PausedState);
}

void S60VideoPlayerSession::stop()
{
    m_player->Stop();
    m_state = QMediaPlayer::StoppedState;
    emit stateChanged(QMediaPlayer::StoppedState);
}

void S60VideoPlayerSession::seek(qint64 ms)
{
    m_player->PauseL();
    m_player->SetPositionL(ms);
    emit positionChanged(position());
}

void S60VideoPlayerSession::setVolume(int volume)
{
    m_player->SetVolumeL(volume);
}

void S60VideoPlayerSession::setMuted(bool muted)
{
    if (muted)
        m_player->SetVolumeL(0);
}

void S60VideoPlayerSession::setMediaStatus(QMediaPlayer::MediaStatus status)
{
    if (m_mediaStatus != status) {
        m_mediaStatus = status;
        emit mediaStatusChanged(status);
    }
}

void S60VideoPlayerSession::MvpuoOpenComplete(TInt aError)
{
    m_player->Prepare();
}

void S60VideoPlayerSession::MvpuoPrepareComplete(TInt aError)
{
    m_numberOfMetaDataEntries = 0;
    TRAP_IGNORE(m_numberOfMetaDataEntries = m_player->NumberOfMetaDataEntriesL();)
    
    emit metaDataChanged();
}

void S60VideoPlayerSession::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
{
    Q_UNUSED(aFrame);
    Q_UNUSED(aError);
}

void S60VideoPlayerSession::MvpuoPlayComplete(TInt aError)
{
    m_state = QMediaPlayer::StoppedState;
    emit stateChanged(QMediaPlayer::StoppedState);
    emit positionChanged(position());
}

void S60VideoPlayerSession::MvpuoEvent(const TMMFEvent &aEvent)
{
    Q_UNUSED(aEvent);
}

bool S60VideoPlayerSession::isMetadataAvailable()
{
    return (m_numberOfMetaDataEntries > 0);
}

QVariant S60VideoPlayerSession::metaData(QtMedia::MetaData key)
{
    return "Test metadata";
}
