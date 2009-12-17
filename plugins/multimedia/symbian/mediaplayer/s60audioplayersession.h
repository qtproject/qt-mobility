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

#ifndef S60AUDIOPLAYERSESSION_H
#define S60AUDIOPLAYERSESSION_H

#include "s60mediaplayersession.h"

#ifdef S60_DRM_SUPPORTED
#include <drmaudiosampleplayer.h>
typedef CDrmPlayerUtility CAudioPlayer;
typedef MDrmAudioPlayerCallback MAudioPlayerObserver;
#else
#include <mdaaudiosampleplayer.h>  
typedef CMdaAudioPlayerUtility CAudioPlayer;
typedef MMdaAudioPlayerCallback MAudioPlayerObserver;
#endif

class S60AudioPlayerSession : public S60MediaPlayerSession, public CBase, public MAudioPlayerObserver
#ifdef S60_DRM_SUPPORTED
      , public MAudioLoadingObserver
#endif
{
    Q_OBJECT

public:
    S60AudioPlayerSession(QObject *parent);
    ~S60AudioPlayerSession();
    
    qint64 duration() const;
    qint64 position() const;
    bool isVideoAvailable() const;
    
    int mediaLoadingProgress() const { return 0; }
    
protected:
    void doLoad(const TDesC &path);
    void doLoadUrl(const TDesC &path) {};
    void doPlay();
    void doStop();
    void doPause();
    void doSetVolume(int volume);
    void doSetPlaybackRate(qreal rate);
    void doSetPosition(qint64 microSeconds);
    void updateMetaDataEntries();
    
private:
#ifdef S60_DRM_SUPPORTED    
    // From MMdaAudioPlayerCallback
    void MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    void MdapcPlayComplete(TInt aError);
#else
    // From MDrmAudioPlayerCallback
    void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    void MapcPlayComplete(TInt aError);
#endif
    
#if defined(S60_DRM_SUPPORTED) &&  defined(__S60_50__) 
    // From MAudioLoadingObserver
    void MaloLoadingStarted() {};
    void MaloLoadingComplete() {};
#endif
    
private:
    CAudioPlayer *m_player;
};

#endif
