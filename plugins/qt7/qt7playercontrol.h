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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT7PLAYERCONTROL_H
#define QT7PLAYERCONTROL_H

#include <qmediaplayercontrol.h>
#include <qmediaresource.h>

class QMediaPlaylist;
class QMediaPlaylistNavigator;
class Qt7Widget;
class Qt7Movie;
class QTimer;


class Qt7PlayerControlPrivate;
class Qt7PlayerControl : public QMediaPlayerControl
{
    Q_OBJECT

public:
    Qt7PlayerControl(QObject *parent);
    ~Qt7PlayerControl();

    QMediaPlayer::State state() const;

    QMediaPlaylist* mediaPlaylist() const;
    bool setMediaPlaylist(QMediaPlaylist *mediaPlaylist);

    qint64 duration() const;

    qint64 position() const;
    void setPosition(qint64);

    int playlistPosition() const;
    void setPlaylistPosition(int position);

    int volume() const;
    void setVolume(int volume);

    bool isMuted() const;
    void setMuted(bool muted);

    bool isBuffering() const;

    int bufferStatus() const;

    bool isVideoAvailable() const;

    bool isSeekable() const;

    float playbackRate() const;
    void setPlaybackRate(float rate);

    void play();
    void pause();
    void stop();

    void advance();
    void back();

    Qt7Movie* movie() const;

private slots:
    void setSource(QMediaResourceList const &resources);
    void update();

private:
    Qt7PlayerControlPrivate* d;

    QMediaPlaylist  *playlist;
    QMediaPlaylistNavigator *navigator;
    int playlistPos;
    QTimer* updateTimer;
};

#endif  // QT7PLAYERCONTROL_H

