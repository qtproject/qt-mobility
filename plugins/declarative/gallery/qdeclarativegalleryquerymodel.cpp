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

#include <QtDeclarative/qdeclarativeinfo.h>

QTM_BEGIN_NAMESPACE

QDeclarativeGalleryQueryModel::QDeclarativeGalleryQueryModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_resultSet(0)
    , m_status(Null)
    , m_rowCount(0)
    , m_updateStatus(Incomplete)
{
    connect(&m_request, SIGNAL(statusChanged(QGalleryAbstractRequest::Status)),
            this, SLOT(_q_statusChanged()));
    connect(&m_request, SIGNAL(progressChanged(int,int)), this, SIGNAL(progressChanged()));

    connect(&m_request, SIGNAL(resultSetChanged(QGalleryResultSet*)),
            this, SLOT(_q_setResultSet(QGalleryResultSet*)));
}

QDeclarativeGalleryQueryModel::~QDeclarativeGalleryQueryModel()
{
}

void QDeclarativeGalleryQueryModel::componentComplete()
{
    m_updateStatus = NoUpdate;

    if (m_filter) {
        connect(m_filter.data(), SIGNAL(filterChanged()), this, SLOT(deferredExecute()));

        m_request.setFilter(m_filter.data()->filter());
    }
    m_request.execute();
}

qreal QDeclarativeGalleryQueryModel::progress() const
{
    const int max = m_request.maximumProgress();

    return max > 0 ? qreal(m_request.currentProgress()) / max : qreal(0.0);
}

void QDeclarativeGalleryQueryModel::setPropertyNames(const QStringList &names)
{
    if (m_updateStatus == Incomplete) {
        m_request.setPropertyNames(names);

        emit propertyNamesChanged();
    }
}

void QDeclarativeGalleryQueryModel::setSortPropertyNames(const QStringList &names)
{
    if (m_request.sortPropertyNames() != names) {
        m_request.setSortPropertyNames(names);

        deferredExecute();

        emit sortPropertyNamesChanged();
    }
}

void QDeclarativeGalleryQueryModel::setAutoUpdate(bool enabled)
{
    if (m_request.autoUpdate() != enabled) {
        m_request.setAutoUpdate(enabled);

        if (enabled)
            deferredExecute();
        else if (m_status == Idle)
            m_request.cancel();

        emit autoUpdateChanged();
    }
}

void QDeclarativeGalleryQueryModel::setScope(Scope scope)
{
    if (m_request.scope() != QGalleryQueryRequest::Scope(scope)) {
        m_request.setScope(QGalleryQueryRequest::Scope(scope));

        deferredExecute();

        emit scopeChanged();
    }
}

void QDeclarativeGalleryQueryModel::setRootItem(const QVariant &itemId)
{
    if (m_request.rootItem() != itemId) {
        m_request.setRootItem(itemId);

        deferredExecute();

        emit rootItemChanged();
    }
}

void QDeclarativeGalleryQueryModel::setFilter(QDeclarativeGalleryFilterBase *filter)
{
    if (m_filter)
        disconnect(m_filter.data(), SIGNAL(filterChanged()), this, SLOT(deferredExecute()));

    m_filter = filter;

    if (m_filter)
        connect(m_filter.data(), SIGNAL(filterChanged()), this, SLOT(deferredExecute()));

    deferredExecute();

    emit filterChanged();
}

void QDeclarativeGalleryQueryModel::setOffset(int offset)
{
    if (m_request.offset() != offset) {
        m_request.setOffset(offset);

        deferredExecute();

        emit offsetChanged();
    }
}

void QDeclarativeGalleryQueryModel::setLimit(int limit)
{
    if (m_request.limit() != limit) {
        m_request.setLimit(limit);

        deferredExecute();

        emit limitChanged();
    }
}

void QDeclarativeGalleryQueryModel::reload()
{
    if (m_updateStatus == PendingUpdate)
        m_updateStatus = CancelledUpdate;

    m_request.setFilter(m_filter ? m_filter.data()->filter() : QGalleryFilter());

    m_request.execute();
}

void QDeclarativeGalleryQueryModel::cancel()
{
    if (m_updateStatus == PendingUpdate)
        m_updateStatus = CancelledUpdate;

    m_request.cancel();
}

void QDeclarativeGalleryQueryModel::clear()
{
    if (m_updateStatus == PendingUpdate)
        m_updateStatus = CancelledUpdate;

    m_request.clear();
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
            return itemType(m_resultSet->itemType());
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

bool QDeclarativeGalleryQueryModel::setData(
        const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && (role -= MetaDataOffset) >= 0) {
        if (m_resultSet->currentIndex() != index.row() && !m_resultSet->fetch(index.row()))
            return false;

        return m_resultSet->setMetaData(role, value);
    } else {
        return false;
    }

}

QModelIndex QDeclarativeGalleryQueryModel::index(
        int row, int column, const QModelIndex &parent) const
{
    return !parent.isValid() && row >= 0 && row < m_rowCount && column == 0
            ? createIndex(row, column)
            : QModelIndex();
}

QScriptValue QDeclarativeGalleryQueryModel::get(const QScriptValue &index) const
{
    QScriptEngine *scriptEngine = index.engine();

    if (!scriptEngine)
       return QScriptValue();

    const int i = index.toInt32();

    if (i < 0 || i >= m_rowCount || (i != m_resultSet->currentIndex() && !m_resultSet->fetch(i)))
       return scriptEngine->undefinedValue();

    QScriptValue object = scriptEngine->newObject();

    object.setProperty(
            QLatin1String("itemId"), qScriptValueFromValue(scriptEngine, m_resultSet->itemId()));
    object.setProperty(
            QLatin1String("itemUrl"), qScriptValueFromValue(scriptEngine, m_resultSet->itemUrl()));

    typedef QVector<QPair<int, QString> >::const_iterator iterator;
    for (iterator it = m_propertyNames.constBegin(), end = m_propertyNames.constEnd();
            it != end;
            ++it) {
        QVariant value = m_resultSet->metaData(it->first);

        if (value.isNull())
            value = QVariant(m_resultSet->propertyType(it->first));

        object.setProperty(it->second, qScriptValueFromValue(scriptEngine, value));
    }

    return object;
}

QVariant QDeclarativeGalleryQueryModel::property(int index, const QString &property) const
{
    if (index < 0
            || index >= m_rowCount
            || (m_resultSet->currentIndex() != index && !m_resultSet->fetch(index))) {
        return QVariant();
    }

    if (property == QLatin1String("itemId")) {
        return m_resultSet->itemId();
    } else if (property == QLatin1String("itemType")) {
        return itemType(m_resultSet->itemType());
    } else {
        const int propertyKey = m_resultSet->propertyKey(property);

        const QVariant value = m_resultSet->metaData(propertyKey);

        return value.isNull()
                ? QVariant(m_resultSet->propertyType(propertyKey))
                : value;
    }
}

void QDeclarativeGalleryQueryModel::set(int index, const QScriptValue &values)
{
    if (index < 0
            || index >= m_rowCount
            || (m_resultSet->currentIndex() != index && !m_resultSet->fetch(index))) {
        return;
    }

    QScriptValueIterator it(values);
    while (it.hasNext()) {
        it.next();
        m_resultSet->setMetaData(m_resultSet->propertyKey(it.name()), it.value().toVariant());
    }
}

void QDeclarativeGalleryQueryModel::setProperty(
        int index, const QString &property, const QVariant &value)
{
    if (index < 0
            || index >= m_rowCount
            || (m_resultSet->currentIndex() != index && !m_resultSet->fetch(index))) {
        return;
    }

    m_resultSet->setMetaData(m_resultSet->propertyKey(property), value);
}



void QDeclarativeGalleryQueryModel::deferredExecute()
{
    if (m_updateStatus == NoUpdate) {
        m_updateStatus = PendingUpdate;

        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    } else if (m_updateStatus == CancelledUpdate) {
        m_updateStatus = PendingUpdate;
    }
}

bool QDeclarativeGalleryQueryModel::event(QEvent *event)
{
    if (event->type() == QEvent::UpdateRequest) {
        UpdateStatus status = m_updateStatus;
        m_updateStatus = NoUpdate;

        if (status == PendingUpdate) {
            m_request.setFilter(m_filter ? m_filter.data()->filter() : QGalleryFilter());
            m_request.execute();
        }

        return true;
    } else {
        return QAbstractListModel::event(event);
    }
}

void QDeclarativeGalleryQueryModel::_q_statusChanged()
{
    m_status = Status(m_request.status());

    if (m_status == Error) {
        const QString message = m_request.errorString();

        if (!message.isEmpty()) {
            qmlInfo(this) << message;
        } else {
            switch (m_request.error()) {
            case QDocumentGallery::ConnectionError:
                qmlInfo(this) << tr("An error was encountered connecting to the document gallery");
                break;
            case QDocumentGallery::ItemTypeError:
                qmlInfo(this) << (m_request.rootType().isEmpty()
                        ? tr("DocumentGallery.InvalidType is not a supported item type")
                        : tr("DocumentGallery.%1 is not a supported item type")
                                .arg(m_request.rootType()));
                break;
            case QDocumentGallery::ItemIdError:
                qmlInfo(this) << tr("The value of rootItem is not a valid item ID");
                break;
            case QDocumentGallery::FilterError:
                qmlInfo(this) << tr("The value of filter is unsupported");
            default:
                break;
            }
        }
        emit statusChanged();
    } else if (m_status == Idle && !m_request.autoUpdate()) {
        m_request.cancel();
    } else {
        emit statusChanged();
    }
}

void QDeclarativeGalleryQueryModel::_q_setResultSet(QGalleryResultSet *resultSet)
{
    if (m_rowCount > 0) {
        beginRemoveRows(QModelIndex(), 0, m_rowCount - 1);
        m_rowCount = 0;
        m_resultSet = resultSet;
        endRemoveRows();
    } else {
        m_resultSet = resultSet;
    }

    if (m_resultSet) {
        QHash<int, QByteArray> roleNames;
        m_propertyNames.clear();

        QStringList propertyNames = m_request.propertyNames();

        typedef QStringList::const_iterator iterator;
        for (iterator it = propertyNames.constBegin(), end = propertyNames.constEnd();
                it != end;
                ++it) {
            const int key = m_resultSet->propertyKey(*it);

            if (key >= 0) {
                roleNames.insert(key + MetaDataOffset, it->toLatin1());
                m_propertyNames.append(qMakePair(key, *it));
            }
        }
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

        const int rowCount = m_resultSet->itemCount();
        if (rowCount > 0) {
            beginInsertRows(QModelIndex(), 0, rowCount - 1);
            m_rowCount = rowCount;
            endInsertRows();
        }
    }

    emit countChanged();
}

void QDeclarativeGalleryQueryModel::_q_itemsInserted(int index, int count)
{
    beginInsertRows(QModelIndex(), index, index + count - 1);
    m_rowCount += count;
    endInsertRows();

    emit countChanged();
}

void QDeclarativeGalleryQueryModel::_q_itemsRemoved(int index, int count)
{
    beginRemoveRows(QModelIndex(), index, index + count - 1);
    m_rowCount -= count;
    endRemoveRows();

    emit countChanged();
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

/*!
    \qmlclass DocumentGalleryModel QDeclarativeDocumentGalleryModel

    \inmodule QtGallery

    \brief The GalleryQueryRequest element is used to specify a model
    containing items from the document gallery.

    \ingroup qml-gallery

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

            model: DocumentGalleryModel {
                rootType: DocumentGallery.Image
                properties: [ "url" ]
                filter: GalleryWildcardFilter {
                    property: "fileName";
                    value: "*.jpg";
                }
            }

            delegate: Image {
                source: url
                width: 128
                height: 128
            }
        }
    }
    \endqml

    \sa DocumentGalleryItem, DocumentGalleryType
*/

QDeclarativeDocumentGalleryModel::QDeclarativeDocumentGalleryModel(QObject *parent)
    : QDeclarativeGalleryQueryModel(parent)
{
    m_request.setRootType(QDocumentGallery::File);
}

QDeclarativeDocumentGalleryModel::~QDeclarativeDocumentGalleryModel()
{
}

void QDeclarativeDocumentGalleryModel::classBegin()
{
    m_request.setGallery(QDeclarativeDocumentGallery::gallery(this));
}

/*!
    \qmlproperty enum DocumentGalleryModel::status

    This property holds the status of a query.  It can be one of:

    \list
    \o Null No query parameters have been specified.
    \o Active Items matching the query parameters are being fetched from the
    gallery.
    \o Finished The query has finished
    \o Idle The query is finished and will be automatically updated as new
    items become available.
    \o Cancelling The query was cancelled but hasn't yet reached the
    cancelled status.
    \o Cancelled The query was cancelled.
    \o Error Information about a type could not be retrieved due to an error.
    \endlist
*/

/*!
    \qmlproperty real DocumentGalleryModel::progress

    This property holds the current progress of the request, from 0.0 (started)
    to 1.0 (finished).
*/

/*!
    \qmlproperty QStringList DocumentGalleryModel::properties

    This property holds the item properties a query should return values for.
*/

/*!
    \qmlproperty QStringList DocumentGalleryModel::sortProperties

    This property holds the properties the results of a query should be sorted
    on.
*/

/*!
    \qmlproperty bool DocumentGalleryModel::autoUpdate

    This property holds whether a query should refresh its results
    automatically.
*/

/*!
    \qmlproperty int DocumentGalleryModel::offset

    This property holds the offset of the first item returned by a query.
*/

/*!
    \qmlproperty int DocumentGalleryModel::limit

    This property contains the maximum number of items returned by a query.
*/

/*!
    \qmlproperty enum DocumentGalleryModel::rootType

    This property contains the type of item a query should return.
    It can be one of:

    \list
    \o DocumentGallery.InvalidType
    \o DocumentGallery.File
    \o DocumentGallery.Folder
    \o DocumentGallery.Document
    \o DocumentGallery.Text
    \o DocumentGallery.Audio
    \o DocumentGallery.Image
    \o DocumentGallery.Video
    \o DocumentGallery.Playlist
    \o DocumentGallery.Artist
    \o DocumentGallery.AlbumArtist
    \o DocumentGallery.Album
    \o DocumentGallery.AudioGenre
    \o DocumentGallery.PhotoAlbum
    \endlist

    The default value is DocumentGallery.File
*/

QDeclarativeDocumentGallery::ItemType QDeclarativeDocumentGalleryModel::rootType() const
{
    return QDeclarativeDocumentGallery::itemTypeFromString(m_request.rootType());
}

void QDeclarativeDocumentGalleryModel::setRootType(QDeclarativeDocumentGallery::ItemType itemType)
{
    if (m_updateStatus == Incomplete) {
        m_request.setRootType(QDeclarativeDocumentGallery::toString(itemType));

        emit rootTypeChanged();
    }
}

/*!
    \qmlproperty GalleryFilter DocumentGalleryModel::filter

    This property contains criteria to used to filter the results of a query.
*/

/*!
    \qmlproperty variant DocumentGalleryModel::rootItem

    This property contains the id of an item that a query should return the
    descendants of.
*/

/*!
    \qmlproperty enum DocumentGalleryModel::scope

    The property contains whether a query should count the direct descendants
    of the \l rootItem or all descendants.
*/

/*!
    \qmlmethod DocumentGalleryModel::reload()

    Re-queries the gallery.
*/

/*!
    \qmlmethod DocumentGalleryModel::cancel()

    Cancels an executing query.
*/

/*!
    \qmlmethod DocumentGalleryModel::clear()

    Clears the results of a query.
*/

/*!
    \qmlsignal DocumentGalleryModel::onSucceeded()

    Signals that a query has finished successfully.
*/

/*!
    \qmlsignal DocumentGalleryModel::onCancelled()

    Signals that a query was cancelled.
*/

/*!
    \qmlsignal DocumentGalleryModel::onFailed(error)

    Signals that a query failed with the given \a error.
*/

/*!
    \qmlsignal DocumentGalleryModel::onFinished(result)

    Signals that a query finished with the given \a result.
*/

/*!
    \qmlproperty int DocumentGalleryModel::count

    This property holds the number of results returned by a query.
*/

/*!
    \qmlmethod DocumentGalleryModel::get(int index)

    Returns the result at \a index in a query model.

    \code
    query.get(0).title
    \endcode
*/

/*!
    \qmlmethod DocumentGalleryModel::property(int index, string property)

    Returns the value of \a property from the result at \a index.

    \code
    query.getProperty(0, "title")
    \endcode
*/

/*!
    \qmlmethod DocumentGalleryModel::set(int index, jsobject dict)

    Changes the item at \a index in the list model with the values in \a dict.
    Properties not appearing in \a dict are left unchanged.
*/

/*!
    \qmlmethod DocumentGalleryModel::setProperty(int index, string property, variant value)

    Changes the \a property of the result at \a index in a model to \a value.

    \code
        model.setProperty(0, "rating", 4)
    \endcode
*/

QVariant QDeclarativeDocumentGalleryModel::itemType(const QString &type) const
{
    return QVariant::fromValue(QDeclarativeDocumentGallery::itemTypeFromString(type));
}

#include "moc_qdeclarativegalleryquerymodel.cpp"

QTM_END_NAMESPACE
