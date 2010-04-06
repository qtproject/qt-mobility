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

#include "qgallerytrackeraggregatelistresponse_p.h"

#include "qgallerytrackerschema_p.h"

QGalleryTrackerAggregateListResponse::QGalleryTrackerAggregateListResponse(
        const QDBusConnection &connection,
        const QGalleryTrackerSchema &schema,
        const QString &query,
        const QStringList &properties,
        const QStringList &sortProperties,
        int minimumPagedItems,
        QObject *parent)
    : QGalleryTrackerListResponse(minimumPagedItems, parent)
    , m_dbusInterface(
            QLatin1String("org.freedesktop.Tracker"),
            QLatin1String("/org/freedesktop/Tracker/Metadata"),
            QLatin1String("org.freedesktop.Tracker.Metadata"),
            connection)
    , m_type(schema.itemType())
    , m_service(schema.service())
    , m_query(query)
{
    QList<int> keys;

    m_propertyNames  = schema.identityPropertyNames();
    m_identityFields = schema.identityFields();

    for (int i = 0; i < m_identityFields.count(); ++i)
        keys.append(i);


    for (QStringList::const_iterator property = properties.begin(), end = properties.end();
            property != end;
            ++property) {
        QPair<QString, QGalleryTrackerSchema::AggregateType> field
                = schema.aggregateField(*property);

        if (!field.first.isEmpty()) {
            switch (field.second) {
            case QGalleryTrackerSchema::Count:
                m_aggregates.append(QLatin1String("COUNT"));
                break;
            case QGalleryTrackerSchema::Sum:
                m_aggregates.append(QLatin1String("SUM"));
                break;
            case QGalleryTrackerSchema::Concatenation:
                m_aggregates.append(QLatin1String("CONCAT"));
                break;
            }

            m_aggregateFields.append(field.first);
            m_propertyNames.append(*property);
            keys.append(keys.count());
        }
    }

    for (QStringList::const_iterator property = sortProperties.begin(), end = sortProperties.end();
            property != end;
            ++property) {
        QPair<QString, QGalleryTrackerSchema::AggregateType> field
                = schema.aggregateField(*property);

        if (!field.first.isEmpty()) {
            switch (field.second) {
            case QGalleryTrackerSchema::Count:
                m_aggregates.append(QLatin1String("COUNT"));
                break;
            case QGalleryTrackerSchema::Sum:
                m_aggregates.append(QLatin1String("SUM"));
                break;
            case QGalleryTrackerSchema::Concatenation:
                m_aggregates.append(QLatin1String("CONCAT"));
                break;
            }

            m_aggregateFields.append(field.first);
            m_propertyNames.append(*property);
            keys.append(keys.count());
        }
    }
    setKeys(keys);
}

QGalleryTrackerAggregateListResponse::~QGalleryTrackerAggregateListResponse()
{
}

QString QGalleryTrackerAggregateListResponse::toString(int key) const
{
    return m_propertyNames.value(key);
}

QUrl QGalleryTrackerAggregateListResponse::url(int) const
{
    return QUrl();
}

QString QGalleryTrackerAggregateListResponse::type(int) const
{
    return m_type;
}

QList<QGalleryResource> QGalleryTrackerAggregateListResponse::resources(int) const
{
    return QList<QGalleryResource>();
}

QDBusPendingCall QGalleryTrackerAggregateListResponse::queryRows(int offset, int limit)
{
    return m_dbusInterface.asyncCall(QLatin1String("GetUniqueValuesWithAggregates"),
            m_service,
            m_identityFields,
            m_query,
            m_aggregates,
            m_aggregateFields,
            false, // Search by service
            offset,
            limit);
}

QString QGalleryTrackerArtistListResponse::id(int index) const
{
    QString artistName = row(index).value(0);

    return !artistName.isEmpty()
            ? QLatin1String("artist::") + artistName
            : QString();
}

QString QGalleryTrackerAlbumListResponse::id(int index) const
{
    QStringList row = QGalleryTrackerListResponse::row(index);

    QString albumTitle = row.value(0);
    QString artistName = row.value(1);

    if (!albumTitle.isEmpty()) {
        return QLatin1String("album::")
                + artistName.replace(QLatin1String("/"), QLatin1String("//"))
                + QLatin1Char('/')
                + albumTitle;
    } else {
        return QString();
    }
}

QString QGalleryTrackerGenreListResponse::id(int index) const
{
    QString genre = row(index).value(0);

    return !genre.isEmpty()
            ? QLatin1String("genre::") + genre
            : QString();
}
