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

#ifndef S60MEDIAPLAYERSESSION_H
#define S60MEDIAPLAYERSESSION_H

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qpair.h>
#include <QMediaPlayer>
#include <e32cmn.h> // for TDesC
#include "s60mediaplayerservice.h"

QTM_BEGIN_NAMESPACE
class QMediaTimeRange;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class QTimer;

class S60MediaPlayerSession : public QObject
{
    Q_OBJECT

public:
    S60MediaPlayerSession(QObject *parent);
    virtual ~S60MediaPlayerSession();

    QMediaPlayer::State state() const;
    QMediaPlayer::MediaStatus mediaStatus() const;
    
    int volume() const;
    bool isMuted() const;
    
    qreal playbackRate() const;   
    
    bool isMetadataAvailable() const; 
    QVariant metaData(const QString &key) const;
    QMap<QString, QVariant> availableMetaData() const;
    
    bool isSeekable() const;
    virtual QMediaTimeRange availablePlaybackRange() const;

    virtual qint64 duration() const = 0;
    virtual qint64 position() const = 0;
    virtual bool isVideoAvailable() const = 0;
    
    virtual void load(const QUrl &url);
    virtual void loadUrl(const QUrl &url);
    virtual void play();
    virtual void pause();
    virtual void stop();
    virtual void setVolume(int volume);
    virtual void setPosition(qint64 pos);
    virtual void setPlaybackRate(qreal rate);
    virtual void setMuted(bool muted);
    virtual void setVideoRenderer(QObject *renderer);
    
    virtual bool isUrl() const;
    virtual void setIsUrl(bool isUlr);
    
protected:    
    virtual void doLoad(const TDesC &path) = 0;
    virtual void doLoadUrl(const TDesC &path) = 0;
    virtual void doPlay() = 0;
    virtual void doStop() = 0;
    virtual void doPause() = 0;
    virtual void doSetVolume(int volume) = 0;
    virtual void doSetPlaybackRate(qreal rate) = 0;
    virtual void doSetPosition(qint64 microSeconds) = 0;
    virtual void updateMetaDataEntries() = 0;
    virtual int mediaLoadingProgress() const = 0;

protected:
    void setError(int error,  const QString &errorString = QString());
    void initComplete();
    void playComplete();
    void setMediaStatus(QMediaPlayer::MediaStatus);
    void setState(QMediaPlayer::State state);
    QMap<QString, QVariant>& metaDataEntries();
    
protected slots:
    void tick();
    bool startTimer();
    void stopTimer();
    
signals:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void stateChanged(QMediaPlayer::State state);
    void mediaStatusChanged(QMediaPlayer::MediaStatus mediaStatus);
    void volumeChanged(int volume);
    void mutingChanged(bool muted);
    void videoAvailableChanged(bool videoAvailable);
    void bufferStatusChanged(int percentFilled);
    void seekableChanged(bool);     
    void availablePlaybackRangesChanged(const QMediaTimeRange&);
    void metaDataChanged();
    void playbackRateChanged(qreal rate);
    void error(int error, const QString &errorString);

private:
    QMap<QString, QVariant> m_metaDataMap;
    qreal m_playbackRate;
    bool m_muted;
    int m_volume;
    QMediaPlayer::State m_state;
    QMediaPlayer::MediaStatus m_mediaStatus;
    QTimer *m_timer;
    int m_error;    
    bool m_isUrl;
};

#endif
