#include "qcontactlistmodel.h"
#include "qcontactlistmodel_p.h"

#include "qcontact.h"
#include "qcontactmanager.h"
#include "qcontactdetails.h"
#include "qcontactrequests.h"
#include "qcontactfilters.h"

QContactListModel::QContactListModel(QContactManager* manager, int cacheSize)
        : QAbstractListModel(),
        d(new QContactListModelPrivate)
{
    setCacheSize(cacheSize);
    setManager(manager);
    backendChanged(); // kick off request for ids.
}

QContactListModel::~QContactListModel()
{
    if (d->m_idRequest)
        delete d->m_idRequest;
}

QContactManager* QContactListModel::manager() const
{
    return d->m_manager;
}

void QContactListModel::setManager(QContactManager* manager)
{
    // also need to cancel all requests from the old  manager
    // TODO when we track requests (in a queue or something)

    // set up the new manager.
    d->m_manager = manager;
    delete d->m_idRequest;
    d->m_idRequest = new QContactIdFetchRequest;
    connect(d->m_idRequest, SIGNAL(progress(QContactIdFetchRequest*,bool)), this, SLOT(contactIdFetchRequestProgress(QContactIdFetchRequest*,bool)));
    d->m_idRequest->setManager(manager);
    if (manager) {
        connect(manager, SIGNAL(contactsAdded(QList<QUniqueId>)), this, SLOT(backendChanged()));
        connect(manager, SIGNAL(contactsChanged(QList<QUniqueId>)), this, SLOT(backendChanged()));
        connect(manager, SIGNAL(contactsRemoved(QList<QUniqueId>)), this, SLOT(backendChanged()));
    }
}

int QContactListModel::cacheSize() const
{
    return (d->m_halfCacheSize * 2);
}

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

QString QContactListModel::relevantDefinitionName() const
{
    return d->m_relevantDefinitionName;
}

QString QContactListModel::relevantFieldName() const
{
    return d->m_relevantFieldName;
}

bool QContactListModel::setRelevantDetailDefinitionAndFieldNames(const QString& definitionName, const QString& fieldName)
{
    if (definitionName.isEmpty() || fieldName.isEmpty())
        return false;

    d->m_relevantDefinitionName = definitionName;
    d->m_relevantFieldName = fieldName;
    return true;
}

int QContactListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return d->m_rowsToIds.count();
}

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
                int rowNumber = i;
                if (i < 0)
                    rowNumber += d->m_rowsToIds.count();
                if (i >= d->m_rowsToIds.count())
                    rowNumber -= d->m_rowsToIds.count();
                newCacheRows.append(rowNumber);
            }
        }

        // create "spots" for the cache entries in our cache map
        QList<int> oldCacheRows = d->m_cache.keys();
        foreach (int row, newCacheRows) {
            if (!d->m_cache.contains(row)) {
                d->m_cache.insert(row, QContact());
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
        if (!d->m_cache.contains(d->m_currentRow))
            currentContact.setDisplayLabel(QString(tr("Loading...")));

        // now fire off an asynchronous request to update our cache
        QContactFetchRequest* req = new QContactFetchRequest;
        QContactIdListFilter idFil;
        QList<QUniqueId> newCacheIds;
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
            ret = currentContact.displayLabel().label();
        }
        break;

        case QContactListModel::IdRole:
        {
            ret = currentContact.id();
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

            QContactDetailDefinition presenceDef = d->m_manager->detailDefinition(QContactPresence::DefinitionName);
            QList<QVariant> allowablePresenceValues = presenceDef.fields().value(QContactPresence::FieldPresence).allowableValues;
            if (presenceDef.isEmpty() || allowablePresenceValues.isEmpty()) {
                // the manager does not support presence details.
                break;
            }

            // grab the possible presence values; they should be in order from (unknown) to least present to most present.
            int bestPresenceSoFar = 0; // unknown
            QList<QContactDetail> presenceDetails = currentContact.details(QContactPresence::DefinitionName);
            foreach (const QContactPresence& pres, presenceDetails) {
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

bool QContactListModel::insertRows(int row, int count, const QModelIndex& parent)
{
    beginInsertRows(parent, row, count);

    // insertion code here.

    endInsertRows();

    return false;
}

bool QContactListModel::removeRows(int row, int count, const QModelIndex& parent)
{
    beginRemoveRows(parent, row, count);

    // removal code here.

    endRemoveRows();

    return false;
}

int QContactListModel::contactRow(const QUniqueId& contactId) const
{
    return d->m_idsToRows.value(contactId);
}

QContact QContactListModel::contact(const QModelIndex& index) const
{
    if (d->m_manager)
        return d->m_manager->contact(d->m_rowsToIds.value(index.row()));
    return QContact();
}

void QContactListModel::contactFetchRequestProgress(QContactFetchRequest* request, bool appendOnly)
{
    Q_UNUSED(appendOnly);

    // first, check to make sure that the request is still valid.
    if (d->m_manager != request->manager()) {
        delete request;
        return; // ignore these results.
    }

    QMap<int, int> rowMap; // sorted list of rows changed.
    QList<QContact> fetched = request->contacts();
    foreach (const QContact& c, fetched) {
        int fetchedRow = d->m_idsToRows.value(c.id(), -1);

        // see if this row should be cached
        if (!d->m_cache.contains(fetchedRow))
            break; // shouldn't cache this row (or already cached); ignore the contact.

        // we need to cache this contact.
        d->m_cache.insert(fetchedRow, c);
        rowMap.insert(fetchedRow, fetchedRow);
    }

    // check to see if the request status is "finished" or "cancelled" - if so, delete the request.
    if (request->status() == QContactAbstractRequest::Finished || request->status() == QContactAbstractRequest::Cancelled)
        delete request;

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

void QContactListModel::contactIdFetchRequestProgress(QContactIdFetchRequest* request, bool appendOnly)
{
    // first, if it's not append only, we need to rebuild the entire list + cache.
    if (!appendOnly) {
        d->m_cache.clear();
        d->m_rowsToIds.clear();
        d->m_idsToRows.clear();
    }

    // then get the results, calculate the start and end indices, and fill our data structures.
    QList<QUniqueId> ids = request->ids();
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

void QContactListModel::backendChanged()
{
    d->m_idRequest->start();
    d->m_idRequest->waitForFinished();
}
