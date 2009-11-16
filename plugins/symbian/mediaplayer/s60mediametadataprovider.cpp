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

#include "s60mediametadataprovider.h"
#include "s60mediaplayersession.h"
#include <QDebug>
//#include <MmfMeta.h> // mmf defined metadata constants

struct S60VideoMetaDataKeyLookup
{
    QtMedia::MetaData key;
    const char *token;
};

S60MediaMetaDataProvider::S60MediaMetaDataProvider(MS60MediaPlayerResolver& mediaPlayerResolver, QObject *parent)
    : QMetaDataControl(parent),
      m_mediaPlayerResolver(mediaPlayerResolver),
      m_session(NULL)
{
   // connect(m_session, SIGNAL(tagsChanged()), SLOT(updateTags()));
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

QVariant S60MediaMetaDataProvider::metaData(QtMedia::MetaData key) const
{
    m_session = m_mediaPlayerResolver.PlayerSession();
    if (m_session && m_session->isMetadataAvailable())
        return m_session->metaData(metaDataKeyAsString(key));
    return QVariant();
}

void S60MediaMetaDataProvider::setMetaData(QtMedia::MetaData key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}
QList<QtMedia::MetaData> S60MediaMetaDataProvider::availableMetaData() const
{
    // TODO:
}

QVariant S60MediaMetaDataProvider::extendedMetaData(const QString &key) const
{
    return QVariant(); //TODO:
}

void S60MediaMetaDataProvider::setExtendedMetaData(const QString &key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QStringList S60MediaMetaDataProvider::availableExtendedMetaData() const
{
    return QStringList(); // TODO: 
}

QString S60MediaMetaDataProvider::metaDataKeyAsString(QtMedia::MetaData key) const
{
    QString metaDataKey;

    switch(key) {
        case QtMedia::Title: metaDataKey = QString("title"); break;
        case QtMedia::AlbumArtist: metaDataKey = QString("artist"); break;
        case QtMedia::Comment: metaDataKey = QString("comment"); break;
        case QtMedia::Genre: metaDataKey = QString("genre"); break;
        case QtMedia::Year: metaDataKey = QString("year"); break;
        case QtMedia::Copyright: metaDataKey = QString("copyright"); break;
        case QtMedia::AlbumTitle: metaDataKey = QString("album"); break;
        case QtMedia::Composer: metaDataKey = QString("composer"); break;
        case QtMedia::TrackNumber: metaDataKey = QString("albumtrack");
        case QtMedia::SubTitle: break; // TODO: Find the matching metadata keys
        case QtMedia::Description:
        case QtMedia::Category:
        case QtMedia::Date:
        case QtMedia::UserRating:
        case QtMedia::Keywords:
        case QtMedia::Language:
        case QtMedia::Publisher:
        case QtMedia::ParentalRating:
        case QtMedia::RatingOrganisation:       
        case QtMedia::Size:
        case QtMedia::MediaType:
        case QtMedia::Duration:
        case QtMedia::AudioBitrate:
        case QtMedia::AudioCodec:
        case QtMedia::AverageLevel:
        case QtMedia::Channels:
        case QtMedia::PeakValue:
        case QtMedia::Frequency:
        case QtMedia::Author:
        case QtMedia::ContributingArtist:
        case QtMedia::Conductor:
        case QtMedia::Lyrics:
        case QtMedia::Mood:
        case QtMedia::TrackCount:
        case QtMedia::CoverArtUriSmall:
        case QtMedia::CoverArtUriLarge:
        case QtMedia::Resolution:
        case QtMedia::PixelAspectRatio:
        case QtMedia::VideoFrameRate:
        case QtMedia::VideoBitRate:
        case QtMedia::VideoCodec:
        case QtMedia::PosterUri:
        case QtMedia::ChapterNumber:
        case QtMedia::Director:
        case QtMedia::LeadPerformer:
        case QtMedia::Writer:
        case QtMedia::CameraManufacturer:
        case QtMedia::CameraModel:
        case QtMedia::Event:
        case QtMedia::Subject:
        default:
            break;
    }
    
    return metaDataKey;
}
