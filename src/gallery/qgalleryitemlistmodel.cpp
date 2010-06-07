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

#include "qgalleryitemlistmodel.h"

#include "qgalleryitemlist.h"

#include <QtCore/qstringlist.h>

QTM_BEGIN_NAMESPACE

class QGalleryItemListModelPrivate
{
    Q_DECLARE_PUBLIC(QGalleryItemListModel)
public:
    typedef QHash<int, QString> RoleProperties;

    QGalleryItemListModelPrivate()
        : q_ptr(0)
        , itemList(0)
        , updateCursorPosition(true)
        , rowCount(0)
        , columnCount(0)
    {
    }

    void updateRoles(int column);
    void updateRoles();

    void _q_inserted(int index, int count);
    void _q_removed(int index, int count);
    void _q_moved(int from, int to, int count);
    void _q_metaDataChanged(int index, int count, const QList<int> &keys);

    QGalleryItemListModel *q_ptr;
    QGalleryItemList *itemList;
    bool updateCursorPosition;
    int rowCount;
    int columnCount;
    QVector<RoleProperties> roleProperties;
    QVector<int> roleKeys;
    QVector<int> columnOffsets;
    QVector<Qt::ItemFlags> itemFlags;
};

void QGalleryItemListModelPrivate::updateRoles(int column)
{
    if (itemList) {
        const QStringList propertyNames = itemList->propertyNames();

        int offset = column != 0 ? columnOffsets.at(column - 1) : 0;

        const RoleProperties &properties = roleProperties.at(column);

        typedef RoleProperties::const_iterator iterator;
        for (iterator it = properties.begin(), end = properties.end(); it != end; ++it) {
            if (propertyNames.contains(it.value())) {
                const int key = itemList->propertyKey(it.value());

                roleKeys.append(it.key());
                roleKeys.append(key);

                offset += 2;

                if ((it.key() == Qt::DisplayRole || it.key() == Qt::EditRole)
                        && (itemList->propertyAttributes(key) & QGalleryProperty::CanWrite)) {
                    itemFlags[column] = Qt::ItemIsEditable;
                } else {
                    itemFlags[column] = Qt::ItemFlags();
                }
            }
        }

        const int difference = columnOffsets.at(column) - offset;
        for (int i = column + 1; i < columnCount; ++i)
            columnOffsets[i] += difference;

        columnOffsets[column] = offset;
    }
}

void QGalleryItemListModelPrivate::updateRoles()
{
    roleKeys.clear();

    if (itemList) {
        const QStringList propertyNames = itemList->propertyNames();

        int offset = 0;

        for (int column = 0; column < columnCount; ++column) {
            const RoleProperties &properties = roleProperties.at(column);

            typedef RoleProperties::const_iterator iterator;
            for (iterator it = properties.begin(), end = properties.end(); it != end; ++it) {
                if (propertyNames.contains(it.value())) {
                    const int key = itemList->propertyKey(it.value());

                    roleKeys.append(it.key());
                    roleKeys.append(key);

                    offset += 2;

                    if ((it.key() == Qt::DisplayRole || it.key() == Qt::EditRole)
                            && (itemList->propertyAttributes(key) & QGalleryProperty::CanWrite)) {
                        itemFlags[column] = Qt::ItemIsEditable;
                    } else {
                        itemFlags[column] = Qt::ItemFlags();
                    }
                }
            }
            columnOffsets[column] = offset;
        }
    } else {
        for (int column = 0; column < columnCount; ++column) {
            columnOffsets[column] = 0;
            itemFlags[column] = Qt::ItemFlags();
        }
    }
}

void QGalleryItemListModelPrivate::_q_inserted(int index, int count)
{
    Q_Q(QGalleryItemListModel);

    q->beginInsertRows(QModelIndex(), index, index + count - 1);
    rowCount = itemList->count();
    q->endInsertRows();
}

void QGalleryItemListModelPrivate::_q_removed(int index, int count)
{
    Q_Q(QGalleryItemListModel);

    q->beginRemoveRows(QModelIndex(), index, index + count -1);
    rowCount = itemList->count();
    q->endRemoveRows();
}

void QGalleryItemListModelPrivate::_q_moved(int from, int to, int count)
{
    Q_Q(QGalleryItemListModel);

    q->beginMoveRows(QModelIndex(), from, from + count - 1, QModelIndex(), to);
    q->endMoveRows();
}

void QGalleryItemListModelPrivate::_q_metaDataChanged(int index, int count, const QList<int> &keys)
{
    Q_Q(QGalleryItemListModel);

    Q_UNUSED(keys);

    if (columnCount > 0) {
        emit q->dataChanged(
                q->createIndex(index, 0), q->createIndex(index + count - 1, columnCount - 1));
    }
}

/*!
    \class QGalleryItemListModel
*/

/*!
*/

QGalleryItemListModel::QGalleryItemListModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d_ptr(new QGalleryItemListModelPrivate)
{
    d_ptr->q_ptr = this;
}

/*!
*/

QGalleryItemListModel::~QGalleryItemListModel()
{
}

/*!
    \property QGalleryItemListModel::itemList
*/

QGalleryItemList *QGalleryItemListModel::itemList() const
{
    return d_func()->itemList;
}

void QGalleryItemListModel::setItemList(QGalleryItemList *list)
{
    Q_D(QGalleryItemListModel);

    if (d->itemList) {
        disconnect(d->itemList, SIGNAL(inserted(int,int)), this, SLOT(_q_inserted(int,int)));
        disconnect(d->itemList, SIGNAL(removed(int,int)), this, SLOT(_q_removed(int,int)));
        disconnect(d->itemList, SIGNAL(moved(int,int,int)), this, SLOT(_q_moved(int,int,int)));
        disconnect(d->itemList, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(_q_metaDataChanged(int,int,QList<int>)));

        if (d->rowCount > 0) {
            beginRemoveRows(QModelIndex(), 0, d->rowCount - 1);
            d->rowCount = 0;
            endRemoveRows();
        }
    }

    d->itemList = list;

    d->updateRoles();

    if (d->itemList) {
        connect(d->itemList, SIGNAL(inserted(int,int)), this, SLOT(_q_inserted(int,int)));
        connect(d->itemList, SIGNAL(removed(int,int)), this, SLOT(_q_removed(int,int)));
        connect(d->itemList, SIGNAL(moved(int,int,int)), this, SLOT(_q_moved(int,int,int)));
        connect(d->itemList, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(_q_metaDataChanged(int,int,QList<int>)));

        if (d->itemList->count() > 0) {
            beginInsertRows(QModelIndex(), 0, d->itemList->count() - 1);
            d->rowCount = d->itemList->count();
            endInsertRows();
        }
    }
}

/*!
    \property QGalleryItemListModel::autoUpdateCursorPosition
*/

bool QGalleryItemListModel::autoUpdateCursorPosition() const
{
    return d_func()->updateCursorPosition;
}

void QGalleryItemListModel::setAutoUpdateCursorPosition(bool enabled)
{
    d_func()->updateCursorPosition = enabled;
}

/*!

*/

QHash<int, QString> QGalleryItemListModel::roleProperties(int column) const
{
    return d_func()->roleProperties.value(column);
}

/*!

*/

void QGalleryItemListModel::setRoleProperties(const QHash<int, QString> &properties, int column)
{
    Q_D(QGalleryItemListModel);

    if (column >= 0 && column < d->columnCount) {
        d->roleProperties[column] = properties;

        d->updateRoles(column);

        if (d->rowCount > 0)
            emit dataChanged(createIndex(0, column), createIndex(d->rowCount - 1, column));
    }
}

/*!
    \reimp
*/

int QGalleryItemListModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? d_func()->rowCount : 0;
}

/*!
    \reimp
*/

int QGalleryItemListModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? d_func()->columnCount : 0;
}

/*!
*/

void QGalleryItemListModel::addColumn(const QHash<int, QString> &properties)
{
    Q_D(QGalleryItemListModel);

    beginInsertColumns(QModelIndex(), d->columnCount, d->columnCount);

    d->roleProperties.append(properties);
    d->itemFlags.append(Qt::ItemFlags());
    d->columnOffsets.append(d->columnOffsets.isEmpty() ? 0 : d->columnOffsets.last());

    d->columnCount += 1;

    d->updateRoles(d->columnCount);

    endInsertColumns();
}

/*!
*/

void QGalleryItemListModel::addColumn(const QString &property, int role)
{
    QHash<int, QString> properties;

    properties.insert(role, property);

    addColumn(properties);
}

/*!
*/

void QGalleryItemListModel::insertColumn(int index, const QHash<int, QString> &properties)
{
    Q_D(QGalleryItemListModel);

    beginInsertColumns(QModelIndex(), index, index);

    d->roleProperties.insert(index, properties);
    d->itemFlags.insert(index, Qt::ItemFlags());
    d->columnOffsets.insert(index, index == 0 ? 0 : d->columnOffsets.at(index - 1));

    d->columnCount += 1;

    d->updateRoles(index);

    endInsertColumns();
}

/*!

*/

void QGalleryItemListModel::insertColumn(int index, const QString &property, int role)
{
    QHash<int, QString> properties;

    properties.insert(role, property);

    insertColumn(index, properties);
}

/*!

*/

void QGalleryItemListModel::removeColumn(int index)
{
    Q_D(QGalleryItemListModel);

    beginRemoveColumns(QModelIndex(), index, index);

    const int offset = index != 0 ? d->columnOffsets.at(index - 1) : 0;
    const int count = d->columnOffsets.at(index);
    const int difference = count - offset;

    d->roleProperties.remove(index);
    d->itemFlags.remove(index);
    d->columnOffsets.remove(index);
    d->roleKeys.remove(offset, difference);

    d->columnCount -= 1;

    for (int i = index; i < d->columnCount; ++i)
        d->columnOffsets[i] -= difference;

    endRemoveColumns();
}

/*!
    \reimp
*/

QModelIndex QGalleryItemListModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_D(const QGalleryItemListModel);

    if (!parent.isValid()
            && row >= 0 && row < d->rowCount
            && column >= 0 && column < d->columnCount) {

        if (d->updateCursorPosition  && row < d->rowCount - 1) {
            const int position = d->itemList->cursorPosition();
            const int pageSize = d->itemList->minimumPagedItems();

            if (row - 16 < position && position > 0)
                d->itemList->setCursorPosition(qMax(0, row - 16));
            else if (row + 16 > position + pageSize)
                d->itemList->setCursorPosition(qMax(0, row + 16 - pageSize));
        }

        return createIndex(row, column);
    } else {
        return QModelIndex();
    }
}

/*!
    \reimp
*/

QModelIndex QGalleryItemListModel::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return QModelIndex();
}

/*!
    \reimp
*/

QVariant QGalleryItemListModel::data(const QModelIndex &index, int role) const
{
    Q_D(const QGalleryItemListModel);

    if (index.isValid()) {
        const int offset = index.column() != 0 ? d->columnOffsets.at(index.column() - 1) : 0;
        const int count = d->columnOffsets.at(index.column());

        for (int i = offset; i < count; i += 2) {
            if (d->roleKeys.at(i) == role)
                return d->itemList->metaData(index.row(), d->roleKeys.at(i + 1));
        }
    }
    return QVariant();
}

/*!
    \reimp
*/

bool QGalleryItemListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_D(QGalleryItemListModel);

    if (index.isValid()) {
        const int offset = index.column() != 0 ? d->columnOffsets.at(index.column() - 1) : 0;
        const int count = d->columnOffsets.at(index.column());

        for (int i = offset; i < count; i += 2) {
            if (d->roleKeys.at(i) == role) {
                const int key = d->roleKeys.at(i + 1);

                if (d->itemList->propertyAttributes(key) & QGalleryProperty::CanWrite) {
                    d->itemList->setMetaData(index.row(), key, value);

                    return true;
                } else {
                    return false;
                }
            }
        }
    }
    return false;
}

/*!
    \reimp
*/

Qt::ItemFlags QGalleryItemListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags;

    if (index.isValid()) {
        flags |= d_func()->itemFlags.at(index.column());

        if (!(d_func()->itemList->status(index.row()) & QGalleryItemList::OutOfRange))
            flags |= Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }

    return flags;
}

#include "moc_qgalleryitemlistmodel.cpp"

QTM_END_NAMESPACE
