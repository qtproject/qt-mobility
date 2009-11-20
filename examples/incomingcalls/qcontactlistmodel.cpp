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

#include "qcontactlistmodel.h"
#include "qcontactlistmodel_p.h"

#include "qcontact.h"
#include "qcontactmanager.h"
#include "qcontactdetails.h"
#include "qcontactrequests.h"
#include "qcontactfilters.h"

/*!
 * Constructs a new QContactListModel which will request data from the given \a manager
 * and cache approximately \a cacheSize contacts.
 *
 * \sa setManager(), setCacheSize()
 */
QContactListModel::QContactListModel(QContactManager* manager, int cacheSize)
        : QAbstractListModel(),
        d(new QContactListModelPrivate)
{
    setCacheSize(cacheSize);
    setManager(manager);
}

/*!
 * Constructs a new copy of the \a other model
 */
QContactListModel::QContactListModel(const QContactListModel& other)
        : QAbstractListModel(), d(other.d)
{
}

/*!
 * Assigns this model to be equal to \a other
 */
QContactListModel& QContactListModel::operator=(const QContactListModel& other)
{
    d = other.d;
    return *this;
}


/*!
 * Cleans up any memory in use by the model
 */
QContactListModel::~QContactListModel()
{
}

/*!
 * Returns a pointer to the manager from which this model requests contact data
 */
QContactManager* QContactListModel::manager() const
{
    return d->m_manager;
}

/*!
 * Sets the manager from which this model requests contact data to \a manager.
 * Any requests made of the old manager will be cancelled and deleted.
 *
 * \sa backendChanged()
 */
void QContactListModel::setManager(QContactManager* manager)
{
    // first, cancel and delete any requests made of the old manager
    QMap<QContactAbstractRequest*, int> updatedRequestCentreRows;
    QList<QContactAbstractRequest*> requests = d->m_requestCentreRows.keys();
    for (int i = 0; i < requests.size(); i++) {
        QContactAbstractRequest* current = requests.at(i);
        if (current->manager() == d->m_manager) {
            current->cancel();
            delete current;
        } else {
            updatedRequestCentreRows.insert(current, d->m_requestCentreRows.value(current));
        }
    }
    d->m_requestCentreRows = updatedRequestCentreRows;

    // secondly, disconnect the signals from the old manager
    if (d->m_manager)
        d->m_manager->disconnect(this);

    // then set up the new manager.
    d->m_manager = manager;
    delete d->m_idRequest;
    d->m_idRequest = new QContactLocalIdFetchRequest;
    connect(d->m_idRequest, SIGNAL(progress(QContactLocalIdFetchRequest*,bool)), this, SLOT(contactIdFetchRequestProgress(QContactLocalIdFetchRequest*,bool)));
    d->m_idRequest->setManager(manager);
    if (manager) {
        connect(manager, SIGNAL(contactsAdded(QList<QContactLocalId>)), this, SLOT(backendChanged()));
        connect(manager, SIGNAL(contactsChanged(QList<QContactLocalId>)), this, SLOT(backendChanged()));
        connect(manager, SIGNAL(contactsRemoved(QList<QContactLocalId>)), this, SLOT(backendChanged()));
    }

    // and kick of a request for the ids.
    backendChanged();
}

/*!
 * Returns the number of contacts that this model will cache
 */
int QContactListModel::cacheSize() const
{
    return (d->m_halfCacheSize * 2);
}

/*!
 * Sets the number of contacts that this model will cache to be approximately \a size contacts.
 * The exact size of the cache will be the next higher size which is divisible by 4, or
 * \a size if \a size is divisible by 4, unless the next higher size would cause integer overflow.
 * Returns true if the cache size was set successfully, and false if a non-positive \a size was
 * specified.
 *
 * \sa cacheSize()
 */
bool QContactListModel::setCacheSize(int size)
{
    // size will be rounded up to nearest where modulo 4 == 0,
    // except where doing so would result in integer overflow
    // (where it will be rounded down)
    if (size > 0) {
        // if the cache size is odd, round up to nearest even then test modulo 4
        // this allows us to cache m_halfCacheSize rows either side of currentRow
        int modulo4 = size % 4;
        if (modulo4 == 0) {
            d->m_halfCacheSize = size / 2;
            d->m_quarterCacheSize = size / 4;
        } else {
            int test = size + (4 - modulo4); // avoid integer overflow.
            d->m_halfCacheSize = (test < 0 ? (size - modulo4) : (size + 4 - modulo4));
            d->m_halfCacheSize = d->m_halfCacheSize / 2;
            d->m_quarterCacheSize = d->m_halfCacheSize / 2;
        }
        return true;
    }

    return false;
}

/*!
 * Returns the policy that the model uses to determine when asynchronous requests should be cleaned up.
 *
 * \sa setRequestPolicy()
 */
QContactListModel::AsynchronousRequestPolicy QContactListModel::requestPolicy() const
{
    return d->m_requestPolicy;
}

/*!
 * Sets the policy that the model uses to determine when to clean up asynchronous requests to \a policy.
 *
 * \sa requestPolicy()
 */
void QContactListModel::setRequestPolicy(QContactListModel::AsynchronousRequestPolicy policy)
{
    d->m_requestPolicy = policy;
}

/*!
 * Returns the definition name of the relevant data detail which is cached by the model
 *
 * \sa setRelevantDetailDefinitionAndFieldNames()
 */
QString QContactListModel::relevantDefinitionName() const
{
    return d->m_relevantDefinitionName;
}

/*!
 * Returns the name of the field of the relevant data detail which is cached by the model
 *
 * \sa setRelevantDetailDefinitionAndFieldNames()
 */
QString QContactListModel::relevantFieldName() const
{
    return d->m_relevantFieldName;
}

/*!
 * Sets the definition name of the relevant detail which is cached by the model to \a definitionName,
 * and the name of the field of such details which is cached to \a fieldName.
 *
 * \sa relevantDefinitionName(), relevantFieldName()
 */
bool QContactListModel::setRelevantDetailDefinitionAndFieldNames(const QString& definitionName, const QString& fieldName)
{
    if (definitionName.isEmpty() || fieldName.isEmpty())
        return false;

    d->m_relevantDefinitionName = definitionName;
    d->m_relevantFieldName = fieldName;
    return true;
}

/*!
 * \reimp
 */
int QContactListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return d->m_rowsToIds.count();
}

/*!
 * \reimp
 */
QVariant QContactListModel::data(const QModelIndex& index, int role) const
{
    if (index.row() == -1)
        return QVariant();

    d->m_currentRow = index.row();
    QContact currentContact = d->m_cache.value(d->m_currentRow);

    // check to see if we need to update our cache
    bool cacheUpdateRequired = d->m_cache.isEmpty();
    if ((d->m_halfCacheSize * 2) < d->m_rowsToIds.count()) {
        // we cannot fit the entire dataset into our cache; calculate window size.
        int maxActiveCacheRow = d->m_currentRow + d->m_quarterCacheSize;
        int minActiveCacheRow = d->m_currentRow - d->m_quarterCacheSize;
        if (maxActiveCacheRow <= d->m_lastCacheCentreRow) {
            cacheUpdateRequired = true;
        } else if (minActiveCacheRow >= d->m_lastCacheCentreRow) {
            cacheUpdateRequired = true;
        }
    }

    if (cacheUpdateRequired) {
        // the current row will be our new cache centre point.
        d->m_lastCacheCentreRow = d->m_currentRow;

        // update our cache - first calculate the new cache window.
        int lowerBound = d->m_lastCacheCentreRow - d->m_halfCacheSize;
        int upperBound = d->m_lastCacheCentreRow + d->m_halfCacheSize;

        // create a list of rows we need to cache
        QList<int> newCacheRows;
        if ((upperBound - lowerBound) >= d->m_rowsToIds.count()) {
            // we can cache the entire dataset.
            newCacheRows = d->m_rowsToIds.keys();
        } else {
            // we can only cache a window of the entire dataset.
            for (int i = lowerBound; i <= upperBound; i++) {
                // wrap-around at top and bottom of cache.
                int rowNumber = i;
                if (i < 0)
                    rowNumber += d->m_rowsToIds.count();
                if (i >= d->m_rowsToIds.count())
                    rowNumber -= d->m_rowsToIds.count();
                newCacheRows.append(rowNumber);
            }
        }

        // clean up any old requests depending on policy
        // please note that this branching is _slow_; might be best to remove it
        // and just always do the default (cancel on cache centrepoint miss)...
        if (d->m_requestPolicy != QContactListModel::NeverCancelPolicy) {
            QList<QContactAbstractRequest*> oldRequests = d->m_requestCentreRows.keys();
            bool cancelRequest = false;

            // we could pull the conditionals outside the loop for better performance...
            for (int i = 0; i < oldRequests.size(); i++) {
                QContactAbstractRequest* current = oldRequests.at(i);
                if (d->m_requestPolicy == QContactListModel::CancelOnCacheUpdatePolicy) {
                    // immediately cancel since update is required.
                    cancelRequest = true;
                } else if (d->m_requestPolicy == QContactListModel::CancelOnCacheMissPolicy) {
                    // slow solution... should probably do bounds checking instead of .contains().
                    if (!newCacheRows.contains(d->m_requestCentreRows.value(current))) {
                        cancelRequest = true;
                    }
                } else {
                    int cacheSize = d->m_halfCacheSize * 2;
                    int requestCentre = d->m_requestCentreRows.value(current);
                    int requestWindowMax = (requestCentre + d->m_halfCacheSize) % cacheSize;
                    int requestWindowMin = (requestCentre - d->m_halfCacheSize) % cacheSize;
                    // slow solution... should probably do bounds checking instead of .contains().
                    if (!newCacheRows.contains(requestWindowMax) && !newCacheRows.contains(requestWindowMin) && !newCacheRows.contains(requestCentre)) {
                        cancelRequest = true;
                    }
                }

                // cancel (and clean up) the request if required by the policy.
                if (cancelRequest) {
                    current->cancel();
                    d->m_requestCentreRows.remove(current);
                    delete current;
                }

                // reset the control variable.
                cancelRequest = false;
            }
        }

        // create "spots" for the cache entries in our cache map
        QList<int> oldCacheRows = d->m_cache.keys();
        foreach (int row, newCacheRows) {
            if (!d->m_cache.contains(row)) {
                QContact temp;
                QContactName loadingName;
                loadingName.setCustomLabel(QString(tr("Loading...")));
                temp.saveDetail(&loadingName);
                d->m_cache.insert(row, temp);
            }
        }

        // remove any out-of-cache-window contacts we have stored
        // and remove from the newCacheRows any rows we already have cached
        foreach (int row, oldCacheRows) {
            if (row < lowerBound || row > upperBound) {
                // don't want to cache this row.
                d->m_cache.remove(row);
            }

            if (newCacheRows.contains(row)) {
                // already have this row in cache.
                newCacheRows.removeOne(row);
            }
        }

        // ensure that the current row's contact is cached; if not create a placeholder.
        if (!d->m_cache.contains(d->m_currentRow)) {
            QContactName loadingName;
            loadingName.setCustomLabel(QString(tr("Loading...")));
            currentContact.saveDetail(&loadingName);
        }

        // now fire off an asynchronous request to update our cache
        QContactFetchRequest* req = new QContactFetchRequest;
        d->m_requestCentreRows.insert(req, d->m_lastCacheCentreRow);
        QContactLocalIdFilter idFil;
        QList<QContactLocalId> newCacheIds;
        for (int i = 0; i < newCacheRows.size(); i++) {
            newCacheIds.append(d->m_rowsToIds.value(newCacheRows.at(i)));
        }
        idFil.setIds(newCacheIds);
        req->setFilter(idFil);
        req->setManager(d->m_manager);
        connect(req, SIGNAL(progress(QContactFetchRequest*, bool)), this, SLOT(contactFetchRequestProgress(QContactFetchRequest*,bool)));
        req->start();
    }

    // now return the data being requested.
    QVariant ret;
    switch (role) {
        case QContactListModel::DisplayLabelRole:
        {
            ret = currentContact.displayLabel();
        }
        break;

        case QContactListModel::IdRole:
        {
            ret = currentContact.id().localId();
        }
        break;

        case QContactListModel::AvatarRole:
        {
            ret = currentContact.detail(QContactAvatar::DefinitionName).value(QContactAvatar::FieldAvatar);
        }
        break;

        case QContactListModel::PresenceRole:
        {
            if (d->m_manager == 0) {
                // the manager has not been initialised.
                break;
            }

            // grab the possible presence values; they should be in order from (unknown) to least present to most present.
            QContactDetailDefinition presenceDef = d->m_manager->detailDefinition(QContactOnlineAccount::DefinitionName);
            QList<QVariant> allowablePresenceValues = presenceDef.fields().value(QContactOnlineAccount::FieldPresence).allowableValues();
            if (presenceDef.isEmpty() || allowablePresenceValues.isEmpty()) {
                // the manager does not support presence details.
                break;
            }

            // calculate the "global presence" of the contact in a naive way.
            int bestPresenceSoFar = 0; // unknown
            QList<QContactDetail> presenceDetails = currentContact.details(QContactOnlineAccount::DefinitionName);
            foreach (const QContactOnlineAccount& pres, presenceDetails) {
                int index = allowablePresenceValues.indexOf(pres.presence());
                if (index > bestPresenceSoFar) {
                    bestPresenceSoFar = index;
                }
            }

            ret = QVariant(allowablePresenceValues.at(bestPresenceSoFar));
        }
        break;

        case QContactListModel::RelevantDataRole:
        {
            ret = currentContact.detail(d->m_relevantDefinitionName).value(d->m_relevantFieldName);
        }
        break;

        default:
        break;
    }

    // return the requested data, or a default-constructed QVariant if not available.
    return ret;
}

/*!
 * \reimp
 */
QVariant QContactListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);

    QVariant ret;
    switch (role) {
        case QContactListModel::DisplayLabelRole:
        {
            ret = QString(tr("Name"));
        }
        break;

        case QContactListModel::IdRole:
        {
            ret = QString(tr("Id"));
        }
        break;

        case QContactListModel::AvatarRole:
        {
            ret = QString(tr("Avatar"));
        }
        break;

        case QContactListModel::PresenceRole:
        {
            ret = QString(tr("Presence"));
        }
        break;

        case QContactListModel::RelevantDataRole:
        {
            // todo: take this as an argument in setRelevant()
            ret = QString(tr("Details"));
        }
        break;

        default:
        break;
    }

    return ret;
}

/*!
 * \reimp
 */
bool QContactListModel::insertRows(int row, int count, const QModelIndex& parent)
{
    beginInsertRows(parent, row, count);

    // insertion code here.

    endInsertRows();

    return false;
}

/*!
 * \reimp
 */
bool QContactListModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, count);

    // removal code here.

    endRemoveRows();

    return false;
}

/*!
 * Returns the row number at which the data of the contact with the given \a contactId is stored, or
 * -1 if no such contact exists in the model
 */
int QContactListModel::contactRow(const QContactLocalId& contactId) const
{
    return d->m_idsToRows.value(contactId, -1);
}

/*!
 * Returns the entire contact which exists in the model at the specified \a index
 */
QContact QContactListModel::contact(const QModelIndex& index) const
{
    if (d->m_manager)
        return d->m_manager->contact(d->m_rowsToIds.value(index.row()));
    return QContact();
}

/*!
 * Processes the progress of the \a request.
 * If the request is still valid, the results are placed in the cache at the required positions.
 * If the cache is updated, the dataChanged() signal is emitted.
 * This implementation ignores the \a appendOnly flag.
 */
void QContactListModel::contactFetchRequestProgress(QContactFetchRequest* request, bool appendOnly)
{
    Q_UNUSED(appendOnly);

    // first, check to make sure that the request is still valid.
    if (d->m_manager != request->manager() || request->status() == QContactAbstractRequest::Cancelled) {
        d->m_requestCentreRows.remove(request);
        delete request;
        return; // ignore these results.
    }

    QMap<int, int> rowMap; // sorted list of rows changed.
    QList<QContact> fetched = request->contacts();
    foreach (const QContact& c, fetched) {
        int fetchedRow = d->m_idsToRows.value(c.id().localId(), -1);

        // see if this row should be cached
        if (!d->m_cache.contains(fetchedRow))
            break; // shouldn't cache this row (or already cached); ignore the contact.

        // we need to cache this contact.
        d->m_cache.insert(fetchedRow, c);
        rowMap.insert(fetchedRow, fetchedRow);
    }

    // check to see if the request status is "finished" - clean up.
    if (request->status() == QContactAbstractRequest::Finished) {
        d->m_requestCentreRows.remove(request);
        delete request;
    }

    // emit data changed for those that have changed.
    QList<int> rows = rowMap.keys();
    while (rows.size() > 0) {
        // we want to emit the dataChanged signal as few times as possible
        // so, we coalesce the changes into lumps of contiguous changes.
        int lowestIndex = rows.at(0);
        int highestIndex = rows.at(0);
        int nbrAccountedFor = 1;
        int nbrRows = rows.size();
        while (nbrAccountedFor < nbrRows) {
            int temp = highestIndex;
            highestIndex = rows.at(nbrAccountedFor);
            if ((highestIndex - temp) > 1) {
                highestIndex = temp;
                break;
            }
            nbrAccountedFor += 1;
        }

        while (nbrAccountedFor > 0) {
            rows.removeFirst();
            nbrAccountedFor -= 1;
        }

        // calculate the indices of the boundaries, and emit the signal.
        QModelIndex lowerBound = QAbstractItemModel::createIndex(lowestIndex, 0);
        QModelIndex upperBound = QAbstractItemModel::createIndex(highestIndex, 0);
        emit dataChanged(lowerBound, upperBound);
    }
}

/*!
 * Processes the results of a contact id fetch request.
 * If the \a appendOnly flag is set, the new data is appended to the existing data
 * and the dataChanged() signal is emitted; otherwise, the model emits the reset() signal
 * once the new data has been loaded.
 */
void QContactListModel::contactIdFetchRequestProgress(QContactLocalIdFetchRequest* request, bool appendOnly)
{
    // first, if it's not append only, we need to rebuild the entire list + cache.
    if (!appendOnly) {
        d->m_cache.clear();
        d->m_rowsToIds.clear();
        d->m_idsToRows.clear();
    }

    // then get the results, calculate the start and end indices, and fill our data structures.
    QList<QContactLocalId> ids = request->ids();
    int startIndex = d->m_idsToRows.count();
    int endIndex = ids.size();
    for (int i = startIndex; i < endIndex; i++) {
        d->m_rowsToIds.insert(i, ids.at(i));
        d->m_idsToRows.insert(ids.at(i), i);
    }

    // and if we need to, emit the reset signals.
    if (!appendOnly)
        reset();
    else
        emit dataChanged(QAbstractItemModel::createIndex(startIndex,0), QAbstractItemModel::createIndex(endIndex,0));
}

/*!
 * Requests data from the new backend.
 */
void QContactListModel::backendChanged()
{
    d->m_idRequest->start();
    d->m_idRequest->waitForFinished();
}
