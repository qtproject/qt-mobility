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

#ifndef QMLAUDIO_P_H
#define QMLAUDIO_P_H

#include <qmlmediabase_p.h>

#include <QtCore/qbasictimer.h>
#include <QtDeclarative/qmlgraphicsitem.h>

class QTimerEvent;

QTM_BEGIN_NAMESPACE

class QmlAudio : public QObject, public QmlMediaBase, public QmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QmlMedia::State state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QmlMedia::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(qreal duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(qreal position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(int bufferStatus READ bufferStatus NOTIFY bufferStatusChanged)
    Q_PROPERTY(bool seekable READ isSeekable NOTIFY seekableChanged)
    Q_PROPERTY(qreal playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(QmlMedia::Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
public:
    QmlAudio(QObject *parent = 0);
    ~QmlAudio();

Q_SIGNALS:
    void sourceChanged(const QUrl &url);

    void stateChanged(QmlMedia::State state);
    void statusChanged(QmlMedia::Status status);

    void durationChanged(qreal duration);
    void positionChanged(qreal position);

    void volumeChanged(qreal volume);
    void mutedChanged(bool muted);

    void bufferStatusChanged(int percentage);

    void seekableChanged(bool seekable);
    void playbackRateChanged(qreal rate);

    void errorChanged(QmlMedia::Error error);
    void errorStringChanged(const QString &error);

private:
    Q_DISABLE_COPY(QmlAudio)
    Q_PRIVATE_SLOT(mediaBase(), void _q_stateChanged(QMediaPlayer::State))
    Q_PRIVATE_SLOT(mediaBase(), void _q_mediaStatusChanged(QMediaPlayer::MediaStatus))
    Q_PRIVATE_SLOT(mediaBase(), void _q_mediaChanged(const QMediaContent &))
    Q_PRIVATE_SLOT(mediaBase(), void _q_durationChanged(qint64))
    Q_PRIVATE_SLOT(mediaBase(), void _q_positionChanged(qint64))
    Q_PRIVATE_SLOT(mediaBase(), void _q_volumeChanged(int))
    Q_PRIVATE_SLOT(mediaBase(), void _q_error(QMediaPlayer::Error, const QString &))
    Q_PRIVATE_SLOT(mediaBase(), void _q_metaDataChanged())

    inline QmlMediaBase *mediaBase() { return this; }
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QmlAudio))

#endif
