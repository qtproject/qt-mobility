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

#include "s60mediaplayersession.h"

#include <QtCore/qdebug.h>
#include <QtCore/qdir.h>
#include <QtCore/qvariant.h>
#include <QtCore/qtimer.h>
#include <QtCore/private/qcore_symbian_p.h>
#include <mmf/common/mmferrors.h>
#include <qmediatimerange.h>

S60MediaPlayerSession::S60MediaPlayerSession(QObject *parent)
    : QObject(parent)
    , m_playbackRate(0)
    , m_muted(false)
    , m_volume(0)
    , m_state(QMediaPlayer::StoppedState)
    , m_mediaStatus(QMediaPlayer::NoMedia)
    , m_timer(new QTimer(this))
    , m_error(KErrNone)
    , m_localMediaFile(true)
    , m_play_requested(false)
{    
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

S60MediaPlayerSession::~S60MediaPlayerSession()
{
}

int S60MediaPlayerSession::volume() const
{
    return m_volume;
}

void S60MediaPlayerSession::setVolume(int volume)
{
    if (m_volume == volume)
        return;
    
    m_volume = volume;

    if (mediaStatus() == QMediaPlayer::LoadedMedia && !isMuted()) {
        TRAPD(err, doSetVolumeL(m_volume));
        setError(err);
    }
}

bool S60MediaPlayerSession::isMuted() const
{
    return m_muted;
}

bool S60MediaPlayerSession::isSeekable() const
{
    if (m_metaDataMap.isEmpty())
        return true;
    return m_metaDataMap.value("seekable").toBool();
}

void S60MediaPlayerSession::setMediaStatus(QMediaPlayer::MediaStatus status)
{
    if (m_mediaStatus == status)
        return;
    
    m_mediaStatus = status;
    emit mediaStatusChanged(status);
    if (m_play_requested)
        play();
}

void S60MediaPlayerSession::setState(QMediaPlayer::State state)
{
    if (m_state == state)
        return;
    
    m_state = state;
    emit stateChanged(state);
}

QMediaPlayer::State S60MediaPlayerSession::state() const 
{ 
    return m_state; 
}

QMediaPlayer::MediaStatus S60MediaPlayerSession::mediaStatus() const 
{ 
    return m_mediaStatus; 
}

void S60MediaPlayerSession::load(const QUrl &url)
{
    m_localMediaFile = true;
    doStop();
    setMediaStatus(QMediaPlayer::LoadingMedia);
    TRAPD(err, doLoadL(qt_QString2TPtrC(QDir::toNativeSeparators(url.toLocalFile()))));
    setError(err);
}

void S60MediaPlayerSession::loadUrl(const QUrl &url)
{
    m_localMediaFile = false;
    doStop();
    setMediaStatus(QMediaPlayer::LoadingMedia);
    TRAPD(err, doLoadUrlL(qt_QString2TPtrC(url.toString())));
    setError(err);
}

void S60MediaPlayerSession::play()
{
    if (state() == QMediaPlayer::PlayingState)
        return;
    
    if (mediaStatus() != QMediaPlayer::LoadedMedia) {
        m_play_requested = true;
        return;
    }
    
    m_play_requested = false;
    setState(QMediaPlayer::PlayingState);
    m_timer->start(1000);
    doPlay();
}

void S60MediaPlayerSession::pause()
{
    m_timer->stop();
    setState(QMediaPlayer::PausedState);
    TRAPD(err, doPauseL());
    setError(err);
}

void S60MediaPlayerSession::stop()
{
    setState(QMediaPlayer::StoppedState);
    m_timer->stop();
    doStop();
}

void S60MediaPlayerSession::setVideoRenderer(QObject *renderer)
{
    Q_UNUSED(renderer);   
}

int S60MediaPlayerSession::mediaLoadingProgress()
{
    int progress = 0;
    TRAPD(err, progress = doGetMediaLoadingProgressL());
    setError(err);
    return progress;
}

bool S60MediaPlayerSession::isMetadataAvailable() const
{
    return !m_metaDataMap.isEmpty();    
}

QVariant S60MediaPlayerSession::metaData(const QString &key) const
{
    return m_metaDataMap.value(key);    
}

QMap<QString, QVariant> S60MediaPlayerSession::availableMetaData() const
{
    return m_metaDataMap;
}

qreal S60MediaPlayerSession::playbackRate() const
{
    return m_playbackRate;
}

void S60MediaPlayerSession::setMuted(bool muted)
{
    if (m_muted == muted)
        return;
    
    m_muted = muted;
    emit mutingChanged(m_muted);
    
    if (isMuted()) {
        TRAPD(err, doSetVolumeL(0));
        setError(err);
    } else {
        setVolume(volume());
    }
}

void S60MediaPlayerSession::setPlaybackRate(qreal rate)
{
    if (m_playbackRate == rate)
        return;
    
    m_playbackRate = rate;
    emit playbackRateChanged(m_playbackRate);
    //None of symbian players supports this.
}

qint64 S60MediaPlayerSession::duration() const
{
    qint64 pos = 0;
    //Cannot seterror since const, error ignored
    TRAP_IGNORE(pos = doGetDurationL());
    return pos;
}

qint64 S60MediaPlayerSession::position() const
{
    qint64 pos = 0;
    //Cannot seterror since const, error ignored
    TRAP_IGNORE(pos = doGetPositionL());
    return pos;
}

void S60MediaPlayerSession::setPosition(qint64 pos)
{
    if (position() == pos)
        return;
    
    if (state() == QMediaPlayer::PlayingState) 
        pause();

    TRAPD(err, doSetPositionL(pos * 1000));
    setError(err);

    if (state() == QMediaPlayer::PausedState)
        play();

    emit positionChanged(position());
}

void S60MediaPlayerSession::initComplete()
{
    if (m_error == KErrNone) {
        setMediaStatus(QMediaPlayer::LoadedMedia);
        TRAPD(err, updateMetaDataEntriesL());
        setError(err);
        setVolume(volume());
        emit durationChanged(duration());
    } else {
        setMediaStatus(QMediaPlayer::NoMedia);
    }
}

void S60MediaPlayerSession::playComplete()
{
    setMediaStatus(QMediaPlayer::EndOfMedia);
    setState(QMediaPlayer::StoppedState);
    emit positionChanged(0);
}

QMap<QString, QVariant>& S60MediaPlayerSession::metaDataEntries()
{
    return m_metaDataMap;
}

QMediaPlayer::Error S60MediaPlayerSession::fromSymbianErrorToMultimediaError(int error)
{
    switch(error) {
        case KErrNoMemory:
        case KErrNotFound:
        case KErrBadHandle:
        case KErrMMAudioDevice:
        case KErrMMVideoDevice:
            return QMediaPlayer::ResourceError;
            
        case KErrMMDecoder:
        case KErrNotSupported:
        case KErrCorrupt:
            return QMediaPlayer::FormatError;
            
        case KErrMMNotEnoughBandwidth:
        case KErrMMSocketServiceNotFound:
        case KErrMMNetworkRead:
        case KErrMMNetworkWrite:
        case KErrMMServerSocket:
        case KErrMMServerNotSupported:
        case KErrMMUDPReceive:
        case KErrMMInvalidProtocol:
        case KErrMMInvalidURL:
        case KErrMMMulticast:
        case KErrMMProxyServer:
        case KErrMMProxyServerNotSupported:
        case KErrMMProxyServerConnect:
            return QMediaPlayer::NetworkError;
            
        case KErrNotReady:
        case KErrInUse:
        case KErrAccessDenied:
        case KErrLocked:
        case KErrMMDRMNotAuthorized:
        case KErrPermissionDenied:
            return QMediaPlayer::AccessDeniedError;
            
        case KErrMMPartialPlayback:            
        default:
            return QMediaPlayer::NoError;
    }
}

void S60MediaPlayerSession::setError(int error, const QString &errorString)
{
    if (error == KErrNone || error == KErrMMPartialPlayback)
        return;
    
    m_error = error;
    QMediaPlayer::Error mediaError = fromSymbianErrorToMultimediaError(m_error);
    
    // TODO: fix to user friendly string at some point
    // These error string are only dev usable
    QString symbianError = QString(errorString); 
    symbianError.append("From Symbian:");
    symbianError.append(QString(m_error));
    emit this->error(mediaError, symbianError);
    
    switch(mediaError){
        case QMediaPlayer::FormatError:
            setMediaStatus(QMediaPlayer::InvalidMedia);
            break;
        case QMediaPlayer::ResourceError:
        case QMediaPlayer::NetworkError:
        case QMediaPlayer::AccessDeniedError:
        case QMediaPlayer::ServiceMissingError:
            setMediaStatus(QMediaPlayer::NoMedia);
            break;
        default:
            break;
    }
}

void S60MediaPlayerSession::tick()
{
    emit positionChanged(position());
    
    if (mediaFileLocal() && mediaLoadingProgress() != 100)
    	emit bufferStatusChanged(mediaLoadingProgress());
}

bool S60MediaPlayerSession::mediaFileLocal() const
{
    return m_localMediaFile; 
}
void S60MediaPlayerSession::setMediaFileLocal(bool localMediaFile)
{
    if (m_localMediaFile == localMediaFile)
        return;
    
    m_localMediaFile = localMediaFile;
}
