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

#ifndef QGALLERYTRACKERITEMLIST_P_H
#define QGALLERYTRACKERITEMLIST_P_H

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

#include <qgalleryabstractresponse.h>

#include "qgallerydbusinterface_p.h"

QT_BEGIN_NAMESPACE
class QDBusPendingCallWatcher;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QGalleryDBusInterfaceFactory;
class QGalleryTrackerCompositeColumn;
class QGalleryTrackerImageColumn;
class QGalleryTrackerSchema;
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

class QGalleryTrackerItemListPrivate;

struct QGalleryTrackerItemListArguments
{
    QGalleryTrackerItemListArguments()
        : idColumn(0)
        , urlColumn(0)
        , typeColumn(0)
        , updateMask(0)
        , identityWidth(0)
    {
    }

    QGalleryTrackerCompositeColumn *idColumn;
    QGalleryTrackerCompositeColumn *urlColumn;
    QGalleryTrackerCompositeColumn *typeColumn;
    int updateMask;
    int identityWidth;
    int valueOffset;
    QGalleryDBusInterfacePointer queryInterface;
    QString queryMethod;
    QVariantList queryArguments;
    QStringList propertyNames;
    QStringList fieldNames;
    QVector<QGalleryProperty::Attributes> propertyAttributes;
    QVector<QGalleryTrackerValueColumn *> valueColumns;
    QVector<QGalleryTrackerCompositeColumn *> compositeColumns;
    QVector<int> aliasColumns;
    QVector<QGalleryTrackerImageColumn *> imageColumns;
    QVector<QGalleryTrackerSortCriteria> sortCriteria;
    QVector<int> resourceKeys;
};

class Q_AUTOTEST_EXPORT QGalleryTrackerItemList : public QGalleryAbstractResponse
{
    Q_OBJECT
public:
    QGalleryTrackerItemList(
            const QGalleryTrackerItemListArguments &arguments,
            bool live,
            int cursorPosition,
            int minimumPagedItems,
            QObject *parent = 0);
    ~QGalleryTrackerItemList();

    QStringList propertyNames() const;
    int propertyKey(const QString &property) const;
    QGalleryProperty::Attributes propertyAttributes(int key) const;

    void setCursorPosition(int position);

    int count() const;

    QVariant id(int index) const;
    QUrl url(int index) const;
    QString type(int index) const;
    QList<QGalleryResource> resources(int index) const;
    ItemStatus status(int index) const;

    QVariant metaData(int index, int key) const;
    void setMetaData(int index, int key, const QVariant &value);

    void cancel();

    bool waitForFinished(int msecs);

    bool event(QEvent *event);

public Q_SLOTS:
    void refresh(int serviceId = -1);

Q_SIGNALS:
    void itemEdited(const QString &service);

protected:
    QGalleryTrackerItemList(QGalleryTrackerItemListPrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE(QGalleryTrackerItemList)
    Q_PRIVATE_SLOT(d_func(), void _q_queryFinished(QDBusPendingCallWatcher *))
    Q_PRIVATE_SLOT(d_func(), void _q_parseFinished())
};


QTM_END_NAMESPACE

#endif
