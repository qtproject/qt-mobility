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

#include "qgstreamercapturemetadatacontrol.h"

#include <gst/gst.h>

struct QGstreamerMetadataKeyLookup
{
    QMediaMetadata::Key key;
    const char *token;
};

static const QGstreamerMetadataKeyLookup qt_gstreamerMetadataKeys[] =
{
    { QMediaMetadata::Title, GST_TAG_TITLE },
    //{ QMediaMetadata::SubTitle, 0 },
    //{ QMediaMetadata::Author, 0 },
    { QMediaMetadata::Comment, GST_TAG_COMMENT },
    { QMediaMetadata::Description, GST_TAG_DESCRIPTION },
    //{ QMediaMetadata::Category, 0 },
    { QMediaMetadata::Genre, GST_TAG_GENRE },
    //{ QMediaMetadata::Year, 0 },
    //{ QMediaMetadata::UserRating, 0 },

    { QMediaMetadata::Language, GST_TAG_LANGUAGE_CODE },

    { QMediaMetadata::Publisher, GST_TAG_ORGANIZATION },
    { QMediaMetadata::Copyright, GST_TAG_COPYRIGHT },
    //{ QMediaMetadata::ParentalRating, 0 },
    //{ QMediaMetadata::RatingOrganisation, 0 },

    // Media
    //{ QMediaMetadata::Size, 0 },
    //{ QMediaMetadata::MediaType, 0 },
    { QMediaMetadata::Duration, GST_TAG_DURATION },

    // Audio
    { QMediaMetadata::AudioBitrate, GST_TAG_BITRATE },
    { QMediaMetadata::AudioCodec, GST_TAG_AUDIO_CODEC },
    //{ QMediaMetadata::Channels, 0 },
    //{ QMediaMetadata::Frequency, 0 },

    // Music
    { QMediaMetadata::AlbumTitle, GST_TAG_ALBUM },
    { QMediaMetadata::AlbumArtist,  GST_TAG_ARTIST},
    { QMediaMetadata::ContributingArtist, GST_TAG_PERFORMER },
    { QMediaMetadata::Composer, GST_TAG_COMPOSER },
    //{ QMediaMetadata::Conductor, 0 },
    //{ QMediaMetadata::Lyrics, 0 },
    //{ QMediaMetadata::Mood, 0 },
    { QMediaMetadata::TrackNumber, GST_TAG_TRACK_NUMBER },

    //{ QMediaMetadata::CoverArtUriSmall, 0 },
    //{ QMediaMetadata::CoverArtUriLarge, 0 },

    // Image/Video
    //{ QMediaMetadata::Resolution, 0 },
    //{ QMediaMetadata::PixelAspectRatio, 0 },

    // Video
    //{ QMediaMetadata::FrameRate, 0 },
    //{ QMediaMetadata::VideoBitRate, 0 },
    { QMediaMetadata::VideoCodec, GST_TAG_VIDEO_CODEC },

    //{ QMediaMetadata::PosterUri, 0 },

    // Movie
    //{ QMediaMetadata::ChapterNumber, 0 },
    //{ QMediaMetadata::Director, 0 },
    { QMediaMetadata::LeadPerformer, GST_TAG_PERFORMER },
    //{ QMediaMetadata::Writer, 0 },

    // Photos
    //{ QMediaMetadata::CameraManufacturer, 0 },
    //{ QMediaMetadata::CameraModel, 0 },
    //{ QMediaMetadata::Event, 0 },
    //{ QMediaMetadata::Subject, 0 }
};

QGstreamerCaptureMetadataControl::QGstreamerCaptureMetadataControl(QObject *parent)
    :QMetadataProviderControl(parent)
{
}

QVariant QGstreamerCaptureMetadataControl::metadata(QMediaMetadata::Key key) const
{
    static const int count = sizeof(qt_gstreamerMetadataKeys) / sizeof(QGstreamerMetadataKeyLookup);

    for (int i = 0; i < count; ++i) {
        if (qt_gstreamerMetadataKeys[i].key == key) {
            const char *name = qt_gstreamerMetadataKeys[i].token;

            return m_values.value(QByteArray::fromRawData(name, qstrlen(name)));
        }
    }
    return QVariant();
}

void QGstreamerCaptureMetadataControl::setMetadata(QMediaMetadata::Key key, const QVariant &value)
{
    static const int count = sizeof(qt_gstreamerMetadataKeys) / sizeof(QGstreamerMetadataKeyLookup);

    for (int i = 0; i < count; ++i) {
        if (qt_gstreamerMetadataKeys[i].key == key) {
            const char *name = qt_gstreamerMetadataKeys[i].token;

            m_values.insert(QByteArray::fromRawData(name, qstrlen(name)), value);

            emit QMetadataProviderControl::metadataChanged();
            emit metadataChanged(m_values);

            return;
        }
    }
}

QVariant QGstreamerCaptureMetadataControl::extendedMetadata(QString const &name) const
{
    return m_values.value(name.toLatin1());
}

void QGstreamerCaptureMetadataControl::setExtendedMetadata(QString const &name, QVariant const &value)
{
    m_values.insert(name.toLatin1(), value);
    emit QMetadataProviderControl::metadataChanged();
    emit metadataChanged(m_values);
}
