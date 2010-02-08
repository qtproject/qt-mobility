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

#include "qmlaudio_p.h"

#include "qmediaplayercontrol.h"

QML_DEFINE_TYPE(Qt,4,6,Audio,QTM_PREPEND_NAMESPACE(QmlAudio));

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass Audio QmlAudio
    \brief The Audio element allows you to add audio playback to a scene.

    \qml
    Audio { source: "audio/song.mp3" }
    \endqml

    \sa Video
*/

/*!
    \internal
    \class QmlAudio
    \brief The QmlAudio class provides an audio item that you can add to a QmlView.
*/

void QmlAudio::_q_error(QMediaPlayer::Error errorCode, const QString &errorString)
{
    m_error = errorCode;
    m_errorString = errorString;

    emit error(Error(errorCode), errorString);
    emit errorChanged();
}


QmlAudio::QmlAudio(QObject *parent)
    : QObject(parent)
{
    setObject(this);
}

QmlAudio::~QmlAudio()
{
    shutdown();
}

/*!
    \qmlmethod Audio::play()

    Starts playback of the media.

    Sets the \l playing property to true, and the \l paused property to false.
*/

void QmlAudio::play()
{
    m_playerControl->play();
}

/*!
    \qmlmethod Audio::pause()

    Pauses playback of the media.

    Sets the \l playing and \l paused properties to true.
*/

void QmlAudio::pause()
{
    m_playerControl->pause();
}

/*!
    \qmlmethod Audio::stop()

    Stops playback of the media.

    Sets the \l playing and \l paused properties to false.
*/

void QmlAudio::stop()
{
    m_playerControl->stop();
}

/*!
    \qmlproperty url Audio::source

    This property holds the source URL of the media.
*/

/*!
    \qmlproperty bool Audio::playing

    This property holds whether the media is playing.

    Defaults to false, and can be set to true to start playback.
*/

/*!
    \qmlproperty bool Audio::paused

    This property holds whether the media is paused.

    Defaults to false, and can be set to true to pause playback.
*/

/*!
    \qmlsignal Audio::onStarted()

    This handler is called when playback is started.
*/

/*!
    \qmlsignal Audio::onResumed()

    This handler is called when playback is resumed from the paused state.
*/

/*!
    \qmlsignal Audio::onPaused()

    This handler is called when playback is paused.
*/

/*!
    \qmlsignal Audio::onStopped()

    This handler is called when playback is stopped.
*/

/*!
    \qmlproperty enum Audio::status

    This property holds the status of media loading. It can be one of:

    \list
    \o NoMedia - no media has been set.
    \o Loading - the media is currently being loaded.
    \o Loaded - the media has been loaded.
    \o Buffering - the media is buffering data.
    \o Stalled - playback has been interrupted while the media is buffering data.
    \o Buffered - the media has buffered data.
    \o EndOfMedia - the media has played to the end.
    \o InvalidMedia - the media cannot be played.
    \o UnknownStatus - the status of the media is unknown.
    \endlist
*/

QmlAudio::Status QmlAudio::status() const
{
    return Status(m_status);
}

/*!
    \qmlsignal Audio::onLoaded()

    This handler is called when the media source has been loaded.
*/

/*!
    \qmlsignal Audio::onBuffering()

    This handler is called when the media  starts buffering.
*/

/*!
    \qmlsignal Audio::onStalled()

    This handler is called when playback has stalled while the media buffers.
*/

/*!
    \qmlsignal Audio::onBuffered()

    This handler is called when the media has finished buffering.
*/

/*!
    \qmlsignal Audio::onEndOfMedia()

    This handler is called when playback stops because end of the media has been reached.
*/
/*!
    \qmlproperty int Audio::duration

    This property holds the duration of the media in milliseconds.

    If the media doesn't have a fixed duration (a live stream for example) this will be 0.
*/

/*!
    \qmlproperty int Audio::position

    This property holds the current playback position in milliseconds.

    If the \l seekable property is true, this property can be set to seek to a new position.
*/

/*!
    \qmlproperty qreal Audio::volume

    This property holds the volume of the audio output, from 0.0 (silent) to 1.0 (maximum volume).
*/

/*!
    \qmlproperty bool Audio::muted

    This property holds whether the audio output is muted.
*/

/*!
    \qmlproperty qreal Audio::bufferProgress

    This property holds how much of the data buffer is currently filled, from 0.0 (empty) to 1.0
    (full).
*/

/*!
    \qmlproperty bool Audio::seekable

    This property holds whether position of the audio can be changed.

    If true; setting a \l position value will cause playback to seek to the new position.
*/

/*!
    \qmlproperty qreal Audio::playbackRate

    This property holds the rate at which audio is played at as a multiple of the normal rate.
*/

/*!
    \qmlproperty enum Audio::error

    This property holds the error state of the audio.  It can be one of:

    \list
    \o NoError - there is no current error.
    \o ResourceError - the audio cannot be played due to a problem allocating resources.
    \o FormatError - the audio format is not supported.
    \o NetworkError - the audio cannot be played due to network issues.
    \o AccessDenied - the audio cannot be played due to insufficient permissions.
    \o ServiceMissing -  the audio cannot be played because the media service could not be
    instantiated.
    \endlist
*/

QmlAudio::Error QmlAudio::error() const
{
    return Error(m_error);
}

/*!
    \qmlproperty string Audio::errorString

    This property holds a string describing the current error condition in more detail.
*/

/*!
    \qmlproperty Error Audio::onError(error, errorString)

    This handler is called when an \l {Error}{error} has occurred.  The errorString parameter
    may contain more detailed information about the error.
*/

// ***************************************
// Documentation for meta-data properties.
// ***************************************

/*!
    \qmlproperty variant Audio::title

    This property holds the tile of the media.

    \sa {QtMedia::Title}
*/

/*!
    \qmlproperty variant Audio::subTitle

    This property holds the sub-title of the media.

    \sa {QtMedia::SubTitle}
*/

/*!
    \qmlproperty variant Audio::author

    This property holds the author of the media.

    \sa {QtMedia::Author}
*/

/*!
    \qmlproperty variant Audio::comment

    This property holds a user comment about the media.

    \sa {QtMedia::Comment}
*/

/*!
    \qmlproperty variant Audio::description

    This property holds a description of the media.

    \sa {QtMedia::Description}
*/

/*!
    \qmlproperty variant Audio::category

    This property holds the category of the media

    \sa {QtMedia::Category}
*/

/*!
    \qmlproperty variant Audio::genre

    This property holds the genre of the media.

    \sa {QtMedia::Genre}
*/

/*!
    \qmlproperty variant Audio::year

    This property holds the year of release of the media.

    \sa {QtMedia::Year}
*/

/*!
    \qmlproperty variant Audio::date

    This property holds the date of the media.

    \sa {QtMedia::Date}
*/

/*!
    \qmlproperty variant Audio::userRating

    This property holds a user rating of the media in the range of 0 to 100.

    \sa {QtMedia::UserRating}
*/

/*!
    \qmlproperty variant Audio::keywords

    This property holds a list of keywords describing the media.

    \sa {QtMedia::Keywords}
*/

/*!
    \qmlproperty variant Audio::language

    This property holds the language of the media, as an ISO 639-2 code.

    \sa {QtMedia::Language}
*/

/*!
    \qmlproperty variant Audio::publisher

    This property holds the publisher of the media.

    \sa {QtMedia::Publisher}
*/

/*!
    \qmlproperty variant Audio::copyright

    This property holds the media's copyright notice.

    \sa {QtMedia::Copyright}
*/

/*!
    \qmlproperty variant Audio::parentalRating

    This property holds the parental rating of the media.

    \sa {QtMedia::ParentalRating}
*/

/*!
    \qmlproperty variant Audio::ratingOrganisation

    This property holds the name of the rating organisation responsible for the
    parental rating of the media.

    \sa {QtMedia::RatingOrganisation}
*/

/*!
    \qmlproperty variant Audio::size

    This property property holds the size of the media in bytes.

    \sa {QtMedia::Size}
*/

/*!
    \qmlproperty variant Audio::mediaType

    This property holds the type of the media.

    \sa {QtMedia::MediaType}
*/

/*!
    \qmlproperty variant Audio::audioBitRate

    This property holds the bit rate of the media's audio stream ni bits per
    second.

    \sa {QtMedia::AudioBitRate}
*/

/*!
    \qmlproperty variant Audio::audioCodec

    This property holds the encoding of the media audio stream.

    \sa {QtMedia::AudioCodec}
*/

/*!
    \qmlproperty variant Audio::averageLevel

    This property holds the average volume level of the media.

    \sa {QtMedia::AverageLevel}
*/

/*!
    \qmlproperty variant Audio::channelCount

    This property holds the number of channels in the media's audio stream.

    \sa {QtMedia::ChannelCount}
*/

/*!
    \qmlproperty variant Audio::peakValue

    This property holds the peak volume of media's audio stream.

    \sa {QtMedia::PeakValue}
*/

/*!
    \qmlproperty variant Audio::sampleRate

    This property holds the sample rate of the media's audio stream in hertz.

    \sa {QtMedia::SampleRate}
*/

/*!
    \qmlproperty variant Audio::albumTitle

    This property holds the title of the album the media belongs to.

    \sa {QtMedia::AlbumTitle}
*/

/*!
    \qmlproperty variant Audio::albumArtist

    This property holds the name of the principal artist of the album the media
    belongs to.

    \sa {QtMedia::AlbumArtist}
*/

/*!
    \qmlproperty variant Audio::contributingArtist

    This property holds the names of artists contributing to the media.

    \sa {QtMedia::ContributingArtist}
*/

/*!
    \qmlproperty variant Audio::composer

    This property holds the composer of the media.

    \sa {QtMedia::Composer}
*/

/*!
    \qmlproperty variant Audio::conductor

    This property holds the conductor of the media.

    \sa {QtMedia::Conductor}
*/

/*!
    \qmlproperty variant Audio::lyrics

    This property holds the lyrics to the media.

    \sa {QtMedia::Lyrics}
*/

/*!
    \qmlproperty variant Audio::mood

    This property holds the mood of the media.

    \sa {QtMedia::Mood}
*/

/*!
    \qmlproperty variant Audio::trackNumber

    This property holds the track number of the media.

    \sa {QtMedia::TrackNumber}
*/

/*!
    \qmlproperty variant Audio::trackCount

    This property holds the number of track on the album containing the media.

    \sa {QtMedia::TrackNumber}
*/

/*!
    \qmlproperty variant Audio::coverArtUrlSmall

    This property holds the URL of a small cover art image.

    \sa {QtMedia::CoverArtUrlSmall}
*/

/*!
    \qmlproperty variant Audio::coverArtUrlLarge

    This property holds the URL of a large cover art image.

    \sa {QtMedia::CoverArtUrlLarge}
*/

/*!
    \qmlproperty variant Audio::resolution

    This property holds the dimension of an image or video.

    \sa {QtMedia::Resolution}
*/

/*!
    \qmlproperty variant Audio::pixelAspectRatio

    This property holds the pixel aspect ratio of an image or video.

    \sa {QtMedia::PixelAspectRatio}
*/

/*!
    \qmlproperty variant Audio::videoFrameRate

    This property holds the frame rate of the media's video stream.

    \sa {QtMedia::VideoFrameRate}
*/

/*!
    \qmlproperty variant Audio::videoBitRate

    This property holds the bit rate of the media's video stream in bits per
    second.

    \sa {QtMedia::VideoBitRate}
*/

/*!
    \qmlproperty variant Audio::videoCodec

    This property holds the encoding of the media's video stream.

    \sa {QtMedia::VideoCodec}
*/

/*!
    \qmlproperty variant Audio::posterUrl

    This property holds the URL of a poster image.

    \sa {QtMedia::PosterUrl}
*/

/*!
    \qmlproperty variant Audio::chapterNumber

    This property holds the chapter number of the media.

    \sa {QtMedia::ChapterNumber}
*/

/*!
    \qmlproperty variant Audio::director

    This property holds the director of the media.

    \sa {QtMedia::Director}
*/

/*!
    \qmlproperty variant Audio::leadPerformer

    This property holds the lead performer in the media.

    \sa {QtMedia::LeadPerformer}
*/

/*!
    \qmlproperty variant Audio::writer

    This property holds the writer of the media.

    \sa {QtMedia::Writer}
*/

// The remaining properties are related to photos, and are technically
// available but will certainly never have values.
#ifndef Q_QDOC

/*!
    \qmlproperty variant Audio::cameraManufacturer

    \sa {QtMedia::CameraManufacturer}
*/

/*!
    \qmlproperty variant Audio::cameraModel

    \sa {QtMedia::CameraModel}
*/

/*!
    \qmlproperty variant Audio::event

    \sa {QtMedia::Event}
*/

/*!
    \qmlproperty variant Audio::subject

    \sa {QtMedia::Subject}
*/

/*!
    \qmlproperty variant Audio::orientation

    \sa {QtMedia::Orientation}
*/

/*!
    \qmlproperty variant Audio::exposureTime

    \sa {QtMedia::ExposureTime}
*/

/*!
    \qmlproperty variant Audio::fNumber

    \sa {QtMedia::FNumber}
*/

/*!
    \qmlproperty variant Audio::exposureProgram

    \sa {QtMedia::ExposureProgram}
*/

/*!
    \qmlproperty variant Audio::isoSpeedRatings

    \sa {QtMedia::ISOSpeedRatings}
*/

/*!
    \qmlproperty variant Audio::exposureBiasValue

    \sa {QtMedia::ExposureBiasValue}
*/

/*!
    \qmlproperty variant Audio::dateTimeDigitized

    \sa {QtMedia::DateTimeDigitized}
*/

/*!
    \qmlproperty variant Audio::subjectDistance

    \sa {QtMedia::SubjectDistance}
*/

/*!
    \qmlproperty variant Audio::meteringMode

    \sa {QtMedia::MeteringMode}
*/

/*!
    \qmlproperty variant Audio::lightSource

    \sa {QtMedia::LightSource}
*/

/*!
    \qmlproperty variant Audio::flash

    \sa {QtMedia::Flash}
*/

/*!
    \qmlproperty variant Audio::focalLength

    \sa {QtMedia::FocalLength}
*/

/*!
    \qmlproperty variant Audio::exposureMode

    \sa {QtMedia::ExposureMode}
*/

/*!
    \qmlproperty variant Audio::whiteBalance

    \sa {QtMedia::WhiteBalance}
*/

/*!
    \qmlproperty variant Audio::DigitalZoomRatio

    \sa {QtMedia::DigitalZoomRatio}
*/

/*!
    \qmlproperty variant Audio::focalLengthIn35mmFilm

    \sa {QtMedia::FocalLengthIn35mmFile}
*/

/*!
    \qmlproperty variant Audio::sceneCaptureType

    \sa {QtMedia::SceneCaptureType}
*/

/*!
    \qmlproperty variant Audio::gainControl

    \sa {QtMedia::GainControl}
*/

/*!
    \qmlproperty variant Audio::contrast

    \sa {QtMedia::contrast}
*/

/*!
    \qmlproperty variant Audio::saturation

    \sa {QtMedia::Saturation}
*/

/*!
    \qmlproperty variant Audio::sharpness

    \sa {QtMedia::Sharpness}
*/

/*!
    \qmlproperty variant Audio::deviceSettingDescription

    \sa {QtMedia::DeviceSettingDescription}
*/

#endif

#include "moc_qmlaudio_p.cpp"

QTM_END_NAMESPACE

