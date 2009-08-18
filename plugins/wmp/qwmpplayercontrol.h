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

#include "qmediaplayercontrol.h"

#include <wmp.h>

class QWmpEvents;
class QWmpPlaylist;

class QWmpPlayerControl : public QMediaPlayerControl
{
    Q_OBJECT
public:
    QWmpPlayerControl(
            IWMPCore3 *player, QWmpEvents *events, QObject *parent = 0);
    ~QWmpPlayerControl();

    QMediaPlayer::State state() const;
    QMediaPlayer::MediaStatus mediaStatus() const;

    QMediaPlaylist* mediaPlaylist() const;
    bool setMediaPlaylist(QMediaPlaylist *playlist);

    qint64 duration() const;

    qint64 position() const;
    void setPosition(qint64 position);

    int playlistPosition() const;
    void setPlaylistPosition(int position);

    int volume() const;
    void setVolume(int volume);

    bool isMuted() const;
    void setMuted(bool muted);

    int bufferStatus() const;

    bool isVideoAvailable() const;
    void setVideoAvailable(bool available);

    float playbackRate() const;
    void setPlaybackRate(float rate);

    bool isSeekable() const;

    void play();
    void pause();
    void stop();

    void advance();
    void back();

    QUrl url() const;
    void setUrl(const QUrl &url);

    using QMediaPlayerControl::positionChanged;

private Q_SLOTS:
    void proxiedItemsInserted(int start, int end);
    void proxiedItemsRemoved(int start, int end);
    void proxiedItemsChanged(int start, int end);

    void bufferingEvent(VARIANT_BOOL buffering);
    void currentItemChangeEvent(IDispatch *dispatch);
    void mediaChangeEvent(IDispatch *dispatch);
    void positionChangeEvent(double from, double to);
    void playStateChangeEvent(long state);
    void openPlaylistChangeEvent(IDispatch *dispatch);

private:
    IWMPCore3 *m_player;
    IWMPControls *m_controls;
    IWMPSettings *m_settings;
    IWMPNetwork *m_network;
    QWmpPlaylist *m_playlistSource;
    QMediaPlaylist *m_playlist;
    QMediaPlaylist *m_proxiedPlaylist;
    IWMPPlaylist *m_proxyPlaylist;
    QMediaPlayer::State m_state;
    QMediaPlayer::MediaStatus m_status;
    qint64 m_duration;
    bool m_buffering;
    bool m_videoAvailable;
};

#endif
