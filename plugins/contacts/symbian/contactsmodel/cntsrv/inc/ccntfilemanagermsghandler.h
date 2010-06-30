/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef CNTFILEMANAGERMSGHANDLER_H
#define CNTFILEMANAGERMSGHANDLER_H

class CCntMsgHandler;

/**
CCntFileManagerMsgHandler class handles all file related operations. It uses
a message handling method to handle the incoming op code.

It delegates the incoming op code to appropriate message handling method by using
a lookup table of member function pointers created by CCntServer.

@internalComponent
*/
class CCntFileManagerMsgHandler : public CCntMsgHandler
	{
public:
	static CCntFileManagerMsgHandler* NewLC(CCntSession& aSession);
    virtual ~CCntFileManagerMsgHandler();
	TInt HandleMessageL(const RMessage2& aMessage);
	
private:
	CCntFileManagerMsgHandler(CCntSession& aSession);
	void ConstructL();
	
public:
	// Delegates to the appropriate CCntDbManagerController method.
	void OpenDataBaseL(const RMessage2& aMessage);	
	void CreateDatabaseL(const RMessage2& aMessage);	
	void ReplaceDatabaseL(const RMessage2& aMessage);	
	void CancelAsyncOpenDatabaseL(const RMessage2& aMessage);	
	void CloseDataBaseL(const RMessage2& aMessage);
	void CloseDbTablesL(const RMessage2& aMessage);
	void ReOpenDbTablesL(const RMessage2& aMessage);
	void DeleteDatabaseL(const RMessage2& aMessage);
	void GetDefaultDatabaseNameL(const RMessage2& aMessage);
	void DatabaseDrive(const RMessage2& aMessage);
	void SetDatabaseDriveL(const RMessage2& aMessage);
	void DatabaseExistsL(const RMessage2& aMessage);
	void ListDatabasesL(const RMessage2& aMessage);
	void GetDatabaseReadyL(const RMessage2& aMessage);
	void FetchTemplateIdsL(const RMessage2& aMessage);
	void FetchGroupIdListsL(const RMessage2& aMessage);
	void FetchSearchResultsL(const RMessage2& aMessage); 
	void FetchPredictiveSearchResultsL(const RMessage2& aMessage);
	
	// Non-CRUD file operations
	void RecoverDbL(const RMessage2& aMessage);
	void RecoverDbStepL(const RMessage2& aMessage);
	void CancelRecoverStepL(const RMessage2& aMessage);
	void CompressL(const RMessage2& aMessage);
	void CompressStepL(const RMessage2& aMessage);
	void CancelCompressStepL(const RMessage2& aMessage);
	void CompressRequiredL(const RMessage2& aMessage);
	void IsDamagedL(const RMessage2& aMessage);
	void DamageDbL(const RMessage2& aMessage);
	void FilesSizeL(const RMessage2& aMessage);
	void WastedSpaceInBytesL(const RMessage2& aMessage);
	
	// Test-only function.
	void GetDefinitionsForExistingViewL(const RMessage2& aMessage);
	void DefinitionsOfExistingViewsL(const RMessage2& aMessage);

private:
	void FileOpenCreateReplaceL(const RMessage2& aMessage, TCntFileMode aMode);
	
	};
	
#endif
