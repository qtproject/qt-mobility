/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qgallerytrackerthumbnailcolumn_p.h"

#include <QtCore/qmetatype.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/qxmlstream.h>

QTM_BEGIN_NAMESPACE

namespace
{
    struct QGalleryItemProperty
    {
        QLatin1String name;
        QLatin1String field;
        QGalleryProperty::Attributes attributes;
    };

    struct QGalleryAggregateProperty
    {
        QLatin1String name;
        QLatin1String field;
        QLatin1String aggregate;
    };

    struct QGalleryThumbnailProperty
    {
        QLatin1String name;
        QVariant::Type type;
    };

    template <typename T>
    struct QGalleryPropertyList
    {
        template <int N> QGalleryPropertyList(const T (&items)[N])
            : items(items), count(N) {}

        const T *items;
        const int count;

        int indexOfProperty(const QString &propertyName) const;

        const T &operator [](int index) const { return items[index]; }
    };

    typedef QGalleryPropertyList<QGalleryItemProperty> QGalleryItemPropertyList;
    typedef QGalleryPropertyList<QGalleryAggregateProperty> QGalleryAggregatePropertyList;
    typedef QGalleryPropertyList<QGalleryThumbnailProperty> QGalleryThumbnailPropertyList;

    enum UpdateId
    {
        FileId          = 0x0001,
        FolderId        = 0x0002,
        DocumentId      = 0x0004,
        AudioId         = 0x0008,
        ImageId         = 0x0010,
        VideoId         = 0x0020,
        PlaylistId      = 0x0040,
        TextId          = 0x0080
    };

    enum UpdateMask
    {
        FileMask        = FileId
                        | FolderId
                        | DocumentId
                        | AudioId
                        | ImageId
                        | VideoId
                        | PlaylistId
                        | TextId,
        FolderMask      = FolderId,
        DocumentMask    = DocumentId,
        AudioMask       = AudioId,
        ImageMask       = ImageId,
        VideoMask       = VideoId,
        PlaylistMask    = PlaylistId,
        TextMask        = TextId
    };

    struct QGalleryTypePrefix : public QLatin1String
    {
        template <int N> QGalleryTypePrefix(const char (&prefix)[N])
            : QLatin1String(prefix), length(N - 1){}

        const int length;

        QStringRef strip(const QString &id) const {
            return QStringRef(&id, length, id.length() - length); }
    };

    struct QGalleryItemType
    { 
        QLatin1String itemType;
        QLatin1String service;
        QGalleryTypePrefix prefix;
        QGalleryItemPropertyList itemProperties;
        QGalleryThumbnailPropertyList thumbnailProperties;
        int updateId;
        int updateMask;
    };

    struct QGalleryAggregateType
    {
        QLatin1String itemType;
        QLatin1String service;
        QGalleryTypePrefix prefix;
        QGalleryItemPropertyList identity;
        QGalleryItemPropertyList properties;
        QGalleryAggregatePropertyList aggregateProperties;
        void (*writeIdCondition)(int *error, QXmlStreamWriter *xml, const QStringRef &itemId);
        int updateMask;
    };

    template <typename T>
    struct QGalleryTypeList
    {
        template <int N> QGalleryTypeList(const T (&items)[N])
            : items(items), count(N) {}

        const T *items;
        const int count;

        int indexOfType(const QString &type) const;
        int indexOfItemId(const QString &itemId) const;
        int indexOfService(const QString &service) const;

        const T &operator [](int index) const { return items[index]; }
    };

    typedef QGalleryTypeList<QGalleryItemType> QGalleryItemTypeList;
    typedef QGalleryTypeList<QGalleryAggregateType> QGalleryAggregateTypeList;

    template <typename T>
    int QGalleryPropertyList<T>::indexOfProperty(const QString &name) const
    {
        for (int i = 0; i < count; ++i) {
            if (items[i].name == name)
                return i;
        }
        return -1;
    }

    template <typename T>
    int QGalleryTypeList<T>::indexOfType(const QString &type) const
    {
        for (int i = 0; i < count; ++i) {
            if (items[i].itemType == type)
                return i;
        }
        return -1;
    }

    template <typename T>
    int QGalleryTypeList<T>::indexOfItemId(const QString &itemId) const
    {
        for (int i = 0; i < count; ++i) {
            if (itemId.startsWith(items[i].prefix))
                return i;
        }
        return -1;
    }

    template <typename T>
    int QGalleryTypeList<T>::indexOfService(const QString &service) const
    {
        for (int i = 0; i < count; ++i) {
            if (items[i].service == service)
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

#define QT_GALLERY_ITEM_PROPERTY(PropertyName, Field, Attr) \
{ QLatin1String(PropertyName), QLatin1String(Field), QGalleryProperty::Attributes(Attr) }

#define QT_GALLERY_AGGREGATE_PROPERTY(PropertyName, Field, Aggregate) \
{ QLatin1String(PropertyName), QLatin1String(Field), QLatin1String(Aggregate) }

#define QT_GALLERY_THUMBNAIL_PROPERTY(PropertyName, Type) \
{ QLatin1String(PropertyName), QVariant::Type }

#define QT_GALLERY_ITEM_TYPE(Type, Service, Prefix, ItemProperties) \
{ \
    QLatin1String(#Type), \
    QLatin1String(#Service), \
    QGalleryTypePrefix(#Prefix"::"), \
    QGalleryItemPropertyList(ItemProperties), \
    QGalleryThumbnailPropertyList(qt_galleryFileThumbnailPropertyList), \
    Type##Id, \
    Type##Mask \
}

#define QT_GALLERY_AGGREGATE_TYPE(Type, Service, Prefix, UpdateMask) \
{ \
    QLatin1String(#Type), \
    QLatin1String(#Service), \
    QGalleryTypePrefix(#Prefix"::"), \
    QGalleryItemPropertyList(qt_gallery##Type##Identity), \
    QGalleryItemPropertyList(qt_gallery##Type##PropertyList),\
    QGalleryAggregatePropertyList(qt_gallery##Type##AggregateList), \
    qt_write##Type##IdCondition, \
    UpdateMask \
}

static bool qt_writePropertyName(
        int *error,
        QXmlStreamWriter *xml,
        const QString &propertyName,
        const QGalleryItemPropertyList &properties)
{
    int index = properties.indexOfProperty(propertyName);

    if (index == -1) {
        *error = QGalleryAbstractRequest::InvalidPropertyError;

        return false;
    } else {
        do {
            xml->writeEmptyElement(QLatin1String("rdfq:Property"));
            xml->writeAttribute(QLatin1String("name"), properties[index].field);
        } while (++index < properties.count && propertyName == properties[index].name);

        return true;
    }

}

template <typename T> bool qt_writeValue(int *error, QXmlStreamWriter *xml, const QVariant &value)
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

template <> bool qt_writeValue<QDateTime>(int *, QXmlStreamWriter *xml, const QVariant &value)
{
    xml->writeStartElement(QLatin1String("rdf:Date"));
    xml->writeCharacters(value.toString());
    xml->writeEndElement();

    return true;
}

template <> bool qt_writeValue<QVariant>(int *error, QXmlStreamWriter *xml, const QVariant &value)
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
        const QGalleryItemPropertyList &properties);

static bool qt_writeCondition(
        int *error,
        QXmlStreamWriter *xml,
        const QGalleryIntersectionFilter &filter,
        const QGalleryItemPropertyList &properties)
{
    if (!filter.isEmpty()) {
        xml->writeStartElement(QLatin1String("rdfq:or"));

        const QList<QGalleryFilter> filters = filter.filters();
        for (QList<QGalleryFilter>::const_iterator it = filters.begin(), end = filters.end();
                it != end;
                ++it) {
            if (!qt_writeCondition(error, xml, *it, properties)) {
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
        const QGalleryItemPropertyList &properties)
{
    if (!filter.isEmpty()) {
        xml->writeStartElement(QLatin1String("rdfq:and"));

        const QList<QGalleryFilter> filters = filter.filters();
        for (QList<QGalleryFilter>::const_iterator it = filters.begin(), end = filters.end();
                it != end;
                ++it) {
            if (!qt_writeCondition(error, xml, *it, properties)) {
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
        const QGalleryItemPropertyList &properties)
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

    if (qt_writePropertyName(error, xml, filter.propertyName(), properties)
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
        const QGalleryItemPropertyList &properties)
{
    const QString propertyName = filter.propertyName();
    const QGalleryFilter::RangeFlags flags = filter.rangeFlags();

    bool isRange = (flags == QGalleryFilter::InclusiveRange || QGalleryFilter::ExclusiveRange);

    if (isRange)
        xml->writeStartElement(QLatin1String("rdfq:and"));

    if (flags & QGalleryFilter::GreaterThanEqualsMinimum) {
        if (flags & QGalleryFilter::EqualsMinimum)
            xml->writeStartElement(QLatin1String("rdfq:greaterThanEqual"));
        else
            xml->writeStartElement(QLatin1String("rdfq:greaterThan"));

        if (!qt_writePropertyName(error, xml, propertyName, properties)
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

        if (!qt_writePropertyName(error, xml, propertyName, properties)
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
        const QGalleryItemPropertyList &properties)
{
    switch (filter.type()) {
    case QGalleryFilter::Invalid:
        return true;
    case QGalleryFilter::Intersection:
        return qt_writeCondition(
                error, xml, filter.toIntersectionFilter(), properties);
    case QGalleryFilter::Union:
        return qt_writeCondition(error, xml, filter.toUnionFilter(), properties);
    case QGalleryFilter::MetaData:
        return qt_writeCondition(error, xml, filter.toMetaDataFilter(), properties);
    case QGalleryFilter::MetaDataRange:
        return qt_writeCondition(
                error, xml, filter.toMetaDataRangeFilter(), properties);
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
    QT_GALLERY_ITEM_PROPERTY("fileName"    , "File:Name"     , CanRead | CanSort | CanFilter), \
  /*QT_GALLERY_ITEM_PROPERTY(""            , "File:Contents" , QGalleryProperty::Attributes()),*/ \
  /*QT_GALLERY_ITEM_PROPERTY(""            , "File:Link"     , QGalleryProperty::Attributes()),*/ \
    QT_GALLERY_ITEM_PROPERTY("mimeType"    , "File:Mime"     , CanRead | CanSort | CanFilter), \
  /*QT_GALLERY_ITEM_PROPERTY(""            , "File:Size"     , QGalleryProperty::Attributes()),*/ \
  /*QT_GALLERY_ITEM_PROPERTY(""            , "File:License"  , QGalleryProperty::Attributes()),*/ \
    QT_GALLERY_ITEM_PROPERTY("copyright"   , "File:Copyright", CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("lastModified", "File:Modified" , CanRead | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("lastAccessed", "File:Accessed" , CanRead | CanSort | CanFilter), \
  /*QT_GALLERY_ITEM_PROPERTY(""            , "File:Added"    , QGalleryProperty::Attributes()),*/ \
  /*QT_GALLERY_ITEM_PROPERTY(""            , "File:Other"    , QGalleryProperty::Attributes()),*/ \
    QT_GALLERY_ITEM_PROPERTY("language"    , "File:Language" , CanRead | CanSort | CanFilter)

static const QGalleryItemProperty qt_galleryFilePropertyList[] =
{
    QT_GALLERY_FILE_PROPERTYS,
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Contributor", QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Coverage"   , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("author"     , "DC:Creator"    , CanRead | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Date"       , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("description", "DC:Description", CanRead | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Format"     , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Identifier" , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Language"   , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Publisher"  , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Relation"   , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Rights"     , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Source"     , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("subject"    , "DC:Subject"    , CanRead | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Keywords"   , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("title"      , "DC:Title"      , CanRead | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Type"       , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("rating"     , "User:Rank"     , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("keywords"   , "User:Keywords" , CanRead | CanSort | CanFilter)
};

static const QGalleryThumbnailProperty qt_galleryFileThumbnailPropertyList[] =
{
    QT_GALLERY_THUMBNAIL_PROPERTY("thumbnailImage", Image),
    QT_GALLERY_THUMBNAIL_PROPERTY("thumbnailPixmap", Pixmap)
};

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

////////
// Audio
////////

static const QGalleryItemProperty qt_galleryAudioPropertyList[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_ITEM_PROPERTY("title"       , "Audio:Title"        , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("artist"      , "Audio:Artist"       , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("albumTitle"  , "Audio:Album"        , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("genre"       , "Audio:Genre"        , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("duration"    , "Audio:Duration"     , CanRead | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""          , "Audio:ReleaseDate"  , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("albumArtist" , "Audio:AlbumArtist"  , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("trackNumber" , "Audio:TrackNo"      , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("discNumber"  , "Audio:DiscNo"       , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("performer"   , "Audio:Performer"    , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""          , "Audio:TrackGain"    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""          , "Audio:AlbumGain"    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""          , "Audio:AlbumPeakGain", QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("description" , "Audio:Comment"      , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("audioCodec"  , "Audio:Codec"        , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY(""            , "Audio:CodecVersion" , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("sampleRate"  , "Audio:SampleRate"   , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("audioBitRate", "Audio:Bitrate"      , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("channelCount", "Audio:Channels"     , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("lastPlayed"  , "Audio:LastPlay"     , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("playCount"   , "Audio:PlayCount"    , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""          , "Audio:DateAdded"    , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("lyrics"      , "Audio:Lyrics"       , CanRead | CanWrite | CanSort | CanFilter)
};

///////////
// Playlist
///////////

static const QGalleryItemProperty qt_galleryPlaylistPropertyList[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_ITEM_PROPERTY("duration"  , "Playlist:Duration"     , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("trackCount", "Playlist:Songs"        , CanRead | CanSort | CanFilter)
//    QT_GALLERY_ITEM_PROPERTY(""        , "Playlist:Name"         , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""        , "Playlist:ValidDuration", QGalleryProperty::Attributes())
};

////////
// Image
////////

static const QGalleryItemProperty qt_galleryImagePropertyList[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_ITEM_PROPERTY("title"             , "Image:Title"          , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("keywords"          , "Image:Keywords"       , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("height"            , "Image:Height"         , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("width"             , "Image:Width"          , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("album"             , "Image:Album"          , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("dateTaken"         , "Image:Date"           , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("author"            , "Image:Creator"        , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("comment"           , "Image:Comments"       , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("description"       , "Image:Description"    , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:Software"       , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("cameraManufacturer", "Image:CameraMake"     , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("cameraModel"       , "Image:CameraModel"    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("orientation"       , "Image:Orientation"    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("exposureProgram"   , "Image:ExposureProgram", CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("exposureTime"      , "Image:ExposureTime"   , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("fNumber"           , "Image:FNumber"        , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("flashEnabled"      , "Image:Flash"          , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("focalLength"       , "Image:FocalLength"    , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:ISOSpeed"       , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("meteringMode"      , "Image:MeteringMode"   , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("whiteBalance"      , "Image:WhiteBalance"   , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("rating"            , "Image:Rating"         , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:Location"       , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:Sublocation"    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:Country"        , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:City"           , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:HasKeywords"    , QGalleryProperty::Attributes())
};

////////
// Video
////////

static const QGalleryItemProperty qt_galleryVideoPropertyList[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_ITEM_PROPERTY("title"         , "Video:Title"          , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("author"        , "Video:Author"         , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("height"        , "Video:Height"         , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("width"         , "Video:Width"          , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("duration"      , "Video:Duration"       , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("comments"      , "Video:Comments"       , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("frameRate"     , "Video:FrameRate"      , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("videoCodec"    , "Video:Codec"          , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("videoBitRate"  , "Video:Bitrate"        , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("playCount"     , "Video:PlayCount"      , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("resumePosition", "Video:PausePosition"  , CanRead | CanWrite | CanSort | CanFilter)
//    QT_GALLERY_ITEM_PROPERTY(""            , "Video:LastPlayedFrame", QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""            , "Video:Source"         , QGalleryProperty::Attributes())
};

///////////
// Document
///////////

static const QGalleryItemProperty qt_galleryDocumentPropertyList[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_ITEM_PROPERTY("title"    , "Doc:Title"    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("subject"  , "Doc:Subject"  , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("author"   , "Doc:Author"   , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("keywords" , "Doc:Keywords" , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("comment"  , "Doc:Comments" , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("pageCount", "Doc:PageCount", CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("wordCount", "Doc:WordCount", CanRead | CanSort | CanFilter)
//    QT_GALLERY_ITEM_PROPERTY(""       , "Doc:Creator"  , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""       , "Doc:URL"      , QGalleryProperty::Attributes())
};

/////////////
// File Types
/////////////

static const QGalleryItemType qt_galleryItemTypeList[] =
{
    QT_GALLERY_ITEM_TYPE(File       , Files        , file    , qt_galleryFilePropertyList),
    QT_GALLERY_ITEM_TYPE(Folder     , Folders      , folder  , qt_galleryFilePropertyList),
    QT_GALLERY_ITEM_TYPE(Document   , Documents    , document, qt_galleryDocumentPropertyList),
    QT_GALLERY_ITEM_TYPE(Audio      , Music        , audio   , qt_galleryAudioPropertyList),
    QT_GALLERY_ITEM_TYPE(Image      , Images       , image   , qt_galleryImagePropertyList),
    QT_GALLERY_ITEM_TYPE(Video      , Videos       , video   , qt_galleryVideoPropertyList),
    QT_GALLERY_ITEM_TYPE(Playlist   , Playlists    , playlist, qt_galleryPlaylistPropertyList),
    QT_GALLERY_ITEM_TYPE(Text       , Text         , text    , qt_galleryFilePropertyList),
    QT_GALLERY_ITEM_TYPE(File       , Development  , file    , qt_galleryFilePropertyList),
    QT_GALLERY_ITEM_TYPE(File       , Other        , file    , qt_galleryFilePropertyList),
};

/////////
// Artist
/////////

static const QGalleryItemProperty qt_galleryArtistIdentity[] =
{
    QT_GALLERY_ITEM_PROPERTY("title", "Audio:Artist", CanRead | CanFilter)
};

static const QGalleryItemProperty qt_galleryArtistPropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("artist", "Audio:Artist", CanRead | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("title" , "Audio:Artist", CanRead | CanFilter),
};

static const QGalleryAggregateProperty qt_galleryArtistAggregateList[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", "SUM"),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , "COUNT"),
};

static void qt_writeArtistIdCondition(int *, QXmlStreamWriter *xml, const QStringRef &itemId)
{
    qt_writeEqualsCondition(xml, QLatin1String("Audio:Artist"), itemId.toString());
}

///////////////
// Album Artist
///////////////

static const QGalleryItemProperty qt_galleryAlbumArtistIdentity[] =
{
    QT_GALLERY_ITEM_PROPERTY("title", "Audio:AlbumArtist", CanRead | CanFilter)
};

static const QGalleryItemProperty qt_galleryAlbumArtistPropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("artist", "Audio:AlbumArtist", CanRead | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("title" , "Audio:AlbumArtist", CanRead | CanFilter),
};

static const QGalleryAggregateProperty qt_galleryAlbumArtistAggregateList[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", "SUM"),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , "COUNT"),
};

static void qt_writeAlbumArtistIdCondition(int *, QXmlStreamWriter *xml, const QStringRef &itemId)
{
    qt_writeEqualsCondition(xml, QLatin1String("Audio:AlbumArtist"), itemId.toString());
}

////////
// Album
////////

static const QGalleryItemProperty qt_galleryAlbumIdentity[] =
{
    QT_GALLERY_ITEM_PROPERTY("artist", "Audio:AlbumArtist", CanRead | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("title" , "Audio:Album"      , CanRead | CanFilter)
};

static const QGalleryItemProperty qt_galleryAlbumPropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("title" , "Audio:Album"      , CanRead | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("artist", "Audio:AlbumArtist", CanRead | CanFilter)
};

static const QGalleryAggregateProperty qt_galleryAlbumAggregateList[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", "SUM"),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , "COUNT"),
};

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

static const QGalleryItemProperty qt_galleryAudioGenreIdentity[] =
{
    QT_GALLERY_ITEM_PROPERTY("title", "Audio:Genre", CanRead | CanFilter)
};

static const QGalleryItemProperty qt_galleryAudioGenrePropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("genre", "Audio:Genre", CanRead | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("title", "Audio:Genre", CanRead | CanFilter),
};

static const QGalleryAggregateProperty qt_galleryAudioGenreAggregateList[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", "SUM"),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , "COUNT"),
};

static void qt_writeAudioGenreIdCondition(int *, QXmlStreamWriter *xml, const QStringRef &itemId)
{
    qt_writeEqualsCondition(xml, QLatin1String("Audio:Genre"), itemId.toString());
}

//////////////
// Photo Album
//////////////

static const QGalleryItemProperty qt_galleryPhotoAlbumIdentity[] =
{
    QT_GALLERY_ITEM_PROPERTY("title", "Image:Album", CanRead | CanFilter)
};

static const QGalleryItemProperty qt_galleryPhotoAlbumPropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("title", "Image:Album", CanRead | CanFilter),
};

static const QGalleryAggregateProperty qt_galleryPhotoAlbumAggregateList[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*", "COUNT")
};

static void qt_writePhotoAlbumIdCondition(int *, QXmlStreamWriter *xml, const QStringRef &itemId)
{
    qt_writeEqualsCondition(xml, QLatin1String("Image:Album"), itemId.toString());
}

//////////////////
// Aggregate Types
//////////////////

static const QGalleryAggregateType qt_galleryAggregateTypeList[] =
{
    QT_GALLERY_AGGREGATE_TYPE(Artist     , Music , artist     , AudioMask),
    QT_GALLERY_AGGREGATE_TYPE(AlbumArtist, Music , albumArtist, AudioMask),
    QT_GALLERY_AGGREGATE_TYPE(Album      , Music , album      , AudioMask),
    QT_GALLERY_AGGREGATE_TYPE(AudioGenre , Music , audioGenre , AudioMask),
    QT_GALLERY_AGGREGATE_TYPE(PhotoAlbum , Images, photoAlbum , ImageMask)

};


class QGalleryTrackerServicePrefixColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerServicePrefixColumn() {}

    QVariant value(QVector<QVariant>::const_iterator row) const;
};

class QGalleryTrackerServiceTypeColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerServiceTypeColumn() {}

    QVariant value(QVector<QVariant>::const_iterator row) const;
};

QVariant QGalleryTrackerServicePrefixColumn::value(QVector<QVariant>::const_iterator row) const
{
    QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);

    const int index = itemTypes.indexOfService((row + 1)->toString());

    return index != -1
            ? QVariant(itemTypes[index].prefix + row->toString())
            : QVariant();
}

QVariant QGalleryTrackerServiceTypeColumn::value(QVector<QVariant>::const_iterator row) const
{
    QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);

    const int index = itemTypes.indexOfService((row + 1)->toString());

    return index != -1
            ? QVariant(itemTypes[index].itemType)
            : QVariant();

}

QGalleryTrackerSchema::QGalleryTrackerSchema()
    : m_itemIndex(-1)
    , m_aggregateIndex(-1)
{
}

QGalleryTrackerSchema::~QGalleryTrackerSchema()
{
}

void QGalleryTrackerSchema::setItemType(const QString &type)
{
    m_itemType = type;
    m_itemIndex = QGalleryItemTypeList(qt_galleryItemTypeList).indexOfType(type);
    m_aggregateIndex = m_itemIndex == -1
            ? QGalleryAggregateTypeList(qt_galleryAggregateTypeList).indexOfType(type)
            : -1;
}

void QGalleryTrackerSchema::resolveTypeFromItemId(const QString &itemId)
{
    QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);
    m_itemIndex = itemTypes.indexOfItemId(itemId);

    if (m_itemIndex != -1) {
        m_itemType = itemTypes[m_itemIndex].itemType;
        m_aggregateIndex = -1;
    } else {
        QGalleryAggregateTypeList aggregateTypes(qt_galleryAggregateTypeList);
        m_aggregateIndex = aggregateTypes.indexOfItemId((itemId));

        if (m_aggregateIndex != -1) {
            m_itemType = aggregateTypes[m_aggregateIndex].itemType;
        } else {
            m_itemType = QString();
        }
    }
}

void QGalleryTrackerSchema::resolveTypeFromService(const QString &service)
{
    QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);

    m_itemIndex = itemTypes.indexOfService(service);
    m_aggregateIndex = -1;

    if (m_itemIndex != -1)
        m_itemType = itemTypes[m_itemIndex].itemType;
}

QString QGalleryTrackerSchema::buildIdQuery(int *error, const QString &itemId)
{
    QString query;

    if (m_itemIndex >= 0) {
        QXmlStreamWriter xml(&query);
        xml.writeStartElement(QLatin1String("rdfq:Condition"));

        qt_writeFileIdCondition(
                error ,&xml, qt_galleryItemTypeList[m_itemIndex].prefix.strip(itemId));

        xml.writeEndElement();
    } else if (m_aggregateIndex >= 0) {
        QXmlStreamWriter xml(&query);
        xml.writeStartElement(QLatin1String("rdfq:Condition"));

        qt_galleryAggregateTypeList[m_aggregateIndex].writeIdCondition(
                error, &xml, qt_galleryAggregateTypeList[m_aggregateIndex].prefix.strip(itemId));

        xml.writeEndElement();
    } else {
        *error = QGalleryAbstractRequest::InvalidItemError;
    }

    return query;
}

QString QGalleryTrackerSchema::buildContainerQuery(int *error, const QString &containerId)
{
    QString query;

    QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);
    int index = itemTypes.indexOfItemId(containerId);

    if (index != -1) {
        QXmlStreamWriter xml(&query);
        xml.writeStartElement(QLatin1String("rdfq:Condition"));

        qt_writeFileContainerCondition(error, &xml, itemTypes[index].prefix.strip(containerId));

        xml.writeEndElement();
    } else {
        QGalleryAggregateTypeList aggregateTypes(qt_galleryAggregateTypeList);
        index = aggregateTypes.indexOfItemId(containerId);

        if (index != -1) {
            QXmlStreamWriter xml(&query);
            xml.writeStartElement(QLatin1String("rdfq:Condition"));

            aggregateTypes[index].writeIdCondition(
                    error, &xml, aggregateTypes[index].prefix.strip(containerId));

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

        QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);
        int index = itemTypes.indexOfItemId(containerId);

        if (index != -1) {
            qt_writeFileScopeCondition(error, &xml, itemTypes[index].prefix.strip(containerId));
        } else {
            QGalleryAggregateTypeList aggregateTypes(qt_galleryAggregateTypeList);
            index = aggregateTypes.indexOfItemId((containerId));

            if (index != -1) {
                aggregateTypes[index].writeIdCondition(
                        error, &xml, aggregateTypes[index].prefix.strip(containerId));
            } else {
                *error = QGalleryAbstractRequest::InvalidItemError;
            }
        }
    }

    if (filter.type() != QGalleryFilter::Invalid) {
        if (m_itemIndex >= 0) {
            qt_writeCondition(
                    error,
                    &xml,
                    filter,
                    qt_galleryItemTypeList[m_itemIndex].itemProperties);
        } else if (m_aggregateIndex >= 0) {
            qt_writeCondition(
                    error,
                    &xml,
                    filter,
                    qt_galleryAggregateTypeList[m_aggregateIndex].properties);
        } else {
            *error = QGalleryAbstractRequest::ItemTypeError;
        }
    }

    if (!containerId.isEmpty())
        xml.writeEndElement();

    xml.writeEndElement();

    return query;
}

QString QGalleryTrackerSchema::uriFromItemId(int *error, const QVariant &itemId)
{
    QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);

    const QString idString = itemId.toString();

    int index = itemTypes.indexOfItemId(idString);

    if (index != -1) {
        return itemTypes[index].prefix.strip(idString).toString();
    } else {
        *error = QGalleryAbstractRequest::InvalidItemError;

        return QString();
    }
}

QString QGalleryTrackerSchema::itemIdFromUri(const QString &uri) const
{
    return m_itemIndex >= 0
            ? qt_galleryItemTypeList[m_itemIndex].prefix + uri
            : QString();
}

QString QGalleryTrackerSchema::service() const
{
    if (m_itemIndex >= 0)
        return qt_galleryItemTypeList[m_itemIndex].service;
    else if (m_aggregateIndex >= 0)
        return qt_galleryAggregateTypeList[m_aggregateIndex].service;
    else
        return QString();
}

int QGalleryTrackerSchema::updateMask() const
{
    if (m_itemIndex >= 0)
        return qt_galleryItemTypeList[m_itemIndex].updateMask;
    else if (m_aggregateIndex >= 0)
        return qt_galleryAggregateTypeList[m_aggregateIndex].updateMask;
    else
        return 0;
}

int QGalleryTrackerSchema::serviceUpdateId(const QString &service)
{
    QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);

    int index = itemTypes.indexOfService(service);

    return index != -1 ? itemTypes[index].updateId : FileId;
}

QStringList QGalleryTrackerSchema::supportedPropertyNames() const
{
    QStringList propertyNames;

    if (m_itemIndex >= 0) {
        const QGalleryItemType &type = qt_galleryItemTypeList[m_itemIndex];

        for (int i = 0; i < type.itemProperties.count; ++i)
            propertyNames.append(type.itemProperties[i].name);

        for (int i = 0; i < type.thumbnailProperties.count; ++i)
            propertyNames.append(type.thumbnailProperties[i].name);
    } else if (m_aggregateIndex >= 0) {
        const QGalleryAggregateType &type = qt_galleryAggregateTypeList[m_aggregateIndex];

        for (int i = 0; i < type.properties.count; ++i)
            propertyNames.append(type.properties[i].name);

        for (int i = 0; i< type.aggregateProperties.count; ++i)
            propertyNames.append(type.aggregateProperties[i].name);
    }
    return propertyNames;
}

QGalleryProperty::Attributes QGalleryTrackerSchema::propertyAttributes(
        const QString &propertyName) const
{
    if (m_itemIndex >= 0) {
        const QGalleryItemType &type = qt_galleryItemTypeList[m_itemIndex];

        const int index = type.itemProperties.indexOfProperty(propertyName);
        if (index != -1)
            return type.itemProperties[index].attributes;
    } else if (m_aggregateIndex >= 0) {
        const QGalleryAggregateType &type = qt_galleryAggregateTypeList[m_aggregateIndex];

        const int index = type.properties.indexOfProperty(propertyName);
        if (index != -1)
            return type.properties[index].attributes;
        else if (type.aggregateProperties.indexOfProperty(propertyName) != -1)
            return QGalleryProperty::CanRead;
    }
    return QGalleryProperty::Attributes();
}


QStringList QGalleryTrackerSchema::identityFields() const
{
    QStringList fields;

    if (m_aggregateIndex >= 0) {
        const QGalleryItemPropertyList &identity
                = qt_galleryAggregateTypeList[m_aggregateIndex].identity;

        for (int i = 0; i < identity.count; ++i)
            fields.append(identity[i].field);
    }
    return fields;
}

void QGalleryTrackerSchema::setPropertyNames(const QStringList &names)
{
    m_propertyNames = names;
}

void QGalleryTrackerSchema::setSortPropertyNames(const QStringList &names)
{
    m_sortPropertyNames = names;
}

QString QGalleryTrackerSchema::queryMethod() const
{
    if (m_itemIndex >= 0)
        return QLatin1String("Query");
    else if (m_aggregateIndex >= 0)
        return QLatin1String("GetUniqueValuesWithAggregates");
    else
        return QString();
}

QVariantList QGalleryTrackerSchema::queryArguments(const QString &query) const
{
    if (m_itemIndex >= 0) {
        const int liveQueryId = 0;
        const QString service = qt_galleryItemTypeList[m_itemIndex].service;
        const QString searchText;
        const QStringList keywords;
        const bool sortByService = false;
        const bool descendingSortOrder = !m_sortCriteria.isEmpty()
                && m_sortCriteria.first().flags & QGalleryTrackerSortCriteria::Descending;

        return QVariantList()
                << liveQueryId
                << service
                << m_propertyFields
                << searchText
                << keywords
                << query
                << sortByService
                << m_sortFields
                << descendingSortOrder;
    } else if (m_aggregateIndex >= 0) {
        const QString service = qt_galleryAggregateTypeList[m_aggregateIndex].service;
        const bool descendingSortOrder = !m_sortCriteria.isEmpty()
                && m_sortCriteria.first().flags & QGalleryTrackerSortCriteria::Descending;

        return QVariantList()
                << service
                << m_propertyFields
                << query
                << m_aggregations
                << m_aggregateFields
                << descendingSortOrder;
    } else {
        return QVariantList();
    }
}

QGalleryTrackerCompositeColumn *QGalleryTrackerSchema::createIdColumn() const
{
    if (m_itemIndex >= 0) {
        return new QGalleryTrackerServicePrefixColumn;
    } else if (m_aggregateIndex >= 0) {
        const QGalleryAggregateType &type = qt_galleryAggregateTypeList[m_aggregateIndex];

        if (type.identity.count == 1) {
            return new QGalleryTrackerPrefixColumn(0, type.prefix);
        } else if (type.identity.count == 2) {
            return new QGalleryTrackerCompositeIdColumn(m_identityColumns, type.prefix);
        }
    }
    return new QGalleryTrackerStaticColumn(QVariant());
}

QGalleryTrackerCompositeColumn *QGalleryTrackerSchema::createUrlColumn() const
{
    if (m_itemIndex >= 0) {
        return new QGalleryTrackerFileUrlColumn(0);
    } else {
        return new QGalleryTrackerStaticColumn(QUrl());
    }
}

QGalleryTrackerCompositeColumn *QGalleryTrackerSchema::createTypeColumn() const
{
    if (m_itemIndex >= 0) {
        return new QGalleryTrackerServiceTypeColumn;
    } else if (m_aggregateIndex >= 0) {
        return new QGalleryTrackerStaticColumn(QString(
                qt_galleryAggregateTypeList[m_aggregateIndex].itemType));
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

QVector<int> QGalleryTrackerSchema::aliasColumns() const
{
    return m_aliasColumns;
}

QVector<QGalleryTrackerImageColumn *> QGalleryTrackerSchema::createImageColumns(
        QGalleryDBusInterfaceFactory *dbus) const
{
    QVector<QGalleryTrackerImageColumn *> columns;
    columns.reserve(m_thumbnailTypes.count());

    for (int i = 0, count = m_thumbnailTypes.count(); i < count; ++i) {
        switch (m_thumbnailTypes.at(i)) {
        case QVariant::Image:
            columns.append(new QGalleryTrackerThumbnailImageColumn(dbus->thumbnailInterface(), i));
            break;
        case QVariant::Pixmap:
            columns.append(new QGalleryTrackerThumbnailPixmapColumn(dbus->thumbnailInterface(), i));
            break;
        default:
            break;
        }
    }

    return columns;
}

int QGalleryTrackerSchema::resolveColumns()
{
    if (m_itemIndex >= 0)
        return resolveFileColumns();
    else if (m_aggregateIndex >= 0)
        return resolveAggregateColumns();
    else
        return QGalleryAbstractRequest::ItemTypeError;
}

int QGalleryTrackerSchema::resolveFileColumns()
{
    QStringList propertyFields;
    QStringList valueNames;
    QStringList aliasNames;
    QStringList thumbnailNames;
    QVector<QGalleryProperty::Attributes> valueAttributes;
    QVector<QGalleryProperty::Attributes> aliasAttributes;
    QVector<QGalleryProperty::Attributes> thumbnailAttributes;
    QVector<QVariant::Type> valueTypes;
    QVector<int> aliasColumns;
    QVector<QVariant::Type> thumbnailTypes;
    QVector<QGalleryTrackerSortCriteria> sortCriteria;

    const QGalleryItemPropertyList &itemProperties
            = qt_galleryItemTypeList[m_itemIndex].itemProperties;
    const QGalleryThumbnailPropertyList &thumbnailProperties
            = qt_galleryItemTypeList[m_itemIndex].thumbnailProperties;

    m_propertyNames.removeDuplicates();

    for (QStringList::const_iterator it = m_propertyNames.constBegin();
            it != m_propertyNames.constEnd();
            ++it) {
        int propertyIndex = itemProperties.indexOfProperty(*it);

        if (propertyIndex >= 0) {
            const QString field = itemProperties[propertyIndex].field;

            int fieldIndex = propertyFields.indexOf(field);

            if (fieldIndex >= 0) {
                aliasColumns.append(fieldIndex + 2);
                aliasNames.append(*it);
                aliasAttributes.append(itemProperties[propertyIndex].attributes);
            } else {
                propertyFields.append(field);
                valueNames.append(*it);
                valueAttributes.append(itemProperties[propertyIndex].attributes);
                valueTypes.append(QVariant::String);
            }
        } else if ((propertyIndex = thumbnailProperties.indexOfProperty(*it)) >= 0) {
            thumbnailTypes.append(thumbnailProperties[propertyIndex].type);
            thumbnailNames.append(*it);
            thumbnailAttributes.append(QGalleryProperty::CanRead);
        } else if (*it == QLatin1String("filePath")) {
            aliasColumns.append(0);
            aliasNames.append(*it);
            aliasAttributes.append(QGalleryProperty::CanRead);
        } else {
            qWarning("Unknown column: %s", qPrintable(*it));
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

        const int propertyIndex = itemProperties.indexOfProperty(propertyName);

        if (propertyIndex >= 0
                && itemProperties[propertyIndex].attributes & QGalleryProperty::CanSort) {
            const QString field = itemProperties[propertyIndex].field;

            int fieldIndex = propertyFields.indexOf(field);

            if (fieldIndex < 0) {
                fieldIndex = propertyFields.count();
                propertyFields.append(field);
                valueNames.append(propertyName);
                valueAttributes.append(itemProperties[propertyIndex].attributes);
                valueTypes.append(QVariant::String);
            }

            sortCriteria.append(QGalleryTrackerSortCriteria(fieldIndex + 2, sortFlags));
        }
    }

    m_propertyFields = propertyFields;
    m_aggregateFields = QStringList();
    m_aggregations = QStringList();
    m_propertyNames = valueNames + aliasNames + thumbnailNames;
    m_propertyAttributes = valueAttributes + aliasAttributes + thumbnailAttributes;
    m_valueTypes = valueTypes;
    m_compositeColumns = QVector<int>();
    m_aliasColumns = aliasColumns;
    m_thumbnailTypes = thumbnailTypes;
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


    const QGalleryAggregateType &type = qt_galleryAggregateTypeList[m_aggregateIndex];
    const QGalleryItemPropertyList &properties = type.properties;
    const QGalleryAggregatePropertyList &aggregateProperties = type.aggregateProperties;

    m_propertyNames.removeDuplicates();

    for (int i = 0; i < type.identity.count; ++i) {
        const QString propertyName = type.identity[i].name;

        identityNames.append(propertyName);

        m_propertyNames.removeAll(propertyName);
    }

    qSort(identityNames.begin(), identityNames.end(), QGalleryReorderLessThan(m_sortPropertyNames));

    for (int i = 0; i < type.identity.count; ++i)
        identityColumns.append(identityNames.indexOf(type.identity[i].name));

    for (QStringList::const_iterator it = identityNames.constBegin();
            it != identityNames.constEnd();
            ++it) {
        int propertyIndex = properties.indexOfProperty(*it);
        Q_ASSERT(propertyIndex >= 0);

        const QString field = properties[propertyIndex].field;
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
            const QString field = properties[propertyIndex].field;

            int fieldIndex = identityFields.indexOf(field);
            Q_ASSERT(fieldIndex != -1);

            aliasColumns.append(fieldIndex);
            aliasNames.append(*it);
            aliasAttributes.append(properties[propertyIndex].attributes);
        } else if ((propertyIndex = aggregateProperties.indexOfProperty(*it)) >= 0) {
            aggregateNames.append(*it);
            aggregateFields.append(aggregateProperties[propertyIndex].field);
            aggregates.append(aggregateProperties[propertyIndex].aggregate);
            aggregateAttributes.append(QGalleryProperty::CanRead);
            aggregateTypes.append(QVariant::String);
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
    m_thumbnailTypes = QVector<QVariant::Type>();
    m_sortCriteria = sortCriteria;

    return QGalleryAbstractRequest::Succeeded;
}

QTM_END_NAMESPACE
