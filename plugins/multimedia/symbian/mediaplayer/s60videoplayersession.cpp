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
#include "s60mediaplayerservice.h"
#include "s60videooverlay.h"

#include <QtCore/qdebug.h>
#include <QtCore/private/qcore_symbian_p.h>
#include <QtGui/qwidget.h>
#include <QApplication>

#include <coecntrl.h>
#include <coemain.h>    // For CCoeEnv
#include <w32std.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>

S60VideoPlayerSession::S60VideoPlayerSession(QMediaService *service)
    : S60MediaPlayerSession(service)
    , m_player(0)
    , m_clipRect(0, 0, 0, 0)
    , m_windowRect(0, 0, 0, 0)
    , m_output(QVideoOutputControl::NoOutput)
    , m_windowId(0)
    , m_wsSession(0)
    , m_screenDevice(0)
    , m_window(0)
    , m_service(*service)
{  
    resetNativeHandles();
    QT_TRAP_THROWING(m_player = CVideoPlayerUtility::NewL(
        *this, 
        0, 
        EMdaPriorityPreferenceNone, 
        *m_wsSession, 
        *m_screenDevice, 
        *m_window, 
        m_windowRect, 
        m_clipRect));
}

S60VideoPlayerSession::~S60VideoPlayerSession()
{
    m_player->Close();
    delete m_player;
}

void S60VideoPlayerSession::doLoadL(const TDesC &path)
{
    m_player->OpenFileL(path);
}

void S60VideoPlayerSession::doLoadUrlL(const TDesC &path)
{
    m_player->OpenUrlL(path);
}

int S60VideoPlayerSession::doGetMediaLoadingProgressL() const
{
	int progress = 0;
    m_player->GetVideoLoadingProgressL(progress);
    return progress;
}

int S60VideoPlayerSession::doGetDurationL() const
{
    return m_player->DurationL().Int64() / 1000;   
}

void S60VideoPlayerSession::setVideoRenderer(QObject *videoOutput)
{
    Q_UNUSED(videoOutput)
    QVideoOutputControl *videoControl = qobject_cast<QVideoOutputControl *>(m_service.control(QVideoOutputControl_iid));
    
    //Render changes
    if (m_output != videoControl->output()) {
        
        if (m_output == QVideoOutputControl::WidgetOutput) {
            S60VideoWidgetControl *widgetControl = qobject_cast<S60VideoWidgetControl *>(m_service.control(QVideoWidgetControl_iid));
            disconnect(widgetControl, SIGNAL(widgetUpdated()), this, SLOT(resetVideoDisplay()));
            disconnect(this, SIGNAL(stateChanged(QMediaPlayer::State)), widgetControl, SLOT(videoStateChanged(QMediaPlayer::State)));
        }

        if (videoControl->output() == QVideoOutputControl::WidgetOutput) {
            S60VideoWidgetControl *widgetControl = qobject_cast<S60VideoWidgetControl *>(m_service.control(QVideoWidgetControl_iid));
            connect(widgetControl, SIGNAL(widgetUpdated()), this, SLOT(resetVideoDisplay()));
            connect(this, SIGNAL(stateChanged(QMediaPlayer::State)), widgetControl, SLOT(videoStateChanged(QMediaPlayer::State)));
        }
        
        m_output = videoControl->output();
        resetVideoDisplay();
    }
}

bool S60VideoPlayerSession::resetNativeHandles()
{
    QVideoOutputControl* videoControl = qobject_cast<QVideoOutputControl *>(m_service.control(QVideoOutputControl_iid));
    WId newId = 0;
    TRect newClipRect = TRect(0,0,0,0);
    
    if (videoControl->output() == QVideoOutputControl::WidgetOutput) {
        S60VideoWidgetControl* widgetControl = qobject_cast<S60VideoWidgetControl *>(m_service.control(QVideoWidgetControl_iid));
        QWidget *videoWidget = widgetControl->videoWidget();
        newId = videoWidget->winId();
        newClipRect = qt_QRect2TRect(QRect(videoWidget->mapToGlobal(videoWidget->pos()), videoWidget->size()));
    } else if (videoControl->output() == QVideoOutputControl::WindowOutput) {
        S60VideoOverlay* windowControl = qobject_cast<S60VideoOverlay *>(m_service.control(QVideoWindowControl_iid));
        newId = windowControl->winId();
        newClipRect = TRect( newId->DrawableWindow()->AbsPosition(), newId->DrawableWindow()->Size());
    } else {
        if (QApplication::activeWindow())
            newId = QApplication::activeWindow()->effectiveWinId();
    }
    
    if (newClipRect == m_clipRect &&  newId == m_windowId) 
        return false;
    
    if (newId) {
        m_windowRect = TRect( newId->DrawableWindow()->AbsPosition(), newId->DrawableWindow()->Size());
        m_clipRect = newClipRect;
        m_windowId = newId;
        CCoeEnv *coeEnv = m_windowId->ControlEnv();
        m_wsSession = &(coeEnv->WsSession());
        m_screenDevice = coeEnv->ScreenDevice();
        m_window = m_windowId->DrawableWindow();
        return true;
    }
    return false;
}

bool S60VideoPlayerSession::isVideoAvailable() const
{
#ifdef PRE_S60_50_PLATFORM
    return true; // this is not support in pre 5th platforms
#endif    
    return m_player->VideoEnabledL();
}

bool S60VideoPlayerSession::isAudioAvailable() const
{
    if (m_player)
        return m_player->AudioEnabledL();
    else
        return false;
}

void S60VideoPlayerSession::doPlay()
{   
    m_player->Play();
}

void S60VideoPlayerSession::doPauseL()
{
    m_player->PauseL();
}

void S60VideoPlayerSession::doStop()
{
    m_player->Stop();
}

qint64 S60VideoPlayerSession::doGetPositionL() const
{
    return m_player->PositionL().Int64() / 1000;
}
void S60VideoPlayerSession::doSetPositionL(qint64 microSeconds)
{
    m_player->SetPositionL(TTimeIntervalMicroSeconds(microSeconds));
}

void S60VideoPlayerSession::doSetVolumeL(int volume)
{
    m_player->SetVolumeL((volume / 100.0)* m_player->MaxVolume());
}

void S60VideoPlayerSession::MvpuoOpenComplete(TInt aError)
{
    setError(aError);
    m_player->Prepare();
}

void S60VideoPlayerSession::MvpuoPrepareComplete(TInt aError)
{
    setError(aError);
    TRAPD(err, 
        m_player->SetDisplayWindowL(*m_wsSession, 
                                    *m_screenDevice, 
                                    *m_window, 
                                    m_windowRect, 
                                    m_clipRect));
    setError(err);
    initComplete();
}

void S60VideoPlayerSession::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
{
    Q_UNUSED(aFrame);
    Q_UNUSED(aError);
}

void S60VideoPlayerSession::MvpuoPlayComplete(TInt aError)
{
    setError(aError);
    playComplete();
}

void S60VideoPlayerSession::MvpuoEvent(const TMMFEvent &aEvent)
{
    Q_UNUSED(aEvent);
}

void S60VideoPlayerSession::updateMetaDataEntriesL()
{
    metaDataEntries().clear();
    int numberOfMetaDataEntries = 0;
    
    numberOfMetaDataEntries = m_player->NumberOfMetaDataEntriesL();
    
    for (int i = 0; i < numberOfMetaDataEntries; i++) {
        CMMFMetaDataEntry *entry = NULL;
        entry = m_player->MetaDataEntryL(i);
        metaDataEntries().insert(QString::fromUtf16(entry->Name().Ptr(), entry->Name().Length()), QString::fromUtf16(entry->Value().Ptr(), entry->Value().Length()));
        delete entry;
    }
    emit metaDataChanged();
}

void S60VideoPlayerSession::resetVideoDisplay()
{
    if (resetNativeHandles()) {
        TRAPD(err, m_player->SetDisplayWindowL(*m_wsSession, 
                                               *m_screenDevice, 
                                               *m_window, 
                                               m_windowRect, 
                                               m_clipRect));
        setError(err);
    }
}
