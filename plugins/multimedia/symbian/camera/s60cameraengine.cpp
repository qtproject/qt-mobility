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
#include <fbs.h>
#include <QtCore/qdebug.h>

#include "s60cameraengine.h"
#include "s60cameraengineobserver.h"


CCameraEngine::CCameraEngine()
{
}

CCameraEngine::CCameraEngine(TInt aCameraHandle, TInt aPriority, MCameraEngineObserver* aObserver) :
    iCameraHandle(aCameraHandle), iPriority(aPriority), iObserver(aObserver), iEngineState(
        EEngineNotReady), iAdvancedSettingsObserver(NULL), iImageCaptureObserver(aObserver)
{
}

CCameraEngine::~CCameraEngine()
{
    StopViewFinder();
    ReleaseViewFinderBuffer(); // releases iViewFinderBuffer
    ReleaseImageBuffer(); // releases iImageBuffer + iImageBitmap, deletes iImageData
    delete iImageBitmap;
    iAdvancedSettingsObserver = NULL;
    iImageCaptureObserver = NULL;

#ifdef S60_CAM_AUTOFOCUS_SUPPORT   
    delete iAutoFocus;
#endif  

    delete iCamera;
}

TInt CCameraEngine::CamerasAvailable()
{
    return CCamera::CamerasAvailable();
}

CCameraEngine* CCameraEngine::NewL(TInt aCameraHandle, TInt aPriority,
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
#ifndef Q_CC_NOKIAX86
    TInt v2err( KErrNone );
#else
    TInt v2err(KErrNotFound);
#endif // !(Q_CC_NOKIAX86)
#ifdef S60_CAM_AUTOFOCUS_SUPPORT 
    // construct CCamera object for S60 3.0/3.1
    if( !iCamera || v2err ) {
        iCamera = CCamera::NewL( *this, iCameraHandle );
        // Setting up AF may fail, can be ignored
        TRAP_IGNORE( iAutoFocus = CCamAutoFocus::NewL( iCamera ) );
    }
#endif //S60_CAM_AUTOFOCUS_SUPPORT
    // for 3.2 onwards use this constructor
#if (defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER) || defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER))
    TRAP( v2err, iCamera = CCamera::New2L( *this, iCameraHandle, iPriority ) );
#endif    
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

void CCameraEngine::ReserveAndPowerOn()
{
    iCamera->Reserve();
}

void CCameraEngine::ReleaseAndPowerOff()
{
    //set engine state to correct
    iEngineState = EEngineNotReady;
    if (iCameraReserved) {
        StopViewFinder();
        FocusCancel();
        iCamera->PowerOff();
        iCamera->Release();
        iCameraReserved = EFalse;
    }
}

void CCameraEngine::StartViewFinderL(TSize& aSize)
{
    if (iEngineState != EEngineIdle)
        User::Leave(KErrNotReady);

    if (0 == (iCameraInfo.iOptionsSupported & TCameraInfo::EViewFinderBitmapsSupported))
        User::Leave(KErrNotSupported);

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
    if (!iCameraReserved)
        User::Leave(KErrNotReady);
    iCamera->CaptureImage();
    iEngineState = EEngineCapturing;
}

void CCameraEngine::HandleEvent(const TECAMEvent &aEvent)
{
    //qDebug()<<"CCameraEngine::HandleEvent";
    if (aEvent.iEventType == KUidECamEventReserveComplete) {
        ReserveComplete(aEvent.iErrorCode);
        return;
    }

    if (aEvent.iEventType == KUidECamEventPowerOnComplete) {
        PowerOnComplete(aEvent.iErrorCode);
        return;
    }
    // AUTOFOCUS RELATED EVENTS
    // Was originally for advancedsettings, now removed.
#if !defined(Q_CC_NOKIAX86)
    // Other events; exposure, digital zoom etc.. see ecamadvancedsettings.h
    if (iAdvancedSettingsObserver) {
        iAdvancedSettingsObserver->HandleAdvancedEvent(aEvent);
    }
    iObserver->MceoHandleOtherEvent(aEvent);

#endif // !Q_CC_NOKIAX86
}

void CCameraEngine::ReserveComplete(TInt aError)
{
    if (aError == KErrNone) {
        iCamera->PowerOn();
    }
    else {
        iObserver->MceoHandleError(EErrReserve, aError);
    }
}

void CCameraEngine::PowerOnComplete(TInt aError)
{
    iCameraReserved = (aError == KErrNone) ? ETrue : EFalse;

    if (!iCameraReserved) {
        iObserver->MceoHandleError(EErrPowerOn, aError);
        return;
    }

    // Init AF
#if !defined(Q_CC_NOKIAX86)
#ifdef S60_CAM_AUTOFOCUS_SUPPORT  
    if( iAutoFocus ) {
        TRAPD(afErr, iAutoFocus->InitL( *this ));
        if (afErr) {
            delete iAutoFocus;
            iAutoFocus = 0;
        }
    }
#endif  
#endif

    iEngineState = EEngineIdle;
    iObserver->MceoCameraReady();
}

void CCameraEngine::ViewFinderReady(MCameraBuffer &aCameraBuffer, TInt aError)
{
    iViewFinderBuffer = &aCameraBuffer;

    if (aError == KErrNone) {
        // Let's trust this does not leave as aError == KErrNone
        iObserver->MceoViewFinderFrameReady(aCameraBuffer.BitmapL(0));
    }
    else {
        iObserver->MceoHandleError(EErrViewFinderReady, aError);
    }
}

void CCameraEngine::ViewFinderFrameReady(CFbsBitmap& aFrame)
{
    iObserver->MceoViewFinderFrameReady(aFrame);
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

void CCameraEngine::ImageBufferReady(MCameraBuffer &aCameraBuffer, TInt aError)
{
    CFbsBitmap* bitmap = 0;
    TDesC8* data = 0;

    iImageBuffer = &aCameraBuffer;

    // Ignore leaves here, trust that aError is set correctly  
    TRAP_IGNORE( bitmap = &aCameraBuffer.BitmapL(0) );
    TRAP_IGNORE( data = aCameraBuffer.DataL(0) );

    HandleImageReady(bitmap, data, aError);
}

void CCameraEngine::ImageReady(CFbsBitmap* aBitmap, HBufC8* aData, TInt aError)
{
    if (aBitmap) {
        iImageBitmap->Duplicate(aBitmap->Handle());
    }
    else {
        iImageData = aData;
    }

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
    }
    else {
        iImageCaptureObserver->MceoHandleError(EErrImageReady, aError);
    }
}

void CCameraEngine::InitComplete(TInt aError)
{
    if (aError) {
        iImageCaptureObserver->MceoHandleError(EErrAutoFocusInit, aError);
    }
}

void CCameraEngine::OptimisedFocusComplete(TInt aError)
{
    iEngineState = EEngineIdle;

    if (aError == KErrNone) {
        iImageCaptureObserver->MceoFocusComplete();
    }
    else {
        iImageCaptureObserver->MceoHandleError(EErrOptimisedFocusComplete, aError);
    }
}

TBool CCameraEngine::IsAutoFocusSupported() const
{
#if !defined(Q_CC_NOKIAX86)
#ifdef S60_CAM_AUTOFOCUS_SUPPORT  
    return (iAutoFocus) ? ETrue : EFalse;
#endif
#else
    return EFalse;
#endif // !Q_CC_NOKIAX86 
}

void CCameraEngine::StartFocusL()
{
    if (iEngineState != EEngineIdle && iEngineState != EEngineViewFinding) {
        return;
    }

#if !defined(Q_CC_NOKIAX86)
#ifdef S60_CAM_AUTOFOCUS_SUPPORT
    if (iAutoFocus) {
        if (!iAFRange) {
            iAFRange = CCamAutoFocus::ERangeNormal;
            iAutoFocus->SetFocusRangeL( iAFRange );
        }
    iAutoFocus->AttemptOptimisedFocusL();
    iEngineState = EEngineFocusing;
    }
#endif
#endif    
}

/*
 -------------------------------------------------------------------------------
 Cancels an ongoing autofocus operation
 -------------------------------------------------------------------------------
 */
void CCameraEngine::FocusCancel()
{
#ifndef Q_CC_NOKIAX86
#ifdef S60_CAM_AUTOFOCUS_SUPPORT
    if (iAutoFocus) {
        iAutoFocus->Cancel();
        iEngineState = EEngineIdle;
    }
#endif
#endif // !Q_CC_NOKIAX86
}

void CCameraEngine::SupportedFocusRanges(TInt& aSupportedRanges) const
{
    aSupportedRanges = 0;
#ifndef Q_CC_NOKIAX86
#ifdef S60_CAM_AUTOFOCUS_SUPPORT
    if (iAutoFocus){
        // CCamAutoFocus doesn't provide a method for getting supported ranges :(
        // Assume everything is supported (rather optimistic...)
        aSupportedRanges = CCamAutoFocus::ERangeMacro |
        CCamAutoFocus::ERangePortrait |
        CCamAutoFocus::ERangeNormal |
        CCamAutoFocus::ERangeInfinite;
    }
#endif //   
#endif // !Q_CC_NOKIAX86
}

void CCameraEngine::SetFocusRange(TInt aFocusRange)
{
#if !defined(Q_CC_NOKIAX86)
#ifdef S60_CAM_AUTOFOCUS_SUPPORT
    if (iAutoFocus) {
        TRAPD(focusErr,
            iAutoFocus->SetFocusRangeL((CCamAutoFocus::TAutoFocusRange)aFocusRange)
        );
        if (focusErr) {
            iObserver->MceoHandleError(EErrAutoFocusRange, focusErr);
        }
    }
#endif  

#else // Q_CC_NOKIAX86
    iImageCaptureObserver->MceoHandleError(EErrAutoFocusRange, KErrNotSupported);
#endif // !Q_CC_NOKIAX86
}

// eof
