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

struct QGstreamerMetaDataKeyLookup
{
    QMediaObject::MetaData key;
    const char *token;
};

static const QGstreamerMetaDataKeyLookup qt_gstreamerMetaDataKeys[] =
{
    { QMediaObject::Title, GST_TAG_TITLE },
    //{ QMediaObject::SubTitle, 0 },
    //{ QMediaObject::Author, 0 },
    { QMediaObject::Comment, GST_TAG_COMMENT },
    { QMediaObject::Description, GST_TAG_DESCRIPTION },
    //{ QMediaObject::Category, 0 },
    { QMediaObject::Genre, GST_TAG_GENRE },
    //{ QMediaObject::Year, 0 },
    //{ QMediaObject::UserRating, 0 },

    { QMediaObject::Language, GST_TAG_LANGUAGE_CODE },

    { QMediaObject::Publisher, GST_TAG_ORGANIZATION },
    { QMediaObject::Copyright, GST_TAG_COPYRIGHT },
    //{ QMediaObject::ParentalRating, 0 },
    //{ QMediaObject::RatingOrganisation, 0 },

    // Media
    //{ QMediaObject::Size, 0 },
    //{ QMediaObject::MediaType, 0 },
    { QMediaObject::Duration, GST_TAG_DURATION },

    // Audio
    { QMediaObject::AudioBitrate, GST_TAG_BITRATE },
    { QMediaObject::AudioCodec, GST_TAG_AUDIO_CODEC },
    //{ QMediaObject::Channels, 0 },
    //{ QMediaObject::Frequency, 0 },

    // Music
    { QMediaObject::AlbumTitle, GST_TAG_ALBUM },
    { QMediaObject::AlbumArtist,  GST_TAG_ARTIST},
    { QMediaObject::ContributingArtist, GST_TAG_PERFORMER },
    { QMediaObject::Composer, GST_TAG_COMPOSER },
    //{ QMediaObject::Conductor, 0 },
    //{ QMediaObject::Lyrics, 0 },
    //{ QMediaObject::Mood, 0 },
    { QMediaObject::TrackNumber, GST_TAG_TRACK_NUMBER },

    //{ QMediaObject::CoverArtUriSmall, 0 },
    //{ QMediaObject::CoverArtUriLarge, 0 },

    // Image/Video
    //{ QMediaObject::Resolution, 0 },
    //{ QMediaObject::PixelAspectRatio, 0 },

    // Video
    //{ QMediaObject::FrameRate, 0 },
    //{ QMediaObject::VideoBitRate, 0 },
    { QMediaObject::VideoCodec, GST_TAG_VIDEO_CODEC },

    //{ QMediaObject::PosterUri, 0 },

    // Movie
    //{ QMediaObject::ChapterNumber, 0 },
    //{ QMediaObject::Director, 0 },
    { QMediaObject::LeadPerformer, GST_TAG_PERFORMER },
    //{ QMediaObject::Writer, 0 },

    // Photos
    //{ QMediaObject::CameraManufacturer, 0 },
    //{ QMediaObject::CameraModel, 0 },
    //{ QMediaObject::Event, 0 },
    //{ QMediaObject::Subject, 0 }
};

QGstreamerCaptureMetaDataControl::QGstreamerCaptureMetaDataControl(QObject *parent)
    :QMetaDataControl(parent)
{
}

QVariant QGstreamerCaptureMetaDataControl::metaData(QMediaObject::MetaData key) const
{
    static const int count = sizeof(qt_gstreamerMetaDataKeys) / sizeof(QGstreamerMetaDataKeyLookup);

    for (int i = 0; i < count; ++i) {
        if (qt_gstreamerMetaDataKeys[i].key == key) {
            const char *name = qt_gstreamerMetaDataKeys[i].token;

            return m_values.value(QByteArray::fromRawData(name, qstrlen(name)));
        }
    }
    return QVariant();
}

void QGstreamerCaptureMetaDataControl::setMetaData(QMediaObject::MetaData key, const QVariant &value)
{
    static const int count = sizeof(qt_gstreamerMetaDataKeys) / sizeof(QGstreamerMetaDataKeyLookup);

    for (int i = 0; i < count; ++i) {
        if (qt_gstreamerMetaDataKeys[i].key == key) {
            const char *name = qt_gstreamerMetaDataKeys[i].token;

            m_values.insert(QByteArray::fromRawData(name, qstrlen(name)), value);

            emit QMetaDataControl::metaDataChanged();
            emit metaDataChanged(m_values);

            return;
        }
    }
}

QVariant QGstreamerCaptureMetaDataControl::extendedMetaData(QString const &name) const
{
    return m_values.value(name.toLatin1());
}

void QGstreamerCaptureMetaDataControl::setExtendedMetaData(QString const &name, QVariant const &value)
{
    m_values.insert(name.toLatin1(), value);
    emit QMetaDataControl::metaDataChanged();
    emit metaDataChanged(m_values);
}
