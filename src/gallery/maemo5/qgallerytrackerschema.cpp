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
#include "qgallerydbusinterface_p.h"
#include "qgallerytrackercountresponse_p.h"
#include "qgallerytrackeritemlist_p.h"
#include "qgallerytrackerthumbnailcolumn_p.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/qxmlstream.h>

QTM_BEGIN_NAMESPACE

namespace
{

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


    struct QGalleryItemProperty
    {
        QLatin1String name;
        QLatin1String field;
        QVariant::Type type;
        QGalleryProperty::Attributes attributes;
    };

    typedef QGalleryPropertyList<QGalleryItemProperty> QGalleryItemPropertyList;

    struct QGalleryAggregateProperty
    {
        QLatin1String name;
        QLatin1String field;
        QLatin1String aggregate;
        QVariant::Type type;
    };

    typedef QGalleryPropertyList<QGalleryAggregateProperty> QGalleryAggregatePropertyList;

    struct QGalleryThumbnailProperty
    {
        QLatin1String name;
        QLatin1String profile;
        QVariant::Type type;
        QGalleryItemPropertyList dependencies;
        QGalleryTrackerImageColumn *(*createColumn)(
                QGalleryDBusInterfaceFactory *dbus,
                int key,
                const QString &profile,
                const QVector<int> &columns);
    };

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
        CanRead         = QGalleryProperty::CanRead,
        CanWrite        = QGalleryProperty::CanWrite,
        CanSort         = QGalleryProperty::CanSort,
        CanFilter       = QGalleryProperty::CanFilter,
        IsResource      = 0x100,
        PropertyMask    = 0xFF
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

#define QT_GALLERY_ITEM_PROPERTY(PropertyName, Field, Type, Attr) \
{ QLatin1String(PropertyName), QLatin1String(Field), QVariant::Type, QGalleryProperty::Attributes(Attr) }

#define QT_GALLERY_AGGREGATE_PROPERTY(PropertyName, Field, Aggregate, Type) \
{ QLatin1String(PropertyName), QLatin1String(Field), QLatin1String(Aggregate), QVariant::Type }

#define QT_GALLERY_THUMBNAIL_PROPERTY(PropertyName, Profile, Type, Dependencies, Factory) \
{ \
    QLatin1String(PropertyName), \
    QLatin1String(Profile), \
    QVariant::Type, \
    QGalleryItemPropertyList(Dependencies), \
    Factory \
}

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
    xml->writeCharacters(value.toDateTime().toString(Qt::ISODate));
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
        xml->writeStartElement(QLatin1String("rdfq:equals"));
        value = QLatin1Char('*') + value.toString();
        break;
    case Qt::MatchRegExp:
        xml->writeStartElement(QLatin1String("rdfq:regex"));
        break;
    case Qt::MatchWildcard:
        xml->writeStartElement(QLatin1String("rdfq:equals"));
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

#define QT_GALLERY_FILE_MIMETYPE_PROPERTY \
    QT_GALLERY_ITEM_PROPERTY("mimeType"    , "File:Mime"     , String, CanRead | CanSort | CanFilter | IsResource)

// These are repeated in a few lists.
#define QT_GALLERY_FILE_PROPERTYS \
    QT_GALLERY_FILE_MIMETYPE_PROPERTY, \
    QT_GALLERY_ITEM_PROPERTY("fileName"    , "File:Name"     , String  , CanRead | CanSort | CanFilter), \
  /*QT_GALLERY_ITEM_PROPERTY(""            , "File:Contents" , String  , QGalleryProperty::Attributes()),*/ \
  /*QT_GALLERY_ITEM_PROPERTY(""            , "File:Link"     , String  , QGalleryProperty::Attributes()),*/ \
    QT_GALLERY_ITEM_PROPERTY("fileSize"    , "File:Size"     , Int     , QGalleryProperty::Attributes()), \
  /*QT_GALLERY_ITEM_PROPERTY(""            , "File:License"  , String  , QGalleryProperty::Attributes()),*/ \
    QT_GALLERY_ITEM_PROPERTY("copyright"   , "File:Copyright", String  , CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("lastModified", "File:Modified" , DateTime, CanRead | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("lastAccessed", "File:Accessed" , DateTime, CanRead | CanSort | CanFilter), \
  /*QT_GALLERY_ITEM_PROPERTY(""            , "File:Added"    , DateTime, QGalleryProperty::Attributes()),*/ \
  /*QT_GALLERY_ITEM_PROPERTY(""            , "File:Other"    , String  ,   QGalleryProperty::Attributes()),*/ \
    QT_GALLERY_ITEM_PROPERTY("language"    , "File:Language" , String  , CanRead | CanSort | CanFilter)

static const QGalleryItemProperty qt_galleryFilePropertyList[] =
{
    QT_GALLERY_FILE_PROPERTYS,
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Contributor", String    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Coverage"   , String    , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("author"     , "DC:Creator"    , String    , CanRead | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Date"       , DateTime  , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("description", "DC:Description", String    , CanRead | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Format"     , String    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Identifier" , String    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Language"   , String    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Publisher"  , String    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Relation"   , String    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Rights"     , String    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Source"     , String    , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("subject"    , "DC:Subject"    , String    , CanRead | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Keywords"   , String    , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("title"      , "DC:Title"      , String    , CanRead | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""         , "DC:Type"       , String    , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("rating"     , "User:Rank"     , String    , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("keywords"   , "User:Keywords" , StringList, CanRead | CanSort | CanFilter)
};

static const QGalleryItemProperty qt_galleryFileThumbnailDependencyList[] =
{
    QT_GALLERY_FILE_MIMETYPE_PROPERTY
};

#ifdef Q_WS_MAEMO_5
#define QT_GALLERY_THUMBNAIL_NORMAL_PROFILE "cropped"
#else
#define QT_GALLERY_THUMBNAIL_NORMAL_PROFILE "normal"
#endif

static const QGalleryThumbnailProperty qt_galleryFileThumbnailPropertyList[] =
{
#ifndef Q_WS_MAEMO_5
    QT_GALLERY_THUMBNAIL_PROPERTY(
            "previewImage",
            "large",
            Image,
            qt_galleryFileThumbnailDependencyList,
            QGalleryTrackerThumbnailColumn::createImageColumn),
    QT_GALLERY_THUMBNAIL_PROPERTY(
            "previewPixmap",
            "large",
            Pixmap,
            qt_galleryFileThumbnailDependencyList,
            QGalleryTrackerThumbnailColumn::createPixmapColumn),
#endif
    QT_GALLERY_THUMBNAIL_PROPERTY(
            "thumbnailImage",
            QT_GALLERY_THUMBNAIL_NORMAL_PROFILE,
            Image,
            qt_galleryFileThumbnailDependencyList,
            QGalleryTrackerThumbnailColumn::createImageColumn),
    QT_GALLERY_THUMBNAIL_PROPERTY(
            "thumbnailPixmap",
            QT_GALLERY_THUMBNAIL_NORMAL_PROFILE,
            Pixmap,
            qt_galleryFileThumbnailDependencyList,
            QGalleryTrackerThumbnailColumn::createPixmapColumn)

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

#define QT_GALLERY_AUDIO_ARTIST_PROPERTY \
    QT_GALLERY_ITEM_PROPERTY("artist"      , "Audio:Artist"     , String, CanRead | CanWrite | CanSort | CanFilter)
#define QT_GALLERY_AUDIO_ALBUMTITLE_PROPERTY \
    QT_GALLERY_ITEM_PROPERTY("albumTitle"  , "Audio:Album"      , String    , CanRead | CanWrite | CanSort | CanFilter)
#define QT_GALLERY_AUDIO_ALBUMARTIST_PROPERTY \
    QT_GALLERY_ITEM_PROPERTY("albumArtist" , "Audio:AlbumArtist", String    , CanRead | CanWrite | CanSort | CanFilter)

static const QGalleryItemProperty qt_galleryAudioPropertyList[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_AUDIO_ARTIST_PROPERTY,
    QT_GALLERY_AUDIO_ALBUMTITLE_PROPERTY,
    QT_GALLERY_AUDIO_ALBUMARTIST_PROPERTY,
    QT_GALLERY_ITEM_PROPERTY("title"       , "Audio:Title"        , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("genre"       , "Audio:Genre"        , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("duration"    , "Audio:Duration"     , Int       ,  CanRead | CanSort | CanFilter | IsResource),
//    QT_GALLERY_ITEM_PROPERTY(""          , "Audio:ReleaseDate"  , DateTime  , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("trackNumber" , "Audio:TrackNo"      , Int       , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("discNumber"  , "Audio:DiscNo"       , Int       , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("performer"   , "Audio:Performer"    , String    , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""          , "Audio:TrackGain"    , Double    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""          , "Audio:AlbumGain"    , Double    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""          , "Audio:AlbumPeakGain", Double    , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("description" , "Audio:Comment"      , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("audioCodec"  , "Audio:Codec"        , String    , CanRead | CanSort | CanFilter | IsResource),
//  QT_GALLERY_ITEM_PROPERTY(""            , "Audio:CodecVersion" , String    , CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("sampleRate"  , "Audio:SampleRate"   , Int       , CanRead | CanWrite | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("audioBitRate", "Audio:Bitrate"      , Int       , CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("channelCount", "Audio:Channels"     , Int       , CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("lastPlayed"  , "Audio:LastPlay"     , DateTime  , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("playCount"   , "Audio:PlayCount"    , Int       , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""          , "Audio:DateAdded"    , DateTime  , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("lyrics"      , "Audio:Lyrics"       , String    , CanRead | CanWrite | CanSort | CanFilter)
};

///////////
// Playlist
///////////

static const QGalleryItemProperty qt_galleryPlaylistPropertyList[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_ITEM_PROPERTY("duration"  , "Playlist:Duration"     , Int, CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("trackCount", "Playlist:Songs"        , Int, CanRead | CanSort | CanFilter)
//    QT_GALLERY_ITEM_PROPERTY(""        , "Playlist:Name"         , String, QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""        , "Playlist:ValidDuration", String, QGalleryProperty::Attributes())
};

////////
// Image
////////

static const QGalleryItemProperty qt_galleryImagePropertyList[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_ITEM_PROPERTY("title"             , "Image:Title"          , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("keywords"          , "Image:Keywords"       , StringList, CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("height"            , "Image:Height"         , Int       , CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("width"             , "Image:Width"          , Int       , CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("album"             , "Image:Album"          , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("dateTaken"         , "Image:Date"           , DateTime  , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("author"            , "Image:Creator"        , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("comment"           , "Image:Comments"       , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("description"       , "Image:Description"    , String    , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:Software"       , String    , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("cameraManufacturer", "Image:CameraMake"     , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("cameraModel"       , "Image:CameraModel"    , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("orientation"       , "Image:Orientation"    , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("exposureProgram"   , "Image:ExposureProgram", String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("exposureTime"      , "Image:ExposureTime"   , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("fNumber"           , "Image:FNumber"        , Int       , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("flashEnabled"      , "Image:Flash"          , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("focalLength"       , "Image:FocalLength"    , Double    , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:ISOSpeed"       , Double    , QGalleryProperty::Attributes()),
    QT_GALLERY_ITEM_PROPERTY("meteringMode"      , "Image:MeteringMode"   , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("whiteBalance"      , "Image:WhiteBalance"   , Double    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("rating"            , "Image:Rating"         , Int       , CanRead | CanWrite | CanSort | CanFilter),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:Location"       , String    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:Sublocation"    , String    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:Country"        , String    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:City"           , String    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""                , "Image:HasKeywords"    , String    , QGalleryProperty::Attributes())
};

////////
// Video
////////

static const QGalleryItemProperty qt_galleryVideoPropertyList[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_ITEM_PROPERTY("title"         , "Video:Title"          , String, CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("author"        , "Video:Author"         , String, CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("height"        , "Video:Height"         , Int   , CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("width"         , "Video:Width"          , Int   , CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("duration"      , "Video:Duration"       , Int   , CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("comments"      , "Video:Comments"       , String, CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("frameRate"     , "Video:FrameRate"      , Double, CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("videoCodec"    , "Video:Codec"          , String, CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("videoBitRate"  , "Video:Bitrate"        , Int   , CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("playCount"     , "Video:PlayCount"      , Int   , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("resumePosition", "Video:PausePosition"  , Int   , CanRead | CanWrite | CanSort | CanFilter)
//    QT_GALLERY_ITEM_PROPERTY(""            , "Video:LastPlayedFrame", Int   , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""            , "Video:Source"         , String, QGalleryProperty::Attributes())
};

///////////
// Document
///////////

static const QGalleryItemProperty qt_galleryDocumentPropertyList[] =
{
    QT_GALLERY_FILE_PROPERTYS,
    QT_GALLERY_ITEM_PROPERTY("title"    , "Doc:Title"    , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("subject"  , "Doc:Subject"  , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("author"   , "Doc:Author"   , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("keywords" , "Doc:Keywords" , StringList, CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("comment"  , "Doc:Comments" , String    , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("pageCount", "Doc:PageCount", Int       , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("wordCount", "Doc:WordCount", Int       , CanRead | CanSort | CanFilter)
//    QT_GALLERY_ITEM_PROPERTY(""       , "Doc:Creator"  , String    , QGalleryProperty::Attributes()),
//    QT_GALLERY_ITEM_PROPERTY(""       , "Doc:URL"      , String    , QGalleryProperty::Attributes())
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
    QT_GALLERY_ITEM_PROPERTY("title", "Audio:Artist", String, CanRead | CanFilter)
};

static const QGalleryItemProperty qt_galleryArtistPropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("artist", "Audio:Artist", String, CanRead | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("title" , "Audio:Artist", String, CanRead | CanFilter),
};

static const QGalleryAggregateProperty qt_galleryArtistAggregateList[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", "SUM"  , Int),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , "COUNT", Int),
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
    QT_GALLERY_ITEM_PROPERTY("title", "Audio:AlbumArtist", String, CanRead | CanFilter | CanSort)
};

static const QGalleryItemProperty qt_galleryAlbumArtistPropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("artist", "Audio:AlbumArtist", String, CanRead | CanFilter | CanSort),
    QT_GALLERY_ITEM_PROPERTY("title" , "Audio:AlbumArtist", String, CanRead | CanFilter | CanSort),
};

static const QGalleryAggregateProperty qt_galleryAlbumArtistAggregateList[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", "SUM"  , Int),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , "COUNT", Int),
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
    QT_GALLERY_ITEM_PROPERTY("artist", "Audio:AlbumArtist", String, CanRead | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("title" , "Audio:Album"      , String, CanRead | CanFilter)
};

static const QGalleryItemProperty qt_galleryAlbumPropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("title" , "Audio:Album"      , String, CanRead | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("artist", "Audio:AlbumArtist", String, CanRead | CanFilter)
};

static const QGalleryAggregateProperty qt_galleryAlbumAggregateList[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", "SUM"  , Int),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , "COUNT", Int),
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
    QT_GALLERY_ITEM_PROPERTY("title", "Audio:Genre", String, CanRead | CanFilter)
};

static const QGalleryItemProperty qt_galleryAudioGenrePropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("genre", "Audio:Genre", String, CanRead | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("title", "Audio:Genre", String, CanRead | CanFilter),
};

static const QGalleryAggregateProperty qt_galleryAudioGenreAggregateList[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", "SUM"  , Int),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , "COUNT", Int),
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
    QT_GALLERY_ITEM_PROPERTY("title", "Image:Album", String, CanRead | CanFilter)
};

static const QGalleryItemProperty qt_galleryPhotoAlbumPropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("title", "Image:Album", String, CanRead | CanFilter),
};

static const QGalleryAggregateProperty qt_galleryPhotoAlbumAggregateList[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*", "COUNT", Int)
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

QGalleryTrackerSchema::QGalleryTrackerSchema(const QString &itemType)
    : m_itemIndex(QGalleryItemTypeList(qt_galleryItemTypeList).indexOfType(itemType))
    , m_aggregateIndex(m_itemIndex == -1
            ? QGalleryAggregateTypeList(qt_galleryAggregateTypeList).indexOfType(itemType)
            : -1)
{
}

QGalleryTrackerSchema::~QGalleryTrackerSchema()
{
}

QGalleryTrackerSchema QGalleryTrackerSchema::fromItemId(const QString &itemId)
{
    const QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);

    const int itemIndex = itemTypes.indexOfItemId(itemId);

    if (itemIndex != -1) {
        return QGalleryTrackerSchema(itemIndex, -1);
    } else {
        QGalleryAggregateTypeList aggregateTypes(qt_galleryAggregateTypeList);

        const int aggregateIndex = aggregateTypes.indexOfItemId(itemId);

        return aggregateIndex != -1
                ? QGalleryTrackerSchema(-1, aggregateIndex)
                : QGalleryTrackerSchema(-1, -1);
    }
}

QGalleryTrackerSchema QGalleryTrackerSchema::fromService(const QString &service)
{
    const QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);

    const int itemIndex = itemTypes.indexOfService(service);

    return itemIndex != -1
            ? QGalleryTrackerSchema(itemIndex, -1)
            : QGalleryTrackerSchema(-1, -1);
}

QString QGalleryTrackerSchema::itemType() const
{
    if (m_itemIndex >= 0)
        return qt_galleryItemTypeList[m_itemIndex].itemType;
    else if (m_aggregateIndex >= 0)
        return qt_galleryAggregateTypeList[m_aggregateIndex].itemType;
    else
        return QString();
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
            return type.itemProperties[index].attributes & PropertyMask;
    } else if (m_aggregateIndex >= 0) {
        const QGalleryAggregateType &type = qt_galleryAggregateTypeList[m_aggregateIndex];

        const int index = type.properties.indexOfProperty(propertyName);
        if (index != -1)
            return type.properties[index].attributes & PropertyMask;
        else if (type.aggregateProperties.indexOfProperty(propertyName) != -1)
            return QGalleryProperty::CanRead;
    }
    return QGalleryProperty::Attributes();
}

int QGalleryTrackerSchema::prepareIdResponse(
        QGalleryTrackerItemListArguments *arguments,
        QGalleryDBusInterfaceFactory *dbus,
        const QString &itemId,
        const QStringList &propertyNames) const
{
    if (m_itemIndex >= 0) {
        int result = QGalleryAbstractRequest::Succeeded;

        QString query;
        {
            QXmlStreamWriter xml(&query);
            xml.writeStartElement(QLatin1String("rdfq:Condition"));

            qt_writeFileIdCondition(
                    &result ,&xml, qt_galleryItemTypeList[m_itemIndex].prefix.strip(itemId));

            xml.writeEndElement();
        }

        if (result == QGalleryAbstractRequest::Succeeded)
            populateItemArguments(arguments, dbus, query, propertyNames, QStringList());

        return result;
    } else if (m_aggregateIndex >= 0) {
        int result = QGalleryAbstractRequest::Succeeded;

        QString query;
        {
            QXmlStreamWriter xml(&query);
            xml.writeStartElement(QLatin1String("rdfq:Condition"));

            qt_galleryAggregateTypeList[m_aggregateIndex].writeIdCondition(
                    &result,
                    &xml,
                    qt_galleryAggregateTypeList[m_aggregateIndex].prefix.strip(itemId));

            xml.writeEndElement();
        }

        if (result == QGalleryAbstractRequest::Succeeded)
            populateAggregateArguments(arguments, dbus, query, propertyNames, QStringList());

        return result;
    } else {
        return QGalleryAbstractRequest::InvalidItemError;
    }
}

int QGalleryTrackerSchema::prepareFilterResponse(
        QGalleryTrackerItemListArguments *arguments,
        QGalleryDBusInterfaceFactory *dbus,
        QGalleryAbstractRequest::Scope scope,
        const QString &scopeItemId,
        const QGalleryFilter &filter,
        const QStringList &propertyNames,
        const QStringList &sortPropertyNames) const
{
    int result = QGalleryAbstractRequest::Succeeded;

    QString query;

    if (!scopeItemId.isEmpty() || filter.isValid())
        result = buildFilterQuery(&query, scope, scopeItemId, filter);

    if (result != QGalleryAbstractRequest::Succeeded) {
        return result;
    } else if (m_itemIndex >= 0) {
        populateItemArguments(arguments, dbus, query, propertyNames, sortPropertyNames);

        return QGalleryAbstractRequest::Succeeded;
    } else if (m_aggregateIndex >= 0) {
        populateAggregateArguments(arguments, dbus, query, propertyNames, sortPropertyNames);

        return QGalleryAbstractRequest::Succeeded;
    } else {
        return QGalleryAbstractRequest::ItemTypeError;
    }
}

int QGalleryTrackerSchema::prepareCountResponse(
        QGalleryTrackerCountResponseArguments *arguments,
        QGalleryDBusInterfaceFactory *dbus,
        QGalleryAbstractRequest::Scope scope,
        const QString &scopeItemId,
        const QGalleryFilter &filter) const
{
    int result = QGalleryAbstractRequest::Succeeded;

    QString query;

    if (!scopeItemId.isEmpty() || filter.isValid())
        result = buildFilterQuery(&query, scope, scopeItemId, filter);

    if (result != QGalleryAbstractRequest::Succeeded) {
        return result;
    } else if (m_itemIndex >= 0) {
        arguments->accumulative = false;
        arguments->updateMask = qt_galleryItemTypeList[m_itemIndex].updateMask;
        arguments->queryInterface = dbus->metaDataInterface();
        arguments->queryMethod = QLatin1String("GetCount");
        arguments->queryArguments = QVariantList()
                << qt_galleryItemTypeList[m_itemIndex].service
                << QLatin1String("*")
                << query;

        return QGalleryAbstractRequest::Succeeded;
    } else if (m_aggregateIndex >= 0) {
        const QGalleryAggregateType &type = qt_galleryAggregateTypeList[m_aggregateIndex];

        if (type.identity.count == 1) {
            const QString countField = type.identity[0].field;

            arguments->accumulative = false;
            arguments->queryInterface = dbus->metaDataInterface();
            arguments->queryMethod = QLatin1String("GetCount");
            arguments->queryArguments = QVariantList()
                    << type.service
                    << countField
                    << query;
        } else {
            QStringList identity;
            for (int i = 0; i < type.identity.count - 1; ++i)
                identity.append(type.identity[i].field);
            const QString countField = type.identity[type.identity.count - 1].field;
            const bool descending = false;

            arguments->accumulative = true;
            arguments->updateMask = type.updateMask;
            arguments->queryInterface = dbus->metaDataInterface();
            arguments->queryMethod = QLatin1String("GetUniqueValuesWithCount");
            arguments->queryArguments = QVariantList()
                    << type.service
                    << identity
                    << query
                    << countField
                    << descending;
        }

        return QGalleryAbstractRequest::Succeeded;
    } else {
        return QGalleryAbstractRequest::ItemTypeError;
    }
}

int QGalleryTrackerSchema::buildFilterQuery(
        QString *query,
        QGalleryAbstractRequest::Scope scope,
        const QString &scopeItemId,
        const QGalleryFilter &filter) const
{
    const QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);
    const QGalleryAggregateTypeList aggregateTypes(qt_galleryAggregateTypeList);

    int result = QGalleryAbstractRequest::Succeeded;

    QXmlStreamWriter xml(query);
    xml.writeStartElement(QLatin1String("rdfq:Condition"));

    if (!scopeItemId.isEmpty()) {
        if (filter.isValid())
            xml.writeStartElement(QLatin1String("rdfq:and"));

        int index;

        if ((index = itemTypes.indexOfItemId(scopeItemId)) != -1) {
            switch (scope) {
            case QGalleryAbstractRequest::AllDescendants:
                qt_writeFileScopeCondition(
                        &result, &xml, itemTypes[index].prefix.strip(scopeItemId));
                break;
            case QGalleryAbstractRequest::DirectDescendants:
                qt_writeFileContainerCondition(
                        &result, &xml, itemTypes[index].prefix.strip(scopeItemId));
                break;
            default:
                result = QGalleryAbstractRequest::InvalidPropertyError;
                break;
            }
        } else if ((index = aggregateTypes.indexOfItemId((scopeItemId))) != -1) {
            aggregateTypes[index].writeIdCondition(
                    &result, &xml, aggregateTypes[index].prefix.strip(scopeItemId));
        } else {
            result = QGalleryAbstractRequest::InvalidItemError;
        }
    }

    if (filter.isValid()) {
        if (m_itemIndex != -1) {
            qt_writeCondition(
                    &result,
                    &xml,
                    filter,
                    itemTypes[m_itemIndex].itemProperties);
        } else if (m_aggregateIndex != -1) {
            qt_writeCondition(
                    &result,
                    &xml,
                    filter,
                    aggregateTypes[m_aggregateIndex].properties);
        } else {
            result = QGalleryAbstractRequest::ItemTypeError;
        }
    }

    if (!scopeItemId.isEmpty() && filter.isValid())
        xml.writeEndElement();

    xml.writeEndElement();

    return result;
}

static QVector<QGalleryTrackerValueColumn *> qt_createValueColumns(
        const QVector<QVariant::Type> &types)
{
    QVector<QGalleryTrackerValueColumn *> columns;

    columns.reserve(types.count());

    for (int i = 0, count = types.count(); i < count; ++i) {
        switch (types.at(i)) {
        case QVariant::String:
            columns.append(new QGalleryTrackerStringColumn);
            break;
        case QVariant::StringList:
            columns.append(new QGalleryTrackerStringListColumn);
            break;
        case QVariant::Int:
            columns.append(new QGalleryTrackerIntegerColumn);
            break;
        case QVariant::Double:
            columns.append(new QGalleryTrackerDoubleColumn);
            break;
        case QVariant::DateTime:
            columns.append(new QGalleryTrackerDateTimeColumn);
            break;
        default:
            Q_ASSERT(false);
            break;
        }
    }

    return columns;
}

static QVector<QGalleryTrackerImageColumn *> qt_createImageColumns(
        QGalleryDBusInterfaceFactory *dbus,
        const QVector<const QGalleryThumbnailProperty *> &properties,
        const QVector<QVector<int> > &columns,
        int keyOffset)
{
    QVector<QGalleryTrackerImageColumn *> imageColumns;
    imageColumns.reserve(properties.count());

    for (int i = 0; i < properties.count(); ++i) {
        imageColumns.append(properties.at(i)->createColumn(
                dbus, keyOffset + i, properties.at(i)->profile, columns.at(i)));
    }

    return imageColumns;
}

void QGalleryTrackerSchema::populateItemArguments(
        QGalleryTrackerItemListArguments *arguments,
        QGalleryDBusInterfaceFactory *dbus,
        const QString &query,
        const QStringList &propertyNames,
        const QStringList &sortPropertyNames) const
{
    const int liveQueryId = 0;
    const QString service = qt_galleryItemTypeList[m_itemIndex].service;
    const QString searchText;
    const QStringList keywords;
    const bool sortByService = false;

    QStringList valueNames;
    QStringList aliasNames;
    QStringList thumbnailNames;
    QStringList sortFieldNames;
    QVector<QGalleryProperty::Attributes> valueAttributes;
    QVector<QGalleryProperty::Attributes> aliasAttributes;
    QVector<QGalleryProperty::Attributes> thumbnailAttributes;
    QVector<QVariant::Type> valueTypes;
    QVector<QVariant::Type> aliasTypes;
    QVector<QVariant::Type> thumbnailTypes;
    QVector<const QGalleryThumbnailProperty *> imageProperties;
    QVector<QVector<int> > imageColumns;

    const QGalleryItemPropertyList &itemProperties
            = qt_galleryItemTypeList[m_itemIndex].itemProperties;
    const QGalleryThumbnailPropertyList &thumbnailProperties
            = qt_galleryItemTypeList[m_itemIndex].thumbnailProperties;

    for (QStringList::const_iterator it = propertyNames.begin(); it != propertyNames.end(); ++it) {
        if (valueNames.contains(*it) || aliasNames.contains(*it) || thumbnailNames.contains(*it))
            continue;

        int propertyIndex = itemProperties.indexOfProperty(*it);

        if (propertyIndex >= 0) {
            const QString field = itemProperties[propertyIndex].field;

            int fieldIndex = arguments->fieldNames.indexOf(field);

            if (fieldIndex >= 0) {
                arguments->aliasColumns.append(fieldIndex + 2);
                aliasNames.append(*it);
                aliasAttributes.append(itemProperties[propertyIndex].attributes);
                aliasTypes.append(itemProperties[propertyIndex].type);
            } else {
                arguments->fieldNames.append(field);
                valueNames.append(*it);
                valueAttributes.append(itemProperties[propertyIndex].attributes);
                valueTypes.append(itemProperties[propertyIndex].type);
            }
        } else if ((propertyIndex = thumbnailProperties.indexOfProperty(*it)) >= 0) {
            const QGalleryItemPropertyList &dependencies
                    = thumbnailProperties[propertyIndex].dependencies;

            QVector<int> columns;
            for (int i = 0; i < dependencies.count; ++i) {
                const QString field = dependencies[i].field;

                int fieldIndex = arguments->fieldNames.indexOf(field);

                if (fieldIndex >= 0) {
                    columns.append(fieldIndex + 2);
                } else {
                    columns.append(arguments->fieldNames.count() + 2);

                    arguments->fieldNames.append(field);
                    valueNames.append(dependencies[i].name);
                    valueAttributes.append(dependencies[i].attributes);
                    valueTypes.append(dependencies[i].type);
                }
            }

            thumbnailNames.append(*it);
            thumbnailAttributes.append(QGalleryProperty::CanRead);
            thumbnailTypes.append(thumbnailProperties[propertyIndex].type);
            imageProperties.append(&thumbnailProperties[propertyIndex]);
            imageColumns.append(columns);
        } else if (*it == QLatin1String("filePath")) {
            arguments->aliasColumns.append(0);
            aliasNames.append(*it);
            aliasAttributes.append(QGalleryProperty::CanRead);
        } else {
            qWarning("Unknown column: %s", qPrintable(*it));
        }
    }

    const bool descending = !sortPropertyNames.isEmpty()
            && sortPropertyNames.first().startsWith(QLatin1Char('-'));

    for (QStringList::const_iterator it = sortPropertyNames.constBegin();
            it != sortPropertyNames.constEnd();
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

            int fieldIndex = arguments->fieldNames.indexOf(field);

            if (fieldIndex < 0) {
                fieldIndex = arguments->fieldNames.count();
                arguments->fieldNames.append(field);
                valueNames.append(propertyName);
                valueAttributes.append(itemProperties[propertyIndex].attributes);
                valueTypes.append(itemProperties[propertyIndex].type);
            }

            sortFieldNames.append(field);
            arguments->sortCriteria.append(QGalleryTrackerSortCriteria(fieldIndex + 2, sortFlags));
        }
    }

    arguments->updateMask = qt_galleryItemTypeList[m_itemIndex].updateMask;
    arguments->identityWidth = 2;
    arguments->valueOffset = 2;
    arguments->queryInterface = dbus->searchInterface();
    arguments->queryMethod = QLatin1String("Query");
    arguments->queryArguments = QVariantList()
            << liveQueryId
            << service
            << arguments->fieldNames
            << searchText
            << keywords
            << query
            << sortByService
            << sortFieldNames
            << descending;
    arguments->idColumn = new QGalleryTrackerServicePrefixColumn;
    arguments->urlColumn = new QGalleryTrackerFileUrlColumn(0);
    arguments->typeColumn = new QGalleryTrackerServiceTypeColumn;
    arguments->valueColumns = qt_createValueColumns(valueTypes);
    arguments->imageColumns = qt_createImageColumns(
            dbus, imageProperties, imageColumns, 2 + valueNames.count() + aliasNames.count());
    arguments->propertyNames = valueNames + aliasNames + thumbnailNames;
    arguments->propertyAttributes = valueAttributes + aliasAttributes + thumbnailAttributes;
    arguments->propertyTypes = valueTypes + aliasTypes + thumbnailTypes;

    for (int i = 0; i < arguments->propertyAttributes.count(); ++i) {
        if (arguments->propertyAttributes.at(i) & IsResource)
            arguments->resourceKeys.append(i + 2);
        arguments->propertyAttributes[i] &= PropertyMask;
    }
}

void QGalleryTrackerSchema::populateAggregateArguments(
        QGalleryTrackerItemListArguments *arguments,
        QGalleryDBusInterfaceFactory *dbus,
        const QString &query,
        const QStringList &propertyNames,
        const QStringList &sortPropertyNames) const
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
    QVector<QVariant::Type> aliasTypes;
    QVector<int> identityColumns;

    const QGalleryAggregateType &type = qt_galleryAggregateTypeList[m_aggregateIndex];
    const QGalleryItemPropertyList &properties = type.properties;
    const QGalleryAggregatePropertyList &aggregateProperties = type.aggregateProperties;

    for (int i = 0; i < type.identity.count; ++i)
        identityNames.append(type.identity[i].name);

    qSort(identityNames.begin(), identityNames.end(), QGalleryReorderLessThan(sortPropertyNames));

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
        identityAttributes.append(properties[propertyIndex].attributes & PropertyMask);
        identityTypes.append(properties[propertyIndex].type);
    }

    for (QStringList::const_iterator it = propertyNames.constBegin();
            it != propertyNames.constEnd();
            ++it) {
        if (qFind(propertyNames.begin(), it, *it) != it || identityNames.contains(*it))
            continue;    // Skip duplicates.

        int propertyIndex = properties.indexOfProperty(*it);

        if (propertyIndex >= 0) {
            const QString field = properties[propertyIndex].field;

            int fieldIndex = identityFields.indexOf(field);
            Q_ASSERT(fieldIndex != -1);

            arguments->aliasColumns.append(fieldIndex);
            aliasNames.append(*it);
            aliasAttributes.append(properties[propertyIndex].attributes & PropertyMask);
            aliasTypes.append(properties[propertyIndex].type);
        } else if ((propertyIndex = aggregateProperties.indexOfProperty(*it)) >= 0) {
            aggregateNames.append(*it);
            aggregateFields.append(aggregateProperties[propertyIndex].field);
            aggregates.append(aggregateProperties[propertyIndex].aggregate);
            aggregateAttributes.append(QGalleryProperty::CanRead);
            aggregateTypes.append(aggregateProperties[propertyIndex].type);
        }
    }

    const bool descending = !sortPropertyNames.isEmpty()
            && sortPropertyNames.first().startsWith(QLatin1Char('-'));

    for (QStringList::const_iterator it = sortPropertyNames.begin();
            it != sortPropertyNames.end();
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
            arguments->sortCriteria.append(QGalleryTrackerSortCriteria(fieldIndex, sortFlags));
    }

    arguments->updateMask = qt_galleryAggregateTypeList[m_aggregateIndex].updateMask;
    arguments->identityWidth = identityColumns.count();
    arguments->valueOffset = 0;
    arguments->queryInterface = dbus->metaDataInterface();
    arguments->queryMethod = QLatin1String("GetUniqueValuesWithAggregates");
    arguments->queryArguments = QVariantList()
            << type.service
            << identityFields
            << query
            << aggregates
            << aggregateFields
            << descending;
    if (type.identity.count == 1)
        arguments->idColumn = new QGalleryTrackerPrefixColumn(0, type.prefix);
    else
        arguments->idColumn = new QGalleryTrackerCompositeIdColumn(identityColumns, type.prefix);
    arguments->urlColumn = new QGalleryTrackerStaticColumn(QVariant());
    arguments->typeColumn = new QGalleryTrackerStaticColumn(type.itemType);
    arguments->valueColumns = qt_createValueColumns(identityTypes + aggregateTypes);
    arguments->propertyNames = identityNames + aggregateNames + aliasNames;
    arguments->propertyAttributes = identityAttributes + aggregateAttributes + aliasAttributes;
    arguments->propertyTypes = identityTypes + aggregateTypes + aliasTypes;
}

QTM_END_NAMESPACE
