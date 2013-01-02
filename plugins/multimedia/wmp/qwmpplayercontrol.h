/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWMPPLAYERCONTROL_H
#define QWMPPLAYERCONTROL_H

#include <qmediaplayercontrol.h>

#include <wmp.h>

class QWmpEvents;
class QWmpPlaylist;

QT_USE_NAMESPACE
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

    bool isAudioAvailable() const;
    void setAudioAvailable(bool available);

    bool isVideoAvailable() const;
    void setVideoAvailable(bool available);

    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);

    bool isSeekable() const;
    QMediaTimeRange availablePlaybackRanges() const;

    void play();
    void pause();
    void stop();

    QMediaContent media() const;
    const QIODevice *mediaStream() const;
    void setMedia(const QMediaContent &content, QIODevice *stream);

    QUrl url() const;
    void setUrl(const QUrl &url);

    bool event(QEvent *event);

    using QMediaPlayerControl::positionChanged;

private Q_SLOTS:

    void bufferingEvent(VARIANT_BOOL buffering);
    void currentItemChangeEvent(IDispatch *dispatch);
    void mediaChangeEvent(IDispatch *dispatch);
    void positionChangeEvent(double from, double to);
    void playStateChangeEvent(long state);

private:
    enum Change
    {
        StateChanged = 0x01,
        StatusChanged = 0x02,
        PositionChanged = 0x04,
        DurationChanged = 0x08
    };

    void scheduleUpdate(int change);

    IWMPCore3 *m_player;
    IWMPControls *m_controls;
    IWMPSettings *m_settings;
    IWMPNetwork *m_network;
    QMediaPlayer::State m_state;
    QMediaPlayer::MediaStatus m_status;
    int m_changes;
    bool m_buffering;
    bool m_audioAvailable;
    bool m_videoAvailable;
};

#endif
