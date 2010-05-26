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

#include "qgstreamercapturemetadatacontrol.h"

#include <gst/gst.h>
#include <gst/gstversion.h>

struct QGstreamerMetaDataKeyLookup
{
    QtMediaServices::MetaData key;
    const char *token;
};

static const QGstreamerMetaDataKeyLookup qt_gstreamerMetaDataKeys[] =
{
    { QtMediaServices::Title, GST_TAG_TITLE },
    //{ QtMediaServices::SubTitle, 0 },
    //{ QtMediaServices::Author, 0 },
    { QtMediaServices::Comment, GST_TAG_COMMENT },
    { QtMediaServices::Description, GST_TAG_DESCRIPTION },
    //{ QtMediaServices::Category, 0 },
    { QtMediaServices::Genre, GST_TAG_GENRE },
    //{ QtMediaServices::Year, 0 },
    //{ QtMediaServices::UserRating, 0 },

    { QtMediaServices::Language, GST_TAG_LANGUAGE_CODE },

    { QtMediaServices::Publisher, GST_TAG_ORGANIZATION },
    { QtMediaServices::Copyright, GST_TAG_COPYRIGHT },
    //{ QtMediaServices::ParentalRating, 0 },
    //{ QtMediaServices::RatingOrganisation, 0 },

    // Media
    //{ QtMediaServices::Size, 0 },
    //{ QtMediaServices::MediaType, 0 },
    { QtMediaServices::Duration, GST_TAG_DURATION },

    // Audio
    { QtMediaServices::AudioBitRate, GST_TAG_BITRATE },
    { QtMediaServices::AudioCodec, GST_TAG_AUDIO_CODEC },
    //{ QtMediaServices::ChannelCount, 0 },
    //{ QtMediaServices::SampleRate, 0 },

    // Music
    { QtMediaServices::AlbumTitle, GST_TAG_ALBUM },
    { QtMediaServices::AlbumArtist,  GST_TAG_ARTIST},
    { QtMediaServices::ContributingArtist, GST_TAG_PERFORMER },
#if (GST_VERSION_MAJOR >= 0) && (GST_VERSION_MINOR >= 10) && (GST_VERSION_MICRO >= 19)
    { QtMediaServices::Composer, GST_TAG_COMPOSER },
#endif
    //{ QtMediaServices::Conductor, 0 },
    //{ QtMediaServices::Lyrics, 0 },
    //{ QtMediaServices::Mood, 0 },
    { QtMediaServices::TrackNumber, GST_TAG_TRACK_NUMBER },

    //{ QtMediaServices::CoverArtUrlSmall, 0 },
    //{ QtMediaServices::CoverArtUrlLarge, 0 },

    // Image/Video
    //{ QtMediaServices::Resolution, 0 },
    //{ QtMediaServices::PixelAspectRatio, 0 },

    // Video
    //{ QtMediaServices::VideoFrameRate, 0 },
    //{ QtMediaServices::VideoBitRate, 0 },
    { QtMediaServices::VideoCodec, GST_TAG_VIDEO_CODEC },

    //{ QtMediaServices::PosterUrl, 0 },

    // Movie
    //{ QtMediaServices::ChapterNumber, 0 },
    //{ QtMediaServices::Director, 0 },
    { QtMediaServices::LeadPerformer, GST_TAG_PERFORMER },
    //{ QtMediaServices::Writer, 0 },

    // Photos
    //{ QtMediaServices::CameraManufacturer, 0 },
    //{ QtMediaServices::CameraModel, 0 },
    //{ QtMediaServices::Event, 0 },
    //{ QtMediaServices::Subject, 0 }
};

QGstreamerCaptureMetaDataControl::QGstreamerCaptureMetaDataControl(QObject *parent)
    :QMetaDataWriterControl(parent)
{
}

QVariant QGstreamerCaptureMetaDataControl::metaData(QtMediaServices::MetaData key) const
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

void QGstreamerCaptureMetaDataControl::setMetaData(QtMediaServices::MetaData key, const QVariant &value)
{
    static const int count = sizeof(qt_gstreamerMetaDataKeys) / sizeof(QGstreamerMetaDataKeyLookup);

    for (int i = 0; i < count; ++i) {
        if (qt_gstreamerMetaDataKeys[i].key == key) {
            const char *name = qt_gstreamerMetaDataKeys[i].token;

            m_values.insert(QByteArray::fromRawData(name, qstrlen(name)), value);

            emit QMetaDataWriterControl::metaDataChanged();
            emit metaDataChanged(m_values);

            return;
        }
    }
}

QList<QtMediaServices::MetaData> QGstreamerCaptureMetaDataControl::availableMetaData() const
{
    static QMap<QByteArray, QtMediaServices::MetaData> keysMap;
    if (keysMap.isEmpty()) {
        const int count = sizeof(qt_gstreamerMetaDataKeys) / sizeof(QGstreamerMetaDataKeyLookup);
        for (int i = 0; i < count; ++i) {
            keysMap[QByteArray(qt_gstreamerMetaDataKeys[i].token)] = qt_gstreamerMetaDataKeys[i].key;
        }
    }

    QList<QtMediaServices::MetaData> res;
    foreach (const QByteArray &key, m_values.keys()) {
        QtMediaServices::MetaData tag = keysMap.value(key, QtMediaServices::MetaData(-1));
        if (tag != -1)
            res.append(tag);
    }

    return res;
}

QVariant QGstreamerCaptureMetaDataControl::extendedMetaData(QString const &name) const
{
    return m_values.value(name.toLatin1());
}

void QGstreamerCaptureMetaDataControl::setExtendedMetaData(QString const &name, QVariant const &value)
{
    m_values.insert(name.toLatin1(), value);
    emit QMetaDataWriterControl::metaDataChanged();
    emit metaDataChanged(m_values);
}

QStringList QGstreamerCaptureMetaDataControl::availableExtendedMetaData() const
{
    QStringList res;
    foreach (const QByteArray &key, m_values.keys())
        res.append(QString(key));

    return res;
}
