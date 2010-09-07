/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"

QTM_BEGIN_NAMESPACE

QMessageAccountFilterPrivate::QMessageAccountFilterPrivate(QMessageAccountFilter *accountFilter)
 : q_ptr(accountFilter)
{
}

QMessageAccountFilterPrivate::~QMessageAccountFilterPrivate()
{
}

bool QMessageAccountFilterPrivate::lessThan(const QMessageAccountFilter filter1, const QMessageAccountFilter filter2)
{
    if (filter1.d_ptr->_field == filter2.d_ptr->_field) {
        if (filter1.d_ptr->_comparatorType == filter2.d_ptr->_comparatorType) {
            return filter1.d_ptr->_comparatorValue < filter2.d_ptr->_comparatorValue;
        }
        return filter1.d_ptr->_comparatorType < filter2.d_ptr->_comparatorType;
    }
    return filter1.d_ptr->_field < filter2.d_ptr->_field;
}

void QMessageAccountFilterPrivate::changeComparatorValuesToOpposite(QMessageAccountFilter& filter)
{
    if (filter.d_ptr->_filterList.count() == 0) {
        if (filter.d_ptr->_comparatorType == QMessageAccountFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter.d_ptr->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
            } else {
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
            }
        } else { // Inclusion
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter.d_ptr->_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::Excludes);
            } else { // Excludes
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::Includes);
            }
        }
    } else {
        for (int i=0; i < filter.d_ptr->_filterList.count(); i++) {
            for (int j=0; j < filter.d_ptr->_filterList[i].count(); j++) {
                QMessageAccountFilterPrivate::changeComparatorValuesToOpposite(filter.d_ptr->_filterList[i][j]);
            }
        }
    }
}

void QMessageAccountFilterPrivate::changeANDsAndORsToOpposite(QMessageAccountFilter& filter)
{
    if (filter.d_ptr->_filterList.count() > 0) {
        QMessageAccountFilter oldFilter = filter;
        filter.d_ptr->_filterList.clear();
        for (int i=0; i < oldFilter.d_ptr->_filterList.count(); i++) {
            if (i == 0) {
                for (int j=0; j < oldFilter.d_ptr->_filterList[i].count(); j++) {
                    if (j == 0) {
                        filter.d_ptr->_filterList.append(QMessageAccountFilterPrivate::SortedMessageAccountFilterList());
                        filter.d_ptr->_filterList[0] << QMessageAccountFilter(oldFilter.d_ptr->_filterList[i][j]);
                    } else {
                        filter |= oldFilter.d_ptr->_filterList[i][j];
                    }
                }
            } else {
                QMessageAccountFilter tempFilter;
                for (int j=0; j < oldFilter.d_ptr->_filterList[i].count(); j++) {
                    if (j == 0) {
                        tempFilter = oldFilter.d_ptr->_filterList[i][j];
                    } else {
                        tempFilter |= oldFilter.d_ptr->_filterList[i][j];
                    }
                }
                filter &= tempFilter;
            }
        }

        for (int i=0; i < filter.d_ptr->_filterList.count(); i++) {
            qSort(filter.d_ptr->_filterList[i].begin(), filter.d_ptr->_filterList[i].end(), QMessageAccountFilterPrivate::lessThan);
        }
    }
}

void QMessageAccountFilterPrivate::applyNot(QMessageAccountFilter& filter)
{
    QMessageAccountFilterPrivate::changeComparatorValuesToOpposite(filter);
    QMessageAccountFilterPrivate::changeANDsAndORsToOpposite(filter);
}

bool QMessageAccountFilterPrivate::filter(const QMessageAccount &messageAccount, const QMessageAccountFilterPrivate &filter)
{
    if ((filter._field == QMessageAccountFilterPrivate::None) &&
        (filter._filterList.count() == 0)) {
        if (filter._notFilter) {
            return false;
        } else {
            return true;
        }
    }

    Qt::CaseSensitivity caseSensitivity = (filter._matchFlags & QMessageDataComparator::MatchCaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;

    switch (filter._field) {
    case QMessageAccountFilterPrivate::Id:
        {
        if (filter._comparatorType == QMessageAccountFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                                if (messageAccount.id().toString() == filter._value.toString()) {
                    return true;
                }
            } else { // NotEqual
                                if (!(messageAccount.id().toString() == filter._value.toString())) {
                    return true;
                }
            }
        } else if (filter._comparatorType == QMessageAccountFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if (filter._ids.contains(messageAccount.id())) {
                    return true;
                }
            } else { // Excludes
                if (!filter._ids.contains(messageAccount.id())) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageAccountFilterPrivate::Name:
        {
        if (filter._comparatorType == QMessageAccountFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                                if (messageAccount.name().compare(filter._value.toString(),caseSensitivity) == 0) {
                    return true;
                }
            } else { // NotEqual
                                if (messageAccount.name().compare(filter._value.toString(),caseSensitivity) != 0) {
                    return true;
                }
            }
        } else if (filter._comparatorType == QMessageAccountFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if (messageAccount.name().contains(filter._value.toString(),caseSensitivity)) {
                    return true;
                }
            } else { // Excludes
            if (!messageAccount.name().contains(filter._value.toString(),caseSensitivity)) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageAccountFilterPrivate::None:
        break;
    }

    return false;
}

bool QMessageAccountFilterPrivate::filter(const QMessageAccount &messageAccount) const
{
    if (!_valid) {
        return false;
    }

    bool result = false;
    if (_filterList.count() == 0) {
        result = QMessageAccountFilterPrivate::filter(messageAccount, *this);
    } else {
        for (int i=0; i < _filterList.count(); i++) {
            for (int j=0; j < _filterList[i].count(); j++) {
                result = QMessageAccountFilterPrivate::filter(messageAccount, *_filterList[i][j].d_ptr);
                if (result == false) {
                    break;
                }
            }
            if (result == true) {
                break;
            }
        }
    }
    return result;
}

QMessageAccountFilterPrivate* QMessageAccountFilterPrivate::implementation(const QMessageAccountFilter &filter)

{
    return filter.d_ptr;
}

QMessageAccountFilter::QMessageAccountFilter()
 : d_ptr(new QMessageAccountFilterPrivate(this))
{
        d_ptr->_matchFlags = 0;

        d_ptr->_valid = true; // Empty filter is valid
        d_ptr->_notFilter = false;
        d_ptr->_ids = QMessageAccountIdList();
        d_ptr->_value = QVariant();
        d_ptr->_field = QMessageAccountFilterPrivate::None;
        d_ptr->_comparatorType = QMessageAccountFilterPrivate::Equality;
        d_ptr->_comparatorValue = 0;
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
    if (&other == this)
        return *this;

    d_ptr->_matchFlags = other.d_ptr->_matchFlags;

    d_ptr->_valid = other.d_ptr->_valid;
    d_ptr->_notFilter = other.d_ptr->_notFilter;
    d_ptr->_ids = other.d_ptr->_ids;
    d_ptr->_value = other.d_ptr->_value;
    d_ptr->_field = other.d_ptr->_field;
    d_ptr->_comparatorType = other.d_ptr->_comparatorType;
    d_ptr->_comparatorValue = other.d_ptr->_comparatorValue;
    d_ptr->_filterList = other.d_ptr->_filterList;

    return *this;
}

void QMessageAccountFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
{
    d_ptr->_matchFlags = matchFlags;
}

QMessageDataComparator::MatchFlags QMessageAccountFilter::matchFlags() const
{
    return d_ptr->_matchFlags;
}

bool QMessageAccountFilter::isEmpty() const
{
    return ((d_ptr->_field == QMessageAccountFilterPrivate::None) &&
                (d_ptr->_notFilter == false) &&
            (d_ptr->_filterList.count()) == 0);
}

bool QMessageAccountFilter::isSupported() const
{
    return d_ptr->_valid;
}

QMessageAccountFilter QMessageAccountFilter::operator~() const
{
    QMessageAccountFilter result(*this);
    if (result.isEmpty()) {
                result.d_ptr->_notFilter = true;
    } else {
                if (result.d_ptr->_notFilter) {
                        result.d_ptr->_notFilter = false;
                } else {
                        QMessageAccountFilterPrivate::applyNot(result);
                }
    }
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
    if (&other == this) {
        return *this;
    }

    if (isEmpty()) {
        *this = other;
        return *this;
    }

    if (other.isEmpty()) {
        return *this;
    }

        if (d_ptr->_notFilter) {
                return *this;
        }

        if (other.d_ptr->_notFilter) {
                *this = other;
                return *this;
        }

    if (d_ptr->_filterList.count() == 0) {
        QMessageAccountFilter newFilter = QMessageAccountFilter(*this);
        d_ptr->_filterList.append(QMessageAccountFilterPrivate::SortedMessageAccountFilterList());
        d_ptr->_filterList[0] << newFilter;
        d_ptr->_value = QVariant();
        d_ptr->_field = QMessageAccountFilterPrivate::None;
        d_ptr->_comparatorType = QMessageAccountFilterPrivate::Equality;
        d_ptr->_comparatorValue = 0;
    }
    int i = 0;
    while (i < d_ptr->_filterList.count()) {
        if (other.d_ptr->_filterList.count() == 0) {
            d_ptr->_filterList[i] << other;
            qSort(d_ptr->_filterList[i].begin(), d_ptr->_filterList[i].end(), QMessageAccountFilterPrivate::lessThan);
        } else {
            int j = 0;
            int k = i;
            while (j < other.d_ptr->_filterList.count()) {
                if (j+1 < other.d_ptr->_filterList.count()) {
                    d_ptr->_filterList.insert(k+j+1,QMessageAccountFilterPrivate::SortedMessageAccountFilterList());
                    d_ptr->_filterList[k+j+1] << d_ptr->_filterList[k+j];
                    i++;
                }
                d_ptr->_filterList[k+j] << other.d_ptr->_filterList[j];
                if (d_ptr->_filterList[k+j].count() > 1) {
                    qSort(d_ptr->_filterList[k+j].begin(), d_ptr->_filterList[k+j].end(), QMessageAccountFilterPrivate::lessThan);
                }
                j++;
            }
        }
        i++;
    }
    d_ptr->_valid = d_ptr->_valid & other.d_ptr->_valid;

    return *this;
}

const QMessageAccountFilter& QMessageAccountFilter::operator|=(const QMessageAccountFilter& other)
{
    if (&other == this) {
        return *this;
    }

    if (isEmpty()) {
        return *this;
    }

    if (other.isEmpty()) {
        *this = other;
        return *this;
    }

        if (d_ptr->_notFilter) {
                *this = other;
                return *this;
        }

        if (other.d_ptr->_notFilter) {
                return *this;
        }

    if (d_ptr->_filterList.count() == 0) {
        QMessageAccountFilter newFilter = QMessageAccountFilter(*this);
        d_ptr->_filterList.append(QMessageAccountFilterPrivate::SortedMessageAccountFilterList());
        d_ptr->_filterList[d_ptr->_filterList.count()-1] << newFilter;
        d_ptr->_value = QVariant();
        d_ptr->_field = QMessageAccountFilterPrivate::None;
        d_ptr->_comparatorType = QMessageAccountFilterPrivate::Equality;
        d_ptr->_comparatorValue = 0;
    }
    if (other.d_ptr->_filterList.count() == 0) {
        d_ptr->_filterList.append(QMessageAccountFilterPrivate::SortedMessageAccountFilterList());
        d_ptr->_filterList[d_ptr->_filterList.count()-1] << other;
    } else {
        d_ptr->_filterList << other.d_ptr->_filterList;
    }
    d_ptr->_valid = d_ptr->_valid & other.d_ptr->_valid;

    return *this;
}

bool QMessageAccountFilter::operator==(const QMessageAccountFilter& other) const
{
    if (d_ptr->_filterList.count() != other.d_ptr->_filterList.count()) {
        return false;
    }

    if (d_ptr->_filterList.count() == 0) {
        return (d_ptr->_notFilter == other.d_ptr->_notFilter &&
                        d_ptr->_field == other.d_ptr->_field &&
                d_ptr->_value == other.d_ptr->_value &&
                d_ptr->_ids == other.d_ptr->_ids &&
                d_ptr->_comparatorType == other.d_ptr->_comparatorType &&
                d_ptr->_comparatorValue == other.d_ptr->_comparatorValue);
    } else { // List
        for (int i=0; i < d_ptr->_filterList.count(); i++) {
            if (d_ptr->_filterList[i].count() != other.d_ptr->_filterList[i].count()) {
                return false;
            }
            for (int j=0; j < d_ptr->_filterList[i].count(); j++) {
                if (!(d_ptr->_filterList[i][j] == other.d_ptr->_filterList[i][j])) {
                    return false;
                }
            }
        }
    }

    return true;
}

QMessageAccountFilter QMessageAccountFilter::byId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageAccountFilter result;
    result.d_ptr->_field = QMessageAccountFilterPrivate::Id;
    result.d_ptr->_value = id.toString();
    result.d_ptr->_comparatorType = QMessageAccountFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageAccountFilter QMessageAccountFilter::byId(const QMessageAccountIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageAccountFilter result;
    result.d_ptr->_field = QMessageAccountFilterPrivate::Id;
    result.d_ptr->_ids = ids;
    result.d_ptr->_comparatorType = QMessageAccountFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageAccountFilter QMessageAccountFilter::byId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageAccountFilter result;
    *result.d_ptr = *filter.d_ptr;
    if (cmp == QMessageDataComparator::Excludes) {
        // Invert the sense of comparison
        result = ~result;
    }
    return result;
}

QMessageAccountFilter QMessageAccountFilter::byName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageAccountFilter result;
    result.d_ptr->_field = QMessageAccountFilterPrivate::Name;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageAccountFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageAccountFilter QMessageAccountFilter::byName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageAccountFilter result;
    result.d_ptr->_field = QMessageAccountFilterPrivate::Name;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageAccountFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QTM_END_NAMESPACE
