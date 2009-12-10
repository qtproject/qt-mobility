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

#ifndef QMLMEDIABASE_P_H
#define QMLMEDIABASE_P_H

#include <qmlmedia_p.h>

#include <QtCore/qbasictimer.h>

QTM_BEGIN_NAMESPACE

class QMediaPlayerControl;
class QMediaService;
class QMediaServiceProvider;
class QMetaDataControl;
class QMetaDataControlMetaObject;

class QmlMediaBase
{
public:
    QmlMediaBase();
    virtual ~QmlMediaBase();

    QUrl source() const;
    void setSource(const QUrl &url);

    QmlMedia::State state() const;
    void setState(QmlMedia::State state);

    QmlMedia::Status status() const;

    qreal duration() const;

    qreal position() const;
    void setPosition(qreal position);

    qreal volume() const;
    void setVolume(qreal volume);

    bool isMuted() const;
    void setMuted(bool muted);

    int bufferStatus() const;

    bool isSeekable() const;

    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);

    QmlMedia::Error error() const;
    QString errorString() const;

    void _q_stateChanged(QMediaPlayer::State state);
    void _q_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void _q_mediaChanged(const QMediaContent &media);
    void _q_durationChanged(qint64 duration);
    void _q_positionChanged(qint64 position);
    void _q_volumeChanged(int volume);
    void _q_error(QMediaPlayer::Error error, const QString &errorString);

    void _q_metaDataChanged();

protected:
    void setObject(QObject *object);

    virtual void sourceChanged(const QUrl &url) = 0;

    virtual void stateChanged(QmlMedia::State state) = 0;
    virtual void statusChanged(QmlMedia::Status status) = 0;

    virtual void durationChanged(qreal duration) = 0;
    virtual void positionChanged(qreal position) = 0;

    virtual void volumeChanged(qreal volume) = 0;
    virtual void mutedChanged(bool muted) = 0;

    virtual void bufferStatusChanged(int percentage) = 0;

    virtual void seekableChanged(bool seekable) = 0;
    virtual void playbackRateChanged(qreal rate) = 0;

    virtual void errorChanged(QmlMedia::Error error) = 0;
    virtual void errorStringChanged(const QString &error) = 0;


    QMediaService *m_mediaService;
    QMediaPlayerControl *m_playerControl;

private:
    QMediaServiceProvider *m_mediaProvider;
    QMetaDataControl *m_metaDataControl;
    QMetaDataControlMetaObject *m_metaObject;

    QmlMedia::State m_state;
    QmlMedia::Status m_status;
    QmlMedia::Error m_error;
    QString m_errorString;
};

QTM_END_NAMESPACE

#endif
