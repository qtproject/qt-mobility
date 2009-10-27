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

#include "s60videoplayersession.h"

#include "s60videorendererinterface.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>

#include <coemain.h>    // For CCoeEnv
#include <coecntrl.h>
#include <w32std.h>

#include <QWidget>
#include <QDir>

S60VideoPlayerSession::S60VideoPlayerSession(QObject *parent)
    :QObject(parent),
     m_state(QMediaPlayer::StoppedState),
     m_mediaStatus(QMediaPlayer::UnknownMediaStatus),
     //m_busHelper(0),
     //m_playbin(0),
     //m_bus(0),
     m_renderer(0),
     m_volume(100),
     m_playbackRate(1.0),
     m_muted(false),
     m_videoAvailable(false),
     m_seekable(false),
     m_lastPosition(0),
     m_duration(-1),
     m_wsSession(0),
     m_screenDevice(0),
     m_window(0)
{
    //TRACE_CONTEXT(VideoPlayer::VideoPlayer, EVideoApi);
    //TRACE_ENTRY_0();

    /*if (!m_videoOutput) {
        m_dummyVideoOutput.reset(new VideoOutput(0));
    }

    videoOutput().setObserver(this);
    */
    
    m_frameSize = QSize(320,240);
   /*const TInt priority = 0;
    const TMdaPriorityPreference preference = EMdaPriorityPreferenceNone;

    //getNativeWindowSystemHandles();

    // TODO: is this the correct way to handle errors which occur when
    // creating a Symbian object in the constructor of a Qt object?
    
    // TODO: check whether videoOutput is visible?  If not, then the 
    // corresponding window will not be active, meaning that the 
    // clipping region will be set to empty and the video will not be
    // visible.  If this is the case, we should set m_mmfOutputChangePending
    // and respond to future showEvents from the videoOutput widget.
    
    //VideoOutput& output = videoOutput();  
    //QWidget* jep = new QWidget();
    CCoeControl* const control = jep->winId();

    CCoeEnv* const coeEnv = control->ControlEnv();
    m_wsSession = &(coeEnv->WsSession());
    m_screenDevice = coeEnv->ScreenDevice();
    m_window = control->DrawableWindow();
*/
 /* #ifdef _DEBUG
      QScopedPointer<ObjectDump::QDumper> dumper(new ObjectDump::QDumper);
      dumper->setPrefix("Phonon::MMF"); // to aid searchability of logs
      ObjectDump::addDefaultAnnotators(*dumper);
      TRACE_0("Dumping VideoOutput:");
      dumper->dumpObject(output);
  #endif*/
/*
    m_windowRect = TRect(
        control->DrawableWindow()->AbsPosition(),
        control->DrawableWindow()->Size());
      
    m_clipRect = m_windowRect;
      
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
    */
}

S60VideoPlayerSession::~S60VideoPlayerSession()
{
    delete m_player;
}

void S60VideoPlayerSession::load(const QUrl &url)
{
    m_url = url.toLocalFile();
    qDebug() << m_url;
}

qint64 S60VideoPlayerSession::duration() const
{
    if (m_player) 
        //m_duration = m_player->DurationL().Int64(); 
    return m_duration;
}

qint64 S60VideoPlayerSession::position() const
{
    qint64 position = 0;
    if (m_player)
        position = m_player->PositionL().Int64();
    return position;
}

qreal S60VideoPlayerSession::playbackRate() const
{
    return m_playbackRate;
}

void S60VideoPlayerSession::setPlaybackRate(qreal rate)
{
    m_playbackRate = rate;
/*
    if (m_playbin) {
        gst_element_seek(m_playbin, rate, GST_FORMAT_TIME,
                         GstSeekFlags(GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT),
                         GST_SEEK_TYPE_NONE,0,
                         GST_SEEK_TYPE_NONE,0 );
    }
    */

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
   // m_testWidget->videoWidget()->winId();
   //qWarning()<<"setVideoRenderer";
	//m_renderer = qobject_cast<S60VideoRendererInterface*>(videoOutput);
    m_testWidget = qobject_cast<S60VideoWidgetControl*>(videoOutput);

    const TInt priority = 0;
    const TMdaPriorityPreference preference = EMdaPriorityPreferenceNone;
    
    getNativeHandles();   
    //m_clipRect = m_windowRect;
      
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

void S60VideoPlayerSession::getNativeHandles()
{
    qDebug() << "winId: " << m_testWidget->videoWidget()->effectiveWinId();
    //m_renderer->winId();

    //m_testWidget = qobject_cast<S60VideoWidgetControl*>(videoOutput);
    //QWidget* jep = new QWidget();
   // CCoeControl* const control =  m_testWidget->videoWidget()->winId();
    CCoeControl* const control =  m_testWidget->videoWidget()->winId();
    control->MakeVisible(ETrue);
    control->SetSize(TSize(240,245));
    control->SetPosition(TPoint(0,0));
    control->ActivateL();
    qDebug() << control->IsVisible();

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
    if (m_state == QMediaPlayer::PausedState)
    {
        m_player->Play();
        m_state = QMediaPlayer::PlayingState;
        emit stateChanged(QMediaPlayer::PlayingState); 
    }
    else
    {
        QString fileName = QDir::toNativeSeparators(m_url.toString());
		qDebug() << fileName;
        TPtrC str(reinterpret_cast<const TUint16*>(fileName.utf16()));
        //m_player->OpenFileL(str);
        //m_player->OpenFileL(_L("c:\\Data\\testvideo.mp4"));
        //QString fileName = m_url.toLocalFile();
        //TPtrC str(reinterpret_cast<const TUint16*>(fileName.utf16()));
        m_player->OpenFileL(str);
        m_state = QMediaPlayer::PlayingState;
        emit stateChanged(QMediaPlayer::PlayingState);        
    }
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
    m_player->SetPositionL(ms);
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

void S60VideoPlayerSession::setSeekable(bool seekable)
{
    if (seekable != m_seekable) {
        m_seekable = seekable;
        emit seekableChanged(m_seekable);
    }
}

void S60VideoPlayerSession::setMediaStatus(QMediaPlayer::MediaStatus status)
{
    if (m_mediaStatus != status) {
        m_mediaStatus = status;
        emit mediaStatusChanged(status);
    }
}

void S60VideoPlayerSession::getStreamsInfo()
{
}

void S60VideoPlayerSession::MvpuoOpenComplete(TInt aError)
{
    qDebug() << "Preparing to play with error: " << aError;
    m_player->Prepare();
   // m_player->StartDirectScreenAccessL();
    //getNativeHandles();
    
/*    TRAPD(err,
          m_player->SetDisplayWindowL
          (
              *m_wsSession, *m_screenDevice,
              *m_window,
              m_windowRect, m_clipRect
          )
         );
*/
}

void S60VideoPlayerSession::MvpuoPrepareComplete(TInt aError)
{
    qDebug() << "Starting to play with error: " << aError;
    m_player->Play();
    //m_totalTime = m_player->DurationL();
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
    qDebug() << "Play completed with error: " << aError;
}

void S60VideoPlayerSession::MvpuoEvent(const TMMFEvent &aEvent)
{
    Q_UNUSED(aEvent);
}
