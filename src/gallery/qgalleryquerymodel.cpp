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
        QObject::connect(&query, SIGNAL(finished()), q_ptr, SIGNAL(finished()));
        QObject::connect(&query, SIGNAL(canceled()), q_ptr, SIGNAL(canceled()));
        QObject::connect(&query, SIGNAL(errorChanged()), q_ptr, SIGNAL(errorChanged()));
        QObject::connect(&query, SIGNAL(error(int,QString)), q_ptr, SIGNAL(error(int,QString)));
        QObject::connect(&query, SIGNAL(galleryChanged()), q_ptr, SIGNAL(galleryChanged()));
        QObject::connect(&query, SIGNAL(sortPropertyNamesChanged()), q_ptr, SIGNAL(sortPropertyNamesChanged()));
        QObject::connect(&query, SIGNAL(autoUpdateChanged()), q_ptr, SIGNAL(autoUpdateChanged()));
        QObject::connect(&query, SIGNAL(offsetChanged()), q_ptr, SIGNAL(offsetChanged()));
        QObject::connect(&query, SIGNAL(limitChanged()), q_ptr, SIGNAL(limitChanged()));
        QObject::connect(&query, SIGNAL(rootTypeChanged()), q_ptr, SIGNAL(rootTypeChanged()));
        QObject::connect(&query, SIGNAL(rootItemChanged()), q_ptr, SIGNAL(rootItemChanged()));
        QObject::connect(&query, SIGNAL(scopeChanged()), q_ptr, SIGNAL(scopeChanged()));
        QObject::connect(&query, SIGNAL(filterChanged()), q_ptr, SIGNAL(filterChanged()));
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

    \ingroup gallery

    \inmodule QtGallery

    \brief The QGalleryQueryModel class provides a model for the results of a
    gallery query.

    The meta-data that should be queried by a QGalleryQueryModel is specified
    by adding columns to the model, each column has a set of \l roleProperties()
    which map item data roles to gallery properties.  After the model query
    has been executed the values of the properties requested for each column can
    be addressed using the roles they were mapped to.

    The \l rootType property identifies the type of gallery item the request
    should return, if the root type has derivative types (i.e. an audio file is
    just a special case of a regular file) these will also be included in the
    result set.

    The \l rootItem property takes the ID of an item the query should only
    return the children of.  Depending on the \l scope of the query this may
    be {AllDescendents}{all descendents} or just the {DirectDescendents}
    {direct descendents} of the root item.

    The results of a query can be further limited by setting a \l filter on the
    model.  The model will evaluate the QGalleryFilter and only include
    items with meta-data matching the expression.

    The order the results are returned in can be specified in the
    sortPropertyNames property which takes an ordered list of property names.
    By default properties are sorted in ascending order, but this can be
    specified explicitly be prefixing the property name with a '+' character
    for ascending order and a '-' character for descending order.

    If the \l autoUpdate property is true when the query is executed it will
    enter an \l {QGalleryAbstractRequest::Idle}{Idle} state on finishing and
    will refresh the queried information if the items matching the query change.
    If the gallery can't provide updates it will instead go immediately to the
    \l {QGalleryAbstractRequest::Finished}{Finished} state. Automatic updates
    can be canceled by calling cancel() on a idle model.

    \sa QGalleryQueryRequest, QDocumentGallery
*/

/*!
    Constructs a new query model.

    The \a parent is passed to QAbstractItemModel.
*/

QGalleryQueryModel::QGalleryQueryModel(QObject *parent)
    : QAbstractItemModel(parent)
    , d_ptr(new QGalleryQueryModelPrivate(0))
{
    d_ptr->setObject(this);
}

/*!
    Constructs a new model which queries items from a \a gallery.

    The \a parent is passed to QAbstractItemModel.
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

    \brief The Gallery a model executes its queries against.
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
    \fn QGalleryQueryModel::galleryChanged()

    Signals that the value of \l gallery has changed.
*/

/*!
    Returns the meta-data properties which a \a column maps to roles.
*/

QHash<int, QString> QGalleryQueryModel::roleProperties(int column) const
{
    return d_func()->roleProperties.value(column);
}

/*!
    Sets the meta-data \a properties which a \a column maps to roles.

    New properties will not be populated until the query is executed.
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
    Adds a column which maps the given \a properties to a query model.

    The column will not be populated until the query is executed.
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
    Adds a column which maps a meta-data \a property to \a role to a query
    model.

    The column will not be populated until the query is executed.
*/

void QGalleryQueryModel::addColumn(const QString &property, int role)
{
    QHash<int, QString> properties;

    properties.insert(role, property);

    addColumn(properties);
}

/*!
    Inserts a column which maps the given \a properties into a query model at
    \a index.

    The column will not be populated until the query is executed.
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
    Inserts a column which maps a meta-data \a property to \a role into a
    query model at \a index.

    The column will not be populated until the query is executed.
*/

void QGalleryQueryModel::insertColumn(int index, const QString &property, int role)
{
    QHash<int, QString> properties;

    properties.insert(role, property);

    insertColumn(index, properties);
}

/*!
    Removes the column at \a index from a query model.
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

    \brief A list of names of meta-data properties the results of a query
    should be sorted on.

    Prefixing a property name with the '+' character indicates it should be sorted
    in ascending order, and a '-' character prefix indicates a descending order.
    If there is no prefix ascending order is assumed.
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
    \fn QGalleryQueryModel::sortPropertyNamesChanged()

    Signals that the value of \l sortPropertyNames has changed.
*/

/*!
    \property QGalleryQueryModel::autoUpdate

    \brief Whether a query should continue to update its result set after the
    initial query succeeded.
*/

bool QGalleryQueryModel::autoUpdate() const
{
    return d_ptr->query.autoUpdate();
}

void QGalleryQueryModel::setAutoUpdate(bool enabled)
{
    d_ptr->query.setAutoUpdate(enabled);
}

/*!
    \fn QGalleryQueryModel::autoUpdateChanged()

    Signals that the value of \l autoUpdate has changed.
*/

/*!
    \property QGalleryQueryModel::offset

    \brief The offset of the first item a query should return.
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
    \fn QGalleryQueryModel::offsetChanged()

    Signals that the value of \l offset has changed.
*/

/*!
    \property QGalleryQueryModel::limit

    \brief The maximum number of items a query should return.
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
    \fn QGalleryQueryModel::limitChanged()

    Signals that the value of \l limit has changed.
*/

/*!
    \property QGalleryQueryModel::rootType

    \brief The root item type the results of a query should be restricted to.
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
    \fn QGalleryQueryModel::rootTypeChanged()

    Signals that the value of \l rootType has changed.
*/

/*!
    \property QGalleryQueryModel::rootItem

    \brief The ID of the item a query should return the descendents of.

    \sa scope()
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
    \fn QGalleryQueryModel::rootItemChanged()

    Signals that the value of \l rootItem has changed.
*/

/*!
    \property QGalleryQueryModel::scope

    \brief Whether a query will return all descendents of its root item or
    just the direct decendents.

    \sa rootItem()
*/

QGalleryQueryRequest::Scope QGalleryQueryModel::scope() const
{
    return d_ptr->query.scope();
}

void QGalleryQueryModel::setScope(QGalleryQueryRequest::Scope scope)
{
    d_ptr->query.setScope(scope);
}

/*!
    \fn QGalleryQueryModel::scopeChanged()

    Signals that the value of \l scope has changed.
*/

/*!
    \property QGalleryQueryModel::filter

    \brief A filter restricting the results of a query.
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
    \fn QGalleryQueryModel::filterChanged()

    Signals that the value of \l filter has changed.
*/

/*!
    Executes a query.
*/

void QGalleryQueryModel::execute()
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
}

/*!
    Cancels a query.
*/

void QGalleryQueryModel::cancel()
{
    d_ptr->query.cancel();
}

/*!
    Clears the results of a query.
*/

void QGalleryQueryModel::clear()
{
    d_ptr->query.clear();
}

/*!
    \property QGalleryQueryModel::error

    \brief The error encountered by an unsuccessful query.
*/

int QGalleryQueryModel::error() const
{
    return d_ptr->query.error();
}

/*!
    \property QGalleryQueryModel::errorString

    \brief A string describing the cause of an \l error in more detail.

    This may be an empty string if more information is not known.
*/

QString QGalleryQueryModel::errorString() const
{
    return d_ptr->query.errorString();
}

/*!
    \fn QGalleryQueryModel::errorChanged()

    Signals that the \l error and \l errorString properties have changed.
*/

/*!
    \fn QGalleryQueryModel::canceled()

    Signals that the query was canceled.
*/

/*!
    \fn QGalleryQueryModel::finished()

    Signals that the query has finished.
*/

/*!
    \property QGalleryQueryModel::state

    \brief The state of a query.
*/

QGalleryAbstractRequest::State QGalleryQueryModel::state() const
{
    return d_ptr->query.state();
}

/*!
    \fn QGalleryQueryModel::stateChanged(QGalleryAbstractRequest::State state)

    Signals that the \a state of the query has changed.
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
            d_ptr->resultSet->fetch(index.row());

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
            d_ptr->resultSet->fetch(index.row());

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
    Returns the ID of the item at \a index.
*/

QVariant QGalleryQueryModel::itemId(const QModelIndex &index) const
{
    if (index.isValid()) {
        if (d_ptr->resultSet->currentIndex() != index.row())
            d_ptr->resultSet->fetch(index.row());

        return d_ptr->resultSet->itemId();
    }
    return QVariant();
}

/*!
    Returns the URL of the item at \a index.
*/

QUrl QGalleryQueryModel::itemUrl(const QModelIndex &index) const
{
    if (index.isValid()) {
        if (d_ptr->resultSet->currentIndex() != index.row())
            d_ptr->resultSet->fetch(index.row());

        return d_ptr->resultSet->itemUrl();
    }
    return QUrl();
}

/*!
    Returns the type of the item at \a index.
*/

QString QGalleryQueryModel::itemType(const QModelIndex &index) const
{
    if (index.isValid()) {
        if (d_ptr->resultSet->currentIndex() != index.row())
            d_ptr->resultSet->fetch(index.row());

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
