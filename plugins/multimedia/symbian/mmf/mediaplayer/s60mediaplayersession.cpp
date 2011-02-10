/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <mmf/common/mmferrors.h>
#include <qmediatimerange.h>

S60MediaPlayerSession::S60MediaPlayerSession(QObject *parent)
    : QObject(parent)
    , m_stream(false)
    , m_playbackRate(0)
    , m_muted(false)
    , m_volume(0)
    , m_state(QMediaPlayer::StoppedState)
    , m_mediaStatus(QMediaPlayer::UnknownMediaStatus)
    , m_progressTimer(new QTimer(this))
    , m_stalledTimer(new QTimer(this))
    , m_error(KErrNone)
    , m_play_requested(false)
    , m_seekable(true)
{    
    connect(m_progressTimer, SIGNAL(timeout()), this, SLOT(tick()));
    connect(m_stalledTimer, SIGNAL(timeout()), this, SLOT(stalled()));
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
    // Dont set symbian players volume until media loaded.
    // Leaves with KerrNotReady although documentation says otherwise.
    if (!m_muted && 
        (  mediaStatus() == QMediaPlayer::LoadedMedia 
        || mediaStatus() == QMediaPlayer::StalledMedia 
        || mediaStatus() == QMediaPlayer::BufferingMedia
        || mediaStatus() == QMediaPlayer::BufferedMedia
        || mediaStatus() == QMediaPlayer::EndOfMedia)) {
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
    return m_seekable;
}

void S60MediaPlayerSession::setMediaStatus(QMediaPlayer::MediaStatus status)
{
    if (m_mediaStatus == status)
        return;
    
    m_mediaStatus = status;
    
    emit mediaStatusChanged(m_mediaStatus);
    
    if (m_play_requested && m_mediaStatus == QMediaPlayer::LoadedMedia)
        play();
}

void S60MediaPlayerSession::setState(QMediaPlayer::State state)
{
    if (m_state == state)
        return;
    
    m_state = state;
    emit stateChanged(m_state);
}

QMediaPlayer::State S60MediaPlayerSession::state() const 
{ 
    return m_state; 
}

QMediaPlayer::MediaStatus S60MediaPlayerSession::mediaStatus() const 
{ 
    return m_mediaStatus; 
}

void S60MediaPlayerSession::load(QUrl url)
{
    setMediaStatus(QMediaPlayer::LoadingMedia);
    startStalledTimer();
    m_stream = (url.scheme() == "file")?false:true;
    m_UrlPath = url;
    TRAPD(err,
        if(m_stream)
            doLoadUrlL(QString2TPtrC(url.toString()));
        else
            doLoadL(QString2TPtrC(QDir::toNativeSeparators(url.toLocalFile()))));
    setError(err);
}

TBool S60MediaPlayerSession::isStreaming()
{
    return m_stream;
}

void S60MediaPlayerSession::play()
{
    if (state() == QMediaPlayer::PlayingState
        || mediaStatus() == QMediaPlayer::UnknownMediaStatus
        || mediaStatus() == QMediaPlayer::NoMedia
        || mediaStatus() == QMediaPlayer::InvalidMedia)
        return;

    if (mediaStatus() == QMediaPlayer::LoadingMedia ||
       (mediaStatus() == QMediaPlayer::StalledMedia &&
        state() == QMediaPlayer::StoppedState))
   {
        m_play_requested = true;
        return;
    }
    
    m_play_requested = false;
    setVolume(m_volume);
    setMuted(m_muted);
    setState(QMediaPlayer::PlayingState);
    startProgressTimer();
    doPlay();
}

void S60MediaPlayerSession::pause()
{
    if (mediaStatus() == QMediaPlayer::NoMedia ||
        mediaStatus() == QMediaPlayer::InvalidMedia)
        return;    
    
    setState(QMediaPlayer::PausedState);
    stopProgressTimer();    
    TRAP_IGNORE(doPauseL());
}

void S60MediaPlayerSession::stop()
{
    m_play_requested = false;
    setState(QMediaPlayer::StoppedState);
    if (mediaStatus() == QMediaPlayer::BufferingMedia ||
        mediaStatus() == QMediaPlayer::BufferedMedia)
        setMediaStatus(QMediaPlayer::LoadedMedia);
    if (mediaStatus() == QMediaPlayer::LoadingMedia)
        setMediaStatus(QMediaPlayer::UnknownMediaStatus);    
    stopProgressTimer();
    stopStalledTimer();
    doStop();
    emit positionChanged(0);
}

void S60MediaPlayerSession::reset()
{
    m_play_requested = false;    
    setError(KErrNone, QString(), true);
    stopProgressTimer();
    stopStalledTimer();
    doStop();
    doClose();
    setState(QMediaPlayer::StoppedState);
    setMediaStatus(QMediaPlayer::UnknownMediaStatus);
}

void S60MediaPlayerSession::setVideoRenderer(QObject *renderer)
{
    Q_UNUSED(renderer);
}

int S60MediaPlayerSession::bufferStatus()
{
    if(   mediaStatus() == QMediaPlayer::LoadingMedia
       || mediaStatus() == QMediaPlayer::UnknownMediaStatus
       || mediaStatus() == QMediaPlayer::NoMedia
       || mediaStatus() == QMediaPlayer::InvalidMedia)
        return 0;
    
    int progress = 0;
    TRAPD(err, progress = doGetBufferStatusL());
    // If buffer status query not supported by codec return 100
    // do not set error
    if(err == KErrNotSupported)
        return 100;
    
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

QVariant S60MediaPlayerSession::metaData(QtMultimediaKit::MetaData key) const
{
    return metaData(metaDataKeyAsString(key));
}

QList<QtMultimediaKit::MetaData> S60MediaPlayerSession::availableMetaData() const
{
    QList<QtMultimediaKit::MetaData> metaDataTags;
    if (isMetadataAvailable()) {
        for (int i = QtMultimediaKit::Title; i <= QtMultimediaKit::DeviceSettingDescription; i++) {
            QString metaDataItem = metaDataKeyAsString((QtMultimediaKit::MetaData)i);
            if (!metaDataItem.isEmpty()) {
                if (!metaData(metaDataItem).isNull()) {
                    metaDataTags.append((QtMultimediaKit::MetaData)i);
                }
            }
        }
    }
    return metaDataTags;
}

QStringList S60MediaPlayerSession::availableExtendedMetaData() const
{
    return m_metaDataMap.keys();
}

QString S60MediaPlayerSession::metaDataKeyAsString(QtMultimediaKit::MetaData key) const
{
    switch(key) {
    case QtMultimediaKit::Title: return "title";
    case QtMultimediaKit::AlbumArtist: return "artist";
    case QtMultimediaKit::Comment: return "comment";
    case QtMultimediaKit::Genre: return "genre";
    case QtMultimediaKit::Year: return "year";
    case QtMultimediaKit::Copyright: return "copyright";
    case QtMultimediaKit::AlbumTitle: return "album";
    case QtMultimediaKit::Composer: return "composer";
    case QtMultimediaKit::TrackNumber: return "albumtrack";
    case QtMultimediaKit::AudioBitRate: return "audiobitrate";
    case QtMultimediaKit::VideoBitRate: return "videobitrate";
    case QtMultimediaKit::Duration: return "duration";
    case QtMultimediaKit::MediaType: return "contenttype";
    case QtMultimediaKit::CoverArtImage: return "attachedpicture";
    case QtMultimediaKit::SubTitle: // TODO: Find the matching metadata keys
    case QtMultimediaKit::Description:
    case QtMultimediaKit::Category:
    case QtMultimediaKit::Date:
    case QtMultimediaKit::UserRating:
    case QtMultimediaKit::Keywords:
    case QtMultimediaKit::Language:
    case QtMultimediaKit::Publisher:
    case QtMultimediaKit::ParentalRating:
    case QtMultimediaKit::RatingOrganisation:
    case QtMultimediaKit::Size:
    case QtMultimediaKit::AudioCodec:
    case QtMultimediaKit::AverageLevel:
    case QtMultimediaKit::ChannelCount:
    case QtMultimediaKit::PeakValue:
    case QtMultimediaKit::SampleRate:
    case QtMultimediaKit::Author:
    case QtMultimediaKit::ContributingArtist:
    case QtMultimediaKit::Conductor:
    case QtMultimediaKit::Lyrics:
    case QtMultimediaKit::Mood:
    case QtMultimediaKit::TrackCount:
    case QtMultimediaKit::CoverArtUrlSmall:
    case QtMultimediaKit::CoverArtUrlLarge:
    case QtMultimediaKit::Resolution:
    case QtMultimediaKit::PixelAspectRatio:
    case QtMultimediaKit::VideoFrameRate:
    case QtMultimediaKit::VideoCodec:
    case QtMultimediaKit::PosterUrl:
    case QtMultimediaKit::ChapterNumber:
    case QtMultimediaKit::Director:
    case QtMultimediaKit::LeadPerformer:
    case QtMultimediaKit::Writer:
    case QtMultimediaKit::CameraManufacturer:
    case QtMultimediaKit::CameraModel:
    case QtMultimediaKit::Event:
    case QtMultimediaKit::Subject:
    default:
        break;
    }

    return QString();
}

void S60MediaPlayerSession::setMuted(bool muted)
{
    m_muted = muted;
    
    if(   m_mediaStatus == QMediaPlayer::LoadedMedia 
       || m_mediaStatus == QMediaPlayer::StalledMedia 
       || m_mediaStatus == QMediaPlayer::BufferingMedia
       || m_mediaStatus == QMediaPlayer::BufferedMedia
       || m_mediaStatus == QMediaPlayer::EndOfMedia) {
        TRAPD(err, doSetVolumeL((m_muted)?0:m_volume));
        setError(err);
    }
}

qint64 S60MediaPlayerSession::duration() const
{
    if(   mediaStatus() == QMediaPlayer::LoadingMedia
       || mediaStatus() == QMediaPlayer::UnknownMediaStatus
       || mediaStatus() == QMediaPlayer::NoMedia
       || mediaStatus() == QMediaPlayer::InvalidMedia)
        return -1;
    
    qint64 pos = 0;
    TRAP_IGNORE(pos = doGetDurationL());
    return pos;
}

qint64 S60MediaPlayerSession::position() const
{
    if(   mediaStatus() == QMediaPlayer::LoadingMedia
       || mediaStatus() == QMediaPlayer::UnknownMediaStatus
       || mediaStatus() == QMediaPlayer::NoMedia
       || mediaStatus() == QMediaPlayer::InvalidMedia)
        return 0;
    
    qint64 pos = 0;
    TRAP_IGNORE(pos = doGetPositionL());
    return pos;
}

void S60MediaPlayerSession::setPosition(qint64 pos)
{
    if (position() == pos)
        return;

    QMediaPlayer::State originalState = state();

    if (originalState == QMediaPlayer::PlayingState) 
        pause();

    TRAPD(err, doSetPositionL(pos * 1000));
    setError(err);

    if (err == KErrNotSupported) {
        m_seekable = false;
        emit seekableChanged(m_seekable);
    }

    if (originalState == QMediaPlayer::PlayingState)
        play();

    emit positionChanged(position());
}

void S60MediaPlayerSession::setAudioEndpoint(const QString& audioEndpoint)
{
    doSetAudioEndpoint(audioEndpoint);
}

void S60MediaPlayerSession::loaded()
{
    stopStalledTimer();
    if (m_error == KErrNone || m_error == KErrMMPartialPlayback) {
        setMediaStatus(QMediaPlayer::LoadedMedia);
        TRAPD(err, updateMetaDataEntriesL());
        setError(err);
        emit durationChanged(duration());
        emit videoAvailableChanged(isVideoAvailable());
        emit audioAvailableChanged(isAudioAvailable());
        emit mediaChanged();

        m_seekable = getIsSeekable();
    }
}

void S60MediaPlayerSession::endOfMedia()
{
    m_state = QMediaPlayer::StoppedState;
    setMediaStatus(QMediaPlayer::EndOfMedia);
    //there is a chance that user might have called play from EOF callback
    //if we are already in playing state, do not send state change callback
    if(m_state == QMediaPlayer::StoppedState)
        emit stateChanged(QMediaPlayer::StoppedState);
    emit positionChanged(0);
}

void S60MediaPlayerSession::buffering()
{
    startStalledTimer();
    setMediaStatus(QMediaPlayer::BufferingMedia);
}

void S60MediaPlayerSession::buffered()
{
    stopStalledTimer();
    setMediaStatus(QMediaPlayer::BufferedMedia);
}
void S60MediaPlayerSession::stalled()
{
    setMediaStatus(QMediaPlayer::StalledMedia);
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
        case KErrAbort:
        case KErrNotSupported:
        case KErrCorrupt:
        case KErrGeneral:
        case KErrArgument:
        case KErrPathNotFound:
        case KErrDied:
        case KErrServerTerminated:
        case KErrServerBusy:
        case KErrCompletion:  
        case KErrBadPower:    
            return QMediaPlayer::ResourceError;
        
        case KErrMMPartialPlayback:   
            return QMediaPlayer::FormatError;

        case KErrMMAudioDevice:
        case KErrMMVideoDevice:
        case KErrMMDecoder:
        case KErrUnknown:    
            return QMediaPlayer::ServiceMissingError;
            
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
        case KErrCouldNotConnect:
            return QMediaPlayer::NetworkError;

        case KErrNotReady:
        case KErrInUse:
        case KErrAccessDenied:
        case KErrLocked:
        case KErrMMDRMNotAuthorized:
        case KErrPermissionDenied:
        case KErrCancel:
        case KErrAlreadyExists:
            return QMediaPlayer::AccessDeniedError;

        case KErrNone:
        default:
            return QMediaPlayer::NoError;
    }
}

int S60MediaPlayerSession::error() const
{
    return m_error;
}

void S60MediaPlayerSession::setError(int error, const QString &errorString, bool forceReset)
{
    if( forceReset ) {
        m_error = KErrNone;
        emit this->error(QMediaPlayer::NoError, QString());
        return;
    }

    // If error does not change and m_error is reseted without forceReset flag
    if (error == m_error || 
        (m_error != KErrNone && error == KErrNone))
        return;
    
    m_error = error;
    QMediaPlayer::Error mediaError = fromSymbianErrorToMultimediaError(m_error);
    QString symbianError = QString(errorString);

    if (mediaError != QMediaPlayer::NoError) {
        // TODO: fix to user friendly string at some point
        // These error string are only dev usable
        symbianError.append("Symbian:");
        symbianError.append(QString::number(m_error));
    }
     
    emit this->error(mediaError, symbianError);
    
    switch(mediaError){
        case QMediaPlayer::ResourceError:
        case QMediaPlayer::NetworkError:
        case QMediaPlayer::AccessDeniedError:
        case QMediaPlayer::ServiceMissingError:
            m_play_requested = false;
            setMediaStatus(QMediaPlayer::InvalidMedia);
            stop();
            break;
    }
}

void S60MediaPlayerSession::tick()
{
    emit positionChanged(position());

    if (bufferStatus() < 100)
        emit bufferStatusChanged(bufferStatus());
}

void S60MediaPlayerSession::startProgressTimer()
{
    m_progressTimer->start(500);
}

void S60MediaPlayerSession::stopProgressTimer()
{
    m_progressTimer->stop();
}

void S60MediaPlayerSession::startStalledTimer()
{
    m_stalledTimer->start(30000);
}

void S60MediaPlayerSession::stopStalledTimer()
{
    m_stalledTimer->stop();
}

QString S60MediaPlayerSession::TDesC2QString(const TDesC& aDescriptor)
{
    return QString::fromUtf16(aDescriptor.Ptr(), aDescriptor.Length());
}

TPtrC S60MediaPlayerSession::QString2TPtrC( const QString& string )
{
	// Returned TPtrC is valid as long as the given parameter is valid and unmodified
    return TPtrC16(static_cast<const TUint16*>(string.utf16()), string.length());
}

QRect S60MediaPlayerSession::TRect2QRect(const TRect& tr)
{
    return QRect(tr.iTl.iX, tr.iTl.iY, tr.Width(), tr.Height());
}

TRect S60MediaPlayerSession::QRect2TRect(const QRect& qr)
{
    return TRect(TPoint(qr.left(), qr.top()), TSize(qr.width(), qr.height()));
}
