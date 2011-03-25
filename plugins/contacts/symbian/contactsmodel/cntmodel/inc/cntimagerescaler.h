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
 * This class is used to rescale thumbnail images large images assigned 
 * to contacts.
 * 
 * Contacts containing a thumbnail fields store paths of the thumbnail image which 
 * is in the file system. Each contact should own its own image meaning that the life 
 * of this thumbnail image extends until the parent contact is deleted. The main aim 
 * of checking the size of these images is to avoid having to big images stored in the 
 * internal folder. Too large images are rescaled using an instance of this class.
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
        EScaling
        };
    
    public:
        static CImageRescaler* NewL();
        ~CImageRescaler();
        
    public:
        void ProcessImageFieldL(CContactItem& aItem);

    protected: // CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    private:
        CImageRescaler();
        void ConstructL();
        TPath ResizeAndCopyImage(const TDesC& aSourceFile, const CContactItem& aItem);
        TBool IsImageToLarge(const TDesC& aSourceFile) const;
        void ResizeImageL(const TDesC& aSourceFile, const TDesC& aDestFile);
        void DecodeL();
        void EncodeL();
        void ScaleL();
        void StopWait();

    private:
        CFbsBitmap* iBitmap; // decoded image
        RFs iFs;
        CImageDecoder* iImageDecoder;
        CImageEncoder* iImageEncoder;
        CBitmapScaler* iScaler;
        TState iState;
        TFileName iSourceFile;
        TFileName iDestFile;
        TSize iScreenSize;
        CActiveSchedulerWait* iWait;
        TInt iErr;
        TPath iImagesDirPath;
    };

#endif // __CNTIMAGERESCALER_H__
