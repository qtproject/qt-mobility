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

#include "qgalleryabstractrequest.h"
#include "qgallerytrackerlistcolumn_p.h"

#include <QtCore/qmetatype.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/qxmlstream.h>

QTM_BEGIN_NAMESPACE

namespace
{
    struct QGalleryPropertyMapItem
    {
        QLatin1String galleryKey;
        QLatin1String trackerKey;
        QGalleryProperty::Attributes attributes;
    };

    struct QGalleryPropertyMap
    {
        template <int N> QGalleryPropertyMap(const QGalleryPropertyMapItem (&items)[N])
            : items(items), count(N) {}

        const QGalleryPropertyMapItem *items;
        const int count;

        int indexOfProperty(const QString &propertyName) const;

        const QGalleryPropertyMapItem &operator [](int index) const { return items[index]; }
    };

    struct QGalleryAggregatePropertyMapItem
    {
        QLatin1String galleryKey;
        QLatin1String trackerKey;
        QGalleryTrackerSchema::AggregateType type;
        QLatin1String aggregate;
    };

    struct QGalleryAggregatePropertyMap
    {
        template <int N> QGalleryAggregatePropertyMap(
                const QGalleryAggregatePropertyMapItem (&items)[N])
            : items(items), count(N) {}

        const QGalleryAggregatePropertyMapItem *items;
        const int count;

        int indexOfProperty(const QString &propertyName) const;

        const QGalleryAggregatePropertyMapItem &operator [](int index) const {
            return items[index]; }
    };

    struct QGalleryTypePrefix : public QLatin1String
    {
        template <int N> QGalleryTypePrefix(const char (&prefix)[N])
            : QLatin1String(prefix), length(N - 1){}

        const int length;

        QStringRef strip(const QString &id) const {
            return QStringRef(&id, length, id.length() - length); }
    };

    struct QGalleryTypeMapItem
    { 
        QLatin1String galleryKey;
        QLatin1String trackerKey;
        QGalleryTypePrefix prefix;
        QGalleryPropertyMap propertyMap;
        QGalleryTrackerSchema::IdFunc idFunc;
    };

    struct QGalleryTypeMap
    {
        template <int N> QGalleryTypeMap(const QGalleryTypeMapItem (&items)[N])
            : items(items), count(N) {}

        const QGalleryTypeMapItem *items;
        const int count;

        int indexOfType(const QString &type) const;
        int indexOfItemId(const QString &itemId) const;

        const QGalleryTypeMapItem &operator [](int index) const { return items[index]; }
    };

    struct QGalleryAggregateTypeMapItem
    {
        QLatin1String galleryKey;
        QLatin1String trackerKey;
        QGalleryTypePrefix prefix;
        QGalleryPropertyMap identity;
        QGalleryPropertyMap propertyMap;
        QGalleryAggregatePropertyMap aggregatePropertyMap;
        void (*writeIdCondition)(int *error, QXmlStreamWriter *xml, const QStringRef &itemId);
        QGalleryTrackerSchema::IdFunc idFunc;
    };

    struct QGalleryAggregateTypeMap
    {
        template <int N> QGalleryAggregateTypeMap(const QGalleryAggregateTypeMapItem (&items)[N])
            : items(items), count(N) {}

        const QGalleryAggregateTypeMapItem *items;
        const int count;

        int indexOfType(const QString &type) const;
        int indexOfItemId(const QString &itemId) const;

        const QGalleryAggregateTypeMapItem &operator [](int index) const { return items[index]; }
    };

    int QGalleryPropertyMap::indexOfProperty(const QString &propertyName) const
    {
        for (int i = 0; i < count; ++i) {
            if (items[i].galleryKey == propertyName)
                return i;
        }
        return -1;
    }

    int QGalleryAggregatePropertyMap::indexOfProperty(const QString &propertyName) const
    {
        for (int i = 0; i < count; ++i) {
            if (items[i].galleryKey == propertyName)
                return i;
        }
        return -1;
    }

    int QGalleryTypeMap::indexOfType(const QString &type) const
    {
        for (int i = 0; i < count; ++i) {
            if (items[i].galleryKey == type)
                return i;
        }
        return -1;
    }

    int QGalleryTypeMap::indexOfItemId(const QString &itemId) const
    {
        for (int i = 0; i < count; ++i) {
            if (itemId.startsWith(items[i].prefix))
                return i;
        }
        return -1;
    }

    int QGalleryAggregateTypeMap::indexOfType(const QString &type) const
    {
        for (int i = 0; i < count; ++i) {
            if (items[i].galleryKey == type)
                return i;
        }
        return -1;
    }

    int QGalleryAggregateTypeMap::indexOfItemId(const QString &itemId) const
    {
        for (int i = 0; i < count; ++i) {
            if (itemId.startsWith(items[i].prefix))
                return i;
        }
        return -1;
    }

    // Re-declare to cut down on prefixes.
    enum
    {
        CanRead = QGalleryProperty::CanRead,
        CanWrite = QGalleryProperty::CanWrite,
        CanSort = QGalleryProperty::CanSort,
        CanFilter = QGalleryProperty::CanFilter
    };

    class QGalleryReorderLessThan
    {
    public:
        QGalleryReorderLessThan(const QStringList &sortOrder)
            : m_sortOrder(QStringList(sortOrder)
                    .replaceInStrings(QLatin1String("+"), QString())
                    .replaceInStrings(QLatin1String("-"), QString()))
        {
        }

        bool operator ()(const QString &property1, const QString &property2) const
        {
            uint index1 = m_sortOrder.indexOf(property1);
            uint index2 = m_sortOrder.indexOf(property2);

            return index1 < index2;
        }

    private:
        QStringList m_sortOrder;
    };
}

#define QT_GALLERY_PROPERTY(GalleryKey, TrackerKey, Attr) \
{ QLatin1String(GalleryKey), QLatin1String(TrackerKey), QGalleryProperty::Attributes(Attr) }

#define QT_GALLERY_AGGREGATE_PROPERTY(GalleryKey, TrackerKey, AggregateType, Aggregate) \
{ QLatin1String(GalleryKey), QLatin1String(TrackerKey), QGalleryTrackerSchema::AggregateType, QLatin1String(Aggregate) }

#define QT_GALLERY_FILE_TYPE(Type, Service, Prefix, PropertyMap) \
{ \
    QLatin1String(#Type), \
    QLatin1String(#Service), \
    QGalleryTypePrefix(#Prefix"::"), \
    QGalleryPropertyMap(PropertyMap), \
    qt_gallery##Type##Id \
}

#define QT_GALLERY_AGGREGATE_TYPE(Type, Service, Prefix) \
{ \
    QLatin1String(#Type), \
    QLatin1String(#Service), \
    QGalleryTypePrefix(#Prefix"::"), \
    QGalleryPropertyMap(qt_gallery##Type##Identity), \
    QGalleryPropertyMap(qt_gallery##Type##PropertyMap),\
    QGalleryAggregatePropertyMap(qt_gallery##Type##AggregateMap), \
    qt_write##Type##IdCondition, \
    qt_gallery##Type##Id \
}

static bool qt_writePropertyName(
        int *error,
        QXmlStreamWriter *xml,
        const QString &field,
        const QGalleryPropertyMap &propertyMap)
{
    for (int i = 0; i < propertyMap.count; ++i) {
        if (field == propertyMap[i].galleryKey) {
            do {
                xml->writeEmptyElement(QLatin1String("rdfq:Property"));
                xml->writeAttribute(QLatin1String("name"), propertyMap[i].trackerKey);
            } while (++i < propertyMap.count && field == propertyMap[i].galleryKey);

            return true;
        }
    }

    *error = QGalleryAbstractRequest::InvalidPropertyError;

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
        *error = QGalleryAbstractRequest::PropertyTypeError;

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
            *error = QGalleryAbstractRequest::PropertyTypeError;

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

static bool qt_writeCondition(
        int *error,
        QXmlStreamWriter *xml,
        const QGalleryFilter &filter,
        const QGalleryPropertyMap &propertyMap);

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
        *error = QGalleryAbstractRequest::UnsupportedFilterOptionError;

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
        *error = QGalleryAbstractRequest::UnsupportedFilterTypeError;

        return false;
    }
}

///////
// File
///////

// These are repeated in a few lists.
#define QT_GALLERY_FILE_PROPERTYS \
    QT_GALLERY_PROPERTY("fileName"    , "File:Name"     , CanRead | CanSort | CanFilter), \
  /*QT_GALLERY_PROPERTY(""            , "File:Contents" , QGalleryProperty::Attributes()),*/ \
  /*QT_GALLERY_PROPERTY(""            , "File:Link"     , QGalleryProperty::Attributes()),*/ \
    QT_GALLERY_PROPERTY("mimeType"    , "File:Mime"     , CanRead | CanSort | CanFilter), \
  /*QT_GALLERY_PROPERTY(""            , "File:Size"     , QGalleryProperty::Attributes()),*/ \
  /*QT_GALLERY_PROPERTY(""            , "File:License"  , QGalleryProperty::Attributes()),*/ \
    QT_GALLERY_PROPERTY("copyright"   , "File:Copyright", CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_PROPERTY("lastModified", "File:Modified" , CanRead | CanSort | CanFilter), \
    QT_GALLERY_PROPERTY("lastAccessed", "File:Accessed" , CanRead | CanSort | CanFilter), \
  /*QT_GALLERY_PROPERTY(""            , "File:Added"    , QGalleryProperty::Attributes()),*/ \
  /*QT_GALLERY_PROPERTY(""            , "File:Other"    , QGalleryProperty::Attributes()),*/ \
    QT_GALLERY_PROPERTY("language"    , "File:Language" , CanRead | CanSort | CanFilter)

static const QGalleryPropertyMapItem qt_galleryFilePropertyMap[] =
{
    QT_GALLERY_FILE_PROPERTYS,
//    QT_GALLERY_PROPERTY(""         , "DC:Contributor", QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""         , "DC:Coverage"   , QGalleryProperty::Attributes()),
    QT_GALLERY_PROPERTY("author"     , "DC:Creator"    , CanRead | CanSort | CanFilter),
//    QT_GALLERY_PROPERTY(""         , "DC:Date"       , QGalleryProperty::Attributes()),
    QT_GALLERY_PROPERTY("description", "DC:Description", CanRead | CanSort | CanFilter),
//    QT_GALLERY_PROPERTY(""         , "DC:Format"     , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""         , "DC:Identifier" , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""         , "DC:Language"   , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""         , "DC:Publisher"  , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""         , "DC:Relation"   , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""         , "DC:Rights"     , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""         , "DC:Source"     , QGalleryProperty::Attributes()),
    QT_GALLERY_PROPERTY("subject"    , "DC:Subject"    , CanRead | CanSort | CanFilter),
//    QT_GALLERY_PROPERTY(""         , "DC:Keywords"   , QGalleryProperty::Attributes()),
    QT_GALLERY_PROPERTY("title"      , "DC:Title"      , CanRead | CanSort | CanFilter),
//    QT_GALLERY_PROPERTY(""         , "DC:Type"       , QGalleryProperty::Attributes()),
    QT_GALLERY_PROPERTY("rating"     , "User:Rank"     , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("keywords"   , "User:Keywords" , CanRead | CanSort | CanFilter)
};

static QString qt_galleryFileId(const QStringList &row);

static void qt_writeFileIdCondition(int *error, QXmlStreamWriter *xml, const QStringRef &itemId)
{
    const int separatorIndex = itemId.string()->lastIndexOf(QLatin1Char('/'));
    
    if (separatorIndex > 0) {
        const QString path = itemId.string()->mid(
                itemId.position(), separatorIndex - itemId.position());
        QString fileName = itemId.string()->mid(separatorIndex + 1);
        
        xml->writeStartElement(QLatin1String("rdfq:and"));
        qt_writeEqualsCondition(xml, QLatin1String("File:Path"), path);
        qt_writeEqualsCondition(xml, QLatin1String("File:Name"), fileName);
        xml->writeEndElement();
    } else {
        *error = QGalleryAbstractRequest::InvalidItemError;
    }
}

static void qt_writeFileContainerCondition(
        int *, QXmlStreamWriter *xml, const QStringRef &itemId)
{
    qt_writeEqualsCondition(xml, QLatin1String("File:Path"), itemId.toString());
}

static void qt_writeFileScopeCondition(int *, QXmlStreamWriter *xml, const QStringRef &itemId)
{
    xml->writeStartElement(QLatin1String("rdfq:or"));

    qt_writeEqualsCondition(xml, QLatin1String("File:Path"), itemId.toString());

    xml->writeStartElement(QLatin1String("rdfq:startsWith"));
    xml->writeEmptyElement(QLatin1String("rdfq:Property"));
    xml->writeAttribute(QLatin1String("name"), QLatin1String("File:Path"));

    xml->writeStartElement(QLatin1String("rdf:String"));
    xml->writeCharacters(itemId.toString());
    xml->writeEndElement();

    xml->writeEndElement();

    xml->writeEndElement();
}

/////////
// Folder
/////////

static QString qt_galleryFolderId(const QStringList &row)
{
    return QLatin1String("folder::") + row.value(0);
}

////////
// Audio
////////

static const QGalleryPropertyMapItem qt_galleryAudioPropertyMap[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_PROPERTY("title"       , "Audio:Title"        , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("artist"      , "Audio:Artist"       , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("albumTitle"  , "Audio:Album"        , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("genre"       , "Audio:Genre"        , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("duration"    , "Audio:Duration"     , CanRead | CanSort | CanFilter),
//    QT_GALLERY_PROPERTY(""          , "Audio:ReleaseDate"  , QGalleryProperty::Attributes()),
    QT_GALLERY_PROPERTY("albumArtist" , "Audio:AlbumArtist"  , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("trackNumber" , "Audio:TrackNo"      , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("discNumber"  , "Audio:DiscNo"       , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("performer"   , "Audio:Performer"    , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_PROPERTY(""          , "Audio:TrackGain"    , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""          , "Audio:AlbumGain"    , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""          , "Audio:AlbumPeakGain", QGalleryProperty::Attributes()),
    QT_GALLERY_PROPERTY("description" , "Audio:Comment"      , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("audioCodec"  , "Audio:Codec"        , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY(""            , "Audio:CodecVersion" , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("sampleRate"  , "Audio:SampleRate"   , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("audioBitRate", "Audio:Bitrate"      , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("channelCount", "Audio:Channels"     , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("lastPlayed"  , "Audio:LastPlay"     , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("playCount"   , "Audio:PlayCount"    , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_PROPERTY(""          , "Audio:DateAdded"    , QGalleryProperty::Attributes()),
    QT_GALLERY_PROPERTY("lyrics"      , "Audio:Lyrics"       , CanRead | CanWrite | CanSort | CanFilter)
};

static QString qt_galleryAudioId(const QStringList &row)
{
    return QLatin1String("audio::") + row.value(0);
}

///////////
// Playlist
///////////

static const QGalleryPropertyMapItem qt_galleryPlaylistPropertyMap[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_PROPERTY("duration"  , "Playlist:Duration"     , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("trackCount", "Playlist:Songs"        , CanRead | CanSort | CanFilter)
//    QT_GALLERY_PROPERTY(""        , "Playlist:Name"         , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""        , "Playlist:ValidDuration", QGalleryProperty::Attributes())
};

static QString qt_galleryPlaylistId(const QStringList &row)
{
    return QLatin1String("playlist::") + row.value(0);
}

////////
// Image
////////

static const QGalleryPropertyMapItem qt_galleryImagePropertyMap[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_PROPERTY("title"             , "Image:Title"          , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("keywords"          , "Image:Keywords"       , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("height"            , "Image:Height"         , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("width"             , "Image:Width"          , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("album"             , "Image:Album"          , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("dateTaken"         , "Image:Date"           , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("author"            , "Image:Creator"        , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("comment"           , "Image:Comments"       , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("description"       , "Image:Description"    , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_PROPERTY(""                , "Image:Software"       , QGalleryProperty::Attributes()),
    QT_GALLERY_PROPERTY("cameraManufacturer", "Image:CameraMake"     , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("cameraModel"       , "Image:CameraModel"    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("orientation"       , "Image:Orientation"    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("exposureProgram"   , "Image:ExposureProgram", CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("exposureTime"      , "Image:ExposureTime"   , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("fNumber"           , "Image:FNumber"        , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("flashEnabled"      , "Image:Flash"          , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("focalLength"       , "Image:FocalLength"    , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_PROPERTY(""                , "Image:ISOSpeed"       , QGalleryProperty::Attributes()),
    QT_GALLERY_PROPERTY("meteringMode"      , "Image:MeteringMode"   , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("whiteBalance"      , "Image:WhiteBalance"   , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("rating"            , "Image:Rating"         , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_PROPERTY(""                , "Image:Location"       , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""                , "Image:Sublocation"    , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""                , "Image:Country"        , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""                , "Image:City"           , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""                , "Image:HasKeywords"    , QGalleryProperty::Attributes())
};

static QString qt_galleryImageId(const QStringList &row)
{
    return QLatin1String("image::") + row.value(0);
}

////////
// Video
////////

static const QGalleryPropertyMapItem qt_galleryVideoPropertyMap[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_PROPERTY("title"         , "Video:Title"          , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("author"        , "Video:Author"         , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("height"        , "Video:Height"         , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("width"         , "Video:Width"          , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("duration"      , "Video:Duration"       , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("comments"      , "Video:Comments"       , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("frameRate"     , "Video:FrameRate"      , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("videoCodec"    , "Video:Codec"          , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("videoBitRate"  , "Video:Bitrate"        , CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("playCount"     , "Video:PlayCount"      , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("resumePosition", "Video:PausePosition"  , CanRead | CanWrite | CanSort | CanFilter)
//    QT_GALLERY_PROPERTY(""            , "Video:LastPlayedFrame", QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""            , "Video:Source"         , QGalleryProperty::Attributes())
};

static QString qt_galleryVideoId(const QStringList &row)
{
    return QLatin1String("video::") + row.value(0);
}

///////////
// Document
///////////

static const QGalleryPropertyMapItem qt_galleryDocumentPropertyMap[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_PROPERTY("title"    , "Doc:Title"    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("subject"  , "Doc:Subject"  , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("author"   , "Doc:Author"   , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("keywords" , "Doc:Keywords" , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("comment"  , "Doc:Comments" , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("pageCount", "Doc:PageCount", CanRead | CanSort | CanFilter),
    QT_GALLERY_PROPERTY("wordCount", "Doc:WordCount", CanRead | CanSort | CanFilter)
//    QT_GALLERY_PROPERTY(""       , "Doc:Creator"  , QGalleryProperty::Attributes()),
//    QT_GALLERY_PROPERTY(""       , "Doc:URL"      , QGalleryProperty::Attributes())
};

static QString qt_galleryDocumentId(const QStringList &row)
{
    return QLatin1String("document::") + row.value(0);
}

///////
// Text
///////

static QString qt_galleryTextId(const QStringList &row)
{
    return QLatin1String("text::") + row.value(0);
}

/////////////
// File Types
/////////////

static const QGalleryTypeMapItem qt_galleryTypeMap[] =
{
    QT_GALLERY_FILE_TYPE(File       , Files        , file    , qt_galleryFilePropertyMap),
    QT_GALLERY_FILE_TYPE(Folder     , Folders      , folder  , qt_galleryFilePropertyMap),
    QT_GALLERY_FILE_TYPE(Document   , Documents    , document, qt_galleryDocumentPropertyMap),
    QT_GALLERY_FILE_TYPE(Audio      , Music        , audio   , qt_galleryAudioPropertyMap),
    QT_GALLERY_FILE_TYPE(Image      , Images       , image   , qt_galleryImagePropertyMap),
    QT_GALLERY_FILE_TYPE(Video      , Videos       , video   , qt_galleryVideoPropertyMap),
    QT_GALLERY_FILE_TYPE(Playlist   , Playlists    , playlist, qt_galleryPlaylistPropertyMap),
    QT_GALLERY_FILE_TYPE(Text       , Text         , text    , qt_galleryFilePropertyMap),
    QT_GALLERY_FILE_TYPE(File       , Development  , file    , qt_galleryFilePropertyMap),
    QT_GALLERY_FILE_TYPE(File       , Other        , file    , qt_galleryFilePropertyMap),
};

static QString qt_galleryFileId(const QStringList &row)
{
    QGalleryTypeMap typeMap(qt_galleryTypeMap);

    QString service = row.value(1);

    for (int i = 0; i < typeMap.count; ++i) {
        if (typeMap[i].trackerKey == service) {
            return typeMap[i].prefix + row.value(0);
        }
    }
    return typeMap[0].prefix + row.value(0);
}

/////////
// Artist
/////////

static const QGalleryPropertyMapItem qt_galleryArtistIdentity[] =
{
    QT_GALLERY_PROPERTY("title", "Audio:Artist", CanRead | CanFilter)
};

static const QGalleryPropertyMapItem qt_galleryArtistPropertyMap[] =
{
    QT_GALLERY_PROPERTY("artist", "Audio:Artist", CanRead | CanFilter),
    QT_GALLERY_PROPERTY("title" , "Audio:Artist", CanRead | CanFilter),
};

static const QGalleryAggregatePropertyMapItem qt_galleryArtistAggregateMap[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", Sum  , "SUM"),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , Count, "COUNT"),
};

static QString qt_galleryArtistId(const QStringList &rows)
{
    return QLatin1String("artist::") + rows.value(0);
}

static void qt_writeArtistIdCondition(int *, QXmlStreamWriter *xml, const QStringRef &itemId)
{
    qt_writeEqualsCondition(xml, QLatin1String("Audio:Artist"), itemId.toString());
}

///////////////
// Album Artist
///////////////

static const QGalleryPropertyMapItem qt_galleryAlbumArtistIdentity[] =
{
    QT_GALLERY_PROPERTY("title", "Audio:AlbumArtist", CanRead | CanFilter)
};

static const QGalleryPropertyMapItem qt_galleryAlbumArtistPropertyMap[] =
{
    QT_GALLERY_PROPERTY("artist", "Audio:AlbumArtist", CanRead | CanFilter),
    QT_GALLERY_PROPERTY("title" , "Audio:AlbumArtist", CanRead | CanFilter),
};

static const QGalleryAggregatePropertyMapItem qt_galleryAlbumArtistAggregateMap[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", Sum  , "SUM"),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , Count, "COUNT"),
};

static QString qt_galleryAlbumArtistId(const QStringList &rows)
{
    return QLatin1String("albumArtist::") + rows.value(0);
}

static void qt_writeAlbumArtistIdCondition(int *, QXmlStreamWriter *xml, const QStringRef &itemId)
{
    qt_writeEqualsCondition(xml, QLatin1String("Audio:AlbumArtist"), itemId.toString());
}

////////
// Album
////////

static const QGalleryPropertyMapItem qt_galleryAlbumIdentity[] =
{
    QT_GALLERY_PROPERTY("artist", "Audio:AlbumArtist", CanRead | CanFilter),
    QT_GALLERY_PROPERTY("title" , "Audio:Album"      , CanRead | CanFilter)
};

static const QGalleryPropertyMapItem qt_galleryAlbumPropertyMap[] =
{
    QT_GALLERY_PROPERTY("title" , "Audio:Album"      , CanRead | CanFilter),
    QT_GALLERY_PROPERTY("artist", "Audio:AlbumArtist", CanRead | CanFilter)
};

static const QGalleryAggregatePropertyMapItem qt_galleryAlbumAggregateMap[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", Sum  , "SUM"),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , Count, "COUNT"),
};

static QString qt_galleryAlbumId(const QStringList &row)
{
    QString albumTitle = row.value(0);
    QString artistName = row.value(1);

    return QLatin1String("album::")
            + artistName.replace(QLatin1String("/"), QLatin1String("//"))
            + QLatin1Char('/')
            + albumTitle;
}

static void qt_writeAlbumIdCondition(
        int *error, QXmlStreamWriter *xml, const QStringRef &itemId)
{
    const QLatin1Char separator('/');

    for (int index = itemId.string()->indexOf(separator, itemId.position());
            index > 0;
            index = itemId.string()->indexOf(separator, index + 2)) {
        if (index + 1 == itemId.string()->length() || itemId.string()->at(index + 1) != separator) {
            QString artistName = itemId.string()->mid(itemId.position(), index - itemId.position());
            artistName = artistName.replace(QLatin1String("//"), QLatin1String("/"));

            const QString albumTitle = itemId.string()->mid(index + 1);

            xml->writeStartElement(QLatin1String("rdfq:and"));
            qt_writeEqualsCondition(xml, QLatin1String("Audio:AlbumArtist"), artistName);
            qt_writeEqualsCondition(xml, QLatin1String("Audio:Album"), albumTitle);
            xml->writeEndElement();

            return;
        }
    }

    *error = QGalleryAbstractRequest::InvalidItemError;
}

//////////////
// Music Genre
//////////////

static const QGalleryPropertyMapItem qt_galleryAudioGenreIdentity[] =
{
    QT_GALLERY_PROPERTY("title", "Audio:Genre", CanRead | CanFilter)
};

static const QGalleryPropertyMapItem qt_galleryAudioGenrePropertyMap[] =
{
    QT_GALLERY_PROPERTY("genre", "Audio:Genre", CanRead | CanFilter),
    QT_GALLERY_PROPERTY("title", "Audio:Genre", CanRead | CanFilter),
};

static const QGalleryAggregatePropertyMapItem qt_galleryAudioGenreAggregateMap[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", Sum  , "SUM"),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , Count, "COUNT"),
};

static QString qt_galleryAudioGenreId(const QStringList &row)
{
    return QLatin1String("audioGenre::") + row.value(0);
}

static void qt_writeAudioGenreIdCondition(int *, QXmlStreamWriter *xml, const QStringRef &itemId)
{
    qt_writeEqualsCondition(xml, QLatin1String("Audio:Genre"), itemId.toString());
}

//////////////
// Photo Album
//////////////

static const QGalleryPropertyMapItem qt_galleryPhotoAlbumIdentity[] =
{
    QT_GALLERY_PROPERTY("title", "Image:Album", CanRead | CanFilter)
};

static const QGalleryPropertyMapItem qt_galleryPhotoAlbumPropertyMap[] =
{
    QT_GALLERY_PROPERTY("title", "Image:Album", CanRead | CanFilter),
};

static const QGalleryAggregatePropertyMapItem qt_galleryPhotoAlbumAggregateMap[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*", Count, "COUNT")
};

static QString qt_galleryPhotoAlbumId(const QStringList &row)
{
    return  QLatin1String("photoAlbum::") + row.value(0);
}

static void qt_writePhotoAlbumIdCondition(int *, QXmlStreamWriter *xml, const QStringRef &itemId)
{
    qt_writeEqualsCondition(xml, QLatin1String("Image:Album"), itemId.toString());
}

//////////////////
// Aggregate Types
//////////////////

static const QGalleryAggregateTypeMapItem qt_galleryAggregateTypeMap[] =
{
    QT_GALLERY_AGGREGATE_TYPE(Artist     , Music , artist),
    QT_GALLERY_AGGREGATE_TYPE(AlbumArtist, Music , albumArtist),
    QT_GALLERY_AGGREGATE_TYPE(Album      , Music , album),
    QT_GALLERY_AGGREGATE_TYPE(AudioGenre , Music , audioGenre),
    QT_GALLERY_AGGREGATE_TYPE(PhotoAlbum , Images, photoAlbum)

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
    m_fileTypeIndex = QGalleryTypeMap(qt_galleryTypeMap).indexOfType(type);
    m_aggregateTypeIndex = m_fileTypeIndex == -1
            ? QGalleryAggregateTypeMap(qt_galleryAggregateTypeMap).indexOfType(type)
            : -1;
}

void QGalleryTrackerSchema::resolveItemType(const QString &itemId)
{
    QGalleryTypeMap typeMap(qt_galleryTypeMap);
    m_fileTypeIndex = typeMap.indexOfItemId(itemId);

    if (m_fileTypeIndex != -1) {
        m_itemType = typeMap[m_fileTypeIndex].galleryKey;
        m_aggregateTypeIndex = -1;
    } else {
        QGalleryAggregateTypeMap aggregateMap(qt_galleryAggregateTypeMap);
        m_aggregateTypeIndex = aggregateMap.indexOfItemId((itemId));

        if (m_aggregateTypeIndex != -1) {
            m_itemType = aggregateMap[m_aggregateTypeIndex].galleryKey;
        } else {
            m_itemType = QString();
        }
    }
}

QString QGalleryTrackerSchema::buildIdQuery(int *error, const QString &itemId)
{
    QString query;

    if (m_fileTypeIndex >= 0) {
        QXmlStreamWriter xml(&query);
        xml.writeStartElement(QLatin1String("rdfq:Condition"));

        qt_writeFileIdCondition(
                error ,&xml, qt_galleryTypeMap[m_fileTypeIndex].prefix.strip(itemId));

        xml.writeEndElement();
    } else if (m_aggregateTypeIndex >= 0) {
        QXmlStreamWriter xml(&query);
        xml.writeStartElement(QLatin1String("rdfq:Condition"));

        qt_galleryAggregateTypeMap[m_aggregateTypeIndex].writeIdCondition(
                error, &xml, qt_galleryAggregateTypeMap[m_aggregateTypeIndex].prefix.strip(itemId));

        xml.writeEndElement();
    } else {
        *error = QGalleryAbstractRequest::InvalidItemError;
    }

    return query;
}

QString QGalleryTrackerSchema::buildContainerQuery(int *error, const QString &containerId)
{
    QString query;

    QGalleryTypeMap typeMap(qt_galleryTypeMap);
    int index = typeMap.indexOfItemId(containerId);

    if (index != -1) {
        QXmlStreamWriter xml(&query);
        xml.writeStartElement(QLatin1String("rdfq:Condition"));

        qt_writeFileContainerCondition(error, &xml, typeMap[index].prefix.strip(containerId));

        xml.writeEndElement();
    } else {
        QGalleryAggregateTypeMap aggregateMap(qt_galleryAggregateTypeMap);
        index = aggregateMap.indexOfItemId(containerId);

        if (index != -1) {
            QXmlStreamWriter xml(&query);
            xml.writeStartElement(QLatin1String("rdfq:Condition"));

            aggregateMap[index].writeIdCondition(
                    error, &xml, aggregateMap[index].prefix.strip(containerId));

            xml.writeEndElement();
        } else {
            *error = QGalleryAbstractRequest::InvalidItemError;
        }
    }

    return query;
}

QString QGalleryTrackerSchema::buildFilterQuery(
        int *error, const QString &containerId, const QGalleryFilter &filter) const
{    
    QString query;

    if (containerId.isEmpty() && filter.type() == QGalleryFilter::Invalid)
        return query;

    QXmlStreamWriter xml(&query);
    xml.writeStartElement(QLatin1String("rdfq:Condition"));

    if (!containerId.isEmpty()) {
        xml.writeStartElement(QLatin1String("rdfq:and"));

        QGalleryTypeMap typeMap(qt_galleryTypeMap);
        int index = typeMap.indexOfItemId(containerId);

        if (index != -1) {
            qt_writeFileScopeCondition(error, &xml, typeMap[index].prefix.strip(containerId));
        } else {
            QGalleryAggregateTypeMap aggregateMap(qt_galleryAggregateTypeMap);
            index = aggregateMap.indexOfItemId((containerId));

            if (index != -1) {
                aggregateMap[index].writeIdCondition(
                        error, &xml, aggregateMap[index].prefix.strip(containerId));
            } else {
                *error = QGalleryAbstractRequest::InvalidItemError;
            }
        }
    }

    if (filter.type() != QGalleryFilter::Invalid) {
        if (m_fileTypeIndex >= 0) {
            qt_writeCondition(
                    error,
                    &xml,
                    filter,
                    qt_galleryTypeMap[m_fileTypeIndex].propertyMap);
        } else if (m_aggregateTypeIndex >= 0) {
            qt_writeCondition(
                    error,
                    &xml,
                    filter,
                    qt_galleryAggregateTypeMap[m_aggregateTypeIndex].propertyMap);
        } else {
            *error = QGalleryAbstractRequest::ItemTypeError;
        }
    }

    if (!containerId.isEmpty())
        xml.writeEndElement();

    xml.writeEndElement();

    return query;
}

QString QGalleryTrackerSchema::uriFromItemId(int *error, const QVariant &itemId) const
{
    QGalleryTypeMap typeMap(qt_galleryTypeMap);

    const QString idString = itemId.toString();

    int index = typeMap.indexOfItemId(idString);

    if (index != -1) {
        return typeMap[index].prefix.strip(idString).toString();
    } else {
        *error = QGalleryAbstractRequest::InvalidItemError;

        return QString();
    }
}

QStringList QGalleryTrackerSchema::urisFromItemIds(int *error, const QVariantList &itemIds) const
{
    QStringList uris;

    QGalleryTypeMap typeMap(qt_galleryTypeMap);

    for (QVariantList::const_iterator it = itemIds.begin(), end = itemIds.end();
            it != end;
            ++it) {
        const QString itemId = it->toString();

        int index = typeMap.indexOfItemId(itemId);

        if (index != -1)
            uris.append(typeMap[index].prefix.strip(itemId).toString());
        else
            *error = QGalleryAbstractRequest::InvalidItemError;
    }

    return uris;
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
            if (propertyName == propertyMap[i].galleryKey)
                return propertyMap[i].trackerKey;
        }
    } else if (m_aggregateTypeIndex >= 0) {
        const QGalleryPropertyMap propertyMap
                = qt_galleryAggregateTypeMap[m_aggregateTypeIndex].propertyMap;

        for (int i = 0; i < propertyMap.count; ++i) {
            if (propertyName == propertyMap[i].galleryKey)
                return propertyMap[i].trackerKey;
        }
    }
    return QString();
}

QGalleryTrackerSchema::IdFunc QGalleryTrackerSchema::idFunc() const
{
    if (m_fileTypeIndex >= 0)
        return qt_galleryFileId;
    else if (m_aggregateTypeIndex >= 0)
        return qt_galleryAggregateTypeMap[m_aggregateTypeIndex].idFunc;
    else
        return 0;
}


QStringList QGalleryTrackerSchema::supportedPropertyNames() const
{
    QStringList propertyNames;

    if (m_fileTypeIndex >= 0) {
        const QGalleryPropertyMap propertyMap = qt_galleryTypeMap[m_fileTypeIndex].propertyMap;

        for (int i = 0; i < propertyMap.count; ++i)
            propertyNames.append(propertyMap[i].galleryKey);
    } else if (m_aggregateTypeIndex >= 0) {
        const QGalleryPropertyMap propertyMap
                = qt_galleryAggregateTypeMap[m_aggregateTypeIndex].propertyMap;

        for (int i = 0; i < propertyMap.count; ++i)
            propertyNames.append(propertyMap[i].galleryKey);
    }
    return propertyNames;
}

QGalleryProperty::Attributes QGalleryTrackerSchema::propertyAttributes(
        const QString &propertyName) const
{
    if (m_fileTypeIndex >= 0) {
        const QGalleryPropertyMap propertyMap = qt_galleryTypeMap[m_fileTypeIndex].propertyMap;
        for (int i = 0; i < propertyMap.count; ++i) {
            if (propertyName == propertyMap[i].galleryKey)
                return propertyMap[i].attributes;
        }
    } else if (m_aggregateTypeIndex >= 0) {
        const QGalleryPropertyMap propertyMap
                = qt_galleryAggregateTypeMap[m_aggregateTypeIndex].propertyMap;
        for (int i = 0; i < propertyMap.count; ++i) {
            if (propertyName == propertyMap[i].galleryKey)
                return propertyMap[i].attributes;
        }

        const QGalleryAggregatePropertyMap aggregateMap
                = qt_galleryAggregateTypeMap[m_aggregateTypeIndex].aggregatePropertyMap;
        for (int i = 0; i < aggregateMap.count; ++i) {
            if (propertyName == aggregateMap[i].galleryKey)
                return QGalleryProperty::CanRead;
        }
    }
    return QGalleryProperty::Attributes();
}


QStringList QGalleryTrackerSchema::identityFields() const
{
    QStringList fields;

    if (m_aggregateTypeIndex >= 0) {
        const QGalleryPropertyMap identity
                = qt_galleryAggregateTypeMap[m_aggregateTypeIndex].identity;

        for (int i = 0; i < identity.count; ++i)
            fields.append(identity[i].trackerKey);
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
            propertyNames.append(identity[i].galleryKey);
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
            if (propertyName == propertyMap[i].galleryKey) {
                return QPair<QString, AggregateType>(
                        propertyMap[i].trackerKey, propertyMap[i].type);
            }
        }
    }
    return qMakePair(QString(), Count);
}

QString QGalleryTrackerSchema::typeFromService(const QString &service)
{
    QGalleryTypeMap typeMap(qt_galleryTypeMap);

    for (int i = 0; i < typeMap.count; ++i) {
        if (typeMap[i].trackerKey == service)
            return typeMap[i].galleryKey;
    }
    return QString();
}

QString QGalleryTrackerSchema::prefixFromService(const QString &service)
{
    QGalleryTypeMap typeMap(qt_galleryTypeMap);

    for (int i = 0; i < typeMap.count; ++i) {
        if (typeMap[i].trackerKey == service)
            return typeMap[i].prefix;
    }
    return QString();
}

void QGalleryTrackerSchema::setPropertyNames(const QStringList &names)
{
    m_propertyNames = names;
}

void QGalleryTrackerSchema::setSortPropertyNames(const QStringList &names)
{
    m_sortPropertyNames = names;
}

QGalleryTrackerCompositeColumn *QGalleryTrackerSchema::createIdColumn() const
{
    if (m_fileTypeIndex >= 0) {
        return new QGalleryTrackerServicePrefixColumn(0, 1);
    } else if (m_aggregateTypeIndex >= 0) {
        QGalleryAggregateTypeMap typeMap(qt_galleryAggregateTypeMap);

        const QString prefix = typeMap[m_aggregateTypeIndex].prefix;

        if (typeMap[m_aggregateTypeIndex].identity.count == 1) {
            return new QGalleryTrackerPrefixColumn(0, prefix);
        } else if (typeMap[m_aggregateTypeIndex].identity.count == 2) {
            return new QGalleryTrackerCompositeIdColumn(m_identityColumns, prefix);
        }
    }
    return new QGalleryTrackerStaticColumn(QVariant());
}

QGalleryTrackerCompositeColumn *QGalleryTrackerSchema::createUrlColumn() const
{
    if (m_fileTypeIndex >= 0) {
        return new QGalleryTrackerFileUrlColumn(0);
    } else {
        return new QGalleryTrackerStaticColumn(QUrl());
    }
}

QGalleryTrackerCompositeColumn *QGalleryTrackerSchema::createTypeColumn() const
{
    if (m_fileTypeIndex >= 0) {
        return new QGalleryTrackerServiceTypeColumn(1);
    } else if (m_aggregateTypeIndex >= 0) {
        return new QGalleryTrackerStaticColumn(QString(
                qt_galleryAggregateTypeMap[m_aggregateTypeIndex].galleryKey));
    } else {
        return new QGalleryTrackerStaticColumn(QVariant());
    }
}

QVector<QGalleryTrackerValueColumn *> QGalleryTrackerSchema::createValueColumns() const
{
    QVector<QGalleryTrackerValueColumn *> columns;

    columns.reserve(m_valueTypes.count());

    for (int i = 0, count = m_valueTypes.count(); i < count; ++i) {
        switch (m_valueTypes.at(i)) {
        case QVariant::String:
            columns.append(new QGalleryTrackerStringColumn);
            break;
        case QVariant::Int:
            columns.append(new QGalleryTrackerStringColumn);
            break;
        case QVariant::Double:
            columns.append(new QGalleryTrackerStringColumn);
            break;
        case QVariant::DateTime:
            columns.append(new QGalleryTrackerStringColumn);
            break;
        default:
            Q_ASSERT(false);
            break;
        }
    }

    return columns;
}

QVector<QGalleryTrackerCompositeColumn *> QGalleryTrackerSchema::createCompositeColumns() const
{
    QVector<QGalleryTrackerCompositeColumn *> columns;

    return columns;
}

QVector<QGalleryTrackerAliasColumn *> QGalleryTrackerSchema::createAliasColumns() const
{
    QVector<QGalleryTrackerAliasColumn *> columns;

    columns.reserve(m_aliasColumns.count());

    for (int i = 0, count = m_aliasColumns.count(); i < count; ++i) {
        columns.append(new QGalleryTrackerAliasColumn(m_aliasColumns.at(i)));
    }

    return columns;
}

QVector<QGalleryTrackerImageColumn *> QGalleryTrackerSchema::createImageColumns() const
{
    QVector<QGalleryTrackerImageColumn *> columns;

    return columns;
}

int QGalleryTrackerSchema::resolveColumns()
{
    if (m_fileTypeIndex >= 0)
        return resolveFileColumns();
    else if (m_aggregateTypeIndex >= 0)
        return resolveAggregateColumns();
    else
        return QGalleryAbstractRequest::ItemTypeError;
}

int QGalleryTrackerSchema::resolveFileColumns()
{
    QStringList propertyFields;
    QStringList valueNames;
    QStringList aliasNames;
    QVector<QGalleryProperty::Attributes> valueAttributes;
    QVector<QGalleryProperty::Attributes> aliasAttributes;
    QVector<QVariant::Type> valueTypes;
    QVector<int> aliasColumns;
    QVector<QGalleryTrackerSortCriteria> sortCriteria;

    const QGalleryPropertyMap &properties = qt_galleryTypeMap[m_fileTypeIndex].propertyMap;

    m_propertyNames.removeDuplicates();

    for (QStringList::const_iterator it = m_propertyNames.constBegin();
            it != m_propertyNames.constEnd();
            ++it) {
        int propertyIndex = properties.indexOfProperty(*it);

        if (propertyIndex >= 0) {
            const QString field = properties[propertyIndex].trackerKey;

            int fieldIndex = propertyFields.indexOf(field);

            if (fieldIndex >= 0) {
                aliasColumns.append(fieldIndex + 2);
                aliasNames.append(*it);
                aliasAttributes.append(properties[propertyIndex].attributes);
            } else {
                propertyFields.append(field);
                valueNames.append(*it);
                valueAttributes.append(properties[propertyIndex].attributes);
                valueTypes.append(QVariant::String);
            }
        } else if (*it == QLatin1String("filePath")) {
            aliasColumns.append(0);
            aliasNames.append(*it);
            aliasAttributes.append(QGalleryProperty::CanRead);
        }
    }

    bool descending = !m_sortPropertyNames.isEmpty()
            && m_sortPropertyNames.first().startsWith(QLatin1Char('-'));

    for (QStringList::const_iterator it = m_sortPropertyNames.constBegin();
            it != m_sortPropertyNames.constEnd();
            ++it) {
        int sortFlags = QGalleryTrackerSortCriteria::Ascending;

        QString propertyName = *it;

        if (propertyName.startsWith(QLatin1Char('-'))) {
            propertyName = propertyName.mid(1);
            sortFlags = QGalleryTrackerSortCriteria::Descending;

            sortFlags |= descending
                    ? QGalleryTrackerSortCriteria::Sorted
                    : QGalleryTrackerSortCriteria::ReverseSorted;
        } else {
            if (propertyName.startsWith(QLatin1Char('+')))
                propertyName = propertyName.mid(1);

            sortFlags |= descending
                    ? QGalleryTrackerSortCriteria::ReverseSorted
                    : QGalleryTrackerSortCriteria::Sorted;
        }

        int propertyIndex = properties.indexOfProperty(propertyName);

        if (propertyIndex >= 0
                && properties[propertyIndex].attributes & QGalleryProperty::CanSort) {
            const QString field = properties[propertyIndex].trackerKey;

            int fieldIndex = propertyFields.indexOf(field);

            if (fieldIndex < 0) {
                fieldIndex = propertyFields.count();
                propertyFields.append(field);
                valueNames.append(propertyName);
                valueAttributes.append(properties[propertyIndex].attributes);
                valueTypes.append(QVariant::String);
            }

            sortCriteria.append(QGalleryTrackerSortCriteria(fieldIndex + 2, sortFlags));
        }
    }

    m_propertyFields = propertyFields;
    m_aggregateFields = QStringList();
    m_aggregations = QStringList();
    m_propertyNames = valueNames + aliasNames;
    m_propertyAttributes = valueAttributes + aliasAttributes;
    m_valueTypes = valueTypes;
    m_compositeColumns = QVector<int>();
    m_aliasColumns = aliasColumns;
    m_imageColumns = QVector<int>();
    m_sortCriteria = sortCriteria;

    return QGalleryAbstractRequest::Succeeded;
}

int QGalleryTrackerSchema::resolveAggregateColumns()
{
    QStringList identityFields;
    QStringList aggregateFields;
    QStringList aggregates;
    QStringList identityNames;
    QStringList aggregateNames;
    QStringList aliasNames;
    QVector<QGalleryProperty::Attributes> identityAttributes;
    QVector<QGalleryProperty::Attributes> aggregateAttributes;
    QVector<QGalleryProperty::Attributes> aliasAttributes;
    QVector<QVariant::Type> identityTypes;
    QVector<QVariant::Type> aggregateTypes;
    QVector<int> identityColumns;
    QVector<int> aliasColumns;
    QVector<QGalleryTrackerSortCriteria> sortCriteria;

    const QGalleryAggregateTypeMapItem &type = qt_galleryAggregateTypeMap[m_aggregateTypeIndex];
    const QGalleryPropertyMap &properties = type.propertyMap;
    const QGalleryAggregatePropertyMap &aggregateProperties = type.aggregatePropertyMap;

    m_propertyNames.removeDuplicates();

    for (int i = 0; i < type.identity.count; ++i) {
        const QString &propertyName = type.identity[i].galleryKey;

        identityNames.append(propertyName);

        m_propertyNames.removeAll(propertyName);
    }

    qSort(identityNames.begin(), identityNames.end(), QGalleryReorderLessThan(m_sortPropertyNames));

    for (int i = 0; i < type.identity.count; ++i)
        identityColumns.append(identityNames.indexOf(type.identity[i].galleryKey));

    for (QStringList::const_iterator it = identityNames.constBegin();
            it != identityNames.constEnd();
            ++it) {
        int propertyIndex = properties.indexOfProperty(*it);
        Q_ASSERT(propertyIndex >= 0);

        const QString field = properties[propertyIndex].trackerKey;
        Q_ASSERT(identityFields.indexOf(field) == -1);

        identityFields.append(field);
        identityAttributes.append(properties[propertyIndex].attributes);
        identityTypes.append(QVariant::String);
    }

    for (QStringList::const_iterator it = m_propertyNames.constBegin();
            it != m_propertyNames.constEnd();
            ++it) {
        int propertyIndex = properties.indexOfProperty(*it);

        if (propertyIndex >= 0) {
            const QString field = properties[propertyIndex].trackerKey;

            int fieldIndex = identityFields.indexOf(field);
            Q_ASSERT(fieldIndex != -1);

            aliasColumns.append(fieldIndex);
            aliasNames.append(*it);
            aliasAttributes.append(properties[propertyIndex].attributes);
        } else if ((propertyIndex = aggregateProperties.indexOfProperty(*it)) >= 0) {
            aggregateNames.append(*it);
            aggregateFields.append(aggregateProperties[propertyIndex].trackerKey);
            aggregates.append(aggregateProperties[propertyIndex].aggregate);
            aggregateAttributes.append(QGalleryProperty::CanRead);
            aggregateTypes.append(QVariant::String);

            break;
        }
    }

    bool descending = !m_sortPropertyNames.isEmpty()
            && m_sortPropertyNames.first().startsWith(QLatin1Char('-'));

    for (QStringList::const_iterator it = m_sortPropertyNames.constBegin();
            it != m_sortPropertyNames.constEnd();
            ++it) {
        int sortFlags = QGalleryTrackerSortCriteria::Ascending;

        QString propertyName = *it;

        if (propertyName.startsWith(QLatin1Char('-'))) {
            propertyName = propertyName.mid(1);
            sortFlags = QGalleryTrackerSortCriteria::Descending;

            sortFlags |= descending
                    ? QGalleryTrackerSortCriteria::Sorted
                    : QGalleryTrackerSortCriteria::ReverseSorted;
        } else {
            if (propertyName.startsWith(QLatin1Char('+')))
                propertyName = propertyName.mid(1);

            sortFlags |= descending
                    ? QGalleryTrackerSortCriteria::ReverseSorted
                    : QGalleryTrackerSortCriteria::Sorted;
        }

        int fieldIndex = identityNames.indexOf(propertyName);

        if (fieldIndex >= 0)
            sortCriteria.append(QGalleryTrackerSortCriteria(fieldIndex, sortFlags));
    }

    m_propertyFields = identityFields;
    m_aggregateFields = aggregateFields;
    m_aggregations = aggregates;
    m_propertyNames = identityNames + aggregateNames + aliasNames;
    m_propertyAttributes = identityAttributes + aggregateAttributes + aliasAttributes;
    m_valueTypes = identityTypes + aggregateTypes;
    m_identityColumns = identityColumns;
    m_compositeColumns = QVector<int>();
    m_aliasColumns = aliasColumns;
    m_imageColumns = QVector<int>();
    m_sortCriteria = sortCriteria;

    return QGalleryAbstractRequest::Succeeded;
}

QTM_END_NAMESPACE
