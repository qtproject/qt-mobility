/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60mediametadataprovider.h"
#include "s60mediaplayersession.h"
#include <QtCore/qdebug.h>

S60MediaMetaDataProvider::S60MediaMetaDataProvider(MS60MediaPlayerResolver& mediaPlayerResolver, QObject *parent)
    : QMetaDataReaderControl(parent)
    , m_mediaPlayerResolver(mediaPlayerResolver)
    , m_session(NULL)
{
}

S60MediaMetaDataProvider::~S60MediaMetaDataProvider()
{
}

bool S60MediaMetaDataProvider::isMetaDataAvailable() const
{
    m_session = m_mediaPlayerResolver.PlayerSession();
    if (m_session)
       return m_session->isMetadataAvailable();
    return false;
}

bool S60MediaMetaDataProvider::isWritable() const
{
    return false;
}

QVariant S60MediaMetaDataProvider::metaData(QtMultimediaKit::MetaData key) const
{
    m_session = m_mediaPlayerResolver.PlayerSession();
    if (m_session && m_session->isMetadataAvailable())
        return m_session->metaData(metaDataKeyAsString(key));
    return QVariant();
}

QList<QtMultimediaKit::MetaData> S60MediaMetaDataProvider::availableMetaData() const
{
    m_session = m_mediaPlayerResolver.PlayerSession();
    QList<QtMultimediaKit::MetaData> metaDataTags;
    if (m_session && m_session->isMetadataAvailable()) {
        for (int i = QtMultimediaKit::Title; i <= QtMultimediaKit::DeviceSettingDescription; i++) {
            QString metaData = metaDataKeyAsString((QtMultimediaKit::MetaData)i);
            if (!metaData.isEmpty()) {
                if (!m_session->metaData(metaData).toString().isEmpty()) {
                    metaDataTags.append((QtMultimediaKit::MetaData)i);
                }
            }
        }
    }
    return metaDataTags;
}

QVariant S60MediaMetaDataProvider::extendedMetaData(const QString &key) const
{
    m_session = m_mediaPlayerResolver.PlayerSession();
    if (m_session && m_session->isMetadataAvailable())
        return m_session->metaData(key);
    return QVariant();
}

QStringList S60MediaMetaDataProvider::availableExtendedMetaData() const
{
    m_session = m_mediaPlayerResolver.PlayerSession();
    if (m_session && m_session->isMetadataAvailable())
        return m_session->availableMetaData().keys();
    return QStringList();
}

QString S60MediaMetaDataProvider::metaDataKeyAsString(QtMultimediaKit::MetaData key) const
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
