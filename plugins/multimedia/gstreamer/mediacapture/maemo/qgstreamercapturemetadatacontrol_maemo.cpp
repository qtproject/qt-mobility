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

#include "qgstreamercapturemetadatacontrol_maemo.h"

#include <gst/gst.h>
#include <gst/gstversion.h>

struct QGstreamerMetaDataKeyLookup
{
    QtMedia::MetaData key;
    const char *token;
};

static const QGstreamerMetaDataKeyLookup qt_gstreamerMetaDataKeys[] =
{
    { QtMedia::Title, GST_TAG_TITLE },
    //{ QtMedia::SubTitle, 0 },
    //{ QtMedia::Author, 0 },
    { QtMedia::Comment, GST_TAG_COMMENT },
    { QtMedia::Description, GST_TAG_DESCRIPTION },
    //{ QtMedia::Category, 0 },
    { QtMedia::Genre, GST_TAG_GENRE },
    //{ QtMedia::Year, 0 },
    //{ QtMedia::UserRating, 0 },

    { QtMedia::Language, GST_TAG_LANGUAGE_CODE },

    { QtMedia::Publisher, GST_TAG_ORGANIZATION },
    { QtMedia::Copyright, GST_TAG_COPYRIGHT },
    //{ QtMedia::ParentalRating, 0 },
    //{ QtMedia::RatingOrganisation, 0 },

    // Media
    //{ QtMedia::Size, 0 },
    //{ QtMedia::MediaType, 0 },
    { QtMedia::Duration, GST_TAG_DURATION },

    // Audio
    { QtMedia::AudioBitRate, GST_TAG_BITRATE },
    { QtMedia::AudioCodec, GST_TAG_AUDIO_CODEC },
    //{ QtMedia::ChannelCount, 0 },
    //{ QtMedia::SampleRate, 0 },

    // Music
    { QtMedia::AlbumTitle, GST_TAG_ALBUM },
    { QtMedia::AlbumArtist,  GST_TAG_ARTIST},
    { QtMedia::ContributingArtist, GST_TAG_PERFORMER },
#if (GST_VERSION_MAJOR >= 0) && (GST_VERSION_MINOR >= 10) && (GST_VERSION_MICRO >= 19)
    { QtMedia::Composer, GST_TAG_COMPOSER },
#endif
    //{ QtMedia::Conductor, 0 },
    //{ QtMedia::Lyrics, 0 },
    //{ QtMedia::Mood, 0 },
    { QtMedia::TrackNumber, GST_TAG_TRACK_NUMBER },

    //{ QtMedia::CoverArtUrlSmall, 0 },
    //{ QtMedia::CoverArtUrlLarge, 0 },

    // Image/Video
    //{ QtMedia::Resolution, 0 },
    //{ QtMedia::PixelAspectRatio, 0 },

    // Video
    //{ QtMedia::VideoFrameRate, 0 },
    //{ QtMedia::VideoBitRate, 0 },
    { QtMedia::VideoCodec, GST_TAG_VIDEO_CODEC },

    //{ QtMedia::PosterUrl, 0 },

    // Movie
    //{ QtMedia::ChapterNumber, 0 },
    //{ QtMedia::Director, 0 },
    { QtMedia::LeadPerformer, GST_TAG_PERFORMER },
    //{ QtMedia::Writer, 0 },

    // Photos
    //{ QtMedia::CameraManufacturer, 0 },
    //{ QtMedia::CameraModel, 0 },
    //{ QtMedia::Event, 0 },
    //{ QtMedia::Subject, 0 }
};

QGstreamerCaptureMetaDataControl::QGstreamerCaptureMetaDataControl(QObject *parent)
    :QMetaDataControl(parent)
{
}

QVariant QGstreamerCaptureMetaDataControl::metaData(QtMedia::MetaData key) const
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

void QGstreamerCaptureMetaDataControl::setMetaData(QtMedia::MetaData key, const QVariant &value)
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

QList<QtMedia::MetaData> QGstreamerCaptureMetaDataControl::availableMetaData() const
{
    static QMap<QByteArray, QtMedia::MetaData> keysMap;
    if (keysMap.isEmpty()) {
        const int count = sizeof(qt_gstreamerMetaDataKeys) / sizeof(QGstreamerMetaDataKeyLookup);
        for (int i = 0; i < count; ++i) {
            keysMap[QByteArray(qt_gstreamerMetaDataKeys[i].token)] = qt_gstreamerMetaDataKeys[i].key;
        }
    }

    QList<QtMedia::MetaData> res;
    foreach (const QByteArray &key, m_values.keys()) {
        QtMedia::MetaData tag = keysMap.value(key, QtMedia::MetaData(-1));
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
    emit QMetaDataControl::metaDataChanged();
    emit metaDataChanged(m_values);
}

QStringList QGstreamerCaptureMetaDataControl::availableExtendedMetaData() const
{
    QStringList res;
    foreach (const QByteArray &key, m_values.keys())
        res.append(QString(key));

    return res;
}
