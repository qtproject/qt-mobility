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

#include <QObject>
#include <QUrl>
#include <QSize>
#include <multimedia/qmediaplayer.h>
#include "s60mediaplayersession.h"
#include <MdaAudioSamplePlayer.h>  

class S60AudioPlayerSession : public S60MediaPlayerSession, public MMdaAudioPlayerCallback
{
    Q_OBJECT

public:
    S60AudioPlayerSession(QObject *parent);
    ~S60AudioPlayerSession();

    qint64 duration() const;
    qint64 position() const;

    bool isBuffering() const;
    int bufferingProgress() const;

    int volume() const;
    bool isMuted() const;

    bool isVideoAvailable() const;

    bool isSeekable() const;

    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);
    
    bool isMetadataAvailable(); 
    QVariant metaData(QtMedia::MetaData key);

private: // From MMdaAudioPlayerCallback
    void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    void MapcPlayComplete(TInt aError);
    
public slots:
    void play();
    void pause();
    void stop();
    void seek(qint64 pos);
    void setVolume(int volume);
    void setMuted(bool muted);

private: 
    void getNativeHandles();

private:
    void setMediaStatus(QMediaPlayer::MediaStatus);
    
private:
    CMdaAudioPlayerUtility *m_player;
    int m_numberOfMetaDataEntries;
};

#endif
