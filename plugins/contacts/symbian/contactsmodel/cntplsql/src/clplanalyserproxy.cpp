/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent
 @released
*/


#include "clplanalyserproxy.h"
#include <cntitem.h>
#include <cntfldst.h>

/**
CLplAnalyserProxy constructor.
*/
CLplAnalyserProxy::CLplAnalyserProxy(MIniFileManager& aIniManager, MContactDbObserverV2& aCntDbObserver, MLplPersistenceBroker& aBroker, MLplTransactionManager& aTranMan, MLplContactProperties& aProperties)
	:
	CLplGenericProxy(aBroker, aTranMan),
	iCntDbObserver(aCntDbObserver),
	iIniManager(aIniManager),
	iProperties(aProperties)
	{
	}


/**
Create a new Contact item.
*/
TContactItemId CLplAnalyserProxy::CreateL(CContactItem& aItem, TUint aSessionId)
	{
	#if defined(__PROFILE_DEBUG__)
		RDebug::Print(_L("[CNTMODEL] MTD: CLplAnalyserProxy::CreateL"));
	#endif 

	// Delegate creation to generic proxy.
	TContactItemId cntId = CLplGenericProxy::CreateL(aItem, aSessionId); 

	// Notify observer of the change.
	switch(aItem.Type().iUid)
		{
		case KUidContactCardTemplateValue:
			{
			NotifyObserverL(EContactDbObserverEventTemplateAdded, aItem.Id());	
			}
			break;
		case KUidContactGroupValue:
			{
			NotifyObserverL(EContactDbObserverEventGroupAdded, aItem.Id());	
			}
			break;
		case KUidContactCardValue:
		case KUidContactOwnCardValue:
		case KUidContactICCEntryValue:
			{
			NotifyObserverL(EContactDbObserverEventContactAdded, aItem.Id());	
			}
			break;
		default:
			break;
		};		

	return(cntId);
	}


/**
Update an existing Contact item (or parts of it as specified by the view
definition).
*/
void CLplAnalyserProxy::UpdateL(CContactItem& aItem, TUint aSessionId, TBool aSpeeDailUpdate)
	{
	// Delegate update to generic proxy.
	CLplGenericProxy::UpdateL(aItem, aSessionId);

	// Notify observer of the change.
	switch(aItem.Type().iUid)
		{
		case KUidContactTemplateValue:
		case KUidContactCardTemplateValue:
			{
			NotifyObserverL(EContactDbObserverEventTemplateChanged, aItem.Id());	
			}
			break;
		case KUidContactOwnCardValue:
			{
			NotifyObserverL(EContactDbObserverEventOwnCardChanged, aItem.Id());	
			// If an item with a speed dial is updated then update the speed
			// dials.
			(void)CheckSpeedDialUpdatesL(aItem);
			}
			break;
		case KUidContactGroupValue:
			{
			// send basic group change event..
            NotifyObserverL(EContactDbObserverEventGroupChanged, aItem.Id(), 0,
                EContactDbObserverEventV2Null, 0);
			
            // ... and extended group change event
            CContactGroup& groupItem = static_cast<CContactGroup&>(aItem);
            bool groupMemberListUpdated = false;
			if (groupItem.iAddedContactIds != NULL && groupItem.iAddedContactIds->Count() > 0)
			    {
			    //some contacts were added
			    groupMemberListUpdated = true;
			    for (int i = 0; i < groupItem.iAddedContactIds->Count(); i++)
			        {
	                NotifyObserverL(EContactDbObserverEventNull, aItem.Id(), 0,
	                    EContactDbObserverEventV2ContactAddedToGroup, groupItem.iAddedContactIds->operator[](i));			        
			        }
			    }
			if (groupItem.iRemovedContactIds != NULL && groupItem.iRemovedContactIds->Count() > 0)
	            {
	            //some contacts were removed
	            groupMemberListUpdated = true;
                for (int i = 0; i < groupItem.iRemovedContactIds->Count(); i++)
                    {
	                NotifyObserverL(EContactDbObserverEventNull, aItem.Id(), 0,
	                    EContactDbObserverEventV2ContactRemovedFromGroup, groupItem.iRemovedContactIds->operator[](i));                    
	                }
	            }
            if (!groupMemberListUpdated)
                {
                NotifyObserverL(EContactDbObserverEventNull, aItem.Id(), 0,
                        EContactDbObserverEventV2GroupChanged, 0);
                }			
			}
			break;
		case KUidContactCardValue:
		case KUidContactICCEntryValue:
			{
			// If an item with a speed dial is updated then update the speed
			// dials.
			if (!CheckSpeedDialUpdatesL(aItem) || !aSpeeDailUpdate)
				{
				NotifyObserverL(EContactDbObserverEventContactChanged, aItem.Id());				
    			}
			}
			break;
		default:
			break;
		};
	}


/**
Delete a Contact item.
*/
CContactItem* CLplAnalyserProxy::DeleteLC(TContactItemId  aItemId, TUint aSessionId, TCntSendEventAction aEventType)
	{
	// Delegate delete to generic proxy.
	CContactItem* item = CLplGenericProxy::DeleteLC(aItemId, aSessionId, aEventType); 

	// Notify observer of the change.
	switch(item->Type().iUid)
		{
		case KUidContactCardTemplateValue:
			{
			NotifyObserverL(EContactDbObserverEventTemplateDeleted, item->Id());
			// If the prefered template is deleted then do the appropriate
			// follow up actions.
			CheckPrefTemplateDeleteL(aItemId);
			}
			break;
		case KUidContactOwnCardValue:
			{						
			if (aEventType == ESendEvent)
 			    {
			    NotifyObserverL(EContactDbObserverEventOwnCardDeleted, item->Id());	
 			    }							
 			else if (aEventType == ESendUnknownChangesEvent)
 				{
 				NotifyObserverL(EContactDbObserverEventUnknownChanges, KNullContactId);
 				}
 				
			// If an item with a speed dial is deleted then update the speed
			// dials.
			CheckSpeedDialDeletesL(aItemId);
			// Resets the Own Card ID to be KNullContactId.
			iProperties.SetOwnCardIdL(KNullContactId);
			}
			break;
		case KUidContactGroupValue:
			{
			NotifyObserverL(EContactDbObserverEventGroupDeleted, item->Id());	
			}
			break;
		case KUidContactCardValue:
		case KUidContactICCEntryValue:
			{
			// If an item with a speed dial is deleted then update the speed
			// dials.
			(void)CheckSpeedDialDeletesL(aItemId);
			if (aEventType == ESendEvent)
 			    {
			    NotifyObserverL(EContactDbObserverEventContactDeleted, item->Id());
 			    }							
 			else if (aEventType == ESendUnknownChangesEvent)
 				{
 				NotifyObserverL(EContactDbObserverEventUnknownChanges, KNullContactId);
 				}					
			}
			break;
		default:
			break;
		};

	// Check if we deleted the current item (managed by initialisation file
	// manager).
	iIniManager.DeleteNotifyL(aItemId);

	return(item);
	}


/**
Change the type of an existing Contact item.
*/
void CLplAnalyserProxy::ChangeTypeL(TContactItemId aItemId, TUid aNewType)
	{
	// Delegate type change to generic proxy.
	CLplGenericProxy::ChangeTypeL(aItemId, aNewType);

	// If a Contact item is changed to be the Own Card then we need to notify
	// observers of the change.
	if (aNewType.iUid == KUidContactOwnCardValue)
		{
		NotifyObserverL(EContactDbObserverEventOwnCardChanged, aItemId);	
		}
	}


/**
Set the connection ID to be used when notifying observer of a database event
(unless a specific connection ID is specified when calling NotifyObserverL()).
*/
void CLplAnalyserProxy::SetConnectionId(TInt aConnectionId)
	{
	iConnectionId = aConnectionId;	
	}


/**
Notify observer of database event.
*/
void CLplAnalyserProxy::NotifyObserverL(const TContactDbObserverEventType aType,
        const TContactItemId aContactId, const TUint aConnectionId,
        const TContactDbObserverEventTypeV2 aTypeV2, const TContactItemId aAdditionalContactId)
	{
	#if defined(__PROFILE_DEBUG__)
		RDebug::Print(_L("[CNTMODEL] MTD: CLplAnalyserProxy::NotifyObserverL"));
	#endif 
	
	TContactDbObserverEventV2 event;
	event.iType = aType;
	event.iContactId = aContactId;

	if(!aConnectionId)
		{
		event.iConnectionId = iConnectionId;		
		}
	else
		{
		event.iConnectionId = aConnectionId;		
		}
	
	event.iTypeV2 = aTypeV2;
	event.iAdditionalContactId = aAdditionalContactId;

	iCntDbObserver.HandleDatabaseEventV2L(event);	

	// Reset connection ID.
	iConnectionId = 0;
	}

	
TBool CLplAnalyserProxy::CheckSpeedDialUpdatesL(CContactItem& aItem)
	{
	TBool retValue = EFalse;

	// Check if the specified Contact ID is present in the speed dial array.
	// The method returns the number of occurances: if zero then this contact
	// is not in the speed dial table.
	CArrayFix<TInt>* speedDialIndices = iIniManager.SpeedDialIndicesForContactIdLC(aItem.Id());

	// Because this contact has one or more speed dial entries we need to ensure
	// that the server-side speed dial array is kept up to date with any changes
	// by the client. 
	for(TInt count = speedDialIndices->Count() -1 ; count>=0; --count)
		{
		// So there is at least one speed dial with the particular contact ID.
		// Fetch speed dial index from array returned by server.
		TInt speedDialIndex = speedDialIndices->At(count);
		// Fetch phone number associated with speed dial position.
		TUid fieldTypeUid = SpeedDialFieldUidFromSpeedDialPosition(speedDialIndex);
		TInt fieldPos = aItem.CardFields().Find(fieldTypeUid);
		
		// Check if the phone number field associated with the speed dial has
		// been updated.
		if	(fieldPos >= KErrNone)
			{		
			// Get the requested field (i.e. the field containing the number for
			// the speed dial).
			CContactItemField& speeddialField = aItem.CardFields()[fieldPos];
			// Check if it is a text field otherwise leave.
			if (speeddialField.StorageType() != KStorageTypeText)
				{
				User::Leave(KErrUnknown);		
				}
			// Fetch phone number associated with speed dial position.
			// Truncate it if its length is > KSpeedDialPhoneLength 
			TInt numLen = Min(speeddialField.TextStorage()->Text().Length(), KSpeedDialPhoneLength );
			TPtrC phoneNumber(speeddialField.TextStorage()->Text().Mid(0, numLen));
			// Update the server with the value of this speed dial.
			iIniManager.SetSpeedDialIdForPositionL(speedDialIndex, aItem.Id(), phoneNumber, iConnectionId, ETrue);
			retValue = ETrue;
			}
		else
			{
			// The phone number field associated with the speed dial has been
			// deleted.
			iIniManager.SetSpeedDialIdForPositionL(speedDialIndex, KNullContactId, KNullDesC(), iConnectionId, ETrue);
			retValue = ETrue;
			}
		}

	CleanupStack::PopAndDestroy(speedDialIndices);

	return retValue;
	}


TBool CLplAnalyserProxy::CheckSpeedDialDeletesL(TContactItemId aItemId)
	{
	TBool retValue = EFalse;
	// Check if the specified contact ID is present in the speed dial array.
	// The method returns the number of occurances: if zero then this contact
	// is not in the speed dial table.	
	CArrayFix<TInt>* speedDialIndices = iIniManager.SpeedDialIndicesForContactIdLC(aItemId);	
	
	// Because this contact has one or more speed dial entries we need to ensure
	// that the server-side speed dial array is kept up to date with any changes
	// by the client. 
	for(TInt count = speedDialIndices->Count() -1 ; count>=0; --count)
		{
		// Fetch speed dial index.
		TInt speedDialIndex = speedDialIndices->At(count);
		// Update the speed dial.
		iIniManager.SetSpeedDialIdForPositionL(speedDialIndex, KNullContactId, KNullDesC(), iConnectionId, ETrue);
		retValue = ETrue;
		}	

	CleanupStack::PopAndDestroy(speedDialIndices);

	return retValue;
	}


void CLplAnalyserProxy::CheckPrefTemplateDeleteL(TContactItemId aItemId)
	{
	if (iProperties.CardTemplatePrefIdL() == aItemId)
		{
		// Make sure the Own Card ID is persisted and up to date.
		iProperties.SetCardTemplatePrefIdL(KNullContactId);
		// There was no notification in the old contacts model but there
		// probably should have been.
		NotifyObserverL(EContactDbObserverEventPreferredTemplateChanged, aItemId);	
		}
	}


/**
Static utility method to map a speed dial index onto a field type UID.

Note: this method is duplicated, should be refactored into shared code/library.
*/
TUid CLplAnalyserProxy::SpeedDialFieldUidFromSpeedDialPosition(TInt aSpeedDialPosition)
	{
	TUid fieldTypeUid = KNullUid;
	switch (aSpeedDialPosition)
		{
	case 1:
		fieldTypeUid = KUidSpeedDialOne;
		break;
	case 2:
		fieldTypeUid = KUidSpeedDialTwo;
		break;
	case 3:
		fieldTypeUid = KUidSpeedDialThree;
		break;
	case 4:
		fieldTypeUid = KUidSpeedDialFour;
		break;
	case 5:
		fieldTypeUid = KUidSpeedDialFive;
		break;
	case 6:
		fieldTypeUid = KUidSpeedDialSix;
		break;
	case 7:
		fieldTypeUid = KUidSpeedDialSeven;
		break;
	case 8:
		fieldTypeUid = KUidSpeedDialEight;
		break;
	case 9:
		fieldTypeUid = KUidSpeedDialNine;
		break;
		}
	return fieldTypeUid; 
	}


/**
Rollback the current transation in the Persistence Layer.
*/
void CLplAnalyserProxy::RollbackCurrentTransactionL(TUint aSessionId)
	{
	// Delegate rollback to generic proxy.
	CLplGenericProxy::RollbackCurrentTransactionL(aSessionId);
	// Notify observer of the rollback.
	NotifyObserverL(EContactDbObserverEventRollback, 0);	
	}
