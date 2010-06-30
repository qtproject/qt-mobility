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


#include <f32file.h>
#include <e32std.h>
#include "t_packagertestserver.h"
#include "t_packagertestdef.h"

void CPackagerTestSession::CreateL()
/** 2nd phase construct for sessions -called by the CServer framework. */
	{
	}


CPackagerTestSession::~CPackagerTestSession()
/** The destructor frees all resources owned by the session, 
prior to its destruction. */
	{
	delete iContactItem;
	delete iContentType;
	delete iContactTextDef;
	delete iContactItemViewDef;
	delete iContactViewDef;
	delete iContactIdArray;
	delete iContactFilter;
	delete iViewContact;
	delete iDesCArray;	
	
	delete iSessionAO;
	}
	
void CPackagerTestSession::ServiceL(const RMessage2& aMessage)
/** Server session ServiceL implementation. 

@param aMessage The request data sent by the client. */
	{
	iCurrentOpCode = aMessage.Function();
	iMessage = &aMessage;
	if(!iSessionAO)
		{
		iSessionAO = CServerActiveObject::NewL(*this);	
		}
	
	UnPackL(aMessage);
	}

void CPackagerTestSession::ServiceError(const RMessage2 &aMessage, TInt aError)
/** Server session ServiceError implementation, called if ServiceL 
leaves. 

@param aMessage The request data sent by the client. 
@param aError The error code which the message will be completed with. */
	{
	CSession2::ServiceError(aMessage, aError); 
	}
	
void CPackagerTestSession::NextStepL() const
/** Called by active object to do second stage of client request,
i.e. compare items and complete the result back to the client */
	{
	iMessage->Complete(DoCompareL());
	}

void CPackagerTestSession::UnPackL(const RMessage2& aMessage)
/** Unpacks serialized data item sent by client and starts active object to
run any required further methods. 

@param aMessage The request data sent by the client. */
	{	
	switch(iCurrentOpCode)
		{
		case EProcessCContactItem:
			Server().PackagerL().SetBufferFromMessageL(aMessage);
			iContactItem = Server().PackagerL().UnpackCntItemLC();
			CleanupStack::Pop(iContactItem);
			iSessionAO->StartActiveObject();
			break;
			
		case EProcessCContentType:
			Server().PackagerL().SetBufferFromMessageL(aMessage);
			iContentType = Server().PackagerL().UnpackCntContentTypeLC();
			CleanupStack::Pop(iContentType);
			iSessionAO->StartActiveObject();
			break;
			
		case EProcessCContactTextDef:
			Server().PackagerL().SetBufferFromMessageL(aMessage);
			iContactTextDef = Server().PackagerL().UnpackCntTextDefLC();
			CleanupStack::Pop(iContactTextDef);
			iSessionAO->StartActiveObject();
			break;
			
		case EProcessCContactItemViewDef:
			Server().PackagerL().SetBufferFromMessageL(aMessage);
			iContactItemViewDef = Server().PackagerL().UnpackCntItemViewDefLC();
			CleanupStack::Pop(iContactItemViewDef);
			iSessionAO->StartActiveObject();
			break;
			
		case EProcessCContactViewDef:
			Server().PackagerL().SetBufferFromMessageL(aMessage);		
		    iContactViewDef = Server().PackagerL().UnpackCntViewDefLC();
		    CleanupStack::Pop(iContactViewDef);
			iSessionAO->StartActiveObject();
		    break;
		    
		case EProcessCContactIdArray:
			Server().PackagerL().SetBufferFromMessageL(aMessage);
		    iContactIdArray = Server().PackagerL().UnpackCntIdArrayLC();
		    CleanupStack::Pop(iContactIdArray);
			iSessionAO->StartActiveObject();
		    break; 
		    
		case EProcessCCntFilter:
			Server().PackagerL().SetBufferFromMessageL(aMessage);
			iContactFilter = Server().PackagerL().UnpackCntFilterLC();
			CleanupStack::Pop(iContactFilter);
			iSessionAO->StartActiveObject();
			break;
			
		case EProcessCViewContact:
			Server().PackagerL().SetBufferFromMessageL(aMessage);
			iViewContact = Server().PackagerL().UnpackViewContactLC();
			CleanupStack::Pop(iViewContact);
			iSessionAO->StartActiveObject();
			break;
			
		case EProcessCDesCArray:
			Server().PackagerL().SetBufferFromMessageL(aMessage);
			iDesCArray = Server().PackagerL().UnpackDesCArrayLC();
			CleanupStack::Pop(iDesCArray);
			iSessionAO->StartActiveObject();
			break;

		default:
			// Request for items from client.
			SendBackToClientL(aMessage);
			
			break;			
		}
	}
	
void CPackagerTestSession::SendBackToClientL(const RMessage2& aMessage)	
/** Packs and sends date back to the client. 

@param aMessage The data to be sent back to client. */
	{
	Server().PackagerL().SetBufferFromMessageL(aMessage);
	switch(iCurrentOpCode)
		{
		case EReceiveCContactItem:
			{
			TInt size(0);
			TPtr8 cntItemBuff = Server().PackagerL().PackL(CntItemL(), size); 
			// Write data if client buffer is large enough
			// otherwise return the required buffer size.
			if(aMessage.GetDesLength(1) >= size)
				{ 
				aMessage.WriteL(1, cntItemBuff); 
				aMessage.Complete(KErrNone);
				}
			else
				{ 
				aMessage.Complete(size);	
				}
			}
			break;
			
		case EReceiveCContentType:
			CompleteWriteBackL(aMessage, Server().PackagerL().PackL(CntContentTypeL())); 
			break;
			
		case EReceiveCContactTextDef:
			CompleteWriteBackL(aMessage, Server().PackagerL().PackL(CntTextDefL())); 
			break;
			
		case EReceiveCContactItemViewDef:
			CompleteWriteBackL(aMessage, Server().PackagerL().PackL(CntItemViewDefL())); 
			break;
			
		case EReceiveCContactViewDef:
			CompleteWriteBackL(aMessage, Server().PackagerL().PackL(CntViewDefL())); 
		    break;
		    
		case EReceiveCContactIdArray:
			CompleteWriteBackL(aMessage, Server().PackagerL().PackL(CntIdArrayL())); 
		    break; 
		    
		case EReceiveCCntFilter:
			CompleteWriteBackL(aMessage, Server().PackagerL().PackL(CntFilterL())); 
			break;
			
		case EReceiveCViewContact:
			CompleteWriteBackL(aMessage, Server().PackagerL().PackL(ViewCntL()));
			break;	
					
		case EReceiveCDesCArray:
			CompleteWriteBackL(aMessage, Server().PackagerL().PackL(DesCArrayL()));
			break;

		default:
			break;			
		}
	
	}
	
void CPackagerTestSession::CompleteWriteBackL(const RMessage2 &aMessage, const TPtr8& aPtr)
/** Checks if clients packager buffer is large enought to hold inoming data, if it is then
the data is sent, otherwise the size of the required data is sent. 

@param aMessage The request data sent by the client
@param aPtr The ptr to the packager buffer data. */
	{
	TInt size = Server().PackagerL().GetTransmittingBuffer().Size();
	// Write data if client buffer is large enough
	// otherwise return the required buffer size.
	if(aMessage.GetDesLength(1) >= size)
		{ 
		aMessage.WriteL(1, aPtr); 
		aMessage.Complete(KErrNone);
		}
	else
		{ 
		aMessage.Complete(size);	
		}
	}
	
TInt CPackagerTestSession::DoCompareL()	const
/** Compare test methods. Compares two objects instantiated from the same class. */
	{
	TInt result = ETrue;
	
	switch(iCurrentOpCode)
		{		
		case EProcessCContactItem:
			result = Server().ComparatorL().Compare(*iContactItem, 
						Server().FactoryL().CreateDefaultCContactItemL());
			break;
			
		case EProcessCContentType:
			result = Server().ComparatorL().Compare(*iContentType, 
						Server().FactoryL().CreateDefaultCContentTypeL());
			break;
			
		case EProcessCContactTextDef:
			result = Server().ComparatorL().Compare(*iContactTextDef, 
						Server().FactoryL().CreateDefaultCContactTextDefL());
			break;
			
		case EProcessCContactItemViewDef:
			result = Server().ComparatorL().Compare(*iContactItemViewDef, 
						Server().FactoryL().CreateDefaultCContactItemViewDefL());
			break;
			
		case EProcessCContactViewDef:
		    result = Server().ComparatorL().Compare(*iContactViewDef, 
		    			Server().FactoryL().CreateDefaultCContactViewDefL());
		    break;
		    
		case EProcessCContactIdArray:
		    result = Server().ComparatorL().Compare(*iContactIdArray, 
		    			Server().FactoryL().CreateDefaultCContactIdArrayL());
		    break; 
		    
		case EProcessCCntFilter:
			result = Server().ComparatorL().Compare(*iContactFilter, 
						Server().FactoryL().CreateDefaultCCntFilterL());
			break;
			
		case EProcessCViewContact:
			result = Server().ComparatorL().Compare(*iViewContact, 
						Server().FactoryL().CreateDefaultCViewContactL());
			break;
			
		case EProcessCDesCArray:
			result = Server().ComparatorL().Compare(*iDesCArray, 
						Server().FactoryL().CreateDefaultCDesCArrayL());
			break;
			
		default:
			break;		
		}
		
	return result;
	}
	
CContactItem& CPackagerTestSession::CntItemL()
/** Returns the CContactItem class member if it has been constructed (this only happens if
an item has been received from the client side), otherwise it returns a default 
CContactItem object. */
	{
	return (iContactItem ? *iContactItem : Server().FactoryL().CreateDefaultCContactItemL());
	}

CContentType& CPackagerTestSession::CntContentTypeL()
/** Returns the CContentType class member if it has been constructed (this only happens if
an item has been received from the client side), otherwise it returns a default 
CContentType object. */
	{
	return (iContentType ? *iContentType : Server().FactoryL().CreateDefaultCContentTypeL());
	}

CContactTextDef& CPackagerTestSession::CntTextDefL()
/** Returns the CContactTextDef class member if it has been constructed (this only happens if
an item has been received from the client side), otherwise it returns a default 
CContactTextDef object. */
	{
	return (iContactTextDef ? *iContactTextDef : Server().FactoryL().CreateDefaultCContactTextDefL());
	}

CContactItemViewDef& CPackagerTestSession::CntItemViewDefL()
/** Returns the CContactItemViewDef class member if it has been constructed (this only happens if
an item has been received from the client side), otherwise it returns a default 
CContactItemViewDef object. */
	{
	return (iContactItemViewDef ? *iContactItemViewDef : Server().FactoryL().CreateDefaultCContactItemViewDefL());
	}

CContactViewDef& CPackagerTestSession::CntViewDefL()
/** Returns the CContactViewDef class member if it has been constructed (this only happens if
an item has been received from the client side), otherwise it returns a default 
CContactViewDef object. */
	{
	return (iContactViewDef ? *iContactViewDef : Server().FactoryL().CreateDefaultCContactViewDefL());
	}

CContactIdArray& CPackagerTestSession::CntIdArrayL()
/** Returns the CContactIdArray class member if it has been constructed (this only happens if
an item has been received from the client side), otherwise it returns a default 
CContactIdArray object. */
	{
	return (iContactIdArray ? *iContactIdArray : Server().FactoryL().CreateDefaultCContactIdArrayL());
	}

CCntFilter& CPackagerTestSession::CntFilterL()
/** Returns the CCntFilter class member if it has been constructed (this only happens if
an item has been received from the client side), otherwise it returns a default 
CCntFilter object. */
	{
	return (iContactFilter ? *iContactFilter : Server().FactoryL().CreateDefaultCCntFilterL());
	}

CViewContact& CPackagerTestSession::ViewCntL()
/** Returns the CViewContact class member if it has been constructed (this only happens if
an item has been received from the client side), otherwise it returns a default 
CViewContact object. */
	{
	return (iViewContact ? *iViewContact : Server().FactoryL().CreateDefaultCViewContactL());
	}

CDesCArray& CPackagerTestSession::DesCArrayL()
/** Returns the CDesCArray class member if it has been constructed (this only happens if
an item has been received from the client side), otherwise it returns a default 
CDesCArray object. */
	{
	return (iDesCArray ? *iDesCArray : Server().FactoryL().CreateDefaultCDesCArrayL());
	}
		
// Standard Constructors are declared inline.

CServer2* CPackagerTestServer::NewLC()
/** Allocates and constructs a new default creator. 

@return Pointer to the new CPackagerTestServer object. */
	{
	CPackagerTestServer* self = new(ELeave) CPackagerTestServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CPackagerTestServer::ConstructL()
/** Second-phase construction.
ensure server object is running. */
	{
	StartL(KPackagerServerTestName);
	}

CPackagerTestServer::~CPackagerTestServer()
/** The destructor frees all resources owned by ~CPackagerTestServer, 
prior to its destruction. */
	{
	delete iPackager;
	delete iFactory;
	delete iComparator;
	}

CSession2* CPackagerTestServer::NewSessionL(const TVersion&,const RMessage2&) const
/** Create a new client session.

@return Pointer to the new CSession2 object. */
	{
	return new (ELeave) CPackagerTestSession();
	}
	
CPackagerCntFactory& CPackagerTestServer::FactoryL() const
/** Retrieves the CPackagerCntFactory class member, constructing a new object 
if it has not been previously allocated. */
	{
	if(!iFactory)
		{
		iFactory = CPackagerCntFactory::NewL();	
		}
	return *iFactory;
	}
	
CPackagerCntComparator& CPackagerTestServer::ComparatorL() const
/** Retrieves the CPackagerCntComparator class member, constructing a new object 
if it has not been previously allocated. */
	{
	if(!iComparator)
		{
		iComparator = CPackagerCntComparator::NewL();
		}
	return *iComparator;
	}
	
CCntPackager& CPackagerTestServer::PackagerL()
/** Retrieves the CCntPackager class member, constructing a new object 
if it has not been previously allocated. */
	{
	if(!iPackager)
		{
		iPackager = CCntPackager::NewL();
		}
	return *iPackager;
	}	

/**
Perform all server initialisation, in particular creation of the
scheduler and server and then run the scheduler
*/

static void RunServerL()
/* Starts server process */
	{
	
	// naming the server thread after the server helps to debug panics
  	// User::LeaveIfError(RThread::RenameMe(KTestServerName));
	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	CPackagerTestServer::NewLC(); 
	
	RProcess::Rendezvous(KErrNone);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2); 	// Cleanup the server and scheduler

	}

TInt E32Main()
/** Server process entry-point */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		TRAP(r,RunServerL());
		delete cleanup;
		}

	__UHEAP_MARKEND;
	return r;
	}
