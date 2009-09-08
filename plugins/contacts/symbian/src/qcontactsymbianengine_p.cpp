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

#include <qtcontacts.h>
#include <QSet>


QContactSymbianEngineData::QContactSymbianEngineData() :
    m_contactDatabase(0),
    m_transformContact(0)
{
    QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION(m_contactDatabase = CContactDatabase::OpenL());
    QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION(m_contactDatabase->AddObserverL(*this));
	
	m_transformContact = new TransformContact;
}

QContactSymbianEngineData::~QContactSymbianEngineData()
{
	if (m_contactDatabase != 0) {
	    m_contactDatabase->RemoveObserver(*this);
	}
	delete m_contactDatabase;
	delete m_transformContact;
}

/* Access */

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
QContact QContactSymbianEngineData::contact(const QUniqueId& contactId, QContactManager::Error& qtError) const
{
	QContact contact;
	TRAPD(err, QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE(contact = contactL(contactId)));
	transformError(err, qtError);
	return contact;
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
 * \param qtError Qt error code.
 * \return List of all IDs for contact entries in the database,
 *  or an empty list if there was a problem or the database is
 *  empty.
 */
QList<QUniqueId> QContactSymbianEngineData::contacts(QContactManager::Error& qtError) const
{
	// Create an empty list
	QList<QUniqueId> ids;
	
	// Attempt to read from database, leaving the list empty if
	// there was a problem
	TRAPD(err, QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE(ids = contactsL()));
	transformError(err, qtError);

	return ids;
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
bool QContactSymbianEngineData::addContact(QContact& contact, int &id, QContactManager::Error& qtError)
{
	// Attempt to persist contact, trapping errors
	TRAPD(err, QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE(id = addContactL(contact)));
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
bool QContactSymbianEngineData::updateContact(QContact& contact, QContactManager::Error& qtError)
{
    TRAPD(err, QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE(updateContactL(contact)));
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
bool QContactSymbianEngineData::removeContact(const QUniqueId &id, QContactManager::Error& qtError)
{
    // removeContactL() can't throw c++ exception
	TRAPD(err, removeContactL(id));
    transformError(err, qtError);
	return (err==KErrNone);
}


/*!
 * Remove the contact objects specified in ids from the database.
 *   
 * \param ids A list contact ids to be removed.
 * \param qtError Qt error code. 
 * \return Error status.
 */
bool QContactSymbianEngineData::removeContacts(const QList<QUniqueId> &ids, QContactManager::Error& qtError)
{
    // removeContactsL() can't throw c++ exception
	TRAPD(err, removeContactsL(ids));
    transformError(err, qtError);
    return (err==KErrNone);
}


/* match */

/*!
 * Find a contact entry in the database with the given communication address.
 * 
 * \param communicationType The kind of communication address to look for; e.g.
 *  telephone numbers or email etc.
 * \param communicationAddress The address to search for in the contacts database.
 * 
 * \return A list of QContact objects with the specificed communication address.
 */
QList<QUniqueId> QContactSymbianEngineData::matchCommunicationAddress(const QString &communicationType, const QString &communicationAddress)
{
	// Create an empty list to return on failure
	QList<QUniqueId> matches;
	
	// Attempt to match address, list will remain empty if there's an error
	TRAP_IGNORE(QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE(matches = matchCommunicationAddressL(communicationType, communicationAddress)));
	
	return matches;
}

/* groups */

/*!
 * Return a list of group UIDs.
 *  
 * \param qtError Qt error code. 
 * \return List of group IDs.
 */
QList<QUniqueId> QContactSymbianEngineData::groups(QContactManager::Error& qtError) const
{
	QList<QUniqueId> list;
	TRAPD(err, QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE(list = groupsL()));
	transformError(err, qtError);
	return list;
}

/*!
 * Retrieve a contact group object corresponding to the supplied UID.
 *  
 * \param groupId The id of the contact to be retrieved.
 * \param qtError Qt error code. 
 * \return The contact group object.
 */
QContactGroup QContactSymbianEngineData::group(const QUniqueId& groupId, QContactManager::Error& qtError) const
{
	QContactGroup qGroup;
	TRAPD(err, QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE(qGroup = groupL(groupId)));
    transformError(err, qtError);
	return qGroup;
}

/*!
 * Save the supplied contact group to the database.
 * 
 * \param group The contact group object to be saved.
 * \param qtError Qt error code. 
 * \return a bool indicating whether the operation was successful.
 */
bool QContactSymbianEngineData::saveGroup(QContactGroup& group, QContactManager::Error& qtError)
{
	TRAPD(err, QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE(saveGroupL(group)));
	transformError(err, qtError);
	return (err==KErrNone); 
}

/*!
 * Remove the contact group corresponding to the supplied UID from the database.
 * 
 * \param groupId The id of contact group to be removed.
 * \param qtError Qt error code. 
 * \return a bool indicating whether the operation was successful.
 */
bool QContactSymbianEngineData::removeGroup(const QUniqueId& groupId, QContactManager::Error& qtError)
{
    // removeGroupL() can't throw c++ exception
	TRAPD(err, removeGroupL(groupId));
	transformError(err, qtError);
	return (err == KErrNone);
}

/*!
 * Return the contact group ID of the built-in SIM phonebook contacts group
 * 
 * \return a unique id relating the SIM phonebook group or 0 if it fails
 */
QUniqueId QContactSymbianEngineData::simPhonebookGroupId() const
{
	QUniqueId val = 0;
	// simPhonebookGroupIdL() can't throw c++ exception
	TRAP_IGNORE(val = simPhonebookGroupIdL());
	return val;
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
	// TODO: Conversion to/from QUniqueId
	
	switch (aEvent.iType)
	{
	case EContactDbObserverEventContactAdded:
		emit contactAdded(aEvent.iContactId);
		break;
	case EContactDbObserverEventContactDeleted:
		emit contactRemoved(aEvent.iContactId);
		break;
	case EContactDbObserverEventContactChanged:
		emit contactChanged(aEvent.iContactId);
		break;
	case EContactDbObserverEventGroupAdded:
		emit groupAdded(aEvent.iContactId);
		break;
	case EContactDbObserverEventGroupDeleted:
		emit groupRemoved(aEvent.iContactId);
		break;
	case EContactDbObserverEventGroupChanged:
		emit groupChanged(aEvent.iContactId);
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
void QContactSymbianEngineData::transformError(TInt symbianError, QContactManager::Error& qtError) const
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

/*! Transform a Symbian contact to a QContact. Note that the contact ID
 * is not restored by this function, it simply converts the field types
 * and appends them to the new QContact.
 *
 * \param contact A reference to a Symbian contact to be converted.
 * \return A Qt contact with the same fields and content as the CContactItem.
*/
QContact QContactSymbianEngineData::transformContact(CContactItem &contact) const
{
	return m_transformContact->transformContact(contact);
}

/*! Transform a QContact into a Symbian CContactItem. This will set the contact item's
 *  local UID from the value of contact.id() as well as converting all
 *  supported fields to the CContactItem format.
 *
 * \param contact A reference to a QContact to be converted.
 * \return A pointer to a heap allocated CContactItem, ownership transferred.
*/
CContactItem *QContactSymbianEngineData::transformContactL(QContact &contact) const
{
	return m_transformContact->transformContactL(contact);
}


/*!
 * Private leaving implementation for contact()
 */
QContact QContactSymbianEngineData::contactL(const QUniqueId &contactId) const
{
	// Read the contact from the CContactDatabase
	TContactItemId id(contactId);
	CContactItem* symContact = m_contactDatabase->ReadContactL(id);
	CleanupStack::PushL(symContact);
	
	// Convert to a QContact
	QContact contact = transformContact(*symContact);
	
	CleanupStack::PopAndDestroy(symContact);
	return contact;
}

/*!
 * Private leaving implementation for contactIds()
 */
QList<QUniqueId> QContactSymbianEngineData::contactsL() const
{
	TTime epoch(0);
	QList<QUniqueId> qIds;
	
    // Populate the ID array, returns the coontact ids + group ids 
	CContactIdArray *ids = m_contactDatabase->ContactsChangedSinceL(epoch);
	CleanupStack::PushL(ids);
	
	// If there was a problem, return an empty list, otherwise
	// copy the IDs to the QUniqueId list. Note that this works
	// while QUniqueId is a typedef to quint32.
	const int idCount(ids->Count());
	for (int i(0); i < idCount; ++i) {
		qIds.append((*ids)[i]);
	}
	
	CleanupStack::PopAndDestroy(ids);
	
	//remove the groups from the list
	QContactManager::Error err = QContactManager::NoError;
	QList<QUniqueId> groupIds = groups(err);
	int indexOf(0);
	
	if (err==QContactManager::NoError) {
        for(int i = (groupIds.count() - 1); i >= 0; i-- ) {
            indexOf = qIds.lastIndexOf( groupIds.at(i));
        
            if(indexOf != -1){
                qIds.removeAt(indexOf);
            }
        }
	}
	
	return qIds;
}

/*!
 * Private leaving implementation for count()
 */
int QContactSymbianEngineData::countL() const
{
	// Call CountL and return number of contacts in database
	int count(0);
	count = m_contactDatabase->CountL();

	return count;	
}

/*!
 * Private leaving implementation for addContact()
 * 
 * \param contact The contact item to save in the database.
 * \return The new contact ID.
 */
int QContactSymbianEngineData::addContactL(QContact &contact)
{
	// convert to CContactItem
	CContactItem* contactItem = transformContactL(contact);

	// Add to the database
	CleanupStack::PushL(contactItem);
	int id = m_contactDatabase->AddNewContactL(*contactItem);
	CleanupStack::PopAndDestroy(contactItem);
	
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
    // Convert to CContactItem
    CContactItem* contactItem = transformContactL(contact);
    CleanupStack::PushL(contactItem);

    // Need to open the contact for write, leaving this item
    // on the cleanup stack to unlock the item in the event of a leave.
    CContactItem* tmpCntItem = m_contactDatabase->OpenContactLX(contactItem->Id());
    CleanupStack::PushL(tmpCntItem);
    
    // Write the entry using the converted  contact
    m_contactDatabase->CommitContactL(*contactItem);
    
    // Destroy the temp entry and the cleanup item to unlock the contact.
    CleanupStack::PopAndDestroy(3); // tmpContactItem, lock object, contactItem
}

/*!
 * Private leaving implementation for matchCommunicationAddress()
 */
QList<QUniqueId> QContactSymbianEngineData::matchCommunicationAddressL( const QString &/*communicationType*/, const QString &communicationAddress )
{
	QList<QUniqueId> contactIds;
	
	//TODO - for now assume matching is done only for phone numbers and there is a 7bit wide match
	TPtrC commPtr(reinterpret_cast<const TUint16*>(communicationAddress.utf16()));
	CContactIdArray* idsArray = m_contactDatabase->MatchPhoneNumberL(commPtr, 7);
	CleanupStack::PushL( idsArray );
	
	const int noContacts = idsArray->Count();
	for(int loop = 0; loop < noContacts; ++loop)
	{
        contactIds.append(QUniqueId((*idsArray)[loop]));
	}
	
	CleanupStack::PopAndDestroy( idsArray );
	
	return contactIds;
}


/*!
 * Private leaving implementation for removeContact
 */
int QContactSymbianEngineData::removeContactL(QUniqueId id)
{
	ASSERT(m_contactDatabase);
	//TODO: in future QUniqueId will be a class so this will need to be changed.
	TContactItemId cId = static_cast<TContactItemId>(id);
	m_contactDatabase->DeleteContactL(cId);
	return 0;
}

/*!
 * Private leaving implementation for removeContacts
 */
int QContactSymbianEngineData::removeContactsL(const QList<QUniqueId> &ids)
{
	ASSERT(m_contactDatabase);
	const int idCount = ids.count();
	CContactIdArray *cIds = CContactIdArray::NewLC(); 
	for (int i = 0; i < idCount; ++i)
	{
		//TODO: in future QUniqueId will be a class so this will need to be changed.
		cIds->AddL(ids[i]);
	}
	m_contactDatabase->DeleteContactsL(*cIds);
	CleanupStack::PopAndDestroy(cIds);
	return 0;
}


/*!
 * Private leaving implementation for groups
 */
QList<QUniqueId> QContactSymbianEngineData::groupsL() const
{
	QList<QUniqueId> list; 
	CContactIdArray* cIdList = m_contactDatabase->GetGroupIdListL();
	CleanupStack::PushL(cIdList);
	const int count = cIdList->Count();
	for (int i = 0; i < count; ++i)
	{
		list.append(QUniqueId((*cIdList)[i]));
	}
	CleanupStack::PopAndDestroy(cIdList);
	return list;
}

/*!
 * Private leaving implementation for group
 */
QContactGroup QContactSymbianEngineData::groupL(const QUniqueId& groupId) const
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
		qGroup.addMember(QUniqueId((*members)[i]));
		//qGroup.addContact(QUniqueId((*members)[i]));
	}
	
	return qGroup;
}

/*!
 * Private leaving implementation for saveGroup
 */
void QContactSymbianEngineData::saveGroupL(QContactGroup& group)
{
	// following line works at time of writing as QUniqueIdIterator is a typedef of QList<QUniqueId>
	QSet<QUniqueId> newMembers = QSet<QUniqueId>::fromList(group.members());
	QSet<QUniqueId> membersToAdd;
	
	CContactGroup* cGroup = NULL;
	
	if (group.id() > 0) // has already been stored in the database
	{
		cGroup = fetchCGroup(group.id());
		if (!cGroup)
		{
			// if the group has an id but is not in the database something is wrong
			User::Leave(KErrNotFound);
		}
		CleanupStack::PushL(cGroup);
		
		// get a list of the old members from the database
		TPtrC oldLabel(cGroup->GetGroupLabelL());
		CContactIdArray* oldIds = cGroup->ItemsContainedLC();
		CleanupStack::Pop(oldIds); // temporarily remove oldIds
		CleanupStack::PopAndDestroy(cGroup); // no longer needed...
		CleanupStack::PushL(oldIds);
		cGroup = NULL;
		
		// if the name has changed, update the label
		TPtrC newLabel(reinterpret_cast<const TUint16*>(group.name().utf16()));
		if (oldLabel.CompareC(newLabel)) // 0/false if unchanged
		{	
			cGroup = static_cast<CContactGroup*>(m_contactDatabase->OpenContactLX(TContactItemId(group.id())));
			CleanupStack::PushL(cGroup);
			cGroup->SetGroupLabelL(newLabel);
			m_contactDatabase->CommitContactL(*cGroup);
    	    CleanupStack::PopAndDestroy(2); // cGroup and hidden lock object
		} 	
		
		// build set of old members from list of ids in the database's version of the group
		QSet<QUniqueId> oldMembers;
		const int count = oldIds->Count();
		for (int i = 0; i < count; ++i)
		{
			oldMembers.insert(QUniqueId((*oldIds)[i]));
		}
		CleanupStack::PopAndDestroy(oldIds);
		oldIds = NULL;
		
		//remove old members from the database that no longer are in the group
		QSet<QUniqueId> membersToDelete = oldMembers - newMembers;
		foreach (QUniqueId id, membersToDelete)
		{
			m_contactDatabase->RemoveContactFromGroupL(TContactItemId(id), TContactItemId(group.id()));
		}
		
		// get a list of the new members that need adding
		membersToAdd = newMembers - oldMembers;
		
		// we can ignore members that haven't changed -- i.e. intersection(oldMembers, newMembers)
	}
	else
	{
		// get the name, create a new group in the database and set the id
		TPtrC label(reinterpret_cast<const TUint16*>(group.name().utf16()));
		cGroup = static_cast<CContactGroup*>(m_contactDatabase->CreateContactGroupLC(label));
		group.setId(QUniqueId(cGroup->Id()));
		CleanupStack::PopAndDestroy(cGroup); // no longer needed...
		membersToAdd = newMembers;
	}
	
	// add any new members
	foreach (QUniqueId id, membersToAdd)
	{
		m_contactDatabase->AddContactToGroupL(TContactItemId(id), TContactItemId(group.id()));
	}
}

/*!
 * Private leaving implementation for removeGroup
 */
void QContactSymbianEngineData::removeGroupL(const QUniqueId& groupId)
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
QUniqueId QContactSymbianEngineData::simPhonebookGroupIdL() const
{
	return m_contactDatabase->PhonebookGroupIdL();
}

/* Utility functions */

/*! 
 * Fetches a CContactGroup from the database
 * 
 * @param id an id of a prospective group
 * @return a pointer to a CContactGroup or NULL if it's not a group
 */
CContactGroup* QContactSymbianEngineData::fetchCGroup(const QUniqueId& id) const
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
bool QContactSymbianEngineData::isGroup(const QUniqueId& id) const
{
	CContactGroup* group = fetchCGroup(id);
	if (!group)
	{
		return false;
	}
	delete group;
	return true;
}

