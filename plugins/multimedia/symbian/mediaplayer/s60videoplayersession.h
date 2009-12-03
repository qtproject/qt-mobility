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

#ifndef S60VIDEOPLAYERSESSION_H
#define S60VIDEOPLAYERSESSION_H

#include <QObject>
#include <QUrl>
#include <QSize>
#include <QMediaPlayer>
#include "s60mediaplayercontrol.h"
#include "s60videowidget.h"
#include <VideoPlayer.h>
#include "s60mediaplayersession.h"

class S60VideoPlayerSession : public S60MediaPlayerSession, public MVideoPlayerUtilityObserver
{
    Q_OBJECT

public:
    S60VideoPlayerSession(QObject *parent);
    ~S60VideoPlayerSession();

    qint64 duration() const;
    qint64 position() const;

    void setVideoRenderer(QObject *renderer);
    bool isVideoAvailable() const;

    bool isSeekable() const;

    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);
    
    void load(const QUrl &url);
    void play();
    void pause();
    void stop();
    void setPosition(qint64 pos);
    void setVolume(int volume);
    void setMuted(bool muted);

private: 
    void nativeHandles();
    void updateMetaDataEntries();
    
private: // From MVideoPlayerUtilityObserver
    void MvpuoOpenComplete(TInt aError);
    void MvpuoPrepareComplete(TInt aError);
    void MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError);
    void MvpuoPlayComplete(TInt aError);
    void MvpuoEvent(const TMMFEvent &aEvent);

    CVideoPlayerUtility* m_player;
    S60VideoWidgetControl* m_videoWidgetControl;
    QWidget *m_dummyWidget;
    RWsSession* m_wsSession;
    CWsScreenDevice* m_screenDevice;
    RWindowBase* m_window;
    TRect m_windowRect;
    TRect m_clipRect;
    
    CCoeControl* control; 
};

#endif
