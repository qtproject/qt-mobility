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
* CPackagerTestServer - server classes
*
*/


#ifndef __PACKAGER_TEST_SERVER_H__
#define __PACKAGER_TEST_SERVER_H__

#include <e32base.h>
#include <f32file.h>
#include "ccntpackager.h"
#include <cntfilt.h>
#include "t_packagertesthelper.h"
#include "t_serveractiveobject.h"

// TestServer Class
class CPackagerTestServer : public CServer2
	{

public:
	static CServer2* NewLC();
	~CPackagerTestServer();
	
	CPackagerCntFactory& FactoryL() const;
	CPackagerCntComparator& ComparatorL()const;
	CCntPackager& PackagerL(); 
	
private:
	CPackagerTestServer();
	void ConstructL ();
    CSession2* NewSessionL (const TVersion& aVersion, const RMessage2& aMessage) const;

private:
	TInt iSessionCount;
	
	// Server keeps only one instance of these objects, shared between sessions.
	CCntPackager* iPackager;
	mutable CPackagerCntFactory* iFactory;
	mutable CPackagerCntComparator* iComparator;
	};

class CServerActiveObject;
// TestSession Class
class CPackagerTestSession : public CSession2
	{

public:
	CPackagerTestSession(); // Move to private
	void CreateL();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2 &aMessage, TInt aError);	
	
	void NextStepL() const;

private:
	~CPackagerTestSession();
	inline CPackagerTestServer& Server() const;

	void UnPackL(const RMessage2& aMessage);
	void SendBackToClientL(const RMessage2& aMessage);
	TInt DoCompareL() const;
	void CompleteWriteBackL(const RMessage2 &aMessage, const TPtr8& aPtr);
	CContactItem& CntItemL();
	CContentType& CntContentTypeL();
	CContactTextDef& CntTextDefL();
	CContactItemViewDef& CntItemViewDefL();
	CContactViewDef& CntViewDefL();
	CContactIdArray& CntIdArrayL();
	CCntFilter& CntFilterL();
	CViewContact& ViewCntL();
	CDesCArray& DesCArrayL();	
	
private:
	mutable CContactItem* iContactItem;
	mutable CContentType* iContentType;
	mutable CContactTextDef* iContactTextDef;
	mutable CContactItemViewDef* iContactItemViewDef;
	mutable CContactViewDef* iContactViewDef;
	mutable CContactIdArray* iContactIdArray;
	mutable CCntFilter* iContactFilter;
	mutable CViewContact* iViewContact;
	mutable CDesCArray* iDesCArray;
	
	TInt iCurrentOpCode;
	CServerActiveObject* iSessionAO;
	const RMessage2* iMessage;
	};


// CPackagerTestServer inline methods	
inline CPackagerTestServer::CPackagerTestServer():CServer2(0, ESharableSessions)
	{
	}


// CPackagerTestSession inline methods		
inline CPackagerTestSession::CPackagerTestSession()
	{}
	
inline CPackagerTestServer& CPackagerTestSession::Server() const
	{
	return *static_cast<CPackagerTestServer*>(const_cast<CServer2*>(CSession2::Server()));
	}
	
#endif // __PACKAGER_TEST_SERVER_H__
