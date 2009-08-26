/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/private/qobject_p.h>
#include <QtCore/QDebug>

#include "qmediametadata.h"
#include "qmetadataprovidercontrol.h"
#include "qabstractmediaobject.h"
#include "qabstractmediaservice.h"

/*!
    \class QMediaMetadata
    \ingroup multimedia

    \preliminary
    \brief Use this class to extract Metadata from a Multimedia object.
*/

/*!
    \enum QMediaMetadata::Key

    Common attributes
    \value Title The title of the media.  QString.
    \value SubTitle The sub-title of the media. QString.
    \value Author The authors of the media. QStringList.
    \value Comment A user comment about the media. QString.
    \value Description A description of the media.  QString
    \value Category The category of the media.  QStringList.
    \value Genre The genre of the media.  QStringList.
    \value Year The year of release of the media.  int.
    \value Date The date of the media. QDate.
    \value UserRating A user rating of the media. int [0..100].
    \value Keywords A list of keywords describing the media.  QStringList.
    \value Language

    \value Publisher The publisher of the media.  QString.
    \value Copyright The media's copyright notice.  QString.
    \value ParentalRating  The parental rating of the media.  QString.
    \value RatingOrganisation The organisation responsible for the parental rating of the media.
    QString.

    Media attributes
    \value Size The size in bytes of the media. qint64
    \value MediaType The type of the media (audio, video, etc).  QString.
    \value Duration The duration in millseconds of the media.  qint64.

    Audio attributes
    \value AudioBitrate The bit rate of the media's audio stream in bits per second.  int.
    \value AudioCodec The codec of the media's audio stream.  QString.
    \value AverageLevel The average volume level of the media.  int.
    \value Channels The number of channels in the media's audio stream. int.
    \value PeakValue The peak volume of the media's audio stream. int
    \value Frequency The frequency of the media's audio stream. int

    Music attributes
    \value AlbumTitle The title of the album the media belongs to.  QString.
    \value AlbumArtist The principal artist of the album the media belongs to.  QString.
    \value ContributingArtist The artists contributing to the media.  QStringList.
    \value Composer The composer of the media.  QStringList.
    \value Conductor The conductor of the media. QString.
    \value Lyrics The lyrics to the media. QString.
    \value Mood The mood of the media.  QString.
    \value TrackNumber The track number of the media.  int.
    \value TrackCount The number of tracks on the album containing the media.  int.

    \value CoverArtUriSmall The URI of a small cover art image. QUrl.
    \value CoverArtUriLarge The URI of a large cover art image. QUrl.

    Image and video attributes
    \value Resolution The dimensions of an image or video.  QSize.
    \value PixelAspectRatio The pixel aspect ratio of an image or video.  QSize.

    Video attributes
    \value FrameRate The frame rate of the media's video stream.  QPair<int, int>.
    \value VideoBitRate The bit rate of the media's video stream in bits per second.  int.
    \value VideoCodec The codec of the media's video stream.  QString.

    \value PosterUri The URI of a poster image.  QUrl.

    Movie attributes
    \value ChapterNumber The chapter number of the media.  int.
    \value Director The director of the media.  QString.
    \value LeadPerformer The lead performer in the media.  QStringList.
    \value Writer The writer of the media.  QStringList.

    Photo attributes
    \value CameraManufacturer The manufacturer of the camera used to capture the media.  QString.
    \value CameraModel The model of the camera used to capture the media.  QString.
    \value Event The event during which the media was captured.  QString.
    \value Subject The subject of the media.  QString.
*/

class QMediaMetadataPrivate
{
public:
    QAbstractMediaService    *service;
    QMetadataProviderControl *provider;
};

/*!
    Construct a QMediaMetadata to read and or write Metadata from the Mulitmedia object \a mediaObject.
*/

QMediaMetadata::QMediaMetadata(QAbstractMediaObject *mediaObject)
    : QObject(mediaObject)
    , d_ptr(new QMediaMetadataPrivate)
{
    Q_D(QMediaMetadata);

    d->service = mediaObject->service();
    d->provider = qobject_cast<QMetadataProviderControl*>(d->service->control(QMetadataProviderControl_iid));

    if (d->provider != 0) {
        connect(d->provider, SIGNAL(metadataChanged()), SIGNAL(metadataChanged()));
        connect(d->provider, SIGNAL(metadataAvailabilityChanged(bool)), SIGNAL(metadataAvailabilityChanged(bool)));
        connect(d->provider, SIGNAL(readOnlyChanged(bool)), SIGNAL(readOnlyChanged(bool)));
    } else {
        qWarning() << "No provider for media metadata";
    }
}

/*!
    Destroys the metadata object.
*/

QMediaMetadata::~QMediaMetadata()
{
    delete d_ptr;
}

bool QMediaMetadata::metadataAvailable() const
{
    Q_D(const QMediaMetadata);

    if (d->provider == 0)
        return false;

    return d->provider->metadataAvailable();
}

bool QMediaMetadata::isReadOnly() const
{
    Q_D(const QMediaMetadata);

    if (d->provider == 0)
        return true;

    return d->provider->isReadOnly();
}

/*!
    Returns the metadata for the attribute with the given \a key.
*/

QVariant QMediaMetadata::metadata(Key key) const
{
    Q_D(const QMediaMetadata);

    if (d->provider == 0)
        return QVariant();

    return d->provider->metadata(key);
}

/*!
    Sets the the \a value of the metadata attribute with the given \a key.
*/

void QMediaMetadata::setMetadata(Key key, const QVariant &value)
{
    Q_D(QMediaMetadata);

    if (d->provider != 0 && !d->provider->isReadOnly())
        d->provider->setMetadata(key, value);
}

/*!
    Returns the metadata for the attribute with the given \a key.
*/

QVariant QMediaMetadata::extendedMetadata(const QString &key) const
{
    Q_D(const QMediaMetadata);

    if (d->provider == 0)
        return QVariant();

    return d->provider->extendedMetadata(key);
}

/*!
    Sets the \a value of the metadata attribute with the given \a key.
*/

void QMediaMetadata::setExtendedMetadata(const QString &key, const QVariant &value)
{
    Q_D(QMediaMetadata);

    if (d->provider != 0)
        d->provider->setExtendedMetadata(key, value);
}

/*!
    \property QMediaMetadata::metadataAvailable
    \brief true if metadata can be read from the Multimedia object.
*/

/*!
    \property QMediaMetadata::readOnly
    \brief true if metadata can only be read from the Multimedia object, false
    if elements can be set.
*/

/*!
    \fn void QMediaMetadata::metadataChanged()

    Signal the metadata has changed.
*/

/*!
    \fn void QMediaMetadata::metadataAvailablilityChanged(bool available)

    Signal the availability of metadata has changed, \a available will
    be true if the multimedia object has metadata.
*/

/*!
    \fn void QMediaMetadata::readOnlyChanged(bool readOnly)

    Signal a change in the read only status of meta data, \a readOnly will be
    true if metadata elements can not be added or adjusted.
*/

