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

    for (QStringList::const_iterator it = m_fields.begin(), end = m_fields.end(); it != end; ++it)
        *statement += QLatin1String(", ") + *it;

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
    QStringList::const_iterator it = m_sortFields.begin();
    QStringList::const_iterator end = m_sortFields.end();

    if (it != end) {
        if (it->startsWith(QLatin1Char('+')))
            *statement = it->mid(1) + QLatin1String(" ASC");
        else if (it->startsWith(QLatin1Char('-')))
            *statement = it->mid(1) + QLatin1String(" DESC");
        else
            *statement = *it;

        for (++it; it != end; ++it) {
            if (it->startsWith(QLatin1Char('+')))
                *statement += QLatin1String(", ") + it->mid(1) + QLatin1String(" ASC");
            else if (it->startsWith(QLatin1Char('-')))
                *statement += QLatin1String(", ") + it->mid(1) + QLatin1String(" DESC");
            else
                *statement += QLatin1String(", ") + *it;
        }
    }

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
    const QString field = filter.fieldName();
    const QVariant value = filter.value();

    if (field.isEmpty() || value.isNull())
        return QGalleryDocumentRequest::InvalidFilter;

    switch (filter.matchFlags() & 0x0F) {
    case Qt::MatchExactly:
        *statement = QString::fromLatin1("%1 = '%2'").arg(field, value.toString());
        return 0;
    case Qt::MatchFixedString:
        *statement = QString::fromLatin1("%1 LIKE '%2'").arg(field, value.toString());
        return 0;
    case Qt::MatchContains:
        *statement = QString::fromLatin1("CONTAINS(%1, '\"%2\"')").arg(field, value.toString());
        return 0;
    case Qt::MatchStartsWith:
        *statement = QString::fromLatin1("CONTAINS(%1, \"%2*\"\')").arg(field, value.toString());
        return 0;
    case Qt::MatchEndsWith:
        *statement = QString::fromLatin1("CONTAINS(%1, \"*%2\"\')").arg(field, value.toString());
        return 0;
    case Qt::MatchWildcard:
        *statement = QString::fromLatin1("%1 LIKE '%2'")
                     .arg(field, value.toString().replace(QLatin1Char('*'), QLatin1Char('%')));
        return 0;
    case Qt::MatchRegExp:
    default:
        return QGalleryDocumentRequest::InvalidFilter;
    }
}

int QWS4GalleryQueryBuilder::buildWhere(
        QString *statement, const QGalleryMetaDataRangeFilter &filter)
{
    const QString field = filter.fieldName();
    const QVariant minimum = filter.minimumValue();
    const QVariant maximum = filter.maximumValue();

    switch (filter.rangeFlags()) {
    case QGalleryFilter::EqualsMinimum:
        if (!field.isEmpty() && !minimum.isNull()) {
            *statement = QString::fromLatin1("%1 = '%2'").arg(field, minimum.toString());
            return 0;
        } else {
            return QGalleryDocumentRequest::InvalidFilter;
        }
    case QGalleryFilter::GreaterThanMinimum:
        if (!field.isEmpty() && !minimum.isNull()) {
            *statement = QString::fromLatin1("%1 > '%2'").arg(field, minimum.toString());
            return 0;
        } else {
            return QGalleryDocumentRequest::InvalidFilter;
        }
    case QGalleryFilter::GreaterThanEqualsMinimum:
        if (!field.isEmpty() && !minimum.isNull()) {
            *statement = QString::fromLatin1("%1 >= '%2'").arg(field, minimum.toString());
            return 0;
        } else {
            return QGalleryDocumentRequest::InvalidFilter;
        }
    case QGalleryFilter::LessThanMaximum:
        if (!field.isEmpty() && !maximum.isNull()) {
            *statement = QString::fromLatin1("%1 < '%2'").arg(field, maximum.toString());
            return 0;
        } else {
            return QGalleryDocumentRequest::InvalidFilter;
        }
    case QGalleryFilter::EqualsMaximum:
        if (!field.isEmpty() && !maximum.isNull()) {
            *statement = QString::fromLatin1("%1 = '%2'").arg(field, maximum.toString());
            return 0;
        } else {
            return QGalleryDocumentRequest::InvalidFilter;
        }
    case QGalleryFilter::LessThanEqualsMaximum:
        if (!field.isEmpty() && !maximum.isNull()) {
            *statement = QString::fromLatin1("%1 <= '%2'").arg(field, maximum.toString());
            return 0;
        } else {
            return QGalleryDocumentRequest::InvalidFilter;
        }
    case QGalleryFilter::InclusiveRange:
        if (!field.isEmpty() && !minimum.isNull() && !maximum.isNull()) {
            *statement = QString::fromLatin1("%1 >= '%2' AND %1 <= '%3'")
                    .arg(field, minimum.toString(), maximum.toString());
            return 0;
        } else {
            return QGalleryDocumentRequest::InvalidFilter;
        }
    case QGalleryFilter::ExclusiveRange:
        if (!field.isEmpty() && !minimum.isNull() && !maximum.isNull()) {
            *statement = QString::fromLatin1("(%1 > '%2' AND %1 < %3)")
                    .arg(field, minimum.toString(), maximum.toString());
            return 0;
        } else {
            return QGalleryDocumentRequest::InvalidFilter;
        }
    default:
        return QGalleryDocumentRequest::InvalidFilter;
    }
}
