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

typedef int (*buildQueryFragment)(QString *fragment, int flags, const QVariant &value);
typedef int (*buildQueryRangeFragment)(
        QString *fragment, int flags, const QVariant &minimum, const QVariant &maximum);

static int buildInvalidQueryFragment(QString *, const QVariant &)
{
    return QGalleryDocumentRequest::InvalidFilter;
}

static int buildInvalidQueryRangeFragment(QString *, int, const QVariant &, const QVariant &)
{
    return QGalleryDocumentRequest::InvalidFilter;
}

struct QWS4QueryFragment
{
    virtual int build(QString *fragment, int flags, const QVariant &value) const = 0;
    virtual int buildRange(
            QString *fragment,
            int flags,
            const QVariant &minimum,
            const QVariant &maximum) const = 0;
};

template <typename T>
struct QWS4QueryTemplateFragment : public QWS4QueryFragment
{
    QWS4QueryTemplateFragment(const char *field)
        : m_field(field)
    {
    }

    int build(QString *fragment, int flags, const QVariant &value) const;
    int buildRange(
            QString *fragment, int flags, const QVariant &minimum, const QVariant &maximum) const;

    QString encode(const QVariant &value) const;

private:
    inline QLatin1String latin1(const char *str) const { return QLatin1String(str); }
    QLatin1String m_field;
};


template <>
int QWS4QueryTemplateFragment<QString>::build(QString *fragment, int flags, const QVariant &value) const
{
    switch (flags & 0x0F) {
    case Qt::MatchExactly:
        *fragment = m_field + latin1(" = ") + encode(value);
        return 0;
    case Qt::MatchFixedString:
        *fragment = m_field + latin1(" LIKE ") + encode(value);
        return 0;
    case Qt::MatchContains:
        *fragment = QString::fromLatin1("CONTAINS(%1, '\"%2\"')").arg(m_field, value.toString());
        return 0;
    case Qt::MatchStartsWith:
        *fragment = QString::fromLatin1("CONTAINS(%1, \"%2*\"\')").arg(m_field, value.toString());
        return 0;
    case Qt::MatchEndsWith:
        *fragment = QString::fromLatin1("CONTAINS(%1, \"*%2\"\')")
                .arg(m_field, value.toString());
        return 0;
    case Qt::MatchWildcard:
        *fragment = QString::fromLatin1("%1 LIKE '%2'")
                .arg(m_field, value.toString().replace(QLatin1Char('*'), QLatin1Char('%')));
        return 0;
    case Qt::MatchRegExp:
    default:
        return QGalleryDocumentRequest::InvalidFilter;
    }
}

template<>
QString QWS4QueryTemplateFragment<QString>::encode(const QVariant &value) const
{   // Have to do better than this, remove quotes etc.
    return QChar(L'\'') + value.toString() + QChar(L'\'');
}

template<typename T>
int QWS4QueryTemplateFragment<T>::build(QString *fragment, int flags, const QVariant &value) const
{
    if ((flags & 0x0F) == Qt::MatchExactly) {
        *fragment = m_field + latin1(" = ") + encode(value);

        return 0;
    } else {
        return QGalleryDocumentRequest::InvalidFilter;
    }
}

template<typename T>
int QWS4QueryTemplateFragment<T>::buildRange(
        QString *fragment, int flags, const QVariant &minimum, const QVariant &maximum) const
{
    QString minimumFragment;
    QString maximumFragment;

    switch (flags & QGalleryFilter::GreaterThanEqualsMinimum) {
    case QGalleryFilter::EqualsMinimum:
        minimumFragment = m_field + latin1(" = ") + encode(minimum);
        break;
    case QGalleryFilter::GreaterThanMinimum:
        minimumFragment = m_field + latin1(" > ") + encode(minimum);
        break;
    case QGalleryFilter::GreaterThanEqualsMinimum:
        minimumFragment = m_field + latin1(" >= ") + encode(minimum);
        break;
    default:
        break;
    }

    switch (flags & QGalleryFilter::LessThanEqualsMaximum) {
    case QGalleryFilter::LessThanMaximum:
        maximumFragment = m_field + latin1(" < ") + encode(maximum);
        break;
    case QGalleryFilter::EqualsMaximum:
        maximumFragment = m_field + latin1(" = ") + encode(maximum);
        break;
    case QGalleryFilter::LessThanEqualsMaximum:
        maximumFragment = m_field + latin1(" <= ") + encode(maximum);
        break;
    default:
        break;
    }

    if ((flags & QGalleryFilter::GreaterThanEqualsMinimum)
            && (flags & QGalleryFilter::LessThanEqualsMaximum)) {
        *fragment = minimumFragment + latin1(" AND ") + maximumFragment;
    } else if (flags & QGalleryFilter::GreaterThanEqualsMinimum) {
        *fragment = minimumFragment;
    } else if (flags & QGalleryFilter::LessThanEqualsMaximum) {
        *fragment = maximumFragment;
    } else {
        return QGalleryDocumentRequest::InvalidFilter;
    }
    return 0;
}

template <typename T>
QString QWS4QueryTemplateFragment<T>::encode(const QVariant &value) const
{ 
    return value.toString();
}


template <typename T>
struct QWS4QueryCompositeFragment : public QWS4QueryFragment
{
    QWS4QueryCompositeFragment(const char *fieldA, const char *fieldB)
        : m_a(fieldA)
        , m_b(fieldB)
    {
    }

    int build(QString *fragment, int flags, const QVariant &value) const;
    int buildRange(
            QString *fragment, int flags, const QVariant &minimum, const QVariant &maximum) const;

private:
    QWS4QueryTemplateFragment<T> m_a;
    QWS4QueryTemplateFragment<T> m_b;
};

template <typename T>
int QWS4QueryCompositeFragment<T>::build(QString *fragment, int flags, const QVariant &value) const
{
    QString fragmentA;
    QString fragmentB;

    int result = m_a.build(fragment, flags, value);
    if (result != 0)
        return result;

    result = m_b.build(fragment, flags, value);
    if (result != 0)
        return result;

    *fragment = QLatin1Char('(') 
            + fragmentA
            + QLatin1String(" OR ")
            + fragmentB
            + QLatin1Char(')');

    return 0;
}

template <typename T>
int QWS4QueryCompositeFragment<T>::buildRange(
            QString *fragment, int flags, const QVariant &minimum, const QVariant &maximum) const
{
    QString fragmentA;
    QString fragmentB;

    int result = m_a.buildRange(fragment, flags, minimum, maximum);
    if (result != 0)
        return result;

    result = m_b.buildRange(fragment, flags, minimum, maximum);
    if (result != 0)
        return result;

    *fragment = QLatin1Char('(') 
            + fragmentA
            + QLatin1String(" OR ")
            + fragmentB
            + QLatin1Char(')');

    return 0;
}

#define QWS4_DEFINE_QUERY_FRAGMENT(TYPE, FIELD, WS_FIELD) \
    static const QWS4QueryTemplateFragment<TYPE> qt_##FIELD##QueryFragment(WS_FIELD);

// Item
QWS4_DEFINE_QUERY_FRAGMENT(QString, author     , "System.Author")
QWS4_DEFINE_QUERY_FRAGMENT(QString, copyright  , "System.Copyright")
QWS4_DEFINE_QUERY_FRAGMENT(QString, description, "System.Description")
QWS4_DEFINE_QUERY_FRAGMENT(QString, mimeType   , "System.MIMEType")
QWS4_DEFINE_QUERY_FRAGMENT(int    , rating     , "System.Rating")
QWS4_DEFINE_QUERY_FRAGMENT(QString, title      , "System.Title")
QWS4_DEFINE_QUERY_FRAGMENT(QString, url        , "System.ItemURL")

// Media
QWS4_DEFINE_QUERY_FRAGMENT(int    , duration, "System.Media.Duration")
QWS4_DEFINE_QUERY_FRAGMENT(QString, producer, "System.Media.Producer")
QWS4_DEFINE_QUERY_FRAGMENT(QString, writer  , "System.Media.Writer")

// Audio/Music
QWS4_DEFINE_QUERY_FRAGMENT(QString, artist     , "System.Music.Artist")
QWS4_DEFINE_QUERY_FRAGMENT(QString, albumArtist, "System.Music.AlbumArtist")
QWS4_DEFINE_QUERY_FRAGMENT(QString, albumId    , "System.Music.AlbumId")
QWS4_DEFINE_QUERY_FRAGMENT(QString, albumTitle , "System.Music.AlbumTitle")
QWS4_DEFINE_QUERY_FRAGMENT(QString, composer   , "System.Music.Composer")
QWS4_DEFINE_QUERY_FRAGMENT(QString, discNumber , "System.Music.SetNumber")
QWS4_DEFINE_QUERY_FRAGMENT(QString, genre      , "System.Music.Genre")
QWS4_DEFINE_QUERY_FRAGMENT(QString, trackNumber, "System.Music.TrackNumber")

// Image/Video
static const QWS4QueryCompositeFragment<int> qt_widthQueryFragment( 
        "System.Image.HorizontalSize", "System.Video.FrameWidth");
static const QWS4QueryCompositeFragment<int> qt_heightQueryFragment(
        "System.Image.VerticalSize", "System.Video.FrameHeight");

// Image/Photo
QWS4_DEFINE_QUERY_FRAGMENT(QString, dateTaken         , "System.Photo.DateTaken")
QWS4_DEFINE_QUERY_FRAGMENT(QString, cameraManufacturer, "System.Photo.CameraManufacturer")
QWS4_DEFINE_QUERY_FRAGMENT(QString, cameraModel       , "System.Photo.CameraModel")

// Video
QWS4_DEFINE_QUERY_FRAGMENT(QString, director, "System.Video.Director")

struct QWS4QueryFragmentLookupItem
{
    const QLatin1String field;
    const QWS4QueryFragment *fragment;
};

#define QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(FIELD) \
    { QLatin1String(#FIELD), &qt_##FIELD##QueryFragment }

static const QWS4QueryFragmentLookupItem qt_queryFragmentLookup[] =
{
    // Item
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(author),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(copyright),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(description),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(mimeType),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(rating),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(title),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(url),

    // Media
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(duration),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(producer),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(writer),

    // Audio/Music
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(artist),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(albumArtist),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(albumId),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(albumTitle),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(composer),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(discNumber),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(genre),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(trackNumber),

    // Image, Video common.
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(width),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(height),

    // Image/Photo
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(dateTaken),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(cameraManufacturer),
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(cameraModel),

    // Video
    QWS4_QUERY_FRAGMENT_LOOKUP_ITEM(director)
};

struct QWS4SelectFragmentLookupItem
{
    const QLatin1String field;
    const QLatin1String fragment;
};

#define QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(FIELD, FRAGMENT) \
    { QLatin1String(#FIELD), QLatin1String(FRAGMENT) }

static const QWS4SelectFragmentLookupItem qt_selectFragmentLookup[] =
{
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(author, "System.Author"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(copyright, "System.Copyright"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(description, "System.Description"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(mimeType, "System.MIMEType"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(rating, "System.Rating"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(title, "System.Title"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(url, "System.ItemUrl"),

    // Media
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(duration, "System.Media.Duration"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(producer, "System.Media.Producer"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(writer, "System.Media.Writer"),

    // Audio/Music
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(artist, "System.Music.Artist"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(albumArtist, "System.Music.AlbumArtist"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(albumId, "System.Music.AlbumID"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(albumTitle, "System.Music.AlbumTitle"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(composer, "System.Music.Composer"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(discNumber, "System.Music.SetNumber"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(genre, "System.Music.Genre"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(trackNumber, "System.Music.TrackNumber"),

    // Image, Video common.
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(width, "System.Image.HorizontalSize"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(width, "System.Video.FrameWidth"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(height, "System.Image.VerticalSize"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(height, "System.Video.FrameHeight"),

    // Image/Photo
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(dateTaken, "System.Photo.DateTaken"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(cameraManufacturer, "System.Photo.CameraManufacturer"),
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(cameraModel, "System.Photo.CameraModel"),

    // Video
    QWS4_SELECT_FRAGMENT_LOOKUP_ITEM(director, "System.Video.Director")
};

struct QWS4OrderByFragmentLookupItem
{
    const QLatin1String field;
    const QLatin1String fragment;
};

#define QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(FIELD, FRAGMENT) \
    { QLatin1String(#FIELD), QLatin1String(FRAGMENT) }

static const QWS4OrderByFragmentLookupItem qt_orderByFragmentLookup[] =
{
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(author, "System.Author"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(copyright, "System.Copyright"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(description, "System.Description"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(mimeType, "System.MIMEType"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(rating, "System.Rating"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(title, "System.Title"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(url, "System.ItemUrl"),

    // Media
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(duration, "System.Media.Duration"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(producer, "System.Media.Producer"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(writer, "System.Media.Writer"),

    // Audio/Music
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(artist, "System.Music.Artist"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(albumArtist, "System.Music.AlbumArtist"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(albumId, "System.Music.AlbumID"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(albumTitle, "System.Music.AlbumTitle"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(composer, "System.Music.Composer"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(discNumber, "System.Music.SetNumber"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(genre, "System.Music.Genre"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(trackNumber, "System.Music.TrackNumber"),

    // Image, Video common.
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(width, "System.Image.HorizontalSize"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(width, "System.Video.FrameWidth"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(height, "System.Image.VerticalSize"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(height, "System.Video.FrameHeight"),

    // Image/Photo
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(dateTaken, "System.Photo.DateTaken"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(cameraManufacturer, "System.Photo.CameraManufacturer"),
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(cameraModel, "System.Photo.CameraModel"),

    // Video
    QWS4_ORDERBY_FRAGMENT_LOOKUP_ITEM(director, "System.Video.Director")
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
    QString select;
    QString where;
    QString orderBy;
    
    int result = buildSelect(&select);
    if (result != 0)
        return result;

    if ((result = buildWhere(&where)) != 0)
        return result;
    
    if ((result = buildOrderBy(&orderBy)) != 0)
        return result;

    //if (m_maximumCount > 0) {
    //    m_query = QString::fromLatin1("SELECT TOP %1 %2\nFROM SystemIndex")
    //            .arg(m_maximumCount + qMax(0, m_startIndex))
    //            .arg(select);
    //} else {
        m_query = QString::fromLatin1("SELECT %2\nFROM SystemIndex").arg(select);
    //}

    if (!where.isEmpty())
        m_query += QString::fromLatin1("\nWHERE %1").arg(where);

    if (!orderBy.isEmpty())
        m_query += QString::fromLatin1("\nORDER BY %1").arg(orderBy);

    return 0;
}

int QWS4GalleryQueryBuilder::buildSelect(QString *statement)
{
    *statement = QLatin1String("System.ItemUrl, System.Kind");

    static int count = sizeof(qt_selectFragmentLookup) / sizeof(QWS4SelectFragmentLookupItem);

    for (QStringList::const_iterator it = m_fields.begin(), end = m_fields.end(); it != end; ++it) {
        for (int i = 0; i < count; ++i) {
            if (qt_selectFragmentLookup[i].field == *it) {
                do {
                    *statement += QLatin1String(", ") + qt_selectFragmentLookup[i].fragment;
                } while (++i < count && qt_selectFragmentLookup[i].field == *it);
            }
        }
    }

    return 0;
}

int QWS4GalleryQueryBuilder::buildWhere(QString *statement)
{
    int result = 0;

    switch (m_filter.type()) {
    case QGalleryFilter::Invalid:
        break;
    case QGalleryFilter::Document:
        result = buildWhere(statement, m_filter.toDocumentFilter());
        break;
    case QGalleryFilter::DocumentUrl:
        result = buildWhere(statement, m_filter.toDocumentUrlFilter());
        break;
    case QGalleryFilter::Container:
        result = buildWhere(statement, m_filter.toContainerFilter());
        break;
    case QGalleryFilter::ContainerUrl:
        result = buildWhere(statement, m_filter.toContainerUrlFilter());
        break;
    case QGalleryFilter::Intersection:
        result = buildWhere(statement, m_filter.toIntersectionFilter());
        break;
    case QGalleryFilter::Union:
        result = buildWhere(statement, m_filter.toUnionFilter());
        break;
    case QGalleryFilter::MetaData:
        result = buildWhere(statement, m_filter.toMetaDataFilter());
        break;
    case QGalleryFilter::MetaDataRange:
        result = buildWhere(statement, m_filter.toMetaDataRangeFilter());
        break;
    default:
        result = QGalleryDocumentRequest::InvalidFilter;
        break;
    }

    if (result == 0 && !m_documentType.isEmpty()) {
        if (!statement->isEmpty()) {
            *statement = QString::fromLatin1("System.Kind = '%1' AND %2")
                    .arg(m_documentType, *statement);
        } else {
            *statement = QString::fromLatin1("System.Kind = '%1'").arg(m_documentType);
        }
    }

    return result;
}

int QWS4GalleryQueryBuilder::buildOrderBy(QString *statement)
{
    QStringList fragments;

    static int count = sizeof(qt_orderByFragmentLookup) / sizeof(QWS4OrderByFragmentLookupItem);

    QStringList::const_iterator it = m_sortFields.begin();
    QStringList::const_iterator end = m_sortFields.end();

    for (; it != end; ++it) {
        QStringRef field;
        if (it->startsWith(QLatin1Char('+')) || it->startsWith(QLatin1Char('-')))
            field = QStringRef(it.operator ->(), 1, it->length() - 1);
        else
            field = QStringRef(it.operator ->());

        for (int i = 0; i < count; ++i) {
            if (qt_orderByFragmentLookup[i].field == field) {
                do {
                    if (it->startsWith(QLatin1Char('+'))) {
                        fragments.append(
                                qt_orderByFragmentLookup[i].fragment + QLatin1String(" ASC"));
                    } else if (it->startsWith(QLatin1Char('-'))) {
                        fragments.append(
                                qt_orderByFragmentLookup[i].fragment + QLatin1String(" DESC"));
                    } else {
                        fragments.append(qt_orderByFragmentLookup[i].fragment);
                    }
                } while (++i < count && qt_orderByFragmentLookup[i].field == field);
                break;
            }
        }
    }

    *statement = fragments.join(QLatin1String(", "));

    return 0;
}

int QWS4GalleryQueryBuilder::buildWhere(
        QString *statement, const QGalleryDocumentFilter &filter)
{
    const QStringList documentIds = filter.documentIds();

    QStringList::const_iterator it = documentIds.begin();
    QStringList::const_iterator end= documentIds.end();

    if (it != end) {
        *statement = QString::fromLatin1("System.ItemUrl = '%1'").arg(*it);

        for (++it; it != end; ++it)
            *statement += QString::fromLatin1(" OR System.ItemUrl = '%1'").arg(*it);

        if (documentIds.count() > 1)
            *statement = QLatin1Char('(') + *statement + QLatin1Char(')');

        return 0;
    } else {
        return QGalleryDocumentRequest::InvalidFilter;
    }
}

int QWS4GalleryQueryBuilder::buildWhere(
        QString *statement, const QGalleryDocumentUrlFilter &filter)
{
    const QList<QUrl> urls = filter.documentUrls();

    if (urls.isEmpty())
        return QGalleryDocumentRequest::InvalidFilter;

    QList<QUrl>::const_iterator it = urls.begin();
    QList<QUrl>::const_iterator end= urls.end();

    if (it != end) {
        *statement = QString::fromLatin1("System.ItemUrl = '%1'").arg(it->toString());

        for (++it; it != end; ++it)
            *statement += QString::fromLatin1(" OR System.ItemUrl = '%1'").arg(it->toString());

        if (urls.count() > 1)
            *statement = QLatin1Char('(') + *statement + QLatin1Char(')');

        return 0;
    } else {
        return QGalleryDocumentRequest::InvalidFilter;
    }
}

int QWS4GalleryQueryBuilder::buildWhere(
        QString *statement, const QGalleryContainerFilter &filter)
{
    const QString id = filter.containerId();

    if (!id.isEmpty()) {
        *statement = QString::fromLatin1("DIRECTORY = '%1'").arg(id);
        return 0;
    } else {
        return QGalleryDocumentRequest::InvalidFilter;
    }
}

int QWS4GalleryQueryBuilder::buildWhere(
        QString *statement, const QGalleryContainerUrlFilter &filter)
{
    const QUrl url = filter.containerUrl();

    if (!url.isEmpty()) {
        *statement = QString::fromLatin1("DIRECTORY = '%1'").arg(url.toString());
        return 0;
    } else {
        return QGalleryDocumentRequest::InvalidFilter;
    }
}

int QWS4GalleryQueryBuilder::buildWhere(
        QString *statement, const QGalleryIntersectionFilter &filter)
{
    const QList<QGalleryFilter> filters = filter.filters();

    QStringList statements;
    
    for (QList<QGalleryFilter>::const_iterator it = filters.begin(), end = filters.end();
            it != end;
            ++it) {
        QString subStatement;

        int result = 0;
    
        switch (it->type()) {
        case QGalleryFilter::Union:
            result = buildWhere(&subStatement, it->toUnionFilter());
            break;
        case QGalleryFilter::MetaData:
            result = buildWhere(&subStatement, it->toMetaDataFilter());
            break;
        case QGalleryFilter::MetaDataRange:
            result = buildWhere(&subStatement, it->toMetaDataRangeFilter());
            break;
        default:
            result = QGalleryDocumentRequest::InvalidFilter;
            break;
        }

        if (result != 0)
            return result;

        statements.append(subStatement);
    }

    switch (statements.count()) {
    case 0:
        return QGalleryDocumentRequest::InvalidFilter;
    case 1:
        *statement = statements.first();
        return 0;
    default:
        *statement = QLatin1Char('(') + statements.join(QLatin1String(" OR ")) + QLatin1Char(')');
        return 0;
    }
}

int QWS4GalleryQueryBuilder::buildWhere(
        QString *statement, const QGalleryUnionFilter &filter)
{
    const QList<QGalleryFilter> filters = filter.filters();

    QStringList statements;

    for (QList<QGalleryFilter>::const_iterator it = filters.begin(), end = filters.end();
            it != end;
            ++it) {
        QString subStatement;

        int result = 0;

        switch (it->type()) {
        case QGalleryFilter::MetaData:
            result = buildWhere(&subStatement, it->toMetaDataFilter());
            break;
        case QGalleryFilter::MetaDataRange:
            result = buildWhere(&subStatement, it->toMetaDataRangeFilter());
            break;
        default:
            result = QGalleryDocumentRequest::InvalidFilter;
            break;
        }

        if (result != 0)
            return result;

        statements.append(subStatement);
    }

    switch (statements.count()) {
    case 0:
        return QGalleryDocumentRequest::InvalidFilter;
    case 1:
        *statement = statements.first();
        return 0;
    default:
        *statement = QLatin1Char('(') + statements.join(QLatin1String(" OR ")) + QLatin1Char(')');
        return 0;
    }
}

int QWS4GalleryQueryBuilder::buildWhere(
        QString *statement, const QGalleryMetaDataFilter &filter)
{
    static int count = sizeof(qt_queryFragmentLookup) / sizeof(QWS4QueryFragmentLookupItem);

    const QString field = filter.fieldName();

    for (int i = 0; i < count; ++i) {
        if (qt_queryFragmentLookup[i].field == field) {
            return qt_queryFragmentLookup[i].fragment->build(
                    statement, filter.matchFlags(), filter.value());
        }
    }

    return QGalleryDocumentRequest::InvalidFilter;
}

int QWS4GalleryQueryBuilder::buildWhere(
        QString *statement, const QGalleryMetaDataRangeFilter &filter)
{
    static int count = sizeof(qt_queryFragmentLookup) / sizeof(QWS4QueryFragmentLookupItem);

    const QString field = filter.fieldName();

    for (int i = 0; i < count; ++i) {
        if (qt_queryFragmentLookup[i].field == field) {
            return qt_queryFragmentLookup[i].fragment->buildRange(
                    statement, filter.rangeFlags(), filter.minimumValue(), filter.maximumValue());
        }
    }

    return QGalleryDocumentRequest::InvalidFilter;
}
