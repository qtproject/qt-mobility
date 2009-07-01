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

#ifndef QMEDIAPLAYER_H
#define QMEDIAPLAYER_H

#include "qabstractmediaobject.h"

#include "qmediaserviceprovider.h"
#include "qmediasource.h"


class QMediaPlayerService;
class QMediaPlaylist;

extern QMediaPlayerService *createMediaPlayerService(QMediaServiceProvider *provider = defaultServiceProvider("mediaplayer"));


class QMediaPlayerPrivate;
class Q_MEDIA_EXPORT QMediaPlayer : public QAbstractMediaObject
{
    Q_OBJECT

    Q_PROPERTY(QMediaPlaylist* mediaPlaylist READ mediaPlaylist WRITE setMediaPlaylist NOTIFY mediaPlaylistChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(qint64 position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutingChanged)
    Q_PROPERTY(bool buffering READ isBuffering NOTIFY bufferingChanged)
    Q_PROPERTY(int bufferStatus READ bufferStatus NOTIFY bufferStatusChanged)
    Q_PROPERTY(bool video READ isVideoAvailable NOTIFY videoAvailablityChanged)
    // Q_PROPERTY(int playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChange)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    Q_ENUMS(State)

public:
    enum State { LoadingState, PlayingState, PausedState, StoppedState, SeekingState, EndOfStreamState };

    QMediaPlayer(QMediaPlayerService *service = createMediaPlayerService(), QObject *parent = 0);
    ~QMediaPlayer();

    State state() const;
    QMediaPlaylist* mediaPlaylist() const;

    qint64 duration() const;
    qint64 position() const;

    int volume() const;
    bool isMuted() const;

    bool isBuffering() const;
    int bufferStatus() const;

    bool isVideoAvailable() const;

    QAbstractMediaService* service() const;


public Q_SLOTS:
    void setMediaPlaylist(QMediaPlaylist *mediaPlaylist);

    void play();
    void pause();
    void stop();

    void setPosition(qint64 position);
    void setVolume(int volume);
    void setMuted(bool muted);

Q_SIGNALS:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void stateChanged(State newState);
    void volumeChanged(int volume);
    void mutingChanged(bool muted);
    void videoAvailabilityChanged(bool videoAvailable);
    void bufferingChanged(bool buffering);
    void bufferStatusChanged(int percentFilled);

private:
    Q_DISABLE_COPY(QMediaPlayer)
    Q_DECLARE_PRIVATE(QMediaPlayer)
};

#endif  // QMEDIAPLAYER_H
