/****************************************************************************
 **
 ** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the Qt Mobility Components.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial Usage
 ** Licensees holding valid Qt Commercial licenses may use this file in
 ** accordance with the Qt Solutions Commercial License Agreement provided
 ** with the Software or, alternatively, in accordance with the terms
 ** contained in a written agreement between you and Nokia.
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
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 ** Please note Third Party Software included with Qt Solutions may impose
 ** additional restrictions and it is the user's responsibility to ensure
 ** that they have met the licensing requirements of the GPL, LGPL, or Qt
 ** Solutions Commercial license and the relevant license of the Third
 ** Party Software they are using.
 **
 ** If you are unsure which license is appropriate for your use, please
 ** contact the sales department at qt-sales@nokia.com.
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
    iObserver(aObserver),
    iImageCaptureObserver(aObserver),
    iAdvancedSettingsObserver(NULL),
    iViewfinderObserver(NULL),
    iCameraHandle(aCameraHandle),
    iPriority(aPriority),
    iEngineState(EEngineNotReady)
{
}

CCameraEngine::~CCameraEngine()
{
    StopViewFinder();
    ReleaseViewFinderBuffer();  // Releases iViewFinderBuffer
    ReleaseImageBuffer();       // Releases iImageBuffer + iImageBitmap, deletes iImageData
    delete iImageBitmap;
    iAdvancedSettingsObserver = NULL;
    iImageCaptureObserver = NULL;
    iViewfinderObserver = NULL;

#ifdef S60_CAM_AUTOFOCUS_SUPPORT   
    delete iAutoFocus;
#endif

    delete iCamera;
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
    if (!CCamera::CamerasAvailable()) {
        User::Leave(KErrHardwareNotAvailable);
    }
#ifndef Q_CC_NOKIAX86 // Not Emulator
    TInt v2err( KErrNone );
#else // Emulator
    TInt v2err(KErrNotFound);
#endif // !(Q_CC_NOKIAX86)

#ifdef S60_CAM_AUTOFOCUS_SUPPORT
    // Construct CCamera object for S60 3.1 (NewL)
    if( !iCamera || v2err ) {
        iCamera = CCamera::NewL( *this, iCameraHandle );
        TRAP_IGNORE( iAutoFocus = CCamAutoFocus::NewL( iCamera ) );
    }
#endif // !S60_CAM_AUTOFOCUS_SUPPORT

    // For S60 3.2 onwards - use this constructor (New2L)
#if (defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER) || \
     defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || \
     defined(SYMBIAN_3_PLATFORM))
    TRAP( v2err, iCamera = CCamera::New2L( *this, iCameraHandle, iPriority ) );
#endif

    if (iCamera == NULL) {
        User::Leave(KErrNoMemory);
    }

    iImageBitmap = new (ELeave) CFbsBitmap;
    iCamera->CameraInfo(iCameraInfo);
}

void CCameraEngine::SetAdvancedObserver(MAdvancedSettingsObserver* aAdvancedSettingsObserver)
{
    iAdvancedSettingsObserver = aAdvancedSettingsObserver;
}

void CCameraEngine::SetImageCaptureObserver(MCameraEngineObserver* aImageCaptureObserver)
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
    if (iEngineState != EEngineIdle) {
        User::Leave(KErrNotReady);
    }

    if (0 == (iCameraInfo.iOptionsSupported & TCameraInfo::EViewFinderBitmapsSupported)) {
        User::Leave(KErrNotSupported);
    }

    if (!iCamera->ViewFinderActive()) {
        iCamera->StartViewFinderBitmapsL(aSize);
    }
    iEngineState = EEngineViewFinding;
}

void CCameraEngine::StopViewFinder()
{
    if (iCamera && iCamera->ViewFinderActive()) {
        iCamera->StopViewFinder();
        iEngineState = EEngineIdle;
    }
}

void CCameraEngine::StartDirectViewFinderL(RWsSession& aSession,
                            CWsScreenDevice& aScreenDevice,
                            RWindowBase& aWindow,
                            TRect& aSize)
{
    if (iEngineState != EEngineIdle) {
        User::Leave(KErrNotReady);
    }

    if (0 == (iCameraInfo.iOptionsSupported & TCameraInfo::EViewFinderDirectSupported)) {
        User::Leave(KErrNotSupported);
    }

    if (!iCamera->ViewFinderActive()) {
        iCamera->StartViewFinderDirectL(aSession, aScreenDevice, aWindow, aSize);
    }
    iEngineState = EEngineViewFinding;
}

void CCameraEngine::PrepareL(TSize& aCaptureSize, CCamera::TFormat aFormat)
{
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

    iObserver->MceoHandleOtherEvent(aEvent);
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
        // Let's trust this does not leave as aError == KErrNone
        iViewfinderObserver->MceoViewFinderFrameReady(aCameraBuffer.BitmapL(0));
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
        iViewFinderBuffer = 0;
    }
}

void CCameraEngine::ReleaseImageBuffer()
{
    if (iImageBuffer) {
        iImageBuffer->Release();
        iImageBuffer = 0;
    }

    if (iImageBitmap) {
        iImageBitmap->Reset();
    }

    delete iImageData;
    iImageData = 0;
}

/*
 * MCameraObserver2
 * Captured image is ready
 */
void CCameraEngine::ImageBufferReady(MCameraBuffer &aCameraBuffer, TInt aError)
{
    CFbsBitmap* bitmap = 0;
    TDesC8* data = 0;

    iImageBuffer = &aCameraBuffer;

    // Ignore leaves here, trust that aError is set correctly
    // TODO Can we trust?
    TRAP_IGNORE( bitmap = &aCameraBuffer.BitmapL(0) );
    TRAP_IGNORE( data = aCameraBuffer.DataL(0) );

    HandleImageReady(bitmap, data, aError);
}

/*
 * MCameraObserver
 * Captured image is ready
 */
void CCameraEngine::ImageReady(CFbsBitmap* aBitmap, HBufC8* aData, TInt aError)
{
    if (aBitmap) {
        iImageBitmap->Duplicate(aBitmap->Handle());
        delete aBitmap; // Delete original
    }
    else
        iImageData = aData;

    HandleImageReady(iImageBitmap, aData, aError);
}

void CCameraEngine::HandleImageReady(CFbsBitmap* aBitmap, TDesC8* aData, TInt aError)
{
    iEngineState = EEngineIdle;

    if (aError == KErrNone) {
        if (aData) {
            iImageCaptureObserver->MceoCapturedDataReady(aData);
        }
        else if (aBitmap) {
            iImageCaptureObserver->MceoCapturedBitmapReady(aBitmap);
        }
    } else {
        iImageCaptureObserver->MceoHandleError(EErrImageReady, aError);
    }
}

//=============================================================================
// S60 3.1 - AutoFocus support (Other platforms, see S60CameraSettings class)
//=============================================================================

void CCameraEngine::InitComplete(TInt aError)
{
    if (aError) {
        iImageCaptureObserver->MceoHandleError(EErrAutoFocusInit, aError);
    }
}

void CCameraEngine::OptimisedFocusComplete(TInt aError)
{
    iEngineState = EEngineIdle;

    if (aError == KErrNone)
        iImageCaptureObserver->MceoFocusComplete();
    else {
        iImageCaptureObserver->MceoHandleError(EErrOptimisedFocusComplete, aError);
    }
}

TBool CCameraEngine::IsCameraReady() const
{
    // If reserved and powered on, but not focusing or capturing
    if (iEngineState == EEngineIdle || iEngineState == EEngineViewFinding)
        return ETrue;

    return EFalse;
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
    if (iEngineState != EEngineIdle && iEngineState != EEngineViewFinding) {
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
    iImageCaptureObserver->MceoHandleError(EErrAutoFocusRange, KErrNotSupported);
#endif // !Q_CC_NOKIAX86
}

// End of file
