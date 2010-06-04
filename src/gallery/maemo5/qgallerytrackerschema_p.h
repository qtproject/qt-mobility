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

#include "qgalleryabstractrequest.h"
#include "qgalleryfilter.h"
#include "qgalleryproperty.h"

#include <QtCore/qstringlist.h>

QTM_BEGIN_NAMESPACE


class QGalleryDBusInterfaceFactory;
class QGalleryTrackerImageColumn;
class QGalleryTrackerValueColumn;

struct QGalleryTrackerItemListArguments;
struct QGalleryTrackerCountResponseArguments;

class QGalleryTrackerSchema
{
public:
    QGalleryTrackerSchema(const QString &itemType);
    ~QGalleryTrackerSchema();

    static QGalleryTrackerSchema fromItemId(const QString &itemId);
    static QGalleryTrackerSchema fromService(const QString &service);

    bool isItemType() const { return m_itemIndex >= 0; }
    bool isAggregateType() const { return m_aggregateIndex >= 0; }

    QString itemType() const;

    static QString uriFromItemId(int *error, const QVariant &itemId);
    QString itemIdFromUri(const QString &uri) const;

    static int serviceUpdateId(const QString &service);

    QStringList supportedPropertyNames() const;
    QGalleryProperty::Attributes propertyAttributes(const QString &propertyName) const;

    int prepareIdResponse(
            QGalleryTrackerItemListArguments *arguments,
            QGalleryDBusInterfaceFactory *dbus,
            const QString &itemId,
            const QStringList &propertyNames) const;

    int prepareFilterResponse(
            QGalleryTrackerItemListArguments *arguments,
            QGalleryDBusInterfaceFactory *dbus,
            QGalleryAbstractRequest::Scope scope,
            const QString &scopeItemId,
            const QGalleryFilter &filter,
            const QStringList &propertyNames,
            const QStringList &sortPropertyNames) const;

    int prepareCountResponse(
            QGalleryTrackerCountResponseArguments *arguments,
            QGalleryDBusInterfaceFactory *dbus,
            QGalleryAbstractRequest::Scope scope,
            const QString &scopeItemId,
            const QGalleryFilter &filter) const;

private:
    QGalleryTrackerSchema(int itemIndex, int aggregateIndex)
        : m_itemIndex(itemIndex), m_aggregateIndex(aggregateIndex) {}

    int buildFilterQuery(
            QString *query,
            QGalleryAbstractRequest::Scope scope,
            const QString &scopeItemId,
            const QGalleryFilter &filter) const;

    void populateItemArguments(
            QGalleryTrackerItemListArguments *arguments,
            QGalleryDBusInterfaceFactory *dbus,
            const QString &query,
            const QStringList &propertyNames,
            const QStringList &sortPropertyNames) const;
    void populateAggregateArguments(
            QGalleryTrackerItemListArguments *arguments,
            QGalleryDBusInterfaceFactory *dbus,
            const QString &query,
            const QStringList &propertyNames,
            const QStringList &sortPropertyNames) const;

    QVector<QGalleryTrackerValueColumn *> createValueColumns(
            const QVector<QVariant::Type> &types) const;
    QVector<QGalleryTrackerImageColumn *> createImageColumns(
            QGalleryDBusInterfaceFactory *dbus, const QVector<QVariant::Type> &types) const;

    int m_itemIndex;
    int m_aggregateIndex;
};

QTM_END_NAMESPACE

#endif
