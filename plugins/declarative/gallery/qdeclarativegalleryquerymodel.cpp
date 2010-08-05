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

#include "qdeclarativegalleryquerymodel.h"

#include "qdeclarativegalleryfilter.h"

#include <qgalleryresultset.h>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass GalleryQueryModel QDeclarativeGalleryQueryModel

    \inmodule QtGallery

    \brief The GalleryQueryRequest element is used to specify a model containing
    items from a gallery.

    This element is part of the \bold {QtMobility.gallery 1.1} module.

    The properties that should be returned for each item by the query are
    specified in \l properties. In addition all queries return the following
    properties:

    \list
    \o itemId The ID of an item.
    \o itemType The type of an item.
    \endlist

    \qml
    import Qt 4.7
    import QtMobility.gallery 1.1

    Rectangle {
        width: 1024
        height: 768

        GridView {
            anchors.fill: parent
            cellWidth: 128
            cellHeight: 128

            model: GalleryQueryModel {
                gallery: DocumentGallery {}

                rootType: "Image"
                properties: [ "url" ]
                filter: GalleryFilter {
                    property: "fileName";
                    value: "*.jpg";
                    comparator: GalleryFilter.Wildcard
                }
            }

            delegate: Image {
                source: url
            }
        }
    }
    \endqml

    \sa GalleryItem, GalleryType
*/

QDeclarativeGalleryQueryModel::QDeclarativeGalleryQueryModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_resultSet(0)
    , m_complete(false)
{
    connect(&m_request, SIGNAL(succeeded()), this, SIGNAL(succeeded()));
    connect(&m_request, SIGNAL(cancelled()), this, SIGNAL(cancelled()));
    connect(&m_request, SIGNAL(stateChanged(QGalleryAbstractRequest::State)),
            this, SIGNAL(stateChanged()));
    connect(&m_request, SIGNAL(resultChanged()), this, SIGNAL(resultChanged()));
    connect(&m_request, SIGNAL(progressChanged(int,int)), this, SIGNAL(progressChanged()));
    connect(&m_request, SIGNAL(resultChanged()), this, SIGNAL(resultChanged()));
    connect(&m_request, SIGNAL(failed(int)), this, SIGNAL(failed(int)));
    connect(&m_request, SIGNAL(finished(int)), this, SIGNAL(finished(int)));

    connect(&m_request, SIGNAL(resultSetChanged(QGalleryResultSet*)),
            this, SLOT(_q_setResultSet(QGalleryResultSet*)));
}

QDeclarativeGalleryQueryModel::~QDeclarativeGalleryQueryModel()
{
}

void QDeclarativeGalleryQueryModel::classBegin()
{
}

void QDeclarativeGalleryQueryModel::componentComplete()
{
    m_complete = true;

    if (m_filter)
        m_request.setFilter(m_filter->filter());
    m_request.execute();
}

/*!
    \qmlproperty QAbstractGallery GalleryQueryModel::gallery

    This property holds the gallery a query should return results from.
*/

/*!
    \qmlproperty enum GalleryQueryModel::state

    This property holds the state of a query.  It can be one of:

    \list
    \o Inactive The query has finished.
    \o Active The query is currently executing.
    \o Cancelling The query has been cancelled, but has yet reached the
    Inactive state.
    \o Idle The query has finished and is monitoring its result set for
    changes.
    \endlist
*/

/*!
    \qmlproperty enum GalleryQueryModel::result

    The property holds the result of a query. It can be one of:

    \list
    \o NoResult The query is still executing.
    \o Succeeded The query finished successfully.
    \o Cancelled The query was cancelled.
    \o NoGallery No gallery was set on the query.
    \o NotSupported Queries are not supported by the \l gallery.
    \o ConnectionError The query failed due to a connection error.
    \o InvalidItemError The query failed because the value of \l scopeItemId
    is not a valid item ID.
    \o ItemTypeError The query failed because the value of \l itemType is not
    a valid item type.
    \o InvalidPropertyError The query failed because the \l filter refers to an
    invalid property.
    \o PropertyTypeError The query failed because the type of a value in the
    \l filter is incompatible with the property.
    \o UnsupportedFilterTypeError The query failed because the set \l filter
    is not supported by the \l gallery.
    \o UnsupportedFilterOptionError The query failed because an option of a
    \l filter is not supported by the \l gallery.
    \endlist
*/

/*!
    \qmlproperty int GalleryQueryModel::currentProgress

    This property holds the current progress value.
*/

/*!
    \qmlproperty int GalleryQueryModel::maximumProgress

    This property holds the maximum progress value.
*/

/*!
    \qmlproperty QStringList GalleryQueryModel::properties

    This property holds the item properties a query should return values for.
*/

/*!
    \qmlproperty QStringList GalleryQueryModel::sortProperties

    This property holds the properties the results of a query should be sorted
    on.
*/

/*!
    \qmlproperty bool GalleryQueryModel::live

    This property holds whether a query should refresh its results
    automatically.
*/

/*!
    \qmlproperty int GalleryQueryModel::offset

    This property holds the offset of the first item returned by a query.
*/

/*!
    \qmlproperty int GalleryQueryModel::limit

    This property contains the maximum number of items returned by a query.
*/

/*!
    \qmlproperty string GalleryQueryModel::rootType

    This property contains the type of item a query should return.
*/

/*!
    \qmlproperty GalleryFilter GalleryQueryModel::filter

    This property contains criteria to used to filter the results of a query.
*/

/*!
    \qmlproperty variant GalleryQueryModel::rootItem

    This property contains the id of an item that a query should return the
    descendants of.
*/

/*!
    \qmlproperty enum GalleryQueryModel::scope

    The property contains whether a query should count the direct descendants
    of the \l scopeItemId or all descendants.
*/

/*!
    \qmlmethod GalleryQueryModel::reload()

    Re-queries the gallery.
*/

/*!
    \qmlmethod GalleryQueryModel::cancel()

    Cancels an executing query.
*/

/*!
    \qmlmethod GalleryQueryModel::clear()

    Clears the results of a query.
*/

/*!
    \qmlsignal GalleryQueryModel::onSucceeded()

    Signals that a query has finished successfully.
*/

/*!
    \qmlsignal GalleryQueryModel::onCancelled()

    Signals that a query was cancelled.
*/

/*!
    \qmlsignal GalleryQueryModel::onFailed(error)

    Signals that a query failed with the given \a error.
*/

/*!
    \qmlsignal GalleryQueryModel::onFinished(result)

    Signals that a query finished with the given \a result.
*/

void QDeclarativeGalleryQueryModel::reload()
{
    m_request.setFilter(m_filter ? m_filter->filter() : QGalleryFilter());
    m_request.execute();
}


int QDeclarativeGalleryQueryModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? m_rowCount : 0;
}

QVariant QDeclarativeGalleryQueryModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        if (m_resultSet->currentIndex() != index.row())
            m_resultSet->fetch(index.row());

        switch (role) {
        case ItemId:
            return m_resultSet->itemId();
        case ItemType:
            return m_resultSet->itemType();
        default:
            {
                QVariant value = m_resultSet->metaData(role - MetaDataOffset);

                return value.isNull()
                        ? QVariant(m_resultSet->propertyType(role - MetaDataOffset))
                        : value;
            }
        }
    } else {
        return QVariant();
    }
}

bool QDeclarativeGalleryQueryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && (role -= MetaDataOffset) > 0) {
        if (m_resultSet->currentIndex() != index.row() && !m_resultSet->fetch(index.row()))
            return false;

        return m_resultSet->setMetaData(role, value);
    } else {
        return false;
    }

}

QModelIndex QDeclarativeGalleryQueryModel::index(int row, int column, const QModelIndex &parent) const
{
    return !parent.isValid() && row >= 0 && row < m_rowCount && column == 0
            ? createIndex(row, column)
            : QModelIndex();
}

void QDeclarativeGalleryQueryModel::_q_setResultSet(QGalleryResultSet *resultSet)
{
    m_resultSet = resultSet;

    if (m_resultSet) {
        QHash<int, QByteArray> roleNames;

        QStringList propertyNames = m_request.propertyNames();

        typedef QStringList::const_iterator iterator;
        for (iterator it = propertyNames.constBegin(), end = propertyNames.constEnd();
                it != end;
                ++it)
            roleNames.insert(m_resultSet->propertyKey(*it) + MetaDataOffset, it->toLatin1());

        roleNames.insert(ItemId, QByteArray("itemId"));
        roleNames.insert(ItemType, QByteArray("itemType"));

        setRoleNames(roleNames);

        connect(m_resultSet, SIGNAL(itemsInserted(int,int)),
                this, SLOT(_q_itemsInserted(int,int)));
        connect(m_resultSet, SIGNAL(itemsRemoved(int,int)),
                this, SLOT(_q_itemsRemoved(int,int)));
        connect(m_resultSet, SIGNAL(itemsMoved(int,int,int)),
                this, SLOT(_q_itemsMoved(int,int,int)));
        connect(m_resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(_q_itemsChanged(int,int)));

        m_rowCount = m_resultSet->itemCount();
    } else {
        m_rowCount = 0;
    }

    reset();
}

void QDeclarativeGalleryQueryModel::_q_itemsInserted(int index, int count)
{
    beginInsertRows(QModelIndex(), index, index + count - 1);
    m_rowCount += count;
    endInsertRows();
}

void QDeclarativeGalleryQueryModel::_q_itemsRemoved(int index, int count)
{
    beginRemoveRows(QModelIndex(), index, index + count - 1);
    m_rowCount -= count;
    endRemoveRows();
}

void QDeclarativeGalleryQueryModel::_q_itemsMoved(int from, int to, int count)
{
    beginMoveRows(QModelIndex(), from, from + count - 1, QModelIndex(), to);
    endMoveRows();
}

void QDeclarativeGalleryQueryModel::_q_itemsChanged(int index, int count)
{
    emit dataChanged(createIndex(index, 0), createIndex(index + count - 1, 0));
}

#include "moc_qdeclarativegalleryquerymodel.cpp"

QTM_END_NAMESPACE
