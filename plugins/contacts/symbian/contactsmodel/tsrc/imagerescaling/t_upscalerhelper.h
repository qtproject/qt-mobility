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

#ifndef __T_UPSCALERHELPER_H__
#define __T_UPSCALERHELPER_H__

#include "rcntmodel.h"
#include "cntstd.h"
#include "cntdbconsts_internal.h"

#include <e32test.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntdb.h>
#include <bitmaptransforms.h>
#include <imageconversion.h>
#include <bautils.h>
#include <pathinfo.h>
#include <driveinfo.h>

_LIT(KDestImage, "c:\\data\\test.jpg");
_LIT8(KMimeTypeJPEG, "image/jpeg");
_LIT(KSrcImage, "c:\\data\\32kb.jpg");

class CTestImageCreator : public CActive
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
        static CTestImageCreator* NewLC(RFs &fs);
        ~CTestImageCreator();
        void DecodeL();
        void EncodeL();
        void ScaleL();

    protected: // CActive
        CTestImageCreator(RFs &fs);
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);
        void StopWait();

    private:
        CFbsBitmap* iSrcBitmap; // decoded image
        CFbsBitmap* iTrgBitmap; // decoded image
        RFs iFs;
        CImageDecoder* iImageDecoder;
        CImageEncoder* iImageEncoder;
        CBitmapScaler* iScaler;
        TState iState;
        TFileName iSourceFile;
        TFileName iDestFile;
        CActiveSchedulerWait* iWait;
        TInt iErr;
    };

#endif
