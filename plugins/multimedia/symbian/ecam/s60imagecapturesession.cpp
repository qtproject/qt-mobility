/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qstring.h>
#include <QtCore/qdir.h>

#include "s60imagecapturesession.h"
#include "s60videowidgetcontrol.h"
#include "s60cameraservice.h"
#include "s60filenamegenerator.h"
#include "s60cameraconstants.h"
#include "s60imagecapturesettings.h"

#include <fbs.h>                // CFbsBitmap
#include <pathinfo.h>
#include <imageconversion.h>    // ICL Decoder (for SnapShot) & Encoder (for Bitmap Images)

using namespace S60CameraConstants;

S60ImageCaptureDecoder::S60ImageCaptureDecoder(S60ImageCaptureSession *imageSession,
                                               RFs *fileSystemAccess,
                                               const TDesC8 *data,
                                               const TDesC16 *fileName) :
    CActive(CActive::EPriorityStandard),
    m_imageSession(imageSession),
    m_fs(fileSystemAccess),
    m_jpegImageData(data),
    m_jpegImageFile(fileName),
    m_fileInput(false)
{
    CActiveScheduler::Add(this);
}

S60ImageCaptureDecoder::~S60ImageCaptureDecoder()
{
    cancelDecoding();

    if (m_imageDecoder) {
        delete m_imageDecoder;
        m_imageDecoder = 0;
    }
}

S60ImageCaptureDecoder *S60ImageCaptureDecoder::FileNewL(S60ImageCaptureSession *imageSession,
                                                         RFs *fileSystemAccess,
                                                         const TDesC16 *fileName)
{
    S60ImageCaptureDecoder* self = new (ELeave) S60ImageCaptureDecoder(imageSession,
                                                                       fileSystemAccess,
                                                                       0,
                                                                       fileName);
    CleanupStack::PushL(self);
    self->ConstructL(true);
    CleanupStack::Pop(self);
    return self;
}

S60ImageCaptureDecoder *S60ImageCaptureDecoder::DataNewL(S60ImageCaptureSession *imageSession,
                                                         RFs *fileSystemAccess,
                                                         const TDesC8 *data)
{
    S60ImageCaptureDecoder* self = new (ELeave) S60ImageCaptureDecoder(imageSession,
                                                                       fileSystemAccess,
                                                                       data,
                                                                       0);
    CleanupStack::PushL(self);
    self->ConstructL(false);
    CleanupStack::Pop(self);
    return self;
}

void S60ImageCaptureDecoder::ConstructL(const bool fileInput)
{
    if (fileInput) {
        if (!m_imageSession || !m_fs || !m_jpegImageFile)
            User::Leave(KErrGeneral);
        m_imageDecoder = CImageDecoder::FileNewL(*m_fs, *m_jpegImageFile);
    } else {
        if (!m_imageSession || !m_fs || !m_jpegImageData)
            User::Leave(KErrGeneral);
        m_imageDecoder = CImageDecoder::DataNewL(*m_fs, *m_jpegImageData);
    }
}

void S60ImageCaptureDecoder::decode(CFbsBitmap *destBitmap)
{
    if (m_imageDecoder) {
        m_imageDecoder->Convert(&iStatus, *destBitmap, 0);
        SetActive();
    } else {
        m_imageSession->setError(KErrGeneral, QLatin1String("Preview image creation failed."));
    }
}

void S60ImageCaptureDecoder::cancelDecoding()
{
    Cancel();
    if (IsActive())
        User::WaitForRequest(iStatus);
}

TFrameInfo *S60ImageCaptureDecoder::frameInfo()
{
    if (m_imageDecoder) {
        m_frameInfo = m_imageDecoder->FrameInfo();
        return &m_frameInfo;
    } else {
        return 0;
    }
}

void S60ImageCaptureDecoder::RunL()
{
    m_imageSession->handleImageDecoded(iStatus.Int());
}

void S60ImageCaptureDecoder::DoCancel()
{
    if (m_imageDecoder)
        m_imageDecoder->Cancel();
}

TInt S60ImageCaptureDecoder::RunError(TInt aError)
{
    m_imageSession->setError(aError, QLatin1String("Preview image creation failed."));
    return KErrNone;
}

//=============================================================================

S60ImageCaptureEncoder::S60ImageCaptureEncoder(S60ImageCaptureSession *imageSession,
                                               RFs *fileSystemAccess,
                                               const TDesC16 *fileName,
                                               TInt jpegQuality) :
    CActive(CActive::EPriorityStandard),
    m_imageSession(imageSession),
    m_fileSystemAccess(fileSystemAccess),
    m_fileName(fileName),
    m_jpegQuality(jpegQuality)
{
    CActiveScheduler::Add(this);
}

S60ImageCaptureEncoder::~S60ImageCaptureEncoder()
{
    cancelEncoding();

    if (m_frameImageData) {
        delete m_frameImageData;
        m_frameImageData = 0;
    }
    if (m_imageEncoder) {
        delete m_imageEncoder;
        m_imageEncoder = 0;
    }
}

S60ImageCaptureEncoder *S60ImageCaptureEncoder::NewL(S60ImageCaptureSession *imageSession,
                                                     RFs *fileSystemAccess,
                                                     const TDesC16 *fileName,
                                                     TInt jpegQuality)
{
    S60ImageCaptureEncoder* self = new (ELeave) S60ImageCaptureEncoder(imageSession,
                                                                       fileSystemAccess,
                                                                       fileName,
                                                                       jpegQuality);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void S60ImageCaptureEncoder::ConstructL()
{
    if (!m_imageSession || !m_fileSystemAccess || !m_fileName)
        User::Leave(KErrGeneral);

    m_imageEncoder = CImageEncoder::FileNewL(*m_fileSystemAccess,
                                             *m_fileName,
                                             CImageEncoder::EOptionNone,
                                             KImageTypeJPGUid);
    CleanupStack::PushL(m_imageEncoder);

    // Set Jpeg Quality
    m_frameImageData = CFrameImageData::NewL();
    CleanupStack::PushL(m_frameImageData);

    TJpegImageData* jpegFormat = new( ELeave ) TJpegImageData;
    CleanupStack::PushL(jpegFormat);

    jpegFormat->iQualityFactor = m_jpegQuality;

    // jpegFormat (TJpegImageData) ownership transferred to m_frameImageData (CFrameImageData)
    User::LeaveIfError( m_frameImageData->AppendImageData(jpegFormat));

    CleanupStack::Pop(jpegFormat);
    CleanupStack::Pop(m_frameImageData);
    CleanupStack::Pop(m_imageEncoder);
}

void S60ImageCaptureEncoder::encode(CFbsBitmap *sourceBitmap)
{
    if (m_imageEncoder) {
        m_imageEncoder->Convert(&iStatus, *sourceBitmap, m_frameImageData);
        SetActive();
    } else {
        m_imageSession->setError(KErrGeneral, QLatin1String("Saving image to file failed."));
    }
}

void S60ImageCaptureEncoder::cancelEncoding()
{
    Cancel();
    if (IsActive())
        User::WaitForRequest(iStatus);
}

void S60ImageCaptureEncoder::RunL()
{
    m_imageSession->handleImageEncoded(iStatus.Int());
}

void S60ImageCaptureEncoder::DoCancel()
{
    if (m_imageEncoder)
        m_imageEncoder->Cancel();
}

TInt S60ImageCaptureEncoder::RunError(TInt aError)
{
    m_imageSession->setError(aError, QLatin1String("Saving image to file failed."));
    return KErrNone;
}

//=============================================================================

S60ImageCaptureSession::S60ImageCaptureSession(QObject *parent) :
    QObject(parent),
    m_cameraEngine(0),
    m_imageSettings(new S60ImageCaptureSettings(this, this)),
    m_previewBitmap(0),
    m_activeScheduler(0),
    m_fileSystemAccess(0),
    m_imageDecoder(0),
    m_imageEncoder(0),
    m_error(KErrNone),
    m_cameraStarted(false),
    m_icState(EImageCaptureNotPrepared),
    m_stillCaptureFileName(QString()),
    m_requestedStillCaptureFileName(QString()),
    m_currentImageId(0),
    m_captureWhenReady(false),
    m_previewDecodingOngoing(false),
    m_previewInWaitLoop(false),
    m_isCameraExternallyStarted(false)
{
    // Install ActiveScheduler if needed
    if (!CActiveScheduler::Current()) {
        m_activeScheduler = new CActiveScheduler;
        CActiveScheduler::Install(m_activeScheduler);
    }
}

S60ImageCaptureSession::~S60ImageCaptureSession()
{
    if (m_imageDecoder) {
        m_imageDecoder->Cancel();
        delete m_imageDecoder;
        m_imageDecoder = 0;
    }
    if (m_imageEncoder) {
        m_imageEncoder->Cancel();
        delete m_imageEncoder;
        m_imageEncoder = 0;
    }

    if (m_previewBitmap) {
        delete m_previewBitmap;
        m_previewBitmap = 0;
    }

    // Uninstall ActiveScheduler if needed
    if (m_activeScheduler) {
        CActiveScheduler::Install(0);
        delete m_activeScheduler;
        m_activeScheduler = 0;
    }
}

bool S60ImageCaptureSession::isDeviceReady()
{
#ifdef Q_CC_NOKIAX86 // Emulator
    return true;
#endif

    if (m_cameraEngine)
        return m_cameraEngine->IsCameraReady();
    return false;
}

bool S60ImageCaptureSession::isImageCapturePrepared() const
{
    return m_icState != EImageCaptureNotPrepared;
}

void S60ImageCaptureSession::setCameraHandle(CCameraEngine* camerahandle)
{
    if (camerahandle) {
        m_cameraEngine = camerahandle;
        resetSession();

        // Set default settings
        m_imageSettings->initializeImageCaptureSettings();
    }
}

void S60ImageCaptureSession::resetSession(bool errorHandling)
{
    m_captureWhenReady = false;
    m_previewDecodingOngoing = false;
    m_previewInWaitLoop = false;
    m_stillCaptureFileName = QString();
    m_requestedStillCaptureFileName = QString();
    m_icState = EImageCaptureNotPrepared;

    m_error = KErrNone;
    m_imageSettings->resetSymbianFormat();
    m_imageSettings->resetSettings(m_cameraEngine, errorHandling);
}

/*
 * This function can be used both internally and from Control classes using
 * this session. The error notification will go to the client application
 * either through QCameraImageCapture (if captureError is true) or QCamera (if
 * captureError is false, default) error signal.
 */
void S60ImageCaptureSession::setError(const TInt error,
                                      const QString &description,
                                      const bool captureError)
{
    if (error == KErrNone)
        return;

    m_error = error;
    QCameraImageCapture::Error cameraError = fromSymbianErrorToQtMultimediaError(error);

    if (captureError) {
        emit this->captureError(m_currentImageId, cameraError, description);
        if (cameraError != QCameraImageCapture::NotSupportedFeatureError &&
            cameraError != QCameraImageCapture::NotReadyError) {
            qWarning("Resetting image capture session to recover from error.");
            resetSession(true);
        }
    } else {
        emit this->cameraError(cameraError, description);
        if (cameraError != QCamera::NotSupportedFeatureError) {
            qWarning("Resetting image capture session to recover from camera error.");
            resetSession(true);
        }
    }
}

QCameraImageCapture::Error S60ImageCaptureSession::fromSymbianErrorToQtMultimediaError(int aError)
{
    switch(aError) {
    case KErrNone:
        return QCameraImageCapture::NoError;                    // No errors have occurred
    case KErrNotReady:
        return QCameraImageCapture::NotReadyError;              // Not ready for operation
    case KErrNotSupported:
        return QCameraImageCapture::NotSupportedFeatureError;   // The feature is not supported
    case KErrNoMemory:
        return QCameraImageCapture::OutOfSpaceError;            // Out of disk space
    case KErrNotFound:
    case KErrBadHandle:
        return QCameraImageCapture::ResourceError;              // No resources available
    default:
        return QCameraImageCapture::ResourceError;              // Other error has occurred
    }
}

int S60ImageCaptureSession::currentImageId() const
{
    return m_currentImageId;
}

int S60ImageCaptureSession::prepareImageCapture()
{
    if (m_cameraEngine) {
        if (!m_cameraEngine->IsCameraReady()) {
            // Reset state to make sure camera is prepared before capturing image
            m_icState = EImageCaptureNotPrepared;
            return KErrNotReady;
        }

        m_imageSettings->commitJpegQuality();
        QSize qtResolution = m_imageSettings->imageResolution();

        // Then prepare with correct resolution and format
        TSize captureSize = TSize(qtResolution.width(), qtResolution.height());
        TRAPD(symbianError, m_cameraEngine->PrepareL(captureSize, m_imageSettings->currentSymbianFormat()));
        if (!symbianError)
            m_icState = EImageCapturePrepared;

        // Check if CaptureSize was modified
        if (captureSize.iWidth != qtResolution.width() || captureSize.iHeight != qtResolution.height()) {
            qtResolution = QSize(captureSize.iWidth, captureSize.iHeight);
            m_imageSettings->setImageResolution(qtResolution);
        }
        emit captureSizeChanged(qtResolution);

#ifdef ECAM_PREVIEW_API
        // Subscribe previews
        MCameraPreviewObserver *observer = this;
        m_cameraEngine->EnablePreviewProvider(observer);
#endif // ECAM_PREVIEW_API

        return symbianError;
    }

    return KErrGeneral;
}

void S60ImageCaptureSession::releaseImageCapture()
{
    // Make sure ImageCapture is prepared the next time it is being activated
    m_icState = EImageCaptureNotPrepared;

#ifdef ECAM_PREVIEW_API
    // Cancel preview subscription
    m_cameraEngine->DisablePreviewProvider();
#endif // ECAM_PREVIEW_API
}

int S60ImageCaptureSession::capture(const QString &fileName)
{
    if (!m_cameraStarted) {
        if (m_isCameraExternallyStarted) {
            m_captureWhenReady = true;
            m_requestedStillCaptureFileName = fileName; // Save name, it will be processed during actual capture
            return m_currentImageId + 1;
        } else {
            setError(KErrNotReady, tr("Unable to capture image before camera is started."), true);
            return 0;
        }
    }

    if (m_icState < EImageCapturePrepared) {
        int prepareSuccess = prepareImageCapture();
        if (prepareSuccess) {
            setError(prepareSuccess, tr("Failure during image capture preparation."), true);
            return 0;
        }
    } else if (m_icState > EImageCapturePrepared) {
        setError(KErrNotReady, tr("Previous operation is still ongoing."), true);
        return 0;
    }

    m_icState = EImageCaptureCapturing;

    // Give new ID for the new image
    m_currentImageId += 1;

    emit readyForCaptureChanged(false);

    if (m_imageSettings->destination() & QCameraImageCapture::CaptureToFile)
        processFileName(fileName);

    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->CaptureL());
        setError(err, tr("Image capture failed."), true);
    } else {
        setError(KErrNotReady, tr("Unexpected camera error."), true);
    }

#ifdef Q_CC_NOKIAX86 // Emulator
    QImage *snapImage = new QImage(QLatin1String("C:/Data/testimage.jpg"));
    emit imageExposed(m_currentImageId);
    emit imageCaptured(m_currentImageId, *snapImage);
    QString qtFileName(m_stillCaptureFileName);
    qtFileName.replace(QChar('\\'), QChar('/'));
    emit imageSaved(m_currentImageId, qtFileName);
#endif // Q_CC_NOKIAX86

    return m_currentImageId;
}

void S60ImageCaptureSession::cancelCapture()
{
    if (m_icState != EImageCaptureCapturing)
        return;

    if (m_cameraEngine)
        m_cameraEngine->CancelCapture();

    m_icState = EImageCapturePrepared;
    emit readyForCaptureChanged(true);
}

void S60ImageCaptureSession::processFileName(const QString &fileName)
{
    // Empty FileName - Use default file name
    if (fileName.isEmpty())
        m_stillCaptureFileName = S60FileNameGenerator::defaultFileName(S60FileNameGenerator::ImageFileName,
                                                                       QLatin1String(".jpg"));
    else // Not empty
        m_stillCaptureFileName = S60FileNameGenerator::generateFileNameFromString(S60FileNameGenerator::ImageFileName,
                                                                                  fileName,
                                                                                  QLatin1String(".jpg"));
}

void S60ImageCaptureSession::MceoFocusComplete()
{
    m_imageSettings->notifyFocusComplete();
}

TSize S60ImageCaptureSession::getScaledPreviewSize(const QSize originalSize) const
{
    // Set proper Preview Size
    TSize scaledSize((originalSize.width() / KSnapshotDownScaleFactor),
                     (originalSize.height() / KSnapshotDownScaleFactor));

    if (scaledSize.iWidth < KSnapshotMinWidth || scaledSize.iHeight < KSnapshotMinHeight)
        scaledSize.SetSize((originalSize.width() / (KSnapshotDownScaleFactor/2)),
                           (originalSize.height() / (KSnapshotDownScaleFactor/2)));
    if (scaledSize.iWidth < KSnapshotMinWidth || scaledSize.iHeight < KSnapshotMinHeight)
        scaledSize.SetSize((originalSize.width() / (KSnapshotDownScaleFactor/4)),
                           (originalSize.height() / (KSnapshotDownScaleFactor/4)));
    if (scaledSize.iWidth < KSnapshotMinWidth || scaledSize.iHeight < KSnapshotMinHeight)
        scaledSize.SetSize(originalSize.width(),
                           originalSize.height());

    return scaledSize;
}

void S60ImageCaptureSession::MceoCapturedDataReady(TDesC8* aData)
{
    if (!aData) {
        releaseImageBuffer();
        setError(KErrGeneral, tr("Captured image is empty."));
        return;
    }

    emit imageExposed(m_currentImageId);

    m_icState = EImageCaptureWritingImage;

    // Send image buffer
    if (m_imageSettings->destination() & QCameraImageCapture::CaptureToBuffer) {
        emit imageAvailable(m_currentImageId, generateImageBuffer(aData));

        if (m_imageSettings->destination() == QCameraImageCapture::CaptureToBuffer) {
#ifdef ECAM_PREVIEW_API
            releaseImageBuffer();
            TInt err = KErrNone;
            QT_TRYCATCH_ERROR(err, emit readyForCaptureChanged(true));
            setError(err, tr("Failure while notifying client that camera is ready for capture."));
#else // ECAM_PREVIEW_API
            // Preview needs to be decoded from the compressed image
            if (m_previewDecodingOngoing) {
                m_previewInWaitLoop = true;
                CActiveScheduler::Start(); // Wait for the completion of the previous Preview generation
            }
            if (m_fileSystemAccess) {
                m_fileSystemAccess->Close();
                delete m_fileSystemAccess;
                m_fileSystemAccess = 0;
            }
            // Delete old instances if needed
            if (m_imageDecoder) {
                delete m_imageDecoder;
                m_imageDecoder = 0;
            }
            if (m_previewBitmap) {
                delete m_previewBitmap;
                m_previewBitmap = 0;
            }
            TRAPD(err, m_fileSystemAccess = new (ELeave) RFs);
            if (err) {
                setError(err, tr("Failed to create preview image."));
                return;
            }
            err = m_fileSystemAccess->Connect();
            if (err) {
                setError(err, tr("Failed to create preview image."));
                return;
            }

            // Generate Thumbnail to be used as Preview
            TRAP(err, m_imageDecoder = S60ImageCaptureDecoder::DataNewL(this, m_fileSystemAccess, aData));
            if (err) {
                setError(err, tr("Failed to create preview image."));
                return;
            }

            TSize scaledSize = getScaledPreviewSize(m_imageSettings->imageResolution());
            TFrameInfo *info = m_imageDecoder->frameInfo();
            if (!info) {
                setError(KErrGeneral, tr("Preview image creation failed."));
                return;
            }

            TRAP(err, m_previewBitmap = new (ELeave) CFbsBitmap);
            if (err) {
                setError(err, tr("Failed to create preview image."));
                return;
            }
            err = m_previewBitmap->Create(scaledSize, info->iFrameDisplayMode);
            if (err) {
                setError(err, tr("Failed to create preview image."));
                return;
            }

            // Jpeg conversion completes in RunL
            m_previewDecodingOngoing = true;
            m_imageDecoder->decode(m_previewBitmap);
#endif // ECAM_PREVIEW_API
        }
    }

    if (m_imageSettings->destination() & QCameraImageCapture::CaptureToFile) {
        TFileName path = convertImagePath();

        // Try to save image and inform if it was succcesful
        TRAPD(err, saveImageL(aData, path));
        if (err) {
            if (m_previewDecodingOngoing)
                m_previewDecodingOngoing = false; // Reset

            setError(err, tr("Writing captured image to a file failed."), true);
            m_icState = EImageCapturePrepared;
            return;
        }
    }

    m_icState = EImageCapturePrepared;
}

void S60ImageCaptureSession::MceoCapturedBitmapReady(CFbsBitmap* aBitmap)
{
    if (!aBitmap) {
        releaseImageBuffer();
        setError(KErrGeneral, tr("Captured image is empty."));
        return;
    }

    emit imageExposed(m_currentImageId);

    m_icState = EImageCaptureWritingImage;

    if (m_imageSettings->destination() & QCameraImageCapture::CaptureToFile) {
        if(aBitmap)
        {
#ifndef ECAM_PREVIEW_API
            if (m_previewDecodingOngoing) {
                m_previewInWaitLoop = true;
                CActiveScheduler::Start(); // Wait for the completion of the previous Preview generation
            }

            // Delete old instances if needed
            if (m_imageDecoder) {
                delete m_imageDecoder;
                m_imageDecoder = 0;
            }
            if (m_previewBitmap) {
                delete m_previewBitmap;
                m_previewBitmap = 0;
            }
#endif // ECAM_CAMERA_API
            if (m_imageEncoder) {
                delete m_imageEncoder;
                m_imageEncoder = 0;
            }
            if (m_fileSystemAccess) {
                m_fileSystemAccess->Close();
                delete m_fileSystemAccess;
                m_fileSystemAccess = 0;
            }

            TInt saveError = KErrNone;
            TFileName path = convertImagePath();

            // Create FileSystem access
            m_fileSystemAccess = new RFs;
            if (!m_fileSystemAccess) {
                setError(KErrNoMemory, tr("Failed to write captured image to a file."));
                return;
            }
            saveError = m_fileSystemAccess->Connect();
            if (saveError) {
                setError(saveError, tr("Failed to write captured image to a file."));
                return;
            }

            TRAP(saveError, m_imageEncoder = S60ImageCaptureEncoder::NewL(this,
                                                                          m_fileSystemAccess,
                                                                          &path,
                                                                          m_imageSettings->jpegQuality()));
            if (saveError)
                setError(saveError, tr("Saving captured image failed."), true);
            m_previewDecodingOngoing = true;
            m_imageEncoder->encode(aBitmap);

        } else {
            setError(KErrBadHandle, tr("Unexpected camera error."), true);
        }
    }

    m_icState = EImageCapturePrepared;
}

void S60ImageCaptureSession::MceoHandleError(TCameraEngineError aErrorType, TInt aError)
{
    Q_UNUSED(aErrorType);
    setError(aError, tr("General camera error."));
}

QVideoFrame S60ImageCaptureSession::generateImageBuffer(TDesC8 *aData)
{
    int bytes = int(aData->Length());
    QSize resolution = m_imageSettings->imageResolution();
    int bytesPerLine = 0;
    QVideoFrame::PixelFormat format = QVideoFrame::Format_Jpeg;
    QVideoFrame newVideoFrame(bytes, resolution, bytesPerLine, format);

    // Deep copy image data into the newly created QVideoFrame image container
    newVideoFrame.map(QAbstractVideoBuffer::WriteOnly);
    uchar *destData = newVideoFrame.bits();
    memcpy(destData, aData->Ptr(), bytes);
    newVideoFrame.unmap();

    return newVideoFrame;
}

TFileName S60ImageCaptureSession::convertImagePath()
{
    TFileName path = KNullDesC();

    // Convert to Symbian path
    TPtrC16 attachmentPath(KNullDesC);

    // Path is already included in filename and native separators are being used
    attachmentPath.Set(reinterpret_cast<const TUint16*>(m_stillCaptureFileName.utf16()));
    path.Append(attachmentPath);

    return path;
}

/*
 * Creates (asynchronously) Preview Image from Jpeg ImageBuffer and also
 * writes Jpeg (synchronously) to a file.
 */
void S60ImageCaptureSession::saveImageL(TDesC8 *aData, TFileName &aPath)
{
    // Checked if aData is NULL already in the MceoCapturedDataReady()

    if (aPath.Size() > 0) {
#ifndef ECAM_PREVIEW_API
        if (m_previewDecodingOngoing) {
            m_previewInWaitLoop = true;
            CActiveScheduler::Start(); // Wait for the completion of the previous Preview generation
        }

        // Delete old instances if needed
        if (m_imageDecoder) {
            delete m_imageDecoder;
            m_imageDecoder = 0;
        }
        if (m_previewBitmap) {
            delete m_previewBitmap;
            m_previewBitmap = 0;
        }
#endif // ECAM_PREVIEW_API
        if (m_fileSystemAccess) {
            m_fileSystemAccess->Close();
            delete m_fileSystemAccess;
            m_fileSystemAccess = 0;
        }

        RFs *fileSystemAccess = new (ELeave) RFs;
        User::LeaveIfError(fileSystemAccess->Connect());
        CleanupClosePushL(*fileSystemAccess);

#ifndef ECAM_PREVIEW_API
        // Generate Thumbnail to be used as Preview
        S60ImageCaptureDecoder *imageDecoder = S60ImageCaptureDecoder::DataNewL(this, fileSystemAccess, aData);
        CleanupStack::PushL(imageDecoder);

        TSize scaledSize = getScaledPreviewSize(m_imageSettings->imageResolution());
        TFrameInfo *info = imageDecoder->frameInfo();
        if (!info) {
            setError(KErrGeneral, tr("Preview image creation failed."));
            return;
        }

        CFbsBitmap *previewBitmap = new (ELeave) CFbsBitmap;
        CleanupStack::PushL(previewBitmap);
        TInt bitmapCreationErr = previewBitmap->Create(scaledSize, info->iFrameDisplayMode);
        if (bitmapCreationErr) {
            setError(bitmapCreationErr, tr("Preview creation failed."));
            return;
        }

        // Jpeg conversion completes in RunL
        m_previewDecodingOngoing = true;
        imageDecoder->decode(previewBitmap);
#endif // ECAM_PREVIEW_API

        RFile file;
        TInt fileWriteErr = KErrNone;
        fileWriteErr = file.Replace(*fileSystemAccess, aPath, EFileWrite);
        if (fileWriteErr) {
#ifndef ECAM_PREVIEW_API
            imageDecoder->cancelDecoding();
#endif // ECAM_PREVIEW_API
            User::Leave(fileWriteErr);
        }
        CleanupClosePushL(file);

        fileWriteErr = file.Write(*aData);
        if (fileWriteErr) {
#ifndef ECAM_PREVIEW_API
            imageDecoder->cancelDecoding();
#endif // ECAM_PREVIEW_API
            User::Leave(fileWriteErr);
        }
        file.Close();
        CleanupStack::PopAndDestroy(&file);

#ifdef ECAM_PREVIEW_API
        CleanupStack::PopAndDestroy(fileSystemAccess);
#else // !ECAM_PREVIEW_API
        // Delete when Image is decoded
        CleanupStack::Pop(previewBitmap);
        CleanupStack::Pop(imageDecoder);
        CleanupStack::Pop(fileSystemAccess);

        // Set member variables (Cannot leave any more)
        m_previewBitmap = previewBitmap;
        m_imageDecoder = imageDecoder;
        m_fileSystemAccess = fileSystemAccess;
#endif // ECAM_PREVIEW_API

        QString qtFileName(m_stillCaptureFileName);
        qtFileName.replace(QChar('\\'), QChar('/'));
        emit imageSaved(m_currentImageId, qtFileName);

        // For custom preview generation, image buffer gets released in RunL()
#ifdef ECAM_PREVIEW_API
        releaseImageBuffer();
#endif // ECAM_PREVIEW_API

        // Inform that we can continue taking more pictures
        emit readyForCaptureChanged(true);

    } else {
        setError(KErrPathNotFound, tr("Invalid path given."), true);
    }
}

void S60ImageCaptureSession::releaseImageBuffer()
{
    if (m_cameraEngine)
        m_cameraEngine->ReleaseImageBuffer();
    else
        setError(KErrNotReady, tr("Unexpected camera error."), true);
}

S60ImageCaptureSettings *S60ImageCaptureSession::settings() const
{
    return m_imageSettings;
}

/*
 * This function handles different camera status changes
 */
void S60ImageCaptureSession::cameraStatusChanged(QCamera::Status status)
{
    if (status == QCamera::ActiveStatus) {
        m_cameraStarted = true;
        if (m_captureWhenReady) {
            capture(m_requestedStillCaptureFileName);
            m_captureWhenReady = false;
        }
    } else if (status == QCamera::UnloadedStatus || status == QCamera::StandbyStatus) {
        m_cameraStarted = false;
        m_icState = EImageCaptureNotPrepared;
    } else if (status == QCamera::LoadedStatus) {
        // Loading -> Loaded
        //    ==> Camera is not started (m_cameraStarted = false)
        // Starting -> Loaded
        //    ==> Camera is started (m_cameraStarted = true)
        if (!m_cameraStarted)
            m_imageSettings->applyCameraSettings();
        m_cameraStarted = false;
    } else if (status != QCamera::StartingStatus) {
        // Camera is not started
        m_cameraStarted = false;
    }
}

void S60ImageCaptureSession::cameraStateChanged(QCamera::State state)
{
    if (state == QCamera::ActiveState)
        m_isCameraExternallyStarted = true;
    else
        m_isCameraExternallyStarted = false;
}

void S60ImageCaptureSession::handleImageDecoded(int error)
{
    // Delete unneeded objects
    if (m_imageDecoder) {
        delete m_imageDecoder;
        m_imageDecoder = 0;
    }
    if (m_fileSystemAccess) {
        m_fileSystemAccess->Close();
        delete m_fileSystemAccess;
        m_fileSystemAccess = 0;
    }

    // Check status of decoding
    if (error != KErrNone) {
        if (m_previewBitmap) {
            m_previewBitmap->Reset();
            delete m_previewBitmap;
            m_previewBitmap = 0;
        }
        releaseImageBuffer();
        if (m_previewInWaitLoop) {
            CActiveScheduler::Stop(); // Notify to continue execution of next Preview Image generation
            m_previewInWaitLoop = false; // Reset
        }
        setError(error, tr("Preview creation failed."));
        return;
    }

    m_previewDecodingOngoing = false;

    QPixmap prevPixmap = QPixmap::fromSymbianCFbsBitmap(m_previewBitmap);
    QImage preview = prevPixmap.toImage(); // Deep copy

    if (m_previewBitmap) {
        m_previewBitmap->Reset();
        delete m_previewBitmap;
        m_previewBitmap = 0;
    }

    QT_TRYCATCH_LEAVING( emit imageCaptured(m_currentImageId, preview) );

    // Release image resources (if not already done)
    releaseImageBuffer();

    if (m_previewInWaitLoop) {
        CActiveScheduler::Stop(); // Notify to continue execution of next Preview Image generation
        m_previewInWaitLoop = false; // Reset
    }
}

void S60ImageCaptureSession::handleImageEncoded(int error)
{
    // Check status of encoding
    if (error != KErrNone) {
        releaseImageBuffer();
        if (m_previewInWaitLoop) {
            CActiveScheduler::Stop(); // Notify to continue execution of next Preview Image generation
            m_previewInWaitLoop = false; // Reset
        }
        setError(error, tr("Saving captured image to file failed."));
        return;
    } else {
        QString qtFileName(m_stillCaptureFileName);
        qtFileName.replace(QChar('\\'), QChar('/'));
        QT_TRYCATCH_LEAVING( emit imageSaved(m_currentImageId, qtFileName) );
    }

    if (m_imageEncoder) {
        delete m_imageEncoder;
        m_imageEncoder = 0;
    }

#ifndef ECAM_PREVIEW_API
    // Start preview generation
    TInt previewError = KErrNone;
    TFileName fileName = convertImagePath();
    TRAP(previewError, m_imageDecoder = S60ImageCaptureDecoder::FileNewL(this, m_fileSystemAccess, &fileName));
    if (previewError) {
        setError(previewError, tr("Failed to create preview image."));
        return;
    }

    TSize scaledSize = getScaledPreviewSize(m_imageSettings->imageResolution());
    TFrameInfo *info = m_imageDecoder->frameInfo();
    if (!info) {
        setError(KErrGeneral, tr("Preview image creation failed."));
        return;
    }

    m_previewBitmap = new CFbsBitmap;
    if (!m_previewBitmap) {
        setError(KErrNoMemory, tr("Failed to create preview image."));
        return;
    }
    previewError = m_previewBitmap->Create(scaledSize, info->iFrameDisplayMode);
    if (previewError) {
        setError(previewError, tr("Preview creation failed."));
        return;
    }

    // Jpeg decoding completes in handleImageDecoded()
    m_imageDecoder->decode(m_previewBitmap);
#endif // ECAM_PREVIEW_API

    // Buffer can be released since Preview is created from file
    releaseImageBuffer();

    // Inform that we can continue taking more pictures
    QT_TRYCATCH_LEAVING( emit readyForCaptureChanged(true) );
}

#ifdef ECAM_PREVIEW_API
void S60ImageCaptureSession::MceoPreviewReady(CFbsBitmap& aPreview)
{
    QPixmap previewPixmap = QPixmap::fromSymbianCFbsBitmap(&aPreview);
    QImage preview = previewPixmap.toImage(); // Deep copy

    // Notify preview availability
    emit imageCaptured(m_currentImageId, preview);
}
#endif // ECAM_PREVIEW_API

// End of file

