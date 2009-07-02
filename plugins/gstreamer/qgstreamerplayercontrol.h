/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWMPPLAYERCONTROL_H
#define QWMPPLAYERCONTROL_H

#include <QtCore/qobject.h>

#include "qmediasource.h"
#include "qmediaplayercontrol.h"
#include "qmediaplayer.h"


class QMediaPlaylist;

/*
class QAbstractMediaControl : public QObject
{
    Q_OBJECT
    public:
    QAbstractMediaControl(QObject *parent = 0)
        :QObject(parent)
    {
    }

    virtual ~QAbstractMediaControl() {}
};

class  QMediaPlayerControl : public QAbstractMediaControl
{
    Q_OBJECT


    Q_PROPERTY(QMediaPlayerControl::State state READ state NOTIFY stateChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(qint64 position READ position WRITE seek NOTIFY positionChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedStateChanged)
    Q_PROPERTY(bool buffering READ isBuffering NOTIFY bufferingStateChanged)
    Q_PROPERTY(int bufferingProgress READ bufferingProgress NOTIFY bufferingProgressChanged)
    Q_PROPERTY(QMediaSource mediaSource READ currentMediaSource NOTIFY currentMediaChanged)
    Q_PROPERTY(int playlistPosition READ playlistPosition WRITE jump NOTIFY playlistPositionChanged)
    Q_PROPERTY(bool video READ isVideoAvailable NOTIFY videoAvailablityChanged)
public:
    enum State
    {
        Stopped,
        Playing,
        Paused,
        Finished,
        Error
    };

    QMediaPlayerControl(QObject *parent = 0);
    
    virtual State state() const = 0;

    virtual qint64 duration() const = 0;
    virtual qint64 position() const = 0;

    virtual QMediaPlaylist *playlist() const = 0;
    virtual bool setPlaylist(QMediaPlaylist *) = 0;
    
    virtual int playlistPosition() const = 0;
    virtual QMediaSource currentMediaSource() const = 0;

    virtual bool isBuffering() const = 0;
    virtual int bufferingProgress() const = 0;

    virtual int volume() const = 0;
    virtual bool isMuted() const = 0;

    virtual bool isVideoAvailable() const = 0;

public Q_SLOTS:
    virtual void jump(int playlistPosition) = 0;
    virtual void advance() = 0;
    virtual void back() = 0;

    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

    virtual void seek(qint64 pos) = 0;

    virtual void setVolume(int volume) = 0;
    virtual void setMuted(bool muted) = 0;

Q_SIGNALS:
    void stateChanged(QMediaPlayerControl::State state);
    void videoAvailablityChanged(bool available);

    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);

    void bufferingStateChanged(bool buffering);
    void bufferingProgressChanged(int percentFilled);

    void mutedStateChanged(bool);
    void volumeChanged(int);

    void currentMediaChanged(const QMediaSource &mediaSource);
    void playlistPositionChanged(int playlistPosition);
};
*/

class QGstreamerPlayerSession;
class QGstreamerPlayerService;
class QMediaPlaylistNavigator;

class QGstreamerPlayerControl : public QMediaPlayerControl
{
    Q_OBJECT
public:
    QGstreamerPlayerControl(QGstreamerPlayerService *service, QObject *parent = 0);
    ~QGstreamerPlayerControl();

    int state() const;

    qint64 position() const;
    qint64 duration() const;

    bool isBuffering() const;
    int bufferStatus() const;

    int volume() const;
    bool isMuted() const;

    int playlistPosition() const;
    QMediaSource currentMediaSource() const;

    QMediaPlaylist *mediaPlaylist() const;
    bool setMediaPlaylist(QMediaPlaylist *);

    bool isVideoAvailable() const;
    void setVideoOutput(QObject *output);

public Q_SLOTS:
    void setPlaylistPosition(int playlistPosition);
    void advance();
    void back();

    void setPosition(qint64 pos);

    void play();
    void pause();
    void stop();

    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void playlistPositionChanged(int position);
    void currentMediaChanged(const QMediaSource&);
    void stateChanged(int newState);
    void volumeChanged(int volume);
    void mutingChanged(bool muted);
    void videoAvailabilityChanged(bool videoAvailable);
    void bufferingChanged(bool buffering);
    void bufferStatusChanged(int percentFilled);

private slots:
    void play(const QMediaSource&);
    void updateState(QMediaPlayer::State state);

private:
    QGstreamerPlayerService *m_service;
    QGstreamerPlayerSession *m_session;
    QMediaPlaylistNavigator *m_navigator;
};

#endif
