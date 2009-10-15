/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactsymbianengine_p.h"
#include "qcontact_p.h"

#include <cntfldst.h>
#include <cntfield.h>

#include <qtcontacts.h>
#include <QSet>
#include "qcontactchangeset.h"
#include "qcontactsymbianfilterdbms.h"
#include "qcontactsymbianfiltersql.h"
#include "qcontactsymbiansorterdbms.h"

#include <QDebug>

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

QContactSymbianEngineData::QContactSymbianEngineData(QContactManager::Error& error) :
    m_contactDatabase(0),
    m_transformContact(0)
{
    TRAPD(err, m_contactDatabase = CContactDatabase::OpenL())
        
    //Database not found, create it
    if(err == KErrNotFound)
    {
        TRAP(err, m_contactDatabase = CContactDatabase::CreateL())
    }
    
    //If database didn't open successfully return error
    if(err != KErrNone)
    {
        transformError(err, error);
    }
    
    //Database opened successfully
    else
    {
    // In pre 10.1 platforms the AddObserverL & RemoveObserver functions are not
    // exported so we need to use CContactChangeNotifier.
    // TODO: Is it ok to use __SYMBIAN_CNTMODEL_USE_SQLITE__ flag for this?
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
        TRAP(err, m_contactChangeNotifier = CContactChangeNotifier::NewL(*m_contactDatabase, this));
#else
        TRAP(err, m_contactDatabase->AddObserverL(*this));
#endif

    	m_transformContact = new TransformContact;
        m_contactFilter = new QContactSymbianFilter(*m_contactDatabase);
        m_contactSorter = new QContactSymbianSorter(*m_contactDatabase, *m_transformContact);
        
        transformError(err, error);
    }
}

QContactSymbianEngineData::~QContactSymbianEngineData()
{
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
	delete m_contactChangeNotifier;
#else
	if (m_contactDatabase != 0) {
	    m_contactDatabase->RemoveObserver(*this);
	}
#endif
	// m_contactFilter needs to be deleted before m_contactDatabase
    delete m_contactFilter;
    delete m_contactSorter;
	delete m_contactDatabase;
	delete m_transformContact;
}

/* Access */
QList<QContactLocalId> QContactSymbianEngineData::contacts(
        const QContactFilter& filter,
        const QList<QContactSortOrder>& sortOrders,
        QContactManager::Error& error) const
{
    return m_contactFilter->contacts(filter, sortOrders, error);
}

QAbstractContactFilter::FilterSupport QContactSymbianEngineData::filterSupported(
        const QContactFilter& filter) const
{
    return m_contactFilter->filterSupported(filter);
}

bool QContactSymbianEngineData::sortOrderSupported(const QList<QContactSortOrder>& sortOrders) const
{
    return m_contactSorter->sortOrderSupported(sortOrders);
}

/*!
 * Read a contact from the contact database.
 *
 * Internal implementation to read a conact, called by
 * QContactManager::contact().
 *
 * \param contactId The Id of the contact to be retrieved.
 * \param qtError Qt error code.
 * \return A QContact for the requested QUniquId value or 0 if the read
 *  operation was unsuccessful (e.g. contact not found).
 */
QContact QContactSymbianEngineData::contact(const QContactLocalId& contactId, QContactManager::Error& qtError) const
{
    // See QT_TRYCATCH_LEAVING note at the begginning of this file
    QContact* contact = new QContact();
    TRAPD(err, QT_TRYCATCH_LEAVING(*contact = contactL(contactId)));
    transformError(err, qtError);
    return *QScopedPointer<QContact>(contact);
}


/*!
 * Check how many contacts are in the database. This is an internal
 * wrapper function for CContactDatabase::CountL().
 *
 * If an error occurs, count will contain the error code and will be
 * less than zero.
 *
 * \return Number of contacts in database or an error code.
 */
int QContactSymbianEngineData::count() const
{
	// Call leaving implementation and trap the error code
	int count(0);
	// countL() can't throw c++ exception
	TRAPD(err, count = countL());

	// If there was a problem, return the negative value as the
	// error code.
	if (err != KErrNone) {
		count = err;
	}

	return count;
}

/*!
 * Complete list of contact IDs in database is needed until we
 * do something with views so that QContactModel can function.
 *
 * It works by retrieving a list of contact IDs changed since
 * epoch, as this API does not exist "properly" in CNTMODEL.
 *
 * \param sortOrders Sort order
 * \param qtError Qt error code.
 * \return List of all IDs for contact entries in the database,
 *  or an empty list if there was a problem or the database is
 *  empty.
 */
QList<QContactLocalId> QContactSymbianEngineData::contacts( const QList<QContactSortOrder>& sortOrders, QContactManager::Error& qtError) const
{
    return m_contactSorter->contacts( sortOrders, qtError );
}

/* add/update/delete */

/*!
 * Add the specified contact item to the persistent contacts store.
 *
 * \param contact The QContact to be saved.
 * \param id The Id of new contact
 * \param qtError Qt error code.
 * \return Error status
 */
bool QContactSymbianEngineData::addContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError)
{
	// Attempt to persist contact, trapping errors
    int err(0);
    QContactLocalId id(0);
    TRAP(err, QT_TRYCATCH_LEAVING(id = addContactL(contact)));
    if(err == KErrNone)
    {
        QContactId contactId;
        contactId.setLocalId(id);
        //contactId.setManagerUri() //needs to be fixed
        
        contact.setId(contactId);
        //TODO: check what to do with groupsChanged
        changeSet.addedContacts().insert(id);
        m_contactsAddedEmitted.append(id);
    }
    transformError(err, qtError);

	return (err==KErrNone);
}

/*!
 * Update an existing contact entry in the database.
 *
 * \param contact The contact to update in the database.
 * \param qtError Qt error code.
 * \return Error status.
 */
bool QContactSymbianEngineData::updateContact(QContact& contact, QContactChangeSet& changeSet, QContactManager::Error& qtError)
{
    int err(0);
    TRAP(err, QT_TRYCATCH_LEAVING(updateContactL(contact)));
    if(err == KErrNone)
    {
        //TODO: check what to do with groupsChanged
        //changeSet.changedContacts().insert(contact.localId());
        m_contactsChangedEmitted.append(contact.localId());
    }
    transformError(err, qtError);
    return (err==KErrNone);
}


/*!
 * Remove the specified contact object from the database.
 *
 * The removal of contacts from the underlying contacts model database
 * is performed in transactions of maximum 50 items at a time. E.g.
 * deleting 177 contacts would be done in 3 transactions of 50 and a
 * final transaction of 27.
 *
 * \param contact The QContact to be removed.
 * \param qtError Qt error code.
 * \return Error status.
 */
bool QContactSymbianEngineData::removeContact(const QContactLocalId &id, QContactChangeSet& changeSet, QContactManager::Error& qtError)
{
    // removeContactL() can't throw c++ exception
	TRAPD(err, removeContactL(id));
    if(err == KErrNone)
    {
        //TODO: check what to do with groupsChanged?
        changeSet.removedContacts().insert(id);
        m_contactsRemovedEmitted.append(id);
    }
    transformError(err, qtError);
	return (err==KErrNone);
}
#if 0
/* groups */

/*!
 * Return a list of group UIDs.
 *
 * \param qtError Qt error code.
 * \return List of group IDs.
 */
QList<QContactLocalId> QContactSymbianEngineData::groups(QContactManager::Error& qtError) const
{
    // See QT_TRYCATCH_LEAVING note at the begginning of this file
    QContactLocalIdList *list = new QContactLocalIdList();
    TRAPD(err, QT_TRYCATCH_LEAVING(*list = groupsL()));
    transformError(err, qtError);
    return *QScopedPointer<QContactLocalIdList>(list);
}

/*!
 * Retrieve a contact group object corresponding to the supplied UID.
 *
 * \param groupId The id of the contact to be retrieved.
 * \param qtError Qt error code.
 * \return The contact group object.
 */
QContactGroup QContactSymbianEngineData::group(const QContactLocalId& groupId, QContactManager::Error& qtError) const
{
    // See QT_TRYCATCH_LEAVING note at the begginning of this file
    QContactGroup *qGroup = new QContactGroup();
    TRAPD(err, QT_TRYCATCH_LEAVING(*qGroup = groupL(groupId)));
    transformError(err, qtError);
    return *QScopedPointer<QContactGroup>(qGroup);
}

/*!
 * Add the supplied contact group to the database.
 *
 * \param group The contact group object to be saved.
 * \changeSet The ids for the contacts and groups that were modified.
 * \param qtError Qt error code.
 * \return a bool indicating whether the operation was successful.
 */
bool QContactSymbianEngineData::addGroup(QContactGroup& group, QContactChangeSet& changeSet, QContactManager::Error& qtError)
{
    int err(0);
    TRAP(err, QT_TRYCATCH_LEAVING(addGroupL(group)));
    if(err == KErrNone)
    {
        // TODO: should we signal contactsChanged in some cases?
        changeSet.changedGroups().insert(group.id());
        m_contactsAddedEmitted.append(group.id());
    }
	transformError(err, qtError);
	return (err==KErrNone);
}

/*!
 * Update the contact group data to the database.
 *
 * \param group The contact group object to be updated.
 * \changeSet The ids for the contacts and groups that were modified.
 * \param qtError Qt error code.
 * \return a bool indicating whether the operation was successful.
 */
bool QContactSymbianEngineData::updateGroup(QContactGroup& group, QContactChangeSet& changeSet, QContactManager::Error& qtError)
{
    int err(0);
    TRAP(err, QT_TRYCATCH_LEAVING(updateGroupL(group)));
    if(err == KErrNone)
    {
        // TODO: should we signal contactsChanged in some cases?
        changeSet.changedGroups().insert(group.id());
        m_contactsChangedEmitted.append(group.id());
    }
    transformError(err, qtError);
    return (err==KErrNone);
}

/*!
 * Remove the contact group corresponding to the supplied UID from the database.
 *
 * \param groupId The id of contact group to be removed.
 * \changeSet The ids for the contacts and groups that were modified.
 * \param qtError Qt error code.
 * \return a bool indicating whether the operation was successful.
 */
bool QContactSymbianEngineData::removeGroup(const QContactLocalId& groupId, QContactChangeSet& changeSet, QContactManager::Error& qtError)
{
    // removeGroupL() can't throw c++ exception
	TRAPD(err, removeGroupL(groupId));
    if(err == KErrNone)
    {
        // TODO: should we signal contactsChanged in some cases?
        changeSet.removedGroups().insert(groupId);
        m_contactsRemovedEmitted.append(groupId);
    }
	transformError(err, qtError);
	return (err == KErrNone);
}
#endif
/*!
 * Return the contact group ID of the built-in SIM phonebook contacts group
 *
 * \return a unique id relating the SIM phonebook group or 0 if it fails
 */
QContactLocalId QContactSymbianEngineData::simPhonebookGroupId() const
{
        QContactLocalId val = 0;
	// simPhonebookGroupIdL() can't throw c++ exception
	TRAP_IGNORE(val = simPhonebookGroupIdL());
	return val;
}

/*!
 * Assigns some contact to be a "my card" contact
 *
 * \param contactId The id of contact to be set up as a "my card" contact.
 * \param qtError Qt error code.
 * \return Operation success status.
 */
bool QContactSymbianEngineData::setSelfContactId(const QContactLocalId& contactId, QContactManager::Error& qtError)
{
    TContactItemId id(contactId);
    CContactItem* symContact = 0;
    TRAPD(err,
        symContact = m_contactDatabase->ReadContactL(id);
        m_contactDatabase->SetOwnCardL(*symContact);
        );
    delete symContact;
    transformError(err, qtError);
    return (err==KErrNone);
}

/*!
 * Returns an Id of the "my card" contact
 *
 * \param qtError Qt error code.
 * \return Id of the "my card" contact.
 */
QContactLocalId QContactSymbianEngineData::selfContactId(QContactManager::Error& qtError) const
{
   qtError = QContactManager::NoError;
   QContactLocalId id(m_contactDatabase->OwnCardId());
   return id;
}

/*!
 * Respond to a contacts database event, delegating this event to
 * an appropriate signal as required.
 *
 * \param aEvent Contacts database event describing the change to the
 *  database.
 */
void QContactSymbianEngineData::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
{
        // TODO: Conversion to/from QContactLocalId

    TContactItemId id = aEvent.iContactId;

	switch (aEvent.iType)
	{
    case EContactDbObserverEventContactAdded:
        if(m_contactsAddedEmitted.contains(id))
            m_contactsAddedEmitted.removeOne(id);
        else
            emit contactAdded(id);
        break;
    case EContactDbObserverEventContactDeleted:
        if(m_contactsRemovedEmitted.contains(id))
            m_contactsRemovedEmitted.removeOne(id);
        else
            emit contactRemoved(id);
        break;
	case EContactDbObserverEventContactChanged:
        if(m_contactsChangedEmitted.contains(id))
            m_contactsChangedEmitted.removeOne(id);
        else
            emit contactChanged(id);
        break;
	case EContactDbObserverEventGroupAdded:
        if(m_contactsAddedEmitted.contains(id))
            m_contactsAddedEmitted.removeOne(id);
        else
            emit groupAdded(id);
		break;
	case EContactDbObserverEventGroupDeleted:
        if(m_contactsRemovedEmitted.contains(id))
            m_contactsRemovedEmitted.removeOne(id);
        else
            emit groupRemoved(id);
        break;
	case EContactDbObserverEventGroupChanged:
        if(m_contactsChangedEmitted.contains(id))
            m_contactsChangedEmitted.removeOne(id);
        else
            emit groupChanged(id);
        break;
	default:
		break; // ignore other events
	}
}

/*! Transform a Symbian contact error id to QContactManager::Error.
 *
 * \param symbianError Symbian error.
 * \param QtError Qt error.
*/
void QContactSymbianEngineData::transformError(TInt symbianError, QContactManager::Error& qtError)
{
	switch(symbianError)
	{
		case KErrNone:
		{
			qtError = QContactManager::NoError;
			break;
		}
		case KErrNotFound:
	    {
	        qtError = QContactManager::DoesNotExistError;
	        break;
	    }
        case KErrAlreadyExists:
        {
            qtError = QContactManager::AlreadyExistsError;
            break;
        }
        case KErrLocked:
        {
            qtError = QContactManager::LockedError;
            break;
        }
        case KErrAccessDenied:
        {
            qtError = QContactManager::PermissionsError;
            break;
        }
        case KErrNoMemory:
        {
            qtError = QContactManager::OutOfMemoryError;
            break;
        }
        case KErrNotSupported:
        {
            qtError = QContactManager::NotSupportedError;
            break;
        }
        case KErrArgument:
        {
            qtError = QContactManager::BadArgumentError;
            break;
        }
		default:
		{
			qtError = QContactManager::UnspecifiedError;
			break;
		}
	}
}

/*!
 * Private leaving implementation for contact()
 */
QContact QContactSymbianEngineData::contactL(const QContactLocalId &contactId) const
{
    // A contact with a zero id is not expected to exist.
    // Symbian contact database uses id 0 internally as the id of the
    // system template.
    if(contactId == 0)
        User::Leave(KErrNotFound);

    // Read the contact from the CContactDatabase
	TContactItemId id(contactId);
	CContactItem* symContact = m_contactDatabase->ReadContactL(id);
	CleanupStack::PushL(symContact);

	// Convert to a QContact
	QContact contact = m_transformContact->transformContactL(*symContact, *m_contactDatabase);

	// Read group membership
    //QList<QContactLocalId> groups = memberOfGroupsL(id);
    //contact.setGroups(groups);

    CleanupStack::PopAndDestroy(symContact);

	return contact;
}

/*!
 * Private leaving implementation for count()
 */
int QContactSymbianEngineData::countL() const
{
	// Call CountL and return number of contacts in database
	return m_contactDatabase->CountL();
}

/*!
 * Private leaving implementation for addContact()
 *
 * \param contact The contact item to save in the database.
 * \return The new contact ID.
 */
int QContactSymbianEngineData::addContactL(QContact &contact)
{
    // Create a new contact card.
    CContactCard* contactItem = CContactCard::NewLC();

    // convert QContact to CContactItem
    m_transformContact->transformContactL(contact, *contactItem);

	// Add to the database
	int id = m_contactDatabase->AddNewContactL(*contactItem);
	CleanupStack::PopAndDestroy(contactItem);

    // Update group memberships to contact database
	//updateMemberOfGroupsL(contact);

    // Return the new ID.
	return id;
}

/*!
 * Private leaving implementation for updateContact()
 *
 * \param contact The contact to update in the database.
 */
void QContactSymbianEngineData::updateContactL(QContact &contact)
{
    // Need to open the contact for write, leaving this item
    // on the cleanup stack to unlock the item in the event of a leave.
    CContactItem* contactItem = m_contactDatabase->OpenContactLX(contact.localId());
    CleanupStack::PushL(contactItem);

    // Copy the data from QContact to CContactItem
    m_transformContact->transformContactL(contact, *contactItem);

    // Write the entry using the converted  contact
    // note commitContactL removes empty fields from the contact
    m_contactDatabase->CommitContactL(*contactItem);

    // retrieve the contact in case of empty fields that have been removed, this could also be handled in transformcontact.
    contact = contactL(contact.localId());

    // Update group memberships to contact database
    //updateMemberOfGroupsL(contact);

    CleanupStack::PopAndDestroy(contactItem);
    CleanupStack::PopAndDestroy(1); // commit lock
}

#if 0
/*!
 * Updates group memberships of a QContact to the contact database.
 * Does not change anything in the contact database if the group memberships have not changed.
 *
 * \param contact The contact to update.
 *
 */
void QContactSymbianEngineData::updateMemberOfGroupsL(QContact contact)
{
    QSet<QContactLocalId> oldGroups = QSet<QContactLocalId>::fromList(memberOfGroupsL(contact.id()));
    QSet<QContactLocalId> newGroups = QSet<QContactLocalId>::fromList(contact.groups());
    QSet<QContactLocalId> toBeRemoved = oldGroups - newGroups;
    QSet<QContactLocalId> membersToAdd = newGroups - oldGroups;

    // Remove group memberships that have been removed
    foreach (QContactLocalId groupId, toBeRemoved)
    {
        m_contactDatabase->RemoveContactFromGroupL(TContactItemId(contact.id()), TContactItemId(groupId));
    }

    // add any new memberships
    foreach (QContactLocalId groupId, membersToAdd)
    {
        m_contactDatabase->AddContactToGroupL(TContactItemId(contact.id()), TContactItemId(groupId));
    }
}
#endif
/*!
 * Private leaving implementation for removeContact
 */
int QContactSymbianEngineData::removeContactL(QContactLocalId id)
{
    // A contact with a zero id is not expected to exist.
    // Symbian contact database uses id 0 internally as the id of the
    // system template.
	if(id == 0)
	    User::Leave(KErrNotFound);

    //TODO: in future QContactLocalId will be a class so this will need to be changed.
    TContactItemId cId = static_cast<TContactItemId>(id);
#if 0
    QList<QContactLocalId> groupIds = memberOfGroupsL(cId);
    foreach (QContactLocalId id, groupIds)
    {
        m_contactDatabase->RemoveContactFromGroupL(TContactItemId(cId), TContactItemId(id));
    }
#endif
	m_contactDatabase->DeleteContactL(cId);


	return 0;
}

#if 0
/*!
 * Private leaving implementation for groups
 */
QList<QContactLocalId> QContactSymbianEngineData::groupsL() const
{
        QList<QContactLocalId> list;
	CContactIdArray* cIdList = m_contactDatabase->GetGroupIdListL();
	CleanupStack::PushL(cIdList);
	const int count = cIdList->Count();
	for (int i = 0; i < count; ++i)
	{
                list.append(QContactLocalId((*cIdList)[i]));
	}
	CleanupStack::PopAndDestroy(cIdList);
	return list;
}

/*!
 * Private leaving implementation for group
 */
QContactGroup QContactSymbianEngineData::groupL(const QContactLocalId& groupId) const
{
	// try to fetch a group item of that id
	CContactGroup* cGroup = fetchCGroup(groupId);
	if (!cGroup)
	{
		User::Leave(KErrNotFound);
	}

	QContactGroup qGroup;
	qGroup.setId(groupId);

	// set name, it has one
	if (cGroup->HasItemLabelField())
	{
		TPtrC cName = cGroup->GetGroupLabelL();
		QString qName = QString::fromUtf16(cName.Ptr(), cName.Length());
		qGroup.setName(qName);
	}

	// add members, if there are any
	const CContactIdArray* members = cGroup->ItemsContained(); // does not take ownership
	const int count = members->Count();
	for (int i = 0; i < count; ++i)
	{
                qGroup.addMember(QContactLocalId((*members)[i]));
	}

	return qGroup;
}

/*!
 * Private leaving implementation for addGroup
 */
void QContactSymbianEngineData::addGroupL(QContactGroup& group)
{
    // Using addGroupL with a zero id is not legal.
    // This function is not used by any client, so it is best to assert in such
    // error case.
    ASSERT(group.id() == 0);

    if(group.name().isNull() || group.name().isEmpty()) {
        User::Leave( KErrArgument );
    }

    // following line works at time of writing as QContactLocalIdIterator is a typedef of QList<QContactLocalId>
        QSet<QContactLocalId> newMembers = QSet<QContactLocalId>::fromList(group.members());
        QSet<QContactLocalId> membersToAdd;

    // get the name, create a new group in the database and set the id
    TPtrC label(reinterpret_cast<const TUint16*>(group.name().utf16()));
    CContactGroup* cGroup = static_cast<CContactGroup*>(m_contactDatabase->CreateContactGroupLC(label));
    group.setId(QContactLocalId(cGroup->Id()));
    CleanupStack::PopAndDestroy(cGroup); // no longer needed...
    membersToAdd = newMembers;

	// add any new members
        foreach (QContactLocalId id, membersToAdd)
	{
		m_contactDatabase->AddContactToGroupL(TContactItemId(id), TContactItemId(group.id()));
	}
}

/*!
 * Private leaving implementation for updateGroup
 */
void QContactSymbianEngineData::updateGroupL(QContactGroup& group)
{
    // Using updateGroupL with a non-zero id is not legal.
    // This function is not used by any client, so it is best to assert in such
    // error case.
    ASSERT(group.id() != 0);

    if(group.name().isNull() || group.name().isEmpty()) {
        User::Leave( KErrArgument );
    }

    // following line works at time of writing as QContactLocalIdIterator is a typedef of QList<QContactLocalId>
    QSet<QContactLocalId> newMembers = QSet<QContactLocalId>::fromList(group.members());
    QSet<QContactLocalId> membersToAdd;

    CContactGroup* cGroup = fetchCGroup(group.id());
    if (!cGroup)
    {
        // if the group has an id but is not in the database something is wrong
        User::Leave(KErrNotFound);
    }
    CleanupStack::PushL(cGroup);

    // if the name has changed, update the label
    TPtrC newLabel(reinterpret_cast<const TUint16*>(group.name().utf16()));
    if (cGroup->GetGroupLabelL().CompareC(newLabel)) // 0/false if unchanged
    {
        CContactGroup* labelGroup = static_cast<CContactGroup*>(m_contactDatabase->OpenContactLX(TContactItemId(group.id())));
        CleanupStack::PushL(labelGroup);
        cGroup->SetGroupLabelL(newLabel);
        m_contactDatabase->CommitContactL(*cGroup);
        CleanupStack::PopAndDestroy(labelGroup);
        CleanupStack::PopAndDestroy(1); // lock object
    }

    // get a list of the old members from the database
    CContactIdArray* oldIds = cGroup->ItemsContainedLC();

    // build set of old members from list of ids in the database's version of the group
    QSet<QContactLocalId> oldMembers;
    const int count = oldIds->Count();
    for (int i = 0; i < count; ++i)
    {
        oldMembers.insert(QContactLocalId((*oldIds)[i]));
    }
    CleanupStack::PopAndDestroy(oldIds);
    oldIds = NULL;

    //remove old members from the database that no longer are in the group
    QSet<QContactLocalId> membersToDelete = oldMembers - newMembers;
    foreach (QContactLocalId id, membersToDelete)
    {
        m_contactDatabase->RemoveContactFromGroupL(TContactItemId(id), TContactItemId(group.id()));
    }

    // get a list of the new members that need adding
    // we can ignore members that haven't changed -- i.e. intersection(oldMembers, newMembers)
    membersToAdd = newMembers - oldMembers;

    // add any new members
    foreach (QContactLocalId id, membersToAdd)
    {
        m_contactDatabase->AddContactToGroupL(TContactItemId(id), TContactItemId(group.id()));
    }
    CleanupStack::PopAndDestroy(cGroup);
}

/*!
 * Private leaving implementation for removeGroup
 */
void QContactSymbianEngineData::removeGroupL(const QContactLocalId& groupId)
{
	// try to fetch a contact item of that id and check it's a group object
	if (!isGroup(groupId))
	{
		User::Leave(KErrNotFound); // no group of this id found
	}
	m_contactDatabase->DeleteContactL(TContactItemId(groupId));
}

/*!
 * Private leaving implementation for simPhonebookGroupId
 */
QContactLocalId QContactSymbianEngineData::simPhonebookGroupIdL() const
{
	return m_contactDatabase->PhonebookGroupIdL();
}

/*!
 * Lists groups the contact is member of currently in the contact database.
 */
QList<QContactLocalId> QContactSymbianEngineData::memberOfGroupsL(
        const TContactItemId contactId) const
{
    QList<QContactLocalId> list;
    CContactIdArray* groupIdList = m_contactDatabase->GetGroupIdListL();
    CleanupStack::PushL(groupIdList);
    for (int i = 0; i < groupIdList->Count(); ++i)
    {
        CContactItem* cGroup = m_contactDatabase->ReadContactL(TContactItemId((*groupIdList)[i]));
        CleanupStack::PushL(cGroup);
        if(static_cast<CContactGroup*>(cGroup)->ContainsItem(contactId))
        {
            list.append(cGroup->Id());
        }
        CleanupStack::PopAndDestroy(cGroup);
    }
    CleanupStack::PopAndDestroy(groupIdList);
    return list;
}

/* Utility functions */

/*!
 * Fetches a CContactGroup from the database
 *
 * @param id an id of a prospective group
 * @return a pointer to a CContactGroup or NULL if it's not a group
 */
CContactGroup* QContactSymbianEngineData::fetchCGroup(const QContactLocalId& id) const
{
	// try to fetch a contact item of that id and turn it into a group object
	CContactItem* cItem = NULL;
	TRAPD(err, cItem = m_contactDatabase->ReadContactL(TContactItemId(id)));
	if (!err && cItem->Type() != KUidContactGroup)
	{
		delete cItem; // not a group so get rid of it
		cItem = NULL;
	}
	return static_cast<CContactGroup*>(cItem);
}

/*!
 * Checks an id is a contact group in the database
 *
 * @param id an id of a prospective group
 * @return bool indicating whether it a group
 */
bool QContactSymbianEngineData::isGroup(const QContactLocalId& id) const
{
	CContactGroup* group = fetchCGroup(id);
	if (!group)
	{
		return false;
	}
	delete group;
	return true;
}
#endif
