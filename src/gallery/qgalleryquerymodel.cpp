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

#include "qgalleryquerymodel.h"

#include "qgalleryresultset.h"
#include "qgalleryqueryrequest.h"

#include <QtCore/qstringlist.h>
#include <QtCore/qpointer.h>

QTM_BEGIN_NAMESPACE

class QGalleryQueryModelPrivate
{
public:
    typedef QHash<int, QString> RoleProperties;

    QGalleryQueryModelPrivate(QAbstractGallery *gallery)
        : q_ptr(0)
        , resultSet(0)
        , columnCount(0)
        , rowCount(0)
        , query(gallery)
    {
    }

    void setObject(QGalleryQueryModel *object)
    {
        q_ptr = object;

        QObject::connect(&query, SIGNAL(resultSetChanged(QGalleryResultSet*)),
                q_ptr, SLOT(_q_resultSetChanged(QGalleryResultSet*)));
        QObject::connect(&query, SIGNAL(stateChanged(QGalleryAbstractRequest::State)),
                q_ptr, SIGNAL(stateChanged(QGalleryAbstractRequest::State)));
        QObject::connect(&query, SIGNAL(resultChanged()), q_ptr, SIGNAL(resultChanged()));
        QObject::connect(&query, SIGNAL(finished(int)), q_ptr, SIGNAL(finished(int)));
        QObject::connect(&query, SIGNAL(succeeded()), q_ptr, SIGNAL(succeeded()));
        QObject::connect(&query, SIGNAL(cancelled()), q_ptr, SIGNAL(cancelled()));
        QObject::connect(&query, SIGNAL(failed(int)), q_ptr, SIGNAL(failed(int)));
    }

    void updateRoles(int column);
    void updateRoles();
    void _q_resultSetChanged(QGalleryResultSet *resultSet);
    void _q_itemsInserted(int index, int count);
    void _q_itemsRemoved(int index, int count);
    void _q_itemsMoved(int from, int to, int count);
    void _q_metaDataChanged(int index, int count, const QList<int> &keys);

    QGalleryQueryModel *q_ptr;
    QGalleryResultSet *resultSet;
    int columnCount;
    int rowCount;
    QGalleryQueryRequest query;
    QVector<RoleProperties> roleProperties;
    QVector<int> roleKeys;
    QVector<int> columnOffsets;
    QVector<Qt::ItemFlags> itemFlags;
    QVector<QHash<int, QVariant> > headerData;
};

void QGalleryQueryModelPrivate::updateRoles(int column)
{
    if (resultSet) {
        int offset = column != 0 ? columnOffsets.at(column - 1) : 0;

        const RoleProperties &properties = roleProperties.at(column);

        itemFlags[column] = Qt::ItemFlags();

        typedef RoleProperties::const_iterator iterator;
        for (iterator it = properties.begin(), end = properties.end(); it != end; ++it) {
            const int key = resultSet->propertyKey(it.value());

            if (key > -1) {
                roleKeys.insert(offset, it.key());
                roleKeys.insert(offset + 1, key);

                offset += 2;

                if (it.key() == Qt::DisplayRole && !properties.contains(Qt::EditRole) &&
                        (resultSet->propertyAttributes(key) & QGalleryProperty::CanWrite)) {
                    roleKeys.insert(offset, Qt::EditRole);
                    roleKeys.insert(offset + 1, key);

                    offset += 2;
                }

                itemFlags[column] |= Qt::ItemIsEnabled | Qt::ItemIsSelectable;

                if ((it.key() == Qt::DisplayRole || it.key() == Qt::EditRole)
                        && (resultSet->propertyAttributes(key) & QGalleryProperty::CanWrite)) {
                    itemFlags[column] |= Qt::ItemIsEditable;
                }
            }
        }

        const int difference = column != 0 ? offset - columnOffsets.at(column - 1) : offset;
        for (int i = column + 1; i < columnCount; ++i)
            columnOffsets[i] += difference;

        columnOffsets[column] = offset;
    }
}

void QGalleryQueryModelPrivate::updateRoles()
{
    roleKeys.clear();

    if (resultSet) {
        int offset = 0;

        for (int column = 0; column < columnCount; ++column) {
            const RoleProperties &properties = roleProperties.at(column);

            itemFlags[column] = Qt::ItemFlags();

            typedef RoleProperties::const_iterator iterator;
            for (iterator it = properties.begin(), end = properties.end(); it != end; ++it) {
                const int key = resultSet->propertyKey(it.value());

                if (key > -1) {
                    roleKeys.append(it.key());
                    roleKeys.append(key);

                    offset += 2;

                    if (it.key() == Qt::DisplayRole && !properties.contains(Qt::EditRole) &&
                            (resultSet->propertyAttributes(key) & QGalleryProperty::CanWrite)) {
                        roleKeys.append(Qt::EditRole);
                        roleKeys.append(key);

                        offset += 2;
                    }

                    itemFlags[column] |= Qt::ItemIsEnabled | Qt::ItemIsSelectable;

                    if ((it.key() == Qt::DisplayRole || it.key() == Qt::EditRole)
                            && (resultSet->propertyAttributes(key) & QGalleryProperty::CanWrite)) {
                        itemFlags[column] |= Qt::ItemIsEditable;
                    }
                }
            }
            columnOffsets[column] = offset;
        }
    }
}

void QGalleryQueryModelPrivate::_q_resultSetChanged(QGalleryResultSet *set)
{
    if (rowCount > 0) {
        q_ptr->beginRemoveRows(QModelIndex(), 0, rowCount - 1);
        rowCount = 0;
        q_ptr->endRemoveRows();
    }

    resultSet = set;

    if (resultSet) {
        QObject::connect(
                resultSet, SIGNAL(itemsInserted(int,int)), q_ptr, SLOT(_q_itemsInserted(int,int)));
        QObject::connect(
                resultSet, SIGNAL(itemsRemoved(int,int)), q_ptr, SLOT(_q_itemsRemoved(int,int)));
        QObject::connect(
                resultSet, SIGNAL(itemsMoved(int,int,int)),
                q_ptr, SLOT(_q_itemsMoved(int,int,int)));
        QObject::connect(
                resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)),
                q_ptr, SLOT(_q_metaDataChanged(int,int,QList<int>)));

        const int count = resultSet->itemCount();
        if (count > 0) {
            q_ptr->beginInsertRows(QModelIndex(), 0, count - 1);
            rowCount = count;
            q_ptr->endInsertRows();
        }
    }
}

void QGalleryQueryModelPrivate::_q_itemsInserted(int index, int count)
{
    q_ptr->beginInsertRows(QModelIndex(), index, index + count - 1);
    rowCount = resultSet->itemCount();
    q_ptr->endInsertRows();
}

void QGalleryQueryModelPrivate::_q_itemsRemoved(int index, int count)
{
    q_ptr->beginRemoveRows(QModelIndex(), index, index + count -1);
    rowCount = resultSet->itemCount();
    q_ptr->endRemoveRows();
}

void QGalleryQueryModelPrivate::_q_itemsMoved(int from, int to, int count)
{
    q_ptr->beginMoveRows(QModelIndex(), from, from + count - 1, QModelIndex(), to);
    q_ptr->endMoveRows();
}

void QGalleryQueryModelPrivate::_q_metaDataChanged(int index, int count, const QList<int> &keys)
{
    for (int i = 0, column = 0; i < roleKeys.count(); i += 2) {
        if (i == columnOffsets.at(column))
            column += 1;

        if (keys.contains(roleKeys.at(i + 1))) {
            const int start = column;

            column += 1;

            for (i = columnOffsets.at(start); i < roleKeys.count(); i += 2) {
                if (i == columnOffsets.at(column))
                    break;

                if (keys.contains(roleKeys.at(i + 1))) {
                    i = columnOffsets.at(column) - 2;

                    column += 1;
                }
            }

            emit q_ptr->dataChanged(
                    q_ptr->createIndex(index, start),
                    q_ptr->createIndex(index + count - 1, column - 1));

            column += 1;
        }
    }
}

/*!
    \class QGalleryQueryModel
*/

/*!
*/

QGalleryQueryModel::QGalleryQueryModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d_ptr(new QGalleryQueryModelPrivate(0))
{
    d_ptr->setObject(this);
}

/*!
*/

QGalleryQueryModel::QGalleryQueryModel(QAbstractGallery *gallery, QObject *parent)
    : QAbstractItemModel(parent)
    , d_ptr(new QGalleryQueryModelPrivate(gallery))
{
    d_ptr->setObject(this);
}

/*!
*/

QGalleryQueryModel::~QGalleryQueryModel()
{
}

/*!
    \property QGalleryQueryModel::gallery
*/

QAbstractGallery *QGalleryQueryModel::gallery() const
{
    return d_ptr->query.gallery();
}

void QGalleryQueryModel::setGallery(QAbstractGallery *gallery)
{
    d_ptr->query.setGallery(gallery);
}

/*!

*/

QHash<int, QString> QGalleryQueryModel::roleProperties(int column) const
{
    return d_func()->roleProperties.value(column);
}

/*!

*/

void QGalleryQueryModel::setRoleProperties(int column, const QHash<int, QString> &properties)
{
    Q_D(QGalleryQueryModel);

    if (column >= 0 && column < d->columnCount) {
        d->roleProperties[column] = properties;

        d->updateRoles(column);

        if (d->rowCount > 0)
            emit dataChanged(createIndex(0, column), createIndex(d->rowCount - 1, column));
    }
}

/*!

*/

void QGalleryQueryModel::addColumn(const QHash<int, QString> &properties)
{
    Q_D(QGalleryQueryModel);

    beginInsertColumns(QModelIndex(), d->columnCount, d->columnCount);

    d->roleProperties.append(properties);
    d->itemFlags.append(Qt::ItemFlags());
    d->columnOffsets.append(d->columnOffsets.isEmpty() ? 0 : d->columnOffsets.last());
    d->headerData.append(QHash<int, QVariant>());

    d->columnCount += 1;

    d->updateRoles(d->columnCount - 1);

    endInsertColumns();
}

/*!

*/

void QGalleryQueryModel::addColumn(const QString &property, int role)
{
    QHash<int, QString> properties;

    properties.insert(role, property);

    addColumn(properties);
}

/*!

*/

void QGalleryQueryModel::insertColumn(int index, const QHash<int, QString> &properties)
{
    Q_D(QGalleryQueryModel);

    beginInsertColumns(QModelIndex(), index, index);

    d->roleProperties.insert(index, properties);
    d->itemFlags.insert(index, Qt::ItemFlags());
    d->columnOffsets.insert(index, index < d->columnCount ? d->columnOffsets.at(index) : 0);
    d->headerData.insert(index, QHash<int, QVariant>());

    d->columnCount += 1;

    d->updateRoles(index);

    endInsertColumns();
}

/*!

*/

void QGalleryQueryModel::insertColumn(int index, const QString &property, int role)
{
    QHash<int, QString> properties;

    properties.insert(role, property);

    insertColumn(index, properties);
}

/*!

*/

void QGalleryQueryModel::removeColumn(int index)
{
    Q_D(QGalleryQueryModel);

    beginRemoveColumns(QModelIndex(), index, index);

    const int offset = index != 0 ? d->columnOffsets.at(index - 1) : 0;
    const int count = d->columnOffsets.at(index);
    const int difference = count - offset;

    d->roleProperties.remove(index);
    d->itemFlags.remove(index);
    d->columnOffsets.remove(index);
    d->roleKeys.remove(offset, difference);
    d->headerData.remove(index);

    d->columnCount -= 1;

    for (int i = index; i < d->columnCount; ++i)
        d->columnOffsets[i] -= difference;

    endRemoveColumns();
}

/*!
    \property QGalleryQueryModel::sortPropertyNames
*/

QStringList QGalleryQueryModel::sortPropertyNames() const
{
    return d_ptr->query.sortPropertyNames();
}

void QGalleryQueryModel::setSortPropertyNames(const QStringList &names)
{
    d_ptr->query.setSortPropertyNames(names);
}

/*!
    \property QGalleryQueryModel::live
*/

bool QGalleryQueryModel::isLive() const
{
    return d_ptr->query.isLive();
}

void QGalleryQueryModel::setLive(bool live)
{
    d_ptr->query.setLive(live);
}

/*!
    \property QGalleryQueryModel::offset
*/

int QGalleryQueryModel::offset() const
{
    return d_ptr->query.offset();
}

void QGalleryQueryModel::setOffset(int offset)
{
    d_ptr->query.setOffset(offset);
}

/*!
    \property QGalleryQueryModel::limit
*/

int QGalleryQueryModel::limit() const
{
    return d_ptr->query.limit();
}

void QGalleryQueryModel::setLimit(int limit)
{
    d_ptr->query.setLimit(limit);
}

/*!
    \property QGalleryQueryModel::rootType
*/

QString QGalleryQueryModel::rootType() const
{
    return d_ptr->query.rootType();
}

void QGalleryQueryModel::setRootType(const QString &itemType)
{
    d_ptr->query.setRootType(itemType);
}

/*!
    \property QGalleryQueryModel::rootItem
*/

QVariant QGalleryQueryModel::rootItem() const
{
    return d_ptr->query.rootItem();
}

void QGalleryQueryModel::setRootItem(const QVariant &itemId)
{
    d_ptr->query.setRootItem(itemId);
}

/*!
    \property QGalleryQueryModel::scope
*/

QGalleryAbstractRequest::Scope QGalleryQueryModel::scope() const
{
    return d_ptr->query.scope();
}

void QGalleryQueryModel::setScope(QGalleryAbstractRequest::Scope scope)
{
    d_ptr->query.setScope(scope);
}

/*!
    \property QGalleryQueryModel::filter
*/

QGalleryFilter QGalleryQueryModel::filter() const
{
    return d_ptr->query.filter();
}

void QGalleryQueryModel::setFilter(const QGalleryFilter &filter)
{
    d_ptr->query.setFilter(filter);
}

/*!

*/

bool QGalleryQueryModel::execute()
{
    QStringList propertyNames;

    typedef QVector<QGalleryQueryModelPrivate::RoleProperties>::const_iterator iterator;
    for (iterator it = d_ptr->roleProperties.constBegin(), end = d_ptr->roleProperties.constEnd();
            it != end;
            ++it) {
        propertyNames += it->values();
    }

    propertyNames.removeDuplicates();

    d_ptr->query.setPropertyNames(propertyNames);

    d_ptr->query.execute();

    d_ptr->updateRoles();

    return d_ptr->query.result() == QGalleryAbstractRequest::Succeeded
            || d_ptr->query.result() == QGalleryAbstractRequest::NoResult;
}

/*!

*/

void QGalleryQueryModel::cancel()
{
    d_ptr->query.cancel();
}

/*!

*/

void QGalleryQueryModel::clear()
{
    d_ptr->query.clear();
}

/*!
    \property QGalleryQueryModel::result
*/

int QGalleryQueryModel::result() const
{
    return d_ptr->query.result();
}


/*!
    \fn QGalleryQueryModel::resultChanged()
*/

/*!
    \fn QGalleryQueryModel::succeeded()
*/

/*!
    \fn QGalleryQueryModel::cancelled()
*/

/*!
    \fn QGalleryQueryModel::failed(int result)
*/

/*!
    \fn QGalleryQueryModel::finished(int result)
*/

/*!
    \property QGalleryQueryModel::state
*/

QGalleryAbstractRequest::State QGalleryQueryModel::state() const
{
    return d_ptr->query.state();
}

/*!
    \fn QGalleryQueryModel::stateChanged()
*/


/*!
    \reimp
*/

QModelIndex QGalleryQueryModel::index(int row, int column, const QModelIndex &parent) const
{
    return !parent.isValid()
            && row >= 0
            && row < d_ptr->rowCount
            && column >= 0
            && column < d_ptr->columnCount
            ? createIndex(row, column)
            : QModelIndex();
}

/*!
    \reimp
*/

QModelIndex QGalleryQueryModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);

    return QModelIndex();
}

/*!
    \reimp
*/

int QGalleryQueryModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? d_ptr->rowCount : 0;
}

/*!
    \reimp
*/

int QGalleryQueryModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? d_ptr->columnCount : 0;
}

/*!
    \reimp
*/

QVariant QGalleryQueryModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        if (d_ptr->resultSet->currentIndex() != index.row())
            d_ptr->resultSet->seek(index.row(), false);

        const int offset = index.column() != 0 ? d_ptr->columnOffsets.at(index.column() - 1) : 0;
        const int count = d_ptr->columnOffsets.at(index.column());

        for (int i = offset; i < count; i += 2) {
            if (d_ptr->roleKeys.at(i) == role)
                return d_ptr->resultSet->metaData(d_ptr->roleKeys.at(i + 1));
        }
    }
    return QVariant();
}

/*!
    \reimp
*/

bool QGalleryQueryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid()) {
        if (d_ptr->resultSet->currentIndex() != index.row())
            d_ptr->resultSet->seek(index.row(), false);

        const int offset = index.column() != 0 ? d_ptr->columnOffsets.at(index.column() - 1) : 0;
        const int count = d_ptr->columnOffsets.at(index.column());

        for (int i = offset; i < count; i += 2) {
            if (d_ptr->roleKeys.at(i) == role)
                return d_ptr->resultSet->setMetaData(d_ptr->roleKeys.at(i + 1), value);
        }
    }
    return false;
}

/*!

*/

QVariant QGalleryQueryModel::itemId(const QModelIndex &index) const
{
    if (index.isValid()) {
        if (d_ptr->resultSet->currentIndex() != index.row())
            d_ptr->resultSet->seek(index.row(), false);

        return d_ptr->resultSet->itemId();
    }
    return QVariant();
}

/*!

*/

QUrl QGalleryQueryModel::itemUrl(const QModelIndex &index) const
{
    if (index.isValid()) {
        if (d_ptr->resultSet->currentIndex() != index.row())
            d_ptr->resultSet->seek(index.row(), false);

        return d_ptr->resultSet->itemUrl();
    }
    return QUrl();
}

/*!

*/

QString QGalleryQueryModel::itemType(const QModelIndex &index) const
{
    if (index.isValid()) {
        if (d_ptr->resultSet->currentIndex() != index.row())
            d_ptr->resultSet->seek(index.row(), false);

        return d_ptr->resultSet->itemType();
    }
    return QString();
}

/*!
    \reimp
*/

QVariant QGalleryQueryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::EditRole)
        role = Qt::DisplayRole;

    return orientation == Qt::Horizontal
            ? d_func()->headerData.value(section).value(role)
            : QVariant();
}

/*!
    \reimp
*/

bool QGalleryQueryModel::setHeaderData(
        int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation == Qt::Horizontal && section >= 0 && section < d_ptr->columnCount) {
        if (role == Qt::EditRole)
            role = Qt::DisplayRole;

        d_ptr->headerData[section].insert(role, value);

        emit headerDataChanged(orientation, section, section);

        return true;
    } else {
        return false;
    }
}

/*!
    \reimp
*/

Qt::ItemFlags QGalleryQueryModel::flags(const QModelIndex &index) const
{
    return d_ptr->itemFlags.value(index.column()) ;
}

#include "moc_qgalleryquerymodel.cpp"

QTM_END_NAMESPACE
