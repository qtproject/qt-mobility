/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
//system includes
#include <e32base.h>
#include <s32mem.h>
#include <cntitem.h>

//user includes
#include "cntsymbiandatabase.h"
#include "cntsymbiantransformerror.h"
#include "qcontactchangeset.h"
#include "qcontactmanagerengine.h"

// Constant
typedef QPair<QContactLocalId, QContactLocalId> QOwnCardPair;

CntSymbianDatabase::CntSymbianDatabase(QContactManagerEngine *engine, QContactManager::Error& error) :
    m_engine(engine),
    m_contactDatabase(0),
    m_currentOwnCardId(0)
{
    TRAPD(err, initializeL());
    CntSymbianTransformError::transformError(err, error);
}

void CntSymbianDatabase::initializeL()
{
    User::LeaveIfNull(m_engine);

    TRAPD(err, m_contactDatabase = CContactDatabase::OpenL());

    // Database not found, create it
    if(err == KErrNotFound) {
        m_contactDatabase = CContactDatabase::CreateL();
    }

#ifndef SYMBIAN_BACKEND_USE_SQLITE
    // In pre 10.1 platforms the AddObserverL & RemoveObserver functions are not
    // exported so we need to use CContactChangeNotifier.
    TRAP(err, m_contactChangeNotifier = CContactChangeNotifier::NewL(*m_contactDatabase, this));
#else
    TRAP(err, m_contactDatabase->AddObserverL(*this));
#endif

    // Read current own card id (self contact id)
    TContactItemId myCard = m_contactDatabase->OwnCardId();
    if (myCard > 0)
        m_currentOwnCardId = QContactLocalId(myCard);

    // Currently the group membership check is only used in pre-10.1
    // platforms. In 10.1 we need to check the performance penalty
    // caused in the instantiation of QContactManager. If the
    // performance is too bad, then the MContactDbObserver API needs to
    // be changed in 10.1 so that we don't need the group membership
    // buffer in the engine level. In other words events like
    // EContactDbObserverEventGroupMembersAdded and 
    // EContactDbObserverEventGroupMembersRemoved need to be added to
    // MContactDbObserver.
#ifndef SYMBIAN_BACKEND_USE_SQLITE
    updateGroupMembershipsL();
#endif
}

CntSymbianDatabase::~CntSymbianDatabase()
{
    m_engine = NULL;
#ifndef SYMBIAN_BACKEND_USE_SQLITE
    delete m_contactChangeNotifier;
#else
    if (m_contactDatabase != 0) {
        m_contactDatabase->RemoveObserver(*this);
    }
#endif
    delete m_contactDatabase;
}

CContactDatabase* CntSymbianDatabase::contactDatabase()
{
    return m_contactDatabase;
}

void CntSymbianDatabase::appendContactsEmitted(const QList<QContactLocalId> &contactList)
{
    m_contactsEmitted += contactList;
}

void CntSymbianDatabase::appendContactEmitted(QContactLocalId id)
{
    m_contactsEmitted.append(id);
}

/*!
 * Respond to a contacts database event, delegating this event to
 * an appropriate signal as required.
 *
 * \param aEvent Contacts database event describing the change to the
 *  database.
 */
void CntSymbianDatabase::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
{
    QContactChangeSet changeSet;
    TContactItemId id = aEvent.iContactId;

    switch (aEvent.iType)
    {
    case EContactDbObserverEventContactAdded:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.addedContacts().insert(id);
        break;
    case EContactDbObserverEventOwnCardDeleted:
        m_currentOwnCardId = QContactLocalId(0);
        // ...and send contact deleted event
    case EContactDbObserverEventContactDeleted:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.removedContacts().insert(id);
        break;
    case EContactDbObserverEventContactChanged:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.changedContacts().insert(id);
        break;
    case EContactDbObserverEventGroupAdded:
        if(m_contactsEmitted.contains(id)) {
            // adding a group triggers also a "changed" event. The work-around
            // is to leave the id to m_contactsEmitted
        } else {
            changeSet.addedContacts().insert(id);
            m_contactsEmitted.append(id);
        }
        break;
    case EContactDbObserverEventGroupDeleted:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.removedContacts().insert(id);
        break;
    case EContactDbObserverEventGroupChanged:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else {
#ifndef SYMBIAN_BACKEND_USE_SQLITE
            // Contact DB observer API does not give information of contacts
            // possibly added to or removed from the group
            QSet<QContactLocalId> added;
            QSet<QContactLocalId> removed;
            TRAPD(err, updateGroupMembershipsL(id, added, removed));
            if(err != KErrNone){
                changeSet.setDataChanged(true);
            } else if(removed.count()) {
                // The group changed event was caused by removing contacts
                // from the group
                changeSet.removedRelationshipsContacts().insert(id);
                changeSet.removedRelationshipsContacts() += removed;
            } else if(added.count()) {
                // The group changed event was caused by adding contacts
                // to the group
                changeSet.addedRelationshipsContacts().insert(id);
                changeSet.addedRelationshipsContacts() += added;
            } else {
                // The group changed event was caused by modifying the group
                changeSet.changedContacts().insert(id);
            }
#else
            // Currently the group membership check is only used in pre-10.1
            // platforms. In 10.1 we need to check the performance penalty
            // caused in the instantiation of QContactManager. If the
            // performance is too bad, then the MContactDbObserver API needs to
            // be changed in 10.1 so that we don't need the group membership
            // buffer in the engine level. In other words events like
            // EContactDbObserverEventGroupMembersAdded and 
            // EContactDbObserverEventGroupMembersRemoved need to be added to
            // MContactDbObserver.
            changeSet.changedContacts().insert(id); //group is a contact
#endif
        }
        break;
    case EContactDbObserverEventOwnCardChanged:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else {
            QOwnCardPair ownCard(m_currentOwnCardId, QContactLocalId(id));
            changeSet.oldAndNewSelfContactId() = ownCard;
            m_currentOwnCardId = QContactLocalId(id);
        }
        break;
    default:
        break; // ignore other events
    }

    changeSet.emitSignals(m_engine);
}

/*
 * Private implementation for updating the buffer containing the members of all
 * groups.
 */
void CntSymbianDatabase::updateGroupMembershipsL()
{
    CContactIdArray *groupIds = m_contactDatabase->GetGroupIdListL();
    for (TInt i(0); i < groupIds->Count(); i++) {
        QContactLocalId id = (*groupIds)[i];
        QSet<QContactLocalId> dummySet;
        updateGroupMembershipsL(id, dummySet, dummySet);
    }
    delete groupIds;
}

/*
 * Private implementation for updating the buffer containing the members of a
 * group.
 */
void CntSymbianDatabase::updateGroupMembershipsL(
    QContactLocalId groupId,
    QSet<QContactLocalId> &added,
    QSet<QContactLocalId> &removed)
{
    QSet<QContactLocalId> groupMembersNew = groupMembersL(groupId);
    QSet<QContactLocalId> groupMembersOld = m_groupContents.value(groupId);

    if(groupMembersOld.count() < groupMembersNew.count()) {
        added = groupMembersNew - groupMembersOld;
        m_groupContents.remove(groupId);
        m_groupContents.insert(groupId, groupMembersNew);
    } else if(groupMembersOld.count() > groupMembersNew.count()) {
        removed = groupMembersOld - groupMembersNew;
        m_groupContents.remove(groupId);
        m_groupContents.insert(groupId, groupMembersNew);
    }
}

/*
 * Private implementation for fetching the members of a group.
 */
QSet<QContactLocalId> CntSymbianDatabase::groupMembersL(QContactLocalId groupId)
{
    QSet<QContactLocalId> groupMembers;

    CContactItem *contactItem = m_contactDatabase->ReadContactLC(TContactItemId(groupId));
    Q_ASSERT(contactItem && contactItem->Type() == KUidContactGroup);
    CContactGroup *group = static_cast<CContactGroup*>(contactItem);
    
    const CContactIdArray *idArray = group->ItemsContained();
    
    //loop through all the contacts and add them to the list
    for (int i(0); i < idArray->Count(); i++) {
        groupMembers.insert((*idArray)[i]);
    }
    CleanupStack::PopAndDestroy(contactItem);

    return groupMembers;
}
