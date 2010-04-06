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

#include "qgallerytrackerschema_p.h"

#include <QtCore/qmetatype.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/qxmlstream.h>

namespace
{
    struct QGalleryPropertyMapItem
    {
        QLatin1String galleryKey;
        QLatin1String trackerKey;
    };

    struct QGalleryPropertyMap
    {
        template <int N> QGalleryPropertyMap(const QGalleryPropertyMapItem (&items)[N])
            : items(items), count(N) {}

        const QGalleryPropertyMapItem *items;
        const int count;
    };

    struct QGalleryAggregatePropertyMapItem
    {
        QLatin1String galleryKey;
        QLatin1String trackerKey;
        QGalleryTrackerSchema::AggregateType type;
    };

    struct QGalleryAggregatePropertyMap
    {
        template <int N> QGalleryAggregatePropertyMap(
                const QGalleryAggregatePropertyMapItem (&items)[N])
            : items(items), count(N) {}

        const QGalleryAggregatePropertyMapItem *items;
        const int count;
    };

    struct QGalleryTypeMapItem
    { 
        QLatin1String galleryKey;
        QLatin1String trackerKey;
        QGalleryPropertyMap propertyMap;
    };

    struct QGalleryTypeMap
    {
        template <int N> QGalleryTypeMap(const QGalleryTypeMapItem (&items)[N])
            : items(items), count(N) {}

        const QGalleryTypeMapItem *items;
        const int count;
    };

    struct QGalleryAggregateTypeMapItem
    {
        QLatin1String galleryKey;
        QLatin1String trackerKey;
        QGalleryPropertyMap identity;
        QGalleryPropertyMap propertyMap;
        QGalleryAggregatePropertyMap aggregatePropertyMap;
        bool (*build)(
                int *error,
                QXmlStreamWriter *xml,
                const QGalleryFilter &filter,
                const QGalleryPropertyMap &propertyMap);
    };

    struct QGalleryAggregateTypeMap
    {
        template <int N> QGalleryAggregateTypeMap(const QGalleryAggregateTypeMapItem (&items)[N])
            : items(items), count(N) {}

        const QGalleryAggregateTypeMapItem *items;
        const int count;
    };
}

static bool qt_writePropertyName(
        int *error,
        QXmlStreamWriter *xml,
        const QString &field,
        const QGalleryPropertyMap &propertyMap)
{
    for (int i = 0; i < propertyMap.count; ++i) {
        if (field == propertyMap.items[i].galleryKey) {
            do {
                xml->writeEmptyElement(QLatin1String("rdfq:Property"));
                xml->writeAttribute(QLatin1String("name"), propertyMap.items[i].trackerKey);
            } while (++i < propertyMap.count && field == propertyMap.items[i].galleryKey);

            return true;
        }
    }

    *error = 1;

    return false;
}

template <typename T>
bool qt_writeValue(int *error, QXmlStreamWriter *xml, const QVariant &value)
{
    if (value.canConvert<QString>()) {
        xml->writeStartElement(QLatin1String("rdf:String"));
        xml->writeCharacters(value.toString());
        xml->writeEndElement();

        return true;
    } else {
        *error = 1;

        return false;
    }
}

template <> bool qt_writeValue<QString>(int *, QXmlStreamWriter *xml, const QVariant &value)
{
    xml->writeStartElement(QLatin1String("rdf:String"));
    xml->writeCharacters(value.toString());
    xml->writeEndElement();

    return true;
}

template <> bool qt_writeValue<int>(int *, QXmlStreamWriter *xml, const QVariant &value)
{
    xml->writeStartElement(QLatin1String("rdf:Integer"));
    xml->writeCharacters(value.toString());
    xml->writeEndElement();

    return true;
}

template <> bool qt_writeValue<qreal>(int *, QXmlStreamWriter *xml, const QVariant &value)
{
    xml->writeStartElement(QLatin1String("rdf:Float"));
    xml->writeCharacters(value.toString());
    xml->writeEndElement();

    return true;
}

template <>
bool qt_writeValue<QDateTime>(int *, QXmlStreamWriter *xml, const QVariant &value)
{
    xml->writeStartElement(QLatin1String("rdf:Date"));
    xml->writeCharacters(value.toString());
    xml->writeEndElement();

    return true;
}

template <>
bool qt_writeValue<QVariant>(int *error, QXmlStreamWriter *xml, const QVariant &value)
{
    switch (value.type()) {
    case QVariant::String:
    case QVariant::Url:
        return qt_writeValue<QString>(error, xml, value);
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::LongLong:
    case QVariant::ULongLong:
        return qt_writeValue<int>(error, xml, value);
    case QMetaType::Float:
    case QVariant::Double:
        return qt_writeValue<qreal>(error, xml, value);
    case QVariant::DateTime:
    case QVariant::Date:
    case QVariant::Time:
        return qt_writeValue<QDateTime>(error, xml, value);
    default:
        if (value.canConvert<QString>()) {
            xml->writeStartElement(QLatin1String("rdf:String"));
            xml->writeCharacters(value.toString());
            xml->writeEndElement();

            return true;
        } else {
            *error = 1;

            return false;
        }
    }
}

static bool qt_writeEqualsCondition(
        QXmlStreamWriter *xml, const QString &propertyName, const QString &value)
{
    xml->writeStartElement(QLatin1String("rdfq:equals"));
    xml->writeEmptyElement(QLatin1String("rdfq:Property"));
    xml->writeAttribute(QLatin1String("name"), propertyName);
    xml->writeStartElement(QLatin1String("rdf:String"));
    xml->writeCharacters(value);
    xml->writeEndElement();
    xml->writeEndElement();

    return true;
}


static bool qt_writeFileCondition(
        int *error, QXmlStreamWriter *xml, const QGalleryItemFilter &filter)
{
    const QStringList itemIds = filter.itemIds();

    if (itemIds.isEmpty()) {
        *error = 1;

        return false;
    } else if (itemIds.count() > 1) {
        xml->writeStartElement(QLatin1String("rdfq:or"));
    }

    for (QStringList::const_iterator id = itemIds.begin(), end = itemIds.end(); id != end; ++id) {
        if (id->startsWith(QLatin1String("file::"))) {
            const int index = id->lastIndexOf(QLatin1Char('/'));

            if (index < 0) {
                *error = 1;

                return false;
            } else {
                xml->writeStartElement(QLatin1String("rdfq:and"));
                qt_writeEqualsCondition(xml, QLatin1String("File:Path"), id->mid(6, index + 1));
                qt_writeEqualsCondition(xml, QLatin1String("File:Name"), id->mid(index + 1));
                xml->writeEndElement();
                return true;
            }
        } else {
            *error = 1;

            return false;
        }
    }

    if (itemIds.count() > 1)
        xml->writeEndElement();

    return true;
}

static bool qt_writeFileCondition(
        int *error, QXmlStreamWriter *xml, const QGalleryItemUrlFilter &filter)
{
    const QList<QUrl> urls = filter.itemUrls();

    for (QList<QUrl>::const_iterator url = urls.begin(), end = urls.end(); url != end; ++url) {
        if (!url->isValid()) {
            *error = 1;

            return false;
        } else if (url->scheme() != QLatin1String("file")) {
            *error = 1;

            return false;
        } else {
            const QString path = url->path();
            const int index = path.lastIndexOf(QLatin1Char('/'));

            if (index < 0) {
                *error = 1;

                return false;
            } else {
                xml->writeStartElement(QLatin1String("rdfq:and"));
                qt_writeEqualsCondition(xml, QLatin1String("File:Path"), path.mid(0, index + 1));
                qt_writeEqualsCondition(xml, QLatin1String("File:Name"), path.mid(index + 1));
                xml->writeEndElement();
            }
        }
    }
    return true;
}

static bool qt_writeArtistCondition(
        int *error, QXmlStreamWriter *xml, const QGalleryItemFilter &filter)
{
    const QStringList itemIds = filter.itemIds();

    if (itemIds.isEmpty()) {
        *error = 1;

        return false;
    }

    for (QStringList::const_iterator itemId = itemIds.begin(), end = itemIds.end();
            itemId != end;
            ++itemId) {
        if (itemId->startsWith(QLatin1String("artist::"))) {
            qt_writeEqualsCondition(xml, QLatin1String("Audio:Artist"), itemId->mid(8));
        } else if (itemId->startsWith(QLatin1String("albumArtist::"))) {
            qt_writeEqualsCondition(xml, QLatin1String("Audio:Artist"), itemId->mid(13));
        } else {
            *error = 1;

            return false;
        }
    }

    return true;
}

static bool qt_writeAlbumCondition(
        int *error, QXmlStreamWriter *xml, const QGalleryItemFilter &filter)
{
    const QStringList itemIds = filter.itemIds();

    if (itemIds.isEmpty()) {
        *error = 1;

        return false;
    }

    for (QStringList::const_iterator itemId = itemIds.begin(), end = itemIds.end();
            itemId != end;
            ++itemId) {
        if (itemId->startsWith(QLatin1String("album::"))) {
            const QLatin1Char separator('/');

            for (int index = itemId->indexOf(separator, 7);
                    index > 0;
                    index = itemId->indexOf(separator, index + 2)) {
                if (itemId->at(index + 1) != separator) {
                    const QString artistName = itemId->mid(7, index - 7).replace(
                            QLatin1String("//"), QLatin1String("/"));
                    const QString albumTitle = itemId->mid(index + 1);

                    xml->writeStartElement(QLatin1String("rdfq:and"));
                    qt_writeEqualsCondition(xml, QLatin1String("Audio:Artist"), artistName);
                    qt_writeEqualsCondition(xml, QLatin1String("Audio:Album"), albumTitle);
                    xml->writeEndElement();
                }

                *error = 1; // Invalid Id;

                return false;
            }
        } else {
            *error = 1;

            return false;
        }
    }
    return true;
}

static bool qt_writeGenreCondition(
        int *error, QXmlStreamWriter *xml, const QGalleryItemFilter &filter)
{
    const QStringList itemIds = filter.itemIds();

    if (itemIds.isEmpty()) {
        *error = 1;

        return false;
    }

    for (QStringList::const_iterator itemId = itemIds.begin(), end = itemIds.end();
            itemId != end;
            ++itemId) {
        if (itemId->startsWith(QLatin1String("genre::"))) {
            qt_writeEqualsCondition(xml, QLatin1String("Audio:Genre"), itemId->mid(7));
        } else {
            *error = 1;

            return false;
        }
    }

    return true;
}

static bool qt_writeCondition(
        int *error,
        QXmlStreamWriter *xml,
        const QGalleryFilter &filter,
        const QGalleryPropertyMap &propertyMap);

static bool qt_writeCondition(
        int *error, QXmlStreamWriter *xml, const QGalleryContainerFilter &filter)
{
    const QString id = filter.containerId();

    if (id.startsWith(QLatin1String("file::"))) {
        qt_writeEqualsCondition(xml, QLatin1String("File:Path"), id.mid(6));

        return true;
    } else if (id.startsWith(QLatin1String("artist::"))) {
        qt_writeEqualsCondition(xml, QLatin1String("Audio:Artist"), id.mid(8));

        return true;
    } else if (id.startsWith(QLatin1String("albumArtist::"))) {
        // Can't differentiate from artist.
        qt_writeEqualsCondition(xml, QLatin1String("Audio:Artist"), id.mid(13));

        return true;
    } else if (id.startsWith(QLatin1String("album::"))) {
        const QLatin1Char separator('/');

        for (int index = id.indexOf(separator, 7);
                index > 0;
                index = id.indexOf(separator, index + 2)) {
            if (id.at(index + 1) != QLatin1Char('/')) {
                const QString artistName = id.mid(7, index - 7).replace(
                        QLatin1String("//"), QLatin1String("/"));
                const QString albumTitle = id.mid(index + 1);

                xml->writeStartElement(QLatin1String("rdfq:and"));
                qt_writeEqualsCondition(xml, QLatin1String("Audio:Artist"), artistName);
                qt_writeEqualsCondition(xml, QLatin1String("Audio:Album"), albumTitle);
                xml->writeEndElement();

                return true;
            }
        }

        *error = 1; // Invalid Id;

        return false;
    } else if (id.startsWith(QLatin1String("genre::"))) {
        qt_writeEqualsCondition(xml, QLatin1String("Audio:Genre"), id.mid(7));

        return true;
    } else {
        *error = 1; // Invalid Id.

        return false;
    }

}

static bool qt_writeCondition(
        int *error, QXmlStreamWriter *xml, const QGalleryContainerUrlFilter &filter)
{
    const QUrl url = filter.containerUrl();

    if (!url.isValid()) {
        *error = 1;

        return false;
    } else if (url.scheme() != QLatin1String("file")) {
        *error = 1;

        return false;
    } else {
        qt_writeEqualsCondition(xml, QLatin1String("File:Path"), url.path());

        return true;
    }
}

static bool qt_writeCondition(
        int *error,
        QXmlStreamWriter *xml,
        const QGalleryIntersectionFilter &filter,
        const QGalleryPropertyMap &propertyMap)
{
    if (!filter.isEmpty()) {
        xml->writeStartElement(QLatin1String("rdfq:or"));

        const QList<QGalleryFilter> filters = filter.filters();
        for (QList<QGalleryFilter>::const_iterator it = filters.begin(), end = filters.end();
                it != end;
                ++it) {
            if (!qt_writeCondition(error, xml, *it, propertyMap)) {
                xml->writeEndElement();

                return false;
            }
        }

        xml->writeEndElement();
    }
    return true;
}

static bool qt_writeCondition(
        int *error,
        QXmlStreamWriter *xml,
        const QGalleryUnionFilter &filter,
        const QGalleryPropertyMap &propertyMap)
{
    if (!filter.isEmpty()) {
        xml->writeStartElement(QLatin1String("rdfq:and"));

        const QList<QGalleryFilter> filters = filter.filters();
        for (QList<QGalleryFilter>::const_iterator it = filters.begin(), end = filters.end();
                it != end;
                ++it) {
            if (!qt_writeCondition(error, xml, *it, propertyMap)) {
                xml->writeEndElement();

                return false;
            }
        }

        xml->writeEndElement();
    }
    return true;
}

static bool qt_writeCondition(
        int *error,
        QXmlStreamWriter *xml,
        const QGalleryMetaDataFilter &filter,
        const QGalleryPropertyMap &propertyMap)
{
    QVariant value = filter.value();

    switch (filter.matchFlags()) {
    case Qt::MatchExactly:
        xml->writeStartElement(QLatin1String("rdfq:equals"));
        break;
    case Qt::MatchContains:
        xml->writeStartElement(QLatin1String("rdfq:contains"));
        break;
    case Qt::MatchStartsWith:
        xml->writeStartElement(QLatin1String("rdfq:startsWith"));
        break;
    case Qt::MatchEndsWith:
        xml->writeStartElement(QLatin1String("rdfq:contains"));
        value = QLatin1Char('*') + value.toString();
        break;
    case Qt::MatchRegExp:
        xml->writeStartElement(QLatin1String("rdfq:regex"));
        break;
    default:
        *error = 1;

        return false;
    }

    if (qt_writePropertyName(error, xml, filter.fieldName(), propertyMap)
            && qt_writeValue<QVariant>(error, xml, value)) {
        xml->writeEndElement();

        return true;
    } else {
        xml->writeEndElement();

        return false;
    }
}

static bool qt_writeCondition(
        int *error,
        QXmlStreamWriter *xml,
        const QGalleryMetaDataRangeFilter &filter,
        const QGalleryPropertyMap &propertyMap)
{
    const QString fieldName = filter.fieldName();
    const QGalleryFilter::RangeFlags flags = filter.rangeFlags();

    bool isRange = (flags == QGalleryFilter::InclusiveRange || QGalleryFilter::ExclusiveRange);

    if (isRange)
        xml->writeStartElement(QLatin1String("rdfq:and"));

    if (flags & QGalleryFilter::GreaterThanEqualsMinimum) {
        if (flags & QGalleryFilter::EqualsMinimum)
            xml->writeStartElement(QLatin1String("rdfq:greaterThanEqual"));
        else
            xml->writeStartElement(QLatin1String("rdfq:greaterThan"));

        if (!qt_writePropertyName(error, xml, fieldName, propertyMap)
                || !qt_writeValue<QVariant>(error, xml, filter.minimumValue())) {
            xml->writeEndElement();
            if (isRange)
                xml->writeEndElement();

            return false;
        }
    }

    if (flags & QGalleryFilter::LessThanEqualsMaximum) {
        if (flags & QGalleryFilter::EqualsMaximum)
            xml->writeStartElement(QLatin1String("rdfq:lessThanEqual"));
        else
            xml->writeStartElement(QLatin1String("rdfq:lessThan"));

        if (!qt_writePropertyName(error, xml, fieldName, propertyMap)
                || !qt_writeValue<QVariant>(error, xml, filter.maximumValue())) {
            xml->writeEndElement();
            if (isRange)
                xml->writeEndElement();

            return false;
        }
    }

    if (isRange)
        xml->writeEndElement();

    return true;
}

static bool qt_writeCondition(
        int *error,
        QXmlStreamWriter *xml,
        const QGalleryFilter &filter,
        const QGalleryPropertyMap &propertyMap)
{
    switch (filter.type()) {
    case QGalleryFilter::Invalid:
        return true;
    case QGalleryFilter::Container:
        return qt_writeCondition(error, xml, filter.toContainerFilter());
    case QGalleryFilter::ContainerUrl:
        return qt_writeCondition(error, xml, filter.toContainerUrlFilter());
    case QGalleryFilter::Intersection:
        return qt_writeCondition(
                error, xml, filter.toIntersectionFilter(), propertyMap);
    case QGalleryFilter::Union:
        return qt_writeCondition(error, xml, filter.toUnionFilter(), propertyMap);
    case QGalleryFilter::MetaData:
        return qt_writeCondition(error, xml, filter.toMetaDataFilter(), propertyMap);
    case QGalleryFilter::MetaDataRange:
        return qt_writeCondition(
                error, xml, filter.toMetaDataRangeFilter(), propertyMap);
    default:
        *error = 1;

        return false;
    }
}

static bool qt_buildFileQuery(
        int *error,
        QXmlStreamWriter *xml,
        const QGalleryFilter &filter,
        const QGalleryPropertyMap &propertyMap)
{
    switch (filter.type()) {
    case QGalleryFilter::Item:
        return qt_writeFileCondition(error, xml, filter.toItemFilter());
    case QGalleryFilter::ItemUrl:
        return qt_writeFileCondition(error, xml, filter.toItemUrlFilter());
    default:
        return qt_writeCondition(error, xml, filter, propertyMap);
    }
}

static bool qt_buildArtistQuery(
        int *error,
        QXmlStreamWriter *xml,
        const QGalleryFilter &filter,
        const QGalleryPropertyMap &propertyMap)
{
    switch (filter.type()) {
    case QGalleryFilter::Item:
        return qt_writeArtistCondition(error, xml, filter.toItemFilter());
    case QGalleryFilter::ItemUrl:
        *error = 1;

        return false;
    default:
        return qt_writeCondition(error, xml, filter, propertyMap);
    }
}

static bool qt_buildAlbumQuery(
        int *error,
        QXmlStreamWriter *xml,
        const QGalleryFilter &filter,
        const QGalleryPropertyMap &propertyMap)
{
    switch (filter.type()) {
    case QGalleryFilter::Item:
        return qt_writeAlbumCondition(error, xml, filter.toItemFilter());
    case QGalleryFilter::ItemUrl:
        *error = 1;

        return false;
    default:
        return qt_writeCondition(error, xml, filter, propertyMap);
    }
}

static bool qt_buildGenreQuery(
        int *error,
        QXmlStreamWriter *xml,
        const QGalleryFilter &filter,
        const QGalleryPropertyMap &propertyMap)
{
    switch (filter.type()) {
    case QGalleryFilter::Item:
        return qt_writeGenreCondition(error, xml, filter.toItemFilter());
    case QGalleryFilter::ItemUrl:
        *error = 1;

        return false;
    default:
        return qt_writeCondition(error, xml, filter, propertyMap);
    }
}

#define QT_GALLERY_PROPERTY(GalleryKey, TrackerKey) \
{ QLatin1String(GalleryKey), QLatin1String(TrackerKey) }

// These are repeated in a few lists.
#define QT_GALLERY_FILE_PROPERTYS \
    QT_GALLERY_PROPERTY("", "File:Contents"), \
    QT_GALLERY_PROPERTY("", "File:Link"), \
    QT_GALLERY_PROPERTY("mimeType", "File:Mime"), \
    QT_GALLERY_PROPERTY("", "File:Size"), \
    QT_GALLERY_PROPERTY("", "File:License"), \
    QT_GALLERY_PROPERTY("copyright", "File:Copyright"), \
    QT_GALLERY_PROPERTY("", "File:Modified"), \
    QT_GALLERY_PROPERTY("", "File:Accessed"), \
    QT_GALLERY_PROPERTY("", "File:Added"), \
    QT_GALLERY_PROPERTY("", "File:Other"), \
    QT_GALLERY_PROPERTY("", "File:Language")

static const QGalleryPropertyMapItem qt_galleryFilePropertyMap[] =
{
    QT_GALLERY_FILE_PROPERTYS,
//    QT_GALLERY_PROPERTY(""           , "DC:Contributor"),
//    QT_GALLERY_PROPERTY(""           , "DC:Coverage"),
    QT_GALLERY_PROPERTY("author"     , "DC:Creator"),
//    QT_GALLERY_PROPERTY(""           , "DC:Date"),
    QT_GALLERY_PROPERTY("description", "DC:Description"),
//    QT_GALLERY_PROPERTY(""           , "DC:Format"),
//    QT_GALLERY_PROPERTY(""           , "DC:Identifier"),
//    QT_GALLERY_PROPERTY(""           , "DC:Language"),
//    QT_GALLERY_PROPERTY(""           , "DC:Publisher"),
//    QT_GALLERY_PROPERTY(""           , "DC:Relation"),
//    QT_GALLERY_PROPERTY(""           , "DC:Rights"),
//    QT_GALLERY_PROPERTY(""           , "DC:Source"),
//    QT_GALLERY_PROPERTY(""           , "DC:Subject"),
//    QT_GALLERY_PROPERTY(""           , "DC:Keywords"),
    QT_GALLERY_PROPERTY("title"      , "DC:Title"),
//    QT_GALLERY_PROPERTY(""           , "DC:Type"),
    QT_GALLERY_PROPERTY("rating"     , "User:Rank") //,
//    QT_GALLERY_PROPERTY(""           , "User:Keywords")
};

static const QGalleryPropertyMapItem qt_galleryAudioPropertyMap[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_PROPERTY("title", "Audio:Title"),
    QT_GALLERY_PROPERTY("artist", "Audio:Artist"),
    QT_GALLERY_PROPERTY("albumTitle", "Audio:Album"),
    QT_GALLERY_PROPERTY("genre", "Audio:Genre"),
    QT_GALLERY_PROPERTY("duration", "Audio:Duration"),
//    QT_GALLERY_PROPERTY("", "Audio:ReleaseDate"),
    QT_GALLERY_PROPERTY("albumArtist", "Audio:AlbumArtist"),
    QT_GALLERY_PROPERTY("trackNumber", "Audio:TrackNo"),
    QT_GALLERY_PROPERTY("discNumber", "Audio:DiscNo"),
//    QT_GALLERY_PROPERTY("", "Audio:Performer"),
//    QT_GALLERY_PROPERTY("", "Audio:TrackGain"),
//    QT_GALLERY_PROPERTY("", "Audio:AlbumGain"),
//    QT_GALLERY_PROPERTY("", "Audio:AlbumPeakGain"),
    QT_GALLERY_PROPERTY("description", "Audio:Comment") //,
//    QT_GALLERY_PROPERTY("", "Audio:Codec"),
//    QT_GALLERY_PROPERTY("", "Audio:CodecVersion"),
//    QT_GALLERY_PROPERTY("", "Audio:SampleRate"),
//    QT_GALLERY_PROPERTY("", "Audio:Bitrate"),
//    QT_GALLERY_PROPERTY("", "Audio:Channels"),
//    QT_GALLERY_PROPERTY("", "Audio:LastPlay"),
//    QT_GALLERY_PROPERTY("", "Audio:PlayCount"),
//    QT_GALLERY_PROPERTY("", "Audio:DateAdded"),
//    QT_GALLERY_PROPERTY("", "Audio:Lyrics")
};

static const QGalleryPropertyMapItem qt_galleryPlaylistPropertyMap[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_PROPERTY("duration", "Playlist:Duration"),
    QT_GALLERY_PROPERTY("trackCount", "Playlist:Songs") //,
//    QT_GALLERY_PROPERTY("", "Playlist:Name"),
//    QT_GALLERY_PROPERTY("", "Playlist:ValidDuration")
};
static const QGalleryPropertyMapItem qt_galleryImagePropertyMap[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_PROPERTY("title"             , "Image:Title"),
//    QT_GALLERY_PROPERTY(""                  , "Image:Keywords"),
    QT_GALLERY_PROPERTY("height"            , "Image:Height"),
    QT_GALLERY_PROPERTY("width"             , "Image:Width"),
    QT_GALLERY_PROPERTY("album"             , "Image:Album"),
    QT_GALLERY_PROPERTY("dateTaken"         , "Image:Date"),
    QT_GALLERY_PROPERTY("author"            , "Image:Creator"),
//    QT_GALLERY_PROPERTY(""                  , "Image:Comments"),
    QT_GALLERY_PROPERTY("description"       , "Image:Description"),
//    QT_GALLERY_PROPERTY(""                  , "Image:Software"),
    QT_GALLERY_PROPERTY("cameraManufacturer", "Image:CameraMake"),
    QT_GALLERY_PROPERTY("cameraModel"       , "Image:CameraModel") //,
//    QT_GALLERY_PROPERTY(""                  , "Image:Orientation"),
//    QT_GALLERY_PROPERTY(""                  , "Image:ExposureProgram"),
//    QT_GALLERY_PROPERTY(""                  , "Image:ExposureTime"),
//    QT_GALLERY_PROPERTY(""                  , "Image:FNumber"),
//    QT_GALLERY_PROPERTY(""                  , "Image:Flash"),
//    QT_GALLERY_PROPERTY(""                  , "Image:FocalLength"),
//    QT_GALLERY_PROPERTY(""                  , "Image:ISOSpeed"),
//    QT_GALLERY_PROPERTY(""                  , "Image:MeteringMode"),
//    QT_GALLERY_PROPERTY(""                  , "Image:WhiteBalance"),
//    QT_GALLERY_PROPERTY(""                  , "Image:Rating"),
//    QT_GALLERY_PROPERTY(""                  , "Image:Location"),
//    QT_GALLERY_PROPERTY(""                  , "Image:Sublocation"),
//    QT_GALLERY_PROPERTY(""                  , "Image:Country"),
//    QT_GALLERY_PROPERTY(""                  , "Image:City"),
//    QT_GALLERY_PROPERTY(""                  , "Image:HasKeywords")
};

static const QGalleryPropertyMapItem qt_galleryVideoPropertyMap[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_PROPERTY("title"      , "Video:Title"),
    QT_GALLERY_PROPERTY("author"     , "Video:Author"),
    QT_GALLERY_PROPERTY("height"     , "Video:Height"),
    QT_GALLERY_PROPERTY("width"      , "Video:Width"),
    QT_GALLERY_PROPERTY("duration"   , "Video:Duration"),
    QT_GALLERY_PROPERTY("description", "Video:Comments") //,
//    QT_GALLERY_PROPERTY(""           , "Video:FrameRate"),
//    QT_GALLERY_PROPERTY(""           , "Video:Codec"),
//    QT_GALLERY_PROPERTY(""           , "Video:Bitrate"),
//    QT_GALLERY_PROPERTY(""           , "Video:PlayCount"),
//    QT_GALLERY_PROPERTY(""           , "Video:PausePosition"),
//    QT_GALLERY_PROPERTY(""           , "Video:LastPlayedFrame"),
//    QT_GALLERY_PROPERTY(""           , "Video:Source")
};

static const QGalleryPropertyMapItem qt_galleryApplicationPropertyMap[] =
{
    QT_GALLERY_FILE_PROPERTYS //,
//    QT_GALLERY_PROPERTY("", "App:Name"),
//    QT_GALLERY_PROPERTY("", "App:DisplayName"),
//    QT_GALLERY_PROPERTY("", "App:GenericName"),
//    QT_GALLERY_PROPERTY("", "App:Comment"),
//    QT_GALLERY_PROPERTY("", "App:Exec"),
//    QT_GALLERY_PROPERTY("", "App:Icon"),
//    QT_GALLERY_PROPERTY("", "App:MimeType"),
//    QT_GALLERY_PROPERTY("", "App:Categories")
};

static const QGalleryPropertyMapItem qt_galleryDocumentPropertyMap[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_PROPERTY("title", "Doc:Title"),
//    QT_GALLERY_PROPERTY("", "Doc:Subject"),
    QT_GALLERY_PROPERTY("author", "Doc:Author") //,
//    QT_GALLERY_PROPERTY("", "Doc:Keywords"),
//    QT_GALLERY_PROPERTY("", "Doc:Comments"),
//    QT_GALLERY_PROPERTY("", "Doc:PageCount"),
//    QT_GALLERY_PROPERTY("", "Doc:WordCount"),
//    QT_GALLERY_PROPERTY("", "Doc:Creator"),
//    QT_GALLERY_PROPERTY("", "Doc:URL")
};

#define QT_GALLERY_FILE_TYPE(GalleryKey, TrackerKey, PropertyMap) \
{ QLatin1String(GalleryKey), QLatin1String(TrackerKey), QGalleryPropertyMap(PropertyMap) }

static const QGalleryTypeMapItem qt_galleryTypeMap[] =
{
    QT_GALLERY_FILE_TYPE("File"     , "Files"        , qt_galleryFilePropertyMap),
    QT_GALLERY_FILE_TYPE("Folder"   , "Folders"      , qt_galleryFilePropertyMap),
    QT_GALLERY_FILE_TYPE("Document" , "Documents"    , qt_galleryDocumentPropertyMap),
//    QT_GALLERY_TYPE("Media"    , ""             , qt_galleryMediaPropertyMap),
    QT_GALLERY_FILE_TYPE("Audio"    , "Music"        , qt_galleryAudioPropertyMap),
    QT_GALLERY_FILE_TYPE("Image"    , "Images"       , qt_galleryImagePropertyMap),
    QT_GALLERY_FILE_TYPE("Video"    , "Videos"       , qt_galleryVideoPropertyMap),
    QT_GALLERY_FILE_TYPE("File"     , "Playlists"    , qt_galleryPlaylistPropertyMap),
    QT_GALLERY_FILE_TYPE("File"     , "Applications" , qt_galleryApplicationPropertyMap),
    QT_GALLERY_FILE_TYPE("File"     , "Text"         , qt_galleryFilePropertyMap),
    QT_GALLERY_FILE_TYPE("File"     , "Development"  , qt_galleryFilePropertyMap),
    QT_GALLERY_FILE_TYPE("File"     , "Other"        , qt_galleryFilePropertyMap),
};


#define QT_GALLERY_AGGREGATE_PROPERTY(GalleryKey, TrackerKey, AggregateType) \
{ QLatin1String(GalleryKey), QLatin1String(TrackerKey), QGalleryTrackerSchema::AggregateType }

static const QGalleryPropertyMapItem qt_galleryArtistIdentity[] =
{
    QT_GALLERY_PROPERTY("title", "Audio:Artist")
};

static const QGalleryPropertyMapItem qt_galleryArtistPropertyMap[] =
{
    QT_GALLERY_PROPERTY("artist", "Audio:Artist"),
    QT_GALLERY_PROPERTY("title" , "Audio:Artist"),
};

static const QGalleryAggregatePropertyMapItem qt_galleryArtistAggregateMap[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", Sum),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , Count),
};

static const QGalleryPropertyMapItem qt_galleryAlbumIdentity[] =
{
    QT_GALLERY_PROPERTY("title", "Audio:Album"),
    QT_GALLERY_PROPERTY("artist", "Audio:Artist")
};

static const QGalleryPropertyMapItem qt_galleryAlbumPropertyMap[] =
{
    QT_GALLERY_PROPERTY("albumTitle", "Audio:Album"),
    QT_GALLERY_PROPERTY("title"     , "Audio:Album"),
    QT_GALLERY_PROPERTY("artist"    , "Audio:Artist")
};

static const QGalleryAggregatePropertyMapItem qt_galleryAlbumAggregateMap[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", Sum),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , Count),
};

static const QGalleryPropertyMapItem qt_galleryGenreIdentity[] =
{
    QT_GALLERY_PROPERTY("title", "Audio:Genre")
};

static const QGalleryPropertyMapItem qt_galleryGenrePropertyMap[] =
{
    QT_GALLERY_PROPERTY("genre", "Audio:Genre"),
    QT_GALLERY_PROPERTY("title", "Audio:Genre"),
};

static const QGalleryAggregatePropertyMapItem qt_galleryGenreAggregateMap[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", Sum),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , Count),
};

#define QT_GALLERY_AGGREGATE_TYPE(Type, Service) \
{ \
    QLatin1String(#Type), \
    QLatin1String(#Service), \
    QGalleryPropertyMap(qt_gallery##Type##Identity), \
    QGalleryPropertyMap(qt_gallery##Type##PropertyMap),\
    QGalleryAggregatePropertyMap(qt_gallery##Type##AggregateMap), \
    qt_build##Type##Query \
}

static const QGalleryAggregateTypeMapItem qt_galleryAggregateTypeMap[] =
{
//    QT_GALLERY_AGGREGATE_TYPE(Location, Folders),
    QT_GALLERY_AGGREGATE_TYPE(Artist  , Music),
    QT_GALLERY_AGGREGATE_TYPE(Album   , Music),
    QT_GALLERY_AGGREGATE_TYPE(Genre   , Music)
};

QGalleryTrackerSchema::QGalleryTrackerSchema()
    : m_fileTypeIndex(-1)
    , m_aggregateTypeIndex(-1)
{
}

QGalleryTrackerSchema::~QGalleryTrackerSchema()
{
}

void QGalleryTrackerSchema::setItemType(const QString &type)
{
    m_itemType = type;
    m_fileTypeIndex = -1;
    m_aggregateTypeIndex = -1;

    {
        QGalleryTypeMap typeMap(qt_galleryTypeMap);

        for (int i = 0; i < typeMap.count; ++i) {
            if (typeMap.items[i].galleryKey == m_itemType) {
                m_fileTypeIndex = i;
                return;
            }
        }
    } {
        QGalleryAggregateTypeMap typeMap(qt_galleryAggregateTypeMap);

        for (int i = 0; i < typeMap.count; ++i) {
            if (typeMap.items[i].galleryKey == m_itemType) {
                m_aggregateTypeIndex = i;
                return;
            }
        }
    }
}

QString QGalleryTrackerSchema::buildQuery(int *error, const QGalleryFilter &filter) const
{
    QString query;

    if (filter.type() != QGalleryFilter::Invalid) {
        if (m_fileTypeIndex >= 0) {
            QXmlStreamWriter xml(&query);

            xml.writeStartElement(QLatin1String("rdfq:Condition"));

            qt_buildFileQuery(error, &xml, filter, qt_galleryTypeMap[m_fileTypeIndex].propertyMap);

            xml.writeEndElement();
        } else if (m_aggregateTypeIndex >= 0) {

            QXmlStreamWriter xml(&query);

            xml.writeStartElement(QLatin1String("rdfq:Condition"));

            qt_galleryAggregateTypeMap[m_aggregateTypeIndex].build(
                    error, &xml, filter, qt_galleryAggregateTypeMap[m_aggregateTypeIndex].propertyMap);

            xml.writeEndElement();
        } else {
            *error = 1;
        }
    }

    return query;
}

QString QGalleryTrackerSchema::service() const
{
    if (m_fileTypeIndex >= 0)
        return qt_galleryTypeMap[m_fileTypeIndex].trackerKey;
    else if (m_aggregateTypeIndex >= 0)
        return qt_galleryAggregateTypeMap[m_aggregateTypeIndex].trackerKey;
    else
        return QString();
}

QString QGalleryTrackerSchema::field(const QString &propertyName) const
{
    if (m_fileTypeIndex >= 0) {
        const QGalleryPropertyMap propertyMap = qt_galleryTypeMap[m_fileTypeIndex].propertyMap;

        for (int i = 0; i < propertyMap.count; ++i) {
            if (propertyName == propertyMap.items[i].galleryKey)
                return propertyMap.items[i].trackerKey;
        }
    } else if (m_aggregateTypeIndex >= 0) {
        const QGalleryPropertyMap propertyMap
                = qt_galleryAggregateTypeMap[m_aggregateTypeIndex].propertyMap;

        for (int i = 0; i < propertyMap.count; ++i) {
            if (propertyName == propertyMap.items[i].galleryKey)
                return propertyMap.items[i].trackerKey;
        }
    }
    return QString();
}

QStringList QGalleryTrackerSchema::identityFields() const
{
    QStringList fields;

    if (m_aggregateTypeIndex >= 0) {
        const QGalleryPropertyMap identity
                = qt_galleryAggregateTypeMap[m_aggregateTypeIndex].identity;

        for (int i = 0; i < identity.count; ++i)
            fields.append(identity.items[i].trackerKey);
    }
    return fields;
}

QStringList QGalleryTrackerSchema::identityPropertyNames() const
{
    QStringList propertyNames;

    if (m_aggregateTypeIndex >= 0) {
        const QGalleryPropertyMap identity
                = qt_galleryAggregateTypeMap[m_aggregateTypeIndex].identity;

        for (int i = 0; i < identity.count; ++i)
            propertyNames.append(identity.items[i].galleryKey);
    }
    return propertyNames;
}

QPair<QString, QGalleryTrackerSchema::AggregateType> QGalleryTrackerSchema::aggregateField(
        const QString &propertyName) const
{
    if (m_aggregateTypeIndex >= 0) {
        const QGalleryAggregatePropertyMap propertyMap
                = qt_galleryAggregateTypeMap[m_aggregateTypeIndex].aggregatePropertyMap;

        for (int i = 0; i < propertyMap.count; ++i) {
            if (propertyName == propertyMap.items[i].galleryKey) {
                return QPair<QString, AggregateType>(
                        propertyMap.items[i].trackerKey, propertyMap.items[i].type);
            }
        }
    }
    return qMakePair(QString(), Count);
}

QString QGalleryTrackerSchema::typeFromService(const QString &service)
{
    QGalleryTypeMap typeMap(qt_galleryTypeMap);

    for (int i = 0; i < typeMap.count; ++i) {
        if (typeMap.items[i].trackerKey == service)
            return typeMap.items[i].galleryKey;
    }
    return QString();
}
