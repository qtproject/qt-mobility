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

QVariant S60MediaMetaDataProvider::metaData(QtMultimedia::MetaData key) const
{
    m_session = m_mediaPlayerResolver.PlayerSession();
    if (m_session && m_session->isMetadataAvailable())
        return m_session->metaData(metaDataKeyAsString(key));
    return QVariant();
}

QList<QtMultimedia::MetaData> S60MediaMetaDataProvider::availableMetaData() const
{
    m_session = m_mediaPlayerResolver.PlayerSession();
    QList<QtMultimedia::MetaData> metaDataTags;
    if (m_session && m_session->isMetadataAvailable()) {
        for (int i = QtMultimedia::Title; i <= QtMultimedia::DeviceSettingDescription; i++) {
            QString metaData = metaDataKeyAsString((QtMultimedia::MetaData)i);
            if (!metaData.isEmpty()) {
                if (!m_session->metaData(metaData).toString().isEmpty()) {
                    metaDataTags.append((QtMultimedia::MetaData)i);
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

QString S60MediaMetaDataProvider::metaDataKeyAsString(QtMultimedia::MetaData key) const
{
    switch(key) {
        case QtMultimedia::Title: return "title";
        case QtMultimedia::AlbumArtist: return "artist";
        case QtMultimedia::Comment: return "comment";
        case QtMultimedia::Genre: return "genre";
        case QtMultimedia::Year: return "year";
        case QtMultimedia::Copyright: return "copyright";
        case QtMultimedia::AlbumTitle: return "album";
        case QtMultimedia::Composer: return "composer";
        case QtMultimedia::TrackNumber: return "albumtrack";
        case QtMultimedia::AudioBitRate: return "audiobitrate";
        case QtMultimedia::VideoBitRate: return "videobitrate";
        case QtMultimedia::Duration: return "duration";
        case QtMultimedia::MediaType: return "contenttype";
        case QtMultimedia::CoverArtImage: return "attachedpicture";
        case QtMultimedia::SubTitle: // TODO: Find the matching metadata keys
        case QtMultimedia::Description:
        case QtMultimedia::Category:
        case QtMultimedia::Date:
        case QtMultimedia::UserRating:
        case QtMultimedia::Keywords:
        case QtMultimedia::Language:
        case QtMultimedia::Publisher:
        case QtMultimedia::ParentalRating:
        case QtMultimedia::RatingOrganisation:
        case QtMultimedia::Size:
        case QtMultimedia::AudioCodec:
        case QtMultimedia::AverageLevel:
        case QtMultimedia::ChannelCount:
        case QtMultimedia::PeakValue:
        case QtMultimedia::SampleRate:
        case QtMultimedia::Author:
        case QtMultimedia::ContributingArtist:
        case QtMultimedia::Conductor:
        case QtMultimedia::Lyrics:
        case QtMultimedia::Mood:
        case QtMultimedia::TrackCount:
        case QtMultimedia::CoverArtUrlSmall:
        case QtMultimedia::CoverArtUrlLarge:
        case QtMultimedia::Resolution:
        case QtMultimedia::PixelAspectRatio:
        case QtMultimedia::VideoFrameRate:
        case QtMultimedia::VideoCodec:
        case QtMultimedia::PosterUrl:
        case QtMultimedia::ChapterNumber:
        case QtMultimedia::Director:
        case QtMultimedia::LeadPerformer:
        case QtMultimedia::Writer:
        case QtMultimedia::CameraManufacturer:
        case QtMultimedia::CameraModel:
        case QtMultimedia::Event:
        case QtMultimedia::Subject:
        default:
            break;
    }

    return QString();
}
