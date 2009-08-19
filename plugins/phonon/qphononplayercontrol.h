/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWMPPLAYERCONTROL_H
#define QWMPPLAYERCONTROL_H

#include <QtCore/qobject.h>

#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>

#include "qmediaplayercontrol.h"
#include "qmediaplayer.h"


class QMediaPlaylist;

class QPhononPlayerSession;
class QPhononPlayerService;
class QMediaPlaylistNavigator;


class QPhononPlayerControl : public QMediaPlayerControl
{
    Q_OBJECT
    Q_PROPERTY(qint64 position READ position WRITE setPosition NOTIFY positionChanged)
public:
    QPhononPlayerControl(Phonon::MediaObject *session, QObject *parent = 0);
    ~QPhononPlayerControl();

    QMediaPlayer::State state() const;
    QMediaPlayer::MediaStatus mediaStatus() const;

    QMediaResourceList currentResources() const;
    void setCurrentResources(const QMediaResourceList &resources);

    qint64 position() const;
    qint64 duration() const;

    int bufferStatus() const;

    int volume() const;
    bool isMuted() const;

    int playlistPosition() const;

    bool isVideoAvailable() const;

    bool isSeekable() const;

    float playbackRate() const;
    void setPlaybackRate(float rate);

public Q_SLOTS:
    void setPosition(qint64 pos);

    void play();
    void pause();
    void stop();

    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    void positionChanged(qint64 position);

private slots:
    void updateState(Phonon::State newState, Phonon::State oldState);
    void updateVolume();

private:
    Phonon::MediaObject *m_session;
    Phonon::AudioOutput *m_audioOutput;
    QMediaPlayer::State m_state;
    QMediaPlayer::MediaStatus m_mediaStatus;
    QMediaResourceList m_resources;
};

#endif
