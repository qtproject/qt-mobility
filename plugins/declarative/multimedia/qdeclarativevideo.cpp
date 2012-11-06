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

#include "qdeclarativevideo_p.h"

#include <qmediaplayercontrol.h>
#include <qmediaservice.h>
#include <qpaintervideosurface_p.h>
#include <qvideorenderercontrol.h>


QT_BEGIN_NAMESPACE


void QDeclarativeVideo::_q_nativeSizeChanged(const QSizeF &size)
{
    setImplicitWidth(size.width());
    setImplicitHeight(size.height());
}

void QDeclarativeVideo::_q_error(int errorCode, const QString &errorString)
{
    m_error = QMediaPlayer::Error(errorCode);
    m_errorString = errorString;

    emit error(Error(errorCode), errorString);
    emit errorChanged();
}


/*!
    \qmlclass Video QDeclarativeVideo
    \brief The Video element allows you to add videos to a scene.
    \inherits Item
    \ingroup qml-multimedia
    \since Mobility 1.0

    This element is part of the \bold{QtMultimediaKit 1.1} module.

    \qml
    import Qt 4.7
    import QtMultimediaKit 1.1

    Video {
        id: video
        width : 800
        height : 600
        source: "video.avi"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                video.play()
            }
        }

        focus: true
        Keys.onSpacePressed: video.paused = !video.paused
        Keys.onLeftPressed: video.position -= 5000
        Keys.onRightPressed: video.position += 5000
    }
    \endqml

    The Video item supports untransformed, stretched, and uniformly scaled video presentation.
    For a description of stretched uniformly scaled presentation, see the \l fillMode property
    description.

    The Video item is only visible when the \l hasVideo property is true and the video is playing.

    \sa Audio
*/

/*!
    \internal
    \class QDeclarativeVideo
    \brief The QDeclarativeVideo class provides a video item that you can add to a QDeclarativeView.
*/

QDeclarativeVideo::QDeclarativeVideo(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_graphicsItem(0)

{
}

QDeclarativeVideo::~QDeclarativeVideo()
{
    shutdown();

    delete m_graphicsItem;
}

/*!
    \qmlproperty url Video::source
    \since Mobility 1.0

    This property holds the source URL of the media.
*/

/*!
    \qmlproperty url Video::autoLoad
    \since Mobility 1.0

    This property indicates if loading of media should begin immediately.

    Defaults to true, if false media will not be loaded until playback is started.
*/

/*!
    \qmlproperty bool Video::playing
    \since Mobility 1.0

    This property holds whether the media is playing.

    Defaults to false, and can be set to true to start playback.
*/

/*!
    \qmlproperty bool Video::paused
    \since Mobility 1.0

    This property holds whether the media is paused.

    Defaults to false, and can be set to true to pause playback.
*/

/*!
    \qmlsignal Video::onStarted()
    \since Mobility 1.0

    This handler is called when playback is started.
*/

/*!
    \qmlsignal Video::onResumed()
    \since Mobility 1.0

    This handler is called when playback is resumed from the paused state.
*/

/*!
    \qmlsignal Video::onPaused()
    \since Mobility 1.0

    This handler is called when playback is paused.
*/

/*!
    \qmlsignal Video::onStopped()
    \since Mobility 1.0

    This handler is called when playback is stopped.
*/

/*!
    \qmlproperty enumeration Video::status

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
    \o UnknownStatus - the status of the media is cannot be determined.
    \endlist
*/

QDeclarativeVideo::Status QDeclarativeVideo::status() const
{
    return Status(m_status);
}

/*!
    \qmlproperty int Video::duration
    \since Mobility 1.0

    This property holds the duration of the media in milliseconds.

    If the media doesn't have a fixed duration (a live stream for example) this will be 0.
*/

/*!
    \qmlproperty int Video::position
    \since Mobility 1.0

    This property holds the current playback position in milliseconds.
*/

/*!
    \qmlproperty real Video::volume
    \since Mobility 1.0

    This property holds the volume of the audio output, from 0.0 (silent) to 1.0 (maximum volume).
*/

/*!
    \qmlproperty bool Video::muted
    \since Mobility 1.0

    This property holds whether the audio output is muted.
*/

/*!
    \qmlproperty bool Video::hasAudio
    \since Mobility 1.0

    This property holds whether the media contains audio.
*/

bool QDeclarativeVideo::hasAudio() const
{
    return !m_complete ? false : m_playerControl->isAudioAvailable();
}

/*!
    \qmlproperty bool Video::hasVideo
    \since Mobility 1.0

    This property holds whether the media contains video.
*/

bool QDeclarativeVideo::hasVideo() const
{
    return !m_complete ? false : m_playerControl->isVideoAvailable();
}

/*!
    \qmlproperty real Video::bufferProgress
    \since Mobility 1.0

    This property holds how much of the data buffer is currently filled, from 0.0 (empty) to 1.0
    (full).
*/

/*!
    \qmlproperty bool Video::seekable
    \since Mobility 1.0

    This property holds whether position of the video can be changed.
*/

/*!
    \qmlproperty real Video::playbackRate
    \since Mobility 1.0

    This property holds the rate at which video is played at as a multiple of the normal rate.
*/

/*!
    \qmlproperty enumeration Video::error

    This property holds the error state of the video.  It can be one of:

    \list
    \o NoError - there is no current error.
    \o ResourceError - the video cannot be played due to a problem allocating resources.
    \o FormatError - the video format is not supported.
    \o NetworkError - the video cannot be played due to network issues.
    \o AccessDenied - the video cannot be played due to insufficient permissions.
    \o ServiceMissing -  the video cannot be played because the media service could not be
    instantiated.
    \endlist
*/


QDeclarativeVideo::Error QDeclarativeVideo::error() const
{
    return Error(m_error);
}

/*!
    \qmlproperty string Video::errorString
    \since Mobility 1.0

    This property holds a string describing the current error condition in more detail.
*/

/*!
    \qmlsignal Video::onError(error, errorString)
    \since Mobility 1.0

    This handler is called when an \l {QMediaPlayer::Error}{error} has
    occurred.  The errorString parameter may contain more detailed
    information about the error.
*/

/*!
    \qmlproperty enumeration Video::fillMode
    \since Mobility 1.0

    Set this property to define how the video is scaled to fit the target area.

    \list
    \o Stretch - the video is scaled to fit.
    \o PreserveAspectFit - the video is scaled uniformly to fit without cropping
    \o PreserveAspectCrop - the video is scaled uniformly to fill, cropping if necessary
    \endlist

    The default fill mode is PreserveAspectFit.
*/

QDeclarativeVideo::FillMode QDeclarativeVideo::fillMode() const
{
    return FillMode(m_graphicsItem->aspectRatioMode());
}

void QDeclarativeVideo::setFillMode(FillMode mode)
{
    m_graphicsItem->setAspectRatioMode(Qt::AspectRatioMode(mode));
}

/*!
    \qmlmethod Video::play()
    \since Mobility 1.0

    Starts playback of the media.

    Sets the \l playing property to true, and the \l paused property to false.
*/

void QDeclarativeVideo::play()
{
    if (!m_complete)
        return;

    setPaused(false);
    setPlaying(true);
}

/*!
    \qmlmethod Video::pause()
    \since Mobility 1.0

    Pauses playback of the media.

    Sets the \l playing and \l paused properties to true.
*/

void QDeclarativeVideo::pause()
{
    if (!m_complete)
        return;

    setPaused(true);
    setPlaying(true);
}

/*!
    \qmlmethod Video::stop()
    \since Mobility 1.0

    Stops playback of the media.

    Sets the \l playing and \l paused properties to false.
*/

void QDeclarativeVideo::stop()
{
    if (!m_complete)
        return;

    setPlaying(false);
    setPaused(false);
}

void QDeclarativeVideo::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{
}

void QDeclarativeVideo::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_graphicsItem->setSize(newGeometry.size());

    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

void QDeclarativeVideo::classBegin()
{
    m_graphicsItem = new QGraphicsVideoItem(this);
    connect(m_graphicsItem, SIGNAL(nativeSizeChanged(QSizeF)),
            this, SLOT(_q_nativeSizeChanged(QSizeF)));

    setObject(this);

    if (m_mediaService) {
        connect(m_playerControl, SIGNAL(audioAvailableChanged(bool)),
                this, SIGNAL(hasAudioChanged()));
        connect(m_playerControl, SIGNAL(videoAvailableChanged(bool)),
                this, SIGNAL(hasVideoChanged()));

        m_mediaObject->bind(m_graphicsItem);
    }
}

void QDeclarativeVideo::componentComplete()
{
    QDeclarativeMediaBase::componentComplete();
}

QT_END_NAMESPACE

// ***************************************
// Documentation for meta-data properties.
// ***************************************

/*!
    \qmlproperty variant Video::metaData.title
    \since Mobility 1.1

    This property holds the tile of the media.

    \sa {QtMultimediaKit::Title}
*/

/*!
    \qmlproperty variant Video::metaData.subTitle
    \since Mobility 1.1

    This property holds the sub-title of the media.

    \sa {QtMultimediaKit::SubTitle}
*/

/*!
    \qmlproperty variant Video::metaData.author
    \since Mobility 1.1

    This property holds the author of the media.

    \sa {QtMultimediaKit::Author}
*/

/*!
    \qmlproperty variant Video::metaData.comment
    \since Mobility 1.1

    This property holds a user comment about the media.

    \sa {QtMultimediaKit::Comment}
*/

/*!
    \qmlproperty variant Video::metaData.description
    \since Mobility 1.1

    This property holds a description of the media.

    \sa {QtMultimediaKit::Description}
*/

/*!
    \qmlproperty variant Video::metaData.category
    \since Mobility 1.1

    This property holds the category of the media

    \sa {QtMultimediaKit::Category}
*/

/*!
    \qmlproperty variant Video::metaData.genre
    \since Mobility 1.1

    This property holds the genre of the media.

    \sa {QtMultimediaKit::Genre}
*/

/*!
    \qmlproperty variant Video::metaData.year
    \since Mobility 1.1

    This property holds the year of release of the media.

    \sa {QtMultimediaKit::Year}
*/

/*!
    \qmlproperty variant Video::metaData.date
    \since Mobility 1.1

    This property holds the date of the media.

    \sa {QtMultimediaKit::Date}
*/

/*!
    \qmlproperty variant Video::metaData.userRating
    \since Mobility 1.1

    This property holds a user rating of the media in the range of 0 to 100.

    \sa {QtMultimediaKit::UserRating}
*/

/*!
    \qmlproperty variant Video::metaData.keywords
    \since Mobility 1.1

    This property holds a list of keywords describing the media.

    \sa {QtMultimediaKit::Keywords}
*/

/*!
    \qmlproperty variant Video::metaData.language
    \since Mobility 1.1

    This property holds the language of the media, as an ISO 639-2 code.

    \sa {QtMultimediaKit::Language}
*/

/*!
    \qmlproperty variant Video::metaData.publisher
    \since Mobility 1.1

    This property holds the publisher of the media.

    \sa {QtMultimediaKit::Publisher}
*/

/*!
    \qmlproperty variant Video::metaData.copyright
    \since Mobility 1.1

    This property holds the media's copyright notice.

    \sa {QtMultimediaKit::Copyright}
*/

/*!
    \qmlproperty variant Video::metaData.parentalRating
    \since Mobility 1.1

    This property holds the parental rating of the media.

    \sa {QtMultimediaKit::ParentalRating}
*/

/*!
    \qmlproperty variant Video::metaData.ratingOrganisation
    \since Mobility 1.1

    This property holds the name of the rating organisation responsible for the
    parental rating of the media.

    \sa {QtMultimediaKit::RatingOrganisation}
*/

/*!
    \qmlproperty variant Video::metaData.size
    \since Mobility 1.1

    This property property holds the size of the media in bytes.

    \sa {QtMultimediaKit::Size}
*/

/*!
    \qmlproperty variant Video::metaData.mediaType
    \since Mobility 1.1

    This property holds the type of the media.

    \sa {QtMultimediaKit::MediaType}
*/

/*!
    \qmlproperty variant Video::metaData.audioBitRate
    \since Mobility 1.1

    This property holds the bit rate of the media's audio stream ni bits per
    second.

    \sa {QtMultimediaKit::AudioBitRate}
*/

/*!
    \qmlproperty variant Video::metaData.audioCodec
    \since Mobility 1.1

    This property holds the encoding of the media audio stream.

    \sa {QtMultimediaKit::AudioCodec}
*/

/*!
    \qmlproperty variant Video::metaData.averageLevel
    \since Mobility 1.1

    This property holds the average volume level of the media.

    \sa {QtMultimediaKit::AverageLevel}
*/

/*!
    \qmlproperty variant Video::metaData.channelCount
    \since Mobility 1.1

    This property holds the number of channels in the media's audio stream.

    \sa {QtMultimediaKit::ChannelCount}
*/

/*!
    \qmlproperty variant Video::metaData.peakValue
    \since Mobility 1.1

    This property holds the peak volume of media's audio stream.

    \sa {QtMultimediaKit::PeakValue}
*/

/*!
    \qmlproperty variant Video::metaData.sampleRate
    \since Mobility 1.1

    This property holds the sample rate of the media's audio stream in hertz.

    \sa {QtMultimediaKit::SampleRate}
*/

/*!
    \qmlproperty variant Video::metaData.albumTitle
    \since Mobility 1.1

    This property holds the title of the album the media belongs to.

    \sa {QtMultimediaKit::AlbumTitle}
*/

/*!
    \qmlproperty variant Video::metaData.albumArtist
    \since Mobility 1.1

    This property holds the name of the principal artist of the album the media
    belongs to.

    \sa {QtMultimediaKit::AlbumArtist}
*/

/*!
    \qmlproperty variant Video::metaData.contributingArtist
    \since Mobility 1.1

    This property holds the names of artists contributing to the media.

    \sa {QtMultimediaKit::ContributingArtist}
*/

/*!
    \qmlproperty variant Video::metaData.composer
    \since Mobility 1.1

    This property holds the composer of the media.

    \sa {QtMultimediaKit::Composer}
*/

/*!
    \qmlproperty variant Video::metaData.conductor
    \since Mobility 1.1

    This property holds the conductor of the media.

    \sa {QtMultimediaKit::Conductor}
*/

/*!
    \qmlproperty variant Video::metaData.lyrics
    \since Mobility 1.1

    This property holds the lyrics to the media.

    \sa {QtMultimediaKit::Lyrics}
*/

/*!
    \qmlproperty variant Video::metaData.mood
    \since Mobility 1.1

    This property holds the mood of the media.

    \sa {QtMultimediaKit::Mood}
*/

/*!
    \qmlproperty variant Video::metaData.trackNumber
    \since Mobility 1.1

    This property holds the track number of the media.

    \sa {QtMultimediaKit::TrackNumber}
*/

/*!
    \qmlproperty variant Video::metaData.trackCount
    \since Mobility 1.1

    This property holds the number of track on the album containing the media.

    \sa {QtMultimediaKit::TrackNumber}
*/

/*!
    \qmlproperty variant Video::metaData.coverArtUrlSmall
    \since Mobility 1.1

    This property holds the URL of a small cover art image.

    \sa {QtMultimediaKit::CoverArtUrlSmall}
*/

/*!
    \qmlproperty variant Video::metaData.coverArtUrlLarge
    \since Mobility 1.1

    This property holds the URL of a large cover art image.

    \sa {QtMultimediaKit::CoverArtUrlLarge}
*/

/*!
    \qmlproperty variant Video::metaData.resolution
    \since Mobility 1.1

    This property holds the dimension of an image or video.

    \sa {QtMultimediaKit::Resolution}
*/

/*!
    \qmlproperty variant Video::metaData.pixelAspectRatio
    \since Mobility 1.1

    This property holds the pixel aspect ratio of an image or video.

    \sa {QtMultimediaKit::PixelAspectRatio}
*/

/*!
    \qmlproperty variant Video::metaData.videoFrameRate
    \since Mobility 1.1

    This property holds the frame rate of the media's video stream.

    \sa {QtMultimediaKit::VideoFrameRate}
*/

/*!
    \qmlproperty variant Video::metaData.videoBitRate
    \since Mobility 1.1

    This property holds the bit rate of the media's video stream in bits per
    second.

    \sa {QtMultimediaKit::VideoBitRate}
*/

/*!
    \qmlproperty variant Video::metaData.videoCodec
    \since Mobility 1.1

    This property holds the encoding of the media's video stream.

    \sa {QtMultimediaKit::VideoCodec}
*/

/*!
    \qmlproperty variant Video::metaData.posterUrl
    \since Mobility 1.1

    This property holds the URL of a poster image.

    \sa {QtMultimediaKit::PosterUrl}
*/

/*!
    \qmlproperty variant Video::metaData.chapterNumber
    \since Mobility 1.1

    This property holds the chapter number of the media.

    \sa {QtMultimediaKit::ChapterNumber}
*/

/*!
    \qmlproperty variant Video::metaData.director
    \since Mobility 1.1

    This property holds the director of the media.

    \sa {QtMultimediaKit::Director}
*/

/*!
    \qmlproperty variant Video::metaData.leadPerformer
    \since Mobility 1.1

    This property holds the lead performer in the media.

    \sa {QtMultimediaKit::LeadPerformer}
*/

/*!
    \qmlproperty variant Video::metaData.writer
    \since Mobility 1.1

    This property holds the writer of the media.

    \sa {QtMultimediaKit::Writer}
*/

// The remaining properties are related to photos, and are technically
// available but will certainly never have values.
#ifndef Q_QDOC

/*!
    \qmlproperty variant Video::metaData.cameraManufacturer
    \since Mobility 1.1

    \sa {QtMultimediaKit::CameraManufacturer}
*/

/*!
    \qmlproperty variant Video::metaData.cameraModel
    \since Mobility 1.1

    \sa {QtMultimediaKit::CameraModel}
*/

/*!
    \qmlproperty variant Video::metaData.event
    \since Mobility 1.1

    \sa {QtMultimediaKit::Event}
*/

/*!
    \qmlproperty variant Video::metaData.subject
    \since Mobility 1.1

    \sa {QtMultimediaKit::Subject}
*/

/*!
    \qmlproperty variant Video::metaData.orientation
    \since Mobility 1.1

    \sa {QtMultimediaKit::Orientation}
*/

/*!
    \qmlproperty variant Video::metaData.exposureTime
    \since Mobility 1.1

    \sa {QtMultimediaKit::ExposureTime}
*/

/*!
    \qmlproperty variant Video::metaData.fNumber
    \since Mobility 1.1

    \sa {QtMultimediaKit::FNumber}
*/

/*!
    \qmlproperty variant Video::metaData.exposureProgram
    \since Mobility 1.1

    \sa {QtMultimediaKit::ExposureProgram}
*/

/*!
    \qmlproperty variant Video::metaData.isoSpeedRatings
    \since Mobility 1.1

    \sa {QtMultimediaKit::ISOSpeedRatings}
*/

/*!
    \qmlproperty variant Video::metaData.exposureBiasValue
    \since Mobility 1.1

    \sa {QtMultimediaKit::ExposureBiasValue}
*/

/*!
    \qmlproperty variant Video::metaData.dateTimeDigitized
    \since Mobility 1.1

    \sa {QtMultimediaKit::DateTimeDigitized}
*/

/*!
    \qmlproperty variant Video::metaData.subjectDistance
    \since Mobility 1.1

    \sa {QtMultimediaKit::SubjectDistance}
*/

/*!
    \qmlproperty variant Video::metaData.meteringMode
    \since Mobility 1.1

    \sa {QtMultimediaKit::MeteringMode}
*/

/*!
    \qmlproperty variant Video::metaData.lightSource
    \since Mobility 1.1

    \sa {QtMultimediaKit::LightSource}
*/

/*!
    \qmlproperty variant Video::metaData.flash
    \since Mobility 1.1

    \sa {QtMultimediaKit::Flash}
*/

/*!
    \qmlproperty variant Video::metaData.focalLength
    \since Mobility 1.1

    \sa {QtMultimediaKit::FocalLength}
*/

/*!
    \qmlproperty variant Video::metaData.exposureMode
    \since Mobility 1.1

    \sa {QtMultimediaKit::ExposureMode}
*/

/*!
    \qmlproperty variant Video::metaData.whiteBalance
    \since Mobility 1.1

    \sa {QtMultimediaKit::WhiteBalance}
*/

/*!
    \qmlproperty variant Video::metaData.DigitalZoomRatio
    \since Mobility 1.1

    \sa {QtMultimediaKit::DigitalZoomRatio}
*/

/*!
    \qmlproperty variant Video::metaData.focalLengthIn35mmFilm
    \since Mobility 1.1

    \sa {QtMultimediaKit::FocalLengthIn35mmFile}
*/

/*!
    \qmlproperty variant Video::metaData.sceneCaptureType
    \since Mobility 1.1

    \sa {QtMultimediaKit::SceneCaptureType}
*/

/*!
    \qmlproperty variant Video::metaData.gainControl
    \since Mobility 1.1

    \sa {QtMultimediaKit::GainControl}
*/

/*!
    \qmlproperty variant Video::metaData.contrast
    \since Mobility 1.1

    \sa {QtMultimediaKit::contrast}
*/

/*!
    \qmlproperty variant Video::metaData.saturation
    \since Mobility 1.1

    \sa {QtMultimediaKit::Saturation}
*/

/*!
    \qmlproperty variant Video::metaData.sharpness
    \since Mobility 1.1

    \sa {QtMultimediaKit::Sharpness}
*/

/*!
    \qmlproperty variant Video::metaData.deviceSettingDescription
    \since Mobility 1.1

    \sa {QtMultimediaKit::DeviceSettingDescription}
*/

#endif

#include "moc_qdeclarativevideo_p.cpp"
