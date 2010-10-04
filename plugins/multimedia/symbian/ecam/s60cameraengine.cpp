/****************************************************************************
 **
 ** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the Qt Mobility Components.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** No Commercial Usage
 ** This file contains pre-release code and may not be distributed.
 ** You may use this file in accordance with the terms and conditions
 ** contained in the Technology Preview License Agreement accompanying
 ** this package.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain additional
 ** rights.  These rights are described in the Nokia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** If you have questions regarding the use of this file, please contact
 ** Nokia at qt-info@nokia.com.
 **
 **
 **
 **
 **
 **
 **
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include "s60cameraengine.h"
#include "s60cameraengineobserver.h"

#include <fbs.h> // CFbsBitmap

CCameraEngine::CCameraEngine()
{
}

CCameraEngine::CCameraEngine(TInt aCameraHandle, 
                             TInt aPriority, 
                             MCameraEngineObserver* aObserver) :
    // CBase initializes member variables to NULL
    iObserver(aObserver),
    iImageCaptureObserver(NULL),
    iAdvancedSettingsObserver(NULL),
    iViewfinderObserver(NULL),
    iCameraHandle(aCameraHandle),
    iPriority(aPriority),
    iEngineState(EEngineNotReady),
    iNew2LImplementation(false)
{
}

CCameraEngine::~CCameraEngine()
{
    StopViewFinder();
    ReleaseViewFinderBuffer();  // Releases iViewFinderBuffer
    ReleaseImageBuffer();       // Releases iImageBuffer + iImageBitmap, deletes iImageData

    if (iImageBitmap) {
        if (!iNew2LImplementation) {
            delete iImageBitmap;
            iImageBitmap = NULL;
        }
    }

    iAdvancedSettingsObserver = NULL;
    iImageCaptureObserver = NULL;
    iViewfinderObserver = NULL;

#ifdef S60_CAM_AUTOFOCUS_SUPPORT   
    delete iAutoFocus;
#endif // S60_CAM_AUTOFOCUS_SUPPORT

    if (iCamera) {
        iCamera->Release();
        delete iCamera;
        iCamera = NULL;
    }
}

TInt CCameraEngine::CamerasAvailable()
{
    return CCamera::CamerasAvailable();
}

CCameraEngine* CCameraEngine::NewL(TInt aCameraHandle,
                                   TInt aPriority,
                                   MCameraEngineObserver* aObserver)
{
    CCameraEngine* self = new (ELeave) CCameraEngine(aCameraHandle, aPriority, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CCameraEngine::ConstructL()
{
    if (!CCamera::CamerasAvailable())
        User::Leave(KErrHardwareNotAvailable);

#ifndef Q_CC_NOKIAX86 // Not Emulator
    TInt err(KErrNone);
#else // Emulator
    TInt err(KErrNotFound);
#endif // !(Q_CC_NOKIAX86)

#ifdef S60_31_PLATFORM
    // Construct CCamera object for S60 3.1 (NewL)
    iNew2LImplementation = false;
    TRAP(err, iCamera = CCamera::NewL(*this, iCameraHandle));
#else // For S60 3.2 onwards - use this constructor (New2L)
    iNew2LImplementation = true;
    TRAP(err, iCamera = CCamera::New2L(*this, iCameraHandle, iPriority));
#endif // S60_31_PLATFORM

#ifdef S60_CAM_AUTOFOCUS_SUPPORT
    TRAP(err, iAutoFocus = CCamAutoFocus::NewL(iCamera));
#endif // S60_CAM_AUTOFOCUS_SUPPORT

    if (iCamera == NULL)
        User::Leave(KErrNoMemory);

    // Create BitMap for Captured Image bitmaps (NewL version)
    if (!iNew2LImplementation)
        iImageBitmap = new (ELeave) CFbsBitmap;

    iCamera->CameraInfo(iCameraInfo);
}

void CCameraEngine::SetAdvancedObserver(MAdvancedSettingsObserver* aAdvancedSettingsObserver)
{
    iAdvancedSettingsObserver = aAdvancedSettingsObserver;
}

void CCameraEngine::SetImageCaptureObserver(MCameraEngineImageCaptureObserver* aImageCaptureObserver)
{
    iImageCaptureObserver = aImageCaptureObserver;
}

void CCameraEngine::SetViewfinderObserver(MCameraViewfinderObserver* aViewfinderObserver)
{
    iViewfinderObserver = aViewfinderObserver;
}

void CCameraEngine::ReserveAndPowerOn()
{
    if (!iCamera || iEngineState > EEngineNotReady) {
        iObserver->MceoHandleError(EErrReserve, KErrNotReady);
    }

    iCamera->Reserve();
}

void CCameraEngine::ReleaseAndPowerOff()
{
    if (iEngineState >= EEngineIdle) {
        StopViewFinder();
        FocusCancel();
        iCamera->PowerOff();
        iCamera->Release();
    }
    iEngineState = EEngineNotReady;
}

void CCameraEngine::StartViewFinderL(TSize& aSize)
{
    if (iEngineState < EEngineIdle) {
        User::Leave(KErrNotReady);
    }

    if (0 == (iCameraInfo.iOptionsSupported & TCameraInfo::EViewFinderBitmapsSupported)) {
        User::Leave(KErrNotSupported);
    }

    if (!iCamera->ViewFinderActive()) {
        iCamera->StartViewFinderBitmapsL(aSize);
    }
}

void CCameraEngine::StopViewFinder()
{
    if (iCamera && iCamera->ViewFinderActive()) {
        iCamera->StopViewFinder();
    }
}

void CCameraEngine::StartDirectViewFinderL(RWsSession& aSession,
                            CWsScreenDevice& aScreenDevice,
                            RWindowBase& aWindow,
                            TRect& aSize)
{
    if (iEngineState < EEngineIdle) {
        User::Leave(KErrNotReady);
    }

    if (0 == (iCameraInfo.iOptionsSupported & TCameraInfo::EViewFinderDirectSupported)) {
        User::Leave(KErrNotSupported);
    }

    if (!iCamera->ViewFinderActive()) {
        iCamera->StartViewFinderDirectL(aSession, aScreenDevice, aWindow, aSize);
    }
}

void CCameraEngine::PrepareL(TSize& aCaptureSize, CCamera::TFormat aFormat)
{
    iImageCaptureFormat = aFormat;

    TInt closestVar = KMaxTInt, selected = 0;
    TSize size;

    // Scan through supported capture sizes and select the closest match
    for (TInt index = 0; index < iCameraInfo.iNumImageSizesSupported; index++) {

        iCamera->EnumerateCaptureSizes(size, index, aFormat);
        if (size == aCaptureSize) {
            selected = index;
            break;
        }

        TSize varSz = size - aCaptureSize;
        TInt variation = varSz.iWidth * varSz.iHeight;
        if (variation < closestVar) {
            closestVar = variation;
            selected = index;
        }
    }

    iCamera->EnumerateCaptureSizes(aCaptureSize, selected, aFormat);
    iCamera->PrepareImageCaptureL(aFormat, selected);
}

void CCameraEngine::CaptureL()
{
    if (iEngineState < EEngineIdle)
        User::Leave(KErrNotReady);

    iCamera->CaptureImage();
    iEngineState = EEngineCapturing;
}

void CCameraEngine::cancelCapture()
{
    iCamera->CancelCaptureImage();
    iEngineState = EEngineIdle;
}

void CCameraEngine::HandleEvent(const TECAMEvent &aEvent)
{
    if (aEvent.iEventType == KUidECamEventReserveComplete) {
        ReserveComplete(aEvent.iErrorCode);
        return;
    }

    if (aEvent.iEventType == KUidECamEventPowerOnComplete) {
        PowerOnComplete(aEvent.iErrorCode);
        return;
    }

#if !defined(Q_CC_NOKIAX86) // Not Emulator
    // Other events; Exposure, Zoom, etc. (See ecamadvancedsettings.h)
    if (iAdvancedSettingsObserver)
        iAdvancedSettingsObserver->HandleAdvancedEvent(aEvent);

    iImageCaptureObserver->MceoHandleOtherEvent(aEvent);
#endif // !Q_CC_NOKIAX86

}

void CCameraEngine::ReserveComplete(TInt aError)
{
    if (aError == KErrNone) {
        iCamera->PowerOn();
    } else {
        iObserver->MceoHandleError(EErrReserve, aError);
    }
}

void CCameraEngine::PowerOnComplete(TInt aError)
{
    if (aError) {
        iObserver->MceoHandleError(EErrPowerOn, aError);
        iEngineState = EEngineNotReady;
        return;
    }

    // Init AutoFocus
#ifndef Q_CC_NOKIAX86  // Not Emulator
#ifdef S60_CAM_AUTOFOCUS_SUPPORT // S60 3.1
    if( iAutoFocus ) {
        TRAPD(afErr, iAutoFocus->InitL( *this ));
        if (afErr) {
            delete iAutoFocus;
            iAutoFocus = 0;
        }
    }
#endif // S60_CAM_AUTOFOCUS_SUPPORT
#endif // !Q_CC_NOKIAX86

    iEngineState = EEngineIdle;
    iObserver->MceoCameraReady();
}

/*
 * MCameraObserver2:
 * New viewfinder frame available
 */
void CCameraEngine::ViewFinderReady(MCameraBuffer &aCameraBuffer, TInt aError)
{
    iViewFinderBuffer = &aCameraBuffer;

    if (aError == KErrNone) {
        TRAPD(err, iViewfinderObserver->MceoViewFinderFrameReady(aCameraBuffer.BitmapL(0)));
        if (err)
            iObserver->MceoHandleError(EErrViewFinderReady, err);
    }
    else {
        iObserver->MceoHandleError(EErrViewFinderReady, aError);
    }
}

/*
 * MCameraObserver:
 * New viewfinder frame available
 */
void CCameraEngine::ViewFinderFrameReady(CFbsBitmap& aFrame)
{
    iViewfinderObserver->MceoViewFinderFrameReady(aFrame);
}

void CCameraEngine::ReleaseViewFinderBuffer()
{
    if (iViewFinderBuffer) {
        iViewFinderBuffer->Release();
        iViewFinderBuffer = NULL;
    }
}

void CCameraEngine::ReleaseImageBuffer()
{
    // Reset ImageBuffer
    if (iImageBuffer) {
        iImageBuffer->Release();
        iImageBuffer = NULL;
    }

    // Reset Bitmap
    if (iImageBitmap) {
        if (iImageBitmap->Handle() != 0) {
            iImageBitmap->Reset(); // Delete Handle to Bitmap
        }
    }

    // Reset Data pointers
    iImageData = NULL;
    iImageData2 = NULL;
}

/*
 * MCameraObserver2
 * Captured image is ready (New2L version)
 */
void CCameraEngine::ImageBufferReady(MCameraBuffer &aCameraBuffer, TInt aError)
{

    iImageBuffer = &aCameraBuffer;
    bool isBitmap = true;
    TInt err = KErrNone;

    switch (iImageCaptureFormat) {
        case CCamera::EFormatFbsBitmapColor4K:
        case CCamera::EFormatFbsBitmapColor64K:
        case CCamera::EFormatFbsBitmapColor16M:
        case CCamera::EFormatFbsBitmapColor16MU:
            TRAP(err, iImageBitmap = &iImageBuffer->BitmapL(0));
            if (err) {
                if (iImageCaptureObserver)
                    iImageCaptureObserver->MceoHandleError(EErrImageReady, err);
            }
            isBitmap = true;
            break;
        case CCamera::EFormatExif:
            TRAP(err, iImageData2 = iImageBuffer->DataL(0));
            if (err) {
                if (iImageCaptureObserver)
                    iImageCaptureObserver->MceoHandleError(EErrImageReady, err);
            }
            isBitmap = false;
            break;

        default:
            if (iImageCaptureObserver)
                iImageCaptureObserver->MceoHandleError(EErrImageReady, KErrNotSupported);
            return;
    }


    // Handle captured image
    HandleImageReady(aError, isBitmap);
}

/*
 * MCameraObserver
 * Captured image is ready (NewL version)
 */
void CCameraEngine::ImageReady(CFbsBitmap* aBitmap, HBufC8* aData, TInt aError)
{
    bool isBitmap = true;

    switch (iImageCaptureFormat) {
        case CCamera::EFormatFbsBitmapColor4K:
        case CCamera::EFormatFbsBitmapColor64K:
        case CCamera::EFormatFbsBitmapColor16M:
        case CCamera::EFormatFbsBitmapColor16MU:
            // Duplicate Bitmap (set Handle to the Captured Image)
            if (iImageBitmap) {
                iImageBitmap->Duplicate(aBitmap->Handle());
                delete aBitmap; // Delete original
            } else {
                if (iImageCaptureObserver)
                    iImageCaptureObserver->MceoHandleError(EErrImageReady, KErrGeneral);
            }
            isBitmap = true;
            break;
        case CCamera::EFormatExif:
            iImageData = aData;
            isBitmap = false;
            break;

        default:
            if (iImageCaptureObserver)
                iImageCaptureObserver->MceoHandleError(EErrImageReady, KErrNotSupported);
            return;
    }

    // Handle captured image
    HandleImageReady(aError, isBitmap);
}

void CCameraEngine::HandleImageReady(const TInt aError, const bool isBitmap)
{
    iEngineState = EEngineIdle;

    if (aError == KErrNone) {
        if (isBitmap)
            if (iImageCaptureObserver) {
                iImageCaptureObserver->MceoCapturedBitmapReady(iImageBitmap);
            }
            else
                ReleaseImageBuffer();
        else {
            if (iNew2LImplementation) {
                if (iImageCaptureObserver) {
                    iImageCaptureObserver->MceoCapturedDataReady(iImageData2);
                }
                else
                    ReleaseImageBuffer();
            } else {
                if (iImageCaptureObserver) {
                    iImageCaptureObserver->MceoCapturedDataReady(iImageData);
                }
                else
                    ReleaseImageBuffer();
            }
        }
    } else {
        if (iImageCaptureObserver)
            iImageCaptureObserver->MceoHandleError(EErrImageReady, aError);
    }
}

//=============================================================================
// S60 3.1 - AutoFocus support (Other platforms, see S60CameraSettings class)
//=============================================================================
// TODO: Set 1 common ifdef for all 3.1 autofocus functions?

void CCameraEngine::InitComplete(TInt aError)
{
    if (aError) {
        if (iImageCaptureObserver)
            iImageCaptureObserver->MceoHandleError(EErrAutoFocusInit, aError);
    }
}

void CCameraEngine::OptimisedFocusComplete(TInt aError)
{
    iEngineState = EEngineIdle;

    if (aError == KErrNone)
        if (iImageCaptureObserver)
            iImageCaptureObserver->MceoFocusComplete();
    else {
        if (iImageCaptureObserver)
            iImageCaptureObserver->MceoHandleError(EErrOptimisedFocusComplete, aError);
    }
}

TBool CCameraEngine::IsCameraReady() const
{
    // If reserved and powered on, but not focusing or capturing
    if (iEngineState == EEngineIdle)
        return ETrue;

    return EFalse;
}

TBool CCameraEngine::IsDirectViewFinderSupported() const
{
    if (iCameraInfo.iOptionsSupported & TCameraInfo::EViewFinderDirectSupported)
        return true;
    else
        return false;
}

TCameraInfo *CCameraEngine::cameraInfo()
{
    return &iCameraInfo;
}

TBool CCameraEngine::IsAutoFocusSupported() const
{
#ifndef Q_CC_NOKIAX86 // Not Emulator

#ifdef S60_CAM_AUTOFOCUS_SUPPORT // S60 3.1
    return (iAutoFocus) ? ETrue : EFalse;
#else // !S60_CAM_AUTOFOCUS_SUPPORT
    return EFalse;
#endif // S60_CAM_AUTOFOCUS_SUPPORT

#else // Q_CC_NOKIAX86 - Emulator
    return EFalse;
#endif // !Q_CC_NOKIAX86
}

/*
 * This function is used for focusing in S60 3.1 platform. Platforms from S60
 * 3.2 onwards should use the focusing provided by the S60CameraSettings class.
 */
void CCameraEngine::StartFocusL()
{
    if (iEngineState != EEngineIdle) {
        return;
    }

#ifndef Q_CC_NOKIAX86  // Not Emulator
#ifdef S60_CAM_AUTOFOCUS_SUPPORT // S60 3.1
    if (iAutoFocus) {
        if (!iAFRange) {
            iAFRange = CCamAutoFocus::ERangeNormal;
            iAutoFocus->SetFocusRangeL(iAFRange);
        }

    iAutoFocus->AttemptOptimisedFocusL();
    iEngineState = EEngineFocusing;
    }
#endif // S60_CAM_AUTOFOCUS_SUPPORT
#endif // !Q_CC_NOKIAX86
}

/*
 * This function is used for cancelling focusing in S60 3.1 platform. Platforms
 * from S60 3.2 onwards should use the focusing provided by the
 * S60CameraSettings class.
 */
void CCameraEngine::FocusCancel()
{
#ifndef Q_CC_NOKIAX86 // Not Emulator
#ifdef S60_CAM_AUTOFOCUS_SUPPORT
    if (iAutoFocus) {
        iAutoFocus->Cancel();
        iEngineState = EEngineIdle;
    }
#endif // S60_CAM_AUTOFOCUS_SUPPORT
#endif // !Q_CC_NOKIAX86
}

void CCameraEngine::SupportedFocusRanges(TInt& aSupportedRanges) const
{
    aSupportedRanges = 0;

#ifndef Q_CC_NOKIAX86 // Not Emulator
#ifdef S60_CAM_AUTOFOCUS_SUPPORT
    if (iAutoFocus) {
        // CCamAutoFocus doesn't provide a method for getting supported ranges!
        // Assume everything is supported (rather optimistic)
        aSupportedRanges = CCamAutoFocus::ERangeMacro |
                           CCamAutoFocus::ERangePortrait |
                           CCamAutoFocus::ERangeNormal |
                           CCamAutoFocus::ERangeInfinite;
    }
#endif // S60_CAM_AUTOFOCUS_SUPPORT
#endif // !Q_CC_NOKIAX86
}

void CCameraEngine::SetFocusRange(TInt aFocusRange)
{
#if !defined(Q_CC_NOKIAX86) // Not Emulator

#ifdef S60_CAM_AUTOFOCUS_SUPPORT
    if (iAutoFocus) {
        TRAPD(focusErr, iAutoFocus->SetFocusRangeL((CCamAutoFocus::TAutoFocusRange)aFocusRange));
        if (focusErr) {
            iObserver->MceoHandleError(EErrAutoFocusRange, focusErr);
        }
    }
#endif // S60_CAM_AUTOFOCUS_SUPPORT

#else // Q_CC_NOKIAX86 // Emulator
    Q_UNUSED(aFocusRange);
    if (iImageCaptureObserver)
        iImageCaptureObserver->MceoHandleError(EErrAutoFocusRange, KErrNotSupported);
#endif // !Q_CC_NOKIAX86
}

// End of file
