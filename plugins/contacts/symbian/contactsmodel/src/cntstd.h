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



#if !defined(__CNTSTD_H__)
#define __CNTSTD_H__

#if !defined(__E32DEF_H__)
#include <e32def.h>
#endif

const TInt KNumberOfNewTables = 8;//Used for discovering if we're opening an old version of cntmodel

_LIT(KContactTableName,"CONTACTS");
_LIT(KContactGroupTableName,"GROUPS");
_LIT(KContactGroupTableNameByGroup,"GROUPS2");
_LIT(KContactPrefTableName,"PREFERENCES");
_LIT(KContactPhoneMatchingTableName,"PHONE");
_LIT(KSyncTableName,"SYNC");

_LIT(KIdentifierColName,"CM_Id");
_LIT(KLastLastSyncColName,"CM_LastSyncDate");
_LIT(KUidStringColName,"CM_UIDString");
_LIT(KTypeColName,"CM_Type");
_LIT(KTemplateRefId,"CM_PrefTemplateRefId");
_LIT(KLastModifiedColName,"CM_Last_modified");
_LIT(KCreationDateColName,"CM_ContactCreationDate");
_LIT(KAttributesColName,"CM_Attributes");
_LIT(KTextFieldsColName,"CM_TextBlob");
_LIT(KFieldHeaderColName, "CM_Header");
_LIT(KSearchableTextColName,"CM_SearchableText");
_LIT(KPhoneMatchingColName,"CM_PhoneMatching");
_LIT(KPhoneMatchingColNameTwo,"CM_ExtendedPhoneMatching");
_LIT(KItemsContainedColName,"CM_Members");
_LIT(KGroupMembersName,"CM_GroupMembers");
_LIT(KReplicationCountColName,"CM_ReplicationCount");
_LIT(KPrefId,"CM_PrefId");
_LIT(KPrefTemplateId,"CM_PrefTemplateId");
_LIT(KPrefTemplateFileVersion,"CM_PrefFileVer");
_LIT(KPrefFileId,"CM_PrefFileId");
_LIT(KContactTableIdIndex,"cnt_id_index");
_LIT(KSyncTableIdIndex,"sync_id_index");
_LIT(KContactGroupIndex,"cnt_group_index");
_LIT(KContactGroupIndexByGroupId,"cnt_group_index2");
_LIT(KContactPhoneMatchingIndex,"cnt_phone_index");
_LIT(KPrefCreationDate,"CM_creationdate");
_LIT(KPrefOwnCardId,"CM_PrefOwnCardId");
_LIT(KPrefCardTemplateId,"CM_PrefCardTemplateId");
_LIT(KPrefGroupIdList,"CM_PrefGroupIdList");
_LIT(KPrefCardTemplatePrefId,"CM_PrefCardTemplatePrefId");
_LIT(KPrefMachineUniqueId,"CM_MachineUID");
_LIT(KPrefSortOrder,"CM_PrefSortOrder");
_LIT(KContactHintField,"CM_HintField");

_LIT(KContactExtHintField, "CM_ExtHintField");

//LPD//Identity TABLE
_LIT(KIdentityTableName, "IDENTITYTABLE");
_LIT(KParentCMID, "Parent_CMID");
_LIT(KFirstNameColName,"CM_FirstName");
_LIT(KLastNameColName,"CM_LastName");
_LIT(KCompanyNameColName,"CM_CompanyName");
_LIT(KFirstNamePronunciationColName,"CM_FirstNmPrn");
_LIT(KLastNamePronunciationColName,"CM_LastNmPrn");
_LIT(KCompanyNamePronunciationColName,"CM_CompanyNmPrn");
_LIT(KIdentityIdIndex, "IdentityIdIndex");

//Email Table
_LIT(KEMailTableName, "EMAILTABLE");
_LIT(KEMailParentCMID, "EmailParent_CMID");///
_LIT(KEMailFieldID, "EMail_FieldID");////Together these match a unique id.
_LIT(KEMailAddress,"EMailAddress");
_LIT(KEMailIdentityIdIndex, "EmailIdentityIdIndex");

//SQL Literals
_LIT(KASC,"ASC");
_LIT(KDESC,"DESC");

//Asych Sort Consts
//Values for sort window
const TInt KSortWindowForeSlots=5;
const TInt KSortWindowRearSlots=0;

//const TInt KMaxPhoneNumberLenghForSearch=50;
_LIT(KSelectStringFromEmailTable,"SELECT EmailParent_CMID FROM EMAILTABLE WHERE EMailAddress LIKE '%S'");
_LIT(KSelectAddressFromEmailTableByCntID,"SELECT EMailAddress FROM EMAILTABLE WHERE EmailParent_CMID = %d");

//Find String Sql lits
//_LIT(KSelectStringFromIdentityTable,"SELECT Parent_CMID FROM IDENTITYTABLE WHERE (CM_FirstName LIKE '%S') OR (CM_LastName LIKE '%S') OR (CM_CompanyName LIKE '%S')");
_LIT(KSelectStringFromIdentityTable,"SELECT * FROM IDENTITYTABLE WHERE (CM_FirstName LIKE '%S') OR (CM_LastName LIKE '%S') OR (CM_CompanyName LIKE '%S') OR (CM_FirstNmPrn LIKE '%S') OR (CM_LastNmPrn LIKE '%S') OR (CM_CompanyNmPrn LIKE '%S')");

//_LIT(KSelectStringFromIdentityTableWhere,"SELECT Parent_CMID FROM IDENTITYTABLE WHERE ");
_LIT(KSelectStringFromIdentityTableWhere,"SELECT * FROM IDENTITYTABLE WHERE ");

_LIT(KSelectAllIdsFromIdentity,"SELECT Parent_CMID, CM_Type FROM IDENTITYTABLE");//Gives an unsorted list of contact ids for every contact
_LIT(KSelectStringForTextDefSearch,"SELECT Parent_CMID,CM_FirstName,CM_LastName,CM_CompanyName,CM_FirstNmPrn,CM_LastNmPrn,CM_CompanyNmPrn,CM_Type FROM IDENTITYTABLE WHERE ");

_LIT(KLeftBracket,"(");
_LIT(KLikeString," LIKE '");
_LIT(KLikeStringRight,"')");
_LIT(KOR," OR ");

_LIT(KNullBlobTextField," ");



_LIT(KSqlDeleteFromTableFormat,"DELETE FROM %S WHERE %S=%d");


enum TCntModelPanic
	{
	ECntPanicInvalidColumnName,
	ECntPanicInvalidColumnIndex,
	ECntPanicInvalidItemType,
	ECntPanicInvalidItemIndex,
	ECntPanicDbmsResourceNotFreed,
	ECntPanicNullFieldDefinition,
	ECntPanicInvalidFieldContentType,
	ECntPanicViewIsEmpty,
	ECntPanicDuplicateViewFields,
	ECntPanicFieldDoesNotExist,
	ECntPanicNullPointer,
	ECntPanicDuplicateIds,
	ECntPanicInternalSortError,
	ECntPanicUnhandledUid,
	ECntPanicFieldTypeIndex,
	ECntPanicInvalidStorageType,
	ECntPanicStorageAlreadyAllocated,
	ECntPanicServerNotFound,
	ECntPanicSystemTemplate,
	ECntPanicAgentNotNull,
	ECntPanicTemplateNotEmpty,
	ECntPanicItemAlreadyExists,
	ECntPanicInvalidSortArray,
	ECntPanicPhoneMatchLength,
	ECntPanicDuffGroup,
	ECntPanicInvalidSpeedDialContactId,
	ECntPanicInvalidSpeedDialIndex,
	ECntPanicInvalidViewIndex,
	ECntPanicInvalidIndexForSubView,
	ECntPanicInvalidIndexForConcatenatedView,
	ECntPanicViewNotReady,
	ECntPanicNoViewIndexMatchCriteria,
	ECntPanicUnderlyingViewUnsorted,
	ECntPanicAsyncViewNotificationPending,
	ECntPanicObserverNotFound,
	ECntPanicComponentViewNotFound,
	ECntPanicInvalidFieldText,
	ECntPanicFieldTextNotInitialised,
	ECntPanicUnableToRestartServer,
	ECntPanicFilterableFieldMaxNumberInUse,
	ECntPanicFieldTextNotBlank,
	ECntPanicInvalidConnectionId,
	ECntPanicAPIRemoved,
	ECntPanicDuplicateBackupEvent,
	ECntPanicDuplicateRestoreEvent,
	ECntPanicUnexpectedEndofRestoreEvent,
	ECntPanicFieldBufferNotAllocated,
	ECntPanicFieldBufferAllocError,
	ECntPanicFieldBufferWrongSize,
	ECntPanicViewSorterStateMachine,
	ECntPhonebookSyncObserverList,
	ECntPanicContactsLeftLocked,
	ECntPanicLast
	};

GLREF_C void Panic(TCntModelPanic aPanic);



#endif
