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
#include "qmessagefolderfilter.h"
#include "qmessagefolderfilter_p.h"

QTM_BEGIN_NAMESPACE

QMessageFolderFilterPrivate::QMessageFolderFilterPrivate(QMessageFolderFilter *folderFilter)
    : q_ptr(folderFilter),
      _operator(Identity),
      _criterion(None),
      _equality(QMessageDataComparator::Equal),
      _inclusion(QMessageDataComparator::Includes),
      _matchFlags(0),
      _valid(true),
      _accountFilter(0),
      _folderFilter(0)
{
}

QMessageFolderFilterPrivate::~QMessageFolderFilterPrivate()
{
    foreach(QMessageFolderFilter* filter, _arguments) {
        delete filter;
    }
    _arguments.clear();
    delete _accountFilter;
    _accountFilter = 0;
    delete _folderFilter;
    _folderFilter = 0;
}

QMessageFolderFilterPrivate& QMessageFolderFilterPrivate::operator=(const QMessageFolderFilterPrivate &other)
{
    _operator = other._operator;
    _criterion = other._criterion;
    _ids = other._ids;
    _accountIds = other._accountIds;
    _value = other._value;
    _equality = other._equality;
    _inclusion = other._inclusion;
    _matchFlags = other._matchFlags;
    _valid = other._valid;
    foreach(QMessageFolderFilter* filter, _arguments) {
        delete filter;
    }
    _arguments.clear();
    foreach(QMessageFolderFilter* _filter, other._arguments) {
        _arguments.append(new QMessageFolderFilter(*_filter));
    }

    delete _accountFilter;
    _accountFilter = 0;
    if (other._accountFilter)
        _accountFilter = new QMessageAccountFilter(*other._accountFilter);
    delete _folderFilter;
    _folderFilter = 0;
    if (other._folderFilter)
        _folderFilter = new QMessageFolderFilter(*other._folderFilter);

    return *this;
}

bool QMessageFolderFilterPrivate::operator==(const QMessageFolderFilterPrivate &other) const
{
    if  ((_operator != other._operator) ||
         (_criterion != other._criterion) ||
         (_ids != other._ids) ||
         (_accountIds != other._accountIds) ||
         (_value != other._value) ||
         (_equality != other._equality) ||
         (_inclusion != other._inclusion) ||
         (_matchFlags != other._matchFlags)) {
             return false;
    }
    if (_arguments.count() != other._arguments.count())
        return false;
    for(int i = 0; i < _arguments.count(); ++i) {
        if (*_arguments[i] != *other._arguments[i]) {
            return false;
        }
    }
    if (_accountFilter || other._accountFilter) {
        if (!_accountFilter || !other._accountFilter || (*_accountFilter != *other._accountFilter)) {
            return false;
        }
    }
    if (_folderFilter || other._folderFilter) {
        if (!_folderFilter || !other._folderFilter || (*_folderFilter != *other._folderFilter)) {
            return false;
        }
    }
    return true;
}

bool QMessageFolderFilterPrivate::matchesFolder(const QMessageFolderFilter &filter, const MapiFolderPtr &folder)
{
    Qt::CaseSensitivity caseSensitivity(Qt::CaseInsensitive);
    if (filter.matchFlags() & QMessageDataComparator::MatchCaseSensitive)
        caseSensitivity = Qt::CaseSensitive;

    QMessageFolderFilterPrivate *f(filter.d_ptr);
    bool result;
    bool negate(false);
    switch (f->_operator) {
    case Not: // fall through
    case Nand: // fall through
    case Nor:
        negate = true;
        break;
    default:
        break;
    } // switch

    switch (f->_operator) {
    case Identity: // fall through
    case Not: {
        QMessageFolderFilterPrivate *f(filter.d_ptr);

        if ((f->_criterion == IdEquality) || (f->_criterion == IdInclusion)) {
            QMessageFolderId folderId(folder->id());

            if (f->_criterion == IdEquality) {
                bool idEqual(folderId == *f->_ids.begin());
                result = (f->_equality == QMessageDataComparator::Equal ? idEqual : !idEqual);
            } else {
                bool idPresent(f->_ids.contains(folderId));
                result = (f->_inclusion == QMessageDataComparator::Includes ? idPresent : !idPresent);
            }
        } else if ((f->_criterion == NameEquality) || (f->_criterion == NameInclusion)) {
            QString folderName(folder->name());

            if (f->_criterion == NameEquality) {
                bool nameEqual(QString::compare(folderName, f->_value, caseSensitivity) == 0);
                result = (f->_equality == QMessageDataComparator::Equal ? nameEqual : !nameEqual);
            } else {
                bool namePartialMatch(folderName.contains(f->_value, caseSensitivity));
                result = (f->_inclusion == QMessageDataComparator::Includes ? namePartialMatch : !namePartialMatch);
            }
        } else if ((f->_criterion == PathEquality) || (f->_criterion == PathInclusion)) {
            QString folderPath(QMessageFolder(folder->id()).path());

            if (f->_criterion == PathEquality) {
                bool pathEqual(QString::compare(folderPath, f->_value, caseSensitivity) == 0);
                result = (f->_equality == QMessageDataComparator::Equal ? pathEqual : !pathEqual);
            } else {
                bool pathPartialMatch(folderPath.contains(f->_value, caseSensitivity));
                result = (f->_inclusion == QMessageDataComparator::Includes ? pathPartialMatch : !pathPartialMatch);
            }
        } else if ((f->_criterion == AccountEquality) || (f->_criterion == AccountInclusion)) {
            QMessageAccountId accountId(folder->accountId());

            if (f->_criterion == AccountEquality) {
                bool idEqual(accountId == *f->_accountIds.begin());
                result = (f->_equality == QMessageDataComparator::Equal ? idEqual : !idEqual);
            } else {
                bool idPresent(f->_accountIds.contains(accountId));
                result = (f->_inclusion == QMessageDataComparator::Includes ? idPresent : !idPresent);
            }
        } else if ((f->_criterion == ParentEquality) || (f->_criterion == ParentInclusion)) {
            QMessageFolderId parentId(folder->parentId());

            if (f->_criterion == ParentEquality) {
                bool idEqual(parentId == *f->_ids.begin());
                result = (f->_equality == QMessageDataComparator::Equal ? idEqual : !idEqual);
            } else {
                bool idPresent(f->_ids.contains(parentId));
                result = (f->_inclusion == QMessageDataComparator::Includes ? idPresent : !idPresent);
            }
        } else if (f->_criterion == AncestorInclusion) {
            QSet<QMessageFolderId> ancestorIds(folder->ancestorIds().toSet());
            bool idIncluded(ancestorIds.intersect(f->_ids).isEmpty() == false);
            result = (f->_inclusion == QMessageDataComparator::Includes ? idIncluded : !idIncluded);
        } else { // None
            result = (f->_equality == QMessageDataComparator::Equal);
        }
        break;
    }
    case And: // fall through
    case Nand: {
        result = true;
        Q_ASSERT(f->_arguments.count());
        foreach(QMessageFolderFilter *subfilter, f->_arguments) {
            if (!f->matchesFolder(*subfilter, folder)) {
                result = false;
                break;
            }
        }
        break;
    }
    case Or: // fall through
    case Nor: {
        result = false;
        Q_ASSERT(f->_arguments.count());
        foreach(QMessageFolderFilter *subfilter, f->_arguments) {
            if (f->matchesFolder(*subfilter, folder)) {
                result = true;
                break;
            }
        }
        break;
    }
    } // switch

    if (negate)
        result = !result;
    return result;
}

QMessageFolderFilter QMessageFolderFilterPrivate::preprocess(QMessageManager::Error *error, MapiSessionPtr session, const QMessageFolderFilter &filter)
{
    QMessageFolderFilter result(filter);
    QMessageFolderFilterPrivate::preprocess(error, session, &result);
    return result;
}

void QMessageFolderFilterPrivate::preprocess(QMessageManager::Error *error, MapiSessionPtr session, QMessageFolderFilter *filter)
{
    if (!filter)
        return;

    bool inclusion(filter->d_ptr->_inclusion == QMessageDataComparator::Includes);
    QMessageFolderFilter result;
    if (inclusion) {
        result = ~QMessageFolderFilter();
    }
    if (filter->d_ptr->_criterion == ParentAccountFilter) {
        QList<MapiStorePtr> stores(session->filterStores(error, *filter->d_ptr->_accountFilter));
        foreach(MapiStorePtr store, stores) {
            if (inclusion) {
                result |= QMessageFolderFilter::byParentAccountId(store->id());
            } else {
                result &= QMessageFolderFilter::byParentAccountId(store->id(), QMessageDataComparator::NotEqual);
            }
        }
    } else if (filter->d_ptr->_criterion == ParentFolderFilter) {
        QList<MapiFolderPtr> folders(session->filterFolders(error, *filter->d_ptr->_folderFilter));
        foreach(MapiFolderPtr folder, folders) {
            if (inclusion) {
                result |= QMessageFolderFilter::byParentFolderId(folder->id());
            } else {
                result &= QMessageFolderFilter::byParentFolderId(folder->id(), QMessageDataComparator::NotEqual);
            }
        }
    } else if (filter->d_ptr->_criterion == AncestorFolderFilter) {
        QList<MapiFolderPtr> folders(session->filterFolders(error, *filter->d_ptr->_folderFilter));
        foreach(MapiFolderPtr folder, folders) {
            if (inclusion) {
                result |= QMessageFolderFilter::byAncestorFolderIds(folder->id());
            } else {
                result &= QMessageFolderFilter::byAncestorFolderIds(folder->id(), QMessageDataComparator::Excludes);
            }
        }
    } else {
        foreach(QMessageFolderFilter *subfilter, filter->d_ptr->_arguments) {
            preprocess(error, session, subfilter);
        }
        return;
    }
    if (filter->d_ptr->_operator == Not) // must be Not or Identity
        result = ~result;
    *filter = result;
}

QMessageFolderFilter::QMessageFolderFilter()
    : d_ptr(new QMessageFolderFilterPrivate(this))
{
}

QMessageFolderFilter::QMessageFolderFilter(const QMessageFolderFilter &other)
    : d_ptr(new QMessageFolderFilterPrivate(this))
{
    this->operator=(other);
}

QMessageFolderFilter::~QMessageFolderFilter()
{
    delete d_ptr;
}

QMessageFolderFilter& QMessageFolderFilter::operator=(const QMessageFolderFilter& other)
{
    *d_ptr = *other.d_ptr;
    return *this;
}

void QMessageFolderFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
{
    d_ptr->_matchFlags = matchFlags;
    d_ptr->_valid = true;
    if (d_ptr->_matchFlags & QMessageDataComparator::MatchFullWord)
        d_ptr->_valid = false; // Not supported
    foreach(QMessageFolderFilter *subfilter, d_ptr->_arguments) {
        subfilter->setMatchFlags(matchFlags);
    }
}

QMessageDataComparator::MatchFlags QMessageFolderFilter::matchFlags() const
{
    return d_ptr->_matchFlags;
}

bool QMessageFolderFilter::isEmpty() const
{
    return ((d_ptr->_operator == QMessageFolderFilterPrivate::Identity) &&
            (d_ptr->_criterion == QMessageFolderFilterPrivate::None) && 
            (d_ptr->_equality == QMessageDataComparator::Equal));
}

bool QMessageFolderFilterPrivate::isNonMatching(const QMessageFolderFilter &filter)
{
    return ((filter.d_ptr->_operator == QMessageFolderFilterPrivate::Not) &&
            (filter.d_ptr->_criterion == QMessageFolderFilterPrivate::None) && 
            (filter.d_ptr->_equality == QMessageDataComparator::Equal));
}

bool QMessageFolderFilter::isSupported() const
{
    return d_ptr->_valid;
}

QMessageFolderFilter QMessageFolderFilter::operator~() const
{
    QMessageFolderFilter result(*this);
    int op = static_cast<int>(d_ptr->_operator) + static_cast<int>(QMessageFolderFilterPrivate::Not);
    op = op % static_cast<int>(QMessageFolderFilterPrivate::OperatorEnd);
    result.d_ptr->_operator = static_cast<QMessageFolderFilterPrivate::Operator>(op);
    return result;
}

QMessageFolderFilter QMessageFolderFilter::operator&(const QMessageFolderFilter& other) const
{
    QMessageFolderFilter result(*this);
    result &= other;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::operator|(const QMessageFolderFilter& other) const
{
    QMessageFolderFilter result(*this);
    result |= other;
    return result;
}

const QMessageFolderFilter& QMessageFolderFilter::operator&=(const QMessageFolderFilter& other)
{
    if (!d_ptr->_valid || !other.d_ptr->_valid) {
        QMessageFolderFilter result;
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
    if (QMessageFolderFilterPrivate::isNonMatching(*this)) {
        return *this;
    }
    if (QMessageFolderFilterPrivate::isNonMatching(other)) {
        *this = other;
        return *this;
    }

    if (d_ptr->_operator != QMessageFolderFilterPrivate::And) {
        QMessageFolderFilter result;
        result.d_ptr->_operator = QMessageFolderFilterPrivate::And;
        result.d_ptr->_arguments.append(new QMessageFolderFilter(*this));
        *this = result;
    }
    d_ptr->_arguments.append(new QMessageFolderFilter(other));
    return *this;
}

const QMessageFolderFilter& QMessageFolderFilter::operator|=(const QMessageFolderFilter& other)
{
    if (!d_ptr->_valid || !other.d_ptr->_valid) {
        QMessageFolderFilter result;
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
    if (QMessageFolderFilterPrivate::isNonMatching(*this)) {
        *this = other;
        return *this;
    }
    if (QMessageFolderFilterPrivate::isNonMatching(other)) {
        return *this;
    }

    if (d_ptr->_operator != QMessageFolderFilterPrivate::Or) {
        QMessageFolderFilter result;
        result.d_ptr->_operator = QMessageFolderFilterPrivate::Or;
        result.d_ptr->_arguments.append(new QMessageFolderFilter(*this));
        *this = result;
    }
    d_ptr->_arguments.append(new QMessageFolderFilter(other));
    return *this;
}

bool QMessageFolderFilter::operator==(const QMessageFolderFilter& other) const
{
    return (*d_ptr == *other.d_ptr);
}

QMessageFolderFilter QMessageFolderFilter::byId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    if (!id.isValid()) {
        QMessageFolderFilter result(~QMessageFolderFilter()); // non-matching
        if (QMessageDataComparator::NotEqual == cmp) {
            result = ~result;
        }
        return result;
    }

    QMessageFolderFilter result;
    result.d_ptr->_ids.insert(id);
    result.d_ptr->_equality = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::IdEquality;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byId(const QMessageFolderIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderIdList validIds;
    foreach(QMessageFolderId id, ids) {
        if (id.isValid()) {
            validIds.append(id);
        }
    }
    if (validIds.isEmpty()) {
        QMessageFolderFilter result(~QMessageFolderFilter()); // non-matching
        if (QMessageDataComparator::Excludes == cmp) {
            result = ~result;
        }
        return result;
    }

    QMessageFolderFilter result;
    result.d_ptr->_ids = validIds.toSet();
    result.d_ptr->_inclusion = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::IdInclusion;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    *result.d_ptr = *filter.d_ptr;
    if (cmp == QMessageDataComparator::Excludes) {
        // Invert the sense of comparison
        result = ~result;
    }
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_value = value;
    result.d_ptr->_equality = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::NameEquality;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_value = value;
    result.d_ptr->_inclusion = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::NameInclusion;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byPath(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_value = value;
    result.d_ptr->_equality = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::PathEquality;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byPath(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_value = value;
    result.d_ptr->_inclusion = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::PathInclusion;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    if (!id.isValid()) {
        QMessageFolderFilter result(~QMessageFolderFilter()); // non-matching
        if (QMessageDataComparator::NotEqual == cmp) {
            result = ~result;
        }
        return result;
    }
    QMessageFolderFilter result;
    result.d_ptr->_accountIds.insert(id);
    result.d_ptr->_equality = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::AccountEquality;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentAccountId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_accountFilter = new QMessageAccountFilter(filter);
    result.d_ptr->_inclusion = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::ParentAccountFilter;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_ids.insert(id);
    result.d_ptr->_equality = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::ParentEquality;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentFolderId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_folderFilter = new QMessageFolderFilter(filter);
    result.d_ptr->_inclusion = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::ParentFolderFilter;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_ids.insert(id);
    result.d_ptr->_inclusion = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::AncestorInclusion;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_folderFilter = new QMessageFolderFilter(filter);
    result.d_ptr->_inclusion = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::AncestorFolderFilter;
    return result;
}

QTM_END_NAMESPACE
