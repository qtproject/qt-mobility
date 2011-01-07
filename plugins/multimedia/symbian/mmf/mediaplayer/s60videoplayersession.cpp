/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtGui/qwidget.h>
#include <QtCore/qtimer.h>
#include <QApplication>

#include <coecntrl.h>
#include <coemain.h>    // For CCoeEnv
#include <w32std.h>
#include <mmf/common/mmfcontrollerframeworkbase.h>

const QString DefaultAudioEndpoint = QLatin1String("Default");

S60VideoPlayerSession::S60VideoPlayerSession(QMediaService *service)
    : S60MediaPlayerSession(service)
    , m_wsSession(&CCoeEnv::Static()->WsSession())
    , m_screenDevice(CCoeEnv::Static()->ScreenDevice())
    , m_service(service)
    , m_player(0)
#ifndef MMF_VIDEO_SURFACES_SUPPORTED
    , m_dsaActive(false)
    , m_dsaStopped(false)
#endif
    , m_videoOutput(0)
    , m_displayWindow(0)
    , m_audioEndpoint(DefaultAudioEndpoint)
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    , m_audioOutput(0)
#endif
    , m_pendingChanges(0)
{
#ifdef MMF_VIDEO_SURFACES_SUPPORTED
    QT_TRAP_THROWING(m_player = CVideoPlayerUtility2::NewL(
        *this,
        0,
        EMdaPriorityPreferenceNone
        ));
    m_player->RegisterForVideoLoadingNotification(*this);
#else
    RWindow *window = 0;
    QRect rect;
    if (S60VideoOutputInterface *output = videoOutput()) {
        window = output->videoWindowHandle();
        rect = output->videoDisplayRect();
    } else {
        QWidget *widget = QApplication::activeWindow();
        if (!widget)
            widget = QApplication::allWidgets().at(0);
        Q_ASSERT(widget);
        WId wid = widget->effectiveWinId();
        if (!wid)
            wid = widget->winId();
        window = static_cast<RWindow *>(wid->DrawableWindow());
        rect = QRect(widget->mapToGlobal(widget->pos()), widget->size());
    }
    QT_TRAP_THROWING(m_player = CVideoPlayerUtility::NewL(
            *this,
            0,
            EMdaPriorityPreferenceNone,
            *m_wsSession,
            *m_screenDevice,
            *window,
            QRect2TRect(rect),
            QRect2TRect(rect)));
    m_dsaActive = true;
    m_player->RegisterForVideoLoadingNotification(*this);
#endif // MMF_VIDEO_SURFACES_SUPPORTED
}

S60VideoPlayerSession::~S60VideoPlayerSession()
{
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
#endif
    m_player->Close();
    delete m_player;
}

void S60VideoPlayerSession::doLoadL(const TDesC &path)
{
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    // m_audioOutput needs to be reinitialized after MapcInitComplete
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
    m_audioOutput = NULL;
#endif
    m_player->OpenFileL(path);
}

void S60VideoPlayerSession::doLoadUrlL(const TDesC &path)
{
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    // m_audioOutput needs to be reinitialized after MapcInitComplete
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
    m_audioOutput = NULL;
#endif
    m_player->OpenUrlL(path);
}

int S60VideoPlayerSession::doGetBufferStatusL() const
{
    int progress = 0;
    m_player->GetVideoLoadingProgressL(progress);
    return progress;
}

qint64 S60VideoPlayerSession::doGetDurationL() const
{
    return m_player->DurationL().Int64() / qint64(1000);
}

void S60VideoPlayerSession::setVideoRenderer(QObject *videoOutput)
{
    if (videoOutput != m_videoOutput) {
        if (m_videoOutput) {
            disconnect(m_videoOutput);
            m_videoOutput->disconnect(this);
        }
        if (videoOutput) {
            connect(videoOutput, SIGNAL(windowHandleChanged()), this, SLOT(windowHandleChanged()));
            connect(videoOutput, SIGNAL(displayRectChanged()), this, SLOT(displayRectChanged()));
            connect(videoOutput, SIGNAL(aspectRatioChanged()), this, SLOT(aspectRatioChanged()));
#ifndef MMF_VIDEO_SURFACES_SUPPORTED
            connect(videoOutput, SIGNAL(beginVideoWindowNativePaint()), this, SLOT(suspendDirectScreenAccess()));
            connect(videoOutput, SIGNAL(endVideoWindowNativePaint()), this, SLOT(resumeDirectScreenAccess()));
#endif
            connect(this, SIGNAL(stateChanged(QMediaPlayer::State)), videoOutput, SLOT(videoStateChanged(QMediaPlayer::State)));
        }
        m_videoOutput = videoOutput;
        windowHandleChanged();
    }
}

S60VideoOutputInterface *S60VideoPlayerSession::videoOutput() const
{
    S60VideoOutputInterface *result = 0;
    if (S60VideoWidgetControl *control = qobject_cast<S60VideoWidgetControl *>(m_videoOutput))
        result = control;
    if (S60VideoOverlay *control = qobject_cast<S60VideoOverlay *>(m_videoOutput))
        result = control;
    return result;
}

void S60VideoPlayerSession::applyPendingChanges(bool force)
{
    if (   force
        || QMediaPlayer::LoadedMedia == mediaStatus()
        || QMediaPlayer::StalledMedia == mediaStatus()
        || QMediaPlayer::BufferingMedia == mediaStatus()
        || QMediaPlayer::BufferedMedia == mediaStatus()
        || QMediaPlayer::EndOfMedia == mediaStatus()) {
        int error = KErrNone;
        S60VideoOutputInterface *output = videoOutput();
        RWindow *const window = output ? output->videoWindowHandle() : 0;

        const QRect qrect = output ? output->videoDisplayRect() : QRect();
        const TRect extentRect = QRect2TRect(qrect);
        TRect clipRect = extentRect;
#ifdef MMF_VIDEO_SURFACES_SUPPORTED
        const TRect windowRect = window ? TRect(TPoint(), window->Size()) : TRect();
        clipRect.Intersection(windowRect);
        if (m_pendingChanges & WindowHandle) {
            if (m_displayWindow) {
                m_player->RemoveDisplayWindow(*m_displayWindow);
                m_displayWindow = 0;
            }
            if (window) {
                window->SetBackgroundColor(TRgb(0, 0, 0, 255));
                TRAP(error, m_player->AddDisplayWindowL(*m_wsSession, *m_screenDevice,
                                                        *window, extentRect, clipRect));
                if (KErrNone == error)
                    m_displayWindow = window;
            }
            m_pendingChanges = ScaleFactors;
        }
        if (KErrNone == error && (m_pendingChanges & DisplayRect) && m_displayWindow) {
            TRAP(error, m_player->SetVideoExtentL(*m_displayWindow, extentRect));
            if (KErrNone == error)
                TRAP(error, m_player->SetWindowClipRectL(*m_displayWindow, clipRect));
            m_pendingChanges ^= DisplayRect;
            m_pendingChanges |= ScaleFactors;
        }
#else
        if (m_pendingChanges & WindowHandle || m_pendingChanges & DisplayRect) {
            if (window)
               TRAP(error, m_player->SetDisplayWindowL(*m_wsSession, *m_screenDevice,
                                                       *window, extentRect, clipRect));
            m_dsaActive = (KErrNone == error);
            m_dsaStopped = false;
            m_pendingChanges = ScaleFactors;
        }

#endif // MMF_VIDEO_SURFACES_SUPPORTED
        if (KErrNone == error && (m_pendingChanges & ScaleFactors) && m_displayWindow && output) {
            QSize scaled = m_originalSize;
            if (output->videoAspectRatio() == Qt::IgnoreAspectRatio)
                scaled.scale(output->videoDisplayRect().size(), Qt::IgnoreAspectRatio);
            else if(output->videoAspectRatio() == Qt::KeepAspectRatio)
                scaled.scale(output->videoDisplayRect().size(), Qt::KeepAspectRatio);
            const qreal width = qreal(scaled.width()) / qreal(m_originalSize.width()) * qreal(100);
            const qreal height = qreal(scaled.height()) / qreal(m_originalSize.height()) * qreal(100);
#ifdef MMF_VIDEO_SURFACES_SUPPORTED
            TRAP(error, m_player->SetScaleFactorL(*m_displayWindow, width, height));
#else
            static const TBool antialias = ETrue;
            TRAP(error, m_player->SetScaleFactorL(width, height, antialias));
#endif // MMF_VIDEO_SURFACES_SUPPORTED
            m_pendingChanges ^= ScaleFactors;
        }
        setError(error);
    }
}

bool S60VideoPlayerSession::isVideoAvailable()
{
#ifdef PRE_S60_50_PLATFORM
    return true; // this is not supported in pre 5th platforms
#else
    if (m_player) {
        bool videoAvailable = true;
        TRAPD(err, videoAvailable = m_player->VideoEnabledL());
        setError(err);
        return videoAvailable;
    } else {
        return false;
    }
#endif
}

bool S60VideoPlayerSession::isAudioAvailable()
{
    if (m_player) {
        bool audioAvailable = false;
        TRAPD(err, audioAvailable = m_player->AudioEnabledL());
        setError(err);
        return audioAvailable;
    } else {
        return false;
    }
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

void S60VideoPlayerSession::doClose()
{
    m_player->Close();
}

qint64 S60VideoPlayerSession::doGetPositionL() const
{
    return m_player->PositionL().Int64() / qint64(1000);
}

void S60VideoPlayerSession::doSetPositionL(qint64 microSeconds)
{
    m_player->SetPositionL(TTimeIntervalMicroSeconds(microSeconds));
}

void S60VideoPlayerSession::doSetVolumeL(int volume)
{
    m_player->SetVolumeL(volume * m_player->MaxVolume() / 100);
}

void S60VideoPlayerSession::MvpuoOpenComplete(TInt aError)
{
    setError(aError);
    if (KErrNone == aError)
        m_player->Prepare();
}

void S60VideoPlayerSession::MvpuoPrepareComplete(TInt aError)
{
    TInt error = aError;
    if (KErrNone == error) {
        TSize originalSize;
        TRAP(error, m_player->VideoFrameSizeL(originalSize));
        if (KErrNone == error) {
            m_originalSize = QSize(originalSize.iWidth, originalSize.iHeight);
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
            applyPendingChanges(true); // force apply even though state is not Loaded
            if (KErrNone == this->error()) // applyPendingChanges() can call setError()
                loaded();
        }
    } else {
        setError(error);
    }
}

void S60VideoPlayerSession::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
{
    Q_UNUSED(aFrame);
    Q_UNUSED(aError);
}

void S60VideoPlayerSession::MvpuoPlayComplete(TInt aError)
{
    endOfMedia();
    setError(aError);
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
        metaDataEntries().insert(TDesC2QString(entry->Name()), TDesC2QString(entry->Value()));
        delete entry;
    }
    emit metaDataChanged();
}

void S60VideoPlayerSession::windowHandleChanged()
{
    m_pendingChanges |= WindowHandle;
    applyPendingChanges();
}

void S60VideoPlayerSession::displayRectChanged()
{
    m_pendingChanges |= DisplayRect;
    applyPendingChanges();
}

void S60VideoPlayerSession::aspectRatioChanged()
{
    m_pendingChanges |= ScaleFactors;
    applyPendingChanges();
}

#ifndef MMF_VIDEO_SURFACES_SUPPORTED
void S60VideoPlayerSession::suspendDirectScreenAccess()
{
    m_dsaStopped = stopDirectScreenAccess();
}

void S60VideoPlayerSession::resumeDirectScreenAccess()
{
    if (!m_dsaStopped)
        return;
    startDirectScreenAccess();
    m_dsaStopped = false;
}

void S60VideoPlayerSession::startDirectScreenAccess()
{
    if (m_dsaActive)
        return;
    TRAPD(err, m_player->StartDirectScreenAccessL());
    if (err == KErrNone)
        m_dsaActive = true;
    setError(err);
}

bool S60VideoPlayerSession::stopDirectScreenAccess()
{
    if (!m_dsaActive)
        return false;
    TRAPD(err, m_player->StopDirectScreenAccessL());
    if (err == KErrNone)
        m_dsaActive = false;
    setError(err);
    return true;
}
#endif

void S60VideoPlayerSession::MvloLoadingStarted()
{
    buffering();
}

void S60VideoPlayerSession::MvloLoadingComplete()
{
    buffered();
}

void S60VideoPlayerSession::doSetAudioEndpoint(const QString& audioEndpoint)
{
    m_audioEndpoint = audioEndpoint;
}

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

void S60VideoPlayerSession::setActiveEndpoint(const QString& name)
{
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
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
void S60VideoPlayerSession::DefaultAudioOutputChanged( CAudioOutput& aAudioOutput,
                                        CAudioOutput::TAudioOutputPreference aNewDefault)
{
    // Emit already implemented in setActiveEndpoint function
    Q_UNUSED(aAudioOutput)
    Q_UNUSED(aNewDefault)
}

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
