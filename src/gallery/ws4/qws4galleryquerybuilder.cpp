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

#include "qws4galleryquerybuilder_p.h"

#include "qgalleryrequest.h"

#include <QtCore/qstringbuilder.h>

namespace QWS4Gallery
{
    typedef QString (*buildQuery)(
            int *error,
            QVector<QWS4GalleryQueryBuilder::Column> *columns,
            const QWS4GalleryQueryBuilder &builder);

    typedef QString (*buildMetaDataFragment)(
            int *error, const QLatin1String &field, int flags, const QVariant &value);

    typedef QString (*buildMetaDataRangeFragment)(
            int *error,
            const QLatin1String &field,
            int flags,
            const QVariant &minimum,
            const QVariant &maximum);

    struct WhereFragmentMap
    {
        QLatin1String field;
        QLatin1String wsField;
        buildMetaDataFragment buildMetaData;
        buildMetaDataRangeFragment buildMetaDataRange;
    };

    struct FieldMap
    {
        QLatin1String field;
        QLatin1String wsField;
    };

    struct AggregateMap
    {
        QLatin1String field;
        QLatin1String wsField;
        QLatin1String alias;
    };

    struct TypeMap
    {
        QLatin1String type;
        buildQuery buildQuery;
    };
};

template <typename T>
static QString qt_encodeMetaData(const QVariant &value)
{
    return value.toString();
}

template <>
static QString qt_encodeMetaData<QString>(const QVariant &value)
{
    return QLatin1Char('\'')
            + value.toString().replace(QLatin1String("'"), QLatin1String("''"))
            + QLatin1Char('\'');
}

template<typename T>
static QString qt_buildMetaDataFragment(
        int *error, const QLatin1String &field, int flags, const QVariant &value)
{
    if ((flags & 0x0F) == Qt::MatchExactly) {
        return field + QLatin1String(" = ") + qt_encodeMetaData<T>(value);
    } else {
        *error = QGalleryDocumentRequest::InvalidFilter;

        return QString();
    }
}

template<>
static QString qt_buildMetaDataFragment<QString>(
        int *error, const QLatin1String &field, int flags, const QVariant &value)
{
    switch (flags & 0x0F) {
    case Qt::MatchExactly:
        return field + QLatin1String(" = ") + qt_encodeMetaData<QString>(value);
    case Qt::MatchFixedString:
        return field + QLatin1String(" LIKE ") + qt_encodeMetaData<QString>(value);
    case Qt::MatchContains:
        return QString::fromLatin1("CONTAINS(%1, '\"%2\"')").arg(field, value.toString()
                .replace(QLatin1String("'"), QLatin1String("''")));
    case Qt::MatchStartsWith:
        return QString::fromLatin1("CONTAINS(%1, \"%2*\"\')").arg(field, value.toString()
                .replace(QLatin1String("'"), QLatin1String("''")));
    case Qt::MatchEndsWith:
        return QString::fromLatin1("CONTAINS(%1, \"*%2\"\')").arg(field, value.toString()
                .replace(QLatin1String("'"), QLatin1String("''")));
    case Qt::MatchWildcard:
        return QString::fromLatin1("%1 LIKE '%2'").arg(field, value.toString()
                .replace(QLatin1Char('*'), QLatin1Char('%'))
                .replace(QLatin1String("'"), QLatin1String("''")));
    case Qt::MatchRegExp:
    default:
        *error = QGalleryDocumentRequest::InvalidFilter;
        return QString();
    }
}

template<typename T>
static QString qt_buildMetaDataRangeFragment(
        int *error,
        const QLatin1String &field,
        int flags,
        const QVariant &minimum,
        const QVariant &maximum)
{
    QString minimumFragment;
    QString maximumFragment;

    switch (flags & QGalleryFilter::GreaterThanEqualsMinimum) {
    case QGalleryFilter::EqualsMinimum:
        minimumFragment = field + QLatin1String(" = ") + qt_encodeMetaData<T>(minimum);
        break;
    case QGalleryFilter::GreaterThanMinimum:
        minimumFragment = field + QLatin1String(" > ") + qt_encodeMetaData<T>(minimum);
        break;
    case QGalleryFilter::GreaterThanEqualsMinimum:
        minimumFragment = field + QLatin1String(" >= ") + qt_encodeMetaData<T>(minimum);
        break;
    default:
        break;
    }

    switch (flags & QGalleryFilter::LessThanEqualsMaximum) {
    case QGalleryFilter::LessThanMaximum:
        maximumFragment = field + QLatin1String(" < ") + qt_encodeMetaData<T>(maximum);
        break;
    case QGalleryFilter::EqualsMaximum:
        maximumFragment = field + QLatin1String(" = ") + qt_encodeMetaData<T>(maximum);
        break;
    case QGalleryFilter::LessThanEqualsMaximum:
        maximumFragment = field + QLatin1String(" <= ") + qt_encodeMetaData<T>(maximum);
        break;
    default:
        break;
    }

    if ((flags & QGalleryFilter::GreaterThanEqualsMinimum)
            && (flags & QGalleryFilter::LessThanEqualsMaximum)) {
        return minimumFragment + QLatin1String(" AND ") + maximumFragment;
    } else if (flags & QGalleryFilter::GreaterThanEqualsMinimum) {
        return minimumFragment;
    } else if (flags & QGalleryFilter::LessThanEqualsMaximum) {
        return maximumFragment;
    } else {
        *error = QGalleryDocumentRequest::InvalidFilter;
        return QString();
    }
}

template <typename T, int N>
int qt_arraySize(const T (&)[N]) { return N; }

#define QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(TYPE, FIELD, WS_FIELD) \
    {                                                      \
        QLatin1String(#FIELD),                             \
        QLatin1String(WS_FIELD),                           \
        qt_buildMetaDataFragment<TYPE>,                    \
        qt_buildMetaDataRangeFragment<TYPE>                \
    }

#define QT_DEFINE_GALLERY_FIELD_MAP(FIELD, WS_FIELD) \
    { QLatin1String(#FIELD), QLatin1String(WS_FIELD) }

#define QT_DEFINE_GALLERY_AGGREGATE_MAP(FIELD, WS_FIELD, ALIAS) \
    { QLatin1String(#FIELD), QLatin1String(WS_FIELD), QLatin1String(ALIAS) }

static QStringList qt_buildAggregates(
        int *,
        QVector<QWS4GalleryQueryBuilder::Column> *columns,
        QStringList *fields,
        const QWS4Gallery::AggregateMap map[], int count)
{
    QStringList fragments;

    for (QStringList::iterator it = fields->begin(); it != fields->end();) {
        bool erase = false;
        for (int i = 0; i < count && !erase; ++i) {
            if (map[i].field == *it) {
                do {
                    fragments.append(map[i].wsField);
                    columns->append(qMakePair(QString(map[i].field), QString(map[i].alias)));
                } while (++i < count && map[i].field == *it);
                erase = true;
            }
        }
        if (erase)
            it = fields->erase(it);
        else
            ++it;
    }

    return fragments;
}

static QStringList qt_buildSelect(
        int *,
        QVector<QWS4GalleryQueryBuilder::Column> *columns,
        const QStringList &fields,
        const QWS4Gallery::FieldMap map[],
        int count)
{
    QStringList fragments;

    for (QStringList::const_iterator it = fields.begin(), end = fields.end(); it != end; ++it) {
        for (int i = 0; i < count; ++i) {
            if (map[i].field == *it) {
                do {
                    fragments.append(map[i].wsField);
                    columns->append(qMakePair(QString(map[i].field), QString(map[i].wsField)));
                } while (++i < count && map[i].field == *it);
                break;
            }
        }
    }

    return fragments;
}

static QString qt_buildOrderBy(
        int *, const QStringList &fields, const QWS4Gallery::FieldMap map[], int count)
{
    QStringList fragments;

    for (QStringList::const_iterator it = fields.begin(), end = fields.end(); it != end; ++it) {
        QStringRef field;
        if (it->startsWith(QLatin1Char('+')) || it->startsWith(QLatin1Char('-')))
            field = QStringRef(it.operator ->(), 1, it->length() - 1);
        else
            field = QStringRef(it.operator ->());

        for (int i = 0; i < count; ++i) {
            if (map[i].field == field) {
                do {
                    if (it->startsWith(QLatin1Char('+'))) {
                        fragments.append(map[i].wsField + QLatin1String(" ASC"));
                    } else if (it->startsWith(QLatin1Char('-'))) {
                        fragments.append(map[i].wsField + QLatin1String(" DESC"));
                    } else {
                        fragments.append(map[i].wsField);
                    }
                } while (++i < count && map[i].field == field);
                break;
            }
        }
    }

    return fragments.join(QLatin1String(", "));
}

static QString qt_buildWhere(
        int *error,
        const QGalleryMetaDataFilter &filter,
        const QWS4Gallery::WhereFragmentMap map[],
        int count)
{
    const QString field = filter.fieldName();

    for (int i = 0; i < count; ++i) {
        if (map[i].field == field) {
            return map[i].buildMetaData(
                error, map[i].wsField, filter.matchFlags(), filter.value());
        }
    }

    *error = QGalleryDocumentRequest::InvalidFilter;

    return QString();
}

static QString qt_buildWhere(
        int *error,
        const QGalleryMetaDataRangeFilter &filter,
        const QWS4Gallery::WhereFragmentMap map[],
        int count)
{
    const QString field = filter.fieldName();

    for (int i = 0; i < count; ++i) {
        if (map[i].field == field) {
            return map[i].buildMetaDataRange(
                    error,
                    map[i].wsField,
                    filter.rangeFlags(),
                    filter.minimumValue(),
                    filter.maximumValue());
        }
    }

    *error = QGalleryDocumentRequest::InvalidFilter;

    return QString();
}

static QString qt_buildWhere(
        int *error,
        const QGalleryUnionFilter &filter,
        const QWS4Gallery::WhereFragmentMap map[],
        int count)
{
    const QList<QGalleryFilter> filters = filter.filters();

    QStringList fragments;

    for (QList<QGalleryFilter>::const_iterator it = filters.begin(), end = filters.end();
            it != end && *error != 0;
            ++it) {
        switch (it->type()) {
        case QGalleryFilter::MetaData:
            fragments.append(qt_buildWhere(error, it->toMetaDataFilter(), map, count));
            break;
        case QGalleryFilter::MetaDataRange:
            fragments.append(qt_buildWhere(error, it->toMetaDataRangeFilter(), map, count));
            break;
        default:
            *error = QGalleryDocumentRequest::InvalidFilter;
            break;
        }
    }

    return fragments.join(QLatin1String(" AND "));
}

static QString qt_buildWhere(
        int *error,
        const QGalleryIntersectionFilter &filter,
        const QWS4Gallery::WhereFragmentMap map[],
        int count)
{
    const QList<QGalleryFilter> filters = filter.filters();

    if (filters.isEmpty())
        *error = QGalleryDocumentRequest::InvalidFilter;

    QStringList fragments;

    for (QList<QGalleryFilter>::const_iterator it = filters.begin(), end = filters.end();
            it != end && *error != 0;
            ++it) {
        switch (it->type()) {
        case QGalleryFilter::Union:
            fragments.append(qt_buildWhere(error, it->toUnionFilter(), map, count));
        case QGalleryFilter::MetaData:
            fragments.append(qt_buildWhere(error, it->toMetaDataFilter(), map, count));
            break;
        case QGalleryFilter::MetaDataRange:
            fragments.append(qt_buildWhere(error, it->toMetaDataRangeFilter(), map, count));
            break;
        default:
            *error = QGalleryDocumentRequest::InvalidFilter;
            break;
        }
    }

    return fragments.join(QLatin1String(" OR "));
}

/////////////////
//
// Media Queries.
//
/////////////////
static const QWS4Gallery::WhereFragmentMap qt_mediaWhereFragmentMap[] =
{
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, author, "System.Author"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, copyright, "System.Copyright"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, description, "System.Description"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, mimeType, "System.MIMEType"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, rating, "System.Rating"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, title, "System.Title"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, url, "System.ItemUrl"),

    // Media
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(int, duration, "System.Media.Duration"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, producer, "System.Media.Producer"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, writer, "System.Media.Writer"),

    // Audio/Music
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, artist, "System.Music.Artist"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, albumArtist, "System.Music.AlbumArtist"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, albumId, "System.Music.AlbumID"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, albumTitle, "System.Music.AlbumTitle"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, composer, "System.Music.Composer"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, discNumber, "System.Music.SetNumber"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, genre, "System.Music.Genre"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, trackNumber, "System.Music.TrackNumber"),

    // Image, Video common.
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(int, width, "System.Image.HorizontalSize"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(int, width, "System.Video.FrameWidth"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(int, height, "System.Image.VerticalSize"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(int, height, "System.Video.FrameHeight"),

    // Image/Photo
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(int, dateTaken, "System.Photo.DateTaken"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(int, cameraManufacturer, "System.Photo.CameraManufacturer"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(int, cameraModel, "System.Photo.CameraModel"),

    // Video
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, director, "System.Video.Director")
};

static const QWS4Gallery::FieldMap qt_mediaFieldMap[] =
{
    QT_DEFINE_GALLERY_FIELD_MAP(author, "System.Author"),
    QT_DEFINE_GALLERY_FIELD_MAP(copyright, "System.Copyright"),
    QT_DEFINE_GALLERY_FIELD_MAP(description, "System.Description"),
    QT_DEFINE_GALLERY_FIELD_MAP(mimeType, "System.MIMEType"),
    QT_DEFINE_GALLERY_FIELD_MAP(rating, "System.Rating"),
    QT_DEFINE_GALLERY_FIELD_MAP(title, "System.Title"),
    QT_DEFINE_GALLERY_FIELD_MAP(url, "System.ItemUrl"),

    // Media
    QT_DEFINE_GALLERY_FIELD_MAP(duration, "System.Media.Duration"),
    QT_DEFINE_GALLERY_FIELD_MAP(producer, "System.Media.Producer"),
    QT_DEFINE_GALLERY_FIELD_MAP(writer, "System.Media.Writer"),

    // Audio/Music
    QT_DEFINE_GALLERY_FIELD_MAP(artist, "System.Music.Artist"),
    QT_DEFINE_GALLERY_FIELD_MAP(albumArtist, "System.Music.AlbumArtist"),
    QT_DEFINE_GALLERY_FIELD_MAP(albumId, "System.Music.AlbumID"),
    QT_DEFINE_GALLERY_FIELD_MAP(albumTitle, "System.Music.AlbumTitle"),
    QT_DEFINE_GALLERY_FIELD_MAP(composer, "System.Music.Composer"),
    QT_DEFINE_GALLERY_FIELD_MAP(discNumber, "System.Music.SetNumber"),
    QT_DEFINE_GALLERY_FIELD_MAP(genre, "System.Music.Genre"),
    QT_DEFINE_GALLERY_FIELD_MAP(trackNumber, "System.Music.TrackNumber"),

    // Image, Video common.
    QT_DEFINE_GALLERY_FIELD_MAP(width, "System.Image.HorizontalSize"),
    QT_DEFINE_GALLERY_FIELD_MAP(width, "System.Video.FrameWidth"),
    QT_DEFINE_GALLERY_FIELD_MAP(height, "System.Image.VerticalSize"),
    QT_DEFINE_GALLERY_FIELD_MAP(height, "System.Video.FrameHeight"),

    // Image/Photo
    QT_DEFINE_GALLERY_FIELD_MAP(dateTaken, "System.Photo.DateTaken"),
    QT_DEFINE_GALLERY_FIELD_MAP(cameraManufacturer, "System.Photo.CameraManufacturer"),
    QT_DEFINE_GALLERY_FIELD_MAP(cameraModel, "System.Photo.CameraModel"),

    // Video
    QT_DEFINE_GALLERY_FIELD_MAP(director, "System.Video.Director")
};

static QString qt_buildMediaWhere(int *error, const QGalleryDocumentFilter &filter)
{
    const QStringList ids = filter.documentIds();

    if (ids.isEmpty())
        *error = QGalleryDocumentRequest::InvalidFilter;

    QStringList fragments;

    for (QStringList::const_iterator it = ids.begin(), end = ids.end(); it != end; ++it)
        fragments.append(QLatin1String("(System.ItemUrl = '") + *it + QLatin1Char('\''));

    return fragments.join(QLatin1String(" OR "));
}

static QString qt_buildMediaWhere(int *error, const QGalleryDocumentUrlFilter &filter)
{
    const QList<QUrl> urls = filter.documentUrls();

    if (urls.isEmpty())
        *error = QGalleryDocumentRequest::InvalidFilter;

    QStringList fragments;

    for (QList<QUrl>::const_iterator it = urls.begin(), end = urls.end(); it != end; ++it)
        fragments.append(QLatin1String("(System.ItemUrl = '") + it->toString() + QLatin1Char('\''));

    return fragments.join(QLatin1String(" OR "));
}

static QString qt_buildMediaWhere(int *error, const QGalleryContainerFilter &filter)
{
    const QString id = filter.containerId();

    if (id.isEmpty()) {
        *error = QGalleryDocumentRequest::InvalidFilter;

        return QString();
    } else {
        return QLatin1String("DIRECTORY = '") + id + QLatin1Char('\'');
    }
}

static QString qt_buildMediaWhere(int *error, const QGalleryContainerUrlFilter &filter)
{
    const QUrl url = filter.containerUrl();

    if (url.isEmpty()) {
        *error = QGalleryDocumentRequest::InvalidFilter;

        return QString();
    } else {
        return QLatin1String("DIRECTORY = '") + url.toString() + QLatin1Char('\'');
    }
}

static QString qt_buildMediaQuery(
        int *error,
        QVector<QWS4GalleryQueryBuilder::Column> *columns,
        const QWS4GalleryQueryBuilder &builder)
{
    QString where;

    const QGalleryFilter filter = builder.filter();
    switch (filter.type()) {
    case QGalleryFilter::Invalid:
        break;
    case QGalleryFilter::Document:
        where = qt_buildMediaWhere(error, filter.toDocumentFilter());
        break;
    case QGalleryFilter::DocumentUrl:
        where = qt_buildMediaWhere(error, filter.toDocumentUrlFilter());
        break;
    case QGalleryFilter::Container:
        where = qt_buildMediaWhere(error, filter.toContainerFilter());
        break;
    case QGalleryFilter::ContainerUrl:
        where = qt_buildMediaWhere(error, filter.toContainerUrlFilter());
        break;
    case QGalleryFilter::Intersection:
        where = qt_buildWhere(
                error,
                filter.toIntersectionFilter(),
                qt_mediaWhereFragmentMap,
                qt_arraySize(qt_mediaWhereFragmentMap));
        break;
    case QGalleryFilter::Union:
        where = qt_buildWhere(
                error,
                filter.toUnionFilter(),
                qt_mediaWhereFragmentMap,
                qt_arraySize(qt_mediaWhereFragmentMap));
        break;
    case QGalleryFilter::MetaData:
        where = qt_buildWhere(
                error,
                filter.toMetaDataFilter(),
                qt_mediaWhereFragmentMap,
                qt_arraySize(qt_mediaWhereFragmentMap));
        break;
    case QGalleryFilter::MetaDataRange:
        where = qt_buildWhere(
                error,
                filter.toMetaDataRangeFilter(),
                qt_mediaWhereFragmentMap,
                qt_arraySize(qt_mediaWhereFragmentMap));
        break;
    default:
        *error = QGalleryDocumentRequest::InvalidFilter;
        return QString();
    }

    QStringList selectFragments = qt_buildSelect(
            error, columns, builder.fields(), qt_mediaFieldMap, qt_arraySize(qt_mediaFieldMap));
    if (*error != 0)
        return QString();

    QString orderBy = qt_buildOrderBy(
            error, builder.sortFields(), qt_mediaFieldMap, qt_arraySize(qt_mediaFieldMap));
    if (*error != 0)
        return QString();

    selectFragments.append(QLatin1String("System.ItemUrl"));
    selectFragments.append(QLatin1String("System.Kind"));
    selectFragments.removeDuplicates();

    QString query
            = QLatin1String("SELECT ")
            + selectFragments.join(QLatin1String(", "))
            + QLatin1String("\nFROM SystemIndex");

    if (!where.isEmpty())
        query += QLatin1String("\nWHERE ") + where;

    if (!orderBy.isEmpty())
        query += QLatin1String("\nORDER BY ") + orderBy;

    return query;
}

/////////////////
//
// Artist Queries.
//
/////////////////
static const QWS4Gallery::WhereFragmentMap qt_artistWhereFragmentMap[] =
{
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, title, "System.Music.Artist"),

    // Audio/Music
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, artist, "System.Music.Artist"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, albumTitle, "System.Music.AlbumTitle"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, genre, "System.Music.Genre")
};

static const QWS4Gallery::AggregateMap qt_artistAggregateFieldMap[] =
{
    QT_DEFINE_GALLERY_AGGREGATE_MAP(rating, "AVG(System.Rating)", "AVG_SystemRating"),

    // Media
    QT_DEFINE_GALLERY_AGGREGATE_MAP(duration, "SUM(System.Media.Duration)", "SUM_SystemMediaDuration"),

    // Audio/Music
    QT_DEFINE_GALLERY_AGGREGATE_MAP(albumCount, "CHILDCOUNT()", "CHILDCOUNT"),
    QT_DEFINE_GALLERY_AGGREGATE_MAP(trackCount, "COUNT()", "COUNT")
};

static const QWS4Gallery::FieldMap qt_artistSelectFieldMap[] =
{
    QT_DEFINE_GALLERY_FIELD_MAP(title, "System.Music.Artist"),
    // Audio/Music
    QT_DEFINE_GALLERY_FIELD_MAP(artist, "System.Music.Artist")
};

static const QWS4Gallery::FieldMap qt_artistOrderByFieldMap[] =
{
    QT_DEFINE_GALLERY_FIELD_MAP(title, "System.Music.Artist"),
    QT_DEFINE_GALLERY_FIELD_MAP(rating, "AVG(System.Rating)"),

    // Media
    QT_DEFINE_GALLERY_FIELD_MAP(duration, "SUM(System.Media.Duration)"),

    // Audio/Music
    QT_DEFINE_GALLERY_FIELD_MAP(albumCount, "CHILDCOUNT()"),
    QT_DEFINE_GALLERY_FIELD_MAP(trackCount, "COUNT()"),
    QT_DEFINE_GALLERY_FIELD_MAP(artist, "System.Music.Artist")
};

static QString qt_buildArtistWhere(int *error, const QGalleryDocumentFilter &filter)
{
    *error = QGalleryDocumentRequest::InvalidFilter;

    return QString();
}

static QString qt_buildArtistWhere(int *error, const QGalleryContainerFilter &filter)
{
    *error = QGalleryDocumentRequest::InvalidFilter;

    return QString();
}

static QString qt_buildArtistQuery(
        int *error,
        QVector<QWS4GalleryQueryBuilder::Column> *columns,
        const QWS4GalleryQueryBuilder &builder)
{
    QString where;

    const QGalleryFilter filter = builder.filter();
    switch (filter.type()) {
    case QGalleryFilter::Invalid:
        break;
    case QGalleryFilter::Document:
        where = qt_buildArtistWhere(error, filter.toDocumentFilter());
        break;
    case QGalleryFilter::Container:
        where = qt_buildArtistWhere(error, filter.toContainerFilter());
        break;
    case QGalleryFilter::Intersection:
        where = qt_buildWhere(
                error,
                filter.toIntersectionFilter(),
                qt_artistWhereFragmentMap,
                qt_arraySize(qt_artistWhereFragmentMap));
        break;
    case QGalleryFilter::Union:
        where = qt_buildWhere(
                error,
                filter.toUnionFilter(),
                qt_artistWhereFragmentMap,
                qt_arraySize(qt_artistWhereFragmentMap));
        break;
    case QGalleryFilter::MetaData:
        where = qt_buildWhere(
                error,
                filter.toMetaDataFilter(),
                qt_artistWhereFragmentMap,
                qt_arraySize(qt_artistWhereFragmentMap));
        break;
    case QGalleryFilter::MetaDataRange:
        where = qt_buildWhere(
                error,
                filter.toMetaDataRangeFilter(),
                qt_artistWhereFragmentMap,
                qt_arraySize(qt_artistWhereFragmentMap));
    default:
        *error = QGalleryDocumentRequest::InvalidFilter;
        return QString();
    }

    QStringList fields = builder.fields();

    QStringList aggregateFragments = qt_buildAggregates(
            error,
            columns,
            &fields,
            qt_artistAggregateFieldMap,
            qt_arraySize(qt_artistAggregateFieldMap));
    if (*error != 0)
        return QString();

    QStringList selectFragments = qt_buildSelect(
            error,
            columns,
            fields,
            qt_artistSelectFieldMap,
            qt_arraySize(qt_artistSelectFieldMap));
    if (*error != 0)
        return QString();

    QString orderBy = qt_buildOrderBy(
            error,
            builder.sortFields(),
            qt_artistOrderByFieldMap,
            qt_arraySize(qt_artistOrderByFieldMap));
    if (*error != 0)
        return QString();

    selectFragments.append(QLatin1String("System.Music.Artist"));
    selectFragments.append(QLatin1String("System.Music.AlbumTitle"));
    selectFragments.removeDuplicates();

    QString query = QLatin1String("GROUP ON System.Music.Artist");

    if (!aggregateFragments.isEmpty())
        query += QLatin1String("\nAGGREGATE ") + aggregateFragments.join(QLatin1String(", "));

    if (!orderBy.isEmpty())
        query += QLatin1String("\nORDER BY ") + orderBy;

    query += QLatin1String("\nOVER(GROUP ON System.Music.AlbumTitle OVER(\nSELECT ")
            + selectFragments.join(QLatin1String(", "))
            + QLatin1String("\nFROM SystemIndex");

    if (!where.isEmpty()) {
        query += QLatin1String("\nWHERE System.Music.Artist IS NOT NULL AND ")
                + where
                + QLatin1String(")))");
    } else {
        query += QLatin1String("\nWHERE System.Music.Artist IS NOT NULL))");
    }
    return query;
}

/////////////////
//
// Album Queries.
//
/////////////////

static const QWS4Gallery::WhereFragmentMap qt_albumWhereFragmentMap[] =
{
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, title, "System.Music.AlbumTitle"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, rating, "System.Rating"),

    // Media
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, producer, "System.Media.Producer"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, writer, "System.Media.Writer"),

    // Audio/Music
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, artist, "System.Music.Artist"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, albumArtist, "System.Music.AlbumArtist"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, albumTitle, "System.Music.AlbumTitle"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, composer, "System.Music.Composer"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, discNumber, "System.Music.SetNumber"),
    QT_DEFINE_GALLERY_WHERE_FRAGMENT_MAP(QString, genre, "System.Music.Genre")
};

static const QWS4Gallery::AggregateMap qt_albumAggregateFieldMap[] =
{
    QT_DEFINE_GALLERY_AGGREGATE_MAP(rating, "AVG(System.Rating)", "AVG_SystemRating"),

    // Media
    QT_DEFINE_GALLERY_AGGREGATE_MAP(duration, "SUM(System.Media.Duration)", "SUM_SystemMediaDuration"),

    // Audio/Music
    QT_DEFINE_GALLERY_AGGREGATE_MAP(albumCount, "CHILDCOUNT()", "CHILDCOUNT"),
    QT_DEFINE_GALLERY_AGGREGATE_MAP(trackCount, "COUNT()", "COUNT")
};

static const QWS4Gallery::FieldMap qt_albumSelectFieldMap[] =
{
    QT_DEFINE_GALLERY_FIELD_MAP(title, "System.Music.AlbumTitle"),

    // Audio/Music
    QT_DEFINE_GALLERY_FIELD_MAP(artist, "System.Music.Artist"),
    QT_DEFINE_GALLERY_FIELD_MAP(albumArtist, "System.Music.AlbumArtist"),
    QT_DEFINE_GALLERY_FIELD_MAP(albumTitle, "System.Music.AlbumTitle")
};

static const QWS4Gallery::FieldMap qt_albumOrderByFieldMap[] =
{
    QT_DEFINE_GALLERY_FIELD_MAP(title, "System.Music.AlbumTitle"),
    QT_DEFINE_GALLERY_FIELD_MAP(rating, "AVG(System.Rating)"),

    // Media
    QT_DEFINE_GALLERY_FIELD_MAP(duration, "SUM(System.Media.Duration)"),

    // Audio/Music
    QT_DEFINE_GALLERY_FIELD_MAP(trackCount, "CHILDCOUNT()"),
    QT_DEFINE_GALLERY_FIELD_MAP(artist, "System.Music.Artist"),
    QT_DEFINE_GALLERY_FIELD_MAP(albumArtist, "System.Music.AlbumArtist"),
    QT_DEFINE_GALLERY_FIELD_MAP(albumTitle, "System.Music.AlbumTitle")
};

static QString qt_buildAlbumWhere(int *error, const QGalleryDocumentFilter &filter)
{
    *error = QGalleryDocumentRequest::InvalidFilter;

    return QString();
}

static QString qt_buildAlbumWhere(int *error, const QGalleryContainerFilter &filter)
{
    *error = QGalleryDocumentRequest::InvalidFilter;

    return QString();
}

static QString qt_buildAlbumQuery(
        int *error,
        QVector<QWS4GalleryQueryBuilder::Column> *columns,
        const QWS4GalleryQueryBuilder &builder)
{
    QString where;

    const QGalleryFilter filter = builder.filter();
    switch (filter.type()) {
    case QGalleryFilter::Invalid:
        break;
    case QGalleryFilter::Document:
        where = qt_buildAlbumWhere(error, filter.toDocumentFilter());
        break;
    case QGalleryFilter::Container:
        where = qt_buildAlbumWhere(error, filter.toContainerFilter());
        break;
    case QGalleryFilter::Intersection:
        where = qt_buildWhere(
                error,
                filter.toIntersectionFilter(),
                qt_albumWhereFragmentMap,
                qt_arraySize(qt_albumWhereFragmentMap));
        break;
    case QGalleryFilter::Union:
        where = qt_buildWhere(
                error,
                filter.toUnionFilter(),
                qt_albumWhereFragmentMap,
                qt_arraySize(qt_albumWhereFragmentMap));
        break;
    case QGalleryFilter::MetaData:
        where = qt_buildWhere(
                error,
                filter.toMetaDataFilter(),
                qt_albumWhereFragmentMap,
                qt_arraySize(qt_albumWhereFragmentMap));
        break;
    case QGalleryFilter::MetaDataRange:
        where = qt_buildWhere(
                error,
                filter.toMetaDataRangeFilter(),
                qt_albumWhereFragmentMap,
                qt_arraySize(qt_albumWhereFragmentMap));
    default:
        *error = QGalleryDocumentRequest::InvalidFilter;
        return QString();
    }

    QStringList fields = builder.fields();

    QStringList aggregateFragments = qt_buildAggregates(
            error,
            columns,
            &fields,
            qt_albumAggregateFieldMap,
            qt_arraySize(qt_albumAggregateFieldMap));
    if (*error != 0)
        return QString();

    QStringList selectFragments = qt_buildSelect(
            error,
            columns,
            fields,
            qt_albumSelectFieldMap,
            qt_arraySize(qt_albumSelectFieldMap));
    if (*error != 0)
        return QString();

    QString orderBy = qt_buildOrderBy(
            error,
            builder.sortFields(),
            qt_albumOrderByFieldMap,
            qt_arraySize(qt_albumOrderByFieldMap));
    if (*error != 0)
        return QString();

    selectFragments.append(QLatin1String("System.Music.AlbumTitle"));
    selectFragments.append(QLatin1String("System.Music.Artist"));
    selectFragments.removeDuplicates();

    QString query = QLatin1String("GROUP ON System.Music.AlbumTitle");

    if (!aggregateFragments.isEmpty())
        query += QLatin1String("\nAGGREGATE ") + aggregateFragments.join(QLatin1String(", "));

    if (!orderBy.isEmpty())
        query += QLatin1String("\nORDER BY ") + orderBy;

    query += QLatin1String("\nOVER(GROUP ON System.Music.Artist OVER(\nSELECT ")
            + selectFragments.join(QLatin1String(", "))
            + QLatin1String("\nFROM SystemIndex");

    if (!where.isEmpty()) {
        query += QLatin1String("\nWHERE System.Music.AlbumTitle IS NOT NULL AND ")
                + where
                + QLatin1String(")))");
    } else {
        query += QLatin1String("\nWHERE System.Music.AlbumTitle IS NOT NULL))");
    }
    return query;
}

//////////////////////////
//
// QWS4GalleryQueryBuilder
//
//////////////////////////

static const QWS4Gallery::TypeMap qt_queryTypeMap[] =
{
    { QLatin1String("Artist"), qt_buildArtistQuery },
    { QLatin1String("Album"), qt_buildAlbumQuery },
};

QWS4GalleryQueryBuilder::QWS4GalleryQueryBuilder()
    : m_startIndex(0)
    , m_maximumCount(-1)
{
}

QWS4GalleryQueryBuilder::~QWS4GalleryQueryBuilder()
{
}

int QWS4GalleryQueryBuilder::buildQuery()
{
    int error = 0;
    m_columns.clear();
    m_fields.removeDuplicates();

    for (int i = 0; i < qt_arraySize(qt_queryTypeMap); ++i) {
        if (qt_queryTypeMap[i].type == m_documentType) {
            m_query = qt_queryTypeMap[i].buildQuery(&error, &m_columns, *this);

            qDebug("Built Query:\n%s", qPrintable(m_query));

            return error;
        }
    }

    m_query = qt_buildMediaQuery(&error, &m_columns, *this);

    qDebug("Built Query:\n%s", qPrintable(m_query));

    return error;
}
