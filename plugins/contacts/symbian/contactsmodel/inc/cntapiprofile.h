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

#if !defined(SYMBIAN_CNT_API_PROFILE)
#define SYMBIAN_CNT_API_PROFILE


// is profile logging enabled?
#if defined(CONTACTS_API_PROFILING)

// for RProcess & RThread
#include <e32std.h>

// Contact Views
#include <cntviewbase.h>


class CContactDbApiTimer;

class TContactsApiProfile
	{
public:
	enum TCntViewClass
		{
		ECntVwClassViewBase,			// "CContactViewBase"
		ECntVwClassLocalView,			// "CContactLocalView"
		ECntVwClassNamedLocalView,		// "CContactNamedLocalView"
		ECntVwClassRemoteViewBase,		// "CContactRemoteViewBase"
		ECntVwClassRemoteView,			// "CContactRemoteView"
		ECntVwClassNamedRemoteView,		// "CContactNamedRemoteView"
		};

	enum TCntViewApi
		{
		ECntViewDestructor,						// "~..."
		// create API
		ECntViewApiNewL,						// "NewL"

		ECntViewApiAllFieldsLC,					// "AllFieldsLC"
		ECntViewApiAtL,							// "AtL"
		ECntViewApiChangeSortOrderL,			// "ChangeSortOrderL"
		ECntViewApiClose,						// "Close"
		ECntViewApiConstructL,					// "ConstructL"
		ECntViewApiContactAtL,					// "ContactAtL"
		ECntViewApiContactViewBase_Reserved_1,	// "ContactViewBase_Reserved_1"
		ECntViewApiContactViewPreferences,		// "ECntViewApiContactViewPreferences"
		ECntViewApiCountL,						// "CountL"
		ECntViewApiFieldsWithSeparatorLC,		// "FieldsWithSeparatorLC"
		ECntViewApiFindL,						// "FindL"
		ECntViewApiGetContactIdsL,				// "GetContactIdsL"
		ECntViewApiGetSortOrderL,				// "GetSortOrderL"
		ECntViewApiGetViewFindConfigPlugin,		// "GetViewFindConfigPlugin"
		ECntViewApiMatchesCriteriaL,			// "MatchesCriteriaL"
		ECntViewApiNotifyObserverAsync,			// "NotifyObserverAsync"
		ECntViewApiNotifyObservers,				// "NotifyObservers"
		ECntViewApiOpen,						// "Open"
		ECntViewApiOpenL,						// "OpenL"
		ECntViewApiSetViewFindConfigPlugin,		// "SetViewFindConfigPlugin"
		ECntViewApiSortL,						// "SortL"
		ECntViewApiSortOrder,					// "SortOrder"
		ECntViewApiUpdateGroupViewL,			// "UpdateGroupViewL"
		};

	enum TCntDbApi
		{
		ECntDbDestructor,					// "~CContactDatabase"
		// create APIs - these create a CContactDatabase object plus NewL/NewLC
		ECntDbApiNewL,						// "NewL/LC"
		ECntDbApiCreateL,					// "CreateL"
		ECntDbApiOpen,						// "Open"
		ECntDbApiOpenL,						// "OpenL"
		ECntDbApiReplaceL,					// "ReplaceL"

		ECntDbApiDoOpenL,					// client side Open

		// most other APIs (omits some very frequently called APIs)
		ECntDbApiAddNewContactL,			// "AddNewContactL"
		ECntDbApiAddContactToGroupL,		// "AddContactToGroupL"

		ECntDbApiCancelAsyncSort,			// "CancelAsyncSort"
		ECntDbApiCloseContactL,				// "CloseContactL"
		ECntDbApiCommitContactL,			// "CommitContactL"
		ECntDbApiCompactL,					// "CompactL"
		ECntDbApiCompressRequired,			// "CompressRequired"
		ECntDbApiConnectionId,				// "ConnectionId"
		ECntDbApiContactDatabaseExistsL,	// "ContactDatabaseExistsL"
		ECntDbApiContactMatchesHintFieldL,	// "ContactMatchesHintFieldL"
		ECntDbApiContactsChangedSinceL,		// "ContactsChangedSinceL"
		ECntDbApiCountL,					// "CountL"

		ECntDbApiCreateCompressorLC,		// "CreateCompressorLC"
		ECntDbApiCreateContactCardTemplateL,	// "CreateContactCardTemplateL/LC"
		ECntDbApiCreateContactGroupL,		// "CreateContactGroupL/LC"
		ECntDbApiCreateRecoverLC,			// "CreateRecoverLC"
		ECntDbApiCreateOwnCardL,			// "CreateOwnCardL/LC"

		ECntDbApiDatabaseBeginL,			// "DatabaseBeginL"
		ECntDbApiDatabaseCommitL,			// "DatabaseCommitL"
		ECntDbApiDatabaseDrive,				// "DatabaseDrive"
		ECntDbApiDatabaseRollback,			// "DatabaseRollback"

		ECntDbApiDeleteContactL,			// "DeleteContactL"
		ECntDbApiDeleteContactsL,			// "DeleteContactsL"
		ECntDbApiDeleteDatabaseL,			// "DeleteDatabaseL"
		ECntDbApiDeletedContactsLC,			// "DeletedContactsLC"
		ECntDbApiDeleteDefaultFileL,		// "DeleteDefaultFileL"

		ECntDbApiExportSelectedContactsL,	// "ExportSelectedContactsL"

		ECntDbApiFileSize,					// "FileSize"
		ECntDbApiFilterDatabaseL,			// "FilterDatabaseL"
		ECntDbApiFindContactFile,			// "FindContactFile"

		ECntDbApiGetCardTemplateIdListL,	// "GetCardTemplateIdListL"
		ECntDbApiGetCurrentDatabase,		// "GetCurrentDatabase"
		ECntDbApiGetCurrentItem,			// "GetCurrentItem"
		ECntDbApiGetDefaultNameL,			// "GetDefaultNameL"
		ECntDbApiGetGroupIdListL,			// "GetGroupIdListL"
		ECntDbApiGetLastSyncDateL,			// "GetLastSyncDateL"
		ECntDbApiGetSpeedDialFieldL,		// "GetSpeedDialFieldL"

		ECntDbApiHandleDatabaseEventL,		// "HandleDatabaseEventL"

		ECntDbApiICCTemplateIdL,			// "ICCTemplateIdL"
		ECntDbApiImportContactsL,			// "ImportContactsL"
		ECntDbApiIsDamaged,					// "IsDamaged"

		ECntDbApiListDatabasesL,			// "ListDatabasesL"
		ECntDbApiLockRecordLC,				// internal method "LockRecordLC"

		ECntDbApiMachineId,					// "MachineId"

		ECntDbApiOpenContactL,				// "OpenContactL/LX"
		ECntDbApiOverrideMachineUniqueId,	// "OverrideMachineUniqueId"

		ECntDbApiPhonebookGroupIdL,			// "PhonebookGroupIdL"

		ECntDbApiReadContactAndAgentL,		// "ReadContactAndAgentL"
		ECntDbApiReadContactL,				// "ReadContactL/LC"
		ECntDbApiReadContactTextDefL,		// "ReadContactTextDefL"
		ECntDbApiReadMinimalContactL,		// "ReadMinimalContactL/LC"

		ECntDbApiRecoverL,					// "RecoverL"
		ECntDbApiRecreateSystemTemplateL,	// "RecreateSystemTemplateL"
		ECntDbApiRemoveContactFromGroupL,	// "RemoveContactFromGroupL"
		ECntDbApiRemoveSpeedDialFieldL,		// "RemoveSpeedDialFieldL"
		ECntDbApiResetServerSpeedDialsL,	// "ResetServerSpeedDialsL"
		ECntDbApiRestoreSortOrderL,			// "RestoreSortOrderL"

		ECntDbApiSetDatabaseDriveL,			// "SetDatabaseDriveL"
		ECntDbApiSetDbViewContactType,		// "SetDbViewContactType"
		ECntDbApiSetCurrentDatabase,		// "SetCurrentDatabase"
		ECntDbApiSetCurrentItem,			// "SetCurrentItem")
		ECntDbApiSetFieldAsSpeedDialL,		// "SetFieldAsSpeedDialL"
		ECntDbApiSetLastSyncDateL,			// "SetLastSyncDateL"
		ECntDbApiSetOwnCardL,				// "SetOwnCardL"
		ECntDbApiSetPrefTempateL,			// "SetPrefTempateL"
		ECntDbApiSetSortedItemsList,		// "SetSortedItemsList"
		ECntDbApiSetViewDefinitionL,		// "SetViewDefinitionL"

		ECntDbApiSortArrayL,				// "SortArrayL/LC"
		ECntDbApiSortAsyncL,				// "SortAsyncL"
		ECntDbApiSortByTypeL,				// "SortByTypeL"
		ECntDbApiSortL,						// "SortL"
		ECntDbApiSortedItemsL,				// "SortedItemsL"

		ECntDbApiStoreSortOrderL,			// "StoreSortOrderL"

		ECntDbApiTemplateId,				// "TemplateId"

		ECntDbApiUnfiledContactsL,			// "UnfiledContactsL"
		ECntDbApiUpdateContactLC,			// "UpdateContactLC"

		ECntDbApiVersion,					// "Version"

		ECntDbApiWastedSpaceInBytes,		// "WastedSpaceInBytes"
		};


	// Logging for CContactDatabase APIs:
	// line 1 = time, PID, Thread Id, method(..);
	// if (aProcessDetail) then line 2 = Proc Name, Thread Name

	// log API method call only
	static	void CntDbMethodLog(const TCntDbApi& aApiId, TBool aProcessDetail=EFalse);
	static	void CntDbMethodLog(const TCntDbApi& aApiId, const TContactItemId aContactId, TBool aProcessDetail=EFalse);
	static	void CntDbMethodLog(const TCntDbApi& aApiId, const TDriveUnit aDriveUnit, TBool aProcessDetail=EFalse);
	static	void CntDbMethodLog(const TCntDbApi& aApiId, const TDesC& aFileName, TBool aProcessDetail=EFalse);
	static	void CntDbMethodLog(const TCntDbApi& aApiId, const TContactItemId aContactItemId, const TContactItemId aContactGroupId, TBool aProcessDetail=EFalse);
	static	void CntDbMethodLog(const TCntDbApi& aApiId, const TContactDbObserverEvent &aDbEvent, TBool aProcessDetail=EFalse);

	// CContactOpen APIs
	static	void CntOpenOpMethodLog(const TDesC& aMethodName, TBool aProcessDetail=EFalse);

	// CContactXyzView classes
	static	void CntViewMethodLog(const TCntViewClass& aViewClass, const TCntViewApi& aViewApi);
	static	void CntViewMethodLog(const TCntViewClass& aViewClass, const TCntViewApi& aViewApi, TInt aIndex);
	static	void CntViewMethodLog(const TCntViewClass& aViewClass, const TCntViewApi& aViewApi, const TContactViewEvent& aEvent);
	static	void CntViewMethodLog(const TCntViewClass& aViewClass, const TCntViewApi& aViewApi, const RContactViewSortOrder& aSortOrder);
	static	void CntViewMethodLog(const TCntViewClass& aViewClass, const TCntViewApi& aViewApi, 
									const RContactViewSortOrder& aSortOrder, TContactViewPreferences aContactTypes);
	static	void CntViewMethodLog(const TCntViewClass& aViewClass, const TCntViewApi& aViewApi,
									const RContactViewSortOrder& aSortOrder,TContactViewPreferences aContactTypes, const TDesC8& aSortPluginName);

	static	void CntViewMethodLog(const TCntViewClass& aViewClass, const TCntViewApi& aViewApi, const TDesC& aViewName,
									const RContactViewSortOrder& aSortOrder, TContactViewPreferences aContactTypes);
	static	void CntViewMethodLog(const TCntViewClass& aViewClass, const TCntViewApi& aViewApi, const TDesC& aViewName,
									const RContactViewSortOrder& aSortOrder, TContactViewPreferences aContactTypes, const TDesC8& aSortPluginName);

private:
	friend class CContactDbApiTimer;
	// convert enumertion to ContactDatabase method name
	static	const TDesC& CntDbMethodName(const TCntDbApi& aApiId);

	// convert enumertions to Contact View class or method names
	static	const TDesC& CntViewClassName(const TCntViewClass& aClassId);
	static	const TDesC& CntViewMethodName(const TCntViewClass& aClassId, const TCntViewApi& aApiId);
	static	const TDesC& CntViewEventName(TInt& aParamFlags, const TContactViewEvent& aEvent);

	static	void CntDbMethodLog(TTime& aTime, const TCntDbApi& aApiId, TBool aProcessDetail);
	static	void CntDbMethodLog(TTime& aTime, const TCntDbApi& aApiId, const TContactItemId aContactId, TBool aProcessDetail);
	static	void CntDbMethodLog(TTime& aTime, const TCntDbApi& aApiId, const TDriveUnit aDriveUnit, TBool aProcessDetail);
	static	void CntDbMethodLog(TTime& aTime, const TCntDbApi& aApiId, const TDesC& aFileName, TBool aProcessDetail);
	static	void CntDbMethodLog(TTime& aTime, const TCntDbApi& aApiId, const TContactItemId aContactItemId, const TContactItemId aContactGroupId, TBool aProcessDetail);
	static	void CntDbMethodLog(TTime& aTime, const TCntDbApi& aApiId, const TContactDbObserverEvent &aDbEvent, TBool aProcessDetail);

	static	void CntDbMethodEndLog(const TContactsApiProfile::TCntDbApi& aApiId, TTime& aStartTime);
#if !defined(EKA2)
	static	void CntDbMethodEndLog(const TContactsApiProfile::TCntDbApi& aApiId, TTime& aStartTime, TInt64& aCpuElapsedTime);
#endif

	// debug print time, PID etc, and class :: method details
	static	void MethodCallLogHelper(TTime& aTime, const TDesC& aClassName, const TDesC& aMethodName, TBool aProcessDetail, TBool aNewLine);
	static	void MethodCallLog(TTime& aTime, const TDesC& aClassName, const TDesC& aMethodName, const TContactItemId aContactId, TBool aProcessDetail);
	static	void MethodCallLog(TTime& aTime, const TDesC& aClassName, const TDesC& aMethodName, const TContactItemId aContactItemId, const TContactItemId aContactGroupId, TBool aProcessDetail);
	static	void MethodCallLog(TTime& aTime, const TDesC& aClassName, const TDesC& aMethodName, const TDriveUnit aDriveUnit, TBool aProcessDetail);
	static	void MethodCallLog(TTime& aTime, const TDesC& aClassName, const TDesC& aMethodName, const TDesC& aFileName, TBool aProcessDetail);
	static	void MethodCallLog(TTime& aTime, const TDesC& aClassName, const TDesC& aMethodName, const TContactDbObserverEvent &aDbEvent, TBool aProcessDetail);


	static	void ViewMethodCallLog(TTime& aTime, const TDesC& aClassName, const TDesC& aMethodName, TInt aIndex, TBool aProcessDetail);
	static	void ViewMethodCallLog(TTime& aTime, const TDesC& aClassName, const TDesC& aMethodName, const TContactViewEvent& aEvent, TBool aProcessDetail);
	static	void ViewMethodCallLog(TTime& aTime, const TDesC& aClassName, const TDesC& aMethodName, const RContactViewSortOrder& aSortOrder, TBool aProcessDetail, TBool aNewLine);
	static	void ViewMethodCallLog(TTime& aTime, const TDesC& aClassName, const TDesC& aMethodName,
											const RContactViewSortOrder& aSortOrder, TContactViewPreferences aContactTypes, TBool aProcessDetail, TBool aNewLine);
	static	void ViewMethodCallLog(TTime& aTime, const TDesC& aClassName, const TDesC& aMethodName,
											const RContactViewSortOrder& aSortOrder, TContactViewPreferences aContactTypes, const TDesC8& aSortPluginName, TBool aProcessDetail);
	static	void ViewMethodCallLog(TTime& aTime, const TDesC& aClassName, const TDesC& aMethodName, const TDesC& aViewName,
											const RContactViewSortOrder& aSortOrder, TContactViewPreferences aContactTypes, TBool aProcessDetail, TBool aNewLine);
	static	void ViewMethodCallLog(TTime& aTime, const TDesC& aClassName, const TDesC& aMethodName, const TDesC& aViewName,
											const RContactViewSortOrder& aSortOrder, TContactViewPreferences aContactTypes, const TDesC8& aSortPluginName, TBool aProcessDetail);

	static	void ProcessNameLog();
	};


// Log & time API method call
NONSHARABLE_CLASS(CContactDbApiTimer) : public CBase
	{	
public:
	static	CContactDbApiTimer* CntDbMethodTimeBeginLC(const TContactsApiProfile::TCntDbApi& aApiId);
	static	CContactDbApiTimer* CntDbMethodTimeBeginLC(const TContactsApiProfile::TCntDbApi& aApiId, const TContactItemId aContactId);
	static	CContactDbApiTimer* CntDbMethodTimeBeginLC(const TContactsApiProfile::TCntDbApi& aApiId, const TDriveUnit aDriveUnit);
	static	CContactDbApiTimer* CntDbMethodTimeBeginLC(const TContactsApiProfile::TCntDbApi& aApiId, const TDesC& aFileName);
	static	CContactDbApiTimer* CntDbMethodTimeBeginLC(const TContactsApiProfile::TCntDbApi& aApiId, const TContactItemId aContactItemId, const TContactItemId aContactGroupId);

	~CContactDbApiTimer();
private:
	CContactDbApiTimer(const TContactsApiProfile::TCntDbApi& aApiId);

private:
	TContactsApiProfile::TCntDbApi	iCntDbApi;
	TTime							iStartTime;
#if !defined(EKA2)
	TTimeIntervalMicroSeconds		iCpuStartTime;
	TTimeIntervalMicroSeconds		iCpuEndTime;
	TInt64							iCpuElapsedTime;
#endif
	};


#endif // CONTACTS_API_PROFILING



// SYMBIAN_CNT_API_PROFILE
#endif
