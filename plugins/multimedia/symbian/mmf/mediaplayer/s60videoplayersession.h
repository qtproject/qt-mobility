/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60VIDEOPLAYERSESSION_H
#define S60VIDEOPLAYERSESSION_H

#include "s60mediaplayersession.h"
#include "s60mediaplayeraudioendpointselector.h"
#include "s60medianetworkaccesscontrol.h"
#include "s60videodisplay.h"

#ifdef VIDEOOUTPUT_GRAPHICS_SURFACES
#include <videoplayer2.h>
#include <graphics/surface.h>
#include <surfaceeventhandler.h>
#else
#include <videoplayer.h>
#endif

#include <QtCore/QCoreApplication>

#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
#include <AudioOutput.h>
#include <MAudioOutputObserver.h>
#endif

class QTimer;
class S60MediaNetworkAccessControl;
class S60NativeWindow;
class S60VideoDisplay;

// Helper classes to pass Symbian events from WServ to the S60VideoPlayerSession
// so it can control video player on certain events if required

class ApplicationFocusObserver
{
public:
    virtual void applicationGainedFocus() = 0;
    virtual void applicationLostFocus() = 0;
};

class S60VideoPlayerEventHandler : public QObject
{
public:
    static S60VideoPlayerEventHandler *instance();
    static bool filterEvent(void *message, long *result);
    void addApplicationFocusObserver(ApplicationFocusObserver *observer);
    void removeApplicationFocusObserver(ApplicationFocusObserver *observer);
private:
    S60VideoPlayerEventHandler();
    ~S60VideoPlayerEventHandler();
private:
    static S60VideoPlayerEventHandler *m_instance;
    static QList<ApplicationFocusObserver *> m_applicationFocusObservers;
    static QCoreApplication::EventFilter m_eventFilter;
};

class S60VideoPlayerSession : public S60MediaPlayerSession
                            , public MVideoPlayerUtilityObserver
                            , public MVideoLoadingObserver
#ifdef VIDEOOUTPUT_GRAPHICS_SURFACES
                            , public MMMFSurfaceEventHandler
#endif
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
                            , public MAudioOutputObserver
#endif // HAS_AUDIOROUTING_IN_VIDEOPLAYER
                            , public ApplicationFocusObserver
{
    Q_OBJECT
public:
    S60VideoPlayerSession(QMediaService *service, S60MediaNetworkAccessControl *object);
    ~S60VideoPlayerSession();

    // S60MediaPlayerSession
    bool isVideoAvailable();
    bool isAudioAvailable();
    void setVideoRenderer(QObject *renderer);

    // MVideoLoadingObserver
    void MvloLoadingStarted();
    void MvloLoadingComplete();
    void setPlaybackRate(qreal rate);
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    // MAudioOutputObserver
    void DefaultAudioOutputChanged(CAudioOutput& aAudioOutput,
                                   CAudioOutput::TAudioOutputPreference aNewDefault);
#endif

    // S60MediaPlayerAudioEndpointSelector
    QString activeEndpoint() const;
    QString defaultEndpoint() const;

    // ApplicationFocusObserver
    void applicationGainedFocus();
    void applicationLostFocus();

signals:
    void nativeSizeChanged(QSize size);
#ifdef VIDEOOUTPUT_GRAPHICS_SURFACES
    void nativeSurfaceChanged(TSurfaceId surface);
#endif

public Q_SLOTS:
    void setActiveEndpoint(const QString &name);

signals:
    void accessPointChanged(int);

protected:
    // S60MediaPlayerSession
    void doLoadL(const TDesC &path);
    void doLoadUrlL(const TDesC &path);
    void doPlay();
    void doStop();
    void doClose();
    void doPauseL();
    void doSetVolumeL(int volume);
    qint64 doGetPositionL() const;
    void doSetPositionL(qint64 microSeconds);
    void updateMetaDataEntriesL();
    int doGetBufferStatusL() const;
    qint64 doGetDurationL() const;
    void doSetAudioEndpoint(const QString &audioEndpoint);
    bool getIsSeekable() const;

private slots:
    void windowHandleChanged();
    void displayRectChanged();
    void aspectRatioChanged();
    void rotationChanged();
#ifndef VIDEOOUTPUT_GRAPHICS_SURFACES
    void suspendDirectScreenAccess();
    void resumeDirectScreenAccess();
#endif

private:
    void applyPendingChanges(bool force = false);
#ifndef VIDEOOUTPUT_GRAPHICS_SURFACES
    void startDirectScreenAccess();
    bool stopDirectScreenAccess();
#endif
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    QString qStringFromTAudioOutputPreference(CAudioOutput::TAudioOutputPreference output) const;
#endif

    // MVideoPlayerUtilityObserver
    void MvpuoOpenComplete(TInt aError);
    void MvpuoPrepareComplete(TInt aError);
    void MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError);
    void MvpuoPlayComplete(TInt aError);
    void MvpuoEvent(const TMMFEvent &aEvent);

#ifdef VIDEOOUTPUT_GRAPHICS_SURFACES
    // MMMFSurfaceEventHandler
    void MmsehSurfaceCreated(TInt aDisplayId, const TSurfaceId& aId,
                             const TRect& aCropRect,
                             TVideoAspectRatio aAspectRatio);
    void MmsehSurfaceParametersChanged(const TSurfaceId& aId,
                                       const TRect& aCropRect,
                                       TVideoAspectRatio aAspectRatio);
    void MmsehRemoveSurface(const TSurfaceId& aId);
#endif

private:
    int m_accessPointId;
    S60MediaNetworkAccessControl *m_networkAccessControl;
    RWsSession *const m_wsSession;
    CWsScreenDevice *const m_screenDevice;
    QMediaService *const m_service;
#ifdef VIDEOOUTPUT_GRAPHICS_SURFACES
    CVideoPlayerUtility2 *m_player;
    TSurfaceId m_nativeSurface;
    S60NativeWindow *m_dummyWindow;
    bool m_bufferingWaitingSurface;
#else
    CVideoPlayerUtility *m_player;
    bool m_dsaActive;
    bool m_dsaStopped;
#endif // VIDEOOUTPUT_GRAPHICS_SURFACES
    QObject *m_videoOutputControl;
    S60VideoDisplay *m_videoOutputDisplay;
    RWindow *m_displayWindow;
    QSize m_nativeSize;
#ifdef HTTP_COOKIES_ENABLED
    TMMFMessageDestinationPckg m_destinationPckg;
#endif
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    CAudioOutput *m_audioOutput;
#endif
    QString m_audioEndpoint;
    enum Parameter {
        WindowHandle = 0x1,
        DisplayRect  = 0x2,
        ScaleFactors = 0x4,
        Rotation     = 0x8
    };
    QFlags<Parameter> m_pendingChanges;
    bool m_backendInitiatedPause;
};

#endif

