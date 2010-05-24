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

#ifndef QGALLERYTRACKERSCHEMA_P_H
#define QGALLERYTRACKERSCHEMA_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qgalleryfilter.h"
#include "qgalleryproperty.h"

#include <QtCore/qstringlist.h>

QTM_BEGIN_NAMESPACE

class QGalleryDBusInterfaceFactory;
class QGalleryTrackerCompositeColumn;
class QGalleryTrackerImageColumn;
class QGalleryTrackerValueColumn;

struct QGalleryTrackerSortCriteria
{
    enum Flag
    {
        Sorted        = 0x01,
        ReverseSorted = 0x02,
        Ascending     = 0x04,
        Descending    = 0x08
    };

    QGalleryTrackerSortCriteria() : column(0), flags(0) {}
    QGalleryTrackerSortCriteria(short column, short flags) : column(column), flags(flags) {}

    short column;
    short flags;
};

class QGalleryTrackerSchema
{
public:
    QGalleryTrackerSchema();
    ~QGalleryTrackerSchema();

    bool isItemType() const { return m_itemIndex >= 0; }
    bool isAggregateType() const { return m_aggregateIndex >= 0; }

    QString itemType() const { return m_itemType; }
    void setItemType(const QString &type);

    void resolveTypeFromItemId(const QString &itemId);
    void resolveTypeFromService(const QString &service);

    QString buildIdQuery(int *error, const QString &itemId);
    QString buildContainerQuery(int *error, const QString &containerId);
    QString buildFilterQuery(
            int *error, const QString &containerId, const QGalleryFilter &filter) const;

    static QString uriFromItemId(int *error, const QVariant &itemId);
    QString itemIdFromUri(const QString &uri) const;

    QString service() const;

    QStringList supportedPropertyNames() const;
    QGalleryProperty::Attributes propertyAttributes(const QString &propertyName) const;

    QStringList identityFields() const;

    QStringList propertyNames() const { return m_propertyNames; }
    void setPropertyNames(const QStringList &names);

    QStringList sortPropertyNames() const { return m_sortPropertyNames; }
    void setSortPropertyNames(const QStringList &names);

    int identityWidth() const { return m_itemIndex >= 0 ? 2 : m_identityColumns.count(); }
    int valueOffset() const { return m_itemIndex >= 0 ? 2 : 0; }

    QStringList fields() const { return m_propertyFields; }
    QVector<QGalleryProperty::Attributes> propertyAttributes() const {
        return m_propertyAttributes; }

    QString queryMethod() const;
    QVariantList queryArguments(const QString &query) const;

    QGalleryTrackerCompositeColumn *createIdColumn() const;
    QGalleryTrackerCompositeColumn *createUrlColumn() const;
    QGalleryTrackerCompositeColumn *createTypeColumn() const;

    QVector<QGalleryTrackerValueColumn *> createValueColumns() const;
    QVector<QGalleryTrackerCompositeColumn *> createCompositeColumns() const;
    QVector<int> aliasColumns() const;
    QVector<QGalleryTrackerImageColumn *> createImageColumns(
            QGalleryDBusInterfaceFactory *dbus) const;

    QVector<QGalleryTrackerSortCriteria> sortCriteria() const { return m_sortCriteria; }

    int resolveColumns();

private:
    int resolveFileColumns();
    int resolveAggregateColumns();

    int m_itemIndex;
    int m_aggregateIndex;
    QString m_itemType;
    QStringList m_propertyNames;
    QStringList m_sortPropertyNames;
    QStringList m_propertyFields;
    QStringList m_aggregations;
    QStringList m_aggregateFields;
    QStringList m_sortFields;
    QVector<QGalleryProperty::Attributes> m_propertyAttributes;
    QVector<QVariant::Type> m_valueTypes;
    QVector<int> m_identityColumns;
    QVector<int> m_compositeColumns;
    QVector<int> m_aliasColumns;
    QVector<QVariant::Type> m_thumbnailTypes;
    QVector<QGalleryTrackerSortCriteria> m_sortCriteria;
};

QTM_END_NAMESPACE

#endif
