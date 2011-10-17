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

#include "s60videoplayersession.h"
#include "s60mediaplayerservice.h"
#include "s60videowidgetcontrol.h"
#include "s60videowidgetdisplay.h"
#include "s60videowindowcontrol.h"
#include "s60videowindowdisplay.h"
#include "s60mmftrace.h"

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QSymbianEvent>
#include <QtGui/QWidget>

#include <coecntrl.h>
#include <coemain.h>    // For CCoeEnv
#include <w32std.h>
#include <mmf/common/mmferrors.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>
#include <mmf/common/mmfvideo.h>
#include <MMFROPCustomCommandConstants.h>
#ifdef HTTP_COOKIES_ENABLED
#include <MMFSessionInfoCustomCommandConstants.h>
#endif

const QString DefaultAudioEndpoint = QLatin1String("Default");
const TUid KHelixUID = {0x101F8514};

//Hard-coding the command to support older versions.
const TInt KMMFROPControllerEnablePausedLoadingStatus = 7;

TVideoRotation videoRotation(qreal angle)
{
    // Convert to clockwise
    angle = 360.0f - angle;
    while (angle >= 360.0f)
        angle -= 360.0f;
    TVideoRotation result = EVideoRotationNone;
    if (angle >= 45.0f && angle < 135.0f)
        result = EVideoRotationClockwise90;
    else if (angle >= 135.0f && angle < 225.0f)
        result = EVideoRotationClockwise180;
    else if (angle >= 225.0f && angle < 315.0f)
        result = EVideoRotationClockwise270;
    return result;
}

S60VideoPlayerEventHandler *S60VideoPlayerEventHandler::m_instance = 0;
QCoreApplication::EventFilter S60VideoPlayerEventHandler::m_eventFilter = 0;
QList<ApplicationFocusObserver *> S60VideoPlayerEventHandler::m_applicationFocusObservers;

S60VideoPlayerEventHandler *S60VideoPlayerEventHandler::instance()
{
    if (!m_instance)
        m_instance = new S60VideoPlayerEventHandler();
    return m_instance;
}

S60VideoPlayerEventHandler::S60VideoPlayerEventHandler()
{
    m_eventFilter = QCoreApplication::instance()->setEventFilter(filterEvent);
}

S60VideoPlayerEventHandler::~S60VideoPlayerEventHandler()
{
    QCoreApplication::instance()->setEventFilter(m_eventFilter);
}

void S60VideoPlayerEventHandler::addApplicationFocusObserver(ApplicationFocusObserver *observer)
{
    m_applicationFocusObservers.append(observer);
}

void S60VideoPlayerEventHandler::removeApplicationFocusObserver(ApplicationFocusObserver *observer)
{
    m_applicationFocusObservers.removeAt(m_applicationFocusObservers.indexOf(observer));
    if (m_applicationFocusObservers.count() == 0) {
        delete m_instance;
        m_instance = 0;
    }
}

bool S60VideoPlayerEventHandler::filterEvent(void *message, long *result)
{
    if (const QSymbianEvent *symbianEvent = reinterpret_cast<const QSymbianEvent*>(message)) {
        switch (symbianEvent->type()) {
        case QSymbianEvent::WindowServerEvent:
            {
                const TWsEvent *wsEvent = symbianEvent->windowServerEvent();
                if (EEventFocusLost == wsEvent->Type() || EEventFocusGained == wsEvent->Type()) {
                    for (QList<ApplicationFocusObserver *>::const_iterator it = m_applicationFocusObservers.constBegin();
                         it != m_applicationFocusObservers.constEnd(); ++it) {
                        if (EEventFocusLost == wsEvent->Type())
                            (*it)->applicationLostFocus();
                        else if (EEventFocusGained == wsEvent->Type())
                            (*it)->applicationGainedFocus();
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    bool ret = false;
    if (m_eventFilter)
        ret = m_eventFilter(message, result);
    return ret;
}

/*!
    Constructs the CVideoPlayerUtility2 object with given \a service and \a object.
    And Registers for Video Loading Notifications.
*/
S60VideoPlayerSession::S60VideoPlayerSession(QMediaService *service, S60MediaNetworkAccessControl *object)
    : S60MediaPlayerSession(service)
    , m_accessPointId(0)
    , m_wsSession(&CCoeEnv::Static()->WsSession())
    , m_screenDevice(CCoeEnv::Static()->ScreenDevice())
    , m_service(service)
    , m_player(0)
#ifndef VIDEOOUTPUT_GRAPHICS_SURFACES
    , m_dsaActive(false)
    , m_dsaStopped(false)
#endif
    , m_videoOutputControl(0)
    , m_videoOutputDisplay(0)
    , m_displayWindow(0)
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    , m_audioOutput(0)
#endif
    , m_audioEndpoint(DefaultAudioEndpoint)
    , m_pendingChanges(0)
    , m_backendInitiatedPause(false)
#ifdef HTTP_COOKIES_ENABLED
    , m_destinationPckg(KUidInterfaceMMFControllerSessionInfo)
#endif
{
    TRACE("S60VideoPlayerSession::S60VideoPlayerSession" << qtThisPtr());

    m_networkAccessControl = object;
#ifdef VIDEOOUTPUT_GRAPHICS_SURFACES
    QT_TRAP_THROWING(m_player = CVideoPlayerUtility2::NewL(
        *this,
        0,
        EMdaPriorityPreferenceNone
        ));
    m_player->RegisterForVideoLoadingNotification(*this);
#else
    RWindow *window = 0;
    QRect extentRect;
    QWidget *widget = QApplication::activeWindow();
    if (!widget)
        widget = QApplication::allWidgets().at(0);
    Q_ASSERT(widget);
    WId wid = widget->effectiveWinId();
    if (!wid)
        wid = widget->winId();
    window = static_cast<RWindow *>(wid->DrawableWindow());
    extentRect = QRect(widget->mapToGlobal(widget->pos()), widget->size());
    TRect clipRect = QRect2TRect(extentRect);
    const TRect desktopRect = QRect2TRect(QApplication::desktop()->screenGeometry());
    clipRect.Intersection(desktopRect);
    QT_TRAP_THROWING(m_player = CVideoPlayerUtility::NewL(
            *this,
            0,
            EMdaPriorityPreferenceNone,
            *m_wsSession,
            *m_screenDevice,
            *window,
            QRect2TRect(extentRect),
            clipRect));
    m_dsaActive = true;
    m_player->RegisterForVideoLoadingNotification(*this);
#endif // VIDEOOUTPUT_GRAPHICS_SURFACES
    S60VideoPlayerEventHandler::instance()->addApplicationFocusObserver(this);
}

/*!
   Destroys the CVideoPlayerUtility2 object.

  And Unregister the observer.
*/

S60VideoPlayerSession::~S60VideoPlayerSession()
{
    TRACE("S60VideoPlayerSession::~S60VideoPlayerSession" << qtThisPtr());
    S60VideoPlayerEventHandler::instance()->removeApplicationFocusObserver(this);
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
#endif
    m_player->Close();
    delete m_player;
}

void S60VideoPlayerSession::applicationGainedFocus()
{
    TRACE("S60VideoPlayerSession::applicationGainedFocus" << qtThisPtr());
    if (m_backendInitiatedPause) {
        m_backendInitiatedPause = false;
        play();
    }
    if (QMediaPlayer::PausedState == state()) {
       TRAPD(err, m_player->RefreshFrameL());
       setError(err);
    }
}

void S60VideoPlayerSession::applicationLostFocus()
{
    TRACE("S60VideoPlayerSession::applicationLostFocus" << qtThisPtr());
    if (QMediaPlayer::PlayingState == state()) {
        if (!m_isaudiostream) {
        m_backendInitiatedPause = true;
        pause();
        }
    }
}

/*!

   Opens the a file from \a path.
*/

void S60VideoPlayerSession::doLoadL(const TDesC &path)
{
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    // m_audioOutput needs to be reinitialized after MapcInitComplete
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
    m_audioOutput = NULL;
#endif
    m_player->OpenFileL(path, KHelixUID);
}

/*!
    Sets the playbackRate with \a rate.
*/

void S60VideoPlayerSession::setPlaybackRate(qreal rate)
{
    /*
     * setPlaybackRate is not supported in S60 3.1 and 3.2
     * This flag will be defined for 3.1 and 3.2
    */
#ifdef PLAY_RATE_NOT_SUPPORTED
    TRACE("S60VideoPlayerSession::setPlaybackRate" << qtThisPtr()
          << "rate" << rate << "not supported");
#else
    TRACE("S60VideoPlayerSession::setPlaybackRate" << qtThisPtr() << "rate" << rate);
    //setPlayVelocity requires rate in the form of
    //50 = 0.5x ;100 = 1.x ; 200 = 2.x ; 300 = 3.x
    //so multiplying rate with 100
    TRAPD(err, m_player->SetPlayVelocityL((TInt)(rate*100)));
    if (KErrNone == err)
        emit playbackRateChanged(rate);
    else
        setError(err);
#endif
}

/*!

   Opens the a Url from \a path for streaming the source.
*/

void S60VideoPlayerSession::doLoadUrlL(const TDesC &path)
{
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    // m_audioOutput needs to be reinitialized after MapcInitComplete
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
    m_audioOutput = NULL;
#endif
    m_accessPointId = m_networkAccessControl->accessPointId();
    m_player->OpenUrlL(path, m_accessPointId, KNullDesC8, KHelixUID);
}

/*!

    Returns the percentage of the video clip loaded.
*/

int S60VideoPlayerSession::doGetBufferStatusL() const
{
    int progress = 0;
    m_player->GetVideoLoadingProgressL(progress);
    return progress;
}

/*!
  Returns the duration of the video sample in microseconds.
*/

qint64 S60VideoPlayerSession::doGetDurationL() const
{
    return m_player->DurationL().Int64() / qint64(1000);
}

/*!
 * Sets the \a videooutput for video rendering.
*/

void S60VideoPlayerSession::setVideoRenderer(QObject *videoOutput)
{
    TRACE("S60VideoPlayerSession::setVideoRenderer" << qtThisPtr()
          << "output" << videoOutput);
    if (videoOutput != m_videoOutputControl) {
        if (m_videoOutputDisplay) {
            m_videoOutputDisplay->setHasContent(false);
            disconnect(m_videoOutputDisplay);
            m_videoOutputDisplay->disconnect(this);
            m_videoOutputDisplay = 0;
        }
        if (videoOutput) {
            if (S60VideoWidgetControl *control = qobject_cast<S60VideoWidgetControl *>(videoOutput))
                m_videoOutputDisplay = control->display();
            if (!m_videoOutputDisplay)
                return;
            m_videoOutputDisplay->setHasContent(QMediaPlayer::PlayingState == state());
            m_videoOutputDisplay->setNativeSize(m_nativeSize);
            connect(this, SIGNAL(nativeSizeChanged(QSize)), m_videoOutputDisplay, SLOT(setNativeSize(QSize)));
            connect(m_videoOutputDisplay, SIGNAL(windowHandleChanged(RWindow *)), this, SLOT(windowHandleChanged()));
            connect(m_videoOutputDisplay, SIGNAL(displayRectChanged(QRect, QRect)), this, SLOT(displayRectChanged()));
            connect(m_videoOutputDisplay, SIGNAL(aspectRatioModeChanged(Qt::AspectRatioMode)), this, SLOT(aspectRatioChanged()));
            connect(m_videoOutputDisplay, SIGNAL(rotationChanged(qreal)), this, SLOT(rotationChanged()));
#ifndef VIDEOOUTPUT_GRAPHICS_SURFACES
            connect(m_videoOutputDisplay, SIGNAL(beginVideoWindowNativePaint()), this, SLOT(suspendDirectScreenAccess()));
            connect(m_videoOutputDisplay, SIGNAL(endVideoWindowNativePaint()), this, SLOT(resumeDirectScreenAccess()));
#endif
        }
        m_videoOutputControl = videoOutput;
        windowHandleChanged();
    }
}

/*!
 * Apply the pending changes for window.
*/
void S60VideoPlayerSession::applyPendingChanges(bool force)
{
    TRACE("S60VideoPlayerSession::applyPendingChanges" << qtThisPtr()
          << "mediaStatus" << mediaStatus() << "force" << force
          << "pendingChanges" << m_pendingChanges);

    if (   force
        || QMediaPlayer::LoadedMedia == mediaStatus()
        || QMediaPlayer::StalledMedia == mediaStatus()
        || QMediaPlayer::BufferingMedia == mediaStatus()
        || QMediaPlayer::BufferedMedia == mediaStatus()
        || QMediaPlayer::EndOfMedia == mediaStatus()) {
        int error = KErrNone;
        RWindow *const window = m_videoOutputDisplay ? m_videoOutputDisplay->windowHandle() : 0;
        const QRect extentRect = m_videoOutputDisplay ? m_videoOutputDisplay->extentRect() : QRect();
        const QRect clipRect = m_videoOutputDisplay ? m_videoOutputDisplay->clipRect() : QRect();
#ifdef VIDEOOUTPUT_GRAPHICS_SURFACES
        if (m_pendingChanges & WindowHandle) {
            if (window && window != m_displayWindow ) {
                TRAP(error, m_player->AddDisplayWindowL(*m_wsSession, *m_screenDevice,
                                                        *window,
                                                        QRect2TRect(extentRect),
                                                        QRect2TRect(clipRect)));
            }
            if (m_displayWindow && m_displayWindow != window && KErrNone == error){
                m_player->RemoveDisplayWindow(*m_displayWindow);
                m_displayWindow = 0;
            }
            if (KErrNone == error && window )
                m_displayWindow = window;
            m_pendingChanges = ScaleFactors;
        }
        if (KErrNone == error && (m_pendingChanges & DisplayRect) && m_displayWindow) {
            TRAP(error, m_player->SetVideoExtentL(*m_displayWindow, QRect2TRect(extentRect)));
            if (KErrNone == error)
                TRAP(error, m_player->SetWindowClipRectL(*m_displayWindow, QRect2TRect(clipRect)));
            m_pendingChanges ^= DisplayRect;
            m_pendingChanges |= ScaleFactors;
        }
#else
        if (m_pendingChanges & WindowHandle || m_pendingChanges & DisplayRect) {
            if (window) {
               TRAP(error, m_player->SetDisplayWindowL(*m_wsSession, *m_screenDevice,
                                                       *window,
                                                       QRect2TRect(extentRect),
                                                       QRect2TRect(clipRect)));
               if (KErrNone == error)
                   m_displayWindow = window;
            }
            m_dsaActive = (KErrNone == error);
            m_dsaStopped = false;
            m_pendingChanges = ScaleFactors;
        }

#endif // VIDEOOUTPUT_GRAPHICS_SURFACES
        if (KErrNone == error && (m_pendingChanges & ScaleFactors) && m_displayWindow && m_videoOutputDisplay) {
            const TVideoRotation rotation = videoRotation(m_videoOutputDisplay->rotation());
            const bool swap = (rotation == EVideoRotationClockwise90 || rotation == EVideoRotationClockwise270);
            const QSize extentSize = swap ? QSize(extentRect.height(), extentRect.width()) : extentRect.size();
            QSize scaled = m_nativeSize;
            if (m_videoOutputDisplay->aspectRatioMode() == Qt::IgnoreAspectRatio)
                scaled.scale(extentSize, Qt::IgnoreAspectRatio);
            else if (m_videoOutputDisplay->aspectRatioMode() == Qt::KeepAspectRatio)
                scaled.scale(extentSize, Qt::KeepAspectRatio);
            else if (m_videoOutputDisplay->aspectRatioMode() == Qt::KeepAspectRatioByExpanding)
                scaled.scale(extentSize, Qt::KeepAspectRatioByExpanding);
            const qreal width = qreal(scaled.width()) / qreal(m_nativeSize.width()) * qreal(100);
            const qreal height = qreal(scaled.height()) / qreal(m_nativeSize.height()) * qreal(100);
#ifdef VIDEOOUTPUT_GRAPHICS_SURFACES
            TRAP(error, m_player->SetScaleFactorL(*m_displayWindow, width, height));
#else
            static const TBool antialias = ETrue;
            TRAP(error, m_player->SetScaleFactorL(width, height, antialias));
#endif // VIDEOOUTPUT_GRAPHICS_SURFACES
            m_pendingChanges ^= ScaleFactors;
        }
        if (KErrNone == error && (m_pendingChanges && Rotation) && m_displayWindow && m_videoOutputDisplay) {
            const TVideoRotation rotation = videoRotation(m_videoOutputDisplay->rotation());
#ifdef VIDEOOUTPUT_GRAPHICS_SURFACES
            TRAP(error, m_player->SetRotationL(*m_displayWindow, rotation));
#else
            TRAP(error, m_player->SetRotationL(rotation));
#endif // VIDEOOUTPUT_GRAPHICS_SURFACES
            m_pendingChanges ^= Rotation;
        }
        setError(error);
    }
}

/*!
 * \return TRUE if video is available.
*/

bool S60VideoPlayerSession::isVideoAvailable()
{
#ifdef PRE_S60_50_PLATFORM
    return true; // this is not supported in pre 5th platforms
#else
    if ( mediaStatus() == QMediaPlayer::LoadingMedia
        || mediaStatus() == QMediaPlayer::UnknownMediaStatus
        || mediaStatus() == QMediaPlayer::NoMedia
        || (mediaStatus() == QMediaPlayer::StalledMedia && state() == QMediaPlayer::StoppedState)
        || mediaStatus() == QMediaPlayer::InvalidMedia)
        return false;

    if (m_player) {
        bool videoAvailable = false;
        TRAPD(err, videoAvailable = m_player->VideoEnabledL());
        setError(err);
        return videoAvailable;
    } else {
        return false;
    }
#endif

}

/*!
 * \return TRUE if Audio available.
*/

bool S60VideoPlayerSession::isAudioAvailable()
{
    if ( mediaStatus() == QMediaPlayer::LoadingMedia
        || mediaStatus() == QMediaPlayer::UnknownMediaStatus
        || mediaStatus() == QMediaPlayer::NoMedia
        || (mediaStatus() == QMediaPlayer::StalledMedia && state() == QMediaPlayer::StoppedState)
        || mediaStatus() == QMediaPlayer::InvalidMedia)
         return false;

    if (m_player) {
        bool audioAvailable = false;
        TRAPD(err, audioAvailable = m_player->AudioEnabledL());
        setError(err);
        return audioAvailable;
    } else {
        return false;
    }
}

/*!
    Start or resume playing the current source.
*/

void S60VideoPlayerSession::doPlay()
{
    m_player->Play();
    if (m_videoOutputDisplay)
        m_videoOutputDisplay->setHasContent(true);
}

/*!
    Pause playing the current source.
*/

void S60VideoPlayerSession::doPauseL()
{
    m_player->PauseL();
}

/*!

    Stop playing, and reset the play position to the beginning.
*/

void S60VideoPlayerSession::doStop()
{
    if (m_stream)
        m_networkAccessControl->resetIndex();

    if (m_videoOutputDisplay)
        m_videoOutputDisplay->setHasContent(false);

    m_player->Stop();
}

/*!
    Closes the current audio clip (allowing another clip to be opened)
*/

void S60VideoPlayerSession::doClose()
{
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    if (m_audioOutput) {
        m_audioOutput->UnregisterObserver(*this);
        delete m_audioOutput;
        m_audioOutput = NULL;
    }
#endif

    m_player->Close();

// close will remove the window handle in media clint video.
// So mark it in pending changes.
    m_pendingChanges |= WindowHandle;
}

/*!
 *  Returns the current playback position in microseconds from the start of the clip.

*/

qint64 S60VideoPlayerSession::doGetPositionL() const
{
    return m_player->PositionL().Int64() / qint64(1000);
}

/*!
    Sets the current playback position to \a microSeconds from the start of the clip.
*/

void S60VideoPlayerSession::doSetPositionL(qint64 microSeconds)
{
    m_player->SetPositionL(TTimeIntervalMicroSeconds(microSeconds));
}

/*!

    Changes the current playback volume to specified \a value.
*/

void S60VideoPlayerSession::doSetVolumeL(int volume)
{
    m_player->SetVolumeL(volume * m_player->MaxVolume() / 100);
}

/*!
 * Notification to the client that the opening of the video clip has completed.
 * If successful then an \a aError will be ZERO else system wide error.
*/

void S60VideoPlayerSession::MvpuoOpenComplete(TInt aError)
{
    TRACE("S60VideoPlayerSession::MvpuoOpenComplete" << qtThisPtr()
          << "error" << aError);

    setError(aError);
#ifdef HTTP_COOKIES_ENABLED
    if (KErrNone == aError) {
        TInt err(KErrNone);
        const QByteArray userAgentString("User-Agent");
        TInt uasize = m_source.canonicalRequest().rawHeader(userAgentString).size();
        TPtrC8 userAgent((const unsigned char*)(m_source.canonicalRequest().rawHeader(userAgentString).constData()), uasize);
        if (userAgent.Length()) {
            err = m_player->CustomCommandSync(m_destinationPckg, EMMFSetSessionInfo, _L8("User-Agent"), userAgent);
            if (err != KErrNone) {
                setError(err);
                return;
            }
        }
        const QByteArray refererString("Referer");
        TInt refsize = m_source.canonicalRequest().rawHeader(refererString).size();
        TPtrC8 referer((const unsigned char*)m_source.canonicalRequest().rawHeader(refererString).constData(),refsize);
        if (referer.Length()) {
            err = m_player->CustomCommandSync(m_destinationPckg, EMMFSetSessionInfo, _L8("Referer"), referer);
            if (err != KErrNone) {
                setError(err);
                return;
            }
        }
        const QByteArray cookieString("Cookie");
        TInt cksize = m_source.canonicalRequest().rawHeader(cookieString).size();
        TPtrC8 cookie((const unsigned char*)m_source.canonicalRequest().rawHeader(cookieString).constData(),cksize);
        if (cookie.Length()) {
            err = m_player->CustomCommandSync(m_destinationPckg, EMMFSetSessionInfo, _L8("Cookie"), cookie);
            if (err != KErrNone) {
                setError(err);
                return;
            }
        }
        m_player->Prepare();
    }
#else
    if (KErrNone == aError)
        m_player->Prepare();
#endif
    const TMMFMessageDestinationPckg dest( KUidInterfaceMMFROPController );
    TRAP_IGNORE(m_player->CustomCommandSync(dest, KMMFROPControllerEnablePausedLoadingStatus, KNullDesC8, KNullDesC8));
}

/*!
 * Notification to the client that the opening of the video clip has been preapred.
 * If successful then an \a aError will be ZERO else system wide error.
*/

void S60VideoPlayerSession::MvpuoPrepareComplete(TInt aError)
{
    TRACE("S60VideoPlayerSession::MvpuoPrepareComplete" << qtThisPtr()
          << "error" << aError);

    if (KErrNone == aError && m_stream) {
        emit accessPointChanged(m_accessPointId);
        }
    if (KErrCouldNotConnect == aError && !(m_networkAccessControl->isLastAccessPoint())) {
        load(m_source);
    return;
    }
    TInt error = aError;
    if (KErrNone == error || KErrMMPartialPlayback == error) {
        TSize originalSize;
        TRAP(error, m_player->VideoFrameSizeL(originalSize));
        if (KErrNone == error) {
            m_nativeSize = QSize(originalSize.iWidth, originalSize.iHeight);
            emit nativeSizeChanged(m_nativeSize);
            m_pendingChanges |= ScaleFactors;
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
            Q_ASSERT(!m_audioOutput);
            TRAP(error, m_audioOutput = CAudioOutput::NewL(*m_player));
            if (KErrNone == error) {
                TRAP(error, m_audioOutput->RegisterObserverL(*this));
                if (KErrNone == error)
                    setActiveEndpoint(m_audioEndpoint);
            }
#endif
        }
        if (KErrNone == error) {
        // changes made to play without pausing in case of audio streaming use case
            if (m_player->VideoFormatMimeType().Length() == 0) {
            m_isaudiostream = true;
            m_backendInitiatedPause = false;
            play();
            } else {
            m_isaudiostream = false;
            }
            applyPendingChanges(true); // force apply even though state is not Loaded
            if (KErrNone == this->error()) // applyPendingChanges() can call setError()
                loaded();
        }
    } else {
        setError(error);
    }
}

/*!
 * Notification that frame requested by a call to GetFrameL is ready.
*/

void S60VideoPlayerSession::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
{
    Q_UNUSED(aFrame);
    Q_UNUSED(aError);
}

/*!
 * Notification that video playback has completed.
 * If successful then \a aError will be ZERO else system wide error.
 * This not called if playback is explicitly stopped by calling stop.
*/

void S60VideoPlayerSession::MvpuoPlayComplete(TInt aError)
{
    TRACE("S60VideoPlayerSession::MvpuoPlayComplete" << qtThisPtr()
          << "error" << aError);

    if (m_stream)
    m_networkAccessControl->resetIndex();

    if (aError != KErrNone) {
        setError(aError);
        doClose();
    } else {
        endOfMedia();
    }
}


/*!
 * General \a event notification from controller.
 * These events are specified by the supplier of the controller.
*/

void S60VideoPlayerSession::MvpuoEvent(const TMMFEvent &aEvent)
{
    TRACE("S60VideoPlayerSession::MvpuoEvent" << qtThisPtr()
          << "type" << (void*)aEvent.iEventType.iUid
          << "error" << aEvent.iErrorCode);
    if (aEvent.iEventType == KMMFEventCategoryVideoPlayerGeneralError) {
        setError(aEvent.iErrorCode);
        doClose();
    }
}

/*!

    Updates meta data entries in the current video clip.
*/

void S60VideoPlayerSession::updateMetaDataEntriesL()
{
    metaDataEntries().clear();
    int numberOfMetaDataEntries = 0;
    numberOfMetaDataEntries = m_player->NumberOfMetaDataEntriesL();
    for (int i = 0; i < numberOfMetaDataEntries; i++) {
        CMMFMetaDataEntry *entry = NULL;
        entry = m_player->MetaDataEntryL(i);
        metaDataEntries().insert(TDesC2QString(entry->Name()), TDesC2QString(entry->Value()));
        delete entry;
    }
    emit metaDataChanged();
}

/*!
 * Apply the window changes when window handle changes.
*/

void S60VideoPlayerSession::windowHandleChanged()
{
    TRACE("S60VideoPlayerSession::windowHandleChanged" << qtThisPtr());

    m_pendingChanges |= WindowHandle;
    applyPendingChanges();
}

/*!
 * Apply the window changes when display Rect changes.
*/

void S60VideoPlayerSession::displayRectChanged()
{
    TRACE("S60VideoPlayerSession::displayRectChanged" << qtThisPtr());

    m_pendingChanges |= DisplayRect;
    applyPendingChanges();
}

/*!
 * Apply the window changes when aspect Ratio changes.
*/

void S60VideoPlayerSession::aspectRatioChanged()
{
    TRACE("S60VideoPlayerSession::aspectRatioChanged" << qtThisPtr());

    m_pendingChanges |= ScaleFactors;
    applyPendingChanges();
}

void S60VideoPlayerSession::rotationChanged()
{
    TRACE("S60VideoPlayerSession::rotationChanged" << qtThisPtr());

    m_pendingChanges |= ScaleFactors;
    m_pendingChanges |= Rotation;
    applyPendingChanges();
}

#ifndef VIDEOOUTPUT_GRAPHICS_SURFACES
void S60VideoPlayerSession::suspendDirectScreenAccess()
{
    TRACE("S60VideoPlayerSession::suspendDirectScreenAccess" << qtThisPtr());

    m_dsaStopped = stopDirectScreenAccess();
}

void S60VideoPlayerSession::resumeDirectScreenAccess()
{
    TRACE("S60VideoPlayerSession::resumeDirectScreenAccess" << qtThisPtr());

    if (!m_dsaStopped)
        return;
    startDirectScreenAccess();
    m_dsaStopped = false;
}

void S60VideoPlayerSession::startDirectScreenAccess()
{
    TRACE("S60VideoPlayerSession::startDirectScreenAccess" << qtThisPtr());

    if (m_dsaActive)
        return;
    TRAPD(err, m_player->StartDirectScreenAccessL());
    if (err == KErrNone)
        m_dsaActive = true;
    setError(err);
}

bool S60VideoPlayerSession::stopDirectScreenAccess()
{
    TRACE("S60VideoPlayerSession::stopDirectScreenAccess" << qtThisPtr());

    if (!m_dsaActive)
        return false;
    TRAPD(err, m_player->StopDirectScreenAccessL());
    if (err == KErrNone)
        m_dsaActive = false;
    setError(err);
    return true;
}
#endif

/*!
 *  The percentage of the temporary buffer filling before playback begins.
*/

void S60VideoPlayerSession::MvloLoadingStarted()
{
    TRACE("S60VideoPlayerSession::MvloLoadingStarted" << qtThisPtr());

    buffering();
}

/*!
 * Buffer is filled with data and to for continuing/start playback.
*/

void S60VideoPlayerSession::MvloLoadingComplete()
{
    TRACE("S60VideoPlayerSession::MvloLoadingComplete" << qtThisPtr());

    buffered();
}

/*!
    Defiens which Audio End point to use.

    \a audioEndpoint audioEndpoint name.
*/

void S60VideoPlayerSession::doSetAudioEndpoint(const QString& audioEndpoint)
{
    m_audioEndpoint = audioEndpoint;
}

/*!

    Returns audioEndpoint name.
*/

QString S60VideoPlayerSession::activeEndpoint() const
{
    QString outputName = m_audioEndpoint;
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    if (m_audioOutput) {
        CAudioOutput::TAudioOutputPreference output = m_audioOutput->AudioOutput();
        outputName = qStringFromTAudioOutputPreference(output);
    }
#endif
    return outputName;
}

/*!
 *  Returns default Audio End point in use.
*/

QString S60VideoPlayerSession::defaultEndpoint() const
{
    QString outputName = DefaultAudioEndpoint;
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    if (m_audioOutput) {
        CAudioOutput::TAudioOutputPreference output = m_audioOutput->DefaultAudioOutput();
        outputName = qStringFromTAudioOutputPreference(output);
    }
#endif
    return outputName;
}

/*!
   Sets active end \a name as an Audio End point.
*/

void S60VideoPlayerSession::setActiveEndpoint(const QString& name)
{
    TRACE("S60VideoPlayerSession::setActiveEndpoint" << qtThisPtr() << "name" << name);

#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    CAudioOutput::TAudioOutputPreference output = CAudioOutput::ENoPreference;
    if (name == DefaultAudioEndpoint)
        output = CAudioOutput::ENoPreference;
    else if (name == QString("All"))
        output = CAudioOutput::EAll;
    else if (name == QString("None"))
        output = CAudioOutput::ENoOutput;
    else if (name == QString("Earphone"))
        output = CAudioOutput::EPrivate;
    else if (name == QString("Speaker"))
        output = CAudioOutput::EPublic;
    if (m_audioOutput) {
        TRAPD(err, m_audioOutput->SetAudioOutputL(output));
        setError(err);
    }
#endif
}

/*!
    The default Audio output has been changed.

    \a aAudioOutput Audio Output object.

    \a aNewDefault is CAudioOutput::TAudioOutputPreference.
*/

#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
void S60VideoPlayerSession::DefaultAudioOutputChanged( CAudioOutput& aAudioOutput,
                                        CAudioOutput::TAudioOutputPreference aNewDefault)
{
    TRACE("S60VideoPlayerSession::DefaultAudioOutputChanged" << qtThisPtr()
          << "newDefault" << aNewDefault);

    // Emit already implemented in setActiveEndpoint function
    Q_UNUSED(aAudioOutput)
    Q_UNUSED(aNewDefault)
}

/*!
 * \return  CAudioOutput::ENoOutput by converting it to QString.
*/

QString S60VideoPlayerSession::qStringFromTAudioOutputPreference(CAudioOutput::TAudioOutputPreference output) const
{
    if (output == CAudioOutput::ENoPreference)
        return QString("Default");
    else if (output == CAudioOutput::EAll)
        return QString("All");
    else if (output == CAudioOutput::ENoOutput)
        return QString("None");
    else if (output == CAudioOutput::EPrivate)
        return QString("Earphone");
    else if (output == CAudioOutput::EPublic)
        return QString("Speaker");
    return QString("Default");
}
#endif //HAS_AUDIOROUTING_IN_VIDEOPLAYER)

/*!
 * \return TRUE if video is Seekable else FALSE.
*/

bool S60VideoPlayerSession::getIsSeekable() const
{
    bool seekable = ETrue;
    int numberOfMetaDataEntries = 0;

    TRAPD(err, numberOfMetaDataEntries = m_player->NumberOfMetaDataEntriesL());
    if (err)
        return seekable;

    for (int i = 0; i < numberOfMetaDataEntries; i++) {
        CMMFMetaDataEntry *entry = NULL;
        TRAP(err, entry = m_player->MetaDataEntryL(i));

        if (err)
            return seekable;

        if (!entry->Name().Compare(KSeekable)) {
            if (!entry->Value().Compare(KFalse))
                seekable = EFalse;
            break;
        }
    }

    return seekable;
}
