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
#ifndef __CNTIMAGERESCALER_H__
#define __CNTIMAGERESCALER_H__

#include <f32file.h>
#include <e32base.h>

// Forward declarations
class CFbsBitmap;
class CBitmapScaler;
class CImageDecoder;
class CImageEncoder;
class CActiveSchedulerWait;
class CContactItem;

/**
 * This class is used to rescale and image assigned to a contact and store an copy
 * of the rescaled image to a contacts images folder as well as store the path in
 * the image field of the CContactItem. A thumbnail is generated also and stored in
 * the thumbnail field of the CContactItem.
 * 
 * The rationale behind this is that each contact should own its own image meaning that 
 * the life of this image extends until the parent contact is deleted. The main aim 
 * of checking the size of these images is to avoid having to big images stored in the 
 * contact images folder. Too large images are rescaled using an instance of this class.
 */
class CImageRescaler : public CActive
    {
    // test class
    friend class T_CntImageRescaler;
    
    // Rescaling process states
    enum TState 
        {
        EStartRescale = 0,
        EDecoding,
        EEncoding,
        EScaling,
        EComplete
        };
    
    public:
        static CImageRescaler* NewL();
        ~CImageRescaler();
        
    public:
        void ProcessImageFieldL(CContactItem& aItem);
        void ScaleAndStoreImage(TRequestStatus* aRequestStatus, const TDesC& aSrcPath);
        void EnsureImageRemoved(const TDesC& aImagePath);
        TPath DestinationPath() const;

    protected: // CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    private:
        CImageRescaler();
        void ConstructL();
        TPath ResizeAndCopyImage(const TDesC& aSourceFile, const CContactItem& aItem);
        TBool IsImageTooLarge(const TDesC& aSourceFile) const;
        void ResizeImageL(const TDesC& aSourceFile, const TDesC& aDestFile);
        void StartRescaleL();
        void DecodeL();
        void EncodeL();
        void ScaleL();
        void StopWait();
        void Reset();
        void CompleteRerescale();
        TPath GenerateDestPath(const TDesC& aSrcPath);
        TBool ExistsInImagesDirectory(const TDesC& aImagePath) const;

    private:
        CFbsBitmap* iBitmap; // decoded image
        RFs iFs;
        CImageDecoder* iImageDecoder;
        CImageEncoder* iImageEncoder;
        CBitmapScaler* iScaler;
        TState iState;
        TPath iSourceFile;
        TPath iDestFile;
        CActiveSchedulerWait* iWait;
        TInt iErr;
        TPath iImagesDirPath;
        TRequestStatus* iRequestStatus;
    };

#endif // __CNTIMAGERESCALER_H__
