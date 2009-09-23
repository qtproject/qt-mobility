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

#include "qgstreamermetadataprovider.h"
#include "qgstreamerplayersession.h"
#include <QDebug>

struct QGstreamerMetaDataKeyLookup
{
    QAbstractMediaObject::MetaData key;
    const char *token;
};

static const QGstreamerMetaDataKeyLookup qt_gstreamerMetaDataKeys[] =
{
    { QAbstractMediaObject::Title, GST_TAG_TITLE },
    //{ QAbstractMediaObject::SubTitle, 0 },
    //{ QAbstractMediaObject::Author, 0 },
    { QAbstractMediaObject::Comment, GST_TAG_COMMENT },
    { QAbstractMediaObject::Description, GST_TAG_DESCRIPTION },
    //{ QAbstractMediaObject::Category, 0 },
    { QAbstractMediaObject::Genre, GST_TAG_GENRE },
    //{ QAbstractMediaObject::Year, 0 },
    //{ QAbstractMediaObject::UserRating, 0 },

    { QAbstractMediaObject::Language, GST_TAG_LANGUAGE_CODE },

    { QAbstractMediaObject::Publisher, GST_TAG_ORGANIZATION },
    { QAbstractMediaObject::Copyright, GST_TAG_COPYRIGHT },
    //{ QAbstractMediaObject::ParentalRating, 0 },
    //{ QAbstractMediaObject::RatingOrganisation, 0 },

    // Media
    //{ QAbstractMediaObject::Size, 0 },
    //{ QAbstractMediaObject::MediaType, 0 },
    { QAbstractMediaObject::Duration, GST_TAG_DURATION },

    // Audio
    { QAbstractMediaObject::AudioBitrate, GST_TAG_BITRATE },
    { QAbstractMediaObject::AudioCodec, GST_TAG_AUDIO_CODEC },
    //{ QAbstractMediaObject::Channels, 0 },
    //{ QAbstractMediaObject::Frequency, 0 },

    // Music
    { QAbstractMediaObject::AlbumTitle, GST_TAG_ALBUM },
    { QAbstractMediaObject::AlbumArtist,  GST_TAG_ARTIST},
    { QAbstractMediaObject::ContributingArtist, GST_TAG_PERFORMER },
    { QAbstractMediaObject::Composer, GST_TAG_COMPOSER },
    //{ QAbstractMediaObject::Conductor, 0 },
    //{ QAbstractMediaObject::Lyrics, 0 },
    //{ QAbstractMediaObject::Mood, 0 },
    { QAbstractMediaObject::TrackNumber, GST_TAG_TRACK_NUMBER },

    //{ QAbstractMediaObject::CoverArtUriSmall, 0 },
    //{ QAbstractMediaObject::CoverArtUriLarge, 0 },

    // Image/Video
    //{ QAbstractMediaObject::Resolution, 0 },
    //{ QAbstractMediaObject::PixelAspectRatio, 0 },

    // Video
    //{ QAbstractMediaObject::FrameRate, 0 },
    //{ QAbstractMediaObject::VideoBitRate, 0 },
    { QAbstractMediaObject::VideoCodec, GST_TAG_VIDEO_CODEC },

    //{ QAbstractMediaObject::PosterUri, 0 },

    // Movie
    //{ QAbstractMediaObject::ChapterNumber, 0 },
    //{ QAbstractMediaObject::Director, 0 },
    { QAbstractMediaObject::LeadPerformer, GST_TAG_PERFORMER },
    //{ QAbstractMediaObject::Writer, 0 },

    // Photos
    //{ QAbstractMediaObject::CameraManufacturer, 0 },
    //{ QAbstractMediaObject::CameraModel, 0 },
    //{ QAbstractMediaObject::Event, 0 },
    //{ QAbstractMediaObject::Subject, 0 }
};

QGstreamerMetaDataProvider::QGstreamerMetaDataProvider(QGstreamerPlayerSession *session, QObject *parent)
    :QMetaDataProviderControl(parent), m_session(session)
{
    connect(m_session, SIGNAL(tagsChanged()), SLOT(updateTags()));
}

QGstreamerMetaDataProvider::~QGstreamerMetaDataProvider()
{
}

bool QGstreamerMetaDataProvider::isMetaDataAvailable() const
{
    return !m_session->tags().isEmpty();
}

bool QGstreamerMetaDataProvider::isWritable() const
{
    return false;
}

QVariant QGstreamerMetaDataProvider::metaData(QAbstractMediaObject::MetaData key) const
{
    static const int count = sizeof(qt_gstreamerMetaDataKeys) / sizeof(QGstreamerMetaDataKeyLookup);

    for (int i = 0; i < count; ++i) {
        if (qt_gstreamerMetaDataKeys[i].key == key) {
            return m_session->tags().value(QByteArray::fromRawData(
                    qt_gstreamerMetaDataKeys[i].token, qstrlen(qt_gstreamerMetaDataKeys[i].token)));
        }
    }
    return QVariant();
}

void QGstreamerMetaDataProvider::setMetaData(QAbstractMediaObject::MetaData key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QVariant QGstreamerMetaDataProvider::extendedMetaData(const QString &key) const
{
    return m_session->tags().value(key.toLatin1());
}

void QGstreamerMetaDataProvider::setExtendedMetaData(const QString &key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}


void QGstreamerMetaDataProvider::updateTags()
{
    emit metaDataChanged();
}
