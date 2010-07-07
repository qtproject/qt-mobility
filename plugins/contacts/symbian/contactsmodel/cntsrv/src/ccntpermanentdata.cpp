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


#include <e32base.h>

#include "ccntrequest.h"
#include "ccntdbmanager.h"
#include "ccntdbmanagercontroller.h"
#include "ccntstatemachine.h"
#include "cntviewprivate.h"
#include "cviewsubsessions.h"
#include <cntviewstore.h>
#include "ccntpermanentdata.h"
#include "ccntlogger.h"

/**
Object factory method.
*/
CCntPermanentData* CCntPermanentData::NewL(CCntDbManagerController& aController)
	{
	CCntPermanentData* self = new (ELeave) CCntPermanentData(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/**
First phase constructor.
*/
CCntPermanentData::CCntPermanentData(CCntDbManagerController& aController) 
: CActive(CActive::EPriorityLow), iState(EStateInitial), iController(aController)
	{
	CActiveScheduler::Add(this);
	}


/**
Second phase constructor.
*/
void CCntPermanentData::ConstructL()
	{
	}

	
/**
Object destructor.
*/	
CCntPermanentData::~CCntPermanentData()
	{
	Cancel();
	Close();
	}

	
/*
*/
void CCntPermanentData::Close()
	{
	iViewDefs.ResetAndDestroy();
	for (TInt i = 0; i < iViews.Count(); i++)
		{
		iViews[i]->Close(*this);
		}
	iViews.Close();
	if (iManager)
		{
		iController.CloseDatabase(*iManager);
		iManager = NULL;
		}	
	}


/**
The permanent data is created in a number of asynchronous steps. The AO uses
a simple state machine to control the steps:

1. Open the default database - EStateOpeningDb.
2. Open each default view one at a time - EStateOpeningView.
   (Views are created one at a time to avoid thrashing the dbms/file server
   with multiple requests for data that is not contiguous.)
3. Once all views are opened, or if there is an error, or the AO is cancelled -
   EStateFinished.
*/	
void CCntPermanentData::RunL()
	{
	if (iStatus.Int() == KErrNone)
		{
		switch (iState)
			{
			case EStateOpeningDb:
				{
				if (OpenNextViewL())
					{
					iState = EStateOpeningView;
					iStatus = KRequestPending;
					SetActive();
					}
				else
					{
					iState = EStateFinished;
					}
				break;
				}
			case EStateOpeningView:
				{
				if (OpenNextViewL())
					{
					iState = EStateOpeningView;
					iStatus = KRequestPending;
					SetActive();
					}
				else
					{
					iState = EStateFinished;
					}		
				break;
				}
			case EStateFinished:
				{
				// Nothing left to do
				break;
				}
			default:
				{
				__ASSERT_DEBUG(0, User::Invariant());
				break;
				}
			}
		}
	else if (iStatus.Int() == KErrNotFound)
		{
		// File could not be found to open.
		iState = EStateFinished;
		}		
	}


/**
Only need to cancel requests in certain states, otherwise there is nothing to
do.
*/	
void CCntPermanentData::DoCancel()
	{
	if (IsActive())
		{
		switch (iState)
			{
			case EStateOpeningDb:
				{
				// Cancel the asynchronous open to stop this from recieving any notifications.
				CReqCancelInternalAsyncOpen* req = NULL;

				TRAP_IGNORE(
				    {
    				// req is Pop'd from the cleanup stack to keep cleanup stack balanced
    				// before and after the TRAP.
				    req = CReqCancelInternalAsyncOpen::NewLC(0);
				    CleanupStack::Pop(req);
                    iManager->StateMachineL().ProcessRequestL(req);  // ownership transferred

	                // ProcessRequestL received ownership of the request, req can be set to 
	                // NULL
                    req = NULL;
					});

                // if request is not being cleaned up within TRAP, clean it up now!
                if (req)
                    {
                    req->Complete();
                    delete req;
                    }

				break;
				}
			case EStateOpeningView: // intentional fall-through.
			default:
				{
				// Self-complete the currently active request.
				CompleteSelf(KErrCancel);
				break;
				}
			}
		iState = EStateFinished;
		}
	}


/**
Any errors will have come from OpenNextViewL() leaving.  This is not a fatal
error for this class so do not pass it on to the scheduler.
*/
TInt CCntPermanentData::RunError(TInt /*aError*/)
	{
	iState = EStateFinished;
	return KErrNone;
	}

	
void CCntPermanentData::CompleteSelf(TInt aReason)
	{
	TRequestStatus* pStat = &iStatus;
  	User::RequestComplete(pStat, aReason);
	}


/**
Kicks off the creation of the default data. This means:

1. Opening the default database.
2. Opening any views that are specified in the default view 'store' (a CenRep
   repository).
*/	
void CCntPermanentData::StartDataCreationL()
	{
	__ASSERT_ALWAYS(!IsActive(), User::Leave(KErrInUse));
	
	TContactViewStore::GetDefaultViewsL(iViewDefs);
	
	if (iViewDefs.Count() == 0)
		{
		// No views; no need to open the default database.
		iState = EStateFinished;
		}
	else
		{
		// Only attempt to open the default file if it exists.
		if (iController.DatabaseExistsL(KNullDesC))
			{
			// Open the default database (this is asynchronous).
			CReqInternalAsyncOpen* openReq = CReqInternalAsyncOpen::NewLC(0, KNullDesC, iStatus);
			// Get an instance of the database manager for this file. One db manager is 
			// created per file and shared amoung sessions working on that file.
			iManager = iController.GetDbManagerL(openReq->FileName(), ECntFileOpen);
			// Process the open request via the state machine
			// The state machine ensures the file - owned by the manager - is in a valid
			// state to process the request.
			iManager->StateMachineL().ProcessRequestL(openReq);  // ownership transferred

	        // ProcessRequestL received ownership of the request, the request only need
	        // to be popped from CleanupStack.
			CleanupStack::Pop(openReq);

			iState = EStateOpeningDb;
			SetActive();
			}
		else
			{
			iState = EStateFinished;
			}
		}
	}


/**
Checks to see if the given CCntDbManager is being held open by this object AND
that it has a reference count of 1, i.e. this is the only object holding a
reference to it.
  
@return ETrue if this is the only object with a reference to the CCntDbManager,
EFalse otherwise.
*/
TBool CCntPermanentData::IsOnlyClientOfDbManager(CCntDbManager* manager) const
	{
	if (iManager == manager && manager->SessionCount() == 1)
		{
		return ETrue;
		}
	return EFalse;
	}


/**
Destroy any views and the manager being held by this object
*/	
void CCntPermanentData::ReleaseDbManagerL(CCntDbManager* manager)
	{
	__ASSERT_ALWAYS(manager == iManager, User::Leave(KErrArgument));
	Close();
	}


/**
Creates the next view from the list of view definitions.  The actual
construction of the view is done asynchronously.  When it has finished this
object is notified by a call to HandleContactViewEvent().
  
@return EFalse if no view was created, ETrue otherwise.
*/
TBool CCntPermanentData::OpenNextViewL()
	{
	if (iViewDefs.Count() == 0)
		{
		return EFalse;
		}

	TBool viewCreated(EFalse);
	do 
		{		
		CContactDefaultViewDefinition* viewDef = iViewDefs[0];
		CleanupStack::PushL(viewDef);
		iViewDefs.Remove(0);
		
		// Attempt to create a view from the definition.
		// If this leaves, the do..while ensures that
		// an attempt to create the next view will happen.
		CContactViewBase* view = NULL;
		TRAPD(err, view = OpenViewFromDefinitionL(*viewDef));
		if (err == KErrNone)
			{
			iViews.AppendL(view);
			viewCreated = ETrue;
			}
		else
			{
  

//		 Log the details of the view that failed to be created.
            DEBUG_PRINTF1(__VERBOSE_DEBUG__,"[CNTMODEL] CCntPermanentData::OpenNextViewL(): Failed to create a view:\n");
            DEBUG_PRINTF2(__VERBOSE_DEBUG__,"[CNTMODEL] View type: %d\n", viewDef->ViewType());
            DEBUG_PRINTF2(__VERBOSE_DEBUG__,"[CNTMODEL] View preferences: %d\n", viewDef->ViewPreferences());
			DEBUG_PRINT2(__VERBOSE_DEBUG__,_L("[CNTMODEL] View name: %S\n"), &viewDef->ViewNameL());
			DEBUG_PRINT2(__VERBOSE_DEBUG__,_L("[CNTMODEL] Sort plugin name: %S\n"), &viewDef->SortPluginNameL());
			DEBUG_PRINTF1(__VERBOSE_DEBUG__,"[CNTMODEL] View field sort order: ");
			RContactViewSortOrder sortOrder = viewDef->SortOrder();
			for (TInt i = 0; i < sortOrder.Count(); i++)
				{
				DEBUG_PRINTF2(__VERBOSE_DEBUG__,"[CNTMODEL] 0x%x, ", sortOrder[i].iUid);
				}
			RDebug::Printf("\n");

			}
		
		CleanupStack::PopAndDestroy(viewDef);
		}
	while (!viewCreated && (iViewDefs.Count() > 0));
	return viewCreated;
	}

	
CContactViewBase* CCntPermanentData::OpenViewFromDefinitionL(const CContactDefaultViewDefinition& aViewDef)
	{
	CContactViewBase* view = NULL;
	switch (aViewDef.ViewType())
		{
		case CContactDefaultViewDefinition::ERemoteView:
			{
			view = &iManager->ViewManagerL().OpenViewL(aViewDef.SortOrder(), *this, 
														aViewDef.ViewPreferences(), 
														KNullUid, aViewDef.SortPluginNameL());
			break;
			}
		case CContactDefaultViewDefinition::ENamedRemoteView:
			{
			view = &iManager->ViewManagerL().OpenNamedViewL(aViewDef.ViewNameL(), 
															aViewDef.SortOrder(), *this, 
															aViewDef.ViewPreferences(), 
															KNullUid, aViewDef.SortPluginNameL());
			break;
			}
		default:
			{
			User::Leave(KErrArgument);
			}
		}
	return view;	
	}


void CCntPermanentData::HandleContactViewEvent(const CContactViewBase& /*aView*/, const TContactViewEvent& /*aEvent*/)
	{
	// Complete the active object regardless of whether the view was 
	// created successfully or not. This class is only trying to pre-create 
	// the view(s) for the main Contacts application. If there are errors 
	// creating the view then the application should deal with them 
	// (as it would if this class didn't exist).
	if (iState == EStateOpeningView)
		{
		CompleteSelf(KErrNone);
		}
	}
