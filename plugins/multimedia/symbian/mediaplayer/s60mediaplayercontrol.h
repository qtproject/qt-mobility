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

#ifndef S60MEDIAPLAYERCONTROL_H
#define S60MEDIAPLAYERCONTROL_H

#include <QtCore/qobject.h>

#include <QMediaPlayerControl>
#include <QMediaPlayer>

#include "ms60mediaplayerresolver.h"
#include <QtCore/qdebug.h>
QTM_BEGIN_NAMESPACE
class QMediaPlaylist;
class QMediaPlaylistNavigator;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class S60MediaPlayerSession;
class S60MediaPlayerService;


class S60MediaSettings
{

public:
    S60MediaSettings() 
        : m_volume(0)
        , m_muted(false)
        , m_playbackRate(-1)
    {
    }
    
    void setVolume(int volume) { m_volume = volume; }
    void setMuted(bool muted) { m_muted = muted; }
    void setPlaybackRate(int rate) { m_playbackRate = rate; }
    
    int volume() const { return m_volume; }
    bool isMuted() const { return m_muted; }
    qreal playbackRate() const { return m_playbackRate; }
    
private:
    int m_volume;
    bool m_muted;
    qreal m_playbackRate;
};

class S60MediaPlayerControl : public QMediaPlayerControl
{
    Q_OBJECT

public:
    S60MediaPlayerControl(MS60MediaPlayerResolver& mediaPlayerResolver, QObject *parent = 0);
    ~S60MediaPlayerControl();

    QMediaPlayer::State state() const;
    QMediaPlayer::MediaStatus mediaStatus() const;

    qint64 position() const;
    qint64 duration() const;

    int bufferStatus() const;

    int volume() const;
    bool isMuted() const;

    bool isVideoAvailable() const;
    void setVideoOutput(QObject *output);

    bool isSeekable() const;
    QPair<qint64, qint64> seekRange() const;
	
    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);

    QMediaContent media() const;
    const QIODevice *mediaStream() const;
    void setMedia(const QMediaContent&, QIODevice *);

    const S60MediaSettings& mediaControlSettings() const;
    
public Q_SLOTS:
    void setPosition(qint64 pos);

    void play();
    void pause();
    void stop();

    void setVolume(int volume);
    void setMuted(bool muted);
    
private: 
    S60MediaPlayerSession* currentPlayerSession();
    
private:
    MS60MediaPlayerResolver &m_mediaPlayerResolver;
    S60MediaPlayerSession *m_session;
    QMediaContent m_currentResource; 
    QIODevice *m_stream;
    S60MediaSettings m_mediaSettings;
};

#endif
