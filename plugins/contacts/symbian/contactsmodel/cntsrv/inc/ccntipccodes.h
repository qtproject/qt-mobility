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


#ifndef CCNTIPCCODES_H
#define CCNTIPCCODES_H

#include <e32base.h>


// Enumeration offsets for different types of capabilities.
#define KCapabilityNone 0
#define KCapabilityReadUserData 100
#define KCapabilityWriteUserData 400

// To be removed. Should be defined in a header file
#define KCntSearchResultList 99
#define KCntPredictiveSearchList 98

/**
Contacts database filename maximum length.  Used to police descriptors on Client
side.
*/
const TInt KCntMaxFilePath = 190;

const TInt KErrNoComplete = 0xbb000000;


/**
Client/Server function codes.  One per API call.  Contains subsession codes
also.
*/
enum TCntClientIpcCodes
	{
	// ---- No capability ----
	ECntCloseView = KCapabilityNone,
	ECntSendPluginUidToServer,
	ECntContactMatchingCriteriaExternalizedSize,
	ECntViewSortOrderExternalizedSize,
	ECntViewAt,
	ECntViewCount,
	ECntViewFind,
	ECntAllFieldsLength,
	ECntGetViewSortOrder,
	ECntRequestViewEvent,
	ECntCancelRequestViewEvent,
	ECntGetIncludedTypes,
	ECntGetContactIds,
	ECntGetSortPluginUidFromServer,
	ECntGetCurrentItem,
	ECntSetHeapFailure,
	ECntResourceCount,
	ECntDbContactCount,
	ECntGetDbViewContactType, // New but not used.
	ECntSetDbViewContactType,
	ECntViewContactAtLength,
	ECntRequestEvent,
	ECntCancelEventRequest,
	ECntConnectionId,
	ECntMachineID,
	ECntOverrideMachineID,
	ECntGetPrefTemplateId,
	ECntICCTemplateId,
	ECntPhonebookGroupId,
	ECntCloseDataBase,
	ECntCancelAsyncOpenDatabase,
	ECntFileUniqueId,
	ECntOpsTimeOut,
	ECntFilesSize,
	ECntMatchesHintField,
	ECntGetDefaultDatabaseName,
	ECntGetCurrentDb,
	ECntDatabaseDrive,
	ECntDatabaseExists,
	ECntSetSortPrefs,
	ECntGetOwnCard,
	ECntFetchTemplateIds,
	ECntFetchGroupIdLists,
	ECntSeekContactInCollection,
	ECntChangeViewDef,
	ECntGetSortPrefs,
	ECntItemClose,
	ECntGetDefinitionsForExistingView, // Test only.
	ECntSetAsyncActivity,
	ECntGetDatabaseReady,
	ECntOpenViewSession,
	ECntCloseViewSession,
    ECntViewChangeSortOrderL,
    ECntSearchResultList = KCntSearchResultList,  // Do not change this enum value
                                                  // and do not add anything immediately
                                                  // after this.
    ECntPredictiveSearchList = KCntPredictiveSearchList,     // Do not change this enum value
                                                    // and do not add anything immediately
                                                    // after this.
                                                    // ---- Read User Data capability ----
	ECntOpenDataBase= KCapabilityReadUserData,
	ECntReOpenDbTables,
	ECntCreateView,
	ECntCreateNamedView,
	ECntViewContactAt,
	ECntAllFieldsText,
	ECntGetContactsMatchingFilter,
	ECntGetContactMatchingCriteria,
	ECntGetSpeedDialContactIdAndPhoneNumber,
	ECntItemRead,
	ECntReadContactTextDef,
	ECntFilterDatabase,
	ECntFind,
	ECntFindAsyncInit,
	ECntFindAsyncTextDefInit,
	ECntFindAsync,
	ECntItemAtL,
	ECntGetCollection,
	ECntTextField,
	ECntListDatabases,
	ECntViewBeginIterate,
	ECntViewEndIterate,
	ECntViewNextItemL,

	// ---- Write User Data capability ----
	ECntCreateDatabase = KCapabilityWriteUserData,
	ECntSetPrefTemplateId,
	ECntSetOwnCard,
	ECntReCreateTemplate,
	ECntSetCurrentItem,
	ECntRemoveCurrentItem,
	ECntSetCurrentDb,
	ECntSetSpeedDialIdForPosition,
	ECntChangeViewSortOrder,
	ECntItemOpen,
	ECntSetDatabaseDrive,
	ECntReplaceDatabase,
	ECntDeleteDatabase,
	ECntItemUpdate,
	ECntItemCommit,
	ECntItemDelete,
	ECntItemCreate,
	EBeginDbTransaction,
	EEndDbTransaction,
	ERollbackDbTransaction,
	ECntCloseDbTables,
	ELockSrvNotSupported
	

		// Not supported
	};


/**
Set to an out of range drive value to indicate all drives.
*/
enum TCntDrives {ECntAllDrives = 26};


#endif
