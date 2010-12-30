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

#include "t_upscalerhelper.h"


CTestImageCreator* CTestImageCreator::NewLC(RFs &fs)
    {
    CTestImageCreator* self =
        new(ELeave) CTestImageCreator(fs);
    CleanupStack::PushL( self );
    return self;
    }

CTestImageCreator::CTestImageCreator(RFs &fs) :
    CActive( EPriorityHigh ),
    iFs(fs),
    iErr(KErrNone)
    {
    CActiveScheduler::Add( this );
    }

void CTestImageCreator::DoCancel()
    {
    if (iImageDecoder)
        {
        iImageDecoder->Cancel();
        }

    if (iImageEncoder)
        {
        iImageEncoder->Cancel();
        }

    iErr = KErrCancel;
    StopWait();
    }

CTestImageCreator::~CTestImageCreator()
    {
    Cancel();
    delete iSrcBitmap; // decoded image
    delete iTrgBitmap;
    delete iImageDecoder;
    delete iImageEncoder;
    delete iWait;
    }

void CTestImageCreator::DecodeL()
    {
    delete iImageDecoder;
    iImageDecoder = NULL;
    delete iSrcBitmap;
    iSrcBitmap = NULL;

    if (!iWait)
        {
        iWait = new( ELeave ) CActiveSchedulerWait();
        }

    iSourceFile = KSrcImage();
    iDestFile = KDestImage();

    // create the decoder
    // create the bitmap
    iSrcBitmap = new( ELeave )CFbsBitmap();
    iImageDecoder = CImageDecoder::FileNewL( iFs, iSourceFile );

    User::LeaveIfError( iSrcBitmap->Create( iImageDecoder->FrameInfo().iOverallSizeInPixels, iImageDecoder->FrameInfo().iFrameDisplayMode ));

    iState = EDecoding;
    iImageDecoder->Convert( &iStatus, *iSrcBitmap );
    SetActive();

    iWait->Start();
    User::LeaveIfError(iErr);
    }

void CTestImageCreator::ScaleL()
    {
    delete iScaler;
    iScaler = NULL;
    delete iTrgBitmap;
    iTrgBitmap = NULL;

    TInt EXPAND = 10;
    TInt width = iImageDecoder->FrameInfo().iOverallSizeInPixels.iWidth * EXPAND;
    TInt height = iImageDecoder->FrameInfo().iOverallSizeInPixels.iHeight * EXPAND;

    iTrgBitmap = new( ELeave )CFbsBitmap();
    User::LeaveIfError( iTrgBitmap->Create( TSize( width, height ), iImageDecoder->FrameInfo().iFrameDisplayMode ));

    iScaler = CBitmapScaler::NewL();
    iScaler->SetQualityAlgorithm( CBitmapScaler::EMaximumQuality );
    iScaler->Scale( &iStatus, *iSrcBitmap, *iTrgBitmap, EFalse);

    iState = EScaling;
    SetActive();
    }


void CTestImageCreator::EncodeL()
    {
    delete iImageEncoder;
    iImageEncoder = NULL;

    iImageEncoder = CImageEncoder::FileNewL( iFs, iDestFile, KMimeTypeJPEG);

    // Start conversion to a JPEG image
    iState = EEncoding;
    iImageEncoder->Convert( &iStatus, *iTrgBitmap );
    SetActive();
    }

TInt CTestImageCreator::RunError( TInt aError )
    {
    // Called when RunL leaves, notify error
    iErr = aError;
    StopWait();
    return KErrNone;
    }


void CTestImageCreator::RunL()
    {
    switch( iState )
        {
        case EDecoding:
            {
            if(iStatus == KErrNone)
                {
                ScaleL();
                }
            else if(iStatus == KErrUnderflow)
                {
                iImageDecoder->ContinueConvert( &iStatus );
                SetActive();
                }
            else
                {
                // Unknown error
                iErr = iStatus.Int();
                StopWait();
                }
            break;
            }

        case EScaling:
            {
            EncodeL();
            break;
            }

        case EEncoding:
            {
            iErr = iStatus.Int();
            StopWait();
            break;
            }

        default:
            {
            iErr = KErrCompletion;
            StopWait();
            break;
            }
        }
    }

void CTestImageCreator::StopWait()
    {
    if (iWait && iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    }
