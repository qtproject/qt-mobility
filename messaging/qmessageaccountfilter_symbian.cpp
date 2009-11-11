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
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"

QMessageAccountFilterPrivate::QMessageAccountFilterPrivate(QMessageAccountFilter *accountFilter)
 : q_ptr(accountFilter)
{
}

QMessageAccountFilterPrivate::~QMessageAccountFilterPrivate()
{
}

bool QMessageAccountFilterPrivate::lessThan(const QMessageAccountFilter filter1, const QMessageAccountFilter filter2)
{
    return filter1.d_ptr->_field < filter2.d_ptr->_field;
}

void QMessageAccountFilterPrivate::applyNot(QMessageAccountFilter& filter)
{
    if (filter.d_ptr->_filterList.count() == 0) {
        if (filter.d_ptr->_comparatorType == QMessageAccountFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter.d_ptr->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
            } else {
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
            }
        } else { // QMessageFolderFilterPrivate::Inclusion
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
                QMessageAccountFilterPrivate::applyNot(filter.d_ptr->_filterList[i][j]);
            }
        }
    }
}

bool QMessageAccountFilterPrivate::filter(QMessageAccount &messageAccount)
{
    if (!_valid) {
        return false;
    }
    
    Qt::CaseSensitivity caseSensitivity = (_options & QMessageDataComparator::CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive; 
    
    switch (_field) {
    case QMessageAccountFilterPrivate::Id:
        {
        if (_comparatorType == QMessageAccountFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
				if (messageAccount.id().toString() == _value.toString()) {
                    return true;
                }
            } else { // NotEqual
				if (!(messageAccount.id().toString() == _value.toString())) {
                    return true;
                }
            }
        } else if (_comparatorType == QMessageAccountFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if (_ids.contains(messageAccount.id())) {
                    return true;
                }
            } else { // Excludes
                if (!_ids.contains(messageAccount.id())) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageAccountFilterPrivate::Name:
        {
        if (_comparatorType == QMessageAccountFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
				if (messageAccount.name().compare(_value.toString(),caseSensitivity) == 0) {
                    return true;
                }
            } else { // NotEqual
				if (messageAccount.name().compare(_value.toString(),caseSensitivity) != 0) {
                    return true;
                }
            }
        } else if (_comparatorType == QMessageAccountFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if (messageAccount.name().contains(_value.toString(),caseSensitivity)) {
                    return true;
                }
            } else { // Excludes
            if (!messageAccount.name().contains(_value.toString(),caseSensitivity)) {
                    return true;
                }
            }
        }
        break;
        }
    }

    return false;
}

QMessageAccountFilterPrivate* QMessageAccountFilterPrivate::implementation(const QMessageAccountFilter &filter)

{
    return filter.d_ptr;
}

QMessageAccountFilter::QMessageAccountFilter()
 : d_ptr(new QMessageAccountFilterPrivate(this))
{
	d_ptr->_options = 0;
	
	d_ptr->_valid = false;
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
    
    d_ptr->_options = other.d_ptr->_options;
    
    d_ptr->_valid = other.d_ptr->_valid;
    d_ptr->_ids = other.d_ptr->_ids;
    d_ptr->_value = other.d_ptr->_value;
    d_ptr->_field = other.d_ptr->_field;
    d_ptr->_comparatorType = other.d_ptr->_comparatorType;
    d_ptr->_comparatorValue = other.d_ptr->_comparatorValue;
    d_ptr->_filterList = other.d_ptr->_filterList;
    
    return *this;
}

void QMessageAccountFilter::setOptions(QMessageDataComparator::Options options)
{
    d_ptr->_options = options;
}

QMessageDataComparator::Options QMessageAccountFilter::options() const
{
    return d_ptr->_options;
}

bool QMessageAccountFilter::isEmpty() const
{
    return ((d_ptr->_field == QMessageAccountFilterPrivate::None) && 
            (d_ptr->_filterList.count()) == 0);
}

bool QMessageAccountFilter::isSupported() const
{
    return d_ptr->_valid;
}

QMessageAccountFilter QMessageAccountFilter::operator~() const
{
    QMessageAccountFilter result(*this);
    QMessageAccountFilterPrivate::applyNot(result);
    return result;
}

QMessageAccountFilter QMessageAccountFilter::operator&(const QMessageAccountFilter& other) const
{
    if (isEmpty()) {
        return QMessageAccountFilter(other);
    }
    
    if (other.isEmpty()) {
        return QMessageAccountFilter(*this);
    }
    
    QMessageAccountFilter result;
    result.d_ptr->_filterList = d_ptr->_filterList;
    if (result.d_ptr->_filterList.count() == 0) {
        result.d_ptr->_filterList.append(QMessageAccountFilterPrivate::SortedMessageAccountFilterList());
        result.d_ptr->_filterList[0] << *this;
    }
    for (int i=0; i < result.d_ptr->_filterList.count(); i++) {
        if (other.d_ptr->_filterList.count() == 0) {
            result.d_ptr->_filterList[i] << other;
        } else {
            for (int j=0; j < other.d_ptr->_filterList.count(); j++) {
                result.d_ptr->_filterList[i] << other.d_ptr->_filterList[j];
            }
        }
        qSort(result.d_ptr->_filterList[i].begin(), result.d_ptr->_filterList[i].end(), QMessageAccountFilterPrivate::lessThan);
    }
    result.d_ptr->_valid = d_ptr->_valid & other.d_ptr->_valid;
    
    return result;
}

QMessageAccountFilter QMessageAccountFilter::operator|(const QMessageAccountFilter& other) const
{
    if (isEmpty()) {
        return QMessageAccountFilter(*this);
    }
    
    if (other.isEmpty()) {
        return QMessageAccountFilter(other);
    }
    
    QMessageAccountFilter result;
    if (d_ptr->_filterList.count() == 0) {
        result.d_ptr->_filterList.append(QMessageAccountFilterPrivate::SortedMessageAccountFilterList());
        result.d_ptr->_filterList[0] << *this;
    } else {
        result.d_ptr->_filterList = d_ptr->_filterList;
    }
    if (other.d_ptr->_filterList.count() == 0) {
        result.d_ptr->_filterList.append(QMessageAccountFilterPrivate::SortedMessageAccountFilterList());
        result.d_ptr->_filterList[result.d_ptr->_filterList.count()-1] << other;
    } else {
        result.d_ptr->_filterList << other.d_ptr->_filterList;
    }
    result.d_ptr->_valid = d_ptr->_valid & other.d_ptr->_valid;
    
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
    
    if (d_ptr->_filterList.count() == 0) {
        d_ptr->_filterList.append(QMessageAccountFilterPrivate::SortedMessageAccountFilterList());
        d_ptr->_filterList[0] << QMessageAccountFilter(*this);
    }
    for (int i=0; i < d_ptr->_filterList.count(); i++) {
        if (other.d_ptr->_filterList.count() == 0) {
            d_ptr->_filterList[i] << other;
        } else {
            for (int j=0; j < other.d_ptr->_filterList.count(); j++) {
                d_ptr->_filterList[i] << other.d_ptr->_filterList[j];
            }
        }
        qSort(d_ptr->_filterList[i].begin(), d_ptr->_filterList[i].end(), QMessageAccountFilterPrivate::lessThan);
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
        return (d_ptr->_field == other.d_ptr->_field &&
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
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    //TODO:
    return QMessageAccountFilter();
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
