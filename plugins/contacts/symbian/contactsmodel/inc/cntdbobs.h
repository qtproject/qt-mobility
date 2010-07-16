/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#if !defined(__CNTDBOBS_H__)
#define __CNTDBOBS_H__

#if !defined(__E32DEF_H__)
#include <e32def.h>
#endif

#if !( defined __SYMBIAN_CNTMODEL_HIDE_DBMS__ ) && !defined __D32DBMS_H__ 
#include <d32dbms.h>
#else
#include <s32std.h>
#include <f32file.h>
#endif 

#include <cntdef.h> //for tcontactitemid 

/** Change event type. For events which update, delete or add a contact item, the 
ID of the item affected is provided in the TContactDbObserverEvent struct.
@publishedAll
@released
 */
enum TContactDbObserverEventType
	{
	/** Null event type. */
	EContactDbObserverEventNull,
	/** Not currently used. */
	EContactDbObserverEventUnused,
	/** A contact item (not a template, group or own card) has been changed. */
	EContactDbObserverEventContactChanged,
	/** A contact item (not a template, group or own card) has been deleted. */
	EContactDbObserverEventContactDeleted,
	/** A contact item (not a template or group) has been added. */
	EContactDbObserverEventContactAdded,
	/** Unknown change event type. */
	EContactDbObserverEventUnknownChanges,
	/** Recover from a damaged database. */
	EContactDbObserverEventRecover,
	/** Rollback from a change to the database. */
	EContactDbObserverEventRollback,
	/** Database tables closed. */
	EContactDbObserverEventTablesClosed,
	/** Database tables opened. */
	EContactDbObserverEventTablesOpened,
	/** A contact card template has changed. */
	EContactDbObserverEventTemplateChanged,
	/** A contact card template has been deleted. */
	EContactDbObserverEventTemplateDeleted, 
	/** A contact card template has been added. */
	EContactDbObserverEventTemplateAdded,
	/** The database's current item has been deleted. */
	EContactDbObserverEventCurrentItemDeleted,
	/** The database's current item has changed. */
	EContactDbObserverEventCurrentItemChanged,
	/** The database's own card has changed. */
	EContactDbObserverEventOwnCardChanged,
	/** The database's preferred template has been set. (See CContactDatabase::SetPrefTemplateL()). */
	EContactDbObserverEventPreferredTemplateChanged,
	/** An own card has been deleted from the database. */
	EContactDbObserverEventOwnCardDeleted,
	/** A contact item group has been added. */
	EContactDbObserverEventGroupAdded,
	/** A contact item group has been updated. */
	EContactDbObserverEventGroupChanged,
	/** A contact item group has been deleted from the database. */
	EContactDbObserverEventGroupDeleted,
	/** The current database has changed. */
	EContactDbObserverEventCurrentDatabaseChanged,
	/** The phone number assigned to one of the speed dial positions 
	has been replaced, removed or modified.*/
	EContactDbObserverEventSpeedDialsChanged, 
	/** Not currently used. */
	EContactDbObserverEventSortOrderChanged,
	/** Contacts DB about to be backed up  */
	EContactDbObserverEventBackupBeginning,
	/** Contacts DB about to be restored */
	EContactDbObserverEventRestoreBeginning,
	/** The backup/restore operation has completed. */
	EContactDbObserverEventBackupRestoreCompleted,
	/** The restore operation has completed but the database could not be opened. */
	EContactDbObserverEventRestoreBadDatabase,
	/** Database has been compressed. */
	EContactDbObserverEventCompress
	};

/** Extended change event type. Includes addtional database events client can 
subsribe for. 
@publishedAll
@released
 */
enum TContactDbObserverEventTypeV2
    {
    /** Null event type. */
    EContactDbObserverEventV2Null,
    /** Group has been updated, some contact was added to a group. */
    EContactDbObserverEventV2ContactAddedToGroup,
    /** Group has been updated, some contact was removed from a group. */
    EContactDbObserverEventV2ContactRemovedFromGroup,
    /** Group has been updated (other change than added or removed contacts). */
    EContactDbObserverEventV2GroupChanged
    };

struct TContactDbObserverEvent
/** Contact database change event details. These include the type of change event 
and the ID of the contact item affected by the change, if relevant.

An object of this type is passed to a contact database's change observer, 
see MContactDbObserver::HandleDatabaseEventL(). 
@publishedAll
@released
*/
	{
	/** The change event type. */
	TContactDbObserverEventType iType;
	/** The ID of the contact item affected. */
	TContactItemId iContactId;
	/** Identifies which connection to the contacts server generated the event. Use 
	CContactDatabase::ConnectionId() to get the ID of the current connection.
	
	Set to zero if the event has no meaningful connection ID. */
	TUint iConnectionId;
	};

struct TContactDbObserverEventV2 : public TContactDbObserverEvent
/** Contact database change event details. In addtion to TContactDbObserverEvent, 
it includes extended event type and ID of the affected contact, e.g. ID of the
contact added to a group or removed from a group. 

An object of this type is passed to a contact database's group change observer,
see MContactDbObserverV2::HandleDatabaseEventV2L(). 
@publishedAll
@released
*/
    {
    /** The change event type. */
    TContactDbObserverEventTypeV2 iTypeV2;
    
    /** The ID of the affected contact. */
    TContactItemId iAdditionalContactId;
    };

class MContactDbObserver
/** The interface for an observer that handles changes to a contact database.

It specifies a single pure virtual function which should be implemented by 
the observer class to test the type of the change event (see the TContactDbObserverEventType 
enum) and handle it.

There is no need for the observer class to explicitly test for all possible 
event types - the event is for information only and event types which are of no interest 
to the observer class can safely be ignored.

The change event type (TContactDbObserverEventType) may be augmented 
in future releases with additional values.

A contact database observer pointer is passed to the NewL() function for the 
CContactChangeNotifier class. 
@publishedAll
@released
*/
	{
public:
	/** Tests the contact database observer event type and handles it. The ID of a 
	contact affected by the change event, if relevant, can be retrieved via TContactDbObserverEvent::iContactId.
	
	@param aEvent Provides information about the change event. */
	virtual void HandleDatabaseEventL(TContactDbObserverEvent aEvent)=0;
	};

class MContactDbObserverV2
/** The interface for an observer that handles changes to a contact database.

It specifies a single pure virtual function which should be implemented by 
the observer class to test the type of the change event (see the TContactDbObserverEventType
and TContactDbObserverEventTypeV2 enums) and handle it.

@publishedAll
@released
*/
    {
public:
    /** Tests the contact database observer event type and handles it. The ID of the 
    contact(s) affected by the change event, if relevant, can be retrieved via 
    TContactDbObserverEvent::iContactId and TContactDbObserverEventV2::iAdditionalContactId.
    
    @param aEvent Provides information about the change event. */
    virtual void HandleDatabaseEventV2L(TContactDbObserverEventV2 aEvent)=0;
    };

#endif
