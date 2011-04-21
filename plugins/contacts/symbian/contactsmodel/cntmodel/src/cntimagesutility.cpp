/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cntimagerescaler.h"
#include <cntimagesutility.h>

EXPORT_C CContactImagesUtility* CContactImagesUtility::NewL()
{
    CContactImagesUtility* self = new (ELeave) CContactImagesUtility();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

EXPORT_C CContactImagesUtility::~CContactImagesUtility()
{
    Cancel();  
    delete iRescaler;
}

EXPORT_C void CContactImagesUtility::ScaleAndStoreImage(MContactImageUtilityObserver* aObserver, const TDesC& aSrcPath)
{
    ASSERT(aObserver);
    iObserver = aObserver;
    iRescaler->ScaleAndStoreImage( &iStatus, aSrcPath );
    SetActive();
}

EXPORT_C void CContactImagesUtility::EnsureImageRemoved(const TDesC& aImagePath)
    {
    iRescaler->EnsureImageRemoved(aImagePath);
    }

void CContactImagesUtility::DoCancel()
{
    iRescaler->Cancel();
}

void CContactImagesUtility::RunL()
{
    TPath destPath = iRescaler->DestinationPath();
    iObserver->UtilityOperationComplete(destPath, iStatus.Int());
}

TInt CContactImagesUtility::RunError(TInt /*aError*/)
{
    // RunL does not have any leaving function
    return KErrNone;
}

CContactImagesUtility::CContactImagesUtility() : CActive(EPriorityStandard)
{
}

void CContactImagesUtility::ConstructL()
{
    iRescaler = CImageRescaler::NewL();
    
    CActiveScheduler::Add( this );
}

// End of File
