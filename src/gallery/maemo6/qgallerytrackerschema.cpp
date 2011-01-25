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
#include "qgallerytrackerresultset_p.h"
#include "qgallerytrackerlistcolumn_p.h"
#include "qgallerytrackertyperesultset_p.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qdir.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qsettings.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qurl.h>
#include <QtCore/qxmlstream.h>
#include <QtCore/QDebug>

QTM_BEGIN_NAMESPACE

namespace
{

    template <typename T>
    struct QGalleryPropertyList
    {
        QGalleryPropertyList() : items(0), count(0) {}
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

    struct QGalleryCompositeProperty
    {
        QLatin1String name;
        QVariant::Type type;
        QGalleryItemPropertyList dependencies;
        QGalleryTrackerCompositeColumn *(*createColumn)(const QVector<int> &columns);
        bool (*writeFilterCondition)(
                QDocumentGallery::Error *error,
                QString *query,
                const QGalleryCompositeProperty &property,
                const QGalleryMetaDataFilter &filter);
    };

    typedef QGalleryPropertyList<QGalleryCompositeProperty> QGalleryCompositePropertyList;

    enum UpdateId
    {
        FileId          = 0x0001,
        FolderId        = 0x0002,
        DocumentId      = 0x0004,
        AudioId         = 0x0008,
        ImageId         = 0x0010,
        VideoId         = 0x0020,
        PlaylistId      = 0x0040,
        TextId          = 0x0080,
        ArtistId = 0x0100,
        AlbumId = 0x0200,
        AlbumArtistId = ArtistId,
        PhotoAlbumId = 0x0400

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
        TextMask        = TextId,
        ArtistMask = ArtistId,
        AlbumMask = AlbumId,
        AlbumArtistMask = AlbumArtistId,
        PhotoAlbumMask = PhotoAlbumId

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
        QLatin1String rdfSuffix;
        QLatin1String typeFragment;
        const char *filterFragment;
        QGalleryTypePrefix prefix;
        QGalleryItemPropertyList itemProperties;
        QGalleryCompositePropertyList compositeProperties;
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
        QGalleryCompositePropertyList compositeProperties;
/*        void (*writeIdCondition)(QDocumentGallery::Error *error, QXmlStreamWriter *xml, const QStringRef &itemId); */
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
        int indexOfRdfTypes(const QStringList &rdfTypes) const;
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

    template <typename T>
    int QGalleryTypeList<T>::indexOfRdfTypes(const QStringList &rdfTypes) const
    {
        int index = -1;
        int rdfIndex = -1;
        for (int i = 0; i < count; ++i) {
            for (int j = rdfTypes.count() - 1; j >= 0; --j) {
                if (rdfTypes.at(j).endsWith(items[i].rdfSuffix)) {
                    if (j > rdfIndex) {
                        index = i;
                        rdfIndex = j;
                    }
                    break;
                }
            }
        }
        return index;
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
}

#define QT_GALLERY_ITEM_PROPERTY(PropertyName, Field, Type, Attr) \
{ QLatin1String(PropertyName), QLatin1String(Field), QVariant::Type, QGalleryProperty::Attributes(Attr) }

#define QT_GALLERY_AGGREGATE_PROPERTY(PropertyName, Field, Aggregate, Type) \
{ QLatin1String(PropertyName), QLatin1String(Field), QLatin1String(Aggregate), QVariant::Type }

#define QT_GALLERY_COMPOSITE_PROPERTY(PropertyName, Type, Dependencies, Factory, QueryBuilder) \
{ QLatin1String(PropertyName), QVariant::Type, QGalleryItemPropertyList(Dependencies), Factory, QueryBuilder }

#define QT_GALLERY_COMPOSITE_PROPERTY_NO_DEPENDENCIES(PropertyName, Type, Factory, QueryBuilder) \
{ QLatin1String(PropertyName), QVariant::Type, QGalleryItemPropertyList(), Factory, QueryBuilder }

#define QT_GALLERY_ITEM_TYPE(Type, RdfPrefix, RdfType, Prefix, PropertyGroup) \
{ \
    QLatin1String(#Type), \
    QLatin1String(#RdfPrefix":"#RdfType), \
    QLatin1String("/"#RdfPrefix"#"#RdfType), \
    QLatin1String("{?x rdf:type "#RdfPrefix":"#RdfType"}"), \
    0, \
    QGalleryTypePrefix(#Prefix"::"), \
    QGalleryItemPropertyList(qt_gallery##PropertyGroup##PropertyList), \
    QGalleryCompositePropertyList(qt_gallery##PropertyGroup##CompositePropertyList), \
    Type##Id, \
    Type##Mask \
}

#define QT_GALLERY_ITEM_TYPE_NO_COMPOSITE(Type, RdfPrefix, RdfType, Prefix, PropertyGroup) \
{ \
    QLatin1String(#Type), \
    QLatin1String(#RdfPrefix":"#RdfType), \
    QLatin1String("/"#RdfPrefix"#"#RdfType), \
    QLatin1String("{?x rdf:type "#RdfPrefix":"#RdfType"}"), \
    0, \
    QGalleryTypePrefix(#Prefix"::"), \
    QGalleryItemPropertyList(qt_gallery##PropertyGroup##PropertyList), \
    QGalleryCompositePropertyList(), \
    Type##Id, \
    Type##Mask \
}

#define QT_GALLERY_ITEM_TYPE_NO_COMPOSITE_FILTERED(Type, RdfPrefix, RdfType, FilterType, FilterProperty, Prefix, PropertyGroup) \
{ \
    QLatin1String(#Type), \
    QLatin1String(#RdfPrefix":"#RdfType), \
    QLatin1String("/"#RdfPrefix"#"#RdfType), \
    QLatin1String("{?x rdf:type "#RdfPrefix":"#RdfType"}{?y rdf:type "#FilterType"}"), \
    #FilterProperty"(?y)=?x", \
    QGalleryTypePrefix(#Prefix"::"), \
    QGalleryItemPropertyList(qt_gallery##PropertyGroup##PropertyList), \
    QGalleryCompositePropertyList(), \
    Type##Id, \
    Type##Mask \
}

#define QT_GALLERY_AGGREGATE_TYPE(Type, RdfPrefix, RdfType, Prefix, UpdateMask) \
{ \
    QLatin1String(#Type), \
    QLatin1String(#RdfPrefix":"#RdfType), \
    QGalleryTypePrefix(#Prefix"::"), \
    QGalleryItemPropertyList(qt_gallery##Type##Identity), \
    QGalleryItemPropertyList(qt_gallery##Type##PropertyList),\
    QGalleryAggregatePropertyList(qt_gallery##Type##AggregateList), \
    QGalleryCompositePropertyList(), \
    UpdateMask \
}

static bool qt_writeCondition(
        QDocumentGallery::Error *error,
        QString *query,
        const QGalleryFilter &filter,
        const QGalleryItemPropertyList &properties,
        const QGalleryCompositePropertyList &composites);

static bool qt_writeConditionHelper(
        QDocumentGallery::Error *error,
        QString *query,
        const QList<QGalleryFilter> &filters,
        const QGalleryItemPropertyList &properties,
        const QGalleryCompositePropertyList &composites,
        const QString& op
        )
{
    if (!filters.isEmpty()) {
        *query += "(";

        int count = filters.size();
        for (QList<QGalleryFilter>::const_iterator it = filters.begin(), end = filters.end();
                it != end;
                ++it) {
            if (!qt_writeCondition(error, query, *it, properties, composites))
                return false;
            if ( --count > 0 )
                *query += op;
        }
        *query += ")";
    }
    return true;
}

static bool qt_writeCondition(
        QDocumentGallery::Error *error,
        QString *query,
        const QGalleryIntersectionFilter &filter,
        const QGalleryItemPropertyList &properties,
        const QGalleryCompositePropertyList &composites)
{
    return qt_writeConditionHelper( error, query, filter.filters(), properties, composites, "&&" );
}

static bool qt_writeCondition(
        QDocumentGallery::Error *error,
        QString *query,
        const QGalleryUnionFilter &filter,
        const QGalleryItemPropertyList &properties,
        const QGalleryCompositePropertyList &composites)
{
    return qt_writeConditionHelper( error, query, filter.filters(), properties, composites, "||" );
}

static bool qt_write_comparison(
        QDocumentGallery::Error *error,
        const QLatin1String &field,
        const QVariant &value,
        const char *op,
        QString *query)
{
    if (value.canConvert(QVariant::String)) {
        *query += QLatin1String("(")
                + field
                + QLatin1String(op)
                + QLatin1String("'")
                + value.toString()
                + QLatin1String("')");
        return true;
    } else {
        *error = QDocumentGallery::FilterError;
        return false;
    }
}

static bool qt_write_function(
        QDocumentGallery::Error *,
        const char *function,
        const QString &field,
        const QRegExp &regExp,
        QString *query)
{
    *query += QLatin1String(function)
            + QLatin1String("(")
            + field
            + QLatin1String(",'")
            + regExp.pattern()
            + QLatin1String("')");
    return true;
}

static bool qt_write_function(
        QDocumentGallery::Error *error,
        const char *function,
        const QString &field,
        const QVariant &value,
        QString *query)
{
    if (value.canConvert(QVariant::String)) {
        *query += QLatin1String(function)
                + QLatin1String("(")
                + field
                + QLatin1String(",'")
                + value.toString()
                + QLatin1String("')");
        return true;
    } else {
        *error = QDocumentGallery::FilterError;
        return false;
    }
}

static bool qt_writeCondition(
        QDocumentGallery::Error *error,
        QString *query,
        const QGalleryMetaDataFilter &filter,
        const QGalleryItemPropertyList &properties,
        const QGalleryCompositePropertyList &composites)
{
    if (filter.isNegated())
        *query += "!";

    const QString propertyName = filter.propertyName();

    int index;

    if ((index = properties.indexOfProperty(propertyName)) != -1) {
        const QVariant value = filter.value();

        switch (filter.comparator()) {
        case QGalleryFilter::Equals:
            return value.type() != QVariant::RegExp
                    ? qt_write_comparison(error, properties[index].field, value, "=", query)
                    : qt_write_function(error, "REGEX", properties[index].field, value.toRegExp(), query);
        case QGalleryFilter::LessThan:
            return qt_write_comparison(error, properties[index].field, value, "<", query);
        case QGalleryFilter::GreaterThan:
            return qt_write_comparison(error, properties[index].field, value, ">", query);
        case QGalleryFilter::LessThanEquals:
            return qt_write_comparison(error, properties[index].field, value, "<=", query);
        case QGalleryFilter::GreaterThanEquals:
            return qt_write_comparison(error, properties[index].field, value, ">=", query);
        case QGalleryFilter::Contains:
            return  qt_write_function(error, "fn:contains", properties[index].field, value, query);
        case QGalleryFilter::StartsWith:
            return qt_write_function(error, "fn:starts-with", properties[index].field, value, query);
        case QGalleryFilter::EndsWith:
            return qt_write_function(error, "fn:ends-with", properties[index].field, value, query);
        case QGalleryFilter::Wildcard:
            return qt_write_function(error, "fn:contains", properties[index].field, value, query);
        case QGalleryFilter::RegExp:
            return value.type() != QVariant::RegExp
                    ? qt_write_function(error, "REGEX", properties[index].field, value, query)
                    : qt_write_function(error, "REGEX", properties[index].field, value.toRegExp(), query);
        default:
            *error = QDocumentGallery::FilterError;

            return false;
        }
        return true;
    } else if ((index = composites.indexOfProperty(propertyName)) != -1
            && composites[index].writeFilterCondition) {
        return composites[index].writeFilterCondition(error, query, composites[index], filter);
    } else {
        *error = QDocumentGallery::FilterError;
        return false;
    }
}

static bool qt_writeCondition(
        QDocumentGallery::Error *error,
        QString *query,
        const QGalleryFilter &filter,
        const QGalleryItemPropertyList &properties,
        const QGalleryCompositePropertyList &composites)
{
    switch (filter.type()) {
    case QGalleryFilter::Intersection:
        return qt_writeCondition(
                error, query, filter.toIntersectionFilter(), properties, composites);
    case QGalleryFilter::Union:
        return qt_writeCondition(error, query, filter.toUnionFilter(), properties, composites);
    case QGalleryFilter::MetaData:
        return qt_writeCondition(error, query, filter.toMetaDataFilter(), properties, composites);
    default:
        Q_ASSERT(filter.type() != QGalleryFilter::Invalid);
        *error = QDocumentGallery::FilterError;
        return false;
    }
}

static QString qt_encodedFilePathUrl(const QString &filePath)
{
    QString encodedUrl = QUrl::fromLocalFile(filePath).toEncoded();
    encodedUrl.replace(QLatin1Char('\''), QLatin1String("\\\'"));
    return encodedUrl;
}

static QString qt_encodedFilePathFragment(const QString &fragment)
{
    QString encodedFragment = QUrl(fragment).toEncoded();
    encodedFragment.replace(QLatin1Char('\''), QLatin1String("\\\'"));
    return encodedFragment;
}

static bool qt_writeFilePathUrlCondition(
        QDocumentGallery::Error *error,
        QString *query,
        const QLatin1String &property,
        const QGalleryMetaDataFilter &filter)
{
    const QVariant value = filter.value();

    if (value.type() != QVariant::String) {
        *error = QDocumentGallery::FilterError;
        return false;
    } else {
        const QString &filePath = value.toString();

        switch (filter.comparator()) {
        case QGalleryFilter::Equals:
            return qt_write_comparison(
                    error, property, qt_encodedFilePathUrl(filePath), "=", query);
        case QGalleryFilter::LessThan:
            return qt_write_comparison(
                    error, property, qt_encodedFilePathUrl(filePath), "<", query);
        case QGalleryFilter::GreaterThan:
            return qt_write_comparison(
                    error, property, qt_encodedFilePathUrl(filePath), ">", query);
        case QGalleryFilter::LessThanEquals:
            return qt_write_comparison(
                    error, property, qt_encodedFilePathUrl(filePath), "<=", query);
        case QGalleryFilter::GreaterThanEquals:
            return qt_write_comparison(
                    error, property, qt_encodedFilePathUrl(filePath), ">=", query);
        case QGalleryFilter::Contains:
            return  qt_write_function(
                    error, "fn:contains", property, qt_encodedFilePathFragment(filePath), query);
        case QGalleryFilter::StartsWith:
            return qt_write_function(
                    error, "fn:starts-with", property, qt_encodedFilePathUrl(filePath), query);
        case QGalleryFilter::EndsWith:
            return qt_write_function(
                    error, "fn:ends-with", property, qt_encodedFilePathFragment(filePath), query);
        case QGalleryFilter::Wildcard:
            return qt_write_function(
                    error, "fn:contains", property, qt_encodedFilePathUrl(filePath), query);
        case QGalleryFilter::RegExp:    // Unsupported.
        default:
            *error = QDocumentGallery::FilterError;
            return false;
        }
    }
}

static bool qt_writeFilePathCondition(
        QDocumentGallery::Error *error,
        QString *query,
        const QGalleryCompositeProperty &,
        const QGalleryMetaDataFilter &filter)
{
    return qt_writeFilePathUrlCondition(error, query, QLatin1String("nie:url(?x)"), filter);
}

static bool qt_writePathCondition(
        QDocumentGallery::Error *error,
        QString *query,
        const QGalleryCompositeProperty &,
        const QGalleryMetaDataFilter &filter)
{
    return qt_writeFilePathUrlCondition(
            error, query, QLatin1String("nie:url(nfo:belongsToContainer(?x))"), filter);
}

static bool qt_writeFileExtensionCondition(
        QDocumentGallery::Error *error,
        QString *query,
        const QGalleryCompositeProperty &,
        const QGalleryMetaDataFilter &filter)
{
    if (filter.comparator() != QGalleryFilter::Equals || filter.value().type() != QVariant::String) {
        *error = QDocumentGallery::FilterError;
        return false;
    } else {
        *query += QLatin1String("fn:ends-with(nfo:fileName(?x),'.")
                + filter.value().toString()
                + QLatin1String("')");
        return true;
    }
}

//nie:DataObject
//  nie:url nie:isPartOf, nie:created, nie:lastRefreshed, nie:interpretedAs, nie:dataSource,
//  nie:byteSize
#define QT_GALLERY_NIE_DATAOBJECT_PROPERTIES \
    QT_GALLERY_ITEM_PROPERTY("url", "nie:url(?x)", String, CanRead | CanSort | CanFilter | IsResource)

//nie:InformationElement
//  nie:usageCounter, nie:rootElementOf, nie:contentSize, nie:isLogicalPartOf, nie:characterSet,
//  nie:licenseType, nie:hasPart, nie:hasLogicalPart, nie:keyword, nie:identifier, nie:license,
//  nie:contentAccessed, nie:contentCreated, nie:version, nie:contentLastModified, nie:isStoredAs,
//  nie:comment, nie:copyright, nie:links, nie:depends, nie:disclaimer, nie:description,
//  nie:generator, nie:relatedTo, nie:legal, nie:informationElementDate, nie:plainTextContent,
//  nie:language, nie:mimeType, nie:subject, nie:title
#define QT_GALLERY_NIE_INFORMATIONELEMENT_PROPERTIES \
    QT_GALLERY_ITEM_PROPERTY("author"     , "dc:creator(?x)"     , String    , CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("comments"   , "nie:comment(?x)"    , String    , CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("copyright"  , "nie:copyright(?x)"  , String    , CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("description", "nie:description(?x)", String    , CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("keywords"   , "nie:keyword(?x)"    , StringList, CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("language"   , "nie:language(?x)"   , String    , CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("mimeType"   , "nie:mimeType(?x)"   , String    , CanRead | CanSort | CanFilter | IsResource), \
    QT_GALLERY_ITEM_PROPERTY("rating"     , "nao:rating(?x)"     , Double    , CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("subject"    , "nie:subject(?x)"    , String    , CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("title"      , "nie:title(?x)"      , String    , CanRead | CanWrite | CanSort | CanFilter)

//nfo:FileDataObject : nie:DataObject, nie:InformationElement
//  nfo:fileLastModified, nfo:fileOwner, nfo:hasHash, nfo:fileUrl, nfo:fileName, nfo:permissions,
//  nfo:fileSize, nfo:fileCreated, nfo:fileLastAccessed
#define QT_GALLERY_NFO_FILEDATAOBJECT_PROPERTIES \
    QT_GALLERY_NIE_DATAOBJECT_PROPERTIES, \
    QT_GALLERY_NIE_INFORMATIONELEMENT_PROPERTIES, \
    QT_GALLERY_ITEM_PROPERTY("fileName"     , "nfo:fileName(?x)"         , String  , CanRead | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("fileSize"     , "nfo:fileSize(?x)"         , Int     , CanRead | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("lastAccessed" , "nfo:fileLastAccessed(?x)" , DateTime, CanRead | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("lastModified" , "nfo:fileLastModified(?x)" , DateTime, CanRead | CanSort | CanFilter)

#define QT_GALLERY_NFO_FILEDATAOBJECT_COMPOSITE_PROPERTIES \
    QT_GALLERY_COMPOSITE_PROPERTY_NO_DEPENDENCIES("fileExtension", String, QGalleryTrackerFileExtensionColumn::create, qt_writeFileExtensionCondition), \
    QT_GALLERY_COMPOSITE_PROPERTY_NO_DEPENDENCIES("filePath"     , String, QGalleryTrackerFilePathColumn::create     , qt_writeFilePathCondition), \
    QT_GALLERY_COMPOSITE_PROPERTY_NO_DEPENDENCIES("path"         , String, QGalleryTrackerPathColumn::create         , qt_writePathCondition)

//nfo:Media : nfo:FileDataObject
//  nfo:equipment, nfo:genre, nfo:averageBitrate, nfo:bitrateType, nfo:encodedBy, nfo:codec,
//  nfo:bitDepth, nfo:hasMediaStream, nfo:compressionType, nfo:device, nfo:duration, nfo:count

#define QT_GALLERY_NFO_MEDIA_PROPERTIES \
    QT_GALLERY_NFO_FILEDATAOBJECT_PROPERTIES, \
    QT_GALLERY_ITEM_PROPERTY("audioBitRate"  , "nfo:averageAudioBitrate(?x)"      , Int     , CanRead | CanSort | CanFilter | IsResource), \
    QT_GALLERY_ITEM_PROPERTY("audioCodec"    , "nfo:codec(?x)"                    , String  , CanRead | CanSort | CanFilter | IsResource), \
    QT_GALLERY_ITEM_PROPERTY("channelCount"  , "nfo:channels(?x)"                 , Int     , CanRead | CanSort | CanFilter | IsResource), \
    QT_GALLERY_ITEM_PROPERTY("duration"      , "nfo:duration(?x)"                 , Int     , CanRead | CanSort | CanFilter | IsResource), \
    QT_GALLERY_ITEM_PROPERTY("genre"         , "nfo:genre(?x)"                    , String  , CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("lastPlayed"    , "nie:contentAccessed(?x)"          , DateTime, CanRead | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("resumePosition", "nfo:streamPosition(?x)"           , Int     , CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("performer"     , "nmm:artistName(nmm:performer(?x))", String  , CanRead | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("playCount"     , "nie:usageCounter(?x)"             , Int     , CanRead | CanWrite | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("sampleRate"    , "nfo:sampleRate(?x)"               , Int     , CanRead | CanSort | CanFilter | IsResource)

//nfo:Visual : nfo:Media
//  nfo:colorDepth, nfo:width, nfo:height, nfo:interlaceMode, nfo:tilt, nfo:heading, nfo:aspectRatio

#define QT_GALLERY_NFO_VISUAL_PROPERTIES \
    QT_GALLERY_ITEM_PROPERTY("height", "nfo:height(?x)", Int, CanRead | CanSort | CanFilter | IsResource), \
    QT_GALLERY_ITEM_PROPERTY("width" , "nfo:width(?x)" , Int, CanRead | CanSort | CanFilter | IsResource)

///////
// File
///////

//nfo:DataContainer : nie:InformationElement

//nfo:Folder : nfo:FileDataObject, nfo:DataContainer

static const QGalleryItemProperty qt_galleryFilePropertyList[] =
{
    QT_GALLERY_NFO_FILEDATAOBJECT_PROPERTIES
};

#define QT_GALLERY_FILE_COMPOSITE_PROPERTIES \
    QT_GALLERY_COMPOSITE_PROPERTY_NO_DEPENDENCIES("filePath", String, QGalleryTrackerFilePathColumn::create, qt_writeFilePathCondition)

    static const QGalleryCompositeProperty qt_galleryFileCompositePropertyList[] =
{
    QT_GALLERY_NFO_FILEDATAOBJECT_COMPOSITE_PROPERTIES
};

////////
// Audio
////////

//nfo:Audio : nfo:Media
//  nfo:peakGain, nfo:gain, nfo:rearChannels, nfo:averageAudioBitrate, nfo:sampleRate,
//  nfo:frontChannels, nfo:bitsPerSample, nfo:sampleCount, nfo:lfeChannels, nfo:sideChannels,
//  nfo:channels

//nmm:MusicPiece : nfo:Audio
//  nmm:internationalStandardRecordingCode, nmm:trackNumber, nmm:lyrics, nmm:lyricist, nmm:composer
//  nmm:length, nmm:performer, nmm:beatsPerMinute, nmm:musicAlbumDisc, nmm:musicAlbum,

static const QGalleryItemProperty qt_galleryAudioPropertyList[] =
{
    QT_GALLERY_NFO_MEDIA_PROPERTIES,
    QT_GALLERY_ITEM_PROPERTY("albumArtist", "nmm:artistName(nmm:albumArtist(nmm:musicAlbum(?x)))", String, CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("albumTitle" , "nmm:albumTitle(nmm:musicAlbum(?x))"                 , String, CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("artist"     , "nmm:artistName(nmm:performer(?x))"                  , String, CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("composer"   , "nmm:artistName(nmm:composer(?x))"                   , String, CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("discNumber" , "nmm:setNumber(nmm:musicAlbumDisc(?x))"              , Int   , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("lyrics"     , "nmm:lyrics(?x)"                                     , String, CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("trackNumber", "nmm:trackNumber(?x)"                                , Int   , CanRead | CanWrite | CanSort | CanFilter)
};

static const QGalleryCompositeProperty qt_galleryAudioCompositePropertyList[] =
{
    QT_GALLERY_NFO_FILEDATAOBJECT_COMPOSITE_PROPERTIES
};

///////////
// Playlist
///////////

//nfo:MediaList : nie:InformationElement
//  nfo:mediaListEntry, nfo:listDuration, nfo:entryCounter, nfo:hasMediaFileListEntry

//nmm:Playlist : nfo:FileDataObject, nfo:MediaList

static const QGalleryItemProperty qt_galleryPlaylistPropertyList[] =
{
    QT_GALLERY_NFO_FILEDATAOBJECT_PROPERTIES,
    QT_GALLERY_ITEM_PROPERTY("duration"  , "nfo:listDuration(?x)", Int, CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("trackCount", "nfo:entryCounter(?x)", Int, CanRead | CanSort | CanFilter)
};

static const QGalleryCompositeProperty qt_galleryPlaylistCompositePropertyList[] =
{
    QT_GALLERY_NFO_FILEDATAOBJECT_COMPOSITE_PROPERTIES
};

////////
// Image
////////

//nfo:Image : nfo:Visual
//  nfo:depicts, nfo:orientation, nfo:horizontalResolution, nfo:verticalResolution

//nmm:Photo : nfo:Image
//  nmm:isColorCorrected, nmm:isCropped, nmm:whiteBalance, nmm:meteringMode, nmm:isoSpeed,
//  nmm:focalLength, nmm:fnumber, nmm:flash, nmm:exposureTime, nmm:camera,

static const QGalleryItemProperty qt_galleryImagePropertyList[] =
{
    QT_GALLERY_NFO_FILEDATAOBJECT_PROPERTIES,
    QT_GALLERY_NFO_VISUAL_PROPERTIES,
    QT_GALLERY_ITEM_PROPERTY("cameraManufacturer", "nfo:manufacturer(nfo:equipment(?x))", String  , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("cameraModel"       , "nfo:model(nfo:equipment(?x))"       , String  , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("exposureTime"      , "nmm:exposureTime(?x)"               , String  , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("dateTaken"         , "nie:contentCreated(?x)"             , DateTime, CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("fNumber"           , "nmm:fnumber(?x)"                    , Int     , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("flashEnabled"      , "nmm:flash(?x)"                      , String  , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("focalLength"       , "nmm:focalLength(?x)"                , Double  , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("meteringMode"      , "nmm:meteringMode(?x)"               , String  , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("orientation"       , "nfo:orientation(?x)"                , String  , CanRead | CanWrite | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("whiteBalance"      , "nmm:whiteBalance(?x)"               , Double  , CanRead | CanWrite | CanSort | CanFilter)
};

static const QGalleryCompositeProperty qt_galleryImageCompositePropertyList[] =
{
    QT_GALLERY_NFO_FILEDATAOBJECT_COMPOSITE_PROPERTIES
};

////////
// Video
////////

//nfo:Video : nfo:Visual
//  nfo:averageVideoBitrate, nfo:frameCount, nfo:frameRate

//nmm:Video : nfo:Video
//  nmm:isContentEncrypted, nmm:subtitle, nmm:hasSubtitle, nmm:leadActor, nmm:producedBy,
//  nmm:director, nmm:category, nmm:MPAARating, nmm:synopsis, nmm:runTime, nmm:episodeNumber
//  nmm:season, nmm:isSeries, nmm:videoAlbum,

static const QGalleryItemProperty qt_galleryVideoPropertyList[] =
{
    QT_GALLERY_NFO_MEDIA_PROPERTIES,
    QT_GALLERY_NFO_VISUAL_PROPERTIES,
    QT_GALLERY_ITEM_PROPERTY("director"      , "nmm:artistName(nmm:director(?x))"  , String, CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("frameRate"     , "nfo:frameRate(?x)"                 , Double, CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("producer"      , "nmm:artistName(nmm:producedBy(?x))", String, CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("videoCodec"    , "nfo:codec(?x)"                     , String, CanRead | CanSort | CanFilter | IsResource),
    QT_GALLERY_ITEM_PROPERTY("videoBitRate"  , "nfo:averageBitrate(?x)"            , Int   , CanRead | CanSort | CanFilter | IsResource),
};

static const QGalleryCompositeProperty qt_galleryVideoCompositePropertyList[] =
{
    QT_GALLERY_NFO_FILEDATAOBJECT_COMPOSITE_PROPERTIES
};

///////////
// Document
///////////

//nfo:Document : nfo:FileDataObject

//nfo:TextDocument : nfo:Document
//  nfo:characterCount, nfo:lineCount, nfo:wordCount

//nfo:PaginatedTextDocument : nfo:PaginatedTextDocument
//  nfo:pageCount

static const QGalleryItemProperty qt_galleryDocumentPropertyList[] =
{
    QT_GALLERY_NFO_FILEDATAOBJECT_PROPERTIES,
    QT_GALLERY_ITEM_PROPERTY("created"  , "nie:contentCreated(?x)", DateTime, CanRead | CanSort | CanFilter), \
    QT_GALLERY_ITEM_PROPERTY("pageCount", "nfo:pageCount(?x)"     , Int     , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("wordCount", "nfo:wordCount(?x)"     , Int     , CanRead | CanSort | CanFilter)
};

static const QGalleryCompositeProperty qt_galleryDocumentCompositePropertyList[] =
{
    QT_GALLERY_NFO_FILEDATAOBJECT_COMPOSITE_PROPERTIES
};

///////
// Text
///////

//nfo:TextDocument : nfo:Document
//  nfo:characterCount, nfo:lineCount, nfo:wordCount

static const QGalleryItemProperty qt_galleryTextPropertyList[] =
{
    QT_GALLERY_NFO_FILEDATAOBJECT_PROPERTIES,
    QT_GALLERY_ITEM_PROPERTY("wordCount", "nfo:wordCount(?x)", Int, CanRead | CanSort | CanFilter)
};

static const QGalleryCompositeProperty qt_galleryTextCompositePropertyList[] =
{
    QT_GALLERY_NFO_FILEDATAOBJECT_COMPOSITE_PROPERTIES
};

////////
// Album
////////

//nmm:MusicAlbum : nfo:MediaList
//  nmm:albumPeakGain, nmm:albumGain, nmm:albumDuration, nmm:albumTitle, nmm:albumTrackCount
//  nmm:albumArtist,

static const QGalleryItemProperty qt_galleryAlbumPropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("albumTitle" , "nmm:albumTitle(?x)"                 , String, CanRead | CanWrite | CanFilter | CanSort),
    QT_GALLERY_ITEM_PROPERTY("artist"     , "nmm:artistName(nmm:albumArtist(?x))", String, CanRead | CanFilter | CanSort),
    QT_GALLERY_ITEM_PROPERTY("albumArtist", "nmm:artistName(nmm:albumArtist(?x))", String, CanRead | CanFilter | CanSort),
    QT_GALLERY_ITEM_PROPERTY("duration"   , "nmm:albumDuration(?x)"              , Int   , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("title"      , "nmm:albumTitle(?x)"                 , String, CanRead | CanWrite | CanFilter | CanSort),
    QT_GALLERY_ITEM_PROPERTY("trackCount" , "nmm:albumTrackCount(?x)"            , Int   , CanRead | CanSort | CanFilter)
};

/////////
// Artist
/////////

//nmm:Artist : nie:InformationElement
//  nmm:artistName

static const QGalleryItemProperty qt_galleryArtistPropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("artist"    , "nmm:artistName(?x)"   , String, CanRead | CanWrite | CanFilter | CanSort),
    QT_GALLERY_ITEM_PROPERTY("duration"  , "SUM(nfo:duration(?y))", Int   , CanRead | CanFilter | CanSort),
    QT_GALLERY_ITEM_PROPERTY("title"     , "nmm:artistName(?x)"   , String, CanRead | CanWrite | CanFilter | CanSort),
    QT_GALLERY_ITEM_PROPERTY("trackCount", "COUNT(?y)"            , Int   , CanRead | CanFilter | CanSort)
};

/////////
// AlbumArtist
/////////

//nmm:Artist : nie:InformationElement
//  nmm:artistName

static const QGalleryItemProperty qt_galleryAlbumArtistPropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("artist"    , "nmm:artistName(?x)"          , String, CanRead | CanWrite | CanFilter | CanSort),
    QT_GALLERY_ITEM_PROPERTY("duration"  , "SUM(nmm:albumDuration(?y))"  , Int   , CanRead | CanFilter | CanSort),
    QT_GALLERY_ITEM_PROPERTY("title"     , "nmm:artistName(?x)"          , String, CanRead | CanWrite | CanFilter | CanSort),
    QT_GALLERY_ITEM_PROPERTY("trackCount", "SUM(nmm:albumTrackCount(?y))", Int   , CanRead | CanFilter | CanSort)
};

/////////////
// PhotoAlbum
/////////////

//nfo:MediaList : nie:InformationElement
//  nfo:mediaListEntry, nfo:listDuration, nfo:entryCounter, nfo:hasMediaFileListEntry

//nmm:ImageList : nfo:MediaList

static const QGalleryItemProperty qt_galleryPhotoAlbumPropertyList[] =
{
    QT_GALLERY_ITEM_PROPERTY("count" , "nfo:entryCounter(?x)", Int   , CanRead | CanSort | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("title" , "nie:title(?x)"       , String, CanRead | CanWrite | CanSort | CanFilter)
};

/////////////
// File Types
/////////////

static const QGalleryItemType qt_galleryItemTypeList[] =
{
    QT_GALLERY_ITEM_TYPE(File      , nfo, FileDataObject, file      , File),
    QT_GALLERY_ITEM_TYPE(Folder    , nfo, Folder        , folder    , File),
    QT_GALLERY_ITEM_TYPE(Document  , nfo, Document      , document  , Document),
    QT_GALLERY_ITEM_TYPE(Audio     , nfo, Audio         , audio     , Audio),
    QT_GALLERY_ITEM_TYPE(Image     , nmm, Photo         , image     , Image),
    QT_GALLERY_ITEM_TYPE(Video     , nfo, Video         , video     , Video),
    QT_GALLERY_ITEM_TYPE(Playlist  , nmm, Playlist      , playlist  , Playlist),
    QT_GALLERY_ITEM_TYPE(Text      , nfo, TextDocument  , text      , Text),
    QT_GALLERY_ITEM_TYPE_NO_COMPOSITE_FILTERED(Artist     , nmm, Artist, nmm:MusicPiece, nmm:performer  , artist     , Artist),
    QT_GALLERY_ITEM_TYPE_NO_COMPOSITE_FILTERED(AlbumArtist, nmm, Artist, nmm:MusicAlbum, nmm:albumArtist, albumArtist, AlbumArtist),
    QT_GALLERY_ITEM_TYPE_NO_COMPOSITE(Album     , nmm, MusicAlbum    , album     , Album),
    QT_GALLERY_ITEM_TYPE_NO_COMPOSITE(PhotoAlbum, nmm, ImageList     , photoAlbum, PhotoAlbum)
};

//////////////
// Music Genre
//////////////

static const QGalleryItemProperty qt_galleryAudioGenreIdentity[] =
{
    QT_GALLERY_ITEM_PROPERTY("title", "nfo:genre(?x)", String, CanRead | CanFilter)
};

static const QGalleryItemProperty qt_galleryAudioGenrePropertyList[] =
{
//    QT_GALLERY_ITEM_PROPERTY("genre", "nfo:genre(?x)", String, CanRead | CanFilter),
    QT_GALLERY_ITEM_PROPERTY("title", "nfo:genre(?x)", String, CanRead | CanFilter)
};

static const QGalleryAggregateProperty qt_galleryAudioGenreAggregateList[] =
{
    QT_GALLERY_AGGREGATE_PROPERTY("duration"  , "Audio:Duration", "SUM"  , Int),
    QT_GALLERY_AGGREGATE_PROPERTY("trackCount", "*"             , "COUNT", Int),
};

//////////////////
// Aggregate Types
//////////////////

static const QGalleryAggregateType qt_galleryAggregateTypeList[] =
{
    QT_GALLERY_AGGREGATE_TYPE(AudioGenre , nfo, Media     , audioGenre , AudioMask)
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

class QGalleryTrackerServiceIndexColumn : public QGalleryTrackerValueColumn
{
public:
    QGalleryTrackerServiceIndexColumn() {}

    QVariant toVariant(const QString &string) const;
};

QVariant QGalleryTrackerServicePrefixColumn::value(QVector<QVariant>::const_iterator row) const
{
    QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);

    const int index = (row + 2)->toInt();

    return index != -1
            ? QVariant(itemTypes[index].prefix + row->toString())
            : QVariant(QLatin1String("file::") + row->toString());
}

QVariant QGalleryTrackerServiceTypeColumn::value(QVector<QVariant>::const_iterator row) const
{
    QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);

    const int index = (row + 2)->toInt();

    return index != -1
            ? QVariant(itemTypes[index].itemType)
            : QVariant(QLatin1String("File"));
}

QVariant QGalleryTrackerServiceIndexColumn::toVariant(const QString &string) const
{
    QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);

    return itemTypes.indexOfRdfTypes(string.split(QLatin1Char(',')));
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

QString QGalleryTrackerSchema::itemType() const
{
    if (m_itemIndex >= 0)
        return qt_galleryItemTypeList[m_itemIndex].itemType;
    else if (m_aggregateIndex >= 0)
        return qt_galleryAggregateTypeList[m_aggregateIndex].itemType;
    else
        return QString();
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

        for (int i = 0; i < type.compositeProperties.count; ++i)
            propertyNames.append(type.compositeProperties[i].name);
    } else if (m_aggregateIndex >= 0) {
        const QGalleryAggregateType &type = qt_galleryAggregateTypeList[m_aggregateIndex];

        for (int i = 0; i < type.properties.count; ++i)
            propertyNames.append(type.properties[i].name);

        for (int i = 0; i < type.aggregateProperties.count; ++i)
            propertyNames.append(type.aggregateProperties[i].name);

        for (int i = 0; i < type.compositeProperties.count; ++i)
            propertyNames.append(type.compositeProperties[i].name);
    }
    return propertyNames;
}

QGalleryProperty::Attributes QGalleryTrackerSchema::propertyAttributes(
        const QString &propertyName) const
{
    if (m_itemIndex >= 0) {
        const QGalleryItemType &type = qt_galleryItemTypeList[m_itemIndex];

        int index;
        if ((index = type.itemProperties.indexOfProperty(propertyName)) != -1) {
            return type.itemProperties[index].attributes & PropertyMask;
        } else if ((index = type.compositeProperties.indexOfProperty(propertyName)) != -1) {
            QGalleryProperty::Attributes attributes = QGalleryProperty::CanRead;
            if (type.compositeProperties[index].writeFilterCondition)
                attributes |= QGalleryProperty::CanFilter;
            return attributes;
        }
    } else if (m_aggregateIndex >= 0) {
        const QGalleryAggregateType &type = qt_galleryAggregateTypeList[m_aggregateIndex];

        int index;
        if ((index = type.properties.indexOfProperty(propertyName)) != -1) {
            return type.properties[index].attributes & PropertyMask;
        } else if (type.aggregateProperties.indexOfProperty(propertyName) != -1) {
            return QGalleryProperty::CanRead;
        } else if ((index = type.compositeProperties.indexOfProperty(propertyName)) != -1) {
            QGalleryProperty::Attributes attributes = QGalleryProperty::CanRead;
            if (type.compositeProperties[index].writeFilterCondition)
                attributes |= QGalleryProperty::CanFilter;
            return attributes;
        }
    }
    return QGalleryProperty::Attributes();
}

QDocumentGallery::Error QGalleryTrackerSchema::prepareItemResponse(
        QGalleryTrackerResultSetArguments *arguments,
        QGalleryDBusInterfaceFactory *dbus,
        const QString &itemId,
        const QStringList &propertyNames) const
{
    if (m_itemIndex >= 0) {
        QString query
                = QLatin1String("FILTER(?x=<")
                + qt_galleryItemTypeList[m_itemIndex].prefix.strip(itemId).toString()
                + QLatin1String(">)");
        populateItemArguments(arguments, dbus, query, propertyNames, QStringList(), 0, 0);

        return QDocumentGallery::NoError;
    } else if (m_aggregateIndex >= 0) {
        Q_ASSERT(qt_galleryAggregateTypeList[m_aggregateIndex].identity.count == 1);
        QString query
                = QLatin1String("FILTER(xsd:string(")
                + qt_galleryAggregateTypeList[m_aggregateIndex].identity[0].field
                + QLatin1String("=\"")
                + qt_galleryAggregateTypeList[m_aggregateIndex].prefix.strip(itemId) .toString()
                + QLatin1String("\")");
        populateAggregateArguments(arguments, dbus, query, propertyNames, QStringList(), 0, 0);

        return QDocumentGallery::NoError;
    }

    return QDocumentGallery::ItemIdError;
}

QDocumentGallery::Error QGalleryTrackerSchema::prepareQueryResponse(
        QGalleryTrackerResultSetArguments *arguments,
        QGalleryDBusInterfaceFactory *dbus,
        QGalleryQueryRequest::Scope scope,
        const QString &rootItemId,
        const QGalleryFilter &filter,
        const QStringList &propertyNames,
        const QStringList &sortPropertyNames,
        int offset,
        int limit) const
{
    QString query;

    QDocumentGallery::Error error = buildFilterQuery(&query, scope, rootItemId, filter);

    if (error != QDocumentGallery::NoError) {
        return error;
    } else if (m_itemIndex >= 0) {
        populateItemArguments(
                arguments, dbus, query, propertyNames, sortPropertyNames, offset, limit);

        return QDocumentGallery::NoError;
    } else if (m_aggregateIndex >= 0) {
        populateAggregateArguments(
                arguments, dbus, query, propertyNames, sortPropertyNames, offset, limit);

        return QDocumentGallery::NoError;
    } else {
        return QDocumentGallery::ItemTypeError;
    }
}

QDocumentGallery::Error QGalleryTrackerSchema::prepareTypeResponse(
        QGalleryTrackerTypeResultSetArguments *arguments, QGalleryDBusInterfaceFactory *dbus) const
{
    if (m_itemIndex >= 0) {
        arguments->service = qt_galleryItemTypeList[m_itemIndex].service;
        arguments->accumulative = false;
        arguments->updateMask = qt_galleryItemTypeList[m_itemIndex].updateMask;

        if (qt_galleryItemTypeList[m_itemIndex].filterFragment) {
            arguments->queryInterface = dbus->metaDataInterface();
            arguments->queryMethod = QLatin1String("SparqlQuery");
            arguments->queryArguments = QVariantList()
                    << QLatin1String("SELECT COUNT(DISTINCT ?x) WHERE {")
                    + qt_galleryItemTypeList[m_itemIndex].typeFragment
                    + QLatin1String("FILTER(")
                    + QLatin1String(qt_galleryItemTypeList[m_itemIndex].filterFragment)
                    + QLatin1String(")}");
        } else {
            arguments->queryInterface = dbus->statisticsInterface();
            arguments->queryMethod = QLatin1String("Get");
            arguments->queryArguments = QVariantList();
        }

        return QDocumentGallery::NoError;
    } else if (m_aggregateIndex >= 0) {
        const QGalleryAggregateType &type = qt_galleryAggregateTypeList[m_aggregateIndex];
        arguments->service = type.service;
        arguments->accumulative = false;
        arguments->updateMask = type.updateMask;
        arguments->queryInterface = dbus->metaDataInterface();
        arguments->queryMethod = QLatin1String("SparqlQuery");

        if (type.service == "nfo:Media") {
            arguments->queryArguments = QVariantList() << QLatin1String(
                    "SELECT COUNT(DISTINCT ?x) "
                    "WHERE {?urn rdf:type nfo:Media. ?urn nfo:genre ?x}");
        }

        return QDocumentGallery::NoError;
    } else {
        return QDocumentGallery::ItemTypeError;
    }
}

QDocumentGallery::Error QGalleryTrackerSchema::buildFilterQuery(
        QString *query,
        QGalleryQueryRequest::Scope scope,
        const QString &rootItemId,
        const QGalleryFilter &filter) const
{
    const QGalleryItemTypeList itemTypes(qt_galleryItemTypeList);
    const QGalleryAggregateTypeList aggregateTypes(qt_galleryAggregateTypeList);

    QDocumentGallery::Error result = QDocumentGallery::NoError;

    QString rootItemStatement;
    QString filterStatement;

    if (m_itemIndex != -1 && itemTypes[m_itemIndex].filterFragment)
        filterStatement = QLatin1String(itemTypes[m_itemIndex].filterFragment);

    Q_UNUSED(scope);
    if (!rootItemId.isEmpty()) {
        int index;
        if ((index = itemTypes.indexOfItemId(rootItemId)) != -1) {
            if (itemTypes[index].itemType == QDocumentGallery::Artist) {
                if (m_itemIndex >= 0) {
                    QString property;
                    if (qt_galleryItemTypeList[m_itemIndex].itemType == QDocumentGallery::Album) {
                        rootItemStatement = QLatin1String("{?track nie:isLogicalPartOf ?x}");
                        property = QLatin1String("nmm:performer(?track)");
                    } else if (qt_galleryItemTypeList[m_itemIndex].itemType == QDocumentGallery::Audio) {
                        property = QLatin1String("nmm:performer(?x)");
                    }
                    if (!property.isEmpty()) {
                        if (!filterStatement.isEmpty())
                            filterStatement += QLatin1String(" && ");
                        filterStatement
                            += property
                            + QLatin1String("=<")
                            + itemTypes[index].prefix.strip(rootItemId).toString()
                            + QLatin1String(">");
                    } else {
                        result = QDocumentGallery::ItemIdError;
                    }
                }
            } else if (itemTypes[index].itemType == QDocumentGallery::AlbumArtist) {
                QString property;
                if (m_itemIndex >= 0) {
                    if (qt_galleryItemTypeList[m_itemIndex].itemType == QDocumentGallery::Audio)
                        property = QLatin1String("nmm:albumArtist(nmm:musicAlbum(?x))");
                    else if (qt_galleryItemTypeList[m_itemIndex].itemType == QDocumentGallery::Album)
                        property = QLatin1String("nmm:albumArtist(?x)");
                }
                if (!property.isEmpty()) {
                    if (!filterStatement.isEmpty())
                        filterStatement += QLatin1String(" && ");
                    filterStatement
                        += property
                        + QLatin1String("=<")
                        + itemTypes[index].prefix.strip(rootItemId).toString()
                        + QLatin1String(">");
                } else {
                    return QDocumentGallery::ItemIdError;
                }
            } else if (itemTypes[index].itemType == QDocumentGallery::Folder) {
                if (m_itemIndex >= 0) {
                    const QString rootUrn = itemTypes[index].prefix.strip(rootItemId).toString();
                    QString property;
                    if (qt_galleryItemTypeList[m_itemIndex].updateMask & FileMask) {
                        if (scope == QGalleryQueryRequest::DirectDescendants) {
                            property = QLatin1String("nfo:belongsToContainer(?x)");
                            if (!filterStatement.isEmpty())
                                filterStatement += QLatin1String(" && ");
                            filterStatement
                                    += property
                                    + QLatin1String("=<")
                                    + rootUrn
                                    + QLatin1String(">");
                        } else {
                            if (!filterStatement.isEmpty())
                                filterStatement += QLatin1String(" && ");
                            filterStatement
                                    += QLatin1String("nie:url(?x) > fn:concat(nie:url(<")
                                    + rootUrn
                                    + QLatin1String(">),'/') && ")
                                    + QLatin1String("nie:url(?x) < fn:concat(nie:url(<")
                                    + rootUrn
                                    + QLatin1String(">),'0')");
                        }
                    } else {
                        result = QDocumentGallery::ItemIdError;
                    }
                }
                else {
                    result = QDocumentGallery::ItemIdError;
                }
            } else if (itemTypes[index].itemType == QDocumentGallery::Album) {
                if (m_itemIndex) {
                    if (qt_galleryItemTypeList[m_itemIndex].itemType == QDocumentGallery::Audio) {
                        QString property = QLatin1String("nmm:musicAlbum(?x)");
                        if (!filterStatement.isEmpty())
                            filterStatement += QLatin1String(" && ");
                        filterStatement
                                += property
                                + QLatin1String("=<")
                                + itemTypes[index].prefix.strip(rootItemId).toString()
                                +QLatin1String(">");
                    } else {
                        result = QDocumentGallery::ItemIdError;
                    }
                } else {
                    result = QDocumentGallery::ItemIdError;
                }
            } else if (itemTypes[index].itemType == QDocumentGallery::PhotoAlbum) {
                if (m_itemIndex) {
                    if (qt_galleryItemTypeList[m_itemIndex].itemType == QDocumentGallery::Image) {
                        QString property = QLatin1String("nie:isLogicalPartOf(?x)");
                        if (!filterStatement.isEmpty())
                            filterStatement += QLatin1String(" && ");
                        filterStatement
                                += property
                                + QLatin1String("=<")
                                + itemTypes[index].prefix.strip(rootItemId).toString()
                                +QLatin1String(">");
                    }
                }
            } else {
                result = QDocumentGallery::ItemIdError;
            }
        } else if ((index = aggregateTypes.indexOfItemId(rootItemId)) != -1) {
            if (aggregateTypes[index].itemType == QDocumentGallery::AudioGenre) {
                if (m_itemIndex >= 0) {
                    QString property;
                    if (qt_galleryItemTypeList[m_itemIndex].itemType == QDocumentGallery::Audio) {
                        property = QLatin1String("nfo:genre(?x)");
                        if (!filterStatement.isEmpty())
                            filterStatement += QLatin1String(" && ");
                        filterStatement
                                += property
                                + QLatin1String("='")
                                + aggregateTypes[index].prefix.strip(rootItemId).toString()
                                + QLatin1String("'");
                    } else if (qt_galleryItemTypeList[m_itemIndex].itemType == QDocumentGallery::Album) {
                        rootItemStatement = QLatin1String("{?track nie:isLogicalPartOf ?x}");
                        property = QLatin1String("nfo:genre(?track)");
                        if (!filterStatement.isEmpty())
                            filterStatement += QLatin1String(" && ");
                        filterStatement
                                += property
                                + QLatin1String("='")
                                + aggregateTypes[index].prefix.strip(rootItemId).toString()
                                + QLatin1String("'");
                    } else if (qt_galleryItemTypeList[m_itemIndex].itemType == QDocumentGallery::Artist) {
                        property = QLatin1String("nfo:genre(?y)");
                        if (!filterStatement.isEmpty())
                            filterStatement += QLatin1String(" && ");
                        filterStatement
                                += property
                                + QLatin1String("='")
                                + aggregateTypes[index].prefix.strip(rootItemId).toString()
                                + QLatin1String("'");
                    } else {
                        result = QDocumentGallery::ItemIdError;
                    }
                }
            }
        } else {
            result = QDocumentGallery::ItemIdError;
        }
    }

    if (filter.isValid()) {
        if (!filterStatement.isEmpty())
            filterStatement += QLatin1String(" && ");
        if (m_itemIndex != -1) {
            qt_writeCondition(
                    &result,
                    &filterStatement,
                    filter,
                    itemTypes[m_itemIndex].itemProperties,
                    itemTypes[m_itemIndex].compositeProperties);
        } else if (m_aggregateIndex != -1) {
            qt_writeCondition(
                    &result,
                    &filterStatement,
                    filter,
                    aggregateTypes[m_aggregateIndex].properties,
                    aggregateTypes[m_itemIndex].compositeProperties);
        } else {
            result = QDocumentGallery::ItemTypeError;
        }
    }
    if (result == QDocumentGallery::NoError) {
        *query = rootItemStatement + (filterStatement.isEmpty()
                ? QLatin1String("")
                : QLatin1String("FILTER(") + filterStatement + QLatin1String(")"));
    }
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

static QString qt_writeSorting(
        const QStringList &propertyNames, const QGalleryItemPropertyList &properties)
{
    QString sortExpression;

    for (QStringList::const_iterator it = propertyNames.constBegin();
            it != propertyNames.constEnd();
            ++it) {
        if (it->startsWith(QLatin1Char('-'))) {
            const int propertyIndex = properties.indexOfProperty(it->mid(1));

            if (propertyIndex != -1) {
                sortExpression
                        += QLatin1String(" DESC(")
                        + properties[propertyIndex].field
                        + QLatin1String(")");
            }
        } else {
            const int propertyIndex = it->startsWith(QLatin1Char('+'))
                    ? properties.indexOfProperty(it->mid(1))
                    : properties.indexOfProperty(*it);

            if (propertyIndex != -1) {
                sortExpression
                        += QLatin1String(" ASC(")
                        + properties[propertyIndex].field
                        + QLatin1String(")");
            }
        }
    }

    return !sortExpression.isEmpty()
            ? QLatin1String(" ORDER BY") + sortExpression
            : sortExpression;
}

void QGalleryTrackerSchema::populateItemArguments(
        QGalleryTrackerResultSetArguments *arguments,
        QGalleryDBusInterfaceFactory *dbus,
        const QString &query,
        const QStringList &propertyNames,
        const QStringList &sortPropertyNames,
        int offset,
        int limit) const
{
    const QString service = qt_galleryItemTypeList[m_itemIndex].service;

    QStringList valueNames;
    QStringList aliasNames;
    QStringList compositeNames;
    QVector<QGalleryProperty::Attributes> valueAttributes;
    QVector<QGalleryProperty::Attributes> aliasAttributes;
    QVector<QGalleryProperty::Attributes> compositeAttributes;
    QVector<QVariant::Type> valueTypes;
    QVector<QVariant::Type> extendedValueTypes;
    QVector<QVariant::Type> aliasTypes;
    QVector<QVariant::Type> compositeTypes;

    const QGalleryItemPropertyList &itemProperties
            = qt_galleryItemTypeList[m_itemIndex].itemProperties;
    const QGalleryCompositePropertyList &compositeProperties
            = qt_galleryItemTypeList[m_itemIndex].compositeProperties;

    for (QStringList::const_iterator it = propertyNames.begin(); it != propertyNames.end(); ++it) {
        if (valueNames.contains(*it) || aliasNames.contains(*it))
            continue;

        int propertyIndex;

        if ((propertyIndex = itemProperties.indexOfProperty(*it)) >= 0) {
            const QString field = itemProperties[propertyIndex].field;

            int fieldIndex = arguments->fieldNames.indexOf(field);

            if (fieldIndex >= 0) {
                Q_ASSERT( 0 );
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
        }
    }

    for (QStringList::const_iterator it = propertyNames.begin(); it != propertyNames.end(); ++it) {
        if (valueNames.contains(*it) || aliasNames.contains(*it) || compositeNames.contains(*it))
            continue;

        int propertyIndex;
        if ((propertyIndex = compositeProperties.indexOfProperty(*it)) >= 0) {
            const QGalleryItemPropertyList &dependencies
                    = compositeProperties[propertyIndex].dependencies;

            QVector<int> columns;
            for (int i = 0; i < dependencies.count; ++i) {
                const QString field = dependencies[i].field;

                int fieldIndex = arguments->fieldNames.indexOf(field);

                if (fieldIndex >= 0) {
                    columns.append(fieldIndex + 2);
                } else {
                    columns.append(arguments->fieldNames.count() + 2);

                    arguments->fieldNames.append(field);
                    extendedValueTypes.append(dependencies[i].type);
                }
            }

            QGalleryProperty::Attributes attributes = QGalleryProperty::CanRead;
            if (compositeProperties[propertyIndex].writeFilterCondition)
                attributes |= QGalleryProperty::CanFilter;

            compositeNames.append(*it);
            compositeAttributes.append(attributes);
            compositeTypes.append(compositeProperties[propertyIndex].type);
            arguments->compositeColumns.append(
                    compositeProperties[propertyIndex].createColumn(columns));
        }
    }

    arguments->service = qt_galleryItemTypeList[m_itemIndex].service;
    arguments->updateMask = qt_galleryItemTypeList[m_itemIndex].updateMask;
    arguments->identityWidth = 1;
    arguments->valueOffset = 3;  // urn + nie:url + rdf:type
    arguments->tableWidth =  arguments->valueOffset + arguments->fieldNames.count();
    arguments->compositeOffset = arguments->valueOffset + valueNames.count();
    arguments->queryInterface = dbus->metaDataInterface();
    arguments->sparql
            = QLatin1String("SELECT ?x nie:url(?x) rdf:type(?x) ")
            + arguments->fieldNames.join(QLatin1String(" "))
            + QLatin1String(" WHERE {")
            + qt_galleryItemTypeList[m_itemIndex].typeFragment
            + query
            + QLatin1String("}")
            + QLatin1String(" GROUP BY ?x")
            + qt_writeSorting(sortPropertyNames, itemProperties);

    if (offset > 0)
        arguments->sparql += QString::fromLatin1(" OFFSET %1").arg(offset);
    if (limit > 0)
        arguments->sparql += QString::fromLatin1(" LIMIT %1").arg(limit);

    arguments->urlColumn.reset(new QGalleryTrackerFileUrlColumn(QGALLERYTRACKERFILEURLCOLUMN_DEFAULT_COL));
    if (qt_galleryItemTypeList[m_itemIndex].updateId & FileMask) {
        arguments->idColumn.reset(new QGalleryTrackerServicePrefixColumn);
        arguments->typeColumn.reset(new QGalleryTrackerServiceTypeColumn);
    } else {
        arguments->idColumn.reset(
                new QGalleryTrackerPrefixColumn(0, qt_galleryItemTypeList[m_itemIndex].prefix));
        arguments->typeColumn.reset(
                new QGalleryTrackerStaticColumn(qt_galleryItemTypeList[m_itemIndex].itemType));
    }
    arguments->valueColumns = QVector<QGalleryTrackerValueColumn *>()
            << new QGalleryTrackerStringColumn
            << new QGalleryTrackerStringColumn
            << new QGalleryTrackerServiceIndexColumn
            << qt_createValueColumns(valueTypes + extendedValueTypes);
    arguments->propertyNames = valueNames + compositeNames + aliasNames;
    arguments->propertyAttributes = valueAttributes + compositeAttributes + aliasAttributes;
    arguments->propertyTypes = valueTypes + compositeTypes + aliasTypes;

    for (int i = 0; i < arguments->propertyAttributes.count(); ++i) {
        if (arguments->propertyAttributes.at(i) & IsResource)
            arguments->resourceKeys.append(i + arguments->valueOffset );
        arguments->propertyAttributes[i] &= PropertyMask;
    }

    for (int i = 0; i < arguments->fieldNames.count(); ++i)
        arguments->fieldNames[i].chop(4);
}

void QGalleryTrackerSchema::populateAggregateArguments(
        QGalleryTrackerResultSetArguments *arguments,
        QGalleryDBusInterfaceFactory *dbus,
        const QString &query,
        const QStringList &propertyNames,
        const QStringList &sortPropertyNames,
        int offset,
        int limit) const
{
    QStringList identityFields;
    QStringList aggregateFields;
    QStringList aggregates;
    QStringList identityNames;
    QStringList aggregateNames;
    QStringList aliasNames;
    QStringList compositeNames;
    QVector<QGalleryProperty::Attributes> identityAttributes;
    QVector<QGalleryProperty::Attributes> aggregateAttributes;
    QVector<QGalleryProperty::Attributes> aliasAttributes;
    QVector<QGalleryProperty::Attributes> compositeAttributes;
    QVector<QVariant::Type> identityTypes;
    QVector<QVariant::Type> aggregateTypes;
    QVector<QVariant::Type> aliasTypes;
    QVector<QVariant::Type> compositeTypes;
    QVector<int> identityColumns;

    const QGalleryAggregateType &type = qt_galleryAggregateTypeList[m_aggregateIndex];
    const QGalleryItemPropertyList &properties = type.properties;
    const QGalleryAggregatePropertyList &aggregateProperties = type.aggregateProperties;
    const QGalleryCompositePropertyList &compositeProperties = type.compositeProperties;

    for (int i = 0; i < type.identity.count; ++i)
        identityNames.append(type.identity[i].name);

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
        if (identityNames.contains(*it)
                || aggregateNames.contains(*it)
                || aliasNames.contains(*it)) {
            continue;
        }

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
        } else if ((propertyIndex = compositeProperties.indexOfProperty(*it)) >= 0) {
            const QGalleryItemPropertyList &dependencies
                    = compositeProperties[propertyIndex].dependencies;

            QVector<int> columns;
            for (int i = 0; i < dependencies.count; ++i) {
                const QString field = dependencies[i].field;

                int fieldIndex = identityFields.indexOf(field);

                Q_ASSERT(fieldIndex >= 0);

                columns.append(fieldIndex);
            }

            QGalleryProperty::Attributes attributes = QGalleryProperty::CanRead;
            if (compositeProperties[propertyIndex].writeFilterCondition)
                attributes |= QGalleryProperty::CanFilter;

            compositeNames.append(*it);
            compositeAttributes.append(attributes);
            compositeTypes.append(compositeProperties[propertyIndex].type);
            arguments->compositeColumns.append(
                    compositeProperties[propertyIndex].createColumn(columns));
        }
    }

    arguments->service = type.service;
    arguments->updateMask = qt_galleryAggregateTypeList[m_aggregateIndex].updateMask;
    arguments->identityWidth = identityColumns.count();
    arguments->valueOffset = 0;
    arguments->tableWidth = identityColumns.count() + aggregates.count();
    arguments->compositeOffset = arguments->tableWidth;
    arguments->queryInterface = dbus->metaDataInterface();
    arguments->sparql
            = QLatin1String("SELECT DISTINCT ")
            + identityFields.join(QLatin1String(" "))
            + QLatin1String(" WHERE{ ?x rdf:type ") + type.service + QLatin1String(" ")
            + query
            + QLatin1String("}")
            + qt_writeSorting(sortPropertyNames, properties);

    if (offset > 0)
        arguments->sparql += QString::fromLatin1(" OFFSET %1").arg(offset);
    if (limit > 0)
        arguments->sparql += QString::fromLatin1(" LIMIT %1").arg(limit);

    if (type.identity.count == 1)
        arguments->idColumn.reset(new QGalleryTrackerPrefixColumn(0, type.prefix));
    else
        arguments->idColumn.reset(new QGalleryTrackerCompositeIdColumn(identityColumns, type.prefix));
    arguments->urlColumn.reset(new QGalleryTrackerStaticColumn(QVariant()));
    arguments->typeColumn.reset(new QGalleryTrackerStaticColumn(type.itemType));
    arguments->valueColumns = qt_createValueColumns(identityTypes + aggregateTypes);
    arguments->propertyNames = identityNames + aggregateNames + compositeNames + aliasNames;
    arguments->propertyAttributes
            = identityAttributes
            + aggregateAttributes
            + compositeAttributes
            + aliasAttributes;
    arguments->propertyTypes = identityTypes + aggregateTypes + compositeTypes + aliasTypes;

    for (int i = 0; i < arguments->fieldNames.count(); ++i)
        arguments->fieldNames[i].chop(4);
}

QString QGalleryTrackerSchema::serviceForType( const QString &galleryType )
{
    QGalleryTypeList<QGalleryItemType> typeList(qt_galleryItemTypeList);
    int index = typeList.indexOfType(galleryType);
    if (index != -1)
        return typeList[index].service;

    QGalleryTypeList<QGalleryAggregateType> aggregateTypeList(qt_galleryAggregateTypeList);
    index = aggregateTypeList.indexOfType(galleryType);
    if (index != -1)
        return aggregateTypeList[index].service;

    qWarning() << galleryType << " does not exists";
    return "";
}

QTM_END_NAMESPACE
