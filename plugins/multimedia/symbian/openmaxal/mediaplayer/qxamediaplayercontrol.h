/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QXAMEDIAPLAYERCONTROL_H
#define QXAMEDIAPLAYERCONTROL_H

#include "qmediaplayercontrol.h"
#include "qmediaplayer.h"

QT_USE_NAMESPACE

class QXAPlaySession;

class QXAMediaPlayerControl : public QMediaPlayerControl
{
    Q_OBJECT
public:
    QXAMediaPlayerControl(QXAPlaySession *session, QObject *parent = 0);
    ~QXAMediaPlayerControl();

    QMediaPlayer::State state() const;
    QMediaPlayer::MediaStatus mediaStatus() const;

    qint64 duration() const;

    qint64 position() const;
    void setPosition(qint64 position);

    int volume() const;
    void setVolume(int volume);

    bool isMuted() const;
    void setMuted(bool muted);

    int bufferStatus() const;

    bool isAudioAvailable() const;
    bool isVideoAvailable() const;

    bool isSeekable() const;

    QMediaTimeRange availablePlaybackRanges() const;

    float playbackRate() const;
    void setPlaybackRate(float rate);

    QMediaContent media() const;
    const QIODevice *mediaStream() const;
    void setMedia(const QMediaContent&, QIODevice *);

    void play();
    void pause();
    void stop();


private:
    QXAPlaySession *mSession;
    QIODevice *mStream;
};

#endif /* QXAMEDIAPLAYERCONTROL_H */
