/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "cntsymbiansorterdbms.h"
#include "cntsymbiantransformerror.h"

#include <cntdb.h>
#include <cntdef.h>
#include <cntfield.h>
#include "cnttransformcontact.h"

typedef QList<QContactLocalId> QContactLocalIdList;

/* ... The macros changed names */
#if QT_VERSION < QT_VERSION_CHECK(4, 6, 0)
#define QT_TRAP_THROWING QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION
#define QT_TRYCATCH_LEAVING QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE
#endif

// NOTE: There is a bug with RVCT compiler which causes the local stack
// variable to corrupt if the called function leaves. As a workaround we are
// reserving the objects from heap so it will not get corrupted.
// This of course applies only to those stack variables which are passed to
// the called function or the return value of the function is placed to the
// variable.

CntSymbianSorterDbms::CntSymbianSorterDbms(CContactDatabase& contactDatabase,CntTransformContact& transformContact):
    m_contactDatabase(contactDatabase),
    m_transformContact(transformContact)
{
}

CntSymbianSorterDbms::~CntSymbianSorterDbms()
{
}

QList<QContactLocalId> CntSymbianSorterDbms::contacts(
            const QList<QContactSortOrder>& sortOrders,
            QContactManager::Error* error)
{
    // Create an empty list
    // See QT_TRYCATCH_LEAVING note at the begginning of this file
    QContactLocalIdList *ids = new QContactLocalIdList();

    // Attempt to read from database, leaving the list empty if
    // there was a problem
    TRAPD(err, QT_TRYCATCH_LEAVING(*ids = contactsL(sortOrders)));

    CntSymbianTransformError::transformError(err, error);

    return *QScopedPointer<QContactLocalIdList>(ids);
}

QList<QContactLocalId> CntSymbianSorterDbms::sort(
            QList<QContactLocalId> contactIds,
            const QList<QContactSortOrder>& sortOrders,
            QContactManager::Error* error)
{
    // Create an empty list
    // See QT_TRYCATCH_LEAVING note at the begginning of this file
    QContactLocalIdList *ids = new QContactLocalIdList();

    // Attempt to read from database, leaving the list empty if
    // there was a problem
    TRAPD(err, QT_TRYCATCH_LEAVING(*ids = sortL(contactIds,sortOrders)));

    CntSymbianTransformError::transformError(err, error);

    return *QScopedPointer<QContactLocalIdList>(ids);
}

bool CntSymbianSorterDbms::sortOrderSupported(const QList<QContactSortOrder>& sortOrders)
{
    foreach(const QContactSortOrder& s, sortOrders ) {
        // Find uids for sortings
        QList<TUid> fieldTypeUids = m_transformContact.supportedSortingFieldTypes(s.detailDefinitionName(), s.detailFieldName());
        if( fieldTypeUids.count() == 0 )
            return false;

        // Only blanks last supported
        if( s.blankPolicy() != QContactSortOrder::BlanksLast )
            return false;

        // Always case sensitive
        if( s.caseSensitivity() != Qt::CaseSensitive )
            return false;

#ifndef SYMBIAN_BACKEND_USE_SQLITE
        // NOTE:
        // Seems that there is a bug in cntmodel which causes that sorting
        // is working correctly only if the direction is the same for all
        // sort orders.
        if( s.direction() != sortOrders[0].direction() )
            return false;
#endif
    }
    return true;
}

QList<QContactLocalId> CntSymbianSorterDbms::contactsL(const QList<QContactSortOrder>& sortOrders) const
{
    // Populate the ID array, returns the coontact ids + group ids
    TTime epoch(0);
    CContactIdArray *ids = m_contactDatabase.ContactsChangedSinceL(epoch);
    CleanupStack::PushL(ids);

    // Remove templates from the list
    CContactIdArray *templateIds = m_contactDatabase.GetCardTemplateIdListL();
    CleanupStack::PushL(templateIds);
    for(TInt i(0); i < templateIds->Count(); ++i) {
        TContactItemId id = (*templateIds)[i];
        TInt index = ids->Find(id);
        if(index > KErrNotFound)
            ids->Remove(index);
    }
    CleanupStack::PopAndDestroy(templateIds);

    // Sort the list
    CContactIdArray* sortedIds = sortL(ids, sortOrders);
    CleanupStack::PopAndDestroy(ids);
    ids = sortedIds;
    CleanupStack::PushL(ids);

    // Add the contact ids to the returned QList
    QList<QContactLocalId> qIds;
    for (TInt i(0); i < ids->Count(); ++i) {
        qIds.append((*ids)[i]);
    }

    CleanupStack::PopAndDestroy(ids);

    return qIds;
}

QList<QContactLocalId> CntSymbianSorterDbms::sortL(const QList<QContactLocalId>& contactIds, const QList<QContactSortOrder>& sortOrders) const
{
    CContactIdArray* ids = CContactIdArray::NewLC();
    foreach(QContactLocalId id, contactIds)
        ids->AddL(id);

    CContactIdArray* sortedIds = sortL(ids, sortOrders);
    CleanupStack::PopAndDestroy(ids);
    CleanupStack::PushL(sortedIds);

    QList<QContactLocalId> qSortedIds;
    for (int i=0; i<sortedIds->Count(); i++)
        qSortedIds.append( (*sortedIds)[i] );
    CleanupStack::PopAndDestroy(sortedIds);

    return qSortedIds;
}

CContactIdArray* CntSymbianSorterDbms::sortL(const CContactIdArray* contactIds, const QList<QContactSortOrder>& sortOrders) const
{
    CArrayFixFlat<CContactDatabase::TSortPref> *sort =
        new (ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(5);
    CleanupStack::PushL(sort);

    // Convert sort orders to TSortPref array
    foreach (const QContactSortOrder& s, sortOrders)
    {
        QList<TUid> fieldTypes = m_transformContact.supportedSortingFieldTypes(s.detailDefinitionName(), s.detailFieldName());
        if (fieldTypes.count())
        {
            foreach(const TUid& fieldType, fieldTypes) {
                CContactDatabase::TSortPref pref;
                // NOTE: TSortPref sets order to ascending by default
                if( s.direction() == Qt::DescendingOrder )
                    pref.iOrder = CContactDatabase::TSortPref::EDesc;
                pref.iFieldType = fieldType;
                sort->AppendL(pref);
            }
        }
        else
        {
            // This should never happen. Sorting should have happened
            // the "slow way" at QContactManagerEngine::sortContacts.
            User::Leave(KErrNotFound);
        }
    }

    CContactIdArray* sortedIds(0);
    // There is a bug in CContactDatabase::SortArrayL, if an empty sort is used
    // the function returns all contacts (and groups) instead of the given
    // contact ids
    if(sortOrders.isEmpty()) {
        sortedIds = CContactIdArray::NewL(contactIds);
    } else {
        sortedIds = m_contactDatabase.SortArrayL(contactIds,sort);
    }
    CleanupStack::PopAndDestroy(sort);
    return sortedIds;
}
