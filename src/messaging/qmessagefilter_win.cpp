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
#include "qmessagefilter.h"
#include "qmessagefilter_p.h"
#include "qvariant.h"
#include "qdebug.h"
#include "winhelpers_p.h"
#include "qmessageaccountid_p.h"
#include "qmessagefolderid_p.h"
#include "qmessageid_p.h"
#include "addresshelper_p.h"

// Not sure if this will work on WinCE
#ifndef PR_SMTP_ADDRESS
#define PR_SMTP_ADDRESS 0x39FE001E
#endif

QTM_BEGIN_NAMESPACE

void QDateTimeToFileTime(const QDateTime &dt, FILETIME *ft)
{
    SYSTEMTIME st;
    st.wYear = dt.date().year();
    st.wMonth = dt.date().month();
    st.wDay = dt.date().day();
    st.wHour = dt.time().hour();
    st.wMinute = dt.time().minute();
    st.wSecond = dt.time().second();
    st.wMilliseconds = dt.time().msec();
    SystemTimeToFileTime(&st, ft);
}

void QStringToWCharArray(const QString &str, wchar_t **buffer)
{
    delete *buffer;
    *buffer = new wchar_t[str.length() +1];
    memcpy(*buffer, str.utf16(), str.length() * sizeof(wchar_t));
    (*buffer)[str.length()] = 0;
}


MapiFolderIterator::MapiFolderIterator()
    :_store(0)
{
}

MapiFolderIterator::MapiFolderIterator(MapiStorePtr store, 
                                       MapiFolderPtr root, 
                                       QSet<QMessage::StandardFolder> standardFoldersInclude, 
                                       QSet<QMessage::StandardFolder> standardFoldersExclude,
                                       FolderIdSet parentInclude,
                                       FolderIdSet parentExclude,
                                       FolderIdSet ancestorInclude,
                                       FolderIdSet ancestorExclude)
    :_store(store), 
    _standardFoldersInclude(standardFoldersInclude), 
    _standardFoldersExclude(standardFoldersExclude),
    _parentInclude(parentInclude),
    _parentExclude(parentExclude),
    _ancestorInclude(ancestorInclude),
    _ancestorExclude(ancestorExclude)
{
    QList<MapiFolderPtr> unprocessed;
    if (root && root->isValid())
        unprocessed.append(root);
    while (_store && _store->isValid() && !unprocessed.isEmpty()) {
        MapiFolderPtr parent(unprocessed.takeLast());
        if (parent->isValid()) {
            if (!_store->session()->equal(parent->entryId(), root->entryId()))
                _folders.append(parent);
            QMessageManager::Error ignored(QMessageManager::NoError);
            while (true) {
                MapiFolderPtr folder(parent->nextSubFolder(&ignored));
                if (ignored != QMessageManager::NoError) {
                    qWarning() << "MapiFolderIterator:: Error getting next subfolder";
                    break;
                }
                if (folder && folder->isValid()) {
                    unprocessed.append(folder);
                } else {
                    if (folder) {
                        qWarning() << "MaiFolderIterator:: Invalid subfolder:" << folder->name();
                    }
                    break; // No more subfolders in the current folder
                }
            }
        } else {
            qWarning() << "Invalid folder:" << parent->name();
        }
    }
}

static bool sFolderMatches(MapiFolderPtr folder,
                           QSet<QMessage::StandardFolder> standardFoldersInclude, 
                           QSet<QMessage::StandardFolder> standardFoldersExclude,
                           FolderIdSet parentInclude,
                           FolderIdSet parentExclude,
                           FolderIdSet ancestorInclude,
                           FolderIdSet ancestorExclude)
{
    if (!folder || !folder->isValid()) {
        if (folder) {
            qWarning() << "MaiFolderIterator::next() Invalid subfolder:" << folder->name();
        }
        return false;
    }
    if (!standardFoldersInclude.isEmpty() || !standardFoldersExclude.isEmpty()) {
        QMessage::StandardFolder folderType(folder->standardFolder());
        if (!(standardFoldersInclude.isEmpty() || standardFoldersInclude.contains(folderType))
            || standardFoldersExclude.contains(folderType))
            return false;
    }

    if (!parentInclude.isEmpty() || !parentExclude.isEmpty()) {
        QMessageFolderId parentId(folder->parentId());
#ifndef QSTRING_FOLDER_ID
        if (!(parentInclude.isEmpty() || parentInclude.contains(folder->id()))
            || parentExclude.contains(folder->id()))
            return false;
#else
        QString path(folder->accountId().toString() + "/" + QMessageFolder(folder->id()).path());
        if (!(parentInclude.isEmpty() || parentInclude.contains(path))
            || parentExclude.contains(path))
            return false;
#endif
    }

    if (!ancestorInclude.isEmpty() || !ancestorExclude.isEmpty()) {
        FolderIdSet folderAncestors;
        QMessageFolderId id(folder->parentId());
        while (id.isValid()) {
            QMessageFolder f(id);
#ifndef QSTRING_FOLDER_ID
            folderAncestors.insert(id);
#else
            folderAncestors.insert(f.parentAccountId().toString() + "/" + f.path());
#endif
            id = f.parentFolderId();
        }
        FolderIdSet includeIntersection(ancestorInclude);
        includeIntersection.intersect(folderAncestors);
        FolderIdSet excludeIntersection(ancestorExclude);
        excludeIntersection.intersect(folderAncestors);
        if (!(ancestorInclude.isEmpty() || !includeIntersection.isEmpty())
            || !excludeIntersection.isEmpty()) {
            return false;
        }
    }
    return true;
}

MapiFolderPtr MapiFolderIterator::next()
{
    while (_store && _store->isValid() && !_folders.isEmpty()) {
        MapiFolderPtr folder(_folders.takeFirst());
        if (sFolderMatches(folder, 
                            _standardFoldersInclude, 
                            _standardFoldersExclude, 
                            _parentInclude, 
                            _parentExclude, 
                            _ancestorInclude, 
                            _ancestorExclude)) {
            return folder;
        }
    }

    return MapiFolderPtr();
}

MapiStoreIterator::MapiStoreIterator()
{
}

MapiStoreIterator::MapiStoreIterator(QList<MapiStorePtr> stores, QSet<QMessageAccountId> accountsInclude, QSet<QMessageAccountId> accountsExclude)
    :_stores(stores), _accountsInclude(accountsInclude), _accountsExclude(accountsExclude)
{
}

static bool sAccountIdMatches(QMessageAccountId key, 
                              QSet<QMessageAccountId> accountsInclude, 
                              QSet<QMessageAccountId> accountsExclude)
{
    if ((accountsInclude.isEmpty() || accountsInclude.contains(key))
        && (accountsExclude.isEmpty() || !accountsExclude.contains(key))) {
        return true;
    }
    return false;
}

MapiStorePtr MapiStoreIterator::next()
{
    while (!_stores.isEmpty()) {
        MapiStorePtr store(_stores.takeFirst());
#ifdef _WIN32_WCE
        QMessageAccountId key(QMessageAccountIdPrivate::from(store->entryId()));
#else
        QMessageAccountId key(QMessageAccountIdPrivate::from(store->recordKey()));
#endif
        if (sAccountIdMatches(key, _accountsInclude, _accountsExclude))
            return store;
    }

    return MapiStorePtr();
}

QMessageFilter QMessageFilterPrivate::from(QMessageFilterPrivate::Field field, const QVariant &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = field;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFilter QMessageFilterPrivate::from(QMessageFilterPrivate::Field field, const QVariant &value, QMessageDataComparator::RelationComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = field;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = Relation;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFilter QMessageFilterPrivate::from(QMessageFilterPrivate::Field field, const QVariant &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = field;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFilterPrivate* QMessageFilterPrivate::implementation(const QMessageFilter &filter)
{
    return filter.d_ptr;
}

MapiFolderIterator QMessageFilterPrivate::folderIterator(const QMessageFilter &filter, QMessageManager::Error *error, const MapiStorePtr &store)
{
    return MapiFolderIterator(store, 
        store->rootFolder(error),
        filter.d_ptr->_standardFoldersInclude, 
        filter.d_ptr->_standardFoldersExclude,
        filter.d_ptr->_parentInclude, 
        filter.d_ptr->_parentExclude,
        filter.d_ptr->_ancestorInclude, 
        filter.d_ptr->_ancestorExclude);
}

MapiStoreIterator QMessageFilterPrivate::storeIterator(const QMessageFilter &filter, QMessageManager::Error *error, const MapiSessionPtr &session)
{
    return MapiStoreIterator(session->allStores(error), filter.d_ptr->_accountsInclude, filter.d_ptr->_accountsExclude);
}

QList<QMessageFilter> QMessageFilterPrivate::subfilters(const QMessageFilter &filter)
{
    QList<QMessageFilter> result;
    QList<QMessageFilter> queue;
    queue.append(filter);
    while (!queue.isEmpty()) {
        QMessageFilter top(queue.takeFirst());
        if (!top.d_ptr->_complex) {
            result.append(top);
            continue;
        }
        // Complex so must consist of two subfilters or'd together and an empty containerFiltersPart
        queue.append(*top.d_ptr->_left);
        queue.append(*top.d_ptr->_right);
    }
    return result;
}

// Several filters require QMessageManager::queryX to be called to evaluate filter member variables, 
// namely byIds(const QMessageFilter &, ...), byParentAccountId(const QMessageAccountFilter &, ...), 
// byFolderIds(const QMessageFolderFilter &, ...), byAncestorFolderIds(const QMessageFolderFilter &, ...)
QMessageFilter QMessageFilterPrivate::preprocess(QMessageManager::Error *error, MapiSessionPtr session, const QMessageFilter &filter)
{
    QMessageFilter result(filter);
    QMessageFilterPrivate::preprocess(error, session, &result);
    return result;
}

// returns true if filter is modified
bool QMessageFilterPrivate::preprocess(QMessageManager::Error *error, MapiSessionPtr session, QMessageFilter *filter)
{
    if (!filter)
        return false;

    // incl only used if filter->d_ptr->field is == *Filter
    QMessageDataComparator::InclusionComparator incl(static_cast<QMessageDataComparator::InclusionComparator>(filter->d_ptr->_comparatorValue));
    bool inclusion(incl == QMessageDataComparator::Includes);
    QMessageFilter result;
    if (filter->d_ptr->_operator == Not) {
        inclusion = !inclusion;
    }
    if (inclusion) {
        result = ~QMessageFilter();
    }
    if (filter->d_ptr->_field == MessageFilter) {
        if (filter->d_ptr->_messageFilter->isEmpty()) {
            result = ~result;  // match all for include, match none for exclude
        } else {
            QMessageIdList ids(session->queryMessages(error, *filter->d_ptr->_messageFilter));
            result = QMessageFilter::byId(ids, inclusion ? QMessageDataComparator::Includes : QMessageDataComparator::Excludes);
        }
    } else if (filter->d_ptr->_field == AccountFilter) {
        if (filter->d_ptr->_accountFilter->isEmpty()) {
            result = ~result;  // match all for include, match none for exclude
        } else {
            QList<MapiStorePtr> stores(session->filterStores(error, *filter->d_ptr->_accountFilter));
            foreach(MapiStorePtr store, stores) {
                if (inclusion) {
                    result |= QMessageFilter::byParentAccountId(store->id());
                } else {
                    result &= QMessageFilter::byParentAccountId(store->id(), QMessageDataComparator::NotEqual);
                }
            }
        }
    } else if (filter->d_ptr->_field == FolderFilter) {
        if (filter->d_ptr->_folderFilter->isEmpty()) {
            result = ~result;  // match all for include, match none for exclude
        } else {
            QList<MapiFolderPtr> folders(session->filterFolders(error, *filter->d_ptr->_folderFilter));
            foreach(MapiFolderPtr folder, folders) {
                if (inclusion) {
                    result |= QMessageFilter::byParentFolderId(folder->id());
                } else {
                    result &= QMessageFilter::byParentFolderId(folder->id(), QMessageDataComparator::NotEqual);
                }
            }
        }
    } else if (filter->d_ptr->_field == AncestorFilter) {
        QList<MapiFolderPtr> folders(session->filterFolders(error, *filter->d_ptr->_folderFilter));
        foreach(MapiFolderPtr folder, folders) {
            if (inclusion) {
                result |= QMessageFilter::byAncestorFolderIds(folder->id());
            } else {
                result &= QMessageFilter::byAncestorFolderIds(folder->id(), QMessageDataComparator::Excludes);
            }
        }
    } else {
        QMessageFilter *l(filter->d_ptr->_left);
        QMessageFilter *r(filter->d_ptr->_right);
        bool modified(true); //TODO: should default to false but tst_qmessagestorekeys (id list exclusion 3) is failing
        modified |= preprocess(error, session, l);
        modified |= preprocess(error, session, r);

        // It's necessary to recombine bool op filters, because the operands may now have non-empty containerFilter parts,
        // specifically in the case that one of the operands has a *Filter field.
        if (modified) {
            switch (filter->d_ptr->_operator) {
            case And:
                *filter = filter->d_ptr->containerFiltersPart() & (*l & *r);
                break;
            case Nand:
                *filter = filter->d_ptr->containerFiltersPart() &  ~(*l & *r);
                break;
            case Or:
                *filter = filter->d_ptr->containerFiltersPart() &  (*l | *r);
                break;
            case Nor:
                *filter = filter->d_ptr->containerFiltersPart() &  ~(*l | *r);
                break;
            }
        }
        return modified;
    }
    *filter = filter->d_ptr->containerFiltersPart() & result; // Must preserve container filters part
    return true;
}

bool QMessageFilterPrivate::restrictionPermitted(const QMessageFilter &filter)
{
    bool result(filter.d_ptr->_restrictionPermitted);
    if (filter.d_ptr->_left)
        result &= QMessageFilterPrivate::restrictionPermitted(*filter.d_ptr->_left);
    if (filter.d_ptr->_right)
        result &= QMessageFilterPrivate::restrictionPermitted(*filter.d_ptr->_right);
    return result;
}

bool QMessageFilterPrivate::matchesMessageRequired(const QMessageFilter &filter)
{
    bool result(filter.d_ptr->_matchesRequired);
    if (filter.d_ptr->_left)
        result |= QMessageFilterPrivate::matchesMessageRequired(*filter.d_ptr->_left);
    if (filter.d_ptr->_right)
        result |= QMessageFilterPrivate::matchesMessageRequired(*filter.d_ptr->_right);
    return result;
}

bool QMessageFilterPrivate::containsSenderSubfilter(const QMessageFilter &filter)
{
    bool result(filter.d_ptr->_field == QMessageFilterPrivate::Sender);
    if (filter.d_ptr->_left)
        result |= QMessageFilterPrivate::containsSenderSubfilter(*filter.d_ptr->_left);
    if (filter.d_ptr->_right)
        result |= QMessageFilterPrivate::containsSenderSubfilter(*filter.d_ptr->_right);
    return result;
}

bool QMessageFilterPrivate::isNonMatching(const QMessageFilter &filter)
{
    return (filter.d_ptr->containerFiltersAreEmpty() 
        && (filter.d_ptr->_field == QMessageFilterPrivate::None)
        && (filter.d_ptr->_operator == QMessageFilterPrivate::Not));
}


bool QMessageFilterPrivate::matchesMessage(const QMessageFilter &filter, const QMessage &message, MapiStore *store)
{
    if (filter.isEmpty())
        return true;

    QMessageAccountId accountId(message.parentAccountId());
    if (!sAccountIdMatches(accountId, filter.d_ptr->_accountsInclude, filter.d_ptr->_accountsExclude))
        return false;

    QMessageManager::Error ignoredError(QMessageManager::NoError);
#ifdef _WIN32_WCE
    MapiFolderPtr folder = store->openFolder(&ignoredError, QMessageIdPrivate::folderRecordKey(message.id()));
#else
    MapiFolderPtr folder = store->openFolderWithKey(&ignoredError, QMessageIdPrivate::folderRecordKey(message.id()));
#endif
    if (ignoredError != QMessageManager::NoError)
        return false;
    if (!sFolderMatches(folder, 
        filter.d_ptr->_standardFoldersInclude, 
        filter.d_ptr->_standardFoldersExclude,
        filter.d_ptr->_parentInclude, 
        filter.d_ptr->_parentExclude,
        filter.d_ptr->_ancestorInclude, 
        filter.d_ptr->_ancestorExclude))
        return false;
    return matchesMessageSimple(filter, message);
}


// Simple matchesMessage function, filter must have an empty nonContainerFiltersPart
bool QMessageFilterPrivate::matchesMessageSimple(const QMessageFilter &filter, const QMessage &message)
{
    bool negate(false);
    bool result(true);

    if (!filter.d_ptr->_valid) {
        qWarning("matchesMessage: Invalid filter application attempted.");
        return false;
    }

    if (filter.d_ptr->_matchFlags & QMessageDataComparator::MatchFullWord) {
        // TODO: Document Full word matching is not supported on MAPI
        qWarning("matchesMessage: Full word matching not supported on MAPI platforms.");
        return false;
    }

    switch (filter.d_ptr->_operator) {
    case QMessageFilterPrivate::Not: // fall through
    case QMessageFilterPrivate::Nand: // fall through
    case QMessageFilterPrivate::Nor:
        negate = true;
        break;
    default:
        break;
    } //end switch

    switch (filter.d_ptr->_operator) {
    case QMessageFilterPrivate::And: // fall through
    case QMessageFilterPrivate::Nand:
        result = matchesMessageSimple(*filter.d_ptr->_left, message) 
            && matchesMessageSimple(*filter.d_ptr->_right, message);
        break;
    case QMessageFilterPrivate::Nor: // fall through
    case QMessageFilterPrivate::Or:
        result = matchesMessageSimple(*filter.d_ptr->_left, message) 
            || matchesMessageSimple(*filter.d_ptr->_right, message);
        break;
    case QMessageFilterPrivate::Not: // fall through
    case QMessageFilterPrivate::Identity: {
        switch (filter.d_ptr->_field) {
        case QMessageFilterPrivate::None:
            break;
        case QMessageFilterPrivate::Recipients: // fall through
        case QMessageFilterPrivate::Sender: // fall through
        case QMessageFilterPrivate::Subject: {
            QString value(filter.d_ptr->_value.toString());
            QStringList messageStrings;
            QString tmp;
            bool caseSensitive(filter.d_ptr->_matchFlags & QMessageDataComparator::MatchCaseSensitive);
            if (!caseSensitive) {
                value = value.toLower();
            }

            if (filter.d_ptr->_field == QMessageFilterPrivate::Recipients) {
                QMessageAddressList addresses(message.to() + message.cc() + message.bcc());
                foreach(QMessageAddress address, addresses) {
                    tmp = address.addressee();
                    if (!caseSensitive) {
                        tmp = tmp.toLower();
                    }
                    messageStrings.append(tmp);
                }
            } else if (filter.d_ptr->_field == QMessageFilterPrivate::Sender) {
                tmp = message.from().addressee();
                if (!caseSensitive) {
                    tmp = tmp.toLower();
                }
                messageStrings.append(tmp);
            } else { // Subject
                tmp = message.subject();
                if (!caseSensitive) {
                    tmp = tmp.toLower();
                }
                messageStrings.append(tmp);
            }

            if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Relation) {
                qWarning("matchesMessage: Unhandled restriction criteria, comparator type relation");
                break;
            } else if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Equality) {
                if (static_cast<QMessageDataComparator::EqualityComparator>(filter.d_ptr->_comparatorValue) != QMessageDataComparator::Equal) {
                    negate = !negate;
                }
            } else if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Inclusion) {
                if (static_cast<QMessageDataComparator::InclusionComparator>(filter.d_ptr->_comparatorValue) == QMessageDataComparator::Excludes) {
                    negate = !negate;
                }
            } else { // Unknown comparator type
                qWarning("matchesMessage: Unhandled restriction criteria, string comparator type unknown");
                break;
            }

            result = false;
            foreach(QString str, messageStrings) {
                if ((filter.d_ptr->_comparatorType == QMessageFilterPrivate::Equality)
                    && (str == value)) {
                    result = true;
                    break;
                } else if ((filter.d_ptr->_comparatorType == QMessageFilterPrivate::Inclusion)
                    && (str.contains(value))) {
                    result = true;
                    break;
                }
            }
            break;
        }
        case QMessageFilterPrivate::TimeStamp: // fall through
        case QMessageFilterPrivate::ReceptionTimeStamp: {
            QDateTime value(filter.d_ptr->_value.toDateTime());
            QDateTime date;
            if (filter.d_ptr->_field == QMessageFilterPrivate::TimeStamp) {
                date = message.date();
            } else { // ReceptionTimeStamp
                date = message.receivedDate();
            }

            if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Relation) {
                QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(filter.d_ptr->_comparatorValue));
                switch (cmp) {
                case QMessageDataComparator::LessThan:
                    result = (date < value);
                    break;
                case QMessageDataComparator::LessThanEqual:
                    result = (date <= value);
                    break;
                case QMessageDataComparator::GreaterThan:
                    result = (date > value);
                    break;
                case QMessageDataComparator::GreaterThanEqual:
                    result = (date >= value);
                    break;
                }
            } else if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Equality) {
                if (static_cast<QMessageDataComparator::EqualityComparator>(filter.d_ptr->_comparatorValue) != QMessageDataComparator::Equal) {
                    negate = !negate;
                }
                result = (date == value);
            } else { // Inclusion
                qWarning("matchesMessage: Unhandled restriction criteria, timestamp comparator type");
                break;
            }

            break;
        }
        case QMessageFilterPrivate::Priority: {
            QMessage::Priority priority(static_cast<QMessage::Priority>(filter.d_ptr->_value.toInt()));
            if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Equality) {
                if (static_cast<QMessageDataComparator::EqualityComparator>(filter.d_ptr->_comparatorValue) != QMessageDataComparator::Equal) {
                    negate = !negate;
                }
                result = (message.priority() == priority);
            } else { // Not equality
                qWarning("matchesMessage: Unhandled restriction criteria, priority comparator type");
            }
            break;
        }
        case QMessageFilterPrivate::Size: {
            int value(filter.d_ptr->_value.toInt());
            int size(message.size());

            if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Relation) {
                QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(filter.d_ptr->_comparatorValue));
                switch (cmp) {
                case QMessageDataComparator::LessThan:
                    result = (size < value);
                    break;
                case QMessageDataComparator::LessThanEqual:
                    result = (size <= value);
                    break;
                case QMessageDataComparator::GreaterThan:
                    result = (size > value);
                    break;
                case QMessageDataComparator::GreaterThanEqual:
                    result = (size >= value);
                    break;
                }
            } else if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Equality) {
                if (static_cast<QMessageDataComparator::EqualityComparator>(filter.d_ptr->_comparatorValue) != QMessageDataComparator::Equal) {
                    negate = !negate;
                }
                result = (size == value);
            } else { // Inclusion
                qWarning("matchesMessage: Unhandled restriction criteria, size comparator type");
                break;
            }

            break;
        }
        case QMessageFilterPrivate::Status: {
            QMessage::Status value(static_cast<QMessage::Status>(filter.d_ptr->_value.toUInt()));
            QMessage::StatusFlags statusFlags(message.status());

            if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Equality) {
                if (static_cast<QMessageDataComparator::EqualityComparator>(filter.d_ptr->_comparatorValue) != QMessageDataComparator::Equal) {
                    negate = !negate;
                }
                result = (statusFlags == value);
            } else if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Inclusion) {
                if (static_cast<QMessageDataComparator::InclusionComparator>(filter.d_ptr->_comparatorValue) == QMessageDataComparator::Excludes) {
                    negate = !negate;
                }
                result = (statusFlags & value);
            } else {
                qWarning("matchesMessage: Unhandled restriction criteria, status comparator type");
                break;
            }
            break;
        }
        case QMessageFilterPrivate::Id: {
            QStringList strIds(filter.d_ptr->_value.toStringList());
            QMessageId messageId(message.id());
            QMessageIdList ids;
            foreach(QString str, strIds) {
                ids.append(QMessageId(str));
            }
            result = ids.contains(messageId);
            if (filter.d_ptr->_comparatorType != QMessageFilterPrivate::Inclusion) {
                qWarning("matchesMessage: Unhandled restriction criteria, id comparator type");
            }
            if (static_cast<QMessageDataComparator::InclusionComparator>(filter.d_ptr->_comparatorValue) == QMessageDataComparator::Excludes) {
                result = !result;
            }
            break;
        }
        case QMessageFilterPrivate::ParentAccountId: // fall through
        case QMessageFilterPrivate::Type: // fall through
        case QMessageFilterPrivate::ParentFolderId: // fall through
        case QMessageFilterPrivate::AncestorFolderIds: // fall through
            // These should all be satisfied (and toplevel with complements fully distributed hence no negation)
            return true;
            break;
        }
    }
    } //end switch

    if (negate)
        result = !result;
    return result;
}

MapiRestriction::MapiRestriction(const QMessageFilter &aFilter)
    :_notRestriction(0),
     _recipientRestriction(0),
     _keyProps(0),
     _restrictions(0),
     _recordKeys(0),
     _buffer(0),
     _buffer2(0),
     _valid(false),
     _empty(false),
     _left(0),
     _right(0)
{
    QMessageFilter filter;
    QMessageFilterPrivate *d_ptr(QMessageFilterPrivate::implementation(aFilter));

    if (!d_ptr->_valid) {
        qWarning("Invalid filter application ignored.");
        return;
    }
    if (d_ptr->_matchFlags & QMessageDataComparator::MatchFullWord) {
        qWarning("Full word matching not supported on MAPI platforms.");
        return;
    }

    if (!QMessageFilterPrivate::restrictionPermitted(aFilter)) {
        _valid = true;
        _empty = true;
        return;
    }

    if (d_ptr->_field == QMessageFilterPrivate::Sender) {
        QString value(d_ptr->_value.toString());
        switch (d_ptr->_comparatorType) {
        case QMessageFilterPrivate::Equality: {
            filter = QMessageFilterPrivate::bySender(value, static_cast<QMessageDataComparator::EqualityComparator>(d_ptr->_comparatorValue));
            break;
        }
        case QMessageFilterPrivate::Inclusion: {
            filter = QMessageFilterPrivate::bySender(value, static_cast<QMessageDataComparator::InclusionComparator>(d_ptr->_comparatorValue));
            break;
        }
        default: { // Relation
            qWarning("Unhandled restriction criteria");
            return;
        }
        }
        if (d_ptr->_operator == QMessageFilterPrivate::Not) {
            // Can't complement sender restriction
            qWarning("Unhandled restriction criteria");
            return;
        }
        d_ptr = QMessageFilterPrivate::implementation(filter);
    } else {
        filter = aFilter;
    }

    if (d_ptr->_operator != QMessageFilterPrivate::Identity) {
        switch (d_ptr->_operator) {
        case QMessageFilterPrivate::Not: // fall through
        case QMessageFilterPrivate::Nand: // fall through
        case QMessageFilterPrivate::Nor:
            _notRestriction = new SRestriction;
            _notRestriction->rt = RES_NOT;
            _notRestriction->res.resNot.ulReserved = 0;
            _notRestriction->res.resNot.lpRes = &_restriction;
            break;
        default:
            break;
        } //end switch
        switch (d_ptr->_operator) {
        case QMessageFilterPrivate::Not: {
            if (d_ptr->_field == QMessageFilterPrivate::None) {
                _restriction.rt = RES_EXIST;
                _restriction.res.resExist.ulReserved1 = 0;
                _restriction.res.resExist.ulPropTag = PR_ENTRYID; // Should match all, so not this is a non-matching filter
                _restriction.res.resExist.ulReserved2 = 0;
                _valid = true;
                return;
            }
            break;
        }
        case QMessageFilterPrivate::Nand: // fall through
        case QMessageFilterPrivate::And: {
            Q_ASSERT(d_ptr->_left);
            Q_ASSERT(d_ptr->_right);
            if (!d_ptr->_left || !d_ptr->_right)
                return;
            _left = new MapiRestriction(*d_ptr->_left);
            _right = new MapiRestriction(*d_ptr->_right);
            _subRestriction[0] = *_left->sRestriction();
            _subRestriction[1] = *_right->sRestriction();
            _restriction.rt = RES_AND;
            _restriction.res.resAnd.cRes = 2;
            _restriction.res.resAnd.lpRes = &_subRestriction[0];
            _valid = true;
            return;
        }
        case QMessageFilterPrivate::Nor: // fall through
        case QMessageFilterPrivate::Or: {
            Q_ASSERT(d_ptr->_left);
            Q_ASSERT(d_ptr->_right);
            if (!d_ptr->_left || !d_ptr->_right)
                return;
            _left = new MapiRestriction(*d_ptr->_left);
            _right = new MapiRestriction(*d_ptr->_right);
            _subRestriction[0] = *_left->sRestriction();
            _subRestriction[1] = *_right->sRestriction();
            _restriction.rt = RES_OR;
            _restriction.res.resOr.cRes = 2;
            _restriction.res.resOr.lpRes = &_subRestriction[0];
            _valid = true;
            return;
        }
        default:
            Q_ASSERT(false);
            qWarning("Unsupported filter boolean algebra case.");
        } // end switch
    } else { // identity operator
        if (d_ptr->_field == QMessageFilterPrivate::None) {
            _valid = true;
            _empty = true;
            return;
        }
    }

    // Identity or Not filter type

    // Handle Recipients as a special case
#if 0 // This MAPI Restriction is producing incorrect results
    if ((d_ptr->_field == QMessageFilterPrivate::RecipientName)
       || (d_ptr->_field == QMessageFilterPrivate::RecipientAddress)) {

        _restriction.rt = RES_SUBRESTRICTION;
        _restriction.res.resSub.ulSubObject = PR_MESSAGE_RECIPIENTS;
        _restriction.res.resSub.lpRes = _recipientRestriction;
        _recipientRestriction = new SRestriction;
        _recipientRestriction->rt = RES_AND;
        _recipientRestriction->res.resAnd.cRes = 2;
        _recipientRestriction->res.resAnd.lpRes = &_subRestriction[0];
        _restriction.res.resSub.lpRes = _recipientRestriction;

        _subRestriction[0].rt = RES_EXIST;
        _subRestriction[0].res.resExist.ulReserved1 = 0;
        _subRestriction[0].res.resExist.ulReserved2 = 0;
   
        _subRestriction[1].rt = RES_CONTENT;
        _subRestriction[1].res.resContent.lpProp = &_keyProp;
        if (d_ptr->_field == QMessageFilterPrivate::RecipientName) {
            _subRestriction[0].res.resExist.ulPropTag = PR_DISPLAY_NAME;
            _subRestriction[1].res.resContent.ulPropTag = PR_DISPLAY_NAME;
            _keyProp.ulPropTag = PR_DISPLAY_NAME;
        } else { // RecipientsAddress
            _subRestriction[0].res.resExist.ulPropTag = PR_SMTP_ADDRESS;
            _subRestriction[1].res.resContent.ulPropTag = PR_SMTP_ADDRESS; // PR_EMAIL_ADDRESS returns unsatisfactory results
            _keyProp.ulPropTag = PR_SMTP_ADDRESS;
        }

        QStringToWCharArray(d_ptr->_value.toString(), &_buffer); 
        _keyProp.Value.LPSZ = _buffer;

        bool negation(false);
        switch (d_ptr->_comparatorType) {
        case QMessageFilterPrivate::Equality: {
            _subRestriction[1].res.resContent.ulFuzzyLevel = FL_FULLSTRING;
            if (static_cast<QMessageDataComparator::EqualityComparator>(d_ptr->_comparatorValue) != QMessageDataComparator::Equal)
                negation = true;
            break;
        }
        case QMessageFilterPrivate::Inclusion: {
            // There is a flaw for Inclusion/Exclusion here,
            // If the search string includes a '<' so that both name and address are specified
            // FL_PREFIX should be used for the address, and (a non-existent) Fl_SUFFIX for name.
            _subRestriction[1].res.resContent.ulFuzzyLevel = FL_SUBSTRING;
            if (static_cast<QMessageDataComparator::InclusionComparator>(d_ptr->_comparatorValue) == QMessageDataComparator::Excludes)
                negation = true;
            break;
        }
        default: { // Relation
            qWarning("Unhandled restriction criteria");
            return;
        }
        }
        if (negation) {
            complement();
        }

        if ((d_ptr->_matchFlags & QMessageDataComparator::MatchCaseSensitive) == 0) {
            _subRestriction[1].res.resContent.ulFuzzyLevel |= FL_IGNORECASE;
        }

        _valid = true;
        return;
    }
#endif

    // Handle Subject and/or Sender as a special case
    if ((d_ptr->_field == QMessageFilterPrivate::Subject)
        || (d_ptr->_field == QMessageFilterPrivate::SenderName)
        || (d_ptr->_field == QMessageFilterPrivate::SenderAddress)) {

        _restriction.rt = RES_CONTENT;
        _restriction.res.resContent.lpProp = &_keyProp;

        if (d_ptr->_field == QMessageFilterPrivate::Subject) {
            _restriction.res.resContent.ulPropTag = PR_SUBJECT;
            _keyProp.ulPropTag = PR_SUBJECT;
        } else if (d_ptr->_field == QMessageFilterPrivate::SenderName) {
            _restriction.res.resContent.ulPropTag = PR_SENDER_NAME;
            _keyProp.ulPropTag = PR_SENDER_NAME;
        } else { // SenderAddress
            _restriction.res.resContent.ulPropTag = PR_SENDER_EMAIL_ADDRESS;
            _keyProp.ulPropTag = PR_SENDER_EMAIL_ADDRESS;
        }

        QString subj(d_ptr->_value.toString());
        QStringToWCharArray(subj, &_buffer); 
        _keyProp.Value.LPSZ = _buffer;

        bool negation(false);
        switch (d_ptr->_comparatorType) {
        case QMessageFilterPrivate::Equality: {
            _restriction.res.resContent.ulFuzzyLevel = FL_FULLSTRING;
            if (static_cast<QMessageDataComparator::EqualityComparator>(d_ptr->_comparatorValue) != QMessageDataComparator::Equal)
                negation = true;
            break;
        }
        case QMessageFilterPrivate::Inclusion: {
            _restriction.res.resContent.ulFuzzyLevel = FL_SUBSTRING;
            if (static_cast<QMessageDataComparator::InclusionComparator>(d_ptr->_comparatorValue) == QMessageDataComparator::Excludes)
                negation = true;
            break;
        }
        default: { // Relation
            qWarning("Unhandled restriction criteria");
            return;
        }
        }

        if (negation) {
            complement();
        }

        if ((d_ptr->_matchFlags & QMessageDataComparator::MatchCaseSensitive) == 0)
            _restriction.res.resContent.ulFuzzyLevel |= FL_IGNORECASE;

        _valid = true;
        return;
    }

    switch (d_ptr->_comparatorType) {
    case QMessageFilterPrivate::Equality:
    case QMessageFilterPrivate::Relation: {
        _restriction.rt = RES_PROPERTY;
        if (d_ptr->_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(d_ptr->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal)
                _restriction.res.resProperty.relop = RELOP_EQ;
            else
                _restriction.res.resProperty.relop = RELOP_NE;
        } else { // Relation
            QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(d_ptr->_comparatorValue));
            switch (cmp) {
            case QMessageDataComparator::LessThan:
                _restriction.res.resProperty.relop = RELOP_LT;
                break;
            case QMessageDataComparator::LessThanEqual:
                _restriction.res.resProperty.relop = RELOP_LE;
                break;
            case QMessageDataComparator::GreaterThan:
                _restriction.res.resProperty.relop = RELOP_GT;
                break;
            case QMessageDataComparator::GreaterThanEqual:
                _restriction.res.resProperty.relop = RELOP_GE;
                break;
            }
        }
        _restriction.res.resProperty.lpProp = &_keyProp;
        switch (d_ptr->_field) {
        case QMessageFilterPrivate::Size: {
            _restriction.res.resProperty.ulPropTag = PR_MESSAGE_SIZE;
            _keyProp.ulPropTag = PR_MESSAGE_SIZE;
            _keyProp.Value.l = d_ptr->_value.toInt();
            _valid = true;
            break;
        }
        case QMessageFilterPrivate::ReceptionTimeStamp: {
            _restriction.res.resProperty.ulPropTag = PR_MESSAGE_DELIVERY_TIME;
            _keyProp.ulPropTag = PR_MESSAGE_DELIVERY_TIME;
            QDateTime dt(d_ptr->_value.toDateTime());
            QDateTimeToFileTime(dt, &_keyProp.Value.ft);
            _valid = true;
            break;
        }
        case QMessageFilterPrivate::TimeStamp: {
            _restriction.res.resProperty.ulPropTag = PR_CLIENT_SUBMIT_TIME;
            _keyProp.ulPropTag = PR_CLIENT_SUBMIT_TIME;
            QDateTime dt(d_ptr->_value.toDateTime());
            QDateTimeToFileTime(dt, &_keyProp.Value.ft);
            _valid = true;
            break;
        }
        case QMessageFilterPrivate::Priority: {
            _restriction.res.resProperty.ulPropTag = PR_PRIORITY;
            _keyProp.ulPropTag = PR_PRIORITY;
            QMessage::Priority priority(static_cast<QMessage::Priority>(d_ptr->_value.toInt()));
            switch (priority) { // TODO: Double check that priority filtering is working
            case QMessage::HighPriority:
                _keyProp.Value.l = PRIO_URGENT;
                break;
            case QMessage::NormalPriority:
                _keyProp.Value.l = PRIO_NORMAL;
                break;
            case QMessage::LowPriority:
                _keyProp.Value.l = PRIO_NONURGENT;
                break;
            default:
                qWarning("Unknown priority encountered during filter processing");
                return;
            }
            _valid = true;
            break;
        }
        default:
            qWarning("Unhandled restriction criteria");
        }
        break;
    }
    case QMessageFilterPrivate::Inclusion: {
        QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(d_ptr->_comparatorValue));
        if (cmp == QMessageDataComparator::Excludes) {
            complement();
        }
        if (d_ptr->_field == QMessageFilterPrivate::Id) {
            QStringList ids(d_ptr->_value.toStringList());
#ifdef _WIN32_WCE
            _recordKeys = new MapiEntryId[ids.count()];
#else
            _recordKeys = new MapiRecordKey[ids.count()];
#endif
            _keyProps = new SPropValue[ids.count()];
            _restrictions = new SRestriction[ids.count()];

            _restriction.rt = RES_OR;
            _restriction.res.resOr.cRes = ids.count();
            _restriction.res.resOr.lpRes = &_restrictions[0];
            for (int i = 0; i < ids.count(); ++i) {
#ifdef _WIN32_WCE
                _recordKeys[i] = QMessageIdPrivate::entryId(QMessageId(ids[i]));
#else
                _recordKeys[i] = QMessageIdPrivate::messageRecordKey(QMessageId(ids[i]));
#endif
                _keyProps[i].ulPropTag = PR_RECORD_KEY;
                _keyProps[i].Value.bin.cb = _recordKeys[i].count();
                _keyProps[i].Value.bin.lpb = reinterpret_cast<LPBYTE>(const_cast<char*>(_recordKeys[i].data()));
                _restrictions[i].rt = RES_PROPERTY;
                _restrictions[i].res.resProperty.relop = RELOP_EQ;
                _restrictions[i].res.resProperty.ulPropTag = PR_RECORD_KEY;
                _restrictions[i].res.resProperty.lpProp = &_keyProps[i];
            }
            _valid = true;
            return;
        }
        if (d_ptr->_field == QMessageFilterPrivate::Status) {
            _restriction.rt = RES_BITMASK;
            QMessage::Status status(static_cast<QMessage::Status>(d_ptr->_value.toUInt()));
            switch (status) {
            case QMessage::Incoming:
                // Restriction is not possible in this case
                _valid = false;
                return;
            case QMessage::Read:
                _restriction.res.resBitMask.relBMR = BMR_NEZ;
                _restriction.res.resBitMask.ulPropTag = PR_MESSAGE_FLAGS;
                _restriction.res.resBitMask.ulMask = MSGFLAG_READ;
                _valid = true;
                return;
            case QMessage::Removed:
                _restriction.res.resBitMask.relBMR = BMR_NEZ;
                _restriction.res.resBitMask.ulPropTag = PR_MSG_STATUS;
                _restriction.res.resBitMask.ulMask = MSGSTATUS_DELMARKED; // Untested
                _valid = true;
                return;
            case QMessage::HasAttachments:
#ifdef _WIN32_WCE
                _restriction.rt = RES_EXIST;
                _restriction.res.resExist.ulReserved1 = 0;
                _restriction.res.resExist.ulPropTag = PR_HASATTACH;
                _restriction.res.resExist.ulReserved2 = 0;
#else
                _restriction.res.resBitMask.relBMR = BMR_NEZ;
                _restriction.res.resBitMask.ulPropTag = PR_MESSAGE_FLAGS;
                _restriction.res.resBitMask.ulMask = MSGFLAG_HASATTACH;
#endif
                _valid = true;
                return;
            default:
                qWarning("Unimplemented status filter"); // Has attachments not done
                return;
            }
        }
        qWarning("Unhandled restriction criteria");
        break;
    }
    }
}

void MapiRestriction::complement()
{
    if (_notRestriction) { // double negative
        delete _notRestriction;
        _notRestriction = 0;
    } else {
        _notRestriction = new SRestriction;
        _notRestriction->rt = RES_NOT;
        _notRestriction->res.resNot.ulReserved = 0;
        _notRestriction->res.resNot.lpRes = &_restriction;
    }

}

MapiRestriction::~MapiRestriction()
{
    delete _notRestriction;
    _notRestriction = 0;
    delete _recipientRestriction;
    _recipientRestriction = 0;
    delete [] _recordKeys;
    _recordKeys = 0;
    delete [] _keyProps;
    _keyProps = 0;
    delete [] _restrictions;
    _restrictions = 0;
    delete _left;
    _left = 0;
    delete _right;
    _right = 0;
    delete _buffer;
    _buffer = 0;
    delete _buffer2;
    _buffer2 = 0;
}

SRestriction *MapiRestriction::sRestriction()
{
    if (_notRestriction)
        return _notRestriction;
    return &_restriction;
}

QMessageFilterPrivate::QMessageFilterPrivate(QMessageFilter *messageFilter)
    :q_ptr(messageFilter),
     _field(None),
     _comparatorType(Equality),
     _comparatorValue(QMessageDataComparator::Equal),
     _operator(Identity),
     _left(0),
     _right(0),
     _valid(true),
#ifdef _WIN32_WCE
     _matchesRequired(true),
     _restrictionPermitted(false),
#else
     _matchesRequired(false),
     _restrictionPermitted(true),
#endif
    _messageFilter(0),
    _accountFilter(0),
    _folderFilter(0),
     _complex(false)
{
}

QMessageFilterPrivate::~QMessageFilterPrivate()
{
    delete _left;
    _left = 0;
    delete _right;
    _right = 0;
    delete _messageFilter;
    _messageFilter = 0;
    delete _accountFilter;
    _accountFilter = 0;
    delete _folderFilter;
    _folderFilter = 0;
}

bool QMessageFilterPrivate::containerFiltersAreEmpty()
{
    return (_standardFoldersInclude.isEmpty() 
            && _standardFoldersExclude.isEmpty() 
            && _accountsInclude.isEmpty() 
            && _accountsExclude.isEmpty()
            && _parentInclude.isEmpty() 
            && _parentExclude.isEmpty()
            && _ancestorInclude.isEmpty() 
            && _ancestorExclude.isEmpty());
}

bool QMessageFilterPrivate::nonContainerFiltersAreEmpty()
{
    return ((_field == QMessageFilterPrivate::None) 
        && (_operator == QMessageFilterPrivate::Identity));
}

QMessageFilter QMessageFilterPrivate::containerFiltersPart()
{
    QMessageFilter result;
    result.d_ptr->_standardFoldersInclude = _standardFoldersInclude;
    result.d_ptr->_standardFoldersExclude = _standardFoldersExclude;
    result.d_ptr->_accountsInclude = _accountsInclude;
    result.d_ptr->_accountsExclude = _accountsExclude;
    result.d_ptr->_parentInclude = _parentInclude;
    result.d_ptr->_parentExclude = _parentExclude;
    result.d_ptr->_ancestorInclude = _ancestorInclude;
    result.d_ptr->_ancestorExclude = _ancestorExclude;
    return result;
}

QMessageFilter QMessageFilterPrivate::nonContainerFiltersPart()
{
    QMessageFilter result;
    result.d_ptr->_matchFlags = _matchFlags;
    result.d_ptr->_field = _field;
    result.d_ptr->_value = _value;
    result.d_ptr->_comparatorType = _comparatorType;
    result.d_ptr->_comparatorValue = _comparatorValue;
    result.d_ptr->_operator = _operator;
    if (_left)
        result.d_ptr->_left = new QMessageFilter(*_left);
    if (_right)
        result.d_ptr->_right = new QMessageFilter(*_right);
    result.d_ptr->_matchesRequired = _matchesRequired;
    result.d_ptr->_restrictionPermitted = _restrictionPermitted;
    if (_messageFilter)
        result.d_ptr->_messageFilter = new QMessageFilter(*_messageFilter);
    if (_accountFilter)
        result.d_ptr->_accountFilter = new QMessageAccountFilter(*_accountFilter);
    if (_folderFilter)
        result.d_ptr->_folderFilter = new QMessageFolderFilter(*_folderFilter);
    result.d_ptr->_valid = _valid;
    result.d_ptr->_complex = _complex;
    return result;
}

QMessageFilter::QMessageFilter()
    :d_ptr(new QMessageFilterPrivate(this))
{
}

QMessageFilter::QMessageFilter(const QMessageFilter &other)
    :d_ptr(new QMessageFilterPrivate(this))
{
    this->operator=(other);
}

QMessageFilter::~QMessageFilter()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageFilter& QMessageFilter::operator=(const QMessageFilter& other)
{
    if (&other == this)
        return *this;
    delete d_ptr->_left;
    d_ptr->_left = 0;
    delete d_ptr->_right;
    d_ptr->_right = 0;
    d_ptr->_matchFlags = other.d_ptr->_matchFlags;
    d_ptr->_field = other.d_ptr->_field;
    d_ptr->_value = other.d_ptr->_value;
    d_ptr->_comparatorType = other.d_ptr->_comparatorType;
    d_ptr->_comparatorValue = other.d_ptr->_comparatorValue;
    d_ptr->_operator = other.d_ptr->_operator;
    d_ptr->_valid = other.d_ptr->_valid;
    d_ptr->_standardFoldersInclude = other.d_ptr->_standardFoldersInclude;
    d_ptr->_standardFoldersExclude = other.d_ptr->_standardFoldersExclude;
    d_ptr->_accountsInclude = other.d_ptr->_accountsInclude;
    d_ptr->_accountsExclude = other.d_ptr->_accountsExclude;
    d_ptr->_parentInclude = other.d_ptr->_parentInclude;
    d_ptr->_parentExclude = other.d_ptr->_parentExclude;
    d_ptr->_ancestorInclude = other.d_ptr->_ancestorInclude;
    d_ptr->_ancestorExclude = other.d_ptr->_ancestorExclude;
    d_ptr->_complex = other.d_ptr->_complex;
    d_ptr->_matchesRequired = other.d_ptr->_matchesRequired;
    d_ptr->_restrictionPermitted = other.d_ptr->_restrictionPermitted;
    delete d_ptr->_messageFilter;
    d_ptr->_messageFilter = 0;
    delete d_ptr->_accountFilter;
    d_ptr->_accountFilter = 0;
    delete d_ptr->_folderFilter;
    d_ptr->_folderFilter = 0;
    if (other.d_ptr->_messageFilter)
        d_ptr->_messageFilter = new QMessageFilter(*other.d_ptr->_messageFilter);
    if (other.d_ptr->_accountFilter)
        d_ptr->_accountFilter = new QMessageAccountFilter(*other.d_ptr->_accountFilter);
    if (other.d_ptr->_folderFilter)
        d_ptr->_folderFilter = new QMessageFolderFilter(*other.d_ptr->_folderFilter);

    if (other.d_ptr->_left)
        d_ptr->_left = new QMessageFilter(*other.d_ptr->_left);
    if (other.d_ptr->_right)
        d_ptr->_right = new QMessageFilter(*other.d_ptr->_right);
    return *this;
}

void QMessageFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
{
    d_ptr->_matchFlags = matchFlags;
    if (d_ptr->_matchFlags & QMessageDataComparator::MatchFullWord) {
        qWarning("Full word matching not supported on MAPI platforms.");
        d_ptr->_valid = false;
    } else {
        if (d_ptr->_left)
            d_ptr->_left->setMatchFlags(matchFlags);
        if (d_ptr->_right)
            d_ptr->_right->setMatchFlags(matchFlags);
    }
}

QMessageDataComparator::MatchFlags QMessageFilter::matchFlags() const
{
    return d_ptr->_matchFlags;
}

bool QMessageFilter::isEmpty() const
{
    return d_ptr->nonContainerFiltersAreEmpty() && d_ptr->containerFiltersAreEmpty();
}

bool QMessageFilter::isSupported() const
{
    return d_ptr->_valid;
}

QMessageFilter QMessageFilter::operator~() const
{
    QMessageFilter result;
    if (d_ptr->containerFiltersAreEmpty() && !d_ptr->_complex) {
        // Simple case of a native filter
        result = *this;
        int op = static_cast<int>(d_ptr->_operator) + static_cast<int>(QMessageFilterPrivate::Not);
        op = op % static_cast<int>(QMessageFilterPrivate::OperatorEnd);
        result.d_ptr->_operator = static_cast<QMessageFilterPrivate::Operator>(op);
        if (QMessageFilterPrivate::containsSenderSubfilter(*this)) {
            result.d_ptr->_restrictionPermitted = false; // Can't simply complement sender restriction
        }
    } else if (d_ptr->_complex) {
        // A filter can be in one of two forms, either
        // 1) An account and/or standard folder restriction &'d with a 'native' filter part 
        //  that can be evaluated using matchesMessage (or better a MAPI SRestriction).
        // or 2) a 'complex' filter part that consists of two subparts |'d together
        //     at least one of which is non-native. That is at least one subpart is either
        //     complex itself, or has an account and/or standard folder restriction.
        //
        // On Windows, only account and folder filters are non-native, i.e. can't be evaluated 
        //  using a MAPI SRestriction.
        // 
        // So ~(X|Y)) -> ~X&~Y -> and & will transform further

        if (!d_ptr->containerFiltersAreEmpty())
            qWarning("Complex filter has non empty container filter part");
        result = ~*d_ptr->_left & ~*d_ptr->_right;
    } else {
        QMessageFilter tmp;
        result = ~QMessageFilter();
        if (!d_ptr->_standardFoldersInclude.isEmpty() || !d_ptr->_standardFoldersExclude.isEmpty()) {
            tmp.d_ptr->_standardFoldersInclude = d_ptr->_standardFoldersExclude;
            tmp.d_ptr->_standardFoldersExclude = d_ptr->_standardFoldersInclude;
            result |= tmp;
            tmp = QMessageFilter();
        }
        if (!d_ptr->_accountsInclude.isEmpty() || !d_ptr->_accountsExclude.isEmpty()) {
            tmp.d_ptr->_accountsInclude = d_ptr->_accountsExclude;
            tmp.d_ptr->_accountsExclude = d_ptr->_accountsInclude;
            result |= tmp;
            tmp = QMessageFilter();
        }
        if (!d_ptr->_parentInclude.isEmpty() || !d_ptr->_parentExclude.isEmpty()) {
            tmp.d_ptr->_parentInclude = d_ptr->_parentExclude;
            tmp.d_ptr->_parentExclude = d_ptr->_parentInclude;
            result |= tmp;
            tmp = QMessageFilter();
        }
        if (!d_ptr->_ancestorInclude.isEmpty() || !d_ptr->_ancestorExclude.isEmpty()) {
            tmp.d_ptr->_ancestorInclude = d_ptr->_ancestorExclude;
            tmp.d_ptr->_ancestorExclude = d_ptr->_ancestorInclude;
            result |= tmp;
        }
        result |= ~d_ptr->nonContainerFiltersPart();
    }
    return result;
}

QMessageFilter QMessageFilter::operator&(const QMessageFilter& other) const
{
    QMessageFilter result(*this);
    result &= other;
    return result;
}

QMessageFilter QMessageFilter::operator|(const QMessageFilter& other) const
{
    QMessageFilter result(*this);
    result |= other;
    return result;
}

const QMessageFilter& QMessageFilter::operator&=(const QMessageFilter& other)
{
    QMessageFilter result;
    if (!d_ptr->_valid || !other.d_ptr->_valid) {
        result.d_ptr->_valid = false;
        *this = result;
        return *this;
    }
    if (&other == this)
        return *this;
    if (isEmpty()) {
        *this = other;
        return *this;
    }
    if (other.isEmpty())
        return *this;
    if (QMessageFilterPrivate::isNonMatching(*this)) {
        return *this;
    }
    if (QMessageFilterPrivate::isNonMatching(other)) {
        *this = other;
        return *this;
    }

    if (!d_ptr->_complex && !other.d_ptr->_complex) {
        // A&B
        // intersect includes and union excludes

        // empty include set means include all
        if (this->d_ptr->_standardFoldersInclude.isEmpty()) {
            result.d_ptr->_standardFoldersInclude = other.d_ptr->_standardFoldersInclude;
        } else {
            result.d_ptr->_standardFoldersInclude = this->d_ptr->_standardFoldersInclude;
            if (!other.d_ptr->_standardFoldersInclude.isEmpty()) {
                result.d_ptr->_standardFoldersInclude &= other.d_ptr->_standardFoldersInclude;
                if (result.d_ptr->_standardFoldersInclude.isEmpty()) {  // non-matching
                    *this = ~QMessageFilter();
                    return *this;
                }
            }
        }
        result.d_ptr->_standardFoldersExclude = this->d_ptr->_standardFoldersExclude;
        result.d_ptr->_standardFoldersExclude |= other.d_ptr->_standardFoldersExclude;

        if (this->d_ptr->_accountsInclude.isEmpty()) {
            result.d_ptr->_accountsInclude = other.d_ptr->_accountsInclude;
        } else {
            result.d_ptr->_accountsInclude = this->d_ptr->_accountsInclude;
            if (!other.d_ptr->_accountsInclude.isEmpty()) {
                result.d_ptr->_accountsInclude &= other.d_ptr->_accountsInclude;
                if (result.d_ptr->_accountsInclude.isEmpty()) { // non-matching
                    *this = ~QMessageFilter();
                    return *this;
                }
            }
        }
        result.d_ptr->_accountsExclude = this->d_ptr->_accountsExclude;
        result.d_ptr->_accountsExclude |= other.d_ptr->_accountsExclude;

        if (this->d_ptr->_parentInclude.isEmpty()) {
            result.d_ptr->_parentInclude = other.d_ptr->_parentInclude;
        } else {
            result.d_ptr->_parentInclude = this->d_ptr->_parentInclude;
            if (!other.d_ptr->_parentInclude.isEmpty()) {
                result.d_ptr->_parentInclude &= other.d_ptr->_parentInclude;
                if (result.d_ptr->_parentInclude.isEmpty()) { // non-matching
                    *this = ~QMessageFilter();
                    return *this;
                }
            }
        }
        result.d_ptr->_parentExclude = this->d_ptr->_parentExclude;
        result.d_ptr->_parentExclude |= other.d_ptr->_parentExclude;

        if (this->d_ptr->_ancestorInclude.isEmpty()) {
            result.d_ptr->_ancestorInclude = other.d_ptr->_ancestorInclude;
        } else {
            result.d_ptr->_ancestorInclude = this->d_ptr->_ancestorInclude;
            if (!other.d_ptr->_ancestorInclude.isEmpty()) {
                result.d_ptr->_ancestorInclude &= other.d_ptr->_ancestorInclude;
                if (result.d_ptr->_ancestorInclude.isEmpty()) { // non-matching
                    *this = ~QMessageFilter();
                    return *this;
                }
            }
        }
        result.d_ptr->_ancestorExclude = this->d_ptr->_ancestorExclude;
        result.d_ptr->_ancestorExclude |= other.d_ptr->_ancestorExclude;

        if (this->d_ptr->nonContainerFiltersPart().isEmpty() 
            || other.d_ptr->nonContainerFiltersPart().isEmpty()) {
            // Degenerate case, empty this or other nonContainerFiltersPart can be thrown away
            if (this->d_ptr->nonContainerFiltersPart().isEmpty()) {
                *this = other;
            } // else throw away empty other nonContainerFiltersPart

            // Just update the containerFiltersPart
            d_ptr->_standardFoldersInclude = result.d_ptr->_standardFoldersInclude;
            d_ptr->_standardFoldersExclude = result.d_ptr->_standardFoldersExclude;
            d_ptr->_accountsInclude = result.d_ptr->_accountsInclude;
            d_ptr->_accountsExclude = result.d_ptr->_accountsExclude;
            d_ptr->_parentInclude = result.d_ptr->_parentInclude;
            d_ptr->_parentExclude = result.d_ptr->_parentExclude;
            d_ptr->_ancestorInclude = result.d_ptr->_ancestorInclude;
            d_ptr->_ancestorExclude = result.d_ptr->_ancestorExclude;
        } else {
            QMessageFilter *left(new QMessageFilter(this->d_ptr->nonContainerFiltersPart()));
            QMessageFilter *right(new QMessageFilter(other.d_ptr->nonContainerFiltersPart()));
            *this = result;
            d_ptr->_left = left;
            d_ptr->_right = right;
            d_ptr->_operator = QMessageFilterPrivate::And;
        }
    } else if (d_ptr->_complex) { // other maybe complex
        // (X|Y)&Z -> X&Z | Y&Z  Query optimizer is not a priority
        result.d_ptr->_left = new QMessageFilter(*this->d_ptr->_left & other); // recursive evaluation
        result.d_ptr->_right = new QMessageFilter(*this->d_ptr->_right & other); // recursive evaluation
        result.d_ptr->_operator = QMessageFilterPrivate::Or;
        result.d_ptr->_complex = true;
        *this = result;
    } else { // this is not complex, other is complex
        // A&(X|Y) -> A&X | A&Y  Query optimizer is not a priority
        result.d_ptr->_left = new QMessageFilter(*this & *other.d_ptr->_left); // recursive evaluation
        result.d_ptr->_right = new QMessageFilter(*this & *other.d_ptr->_right); // recursive evaluation
        result.d_ptr->_operator = QMessageFilterPrivate::Or;
        result.d_ptr->_complex = true;
        *this = result;
    }
    return *this;
}

const QMessageFilter& QMessageFilter::operator|=(const QMessageFilter& other)
{
    QMessageFilter result;
    if (!d_ptr->_valid || !other.d_ptr->_valid) {
        result.d_ptr->_valid = false;
        *this = result;
        return *this;
    }
    if (&other == this)
        return *this;
    if (isEmpty())
        return *this;
    if (other.isEmpty()) {
        *this = other;
        return *this;
    }
    if (QMessageFilterPrivate::isNonMatching(*this)) {
        *this = other;
        return *this;
    }
    if (QMessageFilterPrivate::isNonMatching(other)) {
        return *this;
    }

    if (!d_ptr->_complex 
        && !other.d_ptr->_complex 
        && (d_ptr->containerFiltersPart() == other.d_ptr->containerFiltersPart())) {
        // F&A|F&B - > F&(A|B)
        QMessageFilter *left(new QMessageFilter(this->d_ptr->nonContainerFiltersPart()));
        QMessageFilter *right(new QMessageFilter(other.d_ptr->nonContainerFiltersPart()));
        result.d_ptr->_standardFoldersInclude = d_ptr->_standardFoldersInclude;
        result.d_ptr->_standardFoldersExclude = d_ptr->_standardFoldersExclude;
        result.d_ptr->_accountsInclude = d_ptr->_accountsInclude;
        result.d_ptr->_accountsExclude = d_ptr->_accountsExclude;
        result.d_ptr->_parentInclude = d_ptr->_parentInclude;
        result.d_ptr->_parentExclude = d_ptr->_parentExclude;
        result.d_ptr->_ancestorInclude = d_ptr->_ancestorInclude;
        result.d_ptr->_ancestorExclude = d_ptr->_ancestorExclude;
        *this = result;
        d_ptr->_left = left;
        d_ptr->_right = right;
        d_ptr->_operator = QMessageFilterPrivate::Or;
    // } else { Could have special case for neither complex and for both nonContainerFiltersIsEmpty is true
    } else {
        // X|Y
        QMessageFilter *left(new QMessageFilter(*this));
        QMessageFilter *right(new QMessageFilter(other));
        *this = result;
        d_ptr->_left = left;
        d_ptr->_right = right;
        d_ptr->_operator = QMessageFilterPrivate::Or;
        d_ptr->_complex = true;
    }

    return *this;
}

bool QMessageFilter::operator==(const QMessageFilter& other) const
{
    if (d_ptr->_standardFoldersInclude != other.d_ptr->_standardFoldersInclude)
        return false;
    if (d_ptr->_standardFoldersExclude != other.d_ptr->_standardFoldersExclude)
        return false;
    if (d_ptr->_accountsInclude != other.d_ptr->_accountsInclude)
        return false;
    if (d_ptr->_accountsExclude != other.d_ptr->_accountsExclude)
        return false;
    if (d_ptr->_parentInclude != other.d_ptr->_parentInclude)
        return false;
    if (d_ptr->_parentExclude != other.d_ptr->_parentExclude)
        return false;
    if (d_ptr->_ancestorInclude != other.d_ptr->_ancestorInclude)
        return false;
    if (d_ptr->_ancestorExclude != other.d_ptr->_ancestorExclude)
        return false;
    if (d_ptr->_complex != other.d_ptr->_complex)
        return false;

    if (other.d_ptr->_operator != d_ptr->_operator)
        return false;

    if (other.d_ptr->_matchesRequired != d_ptr->_matchesRequired)
        return false;
    if (other.d_ptr->_restrictionPermitted != d_ptr->_restrictionPermitted)
        return false;
    if (other.d_ptr->_messageFilter || d_ptr->_messageFilter) {
        if (!other.d_ptr->_messageFilter
            || !d_ptr->_messageFilter
            || !(*other.d_ptr->_messageFilter == *d_ptr->_messageFilter))
            return false;
    }
    if (other.d_ptr->_accountFilter || d_ptr->_accountFilter) {
        if (!other.d_ptr->_accountFilter
            || !d_ptr->_accountFilter
            || !(*other.d_ptr->_accountFilter == *d_ptr->_accountFilter))
            return false;
    }
    if (other.d_ptr->_folderFilter || d_ptr->_folderFilter) {
        if (!other.d_ptr->_folderFilter
            || !d_ptr->_folderFilter
            || !(*other.d_ptr->_folderFilter == *d_ptr->_folderFilter))
            return false;
    }

    if (d_ptr->_operator == QMessageFilterPrivate::Identity) {
        if (other.d_ptr->_operator != QMessageFilterPrivate::Identity)
            return false;
        return (d_ptr->_field == other.d_ptr->_field
            && d_ptr->_value == other.d_ptr->_value
            && d_ptr->_comparatorType == other.d_ptr->_comparatorType
            && d_ptr->_comparatorValue == other.d_ptr->_comparatorValue);
    }

    if (d_ptr->_left == other.d_ptr->_left 
        && d_ptr->_right == other.d_ptr->_right)
        return true;
    if (d_ptr->_left == other.d_ptr->_right 
        && d_ptr->_right == other.d_ptr->_left)
        return true; // Commutativity

    // TODO: For a system of determining equality of boolean algebra expressions see:
    // TODO:  Completely distributed normal form http://wikpedia.org/wiki/Boolean_algebra(logic)
    return false;
}

QMessageFilter QMessageFilter::byId(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageDataComparator::InclusionComparator inclCmp(QMessageDataComparator::Includes);
    if (cmp == QMessageDataComparator::NotEqual)
        inclCmp = QMessageDataComparator::Excludes;
    QMessageIdList ids;
    ids << id;

    return QMessageFilter::byId(ids, inclCmp);
}

QMessageFilter QMessageFilter::byId(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    if (QMessageDataComparator::Includes == cmp)
        result = ~QMessageFilter();

    if (ids.isEmpty()) {
        return result;
    }

#ifdef _WIN32_WCE
    QMap<MapiEntryId, QStringList> storeIds;
    foreach(QMessageId id, ids)
        storeIds[QMessageIdPrivate::storeRecordKey(id)].append(id.toString());

    QMapIterator<MapiEntryId, QStringList> i(storeIds);
#else
    QMap<MapiRecordKey, QStringList> storeIds;
    foreach(QMessageId id, ids)
        storeIds[QMessageIdPrivate::storeRecordKey(id)].append(id.toString());

    QMapIterator<MapiRecordKey, QStringList> i(storeIds);
#endif
    while (i.hasNext()) {
        i.next();
        if (QMessageDataComparator::Includes == cmp) {
            QMessageFilter tmp(QMessageFilter::byParentAccountId(QMessageAccountIdPrivate::from(i.key())));
            tmp &= QMessageFilterPrivate::from(QMessageFilterPrivate::Id, QVariant(i.value()), cmp);
            result |= tmp;
        } else {
            QMessageFilter tmp(QMessageFilter::byParentAccountId(QMessageAccountIdPrivate::from(i.key()), QMessageDataComparator::NotEqual));
            tmp |= QMessageFilterPrivate::from(QMessageFilterPrivate::Id, QVariant(i.value()), QMessageDataComparator::Excludes);
            result &= tmp;
        }
    }
    
    return result;
}

QMessageFilter QMessageFilter::byId(const QMessageFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::MessageFilter;
    result.d_ptr->_messageFilter = new QMessageFilter(filter);
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

// For the type filters the assumption is made that there is one store, default SMS store (QMessageAccount) that contains
//  only SMS messages, and all other stores (QMessageAccounts) contain only email messages.
QMessageFilter QMessageFilter::byType(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp)
{
    if (cmp == QMessageDataComparator::Equal)
        return QMessageFilter::byType(type, QMessageDataComparator::Includes);
    return QMessageFilter::byType(type, QMessageDataComparator::Excludes);
}

QMessageFilter QMessageFilter::byType(QMessage::TypeFlags aType, QMessageDataComparator::InclusionComparator cmp)
{
    QMessage::TypeFlags type(aType & (QMessage::Sms | QMessage::Email)); // strip Mms, InstantMessage
    if (type == QMessage::Sms) {
        if (cmp == QMessageDataComparator::Includes) {
            return QMessageFilter::byParentAccountId(QMessageAccount::defaultAccount(QMessage::Sms), QMessageDataComparator::Equal);
        } else {
            return QMessageFilter::byParentAccountId(QMessageAccount::defaultAccount(QMessage::Sms), QMessageDataComparator::NotEqual);
        }
    }
    if (type == QMessage::Email) {
        if (cmp == QMessageDataComparator::Includes) {
            return QMessageFilter::byParentAccountId(QMessageAccount::defaultAccount(QMessage::Sms), QMessageDataComparator::NotEqual);
        } else {
            return QMessageFilter::byParentAccountId(QMessageAccount::defaultAccount(QMessage::Sms), QMessageDataComparator::Equal);
        }
    }
    if (type == (QMessage::Sms | QMessage::Email)) {
        if (cmp == QMessageDataComparator::Includes)
            return QMessageFilter(); // inclusion, match all
        return ~QMessageFilter(); // exclusion, match none
    }
    // Mms/InstantMessage only
    if (cmp == QMessageDataComparator::Includes)
        return ~QMessageFilter(); // mms only inclusion, match none
    return QMessageFilter(); // mms only exclusion, match all
}

QMessageFilter QMessageFilter::bySender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result(QMessageFilterPrivate::from(QMessageFilterPrivate::Sender, QVariant(value), cmp));
    if (cmp != QMessageDataComparator::Equal)
        result.d_ptr->_restrictionPermitted = false;
    result.d_ptr->_matchesRequired = true;
    return result;
}

QMessageFilter QMessageFilterPrivate::bySender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    // Note: An additional matchesMessages check is reqruired, this rule is too liberal and results in a superset or the correct result set being returned
    QString sender(value);
    QString name;
    QString address;
    QString suffix;
    bool startDelimeterFound;
    QMessageAddress::parseEmailAddress(sender, &name, &address, &suffix, &startDelimeterFound);
    if (startDelimeterFound) {
        QMessageFilter result(QMessageFilterPrivate::from(QMessageFilterPrivate::SenderAddress, QVariant(address), QMessageDataComparator::Equal));
        // An exact match requires a Suffix comparision (which is not supported by MAPI) rather than just an Includes
        // Furthermore this seems to trigger some kind of MAPI restriction bug, results are being missed, so comment out for now
        // result &= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderName, QVariant(name), QMessageDataComparator::Equal);
        if (cmp == QMessageDataComparator::Equal) {
            return result;
        } else {
            return ~result;
        }
    } else {
        // value could be name or address, both are set by parseEmailAddress to the same value
        QMessageFilter result1(QMessageFilterPrivate::from(QMessageFilterPrivate::SenderName, QVariant(name), QMessageDataComparator::Equal));
        // Seems to trigger some kind of MAPI restriction bug, results are being missed, so comment out for now
        //result1 &= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderAddress, QVariant(""), QMessageDataComparator::Equal);
        QMessageFilter result2(QMessageFilterPrivate::from(QMessageFilterPrivate::SenderAddress, QVariant(address), QMessageDataComparator::Equal));
        // Seems to trigger some kind of MAPI restriction bug, results are being missed, so comment out for now
        //result2 &= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderName, QVariant(""), QMessageDataComparator::Equal);
        if (cmp == QMessageDataComparator::Equal) {
            return result1 | result2;
        } else {
            return ~(result1 | result2);
        }
    }
}

QMessageFilter QMessageFilter::bySender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result(QMessageFilterPrivate::from(QMessageFilterPrivate::Sender, QVariant(value), cmp));
    result.d_ptr->_matchesRequired = true;
    if (cmp != QMessageDataComparator::Includes)
        result.d_ptr->_restrictionPermitted = false;
    return result;
}

QMessageFilter QMessageFilterPrivate::bySender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    // Note: An additional matchesMessages check is reqruired, this rule is too liberal and results in a superset or the correct result set being returned
    if (value.isEmpty()) {
        if (cmp == QMessageDataComparator::Includes)
            return QMessageFilter();
        return ~QMessageFilter();
    }

    QString sender(value);
    QString name;
    QString address;
    QString suffix;
    bool startDelimeterFound;
    bool endDelimeterFound;
    QMessageAddress::parseEmailAddress(sender, &name, &address, &suffix, &startDelimeterFound, &endDelimeterFound);
    if (startDelimeterFound) {

        QMessageFilter result;
        if (endDelimeterFound) {
            result &= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderAddress, QVariant(address), QMessageDataComparator::Equal);
        } else {
            result &= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderAddress, QVariant(address), QMessageDataComparator::Includes);
        }

        // An exact match requires a Suffix comparision (which is not supported by MAPI) rather than just an Includes
        // Furthermore this seems to trigger some kind of MAPI restriction bug, results are being missed, so comment out for now
        // result &= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderName, QVariant(name), QMessageDataComparator::Includes);

        if (cmp == QMessageDataComparator::Includes) {
            return result;
        } else {
            return ~result;
        }
    } else {
        QMessageFilter result(QMessageFilterPrivate::from(QMessageFilterPrivate::SenderName, QVariant(name), QMessageDataComparator::Includes));
        result |= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderAddress, QVariant(address), QMessageDataComparator::Includes);
        if (cmp == QMessageDataComparator::Includes) {
            return result;
        } else {
            return ~result;
        }
    }
}

QMessageFilter QMessageFilter::byRecipients(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result(QMessageFilterPrivate::from(QMessageFilterPrivate::Recipients, QVariant(value), cmp));
    // Can't evaluate recipients filter using native MAPI restriction
    result.d_ptr->_restrictionPermitted = false;
    result.d_ptr->_matchesRequired = true;
    return result;

    // Unable to get sensible results yet from code path below
#if 0
    if (value.isEmpty()) {
        if (cmp == QMessageDataComparator::Includes)
            return QMessageFilter();
        return ~QMessageFilter();
    }

    QString recipient(value);
    QString name;
    QString address;
    QString suffix;
    bool startDelimeterFound;
    bool endDelimeterFound;
    QMessageAddress::parseEmailAddress(recipient, &name, &address, &suffix, &startDelimeterFound, &endDelimeterFound);
    if (startDelimeterFound) {

        QMessageFilter result;
        if (endDelimeterFound) {
            result &= QMessageFilterPrivate::from(QMessageFilterPrivate::RecipientAddress, QVariant(address), QMessageDataComparator::Equal);
        } else {
            result &= QMessageFilterPrivate::from(QMessageFilterPrivate::RecipientAddress, QVariant(address), QMessageDataComparator::Includes);
        }

        // Need additional matches check, this should be a Suffix comparision rather than just an Includes
        result &= QMessageFilterPrivate::from(QMessageFilterPrivate::RecipientName, QVariant(name), QMessageDataComparator::Includes);

        if (cmp == QMessageDataComparator::Includes) {
            return result;
        } else {
            return ~result;
        }
    } else {
        QMessageFilter result(QMessageFilterPrivate::from(QMessageFilterPrivate::RecipientName, QVariant(name), QMessageDataComparator::Includes));
        result |= QMessageFilterPrivate::from(QMessageFilterPrivate::RecipientAddress, QVariant(address), QMessageDataComparator::Includes);
        if (cmp == QMessageDataComparator::Includes) {
            return result;
        } else {
            return ~result;
        }
    }
#endif
}

QMessageFilter QMessageFilter::bySubject(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterPrivate::from(QMessageFilterPrivate::Subject, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::bySubject(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    if (value.isEmpty()) {
        if (cmp == QMessageDataComparator::Includes)
            return QMessageFilter();
        return ~QMessageFilter();
    }
    return QMessageFilterPrivate::from(QMessageFilterPrivate::Subject, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::byTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterPrivate::from(QMessageFilterPrivate::TimeStamp, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::byTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    return QMessageFilterPrivate::from(QMessageFilterPrivate::TimeStamp, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterPrivate::from(QMessageFilterPrivate::ReceptionTimeStamp, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    return QMessageFilterPrivate::from(QMessageFilterPrivate::ReceptionTimeStamp, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::byStatus(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageDataComparator::InclusionComparator comparator(QMessageDataComparator::Excludes);
    if (cmp == QMessageDataComparator::Equal)
        comparator = QMessageDataComparator::Includes;
    return QMessageFilterPrivate::from(QMessageFilterPrivate::Status, QVariant(value), comparator);
}

QMessageFilter QMessageFilter::byStatus(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    QMessageDataComparator::EqualityComparator comparator(QMessageDataComparator::NotEqual);
    if (cmp == QMessageDataComparator::Includes) {
        comparator = QMessageDataComparator::Equal;
    }
    if (mask & QMessage::Incoming) {
        result &= QMessageFilter::byStatus(QMessage::Incoming, comparator);

        // We can't use a restriction to enforce this filter
        result.d_ptr->_restrictionPermitted = false;
        result.d_ptr->_matchesRequired = true;
    }
    if (mask & QMessage::Read) {
        result &= QMessageFilter::byStatus(QMessage::Read, comparator);
    }
    if (mask & QMessage::Removed) {
        result &= QMessageFilter::byStatus(QMessage::Removed, comparator);
    }
    if (mask & QMessage::HasAttachments) {
        result &= QMessageFilter::byStatus(QMessage::HasAttachments, comparator);
    }
    if (result.isEmpty()) // Be consistent with QMF, but seems wrong. TODO verify correctness
        return ~result;
    return result;
}

QMessageFilter QMessageFilter::byPriority(QMessage::Priority value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterPrivate::from(QMessageFilterPrivate::Priority, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::bySize(int value, QMessageDataComparator::EqualityComparator cmp)
{
    // TODO: Test this filter
    return QMessageFilterPrivate::from(QMessageFilterPrivate::Size, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::bySize(int value, QMessageDataComparator::RelationComparator cmp)
{
    return QMessageFilterPrivate::from(QMessageFilterPrivate::Size, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    if (cmp == QMessageDataComparator::Equal)
        result.d_ptr->_accountsInclude += id;
    else
        result.d_ptr->_accountsExclude += id;
    return result;
}

QMessageFilter QMessageFilter::byParentAccountId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::AccountFilter;
    result.d_ptr->_accountFilter = new QMessageAccountFilter(filter);
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFilter QMessageFilter::byStandardFolder(QMessage::StandardFolder folder, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    if (cmp == QMessageDataComparator::Equal)
        result.d_ptr->_standardFoldersInclude += folder;
    else
        result.d_ptr->_standardFoldersExclude += folder;
    return result;
}

QMessageFilter QMessageFilter::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
#ifndef QSTRING_FOLDER_ID
    result.d_ptr->_parentInclude += id;
#else
    QMessageFolder folder(id);
    result.d_ptr->_parentInclude += folder.parentAccountId().toString() + "/" + folder.path();
#endif
    // An invalid id is a special case, assume it means any top-level folder (with root as parent) in any MAPI store
    if (id.isValid()) {
        result &= QMessageFilter::byParentAccountId(QMessageAccountIdPrivate::from(QMessageFolderIdPrivate::storeRecordKey(id)));
    }
    if (cmp != QMessageDataComparator::Equal) {
        result = ~result;
    }
    return result;
}

QMessageFilter QMessageFilter::byParentFolderId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::FolderFilter;
    result.d_ptr->_folderFilter = new QMessageFolderFilter(filter);
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFilter QMessageFilter::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    if (id.isValid()) {
#ifndef QSTRING_FOLDER_ID
        result.d_ptr->_ancestorInclude += id;
#else
        QMessageFolder folder(id);
        result.d_ptr->_ancestorInclude += folder.parentAccountId().toString() + "/" + folder.path();
#endif
    } else {
        result = ~result;
    }
    if (cmp != QMessageDataComparator::Includes) {
        result = ~result;
    }
    return result;
}

QMessageFilter QMessageFilter::byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::AncestorFilter;
    result.d_ptr->_folderFilter = new QMessageFolderFilter(filter);
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

void QMessageFilterPrivate::debug(const QMessageFilter &filter, const QString &indent)
{
    QString operatorStr;
    QString fieldStr;
    QStringList standardFoldersInclude;
    QStringList standardFoldersExclude;
    QStringList accountsInclude;
    QStringList accountsExclude;
    QStringList fields;
    fields << "None" << "Id" << "Type" << "Sender" << "SenderName" << "SenderAddress" << "Recipients" << "RecipientName" << "RecipientAddress" << "Subject" << "TimeStamp" << "ReceptionTimeStamp" << "Status" << "Priority" << "Size" << "ParentAccountId" << "ParentFolderId" << "AncestorFolderIds" << "MessageFilter" << "AccountFilter" << "FolderFilter" << "AncestorFilter";
    QStringList operators;
    operators << "Identity" << "And" << "Or" << "Not" << "Nand" << "Nor";
    qDebug() << indent << "field" << fields[filter.d_ptr->_field] << "operator" << operators[filter.d_ptr->_operator] << "complex" << filter.d_ptr->_complex;

    qDebug() << indent << "parentInclude" << filter.d_ptr->_parentInclude.count() << "parentExclude" << filter.d_ptr->_parentExclude.count()
        << "accountsInclude" << filter.d_ptr->_accountsInclude.count() << "accountsExclude" << filter.d_ptr->_accountsExclude.count()
        << "ancestorInclude" << filter.d_ptr->_ancestorInclude.count() << "ancestorExclude" << filter.d_ptr->_ancestorExclude.count()
        << "standardFoldersIncludeInclude" << filter.d_ptr->_standardFoldersInclude.count() << "standardFoldersIncludeExclude" << filter.d_ptr->_standardFoldersExclude.count();

    QStringList standards;
    standards << "InboxFolder" << "OutboxFolder" << "DraftsFolder" << "SentFolder" << "TrashFolder";
    QStringList sfi, sfe;
    foreach(QMessage::StandardFolder std, filter.d_ptr->_standardFoldersInclude)
        sfi.append(standards[std-1]);
    foreach(QMessage::StandardFolder std, filter.d_ptr->_standardFoldersExclude)
        sfe.append(standards[std-1]);
    if (!sfi.isEmpty() || !sfe.isEmpty())
        qDebug() << indent << "sfi" << sfi << "sfe" << sfe;
    if (filter.d_ptr->_left) {
        qDebug() << indent << "left";
        QMessageFilterPrivate::debug(*filter.d_ptr->_left, indent + " ");
    }
    if (filter.d_ptr->_right) {
        qDebug() << indent << "right";
        QMessageFilterPrivate::debug(*filter.d_ptr->_right, indent + " ");
    }
}

QTM_END_NAMESPACE
