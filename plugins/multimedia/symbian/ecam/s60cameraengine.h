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
 ** accordance with the Qt Commercial License Agreement provided with
 ** the Software or, alternatively, in accordance with the terms
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
 ** If you are unsure which license is appropriate for your use, please
 ** contact the sales department at qt-sales@nokia.com.
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/
#ifndef S60CCAMERAENGINE_H
#define S60CCAMERAENGINE_H

// INCLUDES
#include <e32base.h>
#include <ecam.h>                 // for MCameraObserver(2)
#ifdef S60_CAM_AUTOFOCUS_SUPPORT
#include <ccamautofocus.h>        // for CCamAutoFocus, MCamAutoFocusObserver
#endif

// FORWARD DECLARATIONS
class MCameraEngineObserver;
class MCameraEngineImageCaptureObserver;
class MAdvancedSettingsObserver;
class MCameraViewfinderObserver;

/*
 * CameraEngine handling ECam operations needed.
 */
NONSHARABLE_CLASS( CCameraEngine ) : public CBase,
                                     public MCameraObserver,
                                     public MCameraObserver2
#ifdef S60_CAM_AUTOFOCUS_SUPPORT
                                    ,public MCamAutoFocusObserver
#endif

{
public: // Enums

    enum TCameraEngineState
    {
        EEngineNotReady = 0,    // 0 - No resources reserved
        EEngineInitializing,    // 1 - Reserving and Powering On
        EEngineIdle,            // 2 - Reseved and Powered On
        EEngineCapturing,       // 3 - Capturing Still Image
        EEngineFocusing         // 4 - Focusing
    };

public: // Constructor & Destructor

    static CCameraEngine* NewL( TInt aCameraHandle,
                                TInt aPriority,
                                MCameraEngineObserver* aObserver );
    ~CCameraEngine();

public:

    /**
     * External Advanced Settings callback observer.
     */
    void SetAdvancedObserver(MAdvancedSettingsObserver *aAdvancedSettingsObserver);

    /**
     * External Image Capture callback observer.
     */
    void SetImageCaptureObserver(MCameraEngineImageCaptureObserver *aImageCaptureObserver);

    /**
     * External Viewfinder callback observer.
     */
    void SetViewfinderObserver(MCameraViewfinderObserver *aViewfinderObserver);

    /**
     * Static function that returns the number of cameras on the device.
     */
    static TInt CamerasAvailable();

    /**
     * Returns the index of the currently active camera device
     */
    TInt currentCameraIndex() const { return iCameraIndex; }

    /**
     * Returns the current state (TCameraEngineState)
     * of the camera engine.
     */
    TCameraEngineState State() const { return iEngineState; }

    /**
     * Returns true if the camera has been reserved and
     * powered on, and not recording or capturing image
     */
    TBool IsCameraReady() const;

    /**
     * Returns whether DirectScreen ViewFinder is supported by the platform
     */
    TBool IsDirectViewFinderSupported() const;

    /**
     * Returns true if the camera supports AutoFocus.
     */
    TBool IsAutoFocusSupported() const;

    /**
     * Returns camera info
     */
    TCameraInfo *cameraInfo();

    /**
     * Captures an image. When complete, observer will receive
     * MceoCapturedDataReady() or MceoCapturedBitmapReady() callback,
     * depending on which image format was used in PrepareL().
     * @leave May leave with KErrNotReady if camera is not
     * reserved or prepared for capture.
     */
    void CaptureL();

    /**
     * Cancels ongoing image capture
     */
    void cancelCapture();

    /**
     * Reserves and powers on the camera. When complete,
     * observer will receive MceoCameraReady() callback
     *
     */
    void ReserveAndPowerOn();

    /**
     * Releases and powers off the camera
     *
     */
    void ReleaseAndPowerOff();

    /**
     * Prepares for image capture.
     * @param aCaptureSize requested capture size. On return,
     * contains the selected size (closest match)
     * @param aFormat Image format to use. Default is JPEG with
     * EXIF information as provided by the camera module
     * @leave KErrNotSupported, KErrNoMemory, KErrNotReady
     */
    void PrepareL( TSize& aCaptureSize,
        CCamera::TFormat aFormat = CCamera::EFormatExif );

    /**
     * Starts the viewfinder. Observer will receive
     * MceoViewFinderFrameReady() callbacks periodically.
     * @param aSize requested viewfinder size. On return,
     * contains the selected size.
     *
     * @leave KErrNotSupported is viewfinding with bitmaps is not
     * supported, KErrNotReady
     */
    void StartViewFinderL( TSize& aSize );

    /**
     * Stops the viewfinder if active.
     */
    void StopViewFinder();

    void StartDirectViewFinderL(RWsSession& aSession,
                                CWsScreenDevice& aScreenDevice,
                                RWindowBase& aWindow,
                                TRect& aSize);

    /**
     * Releases memory for the last received viewfinder frame.
     * Client must call this in response to MceoViewFinderFrameReady()
     * callback, after drawing the viewfinder frame is complete.
     */
    void ReleaseViewFinderBuffer();

    /**
     * Releases memory for the last captured image.
     * Client must call this in response to MceoCapturedDataReady()
     * or MceoCapturedBitmapReady()callback, after processing the
     * data/bitmap is complete.
     */
    void ReleaseImageBuffer();

    /**
     * Starts focusing. Does nothing if AutoFocus is not supported.
     * When complete, observer will receive MceoFocusComplete()
     * callback.
     * @leave KErrInUse, KErrNotReady
     */
    void StartFocusL();

    /**
     * Cancels the ongoing focusing operation.
     */
    void FocusCancel();

    /**
     * Gets a bitfield of supported focus ranges.
     * @param aSupportedRanges a bitfield of either TAutoFocusRange
     * (S60 3.0/3.1 devices) or TFocusRange (S60 3.2 and onwards) values
     */
    void SupportedFocusRanges( TInt& aSupportedRanges ) const;

    /**
     * Sets the focus range
     * @param aFocusRange one of the values returned by
     * SupportedFocusRanges().
     */
    void SetFocusRange( TInt aFocusRange );

    /**
     * Returns a pointer to CCamera object used by the engine.
     * Allows getting access to additional functionality
     * from CCamera - do not use for functionality already provided
     * by CCameraEngine methods.
     */
    CCamera* Camera() { return iCamera; }

protected:  // Protected constructors

    CCameraEngine();
    CCameraEngine( TInt aCameraHandle,
                   TInt aPriority,
                   MCameraEngineObserver* aObserver );
    void ConstructL();

protected: // MCameraObserver

    /**
     * From MCameraObserver
     * Gets called when CCamera::Reserve() is completed.
     * (V2: Called internally from HandleEvent)
     */
    virtual void ReserveComplete(TInt aError);

    /**
     * From MCameraObserver.
     * Gets called when CCamera::PowerOn() is completed.
     * (V2: Called internally from HandleEvent)
     */
    virtual void PowerOnComplete(TInt aError);

    /**
     * From MCameraObserver.
     * Gets called when CCamera::StartViewFinderBitmapsL() is completed.
     * (V2: Called internally from ViewFinderReady)
     */
    virtual void ViewFinderFrameReady( CFbsBitmap& aFrame );

    /**
     * From MCameraObserver.
     * Gets called when CCamera::CaptureImage() is completed.
     */
    virtual void ImageReady( CFbsBitmap* aBitmap, HBufC8* aData, TInt aError );

    /**
     * From MCameraObserver.
     * Video capture not implemented.
     */
    virtual void FrameBufferReady( MFrameBuffer* /*aFrameBuffer*/, TInt /*aError*/ ) {}

protected: // MCameraObserver2

    /**
     * From MCameraObserver2
     * Camera event handler
     */
    virtual void HandleEvent(const TECAMEvent &aEvent);

    /**
     * From MCameraObserver2
     * Notifies the client of new viewfinder data
     */
    virtual void ViewFinderReady(MCameraBuffer &aCameraBuffer, TInt aError);

    /**
     * From MCameraObserver2
     * Notifies the client of a new captured image
     */
    virtual void ImageBufferReady(MCameraBuffer &aCameraBuffer, TInt aError);

    /**
     * From MCameraObserver2
     * Video capture not implemented.
     */
    virtual void VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/, TInt /*aError*/) {}

protected: // MCamAutoFocusObserver

    /**
     * From MCamAutoFocusObserver.
     * Delivers notification of completion of auto focus initialisation to
     * an interested party.
     * @param aError Reason for completion of focus request.
     */
    virtual void InitComplete( TInt aError );

    /**
     * From MCamAutoFocusObserver.
     * Gets called when CCamAutoFocus::AttemptOptimisedFocusL() is
     * completed.
     * (V2: Called internally from HandleEvent)
     */
    virtual void OptimisedFocusComplete( TInt aError );

private:  // Internal functions

    /**
     * Internal function to handle ImageReady callbacks from
     * both observer (V1 & V2) interfaces
     */
    void HandleImageReady(const TInt aError, const bool isBitmap);

private:  // Data

    CCamera                             *iCamera;
    MCameraEngineObserver               *iObserver;
    MCameraEngineImageCaptureObserver   *iImageCaptureObserver;
    MAdvancedSettingsObserver           *iAdvancedSettingsObserver;
    MCameraViewfinderObserver           *iViewfinderObserver;
    MCameraBuffer                       *iViewFinderBuffer;
    /*
     * Following pointers are for the image buffers:
     *  * Makes buffering of 2 concurrent image buffers possible
     */
    MCameraBuffer                       *iImageBuffer1;
    MCameraBuffer                       *iImageBuffer2;
    TDesC8                              *iImageData1;
    TDesC8                              *iImageData2;
    CFbsBitmap                          *iImageBitmap1;
    CFbsBitmap                          *iImageBitmap2;
    TInt                                iCameraIndex;
    TInt                                iPriority;
    TCameraEngineState                  iEngineState;
    TCameraInfo                         iCameraInfo;
    CCamera::TFormat                    iImageCaptureFormat;
    bool                                iNew2LImplementation;
    int                                 iLatestImageBufferIndex; // 0 = Buffer1, 1 = Buffer2
#ifdef S60_CAM_AUTOFOCUS_SUPPORT
    CCamAutoFocus*                      iAutoFocus;
    CCamAutoFocus::TAutoFocusRange      iAFRange;
#endif // S60_CAM_AUTOFOCUS_SUPPORT
};

#endif // S60CCAMERAENGINE_H
