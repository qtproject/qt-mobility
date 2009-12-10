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

#include "qmlmediabase_p.h"

#include <QtCore/qcoreevent.h>

#include <qmediaplayercontrol.h>
#include <qmediaservice.h>
#include <qmediaserviceprovider.h>
#include <qmetadatacontrol.h>
#include <qmetadatacontrolmetaobject_p.h>

QTM_BEGIN_NAMESPACE

namespace
{
    class QmlMediaBasePlayerControl : public QMediaPlayerControl
    {
    public:
        QmlMediaBasePlayerControl(QObject *parent)
            : QMediaPlayerControl(parent)
        {
        }

        QMediaPlayer::State state() const { return QMediaPlayer::StoppedState; }
        QMediaPlayer::MediaStatus mediaStatus() const { return QMediaPlayer::NoMedia; }

        qint64 duration() const { return 0; }
        qint64 position() const { return 0; }
        void setPosition(qint64) {}
        int volume() const { return 100; }
        void setVolume(int) {}
        bool isMuted() const { return false; }
        void setMuted(bool) {}
        int bufferStatus() const { return 0; }
        bool isVideoAvailable() const { return false; }
        bool isSeekable() const { return false; }
        QPair<qint64, qint64> seekRange() const { return QPair<qint64, qint64>(); }
        qreal playbackRate() const { return 1; }
        void setPlaybackRate(qreal) {}
        QMediaContent media() const { return QMediaContent(); }
        const QIODevice *mediaStream() const { return 0; }
        void setMedia(const QMediaContent &, QIODevice *) {}

        void play() {}
        void pause() {}
        void stop() {}
    };
}

void QmlMediaBase::_q_stateChanged(QMediaPlayer::State state)
{
    if (QmlMedia::State(state) != m_state) {
        emit stateChanged(m_state = QmlMedia::State(state));
    }
}

void QmlMediaBase::_q_mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (QmlMedia::Status(status) != m_status)
        emit statusChanged(m_status = QmlMedia::Status(status));
}

void QmlMediaBase::_q_mediaChanged(const QMediaContent &media)
{
    emit sourceChanged(media.canonicalUri());
}

void QmlMediaBase::_q_durationChanged(qint64 duration)
{
    emit durationChanged(duration);
}

void QmlMediaBase::_q_positionChanged(qint64 position)
{
    qint64 duration = m_playerControl->duration();

    if (duration != 0)
        emit positionChanged(qreal(position) / qreal(duration));
    else
        emit positionChanged(0);
}

void QmlMediaBase::_q_volumeChanged(int volume)
{
    emit volumeChanged(qreal(volume) / 100);
}

void QmlMediaBase::_q_error(QMediaPlayer::Error error, const QString &errorString)
{
    m_error = QmlMedia::Error(error);
    m_errorString = errorString;

    emit errorChanged(m_error);
    emit errorStringChanged(m_errorString);
}

void QmlMediaBase::_q_metaDataChanged()
{
    m_metaObject->metaDataChanged();
}

QmlMediaBase::QmlMediaBase()
    : m_mediaService(0)
    , m_playerControl(0)
    , m_mediaProvider(0)
    , m_metaDataControl(0)
    , m_metaObject(0)
    , m_state(QmlMedia::Stopped)
    , m_status(QmlMedia::NoMedia)
    , m_error(QmlMedia::NoError)
{
}

QmlMediaBase::~QmlMediaBase()
{
    delete m_metaObject;

    if (m_mediaProvider)
        m_mediaProvider->releaseService(m_mediaService);
}

void QmlMediaBase::setObject(QObject *object)
{
    if ((m_mediaProvider = QMediaServiceProvider::defaultServiceProvider())) {
        if ((m_mediaService = m_mediaProvider->requestService(Q_MEDIASERVICE_MEDIAPLAYER))) {
            m_playerControl = qobject_cast<QMediaPlayerControl *>(
                    m_mediaService->control(QMediaPlayerControl_iid));
            m_metaDataControl = qobject_cast<QMetaDataControl *>(
                    m_mediaService->control(QMetaDataControl_iid));
        }
    }

    if (m_playerControl) {
        QObject::connect(m_playerControl, SIGNAL(mediaChanged(QMediaContent)),
                object, SLOT(_q_mediaChanged(QMediaContent)));
        QObject::connect(m_playerControl, SIGNAL(stateChanged(QMediaPlayer::State)),
                object, SLOT(_q_stateChanged(QMediaPlayer::State)));
        QObject::connect(m_playerControl, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
                object, SLOT(_q_mediaStatusChanged(QMediaPlayer::MediaStatus)));
        QObject::connect(m_playerControl, SIGNAL(durationChanged(qint64)),
                object, SLOT(_q_durationChanged(qint64)));
        QObject::connect(m_playerControl, SIGNAL(positionChanged(qint64)),
                object, SLOT(_q_positionChanged(qint64)));
        QObject::connect(m_playerControl, SIGNAL(volumeChanged(int)),
                object, SLOT(_q_volumeChanged(int)));
        QObject::connect(m_playerControl, SIGNAL(mutedChanged(bool)),
                object, SIGNAL(mutedChanged(bool)));
    } else {
        m_playerControl = new QmlMediaBasePlayerControl(object);
    }

    if (m_metaDataControl) {
        m_metaObject = new QMetaDataControlMetaObject(m_metaDataControl, object);

        QObject::connect(m_metaDataControl, SIGNAL(metaDataChanged()),
                object, SLOT(_q_metaDataChanged()));
    }
}

QUrl QmlMediaBase::source() const
{
    return m_playerControl->media().canonicalUri();
}

void QmlMediaBase::setSource(const QUrl &url)
{
    m_playerControl->setMedia(QMediaContent(url), 0);
}

QmlMedia::State QmlMediaBase::state() const
{
    return m_state;
}

void QmlMediaBase::setState(QmlMedia::State state)
{
    switch (state) {
    case QmlMedia::Stopped:
        m_playerControl->stop();
        break;
    case QmlMedia::Playing:
        m_playerControl->play();
        break;
    case QmlMedia::Paused:
        m_playerControl->pause();
        break;
    default:
        break;
    }
}

QmlMedia::Status QmlMediaBase::status() const
{
    return m_status;
}

qreal QmlMediaBase::duration() const
{
    return m_playerControl->duration();
}

qreal QmlMediaBase::position() const
{
    qint64 duration = m_playerControl->duration();

    if (duration != 0)
        return qreal(m_playerControl->position()) / qreal(duration);
    else
        return 0;
}

void QmlMediaBase::setPosition(qreal position)
{
    qint64 duration = m_playerControl->duration();

    if (duration != 0)
        m_playerControl->setPosition(position * duration);
}

qreal QmlMediaBase::volume() const
{
    return qreal(m_playerControl->volume()) / 100;
}

void QmlMediaBase::setVolume(qreal volume)
{
    m_playerControl->setVolume(volume * 100);
}

bool QmlMediaBase::isMuted() const
{
    return m_playerControl->isMuted();
}

void QmlMediaBase::setMuted(bool muted)
{
    m_playerControl->setMuted(muted);
}

int QmlMediaBase::bufferStatus() const
{
    return m_playerControl->bufferStatus();
}

bool QmlMediaBase::isSeekable() const
{
    return m_playerControl->isSeekable();
}

qreal QmlMediaBase::playbackRate() const
{
    return m_playerControl->playbackRate();
}

void QmlMediaBase::setPlaybackRate(qreal rate)
{
    m_playerControl->setPlaybackRate(rate);
}

QmlMedia::Error QmlMediaBase::error() const
{
    return m_error;
}

QString QmlMediaBase::errorString() const
{
    return m_errorString;
}

QTM_END_NAMESPACE

