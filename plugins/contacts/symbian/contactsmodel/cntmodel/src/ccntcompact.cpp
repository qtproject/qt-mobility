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


/**
 @file
 @internalComponent
 @released
*/

#include <cntdb.h>
#include "rcntmodel.h"

// All CContactActiveBase classes are unused with SQLite.  It is the (non-exposed) base class for
// CContactActiveCompress and CContactActiveRecover, both of which have no meaning with SQLite.             

/**
For BC proofing.
*/
EXPORT_C void MContactUiActive::MContactUiActive_Reserved1() 
	{ 
	}


CContactActiveBase::~CContactActiveBase()
	{
	}


void CContactActiveBase::SetContactDatabase(CContactDatabase* /*aContactDatabase*/)
	{	
	}	

void CContactActiveBase::Cancel()
/** Deletes the ContactActiveBase object created on the heap 
 */
	{
	delete this; // C-type classes must always be created on the heap	
	}

/** @deprecated */
void CContactActiveBase::SetObserver(MContactUiCompactObserver* /*aObserver*/)
	{
	}


TInt CContactActiveBase::Error() const
/** Signals client of any error that occurred during the operation.
@return A system-wide error code.        */
	{
    return KErrNone;
	}


TInt CContactActiveBase::StepsTogo() const
/** Shows the steps to be completed.
@return The number of steps to be completed.  */
	{
	return 0;
	}


TInt CContactActiveBase::Step()
/** Gets the current compress step, recover step or any error code encountered during the operation.
@return An integer which represents the compress step, recover step or any error code encountered during the operation.
*/
	{
	return EFalse;
	}


void CContactActiveBase::SetFileManagerL(RCntModel& /*aCntSvr*/)
	{
	}
