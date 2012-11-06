/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60IMAGECAPTURESESSION_H
#define S60IMAGECAPTURESESSION_H

#include <QtCore/qurl.h>
#include <QtCore/qlist.h>
#include <QtCore/qmap.h>
#include <QtGui/qicon.h>

#include <qcamera.h>
#include <qcamerafocus.h>
#include <qcameraimagecapture.h>
#include <qvideoframe.h>

#include "s60cameraengine.h"
#include "s60cameraengineobserver.h"
#include "s60cameraconstants.h"

#include <icl/imagedata.h> // TFrameInfo

QT_USE_NAMESPACE
using namespace S60CameraConstants;

QT_FORWARD_DECLARE_CLASS(S60CameraService)
QT_FORWARD_DECLARE_CLASS(S60ImageCaptureSettings)
QT_FORWARD_DECLARE_CLASS(CImageDecoder)
QT_FORWARD_DECLARE_CLASS(CImageEncoder)
QT_FORWARD_DECLARE_CLASS(CFrameImageData)
QT_FORWARD_DECLARE_CLASS(RFs)
QT_FORWARD_DECLARE_CLASS(S60ImageCaptureSession)

#ifndef ECAM_PREVIEW_API
/*
 * This class implements asynchronous image decoding service for the
 * S60ImageCaptureSession.
 */
class S60ImageCaptureDecoder : public CActive
{
public: // Static Contructor & Destructor

    static S60ImageCaptureDecoder* FileNewL(S60ImageCaptureSession *imageSession = 0,
                                            RFs *fileSystemAccess = 0,
                                            const TDesC16 *fileName = 0);
    static S60ImageCaptureDecoder* DataNewL(S60ImageCaptureSession *imageSession = 0,
                                            RFs *fileSystemAccess = 0,
                                            const TDesC8 *data = 0);
    ~S60ImageCaptureDecoder();

public: // Operations

    void decode(CFbsBitmap *destBitmap);
    void cancelDecoding();
    TFrameInfo *frameInfo();

protected: // CActive

    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

protected: // Protected constructors

    S60ImageCaptureDecoder(S60ImageCaptureSession *imageSession,
                           RFs *fileSystemAccess,
                           const TDesC8 *data,
                           const TDesC16 *fileName);
    void ConstructL(const bool fileInput = false);

private: // Data

    S60ImageCaptureSession  *m_imageSession;
    CImageDecoder           *m_imageDecoder;
    RFs                     *m_fs;
    const TDesC8            *m_jpegImageData;
    const TDesC16           *m_jpegImageFile;
    bool                    m_fileInput;
    TFrameInfo              m_frameInfo;

};
#endif // ECAM_PREVIEW_API

//=============================================================================

/*
 * This class implements asynchronous image encoding service for the
 * S60ImageCaptureSession.
 */
class S60ImageCaptureEncoder : public CActive
{
public: // Static Contructor & Destructor

    static S60ImageCaptureEncoder* NewL(S60ImageCaptureSession *imageSession = 0,
                                        RFs *fileSystemAccess = 0,
                                        const TDesC16 *fileName = 0,
                                        TInt jpegQuality = KJpegQualityDefault);
    ~S60ImageCaptureEncoder();

public: // Operations

    void encode(CFbsBitmap *sourceBitmap);
    void cancelEncoding();

protected: // CActive

    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

protected: // Protected constructors

    S60ImageCaptureEncoder(S60ImageCaptureSession *imageSession,
                           RFs *fileSystemAccess,
                           const TDesC16 *fileName,
                           TInt jpegQuality);
    void ConstructL();

private: // Data

    S60ImageCaptureSession  *m_imageSession;
    CImageEncoder           *m_imageEncoder;
    RFs                     *m_fileSystemAccess;
    const TDesC16           *m_fileName;
    CFrameImageData         *m_frameImageData;
    TInt                    m_jpegQuality;

};

//=============================================================================

using namespace S60CameraEngineError;

/*
 * Session handling all image capture activities.
 */
class S60ImageCaptureSession : public QObject,
#ifdef ECAM_PREVIEW_API
                               public MCameraPreviewObserver,
#endif // ECAM_PREVIEW_API
                               public MCameraEngineImageCaptureObserver
{
    Q_OBJECT

public: // Enums

    enum ImageCaptureState {
        EImageCaptureNotPrepared = 0,   // 0 - ImageCapture has not been prepared
        EImageCapturePrepared,          // 1 - ImageCapture has been prepared
        EImageCaptureCapturing,         // 2 - Image capture ongoing
        EImageCaptureWritingImage       // 3 - Image captured and image writing to file ongoing
    };

public: // Constructor & Destructor

    S60ImageCaptureSession(QObject *parent = 0);
    ~S60ImageCaptureSession();

public: // Methods

    void setError(const TInt error, const QString &description, const bool captureError = false);
    int currentImageId() const;

    bool isDeviceReady();
    bool isImageCapturePrepared() const;
    void setCameraHandle(CCameraEngine* camerahandle);

    // Controls
    int prepareImageCapture();
    void releaseImageCapture();
    int capture(const QString &fileName);
    void cancelCapture();
    void releaseImageBuffer();

    // Settings class
    S60ImageCaptureSettings *settings() const;

public: // Image Decoding & Encoding Notifications

#ifndef ECAM_PREVIEW_API
    void handleImageDecoded(int error);
#endif // ECAM_PREVIEW_API
    void handleImageEncoded(int error);

protected: // MCameraEngineObserver

    void MceoFocusComplete();
    void MceoCapturedDataReady(TDesC8* aData);
    void MceoCapturedBitmapReady(CFbsBitmap* aBitmap);
    void MceoHandleError(TCameraEngineError aErrorType, TInt aError);

#ifdef ECAM_PREVIEW_API
protected: // MCameraPreviewObserver

    void MceoPreviewReady(CFbsBitmap& aPreview);
#endif // ECAM_PREVIEW_API

private: // Internal

    QCameraImageCapture::Error fromSymbianErrorToQtMultimediaError(int aError);
    void resetSession(bool errorHandling = false);

    void saveImageL(TDesC8 *aData, TFileName &aPath);
    void processFileName(const QString &fileName);
    TFileName convertImagePath();

    QVideoFrame generateImageBuffer(TDesC8 *aData);

    TSize getScaledPreviewSize(const QSize originalSize) const;

signals: // Notifications

    void stateChanged(QCamera::State);
    // This is used for updating the output aspect ratio
    void captureSizeChanged(const QSize&);

    // Error signals
    void cameraError(int, const QString&);          // For QCamera::error
    void captureError(int, int, const QString&);    // For QCameraImageCapture::error

    // Capture notifications
    void readyForCaptureChanged(bool);
    void imageExposed(int);
    void imageCaptured(const int, const QImage&);
    void imageAvailable(const int, const QVideoFrame&);
    void imageSaved(const int, const QString&);

private slots: // Internal Slots

    void cameraStatusChanged(QCamera::Status);
    void cameraStateChanged(QCamera::State);

private: // Data

    CCameraEngine           *m_cameraEngine;
    S60ImageCaptureSettings *m_imageSettings;
    CFbsBitmap              *m_previewBitmap;
    CActiveScheduler        *m_activeScheduler;
    RFs                     *m_fileSystemAccess;
#ifndef ECAM_PREVIEW_API
    S60ImageCaptureDecoder  *m_imageDecoder;
#endif // ECAM_PREVIEW_API
    S60ImageCaptureEncoder  *m_imageEncoder;
    mutable int             m_error; // Symbian ErrorCode
    bool                    m_cameraStarted;
    ImageCaptureState       m_icState;
    QString                 m_stillCaptureFileName;
    QString                 m_requestedStillCaptureFileName;
    mutable int             m_currentImageId;
    // This indicates that image capture should be triggered right after
    // camera and image setting initialization has completed
    bool                    m_captureWhenReady;
    bool                    m_previewDecodingOngoing;
    bool                    m_previewInWaitLoop;
    bool                    m_isCameraExternallyStarted;
};

#endif // S60IMAGECAPTURESESSION_H
