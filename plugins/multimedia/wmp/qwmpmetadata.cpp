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

#include "qwmpmetadata.h"

#include "qwmpevents.h"
#include "qwmpglobal.h"

#include <qmediacontent.h>

#include <QtCore/qdatetime.h>
#include <QtCore/qdir.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qsize.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>


struct QWmpMetaDataKeyLookup
{
    QtMediaServices::MetaData key;
    const wchar_t *token;
};

static const QWmpMetaDataKeyLookup qt_wmpMetaDataKeys[] =
{
    { QtMediaServices::Title, L"Title" },
    { QtMediaServices::SubTitle, L"WM/SubTitle" },
    { QtMediaServices::Author, L"Author" },
    { QtMediaServices::Comment, L"Comment" },
    { QtMediaServices::Description, L"Description" },
    { QtMediaServices::Category, L"WM/Category" },
    { QtMediaServices::Genre, L"WM/Genre" },
    //{ QtMediaServices::Date, 0 },
    { QtMediaServices::Year, L"WM/Year" },
    { QtMediaServices::UserRating, L"UserRating" },
    //{ QtMediaServices::MetaDatawords, 0 },
    { QtMediaServices::Language, L"Language" },
    { QtMediaServices::Publisher, L"WM/Publisher" },
    { QtMediaServices::Copyright, L"Copyright" },
    { QtMediaServices::ParentalRating, L"ParentalRating" },
    { QtMediaServices::RatingOrganisation, L"RatingOrganisation" },

    // Media
    { QtMediaServices::Size, L"FileSize" },
    { QtMediaServices::MediaType, L"MediaType" },
    { QtMediaServices::Duration, L"Duration" },

    // Audio
    { QtMediaServices::AudioBitRate, L"AudioBitrate" },
    { QtMediaServices::AudioCodec, L"AudioCodec" },
    { QtMediaServices::ChannelCount, L"Channels" },
    { QtMediaServices::SampleRate, L"Frequency" },

    // Music
    { QtMediaServices::AlbumTitle, L"WM/AlbumTitle" },
    { QtMediaServices::AlbumArtist, L"WM/AlbumArtist" },
    { QtMediaServices::ContributingArtist, L"Author" },
    { QtMediaServices::Composer, L"WM/Composer" },
    { QtMediaServices::Conductor, L"WM/Conductor" },
    { QtMediaServices::Lyrics, L"WM/Lyrics" },
    { QtMediaServices::Mood, L"WM/Mood" },
    { QtMediaServices::TrackNumber, L"WM/TrackNumber" },
    //{ QtMediaServices::TrackCount, 0 },
    //{ QtMediaServices::CoverArtUrlSmall, 0 },
    //{ QtMediaServices::CoverArtUrlLarge, 0 },

    // Image/Video
    //{ QtMediaServices::Resolution, 0 },
    //{ QtMediaServices::PixelAspectRatio, 0 },

    // Video
    //{ QtMediaServices::FrameRate, 0 },
    { QtMediaServices::VideoBitRate, L"VideoBitRate" },
    { QtMediaServices::VideoCodec, L"VideoCodec" },

    //{ QtMediaServices::PosterUrl, 0 },

    // Movie
    { QtMediaServices::ChapterNumber, L"ChapterNumber" },
    { QtMediaServices::Director, L"WM/Director" },
    { QtMediaServices::LeadPerformer, L"LeadPerformer" },
    { QtMediaServices::Writer, L"WM/Writer" },

    // Photos
    { QtMediaServices::CameraManufacturer, L"CameraManufacturer" },
    { QtMediaServices::CameraModel, L"CameraModel" },
    { QtMediaServices::Event, L"Event" },
    { QtMediaServices::Subject, L"Subject" }
};

QWmpMetaData::QWmpMetaData(IWMPCore3 *player, QWmpEvents *events, QObject *parent)
    : QMetaDataReaderControl(parent)
    , m_media(0)
{
    player->get_currentMedia(&m_media);

    connect(events, SIGNAL(CurrentItemChange(IDispatch*)),
            this, SLOT(currentItemChangeEvent(IDispatch*)));
    connect(events, SIGNAL(MediaChange(IDispatch*)), this, SLOT(mediaChangeEvent(IDispatch*)));
}

QWmpMetaData::~QWmpMetaData()
{
    if (m_media)
        m_media->Release();
}

bool QWmpMetaData::isMetaDataAvailable() const
{
    return m_media != 0;
}

bool QWmpMetaData::isWritable() const
{
    return m_media != 0;
}

QVariant QWmpMetaData::metaData(QtMediaServices::MetaData key) const
{
    static const int  count = sizeof(qt_wmpMetaDataKeys) / sizeof(QWmpMetaDataKeyLookup);

    switch (key) {
    case QtMediaServices::Date:
        {
            QVariant day = value(m_media, QAutoBStr(L"ReleaseDateDay"));
            QVariant month = value(m_media, QAutoBStr(L"ReleaseDateMonth"));
            QVariant year = value(m_media, QAutoBStr(L"ReleaseDateYear"));

            if (!day.isNull() && !month.isNull() && !year.isNull())
                return QDate(year.toInt(), month.toInt(), day.toInt());
        }
        break;
    case QtMediaServices::CoverArtUrlSmall:
        return albumArtUrl(m_media, "_Small.jpg");
    case QtMediaServices::CoverArtUrlLarge:
        return albumArtUrl(m_media, "_Large.jpg");
    case QtMediaServices::Resolution:
        {
            QVariant width = value(m_media, QAutoBStr(L"WM/VideoWidth"));
            QVariant height = value(m_media, QAutoBStr(L"WM/VideoHeight"));

            if (!width.isNull() && !height.isNull())
                return QSize(width.toInt(), height.toInt());
        }
        break;
    case QtMediaServices::PixelAspectRatio:
        {
            QVariant x = value(m_media, QAutoBStr(L"PixelAspectRatioX"));
            QVariant y = value(m_media, QAutoBStr(L"PixelAspectRatioY"));

            if (!x.isNull() && !y.isNull())
                return QSize(x.toInt(), y.toInt());
        }
        break;
    case QtMediaServices::VideoFrameRate:
        break;
    default:
        for (int i = 0; i < count; ++i) {
            if (qt_wmpMetaDataKeys[i].key == key)
                return value(m_media, QAutoBStr(qt_wmpMetaDataKeys[i].token));
        }
        break;
    }
    return QVariant();
}

QList<QtMediaServices::MetaData> QWmpMetaData::availableMetaData() const
{
    QList<QtMediaServices::MetaData> keys;

    if (m_media) {
        // WMP will return a list of all possible keys so there's no point in filtering the keys
        // in the lookup table.
        static const int  count = sizeof(qt_wmpMetaDataKeys) / sizeof(QWmpMetaDataKeyLookup);
        for (int i = 0; i < count; ++i)
            keys.append(qt_wmpMetaDataKeys[i].key);

        BSTR string = 0;
        if (m_media->get_sourceURL(&string) == S_OK) {
            QString url = QString::fromWCharArray(static_cast<const wchar_t *>(string));
            ::SysFreeString(string);

            if (m_media->getItemInfo(QAutoBStr(L"WM/WMCollectionGroupID"), &string) == S_OK) {
                QString uuid = QString::fromWCharArray(static_cast<const wchar_t *>(string));
                ::SysFreeString(string);

                QString albumArtLarge = QLatin1String("AlbumArt_") + uuid + QLatin1String("_Large.jpg");
                QString albumArtSmall = QLatin1String("AlbumArt_") + uuid + QLatin1String("_Small.jpg");

                QDir dir = QFileInfo(url).absoluteDir();

                if (dir.exists(albumArtLarge))
                    keys.append(QtMediaServices::CoverArtUrlLarge);
                if (dir.exists(albumArtSmall))
                    keys.append(QtMediaServices::CoverArtUrlSmall);
            }
        }
    }
    return keys;
}

QVariant QWmpMetaData::extendedMetaData(const QString &key) const
{
    return value(m_media, QAutoBStr(key));
}

QStringList QWmpMetaData::availableExtendedMetaData() const
{
    return keys(m_media);
}

void QWmpMetaData::currentItemChangeEvent(IDispatch *dispatch)
{
    IWMPMedia *media = m_media;

    m_media = 0;
    if (dispatch)
        dispatch->QueryInterface(__uuidof(IWMPMedia), reinterpret_cast<void **>(&m_media));

    if (media) {
        if (m_media)
            emit metaDataChanged();
        else
            emit metaDataAvailableChanged(false);

        media->Release();
    } else {
        if (m_media)
            emit metaDataAvailableChanged(false);
    }
}

void QWmpMetaData::mediaChangeEvent(IDispatch *dispatch)
{
    IWMPMedia *media = 0;
    if (dispatch &&  dispatch->QueryInterface(
            __uuidof(IWMPMedia), reinterpret_cast<void **>(&media)) == S_OK) {
        VARIANT_BOOL isEqual = VARIANT_FALSE;
        if (media->get_isIdentical(m_media, &isEqual) == S_OK && isEqual)
            emit metaDataChanged();
        media->Release();
    }
}


QStringList QWmpMetaData::keys(IWMPMedia *media)
{
    QStringList keys;

    long count = 0;
    if (media && media->get_attributeCount(&count) == S_OK) {
        for (long i = 0; i < count; ++i) {
            BSTR string;
            if (media->getAttributeName(i, &string) == S_OK) {
                keys.append(QString::fromWCharArray(string, ::SysStringLen(string)));

                ::SysFreeString(string);
            }
        }
    }
    return keys;
}

QVariant QWmpMetaData::value(IWMPMedia *media, BSTR key)
{
    QVariantList values;
    IWMPMedia3 *media3 = 0;
    if (media && media->QueryInterface(
            __uuidof(IWMPMedia3), reinterpret_cast<void **>(&media3)) == S_OK) {
        long count = 0;
        media3->getAttributeCountByType(key, 0, &count);

        // The count appears to only be valid for static properties, dynamic properties like
        // PlaylistIndex will have a count of zero but return a value for index 0.
        if (count == 0)
            count = 1;

        for (long i = 0; i < count; ++i) {
            VARIANT var;
            VariantInit(&var);

            if (media3->getItemInfoByType(key, 0, i, &var) == S_OK) {
                QVariant value = convertVariant(var);

                if (!value.isNull())
                    values.append(value);

                VariantClear(&var);
            }
        }
        media3->Release();
    }

    switch (values.count()) {
    case 0:
        return QVariant();
    case 1:
        return values.first();
    default:
        return values;
    }
}

QMediaContent QWmpMetaData::resources(IWMPMedia *media)
{
    QMediaContent content;

    BSTR string = 0;
    if (media->get_sourceURL(&string) == S_OK) {
        QString url = QString::fromWCharArray(static_cast<const wchar_t *>(string));
        ::SysFreeString(string);

        content = QMediaContent(QUrl(url));
    }

    return content;
}

QVariant QWmpMetaData::convertVariant(const VARIANT &variant)
{
    switch (variant.vt) {
    case VT_I2:
        return variant.iVal;
    case VT_I4:
        return variant.lVal;
    case VT_I8:
        return variant.llVal;
    case VT_UI2:
        return variant.uiVal;
    case VT_UI4:
        return quint32(variant.ulVal);
    case VT_UI8:
        return variant.ullVal;
    case VT_INT:
        return variant.intVal;
    case VT_UINT:
        return variant.uintVal;
    case VT_BSTR:
        return QString::fromWCharArray(variant.bstrVal, ::SysStringLen(variant.bstrVal));
    case VT_DISPATCH:
        {
            IWMPMetadataPicture *picture = 0;
            IWMPMetadataText *text = 0;

            if (variant.pdispVal->QueryInterface(
                    __uuidof(IWMPMetadataPicture), reinterpret_cast<void **>(&picture)) == S_OK) {
                QUrl url;
                BSTR string;
                if (picture->get_URL(&string) == S_OK) {
                    url = QUrl(QString::fromWCharArray(string, ::SysStringLen(string)));

                    ::SysFreeString(string);
                }
                picture->Release();
                return qVariantFromValue(url);
            } else if (variant.pdispVal->QueryInterface(
                    __uuidof(IWMPMetadataText), reinterpret_cast<void **>(&text)) == S_OK) {
                QString description;
                BSTR string;
                if (text->get_description(&string) == S_OK) {
                    description = QString::fromWCharArray(string, SysStringLen(string));

                    ::SysFreeString(string);
                }
                text->Release();
                return description;
            } else {
                qWarning("Unknown dispatch type");
            }
        }
        break;
    default:
        qWarning("Unsupported Type %d %x", variant.vt, variant.vt);
        break;
    }

    return QVariant();
}

QVariant QWmpMetaData::albumArtUrl(IWMPMedia *media, const char *suffix)
{
    BSTR string = 0;
    if (media && media->get_sourceURL(&string) == S_OK) {
        QString url = QString::fromWCharArray(static_cast<const wchar_t *>(string));
        ::SysFreeString(string);

        if (media->getItemInfo(QAutoBStr(L"WM/WMCollectionGroupID"), &string) == S_OK) {
            QString uuid = QString::fromWCharArray(static_cast<const wchar_t *>(string));
            ::SysFreeString(string);

            QString fileName = QLatin1String("AlbumArt_") + uuid + QLatin1String(suffix);

            QDir dir = QFileInfo(url).absoluteDir();

            if (dir.exists(fileName)) {
                return qVariantFromValue(
                        QUrl(QLatin1String("file:///") + dir.absoluteFilePath(fileName)));
            }
        }
    }
    return QVariant();
}
