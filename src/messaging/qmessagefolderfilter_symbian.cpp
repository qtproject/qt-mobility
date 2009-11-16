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
#include "qmessagefolderfilter.h"
#include "qmessagefolderfilter_p.h"

QMessageFolderFilterPrivate::QMessageFolderFilterPrivate(QMessageFolderFilter *folderFilter)
 : q_ptr(folderFilter)
{
}

QMessageFolderFilterPrivate::~QMessageFolderFilterPrivate()
{
}

bool QMessageFolderFilterPrivate::lessThan(const QMessageFolderFilter filter1, const QMessageFolderFilter filter2)
{
    return filter1.d_ptr->_field < filter2.d_ptr->_field;
}

void QMessageFolderFilterPrivate::applyNot(QMessageFolderFilter& filter)
{
    if (filter.d_ptr->_filterList.count() == 0) {
        if (filter.d_ptr->_comparatorType == QMessageFolderFilterPrivate::Equality) {
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
                QMessageFolderFilterPrivate::applyNot(filter.d_ptr->_filterList[i][j]);
            }
        }
    }
}

bool QMessageFolderFilterPrivate::filter(QMessageFolder &messageFolder)
{
    if (!_valid) {
        return false;
    }
    
    bool retVal = false;
    
    Qt::CaseSensitivity caseSensitivity = (_options & QMessageDataComparator::CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive; 
    
    switch (_field) {
    case QMessageFolderFilterPrivate::Id:
        {
        if (_comparatorType == QMessageFolderFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (messageFolder.id().toString() == _value.toString()) {
                    retVal = true;
                }
            } else { // NotEqual
                if (!(messageFolder.id().toString() == _value.toString())) {
                    retVal = true;
                }
            }
        } else if (_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            if (_ids.count() > 0) { // QMessageIdList
                if (cmp == QMessageDataComparator::Includes) {
                    if (_ids.contains(messageFolder.id())) {
                        retVal = true;
                    }
                } else { // Excludes
                    if (!_ids.contains(messageFolder.id())) {
                        retVal = true;
                    }
                }
            } else { // QMessageFilter
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    // TODO:
                }
            }
        }
        break;
        }
    case QMessageFolderFilterPrivate::DisplayName:
        {
        if (_comparatorType == QMessageFolderFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (messageFolder.displayName().compare(_value.toString(),caseSensitivity) == 0) {
                    retVal = true;
                }
            } else { // NotEqual
                if (messageFolder.displayName().compare(_value.toString(),caseSensitivity) != 0) {
                    retVal = true;
                }
            }
        } else if (_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if (messageFolder.displayName().contains(_value.toString(),caseSensitivity)) {
                    retVal = true;
                }
            } else { // Excludes
            if (!messageFolder.displayName().contains(_value.toString(),caseSensitivity)) {
                    retVal = true;
                }
            }
        }
        break;
        }
    case QMessageFolderFilterPrivate::Path:
        {
        if (_comparatorType == QMessageFolderFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (messageFolder.path().compare(_value.toString(),caseSensitivity) == 0) {
                    retVal = true;
                }
            } else { // NotEqual
                if (messageFolder.path().compare(_value.toString(),caseSensitivity) != 0) {
                    retVal = true;
                }
            }
        } else if (_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if (messageFolder.path().contains(_value.toString(),caseSensitivity)) {
                    retVal = true;
                }
            } else { // Excludes
            if (!messageFolder.path().contains(_value.toString(),caseSensitivity)) {
                    retVal = true;
                }
            }
        }
        break;
        }
    case QMessageFolderFilterPrivate::ParentAccountId:
        {
        if (_comparatorType == QMessageFolderFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (messageFolder.parentAccountId().toString() == _value.toString()) {
                    retVal = true;
                }
            } else { // NotEqual
                if (!(messageFolder.parentAccountId().toString() == _value.toString())) {
                    retVal = true;
                }
            }
        } else if (_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                // TODO:
            } else { // Excludes
                // TODO:
            }
        }
        break;
        }
    case QMessageFolderFilterPrivate::ParentFolderId:
        {
        if (_comparatorType == QMessageFolderFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (messageFolder.parentFolderId().toString() == _value.toString()) {
                    retVal = true;
                }
            } else { // NotEqual
                if (!(messageFolder.parentFolderId().toString() == _value.toString())) {
                    retVal = true;
                }
            }
        } else if (_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                // TODO:
            } else { // Excludes
                // TODO:
            }
        }
        break;
        }
    case QMessageFolderFilterPrivate::AncestorFolderIds:
        {
        if (_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            if (!_value.isNull()) { // QMessageFolderId
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    // TODO:
                }
            } else { // QMessageFolderFilter
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    // TODO:
                }
            }
        }
        break;
        }
    }
    
    return retVal;
}

QMessageFolderFilterPrivate* QMessageFolderFilterPrivate::implementation(const QMessageFolderFilter &filter)

{
    return filter.d_ptr;
}

QMessageFolderFilter::QMessageFolderFilter()
 : d_ptr(new QMessageFolderFilterPrivate(this))
{
    d_ptr->_options = 0;
    
    d_ptr->_valid = false;
    d_ptr->_ids = QMessageFolderIdList();
    d_ptr->_value = QVariant();
    d_ptr->_field = QMessageFolderFilterPrivate::None;
    d_ptr->_comparatorType = QMessageFolderFilterPrivate::Equality;
    d_ptr->_comparatorValue = 0;
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

void QMessageFolderFilter::setOptions(QMessageDataComparator::Options options)
{
    d_ptr->_options = options;
}

QMessageDataComparator::Options QMessageFolderFilter::options() const
{
    return d_ptr->_options;
}

bool QMessageFolderFilter::isEmpty() const
{
    return ((d_ptr->_field == QMessageFolderFilterPrivate::None) && 
            (d_ptr->_filterList.count()) == 0);
}

bool QMessageFolderFilter::isSupported() const
{
    return d_ptr->_valid;
}

QMessageFolderFilter QMessageFolderFilter::operator~() const
{
    QMessageFolderFilter result(*this);
    QMessageFolderFilterPrivate::applyNot(result);
    return result;
}

QMessageFolderFilter QMessageFolderFilter::operator&(const QMessageFolderFilter& other) const
{
    if (isEmpty()) {
        return QMessageFolderFilter(other);
    }
    
    if (other.isEmpty()) {
        return QMessageFolderFilter(*this);
    }
    
    QMessageFolderFilter result;
    result.d_ptr->_filterList = d_ptr->_filterList;
    if (result.d_ptr->_filterList.count() == 0) {
        result.d_ptr->_filterList.append(QMessageFolderFilterPrivate::SortedMessageFolderFilterList());
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
        qSort(result.d_ptr->_filterList[i].begin(), result.d_ptr->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
    }
    result.d_ptr->_valid = d_ptr->_valid & other.d_ptr->_valid;
    
    return result;
}

QMessageFolderFilter QMessageFolderFilter::operator|(const QMessageFolderFilter& other) const
{
    if (isEmpty()) {
        return QMessageFolderFilter(*this);
    }
    
    if (other.isEmpty()) {
        return QMessageFolderFilter(other);
    }
    
    QMessageFolderFilter result;
    if (d_ptr->_filterList.count() == 0) {
        result.d_ptr->_filterList.append(QMessageFolderFilterPrivate::SortedMessageFolderFilterList());
        result.d_ptr->_filterList[0] << *this;
    } else {
        result.d_ptr->_filterList = d_ptr->_filterList;
    }
    if (other.d_ptr->_filterList.count() == 0) {
        result.d_ptr->_filterList.append(QMessageFolderFilterPrivate::SortedMessageFolderFilterList());
        result.d_ptr->_filterList[result.d_ptr->_filterList.count()-1] << other;
    } else {
        result.d_ptr->_filterList << other.d_ptr->_filterList;
    }
    result.d_ptr->_valid = d_ptr->_valid & other.d_ptr->_valid;
    
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

    if (d_ptr->_filterList.count() == 0) {
        d_ptr->_filterList.append(QMessageFolderFilterPrivate::SortedMessageFolderFilterList());
        d_ptr->_filterList[0] << QMessageFolderFilter(*this);
    }
    for (int i=0; i < d_ptr->_filterList.count(); i++) {
        if (other.d_ptr->_filterList.count() == 0) {
            d_ptr->_filterList[i] << other;
        } else {
            for (int j=0; j < other.d_ptr->_filterList.count(); j++) {
                d_ptr->_filterList[i] << other.d_ptr->_filterList[j];
            }
        }
        qSort(d_ptr->_filterList[i].begin(), d_ptr->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
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
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byDisplayName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_field = QMessageFolderFilterPrivate::DisplayName;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFolderFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byDisplayName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_field = QMessageFolderFilterPrivate::DisplayName;
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
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_field = QMessageFolderFilterPrivate::ParentFolderId;
    result.d_ptr->_value = id.toString();
    result.d_ptr->_comparatorType = QMessageFolderFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentFolderId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_field = QMessageFolderFilterPrivate::AncestorFolderIds;
    result.d_ptr->_value = id.toString();
    result.d_ptr->_comparatorType = QMessageFolderFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}
