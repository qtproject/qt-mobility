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

QMessageFilterPrivate::QMessageFilterPrivate(QMessageFilter *messageFilter)
 : q_ptr(messageFilter),
   _field(None),
   _comparatorType(Equality),
   _comparatorValue(QMessageDataComparator::Equal),
   _valid(true)
{
}

QMessageFilterPrivate::~QMessageFilterPrivate()
{
}

bool QMessageFilterPrivate::lessThan(const QMessageFilter filter1, const QMessageFilter filter2)
{
    return filter1.d_ptr->_field < filter2.d_ptr->_field;
}

void QMessageFilterPrivate::applyNot(QMessageFilter& filter)
{
    if (filter.d_ptr->_filterList.count() == 0) {
        if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter.d_ptr->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
            } else {
                filter.d_ptr->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
            }
        } if (filter.d_ptr->_comparatorType == QMessageFilterPrivate::Relation) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(filter.d_ptr->_comparatorValue));
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
                QMessageFilterPrivate::applyNot(filter.d_ptr->_filterList[i][j]);
            }
        }
    }
}

bool QMessageFilterPrivate::filter(QMessage& message)
{
    if (!_valid) {
        return false;
    }
    
    Qt::CaseSensitivity caseSensitivity = (_options & QMessageDataComparator::CaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;

    switch (_field) {
    case QMessageFilterPrivate::Id:
        {
        if (_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.id().toString() == _value.toString()) {
                    return true;
                }
            } else { // NotEqual
                if (message.id().toString() != _value.toString()) {
                    return true;
                }
            }
        } else if (_comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            if (_ids.count() > 0) { // QMessageIdList
               if (cmp == QMessageDataComparator::Includes) {
                   if (_ids.contains(message.id())) {
                       return true;
                   }
               } else { // Excludes
                   if (!_ids.contains(message.id())) {
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
        if (_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.type() == _value.toInt()) {
                    return true;
                }
            } else { // NotEqual
                if (message.type() != _value.toInt()) {
                    return true;
                }
            }
        } else if (_comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if ((message.type() & _value.toInt()) == message.type()) {
                    return true;
                }
            } else { // Excludes
            if ((message.type() & _value.toInt()) == 0) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::Sender:
        {
        if (_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.from().recipient().compare(_value.toString(),caseSensitivity) == 0) {
                    return true;
                }
            } else { // NotEqual
                if (message.from().recipient().compare(_value.toString(),caseSensitivity) != 0) {
                    return true;
                }
            }
        } else if (_comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if (message.from().recipient().contains(_value.toString(),caseSensitivity)) {
                    return true;
                }
            } else { // Excludes
            if (!message.from().recipient().contains(_value.toString(),caseSensitivity)) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::Recipients: // to, cc & bcc fields
        {
        if (_comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            bool includes = false;
            // Check to addresses
            QMessageAddressList addrList = message.to();
            for (int i=0; i < addrList.count(); i++) {
                if (addrList[i].recipient().contains(_value.toString(),caseSensitivity)) {
                    includes = true;
                    break;
                }
            }
            if (!includes) {
                // Check cc addresses
                addrList = message.cc();
                for (int i=0; i < addrList.count(); i++) {
                    if (addrList[i].recipient().contains(_value.toString(),caseSensitivity)) {
                        includes = true;
                        break;
                    }
                }
            }
            if (!includes) {
                // Check bcc addresses
                addrList = message.bcc();
                for (int i=0; i < addrList.count(); i++) {
                    if (addrList[i].recipient().contains(_value.toString(),caseSensitivity)) {
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
        if (_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
				if (message.subject().compare(_value.toString(),caseSensitivity) == 0) {
                    return true;
                }
            } else { // NotEqual
				if (message.subject().compare(_value.toString(),caseSensitivity) != 0) {
                    return true;
                }
            }
        } else if (_comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if (message.subject().contains(_value.toString(),caseSensitivity)) {
                    return true;
                }
            } else { // Excludes
            if (!message.subject().contains(_value.toString(),caseSensitivity)) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::TimeStamp:
        {
        if (_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.date() == _value.toDateTime()) {
                    return true;
                }
            } else { // NotEqual
                if (message.date() != _value.toDateTime()) {
                    return true;
                }
            }
        } else if (_comparatorType == QMessageFilterPrivate::Relation) {
            QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::LessThan) {
                if (message.date() < _value.toDateTime()) {
                    return true;
                }
            } else if (cmp == QMessageDataComparator::LessThanEqual) {
                if (message.date() <= _value.toDateTime()) {
                    return true;
                }
            } else if (cmp == QMessageDataComparator::GreaterThan) {
                if (message.date() > _value.toDateTime()) {
                    return true;
                }
            } else { // GreaterThanEqual
                if (message.date() >= _value.toDateTime()) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::ReceptionTimeStamp:
        {
        if (_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.date() == _value.toDateTime()) {
                    return true;
                }
            } else { // NotEqual
                if (message.date() != _value.toDateTime()) {
                    return true;
                }
            }
        } else if (_comparatorType == QMessageFilterPrivate::Relation) {
            QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::LessThan) {
                if (message.receivedDate() < _value.toDateTime()) {
                    return true;
                }
            } else if (cmp == QMessageDataComparator::LessThanEqual) {
                if (message.receivedDate() <= _value.toDateTime()) {
                    return true;
                }
            } else if (cmp == QMessageDataComparator::GreaterThan) {
                if (message.receivedDate() > _value.toDateTime()) {
                    return true;
                }
            } else { // GreaterThanEqual
                if (message.receivedDate() >= _value.toDateTime()) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::Status:
        {
        if (_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.status() == _value.toInt()) {
                    return true;
                }
            } else { // NotEqual
                if (message.status() != _value.toInt()) {
                    return true;
                }
            }
        } else if (_comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Includes) {
                if ((message.status() & _value.toInt()) == message.status()) {
                    return true;
                }
            } else { // Excludes
            if ((message.status() & _value.toInt()) == 0) {
                    return true;
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::Priority:
        {
        QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
        if (cmp == QMessageDataComparator::Equal) {
            if (message.priority() == _value.toInt()) {
                return true;
            }
        } else { // NotEqual
            if (message.priority() != _value.toInt()) {
                return true;
            }
        }
        break;
        }
    case QMessageFilterPrivate::Size:
        {
        if (_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.size() == _value.toInt()) {
                    return true;
                }
            } else { // NotEqual
                if (message.size() != _value.toInt()) {
                    return true;
                }
            }
        } else if (_comparatorType == QMessageFilterPrivate::Relation) {
            QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::LessThan) {
                if (message.size() < _value.toInt()) {
                    return true;
                }
            } else if (cmp == QMessageDataComparator::LessThanEqual) {
                if (message.size() <= _value.toInt()) {
                    return true;
                }
            } else if (cmp == QMessageDataComparator::GreaterThan) {
                if (message.size() > _value.toInt()) {
                    return true;
                }
            } else { // GreaterThanEqual
                if (message.size() >= _value.toInt()) {
                    return true;
                }
            }
        }
        break;
        }        
    case QMessageFilterPrivate::ParentAccountId:
        {
        if (_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.parentAccountId().toString() == _value.toString()) {
                    return true;
                }
            } else { // NotEqual
                if (message.parentAccountId().toString() != _value.toString()) {
                    return true;
                }
            }
        } else if (_comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
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
        QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
        QMessage::StandardFolder standardFolder = static_cast<QMessage::StandardFolder>(_value.toInt()); 
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
        if (_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                if (message.parentFolderId().toString() == _value.toString()) {
                    return true;
                }
            } else { // NotEqual
                if (message.parentFolderId().toString() != _value.toString()) {
                    return true;
                }
            }
        } else if (_comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(_comparatorValue));
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
        if (_comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(_comparatorValue));
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

    return false;
}

QMessageFilterPrivate* QMessageFilterPrivate::implementation(const QMessageFilter &filter)

{
    return filter.d_ptr;
}

QMessageFilter::QMessageFilter()
 : d_ptr(new QMessageFilterPrivate(this))
{
	d_ptr->_options = 0;

	d_ptr->_valid = false;
	d_ptr->_ids = QMessageIdList();
	d_ptr->_value = QVariant();
	d_ptr->_field = QMessageFilterPrivate::None;
	d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
	d_ptr->_comparatorValue = 0;
}

QMessageFilter::QMessageFilter(const QMessageFilter &other)
 : d_ptr(new QMessageFilterPrivate(this))
{
    this->operator=(other);
}

QMessageFilter::~QMessageFilter()
{
    delete d_ptr;
}

QMessageFilter& QMessageFilter::operator=(const QMessageFilter& other)
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

void QMessageFilter::setOptions(QMessageDataComparator::Options options)
{
    d_ptr->_options = options;
}

QMessageDataComparator::Options QMessageFilter::options() const
{
    return d_ptr->_options;
}

bool QMessageFilter::isEmpty() const
{
    return ((d_ptr->_field == QMessageFilterPrivate::None) && 
            (d_ptr->_filterList.count()) == 0);
}

bool QMessageFilter::isSupported() const
{
    return d_ptr->_valid;
}

QMessageFilter QMessageFilter::operator~() const
{
    QMessageFilter result(*this);
    QMessageFilterPrivate::applyNot(result);
    return result;
}

QMessageFilter QMessageFilter::operator&(const QMessageFilter& other) const
{
	if (isEmpty()) {
		return QMessageFilter(other);
	}

	if (other.isEmpty()) {
		return QMessageFilter(*this);
	}

    QMessageFilter result;
    result.d_ptr->_filterList = d_ptr->_filterList;
    if (result.d_ptr->_filterList.count() == 0) {
        result.d_ptr->_filterList.append(QMessageFilterPrivate::SortedMessageFilterList());
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
        qSort(result.d_ptr->_filterList[i].begin(), result.d_ptr->_filterList[i].end(), QMessageFilterPrivate::lessThan);
    }
    result.d_ptr->_valid = d_ptr->_valid & other.d_ptr->_valid;
    
    return result;
}

QMessageFilter QMessageFilter::operator|(const QMessageFilter& other) const
{
	if (isEmpty()) {
		return QMessageFilter(*this);
	}
	
	if (other.isEmpty()) {
		return QMessageFilter(other);
	}
	
    QMessageFilter result;
    if (d_ptr->_filterList.count() == 0) {
        result.d_ptr->_filterList.append(QMessageFilterPrivate::SortedMessageFilterList());
        result.d_ptr->_filterList[0] << *this;
    } else {
        result.d_ptr->_filterList = d_ptr->_filterList;
    }
    if (other.d_ptr->_filterList.count() == 0) {
        result.d_ptr->_filterList.append(QMessageFilterPrivate::SortedMessageFilterList());
        result.d_ptr->_filterList[result.d_ptr->_filterList.count()-1] << other;
    } else {
        result.d_ptr->_filterList << other.d_ptr->_filterList;
    }
    result.d_ptr->_valid = d_ptr->_valid & other.d_ptr->_valid;
    
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
	
    if (d_ptr->_filterList.count() == 0) {
        d_ptr->_filterList.append(QMessageFilterPrivate::SortedMessageFilterList());
        d_ptr->_filterList[0] << QMessageFilter(*this);
    }
    for (int i=0; i < d_ptr->_filterList.count(); i++) {
        if (other.d_ptr->_filterList.count() == 0) {
            d_ptr->_filterList[i] << other;
        } else {
            for (int j=0; j < other.d_ptr->_filterList.count(); j++) {
                d_ptr->_filterList[i] << other.d_ptr->_filterList[j];
            }
        }
        qSort(d_ptr->_filterList[i].begin(), d_ptr->_filterList[i].end(), QMessageFilterPrivate::lessThan);
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
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    // TODO:
    return QMessageFilter(); // stub
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
    result.d_ptr->_field = QMessageFilterPrivate::ReceptionTimeStamp;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::ReceptionTimeStamp;
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
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    // TODO:
    return QMessageFilter(); // stub
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

#ifdef QMESSAGING_OPTIONAL_FOLDER
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
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    // TODO:
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = QMessageFilterPrivate::AncestorFolderIds;
    result.d_ptr->_value = id.toString();
    result.d_ptr->_comparatorType = QMessageFilterPrivate::Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    result.d_ptr->_valid = true;
    return result;
}

QMessageFilter QMessageFilter::byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    // TODO:
    return QMessageFilter(); // stub
}
#endif
