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
#include "cntimagerescaleutility.h"
#include "cntdbconsts_internal.h"

#include <bitmaptransforms.h>
#include <imageconversion.h>
#include <bautils.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <pathinfo.h>
#include <driveinfo.h>
#include <bautils.h>

#define KImageMaxSizePixels TSize(640, 640)
_LIT8(KMimeTypeJPEG, "image/jpeg");
const TInt KMaximumReductionFactor = 8;

CImageRescaler* CImageRescaler::NewL()
    {
    CImageRescaler* self = 
        new(ELeave) CImageRescaler();
    CleanupStack::PushL( self );
    
    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
    }

CImageRescaler::CImageRescaler() : 
    CActive( EPriorityHigh ),
    iErr(KErrNone)
    {
    }

void CImageRescaler::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    
    iWait = new( ELeave ) CActiveSchedulerWait();
    
    iImagesDirPath.Zero();
    TRAP_IGNORE( iImagesDirPath = TCntImageRescaleUtility::ImageDirectoryL() );
    
    CActiveScheduler::Add( this );    
    }

CImageRescaler::~CImageRescaler()
    {
    Cancel();
    Reset();
    iFs.Close();
    delete iWait;
    }


/**
Parse through contact fields and find the image field. Copy the image
pointed by the path to an internal folder and update the contact fields.
This function is used internally by contacts model when saving/updating
a contact that has an image. It is a synchronous function

@param aItem Contact containing fields to be processed. Once the image
is resized and copied into the new location, this parameter will contain
the image field with the new path. If failures occur while resizing, the
image field content is not updated
*/
void CImageRescaler::ProcessImageFieldL(CContactItem& aItem)
    {
    // If there is no images dir do not do anything
    if (!iImagesDirPath.Length())
        {
        return;
        }
    CContactItemFieldSet& fieldSet = aItem.CardFields();
    // Check if there is any image field before
    TInt index = fieldSet.Find(KUidContactFieldCodImage, KUidContactFieldVCardMapUnknown);
    
    // Update the image path field if found and the private image does not exists
    if (index != KErrNotFound )
        {
        // Get a copy of fields set
        // Image path field from list of contact fields
        CContactItemField& imageField = fieldSet[index];
        TPtrC oldImagePath = imageField.TextStorage()->Text();
        
        // Do not resize if old image is stored in private folder
        if ( oldImagePath.Length() && !ExistsInImagesDirectory(oldImagePath) )
            {
            // Resize image if needed
            TPath newImagePath;
            newImagePath = ResizeAndCopyImage(oldImagePath, aItem);
            TCntImageRescaleUtility::StoreImageFieldL( aItem, newImagePath );
            }
        }
    }

/**
Get the destionation path of the rescaled image.

@return Path to the image stored in the contacts images folder. This will be empty
if the image processing is not complete, has been cancelled or there was an error
when processing.
*/
TPath CImageRescaler::DestinationPath() const
    {
    if (iState == EComplete)
        {
        return iDestFile;
        }
    else
        {
        return TPath();
        }
    }

TPath CImageRescaler::GenerateDestPath(const TDesC& aSrcPath)
    {
    // Image file type
    TParse p;
    TInt err = p.Set(aSrcPath, NULL, NULL);
    TPath destFile;
    
    if (err == KErrNone)
        {
        // Generate the image path
        // Format <path>_timestamp_filename.ext
        destFile.Append(iImagesDirPath);
        destFile.Append(_L("_"));
        
        TTime time;
        time.UniversalTime();
        destFile.AppendNum(time.Int64());
        destFile.Append(_L("_"));
        destFile.Append(p.NameAndExt());
        }
    
    return destFile;
    }

/*
Scale down an image,if need be, and store it in the contacts images directory. Use
CImageRescaler::DestinationPath() functions to get the last generated image. This is
an asynchronous method that completes with an error status stored in the aRequestStatus.

@param aRequestStatus Request status to be used to complete the request.
@param aSrcPath A path to the source image

@pre aRequestStatus must not be NULL
*/
void CImageRescaler::ScaleAndStoreImage(TRequestStatus* aRequestStatus, const TDesC& aSrcPath)
    {
    ASSERT(aRequestStatus != NULL);
    iRequestStatus = aRequestStatus;
    *iRequestStatus = KRequestPending;
    
    // Cancel an on going request
    Cancel();
    
    // Reset image file paths
    iSourceFile.Zero();
    iSourceFile.Copy(aSrcPath);
    iDestFile = GenerateDestPath(aSrcPath);
    
    iState = EStartRescale;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

/**
Ensures that the image provided is removed from the contact images folder. The image is
only removed if it exists in the folder, otherwise nothing happens.

@param aImagePath A path pointing to the image to be removed.
*/
void CImageRescaler::EnsureImageRemoved(const TDesC& aImagePath)
    {
    // Check if aImagePath exists in the contact images folder
    if (ExistsInImagesDirectory(aImagePath))
        {
        // Ignore the error.
        BaflUtils::DeleteFile(iFs, aImagePath);
        }
    }

/**
Copy or resize if need be a source image to the internal folder

@return A file path of the new destination of the rescaled image

@param aSourceFile Source image path to be resized
@param aItem Contact item to help naming the new image
*/
TPath CImageRescaler::ResizeAndCopyImage(const TDesC& aSourceFile, const CContactItem& aItem)
    {
    TPtrC guid = const_cast<CContactItem&>(aItem).Guid();
    
    // Remove the old file. An empty guid means that this contact is new and
    // does not have any image associated with it in the images dir
    if (guid.Length())
        {
        TPath oldFile;
        oldFile.Append(iImagesDirPath);
        oldFile.Append(guid);
        oldFile.Append(_L("*"));
        
        // Remove previous file(s)
        BaflUtils::DeleteFile(iFs, oldFile); // Error value not necessary
        }
    
    TPath destFile = GenerateDestPath(aSourceFile);
    
    // Check the size of the image
    if (IsImageTooLarge(aSourceFile))
        {
        // Resize image
        TRAPD(err, ResizeImageL(aSourceFile, destFile));
        if (err != KErrNone && err != KErrArgument)
            {
            // Copy original image to the images directory to ensure
            // the contact keeps a copy of the image
            if (BaflUtils::CopyFile(iFs, aSourceFile, destFile) != KErrNone)
                {
                destFile.Zero();
                }
            }
        }
    else
        {
        // Copy image to the images directory if the image is not to big
        if (BaflUtils::CopyFile(iFs, aSourceFile, destFile) != KErrNone)
            {
            destFile.Zero();
            }
        }
    
    return destFile;
    }

TBool CImageRescaler::IsImageTooLarge(const TDesC& aSourceFile) const
{
    TEntry srcFile;
    if (iFs.Entry(aSourceFile,srcFile) != KErrNone) {
        // Assume image is not too large
        return EFalse;
    }
    return (srcFile.iSize > KMaxImageSize);
}

/**
Resize an image synchronously

@param  aSourceFile The source path of the file to resize
@param  aSourceFile The destination path of the resized image

@leave KErrCompletion The operation completes prematurely
@leave KErrCancel The operation is cancelled
@leave KErrArgument The argumants are wrong
*/
void CImageRescaler::ResizeImageL(const TDesC& aSourceFile, const TDesC& aDestFile)
    {
    // Synchronous rescaling
    // Cancel any current requests
    Cancel();
    
    // Reset image file paths
    iSourceFile = aSourceFile;
    iDestFile = aDestFile;

    // Start rescaling
    DecodeL();
    
    // Synchronize asynchronous operations (wait loop is finished in RunL when
    // image operations are done, or in RunError if an error occurs)
    iWait->Start();
    
    // Reset rescaler after request completion
    Reset();
    
    User::LeaveIfError(iErr);
    }

/**
Start decoding asychronously
*/
void CImageRescaler::DecodeL()
    {    
    // Check for a valid source file
    if (!iSourceFile.Length() || !BaflUtils::FileExists(iFs, iSourceFile))
        {
        User::Leave(KErrArgument);
        }
    
    delete iImageDecoder;
    iImageDecoder = NULL;
    delete iBitmap;
    iBitmap = NULL;

    // create the decoder
    // create the bitmap
    iBitmap = new( ELeave )CFbsBitmap();
    iImageDecoder = CImageDecoder::FileNewL( iFs, iSourceFile, CImageDecoder::EPreferFastDecode );
    
    TSize size = KImageMaxSizePixels;
    TSize orignialSize( iImageDecoder->FrameInfo().iOverallSizeInPixels );
    
    // size in both x and y dimension must be non-zero, positive value
    TSize loadSize( orignialSize) ;
    if( orignialSize.iHeight < size.iHeight || orignialSize.iWidth < size.iWidth )
        {
        loadSize = orignialSize;
        }
    else 
        {
        // size reduction factor. 1/1, 1/2, 1/4, and 1/8 are possible values for all
        // plug-ins.
        TInt reductionFactor = 1;
        while ( reductionFactor < KMaximumReductionFactor && ( size.iWidth <
            loadSize.iWidth / 2 ) && ( size.iHeight < loadSize.iHeight / 2 ))
            {
            // magic: use loadSize that is half of previous size
            loadSize.iWidth /= 2;
            loadSize.iHeight /= 2;
            reductionFactor *= 2;
            }
        // if original size is not an exact multiple of reduction factor,
        // we need to round loadSize up
        if ( reductionFactor && orignialSize.iWidth % reductionFactor )
            {
            loadSize.iWidth++;
            }
        if ( reductionFactor && orignialSize.iHeight % reductionFactor )
            {
            loadSize.iHeight++;
            }
        }

    User::LeaveIfError( iBitmap->Create( loadSize, EColor16M ));

    iState = EDecoding;
    iImageDecoder->Convert( &iStatus, *iBitmap );
    SetActive();
    }

/**
Start scaling asynchronously
*/
void CImageRescaler::ScaleL()
    {
    delete iScaler;
    iScaler = NULL;
    
    iScaler = CBitmapScaler::NewL();
    TSize s = KImageMaxSizePixels;
    iScaler->Scale( &iStatus, *iBitmap, s);

    iState = EScaling;
    SetActive();
    }

/**
Start encoding asynchronously
*/
void CImageRescaler::EncodeL()
    {
    delete iImageEncoder; 
    iImageEncoder = NULL;
    
    if ( iImagesDirPath.Length() && iDestFile.Length() )
        {
        iImageEncoder = CImageEncoder::FileNewL( iFs, iDestFile, KMimeTypeJPEG, CImageEncoder::EPreferFastEncode );
        if ( iImageEncoder )
            {        
            // Start converstion to a JPEG image
            iState = EEncoding;
            iImageEncoder->Convert( &iStatus, *iBitmap );            
            }
        }
    else
        {
        //If there is no images dir do not do anything
        //Just complete the request
        iState = EEncoding;
        TRequestStatus* ptrStatus = &iStatus;
        User::RequestComplete( ptrStatus, KErrNone );
        }
    
    SetActive();
    }

void CImageRescaler::DoCancel()
    {
    if (iImageDecoder)
        {
        iImageDecoder->Cancel();
        }
    
    if (iImageEncoder)
        {
        iImageEncoder->Cancel();
        }
    
    if (iScaler)
        {
        iScaler->Cancel();
        }

    iErr = KErrCancel;
    CompleteRerescale();
    }

void CImageRescaler::RunL()
    {
    switch( iState ) 
        {
        case EStartRescale:
            {
            // Start rescaling process
            DecodeL();
            break;
            }
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
                CompleteRerescale();
                }
            break;
            }
        case EEncoding:
            {
            iErr = iStatus.Int();
            CompleteRerescale();
            break;
            }
        case EScaling:
            {
            EncodeL();
            break;
            }
        default:
            {
            iErr = KErrGeneral;
            CompleteRerescale();
            break;
            }
        }   
    }

void CImageRescaler::StopWait()
    {    
    if (iWait && iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    }

void CImageRescaler::CompleteRerescale()
    {
    iState = EComplete;
    if (iRequestStatus)
        {
        TRequestStatus* status = iRequestStatus;
        User::RequestComplete( status, iErr );
        iRequestStatus = NULL;
        }
    StopWait();
    }

TInt CImageRescaler::RunError( TInt aError )
    {
    // Called when RunL leaves, notify error
    iErr = aError;
    CompleteRerescale();
    return KErrNone;
    }

/**
Initialise values to defaults.
*/
void CImageRescaler::Reset()
    {
    if (iImageDecoder)
        {
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
    
    if (iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    
    if (iScaler)
        {
        delete iScaler;
        iScaler = NULL;
        }
    
    if (iImageEncoder)
        {
        delete iImageEncoder; 
        iImageEncoder = NULL;
        }
    }

/*
Check if the provided image exists in the contact images directory

@param A path pointing to the image to check.
@return True if the image exist in the image, false if the image path is empty or
does not exist in the contact images directory.
*/
TBool CImageRescaler::ExistsInImagesDirectory(const TDesC& aImagePath) const
    {
    if (iImagesDirPath.Length() && aImagePath.Length() && aImagePath.Find(iImagesDirPath) != KErrNotFound)
        {
        return true;
        }
    else
        {
        return false;
        }
    }

// End file
