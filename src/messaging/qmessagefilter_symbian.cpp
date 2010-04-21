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
#include "qmessagefilter.h"
#include "qmessagefilter_p.h"

QTM_BEGIN_NAMESPACE

QMessageFilterPrivate::QMessageFilterPrivate(QMessageFilter *messageFilter)
 : q_ptr(messageFilter),
   _valid(true),
   _field(None),
   _comparatorType(Equality),
   _comparatorValue(QMessageDataComparator::Equal)
{
}

QMessageFilterPrivate::~QMessageFilterPrivate()
{
}

bool QMessageFilterPrivate::lessThan(const QMessageFilter filter1, const QMessageFilter filter2)
{
    if (filter1.d_ptr->_field == filter2.d_ptr->_field) {
        if (filter1.d_ptr->_comparatorType == filter2.d_ptr->_comparatorType) {
            return filter1.d_ptr->_comparatorValue < filter2.d_ptr->_comparatorValue;
        }
        return filter1.d_ptr->_comparatorType < filter2.d_ptr->_comparatorType;
    }
    return filter1.d_ptr->_field < filter2.d_ptr->_field;
}

void QMessageFilterPrivate::changeComparatorValuesToOpposite(QMessageFilter& filter)
{
    if (filter.d_ptr->_filterList.count() == 0) {
        if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter.d_ptr->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
            } else {
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
            }
        } else if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Relation) {
            QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(filter.d_ptr->_comparatorValue));
            if (cmp == QMessageDataComparator::LessThan) {
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::GreaterThanEqual);
            } else if (cmp == QMessageDataComparator::LessThanEqual) {
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::GreaterThan);
            } else if (cmp == QMessageDataComparator::GreaterThan) {
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::LessThanEqual);
            } else { // GreaterThanEqual
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::LessThan);
            }
        } else { // Inclusion
            if (filter.d_ptr->_field == QMessageFilterPrivate::Status) {
                if (!filter.d_ptr->_value.isNull()) {
                    filter.d_ptr->_notFilterForComparator = !filter.d_ptr->_notFilterForComparator;
                } else {
                    filter.d_ptr->_notFilter = !filter.d_ptr->_notFilter;
                }
            } else {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter.d_ptr->_comparatorValue));
                if (cmp == QMessageDataComparator::Includes) {
                    filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::Excludes);
                } else { // Excludes
                    filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::Includes);
                }
            }
        }
    } else {
        for (int i=0; i < filter.d_ptr->_filterList.count(); i++) {
            for (int j=0; j < filter.d_ptr->_filterList[i].count(); j++) {
                QMessageFilterPrivate::changeComparatorValuesToOpposite(filter.d_ptr->_filterList[i][j]);
            }
        }
    }
}

void QMessageFilterPrivate::changeANDsAndORsToOpposite(QMessageFilter& filter)
{
    if (filter.d_ptr->_filterList.count() > 0) {
        QMessageFilter oldFilter = filter;
        filter.d_ptr->_filterList.clear();
        for (int i=0; i < oldFilter.d_ptr->_filterList.count(); i++) {
            if (i == 0) {
                for (int j=0; j < oldFilter.d_ptr->_filterList[i].count(); j++) {
                    if (j == 0) {
                        filter.d_ptr->_filterList.append(QMessageFilterPrivate::SortedMessageFilterList());
                        filter.d_ptr->_filterList[0] << QMessageFilter(oldFilter.d_ptr->_filterList[i][j]);
                    } else {
                        filter |= oldFilter.d_ptr->_filterList[i][j];
                    }
                }
            } else {
                QMessageFilter tempFilter;
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
            qSort(filter.d_ptr->_filterList[i].begin(), filter.d_ptr->_filterList[i].end(), QMessageFilterPrivate::lessThan);
        }
    }
}

void QMessageFilterPrivate::applyNot(QMessageFilter& filter)
{
    QMessageFilterPrivate::changeComparatorValuesToOpposite(filter);
    QMessageFilterPrivate::changeANDsAndORsToOpposite(filter);
}

bool QMessageFilterPrivate::filter(const QMessage &message, const QMessageFilterPrivate &filter)
{
    if ((filter._field == QMessageFilterPrivate::None) &&
        (filter._filterList.count() == 0)) {
        if (filter._notFilter) {
            return false;
        } else {
            return true;
        }
    }

    Qt::CaseSensitivity caseSensitivity = (filter._matchFlags & QMessageDataComparator::MatchCaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;

    switch (filter._field) {
    case QMessageFilterPrivate::Id:
        {
        if (filter._comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.id().toString() == filter._value.toString()) {
                    return true;
                }
            } else { // NotEqual
                if (message.id().toString() != filter._value.toString()) {
                    return true;
                }
            }
        } else if (filter._comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (filter._ids.count() > 0) { // QMessageIdList
               if (cmp == QMessageDataComparator::Includes) {
                   if (filter._ids.contains(message.id())) {
                       return true;
                   }
               } else { // Excludes
                   if (!filter._ids.contains(message.id())) {
                       return true;
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
    case QMessageFilterPrivate::Type:
        {
        QMessage::Type type = static_cast<QMessage::Type>(filter._value.toInt());
        if (filter._comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.type() == type) {
                    return true;
                }
            } else { // NotEqual
                if (message.type() != type) {
                    return true;
                }
            }
        } else if (filter._comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if ((message.type() & type) == message.type()) {
                    return true;
                }
            } else { // Excludes
            if ((message.type() & type) == 0) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::Sender:
        {
        if (filter._comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (filter._value.toString().length() > 0) {
                    if (message.from().addressee().compare(filter._value.toString(),caseSensitivity) == 0) {
                        return true;
                    }
                }
            } else { // NotEqual
                if (filter._value.toString().length() > 0) {
                    if (message.from().addressee().compare(filter._value.toString(),caseSensitivity) != 0) {
                        return true;
                    }
                } else {
                    return true;
                }
            }
        } else if (filter._comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if (message.from().addressee().contains(filter._value.toString(),caseSensitivity)) {
                    return true;
                }
            } else { // Excludes
            if (!message.from().addressee().contains(filter._value.toString(),caseSensitivity)) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::Recipients: // to, cc & bcc fields
        {
        if (filter._comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            bool includes = false;
            // Check to addresses
            QMessageAddressList addrList = message.to();
            for (int i=0; i < addrList.count(); i++) {
                if (addrList[i].addressee().contains(filter._value.toString(),caseSensitivity)) {
                    includes = true;
                    break;
                }
            }
            if (!includes) {
                // Check cc addresses
                addrList = message.cc();
                for (int i=0; i < addrList.count(); i++) {
                    if (addrList[i].addressee().contains(filter._value.toString(),caseSensitivity)) {
                        includes = true;
                        break;
                    }
                }
            }
            if (!includes) {
                // Check bcc addresses
                addrList = message.bcc();
                for (int i=0; i < addrList.count(); i++) {
                    if (addrList[i].addressee().contains(filter._value.toString(),caseSensitivity)) {
                        includes = true;
                        break;
                    }
                }
            }
            if (cmp == QMessageDataComparator::Includes) {
                if (includes) {
                    return true;
                }
            } else { // Excludes
                if (!includes) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::Subject:
        {
        if (filter._comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (filter._value.toString().length() > 0) {
                    if (message.subject().compare(filter._value.toString(),caseSensitivity) == 0) {
                        return true;
                    }
                }
            } else { // NotEqual
                if (filter._value.toString().length() > 0) {
                    if (message.subject().compare(filter._value.toString(),caseSensitivity) != 0) {
                        return true;
                    }
                } else {
                    return true;
                }
            }
        } else if (filter._comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if (message.subject().contains(filter._value.toString(),caseSensitivity)) {
                    return true;
                }
            } else { // Excludes
            if (!message.subject().contains(filter._value.toString(),caseSensitivity)) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::TimeStamp:
        {
        if (filter._comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.date() == filter._value.toDateTime()) {
                    return true;
                }
            } else { // NotEqual
                if (message.date() != filter._value.toDateTime()) {
                    return true;
                }
            }
        } else if (filter._comparatorType == QMessageFilterPrivate::Relation) {
            QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::LessThan) {
                if (message.date() < filter._value.toDateTime()) {
                    return true;
                }
            } else if (cmp == QMessageDataComparator::LessThanEqual) {
                if (message.date() <= filter._value.toDateTime()) {
                    return true;
                }
            } else if (cmp == QMessageDataComparator::GreaterThan) {
                if (message.date() > filter._value.toDateTime()) {
                    return true;
                }
            } else { // GreaterThanEqual
                if (message.date() >= filter._value.toDateTime()) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::ReceptionTimeStamp:
        {
        if (filter._comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.date() == filter._value.toDateTime()) {
                    return true;
                }
            } else { // NotEqual
                if (message.date() != filter._value.toDateTime()) {
                    return true;
                }
            }
        } else if (filter._comparatorType == QMessageFilterPrivate::Relation) {
            QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::LessThan) {
                if (message.receivedDate() < filter._value.toDateTime()) {
                    return true;
                }
            } else if (cmp == QMessageDataComparator::LessThanEqual) {
                if (message.receivedDate() <= filter._value.toDateTime()) {
                    return true;
                }
            } else if (cmp == QMessageDataComparator::GreaterThan) {
                if (message.receivedDate() > filter._value.toDateTime()) {
                    return true;
                }
            } else { // GreaterThanEqual
                if (message.receivedDate() >= filter._value.toDateTime()) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::Status:
        {
        if (filter._comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (filter._value.toInt() == 0) {
                if (cmp == QMessageDataComparator::Equal) {
                    return false;
                } else {
                    return true;
                }
            }
            QMessage::Status status = static_cast<QMessage::Status>(filter._value.toInt());
            if (cmp == QMessageDataComparator::Equal) {
                if ((message.status() & status) == status) {
                    return true;
                }
            } else { // NotEqual
                if (!((message.status() & status) == status)) {
                    return true;
                }
            }
        } else if (filter._comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (filter._value.toInt() == 0) {
                if (cmp == QMessageDataComparator::Includes) {
                    if (filter._notFilterForComparator) {
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    if (filter._notFilterForComparator) {
                        return true;
                    } else {
                        return false;
                    }
                }
            }
            if (cmp == QMessageDataComparator::Includes) {
                if ((message.status() | filter._value.toInt()) == message.status()) {
                    if (filter._notFilterForComparator) {
                        return false;
                    } else { 
                        return true;
                    }
                } else {
                    if (filter._notFilterForComparator) {
                        return true;
                    }
                }
            } else { // Excludes
                int mask = 15 - filter._value.toInt(); // 15 = 0b1111
                if ((message.status() & mask) == message.status()) {
                    if (filter._notFilterForComparator) {
                        return false;
                    } else { 
                        return true;
                    }
                } else {
                    if (filter._notFilterForComparator) {
                        return true;
                    }
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::Priority:
        {
        QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
        if (cmp == QMessageDataComparator::Equal) {
            if (message.priority() == filter._value.toInt()) {
                return true;
            }
        } else { // NotEqual
            if (message.priority() != filter._value.toInt()) {
                return true;
            }
        }
        break;
        }
    case QMessageFilterPrivate::Size:
        {
        if (filter._comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.size() == filter._value.toInt()) {
                    return true;
                }
            } else { // NotEqual
                if (message.size() != filter._value.toInt()) {
                    return true;
                }
            }
        } else if (filter._comparatorType == QMessageFilterPrivate::Relation) {
            QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::LessThan) {
                if (message.size() < filter._value.toInt()) {
                    return true;
                }
            } else if (cmp == QMessageDataComparator::LessThanEqual) {
                if (message.size() <= filter._value.toInt()) {
                    return true;
                }
            } else if (cmp == QMessageDataComparator::GreaterThan) {
                if (message.size() > filter._value.toInt()) {
                    return true;
                }
            } else { // GreaterThanEqual
                if (message.size() >= filter._value.toInt()) {
                    return true;
                }
            }
        }
        break;
        }        
    case QMessageFilterPrivate::ParentAccountId:
        {
        if (filter._comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.parentAccountId().toString() == filter._value.toString()) {
                    return true;
                }
            } else { // NotEqual
                if (message.parentAccountId().toString() != filter._value.toString()) {
                    return true;
                }
            }
        } else if (filter._comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                //TODO:
            } else { // Excludes
                //TODO:
            }
        }
        break;
        }
    case QMessageFilterPrivate::StandardFolder:
        {
        QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
        QMessage::StandardFolder standardFolder = static_cast<QMessage::StandardFolder>(filter._value.toInt()); 
        if (cmp == QMessageDataComparator::Equal) {
            if (message.standardFolder() == standardFolder) {
                return true;
            }
        } else { // NotEqual
            if (message.standardFolder() != standardFolder) {
                return true;
            }
        }
        break;
        }
    case QMessageFilterPrivate::ParentFolderId:
        {
        if (filter._comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.parentFolderId().toString() == filter._value.toString()) {
                    return true;
                }
            } else { // NotEqual
                if (message.parentFolderId().toString() != filter._value.toString()) {
                    return true;
                }
            }
        } else if (filter._comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                // TODO:
            } else { // Excludes
                // TODO:
            }
        }
        break;
        }
    case QMessageFilterPrivate::AncestorFolderIds:
        {
        if (filter._comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(filter._comparatorValue));
            if (!filter._value.isNull()) { // QMessageFolderId
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
    case QMessageFilterPrivate::ParentAccountIdFilter:
    case QMessageFilterPrivate::ParentFolderIdFilter:
    case QMessageFilterPrivate::None:
        break;
    }

    return false;
}

bool QMessageFilterPrivate::filter(const QMessage &message) const
{
    if (!_valid) {
        return false;
    }

    bool result = false;
    if (_filterList.count() == 0) {
        result = QMessageFilterPrivate::filter(message, *this);
    } else {
        for (int i=0; i < _filterList.count(); i++) {
            for (int j=0; j < _filterList[i].count(); j++) {
                result = QMessageFilterPrivate::filter(message, *_filterList[i][j].d_ptr);
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

QMessageFilterPrivate* QMessageFilterPrivate::implementation(const QMessageFilter &filter)

{
    return filter.d_ptr;
}

QMessageFilter::QMessageFilter()
 : d_ptr(new QMessageFilterPrivate(this))
{
	d_ptr->_matchFlags = 0;

	d_ptr->_valid = true; // Empty filter is valid
	d_ptr->_notFilter = false;
	d_ptr->_notFilterForComparator = false;
	d_ptr->_ids = QMessageIdList();
	d_ptr->_value = QVariant();
	d_ptr->_field = QMessageFilterPrivate::None;
	d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
	d_ptr->_comparatorValue = 0;
    d_ptr->_accountFilter = 0;
    d_ptr->_folderFilter = 0;
}

QMessageFilter::QMessageFilter(const QMessageFilter &other)
 : d_ptr(new QMessageFilterPrivate(this))
{
    d_ptr->_accountFilter = 0;
    d_ptr->_folderFilter = 0;

    this->operator=(other);
}

QMessageFilter::~QMessageFilter()
{
    delete d_ptr->_accountFilter;
    d_ptr->_accountFilter = 0;
    delete d_ptr->_folderFilter;
    d_ptr->_folderFilter = 0;

    delete d_ptr;
}

QMessageFilter& QMessageFilter::operator=(const QMessageFilter& other)
{
    if (&other == this)
        return *this;
    
    d_ptr->_matchFlags = other.d_ptr->_matchFlags;
    
    d_ptr->_valid = other.d_ptr->_valid;
    d_ptr->_notFilter = other.d_ptr->_notFilter;
    d_ptr->_notFilterForComparator = other.d_ptr->_notFilterForComparator;
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
    delete d_ptr->_folderFilter;
    d_ptr->_folderFilter = 0;
    if (other.d_ptr->_folderFilter) {
        d_ptr->_folderFilter = new QMessageFolderFilter(*other.d_ptr->_folderFilter);
    }
    
    return *this;
}

void QMessageFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
{
    d_ptr->_matchFlags = matchFlags;
}

QMessageDataComparator::MatchFlags QMessageFilter::matchFlags() const
{
    return d_ptr->_matchFlags;
}

bool QMessageFilter::isEmpty() const
{
    return ((d_ptr->_field == QMessageFilterPrivate::None) &&
    		(d_ptr->_notFilter == false) && 
            (d_ptr->_filterList.count()) == 0);
}

bool QMessageFilter::isSupported() const
{
    return d_ptr->_valid;
}

QMessageFilter QMessageFilter::operator~() const
{
    QMessageFilter result(*this);
    if (result.isEmpty()) {
		result.d_ptr->_notFilter = true;
    } else {
		if (result.d_ptr->_notFilter) {
			result.d_ptr->_notFilter = false;
		} else {
			QMessageFilterPrivate::applyNot(result);
		}
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
	
	if (d_ptr->_notFilter && !d_ptr->_notFilterForComparator) {
		return *this;
	}

	if (other.d_ptr->_notFilter && !d_ptr->_notFilterForComparator) {
		*this = other;
		return *this;
	}
	
    if (d_ptr->_filterList.count() == 0) {
        QMessageFilter newFilter = QMessageFilter(*this); 
        d_ptr->_filterList.append(QMessageFilterPrivate::SortedMessageFilterList());
        d_ptr->_filterList[0] << newFilter;
        d_ptr->_value = QVariant();
        d_ptr->_field = QMessageFilterPrivate::None;
        d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
        d_ptr->_comparatorValue = 0;
    }
    int i = 0;
    while (i < d_ptr->_filterList.count()) {
        if (other.d_ptr->_filterList.count() == 0) {
            d_ptr->_filterList[i] << other;
            qSort(d_ptr->_filterList[i].begin(), d_ptr->_filterList[i].end(), QMessageFilterPrivate::lessThan);
        } else {
            int j = 0;
            int k = i;
            while (j < other.d_ptr->_filterList.count()) {
                if (j+1 < other.d_ptr->_filterList.count()) {
                    d_ptr->_filterList.insert(k+j+1,QMessageFilterPrivate::SortedMessageFilterList());
                    d_ptr->_filterList[k+j+1] << d_ptr->_filterList[k+j];
                    i++;
                }
                d_ptr->_filterList[k+j] << other.d_ptr->_filterList[j];
                if (d_ptr->_filterList[k+j].count() > 1) {
                    qSort(d_ptr->_filterList[k+j].begin(), d_ptr->_filterList[k+j].end(), QMessageFilterPrivate::lessThan);
                }
                j++;
            }
        }
        i++;
    }
    d_ptr->_valid = d_ptr->_valid & other.d_ptr->_valid;
    
    return *this;
}

const QMessageFilter& QMessageFilter::operator|=(const QMessageFilter& other)
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

	if (d_ptr->_notFilter && !d_ptr->_notFilterForComparator) {
		*this = other;
		return *this;
	}

	if (other.d_ptr->_notFilter && !d_ptr->_notFilterForComparator) {
		return *this;
	}
    
    if (d_ptr->_filterList.count() == 0) {
        QMessageFilter newFilter = QMessageFilter(*this);
        d_ptr->_filterList.append(QMessageFilterPrivate::SortedMessageFilterList());
        d_ptr->_filterList[d_ptr->_filterList.count()-1] << newFilter;
        d_ptr->_value = QVariant();
        d_ptr->_field = QMessageFilterPrivate::None;
        d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
        d_ptr->_comparatorValue = 0;
    } 
    if (other.d_ptr->_filterList.count() == 0) {
        d_ptr->_filterList.append(QMessageFilterPrivate::SortedMessageFilterList());
        d_ptr->_filterList[d_ptr->_filterList.count()-1] << other;
    } else {
        d_ptr->_filterList << other.d_ptr->_filterList;
    }
    d_ptr->_valid = d_ptr->_valid & other.d_ptr->_valid;

    return *this;
}

bool QMessageFilter::operator==(const QMessageFilter& other) const
{
    if (d_ptr->_filterList.count() != other.d_ptr->_filterList.count()) {
        return false;
    }
    
    if (d_ptr->_filterList.count() == 0) {
        return (d_ptr->_notFilter == other.d_ptr->_notFilter &&
                d_ptr->_notFilterForComparator == other.d_ptr->_notFilterForComparator &&
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
    if (d_ptr->_folderFilter || other.d_ptr->_folderFilter) {
        if (!d_ptr->_folderFilter || !other.d_ptr->_folderFilter || (*d_ptr->_folderFilter != *other.d_ptr->_folderFilter)) {
            return false;
        }
    }
    
    
    return true;
}

QMessageFilter QMessageFilter::byId(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::Id;
    result.d_ptr->_value = id.toString();
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byId(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
	QMessageFilter result;
	result.d_ptr->_field = QMessageFilterPrivate::Id;
	result.d_ptr->_ids = ids;
	result.d_ptr->_comparatorType = QMessageFilterPrivate::Inclusion;
	result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
	return result;
}

QMessageFilter QMessageFilter::byId(const QMessageFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    *result.d_ptr = *filter.d_ptr;
    if (cmp == QMessageDataComparator::Excludes) {
        // Invert the sense of comparison
        result = ~result;
    }
    return result;
}

QMessageFilter QMessageFilter::byType(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::Type;
    result.d_ptr->_value = type;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byType(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::Type;
    result.d_ptr->_value = static_cast<int>(type);
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::bySender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::Sender;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::bySender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::Sender;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byRecipients(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::Recipients;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::bySubject(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::Subject;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::bySubject(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::Subject;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::TimeStamp;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::TimeStamp;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Relation;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::TimeStamp;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::TimeStamp;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Relation;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byStatus(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::Status;
    result.d_ptr->_value = static_cast<int>(value);
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byStatus(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::Status;
    result.d_ptr->_value = static_cast<int>(mask);
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byPriority(QMessage::Priority value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::Priority;
    result.d_ptr->_value = static_cast<int>(value);
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::bySize(int value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::Size;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::bySize(int value, QMessageDataComparator::RelationComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::Size;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Relation;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::ParentAccountId;
    result.d_ptr->_value = id.toString();
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byParentAccountId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::ParentAccountIdFilter;
    result.d_ptr->_accountFilter = new QMessageAccountFilter(filter);
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFilter QMessageFilter::byStandardFolder(QMessage::StandardFolder folder, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::StandardFolder;
    result.d_ptr->_value = folder;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::ParentFolderId;
    result.d_ptr->_value = id.toString();
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byParentFolderId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::ParentFolderIdFilter;
    result.d_ptr->_folderFilter = new QMessageFolderFilter(filter);
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFilter QMessageFilter::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    QMessageFilter result;
    result.d_ptr->_valid = false;
    return result;
}

QMessageFilter QMessageFilter::byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    QMessageFilter result;
    result.d_ptr->_valid = false;
    return result;
}

QTM_END_NAMESPACE
