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
#include "s60videowidget.h"
#include "s60directscreenaccess.h"

#include <QtCore/qdebug.h>
#include <QtGui/qwidget.h>

#include <coemain.h>    // For CCoeEnv
#include <w32std.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>

#include <QFile>

S60VideoPlayerSession::S60VideoPlayerSession(QObject *parent)
    : S60MediaPlayerSession(parent)
    , m_wsSession(0)
    , m_screenDevice(0)
    , m_window(0)
    , m_videoWidgetControl(0)
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
    
    m_dsa = S60DirectScreenAccess::NewL(*m_wsSession, *m_screenDevice, *m_window);
    m_dsa->Start();
}

S60VideoPlayerSession::~S60VideoPlayerSession()
{
    delete m_player;
    delete m_dummyWidget;
}

void S60VideoPlayerSession::doLoad(const TDesC &path)
{
    TRAPD(err, m_player->OpenFileL(path);)
    if (err) {
        setMediaStatus(QMediaPlayer::NoMedia);
    }
}

void S60VideoPlayerSession::doLoadUrl(const TDesC &path)
{
	TRAPD(err, m_player->OpenUrlL(path);)
	if (err) {
		setMediaStatus(QMediaPlayer::NoMedia);
	}
}

qint64 S60VideoPlayerSession::duration() const
{         
    Int64 duration = 0;
    TRAP_IGNORE(duration = m_player->DurationL().Int64() / 1000;)
    return duration;   
}

qint64 S60VideoPlayerSession::position() const
{
    Int64 position = 0;
    TRAP_IGNORE(position = m_player->PositionL().Int64() / 1000;)
    return position; 
}

int S60VideoPlayerSession::mediaLoadingProgress() const
{
	int progress = 0;
	TRAP_IGNORE(m_player->GetVideoLoadingProgressL(progress);)
	return progress;
}

void S60VideoPlayerSession::doSetPlaybackRate(qreal rate)
{
    // TODO:
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
		m_coeControl =  m_dummyWidget->winId();
    } else {
        m_coeControl = m_videoWidgetControl->videoWidget()->winId();
        m_coeControl->MakeVisible(ETrue);
        m_coeControl->SetPosition(TPoint(0,0));
    }
    
    CCoeEnv* const coeEnv = m_coeControl->ControlEnv();
    m_wsSession = &(coeEnv->WsSession());
    m_screenDevice = coeEnv->ScreenDevice();
    m_window = m_coeControl->DrawableWindow();

    m_windowRect = TRect(
        m_coeControl->DrawableWindow()->AbsPosition(),
        m_coeControl->DrawableWindow()->Size());
    m_clipRect = m_windowRect;
}

bool S60VideoPlayerSession::isVideoAvailable() const
{
    return true;
}

void S60VideoPlayerSession::doPlay()
{   
    m_player->Play();
}

void S60VideoPlayerSession::doPause()
{
    TRAP_IGNORE(m_player->PauseL();)
}

void S60VideoPlayerSession::doStop()
{
    m_player->Stop();
    m_player->Close();
    m_coeControl->DrawDeferred();
}

void S60VideoPlayerSession::doSetPosition(qint64 microSeconds)
{
    TRAPD(err,m_player->SetPositionL(TTimeIntervalMicroSeconds(microSeconds));)
}

void S60VideoPlayerSession::doSetVolume(int volume)
{
    TRAP_IGNORE(m_player->SetVolumeL((volume / 100.0)* m_player->MaxVolume()));
}

void S60VideoPlayerSession::MvpuoOpenComplete(TInt aError)
{
    Q_UNUSED(aError)
    m_player->Prepare();
}

void S60VideoPlayerSession::MvpuoPrepareComplete(TInt aError)
{    
	TRAPD(err,
	m_player->SetDisplayWindowL(*m_wsSession, 
								  *m_screenDevice, 
								  *m_window, 
								  m_windowRect, 
								  m_clipRect);
	);
	
    setError(aError);
    initComplete();
}

void S60VideoPlayerSession::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
{
    Q_UNUSED(aFrame);
    Q_UNUSED(aError);
}

void S60VideoPlayerSession::MvpuoPlayComplete(TInt aError)
{
    m_coeControl->DrawDeferred();
    m_player->Close();
    setError(aError);
    playComplete();
}

void S60VideoPlayerSession::MvpuoEvent(const TMMFEvent &aEvent)
{
    Q_UNUSED(aEvent);
}

void S60VideoPlayerSession::updateMetaDataEntries()
{
    metaDataEntries().clear();
    int numberOfMetaDataEntries = 0;
    
    TRAP_IGNORE(numberOfMetaDataEntries = m_player->NumberOfMetaDataEntriesL();)
    
    for (int i = 0; i < numberOfMetaDataEntries; i++) {
        CMMFMetaDataEntry *entry = NULL;
        TRAPD(err, entry = m_player->MetaDataEntryL(i));
        
        if (err == KErrNone) {
            metaDataEntries().insert(QString::fromUtf16(entry->Name().Ptr(), entry->Name().Length()), QString::fromUtf16(entry->Value().Ptr(), entry->Value().Length()));
        }
        delete entry;
    }
    
    emit metaDataChanged();
}
