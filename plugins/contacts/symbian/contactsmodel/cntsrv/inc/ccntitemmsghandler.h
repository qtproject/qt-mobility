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

#ifndef CNTITEMMSGHANDLER_H
#define CNTITEMMSGHANDLER_H

class CCntMsgHandler;

/**
CCntItemMsgHandler class handles Contact Item database CRUD operations. It uses
a message handling method to handle the incoming op code.

It delegates the incoming op code to appropriate message handling method by using
a lookup table of member function pointers created by CCntServer.

@internalComponent
*/
class CCntItemMsgHandler : public CCntMsgHandler
	{
public:
	static CCntItemMsgHandler* NewLC(CCntSession& aSession);
    virtual ~CCntItemMsgHandler();
	TInt HandleMessageL(const RMessage2& aMessage);
	
private:
	CCntItemMsgHandler(CCntSession& aSession);
	
public:
	void ItemCreateL(const RMessage2& aMessage);
	void ItemUpdateL(const RMessage2& aMessage);
	void ItemDeleteL(const RMessage2& aMessage);
	void ItemReadL(const RMessage2& aMessage);
	void ItemCommitL(const RMessage2& aMessage);
	void ItemOpenL(const RMessage2& aMessage);
	void ItemCloseL(const RMessage2& aMessage);
	
	void ConnectionId(const RMessage2& aMessage);
	void GetCurrentItemL(const RMessage2& aMessage);
	void RemoveCurrentItemL(const RMessage2& aMessage);
	void SetCurrentItemL(const RMessage2& aMessage);

	void GetCurrentDb(const RMessage2& aMessage);
	void SetCurrentDbL(const RMessage2& aMessage);
	void GetSpeedDialContactIdAndPhoneNumberL(const RMessage2& aMessage);
	void SetSpeedDialIdForPositionL(const RMessage2& aMessage);
	void SetOwnCardL(const RMessage2& aMessage);
	void GetOwnCardL(const RMessage2& aMessage);
	void GetCollectionL(const RMessage2& aMessage);
	
	void SetSortPrefsL(const RMessage2& aMessage);
	void GetSortPrefsL(const RMessage2& aMessage);
	void SetDbViewContactTypeL(const RMessage2& aMessage);
	void GetDbViewContactTypeL(const RMessage2& aMessage);
	void DbContactCountL(const RMessage2& aMessage);
	void FindAsyncL(const RMessage2& aMessage);
	
	void FindAsyncInitL(const RMessage2& aMessage);
	void FindAsyncTextDefInitL(const RMessage2& aMessage);
	void FindL(const RMessage2& aMessage);
	void FilterDatabaseL(const RMessage2& aMessage);
	void SetAsyncActivityL(const RMessage2& aMessage);
	void ResourceCount(const RMessage2& aMessage);
	void SetHeapFailure(const RMessage2& aMessage);
	
	void SeekContactL(const RMessage2& aMessage);
	};
	
#endif
