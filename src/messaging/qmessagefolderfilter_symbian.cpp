/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessagefolderfilter.h"
#include "qmessagefolderfilter_p.h"

QTM_BEGIN_NAMESPACE

QMessageFolderFilterPrivate::QMessageFolderFilterPrivate(QMessageFolderFilter *folderFilter)
 : q_ptr(folderFilter)
{
}

QMessageFolderFilterPrivate::~QMessageFolderFilterPrivate()
{
}

bool QMessageFolderFilterPrivate::lessThan(const QMessageFolderFilter filter1, const QMessageFolderFilter filter2)
{
    if (filter1.d_ptr->_field == filter2.d_ptr->_field) {
        if (filter1.d_ptr->_comparatorType == filter2.d_ptr->_comparatorType) {
            return filter1.d_ptr->_comparatorValue < filter2.d_ptr->_comparatorValue;
        }
        return filter1.d_ptr->_comparatorType < filter2.d_ptr->_comparatorType;
    }
    return filter1.d_ptr->_field < filter2.d_ptr->_field;
}

void QMessageFolderFilterPrivate::changeComparatorValuesToOpposite(QMessageFolderFilter& filter)
{
    if (filter.d_ptr->_filterList.count() == 0) {
        if (filter.d_ptr->_comparatorType == QMessageFolderFilterPrivate::Equality) {
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
                QMessageFolderFilterPrivate::changeComparatorValuesToOpposite(filter.d_ptr->_filterList[i][j]);
            }
        }
    }
}

void QMessageFolderFilterPrivate::changeANDsAndORsToOpposite(QMessageFolderFilter& filter)
{
    if (filter.d_ptr->_filterList.count() > 0) {
        QMessageFolderFilter oldFilter = filter;
        filter.d_ptr->_filterList.clear();
        for (int i=0; i < oldFilter.d_ptr->_filterList.count(); i++) {
            if (i == 0) {
                for (int j=0; j < oldFilter.d_ptr->_filterList[i].count(); j++) {
                    if (j == 0) {
                        filter.d_ptr->_filterList.append(QMessageFolderFilterPrivate::SortedMessageFolderFilterList());
                        filter.d_ptr->_filterList[0] << QMessageFolderFilter(oldFilter.d_ptr->_filterList[i][j]);
                    } else {
                        filter |= oldFilter.d_ptr->_filterList[i][j];
                    }
                }
            } else {
                QMessageFolderFilter tempFilter;
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
            qSort(filter.d_ptr->_filterList[i].begin(), filter.d_ptr->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
        }
    }
}

void QMessageFolderFilterPrivate::applyNot(QMessageFolderFilter& filter)
{
    QMessageFolderFilterPrivate::changeComparatorValuesToOpposite(filter);
    QMessageFolderFilterPrivate::changeANDsAndORsToOpposite(filter);
}

bool QMessageFolderFilterPrivate::filter(const QMessageFolder &messageFolder, const QMessageFolderFilterPrivate &filter)
{
    if ((filter._field == QMessageFolderFilterPrivate::None) &&
        (filter._filterList.count() == 0)) {
        if (filter._notFilter) {
            return false;
        } else {
            return true;
        }
    }

    bool retVal = false;
    
    Qt::CaseSensitivity caseSensitivity = (filter._matchFlags & QMessageDataComparator::MatchCaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive; 
    
    switch (filter._field) {
    case QMessageFolderFilterPrivate::Id:
        {
        if (filter._comparatorType == QMessageFolderFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (messageFolder.id().toString() == filter._value.toString()) {
                    retVal = true;
                }
            } else { // NotEqual
                if (!(messageFolder.id().toString() == filter._value.toString())) {
                    retVal = true;
                }
            }
        } else if (filter._comparatorType == QMessageFolderFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (filter._ids.count() > 0) { // QMessageIdList
                if (cmp == QMessageDataComparator::Includes) {
                    if (filter._ids.contains(messageFolder.id())) {
                        retVal = true;
                    }
                } else { // Excludes
                    if (!filter._ids.contains(messageFolder.id())) {
                        retVal = true;
                    }
                }
            } else { // QMessageFilter
                if (cmp == QMessageDataComparator::Includes) {
                    // Not supported
                } else { // Excludes
					// Not supported
                }
            }
        }
        break;
        }
    case QMessageFolderFilterPrivate::Name:
        {
        if (filter._comparatorType == QMessageFolderFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (messageFolder.name().compare(filter._value.toString(),caseSensitivity) == 0) {
                    retVal = true;
                }
            } else { // NotEqual
                if (messageFolder.name().compare(filter._value.toString(),caseSensitivity) != 0) {
                    retVal = true;
                }
            }
        } else if (filter._comparatorType == QMessageFolderFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if (messageFolder.name().contains(filter._value.toString(),caseSensitivity)) {
                    retVal = true;
                }
            } else { // Excludes
                if (!messageFolder.name().contains(filter._value.toString(),caseSensitivity)) {
                    retVal = true;
                }
            }
        }
        break;
        }
    case QMessageFolderFilterPrivate::Path:
        {
        if (filter._comparatorType == QMessageFolderFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (messageFolder.path().compare(filter._value.toString(),caseSensitivity) == 0) {
                    retVal = true;
                }
            } else { // NotEqual
                if (messageFolder.path().compare(filter._value.toString(),caseSensitivity) != 0) {
                    retVal = true;
                }
            }
        } else if (filter._comparatorType == QMessageFolderFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if (messageFolder.path().contains(filter._value.toString(),caseSensitivity)) {
                    retVal = true;
                }
            } else { // Excludes
            if (!messageFolder.path().contains(filter._value.toString(),caseSensitivity)) {
                    retVal = true;
                }
            }
        }
        break;
        }
    case QMessageFolderFilterPrivate::ParentAccountId:
        {
        if (filter._comparatorType == QMessageFolderFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (messageFolder.parentAccountId().toString() == filter._value.toString()) {
                    retVal = true;
                }
            } else { // NotEqual
                if (!(messageFolder.parentAccountId().toString() == filter._value.toString())) {
                    retVal = true;
                }
            }
        } else if (filter._comparatorType == QMessageFolderFilterPrivate::Inclusion) { // QMessageAccountFilter
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
				// Not supported
            } else { // Excludes
				// Not supported
            }
        }
        break;
        }
    case QMessageFolderFilterPrivate::ParentFolderId:
        {
        if (filter._comparatorType == QMessageFolderFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (messageFolder.parentFolderId().toString() == filter._value.toString()) {
                    retVal = true;
                }
            } else { // NotEqual
                if (!(messageFolder.parentFolderId().toString() == filter._value.toString())) {
                    retVal = true;
                }
            }
        } else if (filter._comparatorType == QMessageFolderFilterPrivate::Inclusion) { // QMessageFolderFilter
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
				// Not supported
            } else { // Excludes
				// Not supported
            }
        }
        break;
        }
    case QMessageFolderFilterPrivate::AncestorFolderIds:
        {
        if (filter._comparatorType == QMessageFolderFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (!filter._value.isNull()) { // QMessageFolderId
                if (cmp == QMessageDataComparator::Includes) {
					if (messageFolder.parentFolderId().toString() == filter._value.toString()) {
						retVal = true;
					}
                } else { // Excludes
					if (!(messageFolder.parentFolderId().toString() == filter._value.toString())) {
						retVal = true;
					}
                }
            } else { // QMessageFolderFilter
                if (cmp == QMessageDataComparator::Includes) {
                    // Not supported
                } else { // Excludes
                    // Not supported
                }
            }
        }
        break;
        }
    case QMessageFolderFilterPrivate::ParentAccountIdFilter:
    case QMessageFolderFilterPrivate::None:
        break;
    }
    
    return retVal;
}

bool QMessageFolderFilterPrivate::filter(const QMessageFolder &messageFolder) const
{
    if (!_valid) {
        return false;
    }

    bool result = false;
    if (_filterList.count() == 0) {
        result = QMessageFolderFilterPrivate::filter(messageFolder, *this);
    } else {
        for (int i=0; i < _filterList.count(); i++) {
            for (int j=0; j < _filterList[i].count(); j++) {
                result = QMessageFolderFilterPrivate::filter(messageFolder, *_filterList[i][j].d_ptr);
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

QMessageFolderFilterPrivate* QMessageFolderFilterPrivate::implementation(const QMessageFolderFilter &filter)

{
    return filter.d_ptr;
}

QMessageFolderFilter::QMessageFolderFilter()
 : d_ptr(new QMessageFolderFilterPrivate(this))
{
    d_ptr->_matchFlags = 0;
    
    d_ptr->_valid = true; // Empty filter is valid
    d_ptr->_notFilter = false;
    d_ptr->_ids = QMessageFolderIdList();
    d_ptr->_value = QVariant();
    d_ptr->_field = QMessageFolderFilterPrivate::None;
    d_ptr->_comparatorType = QMessageFolderFilterPrivate::Equality;
    d_ptr->_comparatorValue = 0;
    d_ptr->_accountFilter = 0;
}

QMessageFolderFilter::QMessageFolderFilter(const QMessageFolderFilter &other)
 : d_ptr(new QMessageFolderFilterPrivate(this))
{
    d_ptr->_accountFilter = 0;
    this->operator=(other);
}

QMessageFolderFilter::~QMessageFolderFilter()
{
    delete d_ptr->_accountFilter;
    d_ptr->_accountFilter = 0;

    delete d_ptr;
}

QMessageFolderFilter& QMessageFolderFilter::operator=(const QMessageFolderFilter& other)
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
    
    delete d_ptr->_accountFilter;
    d_ptr->_accountFilter = 0;
    if (other.d_ptr->_accountFilter) {
        d_ptr->_accountFilter = new QMessageAccountFilter(*other.d_ptr->_accountFilter);
    }
    
    return *this;
}

void QMessageFolderFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
{
    d_ptr->_matchFlags = matchFlags;
}

QMessageDataComparator::MatchFlags QMessageFolderFilter::matchFlags() const
{
    return d_ptr->_matchFlags;
}

bool QMessageFolderFilter::isEmpty() const
{
    return ((d_ptr->_field == QMessageFolderFilterPrivate::None) &&
    		(d_ptr->_notFilter == false) && 
            (d_ptr->_filterList.count()) == 0);
}

bool QMessageFolderFilter::isSupported() const
{
    return d_ptr->_valid;
}

QMessageFolderFilter QMessageFolderFilter::operator~() const
{
    QMessageFolderFilter result(*this);
    if (result.isEmpty()) {
		result.d_ptr->_notFilter = true;
    } else {
		if (result.d_ptr->_notFilter) {
			result.d_ptr->_notFilter = false;
		} else {
			QMessageFolderFilterPrivate::applyNot(result);
		}
    }
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
        QMessageFolderFilter newFilter = QMessageFolderFilter(*this); 
        d_ptr->_filterList.append(QMessageFolderFilterPrivate::SortedMessageFolderFilterList());
        d_ptr->_filterList[0] << newFilter;
        d_ptr->_value = QVariant();
        d_ptr->_field = QMessageFolderFilterPrivate::None;
        d_ptr->_comparatorType = QMessageFolderFilterPrivate::Equality;
        d_ptr->_comparatorValue = 0;
    }
    int i = 0;
    while (i < d_ptr->_filterList.count()) {
        if (other.d_ptr->_filterList.count() == 0) {
            d_ptr->_filterList[i] << other;
            qSort(d_ptr->_filterList[i].begin(), d_ptr->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
        } else {
            int j = 0;
            int k = i;
            while (j < other.d_ptr->_filterList.count()) {
                if (j+1 < other.d_ptr->_filterList.count()) {
                    d_ptr->_filterList.insert(k+j+1,QMessageFolderFilterPrivate::SortedMessageFolderFilterList());
                    d_ptr->_filterList[k+j+1] << d_ptr->_filterList[k+j];
                    i++;
                }
                d_ptr->_filterList[k+j] << other.d_ptr->_filterList[j];
                if (d_ptr->_filterList[k+j].count() > 1) {
                    qSort(d_ptr->_filterList[k+j].begin(), d_ptr->_filterList[k+j].end(), QMessageFolderFilterPrivate::lessThan);
                }
                j++;
            }
        }
        i++;
    }
    d_ptr->_valid = d_ptr->_valid & other.d_ptr->_valid;

    return *this;
}

const QMessageFolderFilter& QMessageFolderFilter::operator|=(const QMessageFolderFilter& other)
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
        QMessageFolderFilter newFilter = QMessageFolderFilter(*this);
        d_ptr->_filterList.append(QMessageFolderFilterPrivate::SortedMessageFolderFilterList());
        d_ptr->_filterList[d_ptr->_filterList.count()-1] << newFilter;
        d_ptr->_value = QVariant();
        d_ptr->_field = QMessageFolderFilterPrivate::None;
        d_ptr->_comparatorType = QMessageFolderFilterPrivate::Equality;
        d_ptr->_comparatorValue = 0;
    } 
    if (other.d_ptr->_filterList.count() == 0) {
        d_ptr->_filterList.append(QMessageFolderFilterPrivate::SortedMessageFolderFilterList());
        d_ptr->_filterList[d_ptr->_filterList.count()-1] << other;
    } else {
        d_ptr->_filterList << other.d_ptr->_filterList;
    }
    d_ptr->_valid = d_ptr->_valid & other.d_ptr->_valid;

    return *this;
}

bool QMessageFolderFilter::operator==(const QMessageFolderFilter& other) const
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
    
    if (d_ptr->_accountFilter || other.d_ptr->_accountFilter) {
        if (!d_ptr->_accountFilter || !other.d_ptr->_accountFilter || (*d_ptr->_accountFilter != *other.d_ptr->_accountFilter)) {
            return false;
        }
    }
    
    return true;
}

QMessageFolderFilter QMessageFolderFilter::byId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_field = QMessageFolderFilterPrivate::Id;
    result.d_ptr->_value = id.toString();
    result.d_ptr->_comparatorType = QMessageFolderFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byId(const QMessageFolderIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_field = QMessageFolderFilterPrivate::Id;
    result.d_ptr->_ids = ids;
    result.d_ptr->_comparatorType = QMessageFolderFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
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
    result.d_ptr->_field = QMessageFolderFilterPrivate::Name;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFolderFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_field = QMessageFolderFilterPrivate::Name;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFolderFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byPath(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_field = QMessageFolderFilterPrivate::Path;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFolderFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byPath(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_field = QMessageFolderFilterPrivate::Path;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFolderFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_field = QMessageFolderFilterPrivate::ParentAccountId;
    result.d_ptr->_value = id.toString();
    result.d_ptr->_comparatorType = QMessageFolderFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentAccountId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_field = QMessageFolderFilterPrivate::ParentAccountIdFilter;
    result.d_ptr->_accountFilter = new QMessageAccountFilter(filter);
    result.d_ptr->_comparatorType = QMessageFolderFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    QMessageFolderFilter result;
    result.d_ptr->_valid = false;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentFolderId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    QMessageFolderFilter result;
    result.d_ptr->_valid = false;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    QMessageFolderFilter result;
    result.d_ptr->_valid = false;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    QMessageFolderFilter result;
    result.d_ptr->_valid = false;
    return result;
}

QTM_END_NAMESPACE
