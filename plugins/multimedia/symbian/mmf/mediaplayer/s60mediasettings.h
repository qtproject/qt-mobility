/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60MEDIASETTINGS_H
#define S60MEDIASETTINGS_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <qmediaplayer.h>

class S60MediaSettings : public QObject
{
public:
    S60MediaSettings(QObject *parent = 0);

    enum MediaType {
        Unknown,
        Video,
        Audio,
        Data
    };

    void setVolume(int volume);
    void setMuted(bool muted);
    void setPlaybackRate(qreal rate);
    void setMediaStatus(QMediaPlayer::MediaStatus status);
    void setAudioEndpoint(const QString &audioEndpoint);
    void setMediaType(MediaType type);
    void setVideoOutput(QObject *value);

    int volume() const;
    bool isMuted() const;
    qreal playbackRate() const;
    QMediaPlayer::MediaStatus mediaStatus() const;
    QString audioEndpoint() const;
    MediaType mediaType() const;
    QObject *videoOutput() const;

private:
    int m_volume;
    bool m_muted;
    qreal m_playbackRate;
    QMediaPlayer::MediaStatus m_mediaStatus;
    QString m_audioEndpoint;
    MediaType m_mediaType;
    QObject *m_videoOutput;
};

#endif // S60MEDIASETTINGS_H
