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

QTM_BEGIN_NAMESPACE

class QGalleryTrackerListColumn
{
public:
    QGalleryTrackerListColumn(const QString &name, QGalleryProperty::Attributes attributes)
        : m_name(name), m_attributes(attributes) {}
    virtual ~QGalleryTrackerListColumn() {}

    QString name() const { return m_name; }
    QGalleryProperty::Attributes attributes() const { return m_attributes; }

private:
    const QString m_name;
    const QGalleryProperty::Attributes m_attributes;
};

class QGalleryTrackerValueColumn : public QGalleryTrackerListColumn
{
public:
    enum SortFlag
    {
        Sorted        = 0x01,
        ReverseSorted = 0x2,
        Ascending     = 0x4,
        Descending    = 0x08
    };

    QGalleryTrackerValueColumn(
            const QString &name, QGalleryProperty::Attributes attributes, int sortFlags)
        : QGalleryTrackerListColumn(name, attributes), m_sortFlags(sortFlags) {}

    int sortFlags() const { return m_sortFlags; }

    virtual QVariant toVariant(const QString &string) const = 0;
    virtual QString toString(const QVariant &variant) const = 0;

    virtual int compare(const QVariant &value1, const QVariant &value2) const = 0;

private:
    int m_sortFlags;
};

class QGalleryTrackerCompositeColumn : public QGalleryTrackerListColumn
{
public:
    QGalleryTrackerCompositeColumn(const QString &name, QGalleryProperty::Attributes attributes)
        : QGalleryTrackerListColumn(name, attributes) {}

    virtual QVariant value(QVector<QVariant>::const_iterator row) const = 0;

    virtual int compare(
            QVector<QVariant>::const_iterator row1,
            QVector<QVariant>::const_iterator row2) const = 0;
    
    virtual bool isEqual(
            QVector<QVariant>::const_iterator row1,
            QVector<QVariant>::const_iterator row2) const = 0;
};

class QGalleryTrackerAliasColumn : public QGalleryTrackerListColumn
{
public:
    QGalleryTrackerAliasColumn(
            const QString &name, QGalleryProperty::Attributes attributes, int aliasedKey)
        : QGalleryTrackerListColumn(name, attributes), m_aliasedKey(aliasedKey) {}

    int aliasedKey() const { return m_aliasedKey; }

private:
    const int m_aliasedKey;
};

class QGalleryTrackerImageColumn : public QGalleryTrackerListColumn
{
public:
    QGalleryTrackerImageColumn(const QString &name, QGalleryProperty::Attributes attributes)
        : QGalleryTrackerListColumn(name, attributes) {}
    virtual ~QGalleryTrackerImageColumn() {}

    virtual QString imagePath(QVector<QVariant>::const_iterator row) const = 0;
};

class QGalleryTrackerItemListPrivate;

class QGalleryTrackerItemList : public QGalleryAbstractResponse
{
    Q_OBJECT
public:
    QGalleryTrackerItemList(
            QGalleryTrackerCompositeColumn *idColumn,
            QGalleryTrackerCompositeColumn *urlColumn,
            QGalleryTrackerCompositeColumn *typeColumn,
            int valueOffset,
            const QVector<QGalleryTrackerValueColumn *> &valueColumns,
            const QVector<QGalleryTrackerCompositeColumn *> &compositeColumns,
            const QVector<QGalleryTrackerAliasColumn *> &aliasColumns,
            const QVector<QGalleryTrackerImageColumn *> &imageColumns,
            QObject *parent = 0);
    ~QGalleryTrackerItemList();

    QStringList propertyNames() const;
    int propertyKey(const QString &property) const;
    QGalleryProperty::Attributes propertyAttributes(int key) const;

    int count() const;

    QString id(int index) const;
    QUrl url(int index) const;
    QString type(int index) const;
    QList<QGalleryResource> resources(int index) const;
    ItemStatus status(int index) const;

    QVariant metaData(int index, int key) const;
    void setMetaData(int index, int key, const QVariant &value);

private:
    Q_DECLARE_PRIVATE(QGalleryTrackerItemList)
};


QTM_END_NAMESPACE

#endif
