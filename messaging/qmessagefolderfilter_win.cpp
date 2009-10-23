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
#ifdef QMESSAGING_OPTIONAL_FOLDER
#include "qmessagefolderfilter.h"
#include "qmessagefolderfilter_p.h"

namespace {

void invertComparator(QMessageFolderFilterPrivate &f)
{
	// Invert the sense of comparison
    if ((f._criterion == QMessageFolderFilterPrivate::None) || 
        (f._criterion == QMessageFolderFilterPrivate::IdEquality) ||
        (f._criterion == QMessageFolderFilterPrivate::NameEquality) ||
        (f._criterion == QMessageFolderFilterPrivate::PathEquality) ||
        (f._criterion == QMessageFolderFilterPrivate::AccountEquality) ||
        (f._criterion == QMessageFolderFilterPrivate::ParentEquality)) {
        f._equality = (f._equality == QMessageDataComparator::Equal ? QMessageDataComparator::NotEqual : QMessageDataComparator::Equal);
    } else if ((f._criterion == QMessageFolderFilterPrivate::IdInclusion) ||
               (f._criterion == QMessageFolderFilterPrivate::NameInclusion) ||
               (f._criterion == QMessageFolderFilterPrivate::PathInclusion) ||
               (f._criterion == QMessageFolderFilterPrivate::AncestorInclusion)) {
        f._inclusion = (f._inclusion == QMessageDataComparator::Includes ? QMessageDataComparator::Excludes : QMessageDataComparator::Includes); 
    }
}

}

bool QMessageFolderFilterPrivate::matchesStore(const QMessageFolderFilter &filter, const MapiStorePtr &store)
{
    // TODO - return false if the filter excludes the entire store
    return true;
}

bool QMessageFolderFilterPrivate::matchesFolder(const QMessageFolderFilter &filter, const MapiFolderPtr &folder)
{
    QMessageFolderFilterPrivate *f(filter.d_ptr);

    if ((f->_criterion == IdEquality) || (f->_criterion == IdInclusion)) {
        QMessageFolderId folderId(folder->id());

        if (f->_criterion == IdEquality) {
            bool idEqual(folderId == *f->_ids.begin());
            return (f->_equality == QMessageDataComparator::Equal ? idEqual : !idEqual);
        } else {
            bool idPresent(f->_ids.contains(folderId));
            return (f->_inclusion == QMessageDataComparator::Includes ? idPresent : !idPresent);
        }
    } else if ((f->_criterion == NameEquality) || (f->_criterion == NameInclusion)) {
        QString folderName(folder->name());

        if (f->_criterion == NameEquality) {
            // TODO: Apply options
            bool nameEqual(folderName == f->_value);
            return (f->_equality == QMessageDataComparator::Equal ? nameEqual : !nameEqual);
        } else {
            bool namePartialMatch(folderName.contains(f->_value));
            return (f->_inclusion == QMessageDataComparator::Includes ? namePartialMatch : !namePartialMatch);
        }
    } else if ((f->_criterion == PathEquality) || (f->_criterion == PathInclusion)) {
        // Path is the same as DisplayName for windows
        QString folderPath(folder->name());

        if (f->_criterion == PathEquality) {
            // TODO: Apply options
            bool pathEqual(folderPath == f->_value);
            return (f->_equality == QMessageDataComparator::Equal ? pathEqual : !pathEqual);
        } else {
            bool pathPartialMatch(folderPath.contains(f->_value));
            return (f->_inclusion == QMessageDataComparator::Includes ? pathPartialMatch : !pathPartialMatch);
        }
    } else if ((f->_criterion == AccountEquality) || (f->_criterion == AccountInclusion)) {
        QMessageAccountId accountId(folder->accountId());

        if (f->_criterion == AccountEquality) {
            bool idEqual(accountId == *f->_accountIds.begin());
            return (f->_equality == QMessageDataComparator::Equal ? idEqual : !idEqual);
        } else {
            bool idPresent(f->_accountIds.contains(accountId));
            return (f->_inclusion == QMessageDataComparator::Includes ? idPresent : !idPresent);
        }
    } else if ((f->_criterion == ParentEquality) || (f->_criterion == ParentInclusion)) {
        QMessageFolderId parentId(folder->parentId());

        if (f->_criterion == AccountEquality) {
            bool idEqual(parentId == *f->_ids.begin());
            return (f->_equality == QMessageDataComparator::Equal ? idEqual : !idEqual);
        } else {
            bool idPresent(f->_ids.contains(parentId));
            return (f->_inclusion == QMessageDataComparator::Includes ? idPresent : !idPresent);
        }
    } else if (f->_criterion == AncestorInclusion) {
        QSet<QMessageFolderId> ancestorIds(folder->ancestorIds().toSet());
        return (ancestorIds.intersect(f->_ids).isEmpty() == false);
    } else { // None
        return (f->_equality == QMessageDataComparator::Equal);
    }
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
    return ((d_ptr->_criterion == QMessageFolderFilterPrivate::None) && 
            (d_ptr->_equality == QMessageDataComparator::Equal));
}

bool QMessageFolderFilter::isSupported() const
{
    return true;
}

QMessageFolderFilter QMessageFolderFilter::operator~() const
{
    QMessageFolderFilter result(*this);
	invertComparator(*result.d_ptr);
    return result;
}

QMessageFolderFilter QMessageFolderFilter::operator&(const QMessageFolderFilter& other) const
{
    // TODO: Not yet implemented
    Q_UNUSED(other)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::operator|(const QMessageFolderFilter& other) const
{
    // TODO: Not yet implemented
    Q_UNUSED(other)
    return QMessageFolderFilter(); // stub
}

const QMessageFolderFilter& QMessageFolderFilter::operator&=(const QMessageFolderFilter& other)
{
    *this = this->operator&(other);
    return *this;
}

const QMessageFolderFilter& QMessageFolderFilter::operator|=(const QMessageFolderFilter& other)
{
    *this = this->operator|(other);
    return *this;
}

bool QMessageFolderFilter::operator==(const QMessageFolderFilter& other) const
{
    return (*d_ptr == *other.d_ptr);
}

QMessageFolderFilter QMessageFolderFilter::byId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_ids.insert(id);
    result.d_ptr->_equality = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::IdEquality;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byId(const QMessageFolderIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_ids = ids.toSet();
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
        invertComparator(*result.d_ptr);
    }
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byDisplayName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_value = value;
    result.d_ptr->_equality = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::NameEquality;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byDisplayName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
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
    QMessageFolderFilter result;
    result.d_ptr->_accountIds.insert(id);
    result.d_ptr->_equality = cmp;
    result.d_ptr->_criterion = QMessageFolderFilterPrivate::AccountEquality;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentAccountId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    // TODO: ...
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
    // TODO: ...
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
    // TODO: ...
    return result;
}
#endif
