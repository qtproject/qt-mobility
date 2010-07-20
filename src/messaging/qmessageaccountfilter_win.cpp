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
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"

QTM_BEGIN_NAMESPACE

QMessageAccountFilterPrivate::QMessageAccountFilterPrivate(QMessageAccountFilter *accountFilter)
    : q_ptr(accountFilter),
      _operator(Identity),
      _criterion(None),
      _equality(QMessageDataComparator::Equal),
      _inclusion(QMessageDataComparator::Includes),
      _matchFlags(0),
      _valid(true)
{
}

QMessageAccountFilterPrivate::~QMessageAccountFilterPrivate()
{
    foreach(QMessageAccountFilter* filter, _arguments) {
        delete filter;
    }
    _arguments.clear();
}

QMessageAccountFilterPrivate &QMessageAccountFilterPrivate::operator=(const QMessageAccountFilterPrivate &other)
{
    _operator = other._operator;
    _criterion = other._criterion;
    _ids = other._ids;
    _name = other._name;
    _equality = other._equality;
    _inclusion = other._inclusion;
    _matchFlags = other._matchFlags;
    _valid = other._valid;
    foreach(QMessageAccountFilter* filter, _arguments) {
        delete filter;
    }
    _arguments.clear();
    foreach(QMessageAccountFilter* _filter, other._arguments) {
        _arguments.append(new QMessageAccountFilter(*_filter));
    }

    return *this;
}

bool QMessageAccountFilterPrivate::operator==(const QMessageAccountFilterPrivate &other)
{
    bool result((_operator == other._operator) &&
                (_criterion == other._criterion) &&
                (_ids == other._ids) &&
                (_name == other._name) &&
                (_equality == other._equality) &&
                (_inclusion == other._inclusion) &&
                (_matchFlags == other._matchFlags) &&
                (_valid == other._valid));
    if (!result)
        return false;
    if (_arguments.count() != other._arguments.count())
        result = false;
    for(int i = 0; result && (i < _arguments.count()); ++i) {
        if (*_arguments[i] != *other._arguments[i]) {
            result = false;
            break;
        }
    }
    return result;
}

bool QMessageAccountFilterPrivate::matchesStore(const QMessageAccountFilter &filter, const MapiStorePtr &store)
{
    QMessageAccountFilterPrivate *f(filter.d_ptr);
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
        if ((f->_criterion == IdEquality) || (f->_criterion == IdInclusion)) {
            QMessageAccountId storeId(store->id());

            if (f->_criterion == IdEquality) {
                bool idEqual(storeId == *f->_ids.begin());
                result = (f->_equality == QMessageDataComparator::Equal ? idEqual : !idEqual);
            } else {
                bool idPresent(f->_ids.contains(storeId));
                result = (f->_inclusion == QMessageDataComparator::Includes ? idPresent : !idPresent);
            }
        } else if ((f->_criterion == NameEquality) || (f->_criterion == NameInclusion)) {
            QString storeName(store->name());
            QString fName(f->_name);
            if ((f->_matchFlags & QMessageDataComparator::MatchCaseSensitive) == 0) {
                storeName = storeName.toLower();
                fName = fName.toLower();
            }

            if (f->_criterion == NameEquality) {
                // TODO: Apply matchFlags
                bool nameEqual(storeName == fName);
                result = (f->_equality == QMessageDataComparator::Equal ? nameEqual : !nameEqual);
            } else {
                bool namePartialMatch(storeName.contains(fName));
                result = (f->_inclusion == QMessageDataComparator::Includes ? namePartialMatch : !namePartialMatch);
            }
        } else { // None
            result = (f->_equality == QMessageDataComparator::Equal);
        }
        break;
    }
    case And: // fall through
    case Nand: {
        result = true;
        Q_ASSERT(f->_arguments.count());
        foreach(QMessageAccountFilter *subfilter, f->_arguments) {
            if (!f->matchesStore(*subfilter, store)) {
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
        foreach(QMessageAccountFilter *subfilter, f->_arguments) {
            if (f->matchesStore(*subfilter, store)) {
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

QMessageAccountFilter::QMessageAccountFilter()
    : d_ptr(new QMessageAccountFilterPrivate(this))
{
}

QMessageAccountFilter::QMessageAccountFilter(const QMessageAccountFilter &other)
    : d_ptr(new QMessageAccountFilterPrivate(this))
{
    this->operator=(other);
}

QMessageAccountFilter::~QMessageAccountFilter()
{
    delete d_ptr;
}

QMessageAccountFilter& QMessageAccountFilter::operator=(const QMessageAccountFilter& other)
{
    *d_ptr = *other.d_ptr;
    return *this;
}

void QMessageAccountFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
{
    d_ptr->_matchFlags = matchFlags;
    d_ptr->_valid = true;
    if (d_ptr->_matchFlags & QMessageDataComparator::MatchFullWord)
        d_ptr->_valid = false; // Not supported
    foreach(QMessageAccountFilter *subfilter, d_ptr->_arguments) {
        subfilter->setMatchFlags(matchFlags);
    }
}

QMessageDataComparator::MatchFlags QMessageAccountFilter::matchFlags() const
{
    return d_ptr->_matchFlags;
}

bool QMessageAccountFilter::isEmpty() const
{
    return ((d_ptr->_operator == QMessageAccountFilterPrivate::Identity) &&
            (d_ptr->_criterion == QMessageAccountFilterPrivate::None) && 
            (d_ptr->_equality == QMessageDataComparator::Equal));
}

bool QMessageAccountFilterPrivate::isNonMatching(const QMessageAccountFilter &filter)
{
    return ((filter.d_ptr->_operator == QMessageAccountFilterPrivate::Not) &&
            (filter.d_ptr->_criterion == QMessageAccountFilterPrivate::None) && 
            (filter.d_ptr->_equality == QMessageDataComparator::Equal));
}

bool QMessageAccountFilter::isSupported() const
{
    return d_ptr->_valid;
}

QMessageAccountFilter QMessageAccountFilter::operator~() const
{
    QMessageAccountFilter result(*this);
    int op = static_cast<int>(d_ptr->_operator) + static_cast<int>(QMessageAccountFilterPrivate::Not);
    op = op % static_cast<int>(QMessageAccountFilterPrivate::OperatorEnd);
    result.d_ptr->_operator = static_cast<QMessageAccountFilterPrivate::Operator>(op);
    return result;
}

QMessageAccountFilter QMessageAccountFilter::operator&(const QMessageAccountFilter& other) const
{
    QMessageAccountFilter result(*this);
    result &= other;
    return result;
}

QMessageAccountFilter QMessageAccountFilter::operator|(const QMessageAccountFilter& other) const
{
    QMessageAccountFilter result(*this);
    result |= other;
    return result;
}

const QMessageAccountFilter& QMessageAccountFilter::operator&=(const QMessageAccountFilter& other)
{
    if (!d_ptr->_valid || !other.d_ptr->_valid) {
        QMessageAccountFilter result;
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
    if (QMessageAccountFilterPrivate::isNonMatching(*this)) {
        return *this;
    }
    if (QMessageAccountFilterPrivate::isNonMatching(other)) {
        *this = other;
        return *this;
    }

    if (d_ptr->_operator != QMessageAccountFilterPrivate::And) {
        QMessageAccountFilter result;
        result.d_ptr->_operator = QMessageAccountFilterPrivate::And;
        result.d_ptr->_arguments.append(new QMessageAccountFilter(*this));
        *this = result;
    }
    d_ptr->_arguments.append(new QMessageAccountFilter(other));
    return *this;
}

const QMessageAccountFilter& QMessageAccountFilter::operator|=(const QMessageAccountFilter& other)
{
    if (!d_ptr->_valid || !other.d_ptr->_valid) {
        QMessageAccountFilter result;
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
    if (QMessageAccountFilterPrivate::isNonMatching(*this)) {
        *this = other;
        return *this;
    }
    if (QMessageAccountFilterPrivate::isNonMatching(other)) {
        return *this;
    }

    if (d_ptr->_operator != QMessageAccountFilterPrivate::Or) {
        QMessageAccountFilter result;
        result.d_ptr->_operator = QMessageAccountFilterPrivate::Or;
        result.d_ptr->_arguments.append(new QMessageAccountFilter(*this));
        *this = result;
    }
    d_ptr->_arguments.append(new QMessageAccountFilter(other));
    return *this;
}

bool QMessageAccountFilter::operator==(const QMessageAccountFilter& other) const
{
    return (*d_ptr == *other.d_ptr);
}

QMessageAccountFilter QMessageAccountFilter::byId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageAccountFilter result;
    result.d_ptr->_ids.insert(id);
    result.d_ptr->_equality = cmp;
    result.d_ptr->_criterion = QMessageAccountFilterPrivate::IdEquality;
    return result;
}

QMessageAccountFilter QMessageAccountFilter::byId(const QMessageAccountIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageAccountFilter result;
    result.d_ptr->_ids = ids.toSet();
    result.d_ptr->_inclusion = cmp;
    result.d_ptr->_criterion = QMessageAccountFilterPrivate::IdInclusion;
    return result;
}

QMessageAccountFilter QMessageAccountFilter::byId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageAccountFilter result;
    *result.d_ptr = *filter.d_ptr;
    if (cmp == QMessageDataComparator::Excludes) {
        result = ~result;
    }
    return result;
}

QMessageAccountFilter QMessageAccountFilter::byName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageAccountFilter result;
    result.d_ptr->_name = value;
    result.d_ptr->_equality = cmp;
    result.d_ptr->_criterion = QMessageAccountFilterPrivate::NameEquality;
    return result;
}

QMessageAccountFilter QMessageAccountFilter::byName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageAccountFilter result;
    result.d_ptr->_name = value;
    result.d_ptr->_inclusion = cmp;
    result.d_ptr->_criterion = QMessageAccountFilterPrivate::NameInclusion;
    return result;
}

QTM_END_NAMESPACE
