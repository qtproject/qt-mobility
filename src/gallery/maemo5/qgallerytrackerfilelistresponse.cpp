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

#include "qgallerytrackerfilelistresponse_p.h"

#include "qgallerytrackerschema_p.h"

QGalleryTrackerFileListResponse::QGalleryTrackerFileListResponse(
        const QDBusConnection &connection,
        const QGalleryTrackerSchema &schema,
        const QString &query,
        const QStringList &properties,
        const QStringList &sortProperties,
        int minimumPagedItems,
        QObject *parent)
    : QGalleryTrackerListResponse(schema, minimumPagedItems, parent)
    , m_dbusInterface(
            QLatin1String("org.freedesktop.Tracker"),
            QLatin1String("/org/freedesktop/Tracker/Search"),
            QLatin1String("org.freedesktop.Tracker.Search"),
            connection)
    , m_service(schema.service())
    , m_query(query)
{
    for (QStringList::const_iterator property = properties.begin(), end = properties.end();
            property != end;
            ++property) {
        QString field = schema.field(*property);

        if (!field.isEmpty() && !m_fields.contains(field)) {
            m_fields.append(field);
            m_propertyNames.append(*property);
        }
    }

    m_sortDescending = !sortProperties.isEmpty()
            && sortProperties.first().startsWith(QLatin1Char('-'));

    for (QStringList::const_iterator property = sortProperties.begin(), end = sortProperties.end();
            property != end;
            ++property) {
        QString field = property->startsWith(QLatin1Char('+'))
                     || property->startsWith(QLatin1Char('-'))
                ? schema.field(property->mid(1))
                : schema.field(*property);

        if (!field.isEmpty())
            m_sortFields.append(field);
    }
}

QGalleryTrackerFileListResponse::~QGalleryTrackerFileListResponse()
{
}

QStringList QGalleryTrackerFileListResponse::propertyNames() const
{
    return m_propertyNames;
}

int QGalleryTrackerFileListResponse::propertyKey(const QString &name) const
{
    return m_propertyNames.indexOf(name) + 2;
}

QUrl QGalleryTrackerFileListResponse::url(int index) const
{
    QUrl url;

    QStringList row = QGalleryTrackerListResponse::row(index);

    if (!row.isEmpty()) {
        url.setScheme(QLatin1String("file"));
        url.setPath(row.at(0));
    }

    return url;
}

QString QGalleryTrackerFileListResponse::type(int index) const
{
    return QGalleryTrackerSchema::typeFromService(row(index).value(1));
}

QList<QGalleryResource> QGalleryTrackerFileListResponse::resources(int index) const
{
    QStringList row = QGalleryTrackerListResponse::row(index);

    if (!row.isEmpty()) {
        QMap<int, QVariant> properties;

        for (QList<int>::const_iterator it = m_resourceKeys.begin(), end = m_resourceKeys.end();
                it != end;
                ++it) {
            QString value = row.value(*it);

            if (!value.isEmpty()) {
                properties.insert(*it, value);
            }
        }

        return QList<QGalleryResource>() << QGalleryResource(QUrl(row.at(0)), properties);
    } else {
        return QList<QGalleryResource>();
    }
}

QDBusPendingCall QGalleryTrackerFileListResponse::queryRows(int offset, int limit)
{
    QList<QVariant> arguments = QList<QVariant>()
            << 0 //live query id.
            << m_service
            << m_fields
            << QString() // Search text
            << QStringList()
            << m_query
            << false // Search by service
            << m_sortFields
            << m_sortDescending
            << offset
            << limit;

    return m_dbusInterface.asyncCallWithArgumentList (QLatin1String("Query"), arguments);
}
