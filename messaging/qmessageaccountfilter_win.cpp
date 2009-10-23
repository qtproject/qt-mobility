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

namespace {

void invertComparator(QMessageAccountFilterPrivate &f)
{
	// Invert the sense of comparison
    if ((f._criterion == QMessageAccountFilterPrivate::None) || 
        (f._criterion == QMessageAccountFilterPrivate::IdEquality) ||
        (f._criterion == QMessageAccountFilterPrivate::NameEquality)) {
        f._equality = (f._equality == QMessageDataComparator::Equal ? QMessageDataComparator::NotEqual : QMessageDataComparator::Equal);
    } else if ((f._criterion == QMessageAccountFilterPrivate::IdInclusion) ||
               (f._criterion == QMessageAccountFilterPrivate::NameInclusion)) {
        f._inclusion = (f._inclusion == QMessageDataComparator::Includes ? QMessageDataComparator::Excludes : QMessageDataComparator::Includes); 
    }
}

}

bool QMessageAccountFilterPrivate::matchesStore(const QMessageAccountFilter &filter, const MapiStorePtr &store)
{
    QMessageAccountFilterPrivate *f(filter.d_ptr);

    if ((f->_criterion == IdEquality) || (f->_criterion == IdInclusion)) {
        QMessageAccountId storeId(store->id());

        if (f->_criterion == IdEquality) {
            bool idEqual(storeId == *f->_ids.begin());
            return (f->_equality == QMessageDataComparator::Equal ? idEqual : !idEqual);
        } else {
            bool idPresent(f->_ids.contains(storeId));
            return (f->_inclusion == QMessageDataComparator::Includes ? idPresent : !idPresent);
        }
    } else if ((f->_criterion == NameEquality) || (f->_criterion == NameInclusion)) {
        QString storeName(store->name());

        if (f->_criterion == NameEquality) {
            // TODO: Apply options
            bool nameEqual(storeName == f->_name);
            return (f->_equality == QMessageDataComparator::Equal ? nameEqual : !nameEqual);
        } else {
            bool namePartialMatch(storeName.contains(f->_name));
            return (f->_inclusion == QMessageDataComparator::Includes ? namePartialMatch : !namePartialMatch);
        }
    } else { // None
        return (f->_equality == QMessageDataComparator::Equal);
    }
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
    return ((d_ptr->_criterion == QMessageAccountFilterPrivate::None) && 
            (d_ptr->_equality == QMessageDataComparator::Equal));
}

bool QMessageAccountFilter::isSupported() const
{
    return true;
}

QMessageAccountFilter QMessageAccountFilter::operator~() const
{
    QMessageAccountFilter result(*this);
    invertComparator(*result.d_ptr);
    return result;
}

QMessageAccountFilter QMessageAccountFilter::operator&(const QMessageAccountFilter& other) const
{
    // TODO: Not yet implemented
    Q_UNUSED(other)
    return QMessageAccountFilter(); // stub
}

QMessageAccountFilter QMessageAccountFilter::operator|(const QMessageAccountFilter& other) const
{
    // TODO: Not yet implemented
    Q_UNUSED(other)
    return QMessageAccountFilter(); // stub
}

const QMessageAccountFilter& QMessageAccountFilter::operator&=(const QMessageAccountFilter& other)
{
    *this = this->operator&(other);
    return *this;
}

const QMessageAccountFilter& QMessageAccountFilter::operator|=(const QMessageAccountFilter& other)
{
    *this = this->operator|(other);
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
        // Invert the sense of comparison
        invertComparator(*result.d_ptr);
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

