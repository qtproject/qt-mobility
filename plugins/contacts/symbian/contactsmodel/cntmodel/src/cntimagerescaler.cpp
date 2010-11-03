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
#include "cntdbconsts_internal.h"

#include <bitmaptransforms.h>
#include <imageconversion.h>
#include <hal.h>
#include <bautils.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <pathinfo.h>
#include <driveinfo.h>
#include <bautils.h>

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
    
    TInt screenWidth = 0;
    TInt screenHeight = 0;
    
    HAL::Get( HALData::EDisplayXPixels, screenWidth );
    HAL::Get ( HALData::EDisplayYPixels, screenHeight );
    
    TInt length = screenHeight > screenWidth ? screenHeight : screenWidth;
    iScreenSize = TSize( length, length );
    
    iWait = new( ELeave ) CActiveSchedulerWait();
    
    SetImagesDirL();
    
    CActiveScheduler::Add( this );
    }

/**
Find the images folder. If it exists set the path in a local variable
*/
void CImageRescaler::SetImagesDirL()
    {
    TInt drive;
    
#ifdef __WINS__
    TInt err = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultPhoneMemory, drive);
#else
    TInt err = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultMassStorage, drive);
#endif
    
    // Do not leave with this error. The phone does not have to have this support
    if (err == KErrNotSupported)
        {
        return;
        }
    else
        {
        User::LeaveIfError(err);
        }
    
    // Get the root path in this drive to create
    // to create the images directory
    iImagesDirPath = TPath();
    User::LeaveIfError(PathInfo::GetRootPath(iImagesDirPath, drive));
    iImagesDirPath.Append(KImagesFolder);
    
    // Check if images directory exists
    if (!BaflUtils::FolderExists(iFs, iImagesDirPath))
        {
        iImagesDirPath.Zero();
        }
    }
CImageRescaler::~CImageRescaler()
    {
    Cancel();
    iFs.Close();
    delete iWait;
    delete iImageEncoder;
    delete iImageDecoder;
    delete iBitmap;
    delete iScaler;
    }


/**
Parse through contact fields and find the image field. Copy the image
pointed by the path to an internal folder and update the contact fields.

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
    
    // Check if there is any image field before
    TInt index = aItem.CardFields().Find(KUidContactFieldCodImage, KUidContactFieldVCardMapUnknown);
    
    // Update the image path field if found
    if (index != KErrNotFound)
        {
        // Get a copy of fields set
        CContactItemFieldSet& fieldSet = aItem.CardFields();
        
        // Image path field from list of contact fields
        CContactItemField& field = fieldSet[index];
        TPtrC oldImagePath = field.TextStorage()->Text();
        
        // Do not resize if old image is stored in private folder
        if (oldImagePath.Find(iImagesDirPath) == KErrNotFound)
            {
            // Resize image if needed
            TPath newImagePath;
            newImagePath = ResizeAndCopyImage(oldImagePath, aItem);
            if (newImagePath.Length())
                {
                // Replace old image field
                fieldSet.Remove(index);
                
                CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldCodImage);
                newField->SetMapping(KUidContactFieldVCardMapUnknown);
                newField->TextStorage()->SetTextL(newImagePath);
                fieldSet.AddL(*newField); // Ownership taken
                CleanupStack::Pop(newField);
                }
            }
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
        
        // Remove previous file
        BaflUtils::DeleteFile(iFs, oldFile); // Error value not necessary
        }
    
    // Image file type
    TParse p;
    p.Set(aSourceFile, NULL, NULL);
    
    // Generate the image path
    // Format <path>guid_timestamp_filename.ext
    TPath destFile;
    destFile.Append(iImagesDirPath);
    destFile.Append(guid);
    destFile.Append(_L("_"));
    
    TTime time;
    time.UniversalTime();
    destFile.AppendNum(time.Int64());
    destFile.Append(_L("_"));
    destFile.Append(p.NameAndExt());
    
    // Check the size of the image
    if (IsImageToLarge(aSourceFile))
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

TBool CImageRescaler::IsImageToLarge(const TDesC& aSourceFile) const
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
    // Test args
    if (!aSourceFile.Length() || !aDestFile.Length() || !BaflUtils::FileExists(iFs, aSourceFile))
        {
        User::Leave(KErrArgument);
        }
    
    // Cancel any current requests
    Cancel();
    
    iSourceFile = aSourceFile;
    iDestFile = aDestFile;

    // Start rescaling
    DecodeL();
    
    // Synchronize asynchronous operations (wait loop is finished in RunL when
    // image operations are done, or in RunError if an error occurs)
    iWait->Start();
    User::LeaveIfError(iErr);
    }

/**
Start decoding asychronously
*/
void CImageRescaler::DecodeL()
    {
    delete iImageDecoder;
    iImageDecoder = NULL;
    delete iBitmap;
    iBitmap = NULL;

    // create the decoder
    // create the bitmap
    iBitmap = new( ELeave )CFbsBitmap();
    iImageDecoder = CImageDecoder::FileNewL( iFs, iSourceFile );
    
    TSize size(iScreenSize);
    TSize orignialSize( iImageDecoder->FrameInfo().iOverallSizeInPixels );
    TBool fullyScaleable( iImageDecoder->FrameInfo().iFlags & TFrameInfo::EFullyScaleable );
    
    // size in both x and y dimension must be non-zero, positive value
    TSize loadSize( orignialSize) ;
    if( orignialSize.iHeight < size.iHeight || orignialSize.iWidth < size.iWidth )
        {
        loadSize = orignialSize;
        }
    else if(fullyScaleable)
        {
        // image fully scalable, decoder converts to required size
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

    User::LeaveIfError( iBitmap->Create( loadSize, iImageDecoder->FrameInfo().iFrameDisplayMode ));

    iState = EDecoding;
    iImageDecoder->Convert( &iStatus, * iBitmap );
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
    iScaler->Scale( &iStatus, *iBitmap, iScreenSize);

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

    iImageEncoder = CImageEncoder::FileNewL( iFs, iDestFile, KMimeTypeJPEG);
    
    // Start converstion to a JPEG image
    iState = EEncoding;
    iImageEncoder->Convert( &iStatus, *iBitmap );
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
    StopWait();
    }

void CImageRescaler::RunL()
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
        case EEncoding:
            {
            iErr = iStatus.Int();
            StopWait();
            break;
            }
        case EScaling:
            {
            EncodeL();
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

void CImageRescaler::StopWait()
    {    
    if (iWait && iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    }

TInt CImageRescaler::RunError( TInt aError )
    {
    // Called when RunL leaves, notify error
    iErr = aError;
    StopWait();
    return KErrNone;
    }
