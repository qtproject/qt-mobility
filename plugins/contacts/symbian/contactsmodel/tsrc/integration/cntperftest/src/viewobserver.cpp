/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/
 
#include "viewobserver.h"

CViewObserver* CViewObserver::NewLC(const CContactViewTest &aTest, 
									void (CContactViewTest::*aFunc)())
	{
	CViewObserver *self = new(ELeave) CViewObserver(aTest, aFunc);
	CleanupStack::PushL(self);
	return self;
	}
	
CViewObserver::~CViewObserver()
	{
	CleanView();
	}

CViewObserver::CViewObserver( 	const CContactViewTest &aTest,
								void (CContactViewTest::*aFunc)()) : 
														CActive(EPriorityStandard),
														iMainFunc( aFunc ), 
														iTest( const_cast<CContactViewTest &>(aTest) )
	{
	CActiveScheduler::Add(this);
	}
	
	
void CViewObserver::RunL()
	{
	
	if(iMainFunc)
		{
		(iTest.*iMainFunc)();
		}
	else
		{
		(iTest.*iDoFunc)(*iLocalView);
		}
		
	if( !iDoFunc && !iMainFunc )
		{
		CActiveScheduler::Stop();
		}
	}
	
void CViewObserver::DoCancel()
	{
	}

TInt CViewObserver::RunError(TInt aError)
	{
	_LIT(KViewError,"ViewObserver:: Error in doTest runL: %d");
	iTest.ERR_PRINTF2(KViewError , aError );
	return aError;
	}

void CViewObserver::Activate()
	{
    if (!IsActive())
        {
    	TRequestStatus *pS=&iStatus;
    	User::RequestComplete(pS,KErrNone);
        SetActive();
        }
	}
	
	
void CViewObserver::HandleContactViewEvent(	const CContactViewBase& aView, 
											const TContactViewEvent& aEvent)
	{
	if (&aView==iLocalView)
		{
		switch(aEvent.iEventType)
			{
			case TContactViewEvent::EReady:
				{				
				Activate();
				break;
				}
			case TContactViewEvent::EItemAdded:
			case TContactViewEvent::EItemRemoved:
			case TContactViewEvent::EGroupChanged:
			case TContactViewEvent::ESortOrderChanged:
			case TContactViewEvent::EUnavailable:
				{
				break;
				}
			default:
				{
				_LIT(KViewError,"CViewObserver::HandleContactViewEvent, non ready event: %d and error: %d");
				iTest.ERR_PRINTF3(KViewError , aEvent.iEventType, aEvent.iInt );
				RDebug::Print(KViewError , aEvent.iEventType, aEvent.iInt  );
				iEvent = TContactViewEvent(aEvent.iEventType, aEvent.iInt, aEvent.iContactId);
				Activate();
				break;
				}
			}
		}
	}
	
void CViewObserver::CleanView()
	{
	if(iLocalView)
		{
		iLocalView->Close(*this);
		iLocalView = NULL;
		}
	}
	
void CViewObserver::SetView(const CContactViewBase *aLocalView)
	{
	CleanView();
	iLocalView = const_cast<CContactViewBase *>(aLocalView);
	}

