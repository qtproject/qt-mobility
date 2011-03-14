/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGALLERYTRACKERRESULTSET_P_H
#define QGALLERYTRACKERRESULTSET_P_H

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

#include <qgalleryresultset.h>

#include "qgallerydbusinterface_p.h"
#include "qgallerytrackerlistcolumn_p.h"

QT_BEGIN_NAMESPACE
class QDBusPendingCallWatcher;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QGalleryDBusInterfaceFactory;
class QGalleryTrackerImageColumn;
class QGalleryTrackerSchema;

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

class QGalleryTrackerResultSetPrivate;

struct QGalleryTrackerResultSetArguments
{
    QGalleryTrackerResultSetArguments()
        : idColumn(0)
        , urlColumn(0)
        , typeColumn(0)
        , updateMask(0)
        , identityWidth(0)
        , tableWidth(0)
        , valueOffset(0)
        , compositeOffset(0)
    {
    }

    ~QGalleryTrackerResultSetArguments()
    {
        qDeleteAll(valueColumns);
        qDeleteAll(compositeColumns);
    }

    void clear()
    {
        valueColumns.clear();
        compositeColumns.clear();
    }

    QScopedPointer<QGalleryTrackerCompositeColumn> idColumn;
    QScopedPointer<QGalleryTrackerCompositeColumn> urlColumn;
    QScopedPointer<QGalleryTrackerCompositeColumn> typeColumn;
    int updateMask;
    int identityWidth;
    int tableWidth;
    int valueOffset;
    int compositeOffset;
    QGalleryDBusInterfacePointer queryInterface;
    QString queryMethod;
    QVariantList queryArguments;
    QStringList propertyNames;
    QStringList fieldNames;
    QVector<QGalleryProperty::Attributes> propertyAttributes;
    QVector<QVariant::Type> propertyTypes;
    QVector<QGalleryTrackerValueColumn *> valueColumns;
    QVector<QGalleryTrackerCompositeColumn *> compositeColumns;
    QVector<int> aliasColumns;
    QVector<QGalleryTrackerSortCriteria> sortCriteria;
    QVector<int> resourceKeys;
};

class QM_AUTOTEST_EXPORT QGalleryTrackerResultSet : public QGalleryResultSet
{
    Q_OBJECT
public:
    QGalleryTrackerResultSet(
            QGalleryTrackerResultSetArguments *arguments,
            bool autoUpdate,
            int cursorPosition,
            int minimumPagedItems,
            QObject *parent = 0);
    ~QGalleryTrackerResultSet();

    QStringList propertyNames() const;
    int propertyKey(const QString &property) const;
    QGalleryProperty::Attributes propertyAttributes(int key) const;
    QVariant::Type propertyType(int key) const;

    int itemCount() const;

    int currentIndex() const;
    bool fetch(int index);

    QVariant itemId() const;
    QUrl itemUrl() const;
    QString itemType() const;
    QList<QGalleryResource> resources() const;

    QVariant metaData(int key) const;
    bool setMetaData(int key, const QVariant &value);

    void cancel();

    bool waitForFinished(int msecs);

    bool event(QEvent *event);

public Q_SLOTS:
    void refresh(int serviceId = -1);

Q_SIGNALS:
    void itemEdited(const QString &service);

protected:
    QGalleryTrackerResultSet(QGalleryTrackerResultSetPrivate &dd, QObject *parent);

    void timerEvent(QTimerEvent *event);

private:
    Q_DECLARE_PRIVATE(QGalleryTrackerResultSet)
    Q_PRIVATE_SLOT(d_func(), void _q_queryFinished(QDBusPendingCallWatcher *))
    Q_PRIVATE_SLOT(d_func(), void _q_parseFinished())
};


QTM_END_NAMESPACE

#endif
