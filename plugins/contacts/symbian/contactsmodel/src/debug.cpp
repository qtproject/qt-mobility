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


#include <cntdbobs.h>
#include "ccntipccodes.h"
#include <cntviewbase.h>

#if defined(__VERBOSE_DEBUG__)	

#define NM_CASE(x) case x: enumStr = _S(#x); break;

void DebugLogViewNotification(const TDesC& aMethod, const TContactViewEvent& aEvent)
	{
	const TText* enumStr;
	switch (aEvent.iEventType)
		{
		NM_CASE(TContactViewEvent::EUnavailable);
		NM_CASE(TContactViewEvent::EReady);
		NM_CASE(TContactViewEvent::ESortOrderChanged);
		NM_CASE(TContactViewEvent::ESortError);
		NM_CASE(TContactViewEvent::EServerError);
		NM_CASE(TContactViewEvent::EIndexingError);
		NM_CASE(TContactViewEvent::EItemAdded);
		NM_CASE(TContactViewEvent::EItemRemoved);
		NM_CASE(TContactViewEvent::EGroupChanged);
		default: enumStr = _S("UnsupportedEvent");
		}
	RDebug::Print(_L("%S: %s, ContactID: %d, IndexCode: %d \n"), 
			&aMethod,
			enumStr,
			aEvent.iContactId,
			aEvent.iInt);	
	}



void DebugLogNotification(const TDesC& aMethod, const TContactDbObserverEvent &aEvent)
	{
	const TText* enumStr;

	switch (aEvent.iType)
		{//case EContactDbObserverEventContactChanged: eventTypeStr = _S("EContactDbObserverEventContactChanged");
		NM_CASE( EContactDbObserverEventNull);
		NM_CASE( EContactDbObserverEventUnused);
		NM_CASE( EContactDbObserverEventContactChanged);
		NM_CASE( EContactDbObserverEventContactDeleted);
		NM_CASE( EContactDbObserverEventContactAdded);
		NM_CASE( EContactDbObserverEventUnknownChanges);
		NM_CASE( EContactDbObserverEventRecover);
		NM_CASE( EContactDbObserverEventRollback);
		NM_CASE( EContactDbObserverEventTablesClosed);
		NM_CASE( EContactDbObserverEventTablesOpened);
		NM_CASE( EContactDbObserverEventTemplateChanged);
		NM_CASE( EContactDbObserverEventTemplateDeleted);
		NM_CASE( EContactDbObserverEventTemplateAdded);
		NM_CASE( EContactDbObserverEventCurrentItemDeleted);
		NM_CASE( EContactDbObserverEventCurrentItemChanged);
		NM_CASE( EContactDbObserverEventOwnCardChanged);
		NM_CASE( EContactDbObserverEventPreferredTemplateChanged);
		NM_CASE( EContactDbObserverEventOwnCardDeleted);
		NM_CASE( EContactDbObserverEventGroupAdded);
		NM_CASE( EContactDbObserverEventGroupChanged);
		NM_CASE( EContactDbObserverEventGroupDeleted);
		NM_CASE( EContactDbObserverEventCurrentDatabaseChanged);
		NM_CASE( EContactDbObserverEventSpeedDialsChanged);
		NM_CASE( EContactDbObserverEventSortOrderChanged);
		NM_CASE( EContactDbObserverEventBackupBeginning);
		NM_CASE( EContactDbObserverEventRestoreBeginning);
		NM_CASE( EContactDbObserverEventBackupRestoreCompleted);
		NM_CASE( EContactDbObserverEventRestoreBadDatabase);
		default: enumStr = _S("UnsupportedEvent");
		}

	RDebug::Print(_L("%S: Message: %s, ContactID: %d, ConnectionId: %d \n"), 
			&aMethod,
			enumStr,
			aEvent.iContactId,
			aEvent.iConnectionId);	
	}

void DebugLogIPC(const TDesC& aMethod, TInt aOpCode, TUint aSessionId, TInt aErrCode)
	{
	const TText* enumStr;

	switch (aOpCode)
		{//case ECntCloseView: enumStr = _S("ECntCloseView");
		NM_CASE( ECntCloseView);
		NM_CASE( ECntSendPluginUidToServer);
		NM_CASE( ECntContactMatchingCriteriaExternalizedSize);
		NM_CASE( ECntViewAt);									
		NM_CASE( ECntViewCount);								
		NM_CASE( ECntViewFind);								
		NM_CASE( ECntAllFieldsLength);							
		NM_CASE( ECntGetViewSortOrder);
		NM_CASE( ECntRequestViewEvent);							
		NM_CASE( ECntCancelRequestViewEvent);					
		NM_CASE( ECntGetIncludedTypes);							
		NM_CASE( ECntGetContactIds);							
		NM_CASE( ECntGetSortPluginUidFromServer);				
		NM_CASE( ECntGetCurrentItem);							
		NM_CASE( ECntSetOwnCard);											
		NM_CASE( ECntGetOwnCard);									
		NM_CASE( ECntSetHeapFailure);					
		NM_CASE( ECntResourceCount);					
		NM_CASE( ECntFetchTemplateIds);							
		NM_CASE( ECntFetchGroupIdLists);							
		NM_CASE( ECntDbContactCount);								
		NM_CASE( ECntGetDbViewContactType);						
		NM_CASE( ECntSetDbViewContactType);						
		NM_CASE( ECntOpenDataBase);										
		NM_CASE( ECntCloseDataBase);										
		NM_CASE( ECntCloseDbTables);								
		NM_CASE( ECntReOpenDbTables);								
		NM_CASE( EBeginDbTransaction);							
		NM_CASE( EEndDbTransaction);								
		NM_CASE( ERollbackDbTransaction);
		NM_CASE( ECntCancelAsyncOpenDatabase);								
		NM_CASE( ECntGetDefaultDatabaseName);									
		NM_CASE( ECntDatabaseDrive);										
		NM_CASE( ECntDatabaseExists);										
		NM_CASE( ECntListDatabases);										
		NM_CASE( ECntMachineID);									
		NM_CASE( ECntOverrideMachineID);							
		NM_CASE( ECntItemAtL);										
		NM_CASE( ECntGetCurrentDb);							
		NM_CASE( ECntGetSpeedDialContactIdAndPhoneNumber);	
		NM_CASE( ECntGetPrefTemplateId);								
		NM_CASE( ECntSetPrefTemplateId);								
		NM_CASE( ECntGetCollection);									
		NM_CASE( ECntGetSortPrefs);									
		NM_CASE( ECntReadContactTextDef);								
		NM_CASE( ECntCreateView);							
		NM_CASE( ECntCreateNamedView);						
		NM_CASE( ECntViewContactAt);						
		NM_CASE( ECntViewContactAtLength);					
		NM_CASE( ECntAllFieldsText);						
		NM_CASE( ECntGetContactsMatchingFilter);			
		NM_CASE( ECntGetContactMatchingCriteria);			
		NM_CASE( ECntRequestEvent);							
		NM_CASE( ECntCancelEventRequest);					
		NM_CASE( ECntConnectionId);							
		NM_CASE( ECntReplaceDatabase);										
		NM_CASE( ECntReCreateTemplate);								
		NM_CASE( ECntFileUniqueId);									
		NM_CASE( ECntCreateDatabase);											
		NM_CASE( ECntDeleteDatabase);											
		NM_CASE( ECntItemCreate);										
		NM_CASE( ECntItemUpdate);										
		NM_CASE( ECntItemCommit);										
		NM_CASE( ECntItemDelete);										
		NM_CASE( ECntItemRead);										
		NM_CASE( ECntItemOpen);										
		NM_CASE( ECntItemClose);										
		NM_CASE( ECntChangeViewDef);	
		NM_CASE( ECntFilesSize);								
		NM_CASE( ECntSetDatabaseDrive);								
		NM_CASE( ECntSetSortPrefs);							
		NM_CASE( ECntSetCurrentItem);				
		NM_CASE( ECntRemoveCurrentItem);			
		NM_CASE( ECntSetCurrentDb);					
		NM_CASE( ECntSetSpeedDialIdForPosition);	
		NM_CASE( ECntChangeViewSortOrder);
		NM_CASE( ECntTextField);
		NM_CASE( ECntICCTemplateId);
		NM_CASE( ECntPhonebookGroupId);
		NM_CASE( ECntFind);	
		NM_CASE( ECntFindAsyncInit);	
		NM_CASE( ECntFindAsyncTextDefInit);	
		NM_CASE( ECntFindAsync);
		NM_CASE( ECntSetAsyncActivity);
		NM_CASE( ECntGetDatabaseReady);
		NM_CASE( ECntOpenViewSession);
		NM_CASE( ECntCloseViewSession);
    	NM_CASE( ECntViewChangeSortOrderL);
		NM_CASE( ECntViewBeginIterate);
		NM_CASE( ECntViewEndIterate);
		NM_CASE( ECntViewNextItemL);
		default: enumStr = _S("UnsupportedIPC");
		}

	RDebug::Print(_L("%S: %s, SessId: %d, ErrCode: %d\n"), 
		&aMethod, enumStr, aSessionId, aErrCode);	
	}

#endif
