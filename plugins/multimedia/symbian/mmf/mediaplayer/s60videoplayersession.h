/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60VIDEOPLAYERSESSION_H
#define S60VIDEOPLAYERSESSION_H

#include "s60mediaplayersession.h"
#include "s60mediaplayeraudioendpointselector.h"
#ifdef MMF_VIDEO_SURFACES_SUPPORTED
#include <videoplayer2.h>
#else
#include <videoplayer.h>
#endif // MMF_VIDEO_SURFACES_SUPPORTED
#include <QtGui/qwidget.h>
#include <qvideowidget.h>

#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
#include <AudioOutput.h>
#include <MAudioOutputObserver.h>
#endif
class QTimer;

class S60VideoPlayerSession : public S60MediaPlayerSession,
                              public MVideoPlayerUtilityObserver,
                              public MVideoLoadingObserver
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
                              , public MAudioOutputObserver
#endif //HAS_AUDIOROUTING_IN_VIDEOPLAYER
{
    Q_OBJECT
public:
    S60VideoPlayerSession(QMediaService *service);
    ~S60VideoPlayerSession();

    //From S60MediaPlayerSession
    bool isVideoAvailable() const;
    bool isAudioAvailable() const;
    void setVideoRenderer(QObject *renderer);

    //From MVideoLoadingObserver
    void MvloLoadingStarted();
    void MvloLoadingComplete();

#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    // From MAudioOutputObserver
    void DefaultAudioOutputChanged(CAudioOutput& aAudioOutput,
        CAudioOutput::TAudioOutputPreference aNewDefault);
#endif //HAS_AUDIOROUTING_IN_VIDEOPLAYER
public:
    // From S60MediaPlayerAudioEndpointSelector
    QString activeEndpoint() const;
    QString defaultEndpoint() const;
public Q_SLOTS:
    void setActiveEndpoint(const QString& name);

protected:
    //From S60MediaPlayerSession
    void doLoadL(const TDesC &path);
    void doLoadUrlL(const TDesC &path);
    void doPlay();
    void doStop();
    void doPauseL();
    void doSetVolumeL(int volume);
    qint64 doGetPositionL() const;
    void doSetPositionL(qint64 microSeconds);
    void updateMetaDataEntriesL();
    int doGetBufferStatusL() const;
    qint64 doGetDurationL() const;
    void doSetAudioEndpoint(const QString& audioEndpoint);

private slots: 
    void resetVideoDisplay();
    void suspendDirectScreenAccess();
    void resumeDirectScreenAccess();
    
private: 
    bool resetNativeHandles();
    QPair<qreal, qreal> scaleFactor();
    void startDirectScreenAccess();
    bool stopDirectScreenAccess();
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    QString qStringFromTAudioOutputPreference(CAudioOutput::TAudioOutputPreference output) const;
#endif
    
    // From MVideoPlayerUtilityObserver
    void MvpuoOpenComplete(TInt aError);
    void MvpuoPrepareComplete(TInt aError);
    void MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError);
    void MvpuoPlayComplete(TInt aError);
    void MvpuoEvent(const TMMFEvent &aEvent);

private:
    // Qwn
#ifdef MMF_VIDEO_SURFACES_SUPPORTED
    CVideoPlayerUtility2 *m_player;
#else
    CVideoPlayerUtility *m_player;
#endif // MMF_VIDEO_SURFACES_SUPPORTED
    TRect m_rect;
    //QVideoOutputControl::Output m_output;
    QObject *m_videoOutput;
    WId m_windowId;
    bool m_dsaActive;
    bool m_dsaStopped;
    
    //Reference
    RWsSession &m_wsSession;
    CWsScreenDevice &m_screenDevice;
    RWindowBase *m_window;
    RWindow *m_displayWindow;
    QMediaService &m_service;
    Qt::AspectRatioMode m_aspectRatioMode;
    QSize m_originalSize;
#ifdef HAS_AUDIOROUTING_IN_VIDEOPLAYER
    CAudioOutput *m_audioOutput;
#endif
    QString m_audioEndpoint;
};

#endif
