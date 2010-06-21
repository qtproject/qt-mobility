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

#ifndef CNTVIEWMSGHANDLER_H
#define CNTVIEWMSGHANDLER_H

class CCntMsgHandler;
class CViewSubSessionBase;

/**
CCntViewMsgHandler class handles all View related operations. It uses
a message handling method to handle the incoming op code.

It delegates the incoming op code to appropriate message handling method by using
a lookup table of member function pointers created by CCntServer.

@internalComponent
*/
class CCntViewMsgHandler : public CCntMsgHandler
	{
public:
	static CCntViewMsgHandler* NewLC(CCntSession& aSession);
    virtual ~CCntViewMsgHandler();
	TInt HandleMessageL(const RMessage2& aMessage);
	
private:
	CCntViewMsgHandler(CCntSession& aSession);
	void ConstructorL();
	
	// Helper methods
	void CreateViewSubSessionL(const RMessage2& aMessage);
	void CreateNamedViewSubSessionL(const RMessage2& aMessage);
	void CloseViewSubSessionL(const RMessage2 &aMessage);
	CViewSubSessionBase& ViewFromHandleL(TUint aHandle);
	void CompleteMessage(TInt aRet, const RMessage2& aMessage);
	
	void ValidateViewContactL(const CViewContact& aViewContact, TInt aSessionId);

public:	
	void ItemAtL(const RMessage2& aMessage);
	void OpenViewSessionL(const RMessage2& aMessage);
	void CloseViewSessionL(const RMessage2& aMessage);
	void ChangeSortOrderL(const RMessage2& aMessage);
	void BeginIterateL(const RMessage2& aMessage);
	void EndIterateL(const RMessage2& aMessage);
	void NextItemL(const RMessage2& aMessage);	
	void TextFieldL(const RMessage2& aMessage);
    void ReadContactTextDefL(const RMessage2& aMessage);
	void CreateViewL(const RMessage2& aMessage);
	void CreateNamedViewL(const RMessage2& aMessage);
	void CloseViewL(const RMessage2& aMessage);
	void MatchesHintFieldL(const RMessage2& aMessage);
	void ViewSubSessionServiceL(const RMessage2& aMessage);
	
private:
	CObjectCon*  iContainer;
	CObjectIx*   iSubSessions;
	};
	
/**
Subsession container object cleanup class.
*/
class TObjectCleanup
	{
public:
	TObjectCleanup() : iObject(NULL),iIndex(NULL) {}
	static void Cleanup(TAny* aSelf) 
		{ 
		TObjectCleanup* cleanupObj = static_cast<TObjectCleanup*>(aSelf);
		cleanupObj->DoCleanup();
		}
private:
	void DoCleanup()
		{
		if (iIndex)
			{
			iIndex->Remove(iHandle);
			}
		else if (iObject)
			{
			iObject->Close();
			}
		delete this;
		}
public:
	CObject* iObject;
	CObjectIx* iIndex;
	TInt iHandle;
	};
	
#endif
