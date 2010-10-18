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

#include <QtCore/qstring.h>
#include <QtCore/qdir.h>

#include "s60imagecapturesession.h"
#include "s60videowidgetcontrol.h"
#include "s60cameraservice.h"
#include "s60cameraconstants.h"

#include <fbs.h>        // CFbsBitmap
#include <pathinfo.h>

S60ImageCaptureSession::S60ImageCaptureSession(QObject *parent) :
    QObject(parent),
    m_cameraEngine(NULL),
    m_advancedSettings(NULL),
    m_cameraInfo(NULL),
    m_error(KErrNone),
    m_activeDeviceIndex(KDefaultCameraDevice),
    m_cameraStarted(false),
    m_icState(EImageCaptureNotPrepared),
    m_currentCodec(QString()),
    m_captureSize(QSize()),
    m_symbianImageQuality(QtMultimediaKit::VeryHighQuality * KSymbianImageQualityCoefficient),
    m_stillCaptureFileName(QString()),
    m_currentImageId(0),
    m_captureWhenReady(false)
{
    // Define supported image codecs
	m_supportedImageCodecs << "image/jpg";
}

S60ImageCaptureSession::~S60ImageCaptureSession()
{
    // Delete AdvancedSettings
    deleteAdvancedSettings();

    m_formats.clear();
    m_supportedImageCodecs.clear();
}

CCamera::TFormat S60ImageCaptureSession::defaultImageFormat()
{
    // Primary Camera
    if (m_activeDeviceIndex == 0)
        return KDefaultImageFormatPrimaryCam;

    // Secondary Camera or other
    else
        return KDefaultImageFormatSecondaryCam;
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

void S60ImageCaptureSession::deleteAdvancedSettings()
{
    if (m_advancedSettings) {
        delete m_advancedSettings;
        m_advancedSettings = NULL;
    }
}

void S60ImageCaptureSession::setCameraHandle(CCameraEngine* camerahandle)
{
    if (camerahandle) {
        m_cameraEngine = camerahandle;
        resetSession();

        // Set default settings
        initializeImageCaptureSettings();
    }
}

void S60ImageCaptureSession::setCurrentDevice(TInt deviceindex)
{
    m_activeDeviceIndex = deviceindex;
}

void S60ImageCaptureSession::resetSession()
{
    // Delete old AdvancedSettings
    deleteAdvancedSettings();

    m_error = KErrNone;
    m_currentFormat = defaultImageFormat();
    TRAPD(err, m_advancedSettings = S60CameraSettings::NewL(this, m_cameraEngine));
    if (err == KErrNotSupported) {
        // Adv. settings may not be supported for other than the Primary Camera
        if (m_cameraEngine->currentCameraIndex() == 0)
            setError(err, QString("Unexpected camera error."));
    }
    else
        setError(err, QString("Unexpected camera error."));

    if (m_advancedSettings) {
        if (m_cameraEngine)
            m_cameraEngine->SetAdvancedObserver(m_advancedSettings);
        else
            setError(KErrNotReady, QString("Unexpected camera error."));
    }

    updateImageCaptureFormats();

    if (m_advancedSettings)
        emit advancedSettingCreated();
}

S60CameraSettings* S60ImageCaptureSession::advancedSettings()
{
    return m_advancedSettings;
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
        if (cameraError != QCameraImageCapture::NotSupportedFeatureError)
            resetSession();
    } else {
        emit this->cameraError(cameraError, description);
        if (cameraError != QCamera::NotSupportedFeatureError)
            resetSession();
    }
}

QCameraImageCapture::Error S60ImageCaptureSession::fromSymbianErrorToQtMultimediaError(int aError)
{
    switch(aError) {
        case KErrNone:
            return QCameraImageCapture::NoError; // No errors have occurred
        case KErrNotReady:
            return QCameraImageCapture::NotReadyError; // Not ready for operation
        case KErrNotSupported:
            return QCameraImageCapture::NotSupportedFeatureError; // The feature is not supported
        case KErrNoMemory:
            return QCameraImageCapture::OutOfSpaceError; // Out of disk space
        case KErrNotFound:
        case KErrBadHandle:
            return QCameraImageCapture::ResourceError; // No resources available

        default:
            return QCameraImageCapture::ResourceError; // Other error has occured
    }
}

int S60ImageCaptureSession::currentImageId() const
{
    return m_currentImageId;
}

void S60ImageCaptureSession::initializeImageCaptureSettings()
{
    m_currentCodec = KDefaultImageCodec;
    m_currentFormat = defaultImageFormat();

    QList<QSize> resolutions = supportedCaptureSizesForCodec(imageCaptureCodec());
    if (resolutions.size() > 0)
        m_captureSize = resolutions[0]; // First is the maximum

    m_symbianImageQuality = 100; // Best quality
}

/*
 * This function selects proper format to be used for the captured image based
 * on the requested image codec.
 */
CCamera::TFormat S60ImageCaptureSession::selectFormatForCodec(const QString &codec)
{
    CCamera::TFormat format = CCamera::EFormatMonochrome;

    if (codec == "image/jpg") {
        // Primary Camera
        if (m_activeDeviceIndex == 0)
            format = KDefaultImageFormatPrimaryCam;

        // Secondary Camera or other
        else
            format = KDefaultImageFormatSecondaryCam;

        return format;
    }

    setError(KErrNotSupported, QString("Failed to select color format to be used with image codec."));
    return format;
}

int S60ImageCaptureSession::prepareImageCapture()
{
    if (m_cameraEngine) {
        if (!m_cameraEngine->IsCameraReady()) {
            // Reset state to make sure camera is prepared before capturing image
            m_icState = EImageCaptureNotPrepared;
            return KErrNotReady;
        }

        // First set the quality
        CCamera *camera = m_cameraEngine->Camera();
        if (camera)
            camera->SetJpegQuality(m_symbianImageQuality);
        else
            setError(KErrNotReady, QString("Setting image quality failed."), true);

        // Then prepare with correct resolution and format
        TSize captureSize = TSize(m_captureSize.width(), m_captureSize.height());
        TRAPD(symbianError, m_cameraEngine->PrepareL(captureSize, m_currentFormat));
        if (!symbianError)
            m_icState = EImageCapturePrepared;

        return symbianError;
    }

    return KErrGeneral;
}

int S60ImageCaptureSession::capture(const QString &fileName)
{
    if (!m_cameraStarted) {
        m_captureWhenReady = true;
        m_stillCaptureFileName = fileName; // Save name, it will be processed during actual capture
        return 0;
    }

    if (m_icState < EImageCapturePrepared) {
        int prepareSuccess = prepareImageCapture();
        if (prepareSuccess) {
            setError(prepareSuccess, QString("Failure during image capture preparation."), true);
            return 0;
        }
    } else if (m_icState > EImageCapturePrepared) {
        setError(KErrNotReady, QString("Previous operation is still ongoing."), true);
        return 0;
    }

    m_icState = EImageCaptureCapturing;

    // Give new ID for the new image
    m_currentImageId += 1;

    emit readyForCaptureChanged(false);

    processFileName(fileName);

    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->CaptureL());
        setError(err, QString("Image capture failed."), true);
    } else {
        setError(KErrNotReady, QString("Unexpected camera error."), true);
    }

#ifdef Q_CC_NOKIAX86 // Emulator
    QImage *snapImage = new QImage(QLatin1String("C:/Data/testimage.jpg"));
    emit imageExposed(m_currentImageId);
    emit imageCaptured(m_currentImageId, *snapImage);
    emit imageSaved(m_currentImageId, m_stillCaptureFileName);
#endif // Q_CC_NOKIAX86

    return m_currentImageId;
}

void S60ImageCaptureSession::cancelCapture()
{
    if (m_icState != EImageCaptureCapturing) {
        return;
    }

    if (m_cameraEngine)
        m_cameraEngine->cancelCapture();

    m_icState = EImageCapturePrepared;
}

void S60ImageCaptureSession::processFileName(const QString &fileName)
{
    // Empty FileName - Use default file name and path (C:\Data\Images\image.jpg)
    if (fileName.isEmpty()) {
        // Make sure default directory exists
        QDir videoDir(QDir::rootPath());
        if (!videoDir.exists(KDefaultImagePath)) {
            videoDir.mkpath(KDefaultImagePath);
        }
        QString defaultFile = KDefaultImagePath;
        defaultFile.append("\\");
        defaultFile.append(KDefaultImageFileName);
        m_stillCaptureFileName = defaultFile;

    } else { // Not empty

        QString fullFileName;

        // Relative FileName
        if (!fileName.contains(":")) {
            // Extract file name and path from the URL
            fullFileName = KDefaultImagePath;
            if (fileName.at(0) != '\\')
                fullFileName.append("\\");
            fullFileName.append(QDir::toNativeSeparators(fileName));

        // Absolute FileName
        } else {
            // Extract file name and path from the given location
            fullFileName = QDir::toNativeSeparators(fileName);
        }

        QString fileNameOnly = fullFileName.right(fullFileName.length() - fullFileName.lastIndexOf("\\") - 1);
        QString directory = fullFileName.left(fullFileName.lastIndexOf("\\"));
        if (directory.lastIndexOf("\\") == (directory.length() - 1))
            directory = directory.left(directory.length() - 1);

        // URL is Absolute path, not including file name
        if (!fileNameOnly.contains(".")) {
            if (fileNameOnly != "") {
                directory.append("\\");
                directory.append(fileNameOnly);
            }
            fileNameOnly = KDefaultImageFileName;
        }

        // Make sure absolute directory exists
        QDir imageDir(QDir::rootPath());
        if (!imageDir.exists(directory)) {
            imageDir.mkpath(directory);
        }

        QString resolvedFileName = directory;
        resolvedFileName.append("\\");
        resolvedFileName.append(fileNameOnly);
        m_stillCaptureFileName = resolvedFileName;
    }
}

void S60ImageCaptureSession::MceoFocusComplete()
{
    emit focusStatusChanged(QCamera::Locked, QCamera::LockAcquired);
}

void S60ImageCaptureSession::MceoCapturedDataReady(TDesC8* aData)
{
    emit imageExposed(m_currentImageId);

    m_icState = EImageCaptureWritingImage;

    // Create SnapShot from ImageData
    QImage snapImage;
    snapImage.loadFromData((const uchar *)aData->Ptr(), aData->Length(), "JPG");

    if (!snapImage.isNull()) {
        emit imageCaptured(m_currentImageId, snapImage);
    } else {
        setError(KErrNotSupported, QString("Failed to create snapshot from captured data."));
    }

    TFileName path = convertImagePath();

    // Try to save image and inform if it was succcesful
    TRAPD(err, saveImageL(aData, path));
    if (err) {
        setError(err, QString("Writing captured image to a file failed."), true);
        m_icState = EImageCapturePrepared;
        return;
    } else {
        emit imageSaved(m_currentImageId, m_stillCaptureFileName);
    }

    m_icState = EImageCapturePrepared;

    // Release image resources
    releaseImageBuffer();
}

void S60ImageCaptureSession::MceoCapturedBitmapReady(CFbsBitmap* aBitmap)
{
    emit imageExposed(m_currentImageId);

    m_icState = EImageCaptureWritingImage;

    if(aBitmap)
    {
        TSize size = aBitmap->SizeInPixels();
        TUint scanLineLength = CFbsBitmap::ScanLineLength(size.iWidth, aBitmap->DisplayMode());
        TUint32 sizeInWords = size.iHeight * scanLineLength / sizeof( TUint32 );
        TUint32 *pixelData = new TUint32[sizeInWords];

        if (!pixelData)
            return;

        // Convert to QImage
        aBitmap->LockHeap();
        TUint32 *dataPtr = aBitmap->DataAddress();
        memcpy(pixelData, dataPtr, sizeof(TUint32) * sizeInWords);
        aBitmap->UnlockHeap();

        TDisplayMode displayMode = aBitmap->DisplayMode();

        QImage::Format format = QImage::Format_Invalid;
        switch(displayMode)
        {
            case EColor256:
                format = QImage::Format_Indexed8;
                break;
            case EColor4K:
                format = QImage::Format_RGB444;
                break;
            case EColor64K:
                format = QImage::Format_RGB16;
                break;
            case EColor16M:
                format = QImage::Format_RGB666;
                break;
            case EColor16MU:
                format = QImage::Format_RGB32;
                break;
            case EColor16MA:
                format = QImage::Format_ARGB32;
                break;
            default:
                setError(KErrNotSupported, QString("Image format is not supported."), true);
                break;
        }

        // Create SnapShot
        QImage *snapImage = new QImage(
                (uchar*)pixelData,
                size.iWidth,
                size.iHeight,
                CFbsBitmap::ScanLineLength(size.iWidth, aBitmap->DisplayMode()),
                format);
        emit imageCaptured(m_currentImageId, *snapImage);

        TFileName path = convertImagePath();
        TInt saveError = aBitmap->Save(path);
        if (saveError == saveError)
            emit imageSaved(m_currentImageId, m_stillCaptureFileName);
        else {
            setError(saveError, QString("Saving captured image failed."), true);
        }
        aBitmap = NULL;
        releaseImageBuffer();

    } else {
        setError(KErrBadHandle, QString("Unexpected camera error."), true);
    }

    m_icState = EImageCapturePrepared;
}

void S60ImageCaptureSession::MceoHandleError(TCameraEngineError aErrorType, TInt aError)
{
    Q_UNUSED(aErrorType);
    setError(aError, QString("General camera error."));
}

TFileName S60ImageCaptureSession::convertImagePath()
{
    TFileName path = KNullDesC();

    // Convert to Symbian path
    TPtrC16 attachmentPath(KNullDesC);

    // Path is already included in filename
    attachmentPath.Set(reinterpret_cast<const TUint16*>(QDir::toNativeSeparators(m_stillCaptureFileName).utf16()));
    path.Append(attachmentPath);

    return path;
}

void S60ImageCaptureSession::saveImageL(TDesC8* aData, TFileName aPath)
{
    if (aData == NULL) {
        setError(KErrGeneral, QString("Captured image data is not available."), true);
    }

    if (aPath.Size() > 0) {
        RFs fs;
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);

        RFile file;
        User::LeaveIfError(file.Replace(fs, aPath, EFileWrite));
        CleanupClosePushL(file);
        User::LeaveIfError(file.Write(*aData));

        CleanupStack::PopAndDestroy(&file);
        CleanupStack::PopAndDestroy(&fs);
    } else {
        setError(KErrPathNotFound, QString("Invalid path given."), true);
    }
}

void S60ImageCaptureSession::releaseImageBuffer()
{
    if (m_cameraEngine)
        m_cameraEngine->ReleaseImageBuffer();
    else
        setError(KErrNotReady, QString("Unexpected camera error."), true);

    // Inform that we can continue taking more pictures
    emit readyForCaptureChanged(true);
}

/*
 * Queries camera properties
 * Results are returned to member variable m_info
 *
 * @return boolean indicating if querying the info was a success
 */
bool S60ImageCaptureSession::queryCurrentCameraInfo()
{
    if (m_cameraEngine) {
        m_cameraInfo = m_cameraEngine->cameraInfo();
        return true;
    }

    return false;
}

/*
 * This function handles different camera status changes
 */
void S60ImageCaptureSession::cameraStatusChanged(QCamera::Status status)
{
    if (status == QCamera::ActiveStatus) {
        m_cameraStarted = true;
        if (m_captureWhenReady) {
            capture(m_stillCaptureFileName);
        }
    }else if (status == QCamera::UnloadedStatus) {
        m_cameraStarted = false;
        m_icState = EImageCaptureNotPrepared;
    }
    else
        m_cameraStarted = false;
}

QSize S60ImageCaptureSession::captureSize() const
{
    return m_captureSize;
}

QSize S60ImageCaptureSession::minimumCaptureSize()
{
    return supportedCaptureSizesForCodec(formatMap().key(m_currentFormat)).first();
}
QSize S60ImageCaptureSession::maximumCaptureSize()
{
    return supportedCaptureSizesForCodec(formatMap().key(m_currentFormat)).last();
}

void S60ImageCaptureSession::setCaptureSize(const QSize &size)
{
    if (m_captureSize.isNull() || size.isEmpty()) {
        // An empty QSize indicates the encoder should make an optimal choice based on what is
        // available from the image source and the limitations of the codec.
        m_captureSize = supportedCaptureSizesForCodec(formatMap().key(m_currentFormat)).last();

    }
    else
        m_captureSize = size;
}

QList<QSize> S60ImageCaptureSession::supportedCaptureSizesForCodec(const QString &codecName)
{
    QList<QSize> list;

    // If we have CameraEngine loaded and we can update CameraInfo
    if (m_cameraEngine && queryCurrentCameraInfo()) {
        CCamera::TFormat format;
        if (codecName == "")
            format = defaultImageFormat();
        else
            format = selectFormatForCodec(codecName);

        CCamera *camera = m_cameraEngine->Camera();
        TSize imageSize;
        for (int i = 0; i < m_cameraInfo->iNumImageSizesSupported; i++) {
            camera->EnumerateCaptureSizes(imageSize, i, format);
            list << QSize(imageSize.iWidth, imageSize.iHeight); // Add resolution to the list
        }
    }

#ifdef Q_CC_NOKIAX86 // Emulator
    // Add some for testing purposes
    list << QSize(50, 50);
    list << QSize(100, 100);
    list << QSize(800,600);
#endif

    return list;
}

QMap<QString, int> S60ImageCaptureSession::formatMap()
{
    QMap<QString, int> formats;

    // Format list copied from CCamera::TFormat (in ecam.h)
    formats.insert("Monochrome",        0x0001);
    formats.insert("16bitRGB444",       0x0002);
    formats.insert("16BitRGB565",       0x0004);
    formats.insert("32BitRGB888",       0x0008);
    formats.insert("Jpeg",              0x0010);
    formats.insert("Exif",              0x0020);
    formats.insert("FbsBitmapColor4K",  0x0040);
    formats.insert("FbsBitmapColor64K", 0x0080);
    formats.insert("FbsBitmapColor16M", 0x0100);
    formats.insert("UserDefined",       0x0200);
    formats.insert("YUV420Interleaved", 0x0400);
    formats.insert("YUV420Planar",      0x0800);
    formats.insert("YUV422",            0x1000);
    formats.insert("YUV422Reversed",    0x2000);
    formats.insert("YUV444",            0x4000);
    formats.insert("YUV420SemiPlanar",  0x8000);
    formats.insert("FbsBitmapColor16MU", 0x00010000);
    formats.insert("MJPEG",             0x00020000);
    formats.insert("EncodedH264",       0x00040000);

    return formats;
}

QMap<QString, QString> S60ImageCaptureSession::codecDescriptionMap()
{
    QMap<QString, QString> formats;

    formats.insert("image/jpg", "JPEG image codec");

    return formats;
}

QStringList S60ImageCaptureSession::supportedImageCaptureCodecs()
{
#ifdef Q_CC_NOKIAX86 // Emulator
    return formatMap().keys();
#endif
    return m_supportedImageCodecs;
}

void S60ImageCaptureSession::updateImageCaptureFormats()
{
    m_formats.clear();
    if (m_cameraEngine && queryCurrentCameraInfo()) {
        TUint32 supportedFormats = m_cameraInfo->iImageFormatsSupported;

#ifdef S60_3X_PLATFORM // S60 3.1 & 3.2
        int maskEnd = CCamera::EFormatFbsBitmapColor16MU;
#else // S60 5.0 or later
        int maskEnd = CCamera::EFormatEncodedH264;
#endif // S60_3X_PLATFORM

        for (int mask = CCamera::EFormatMonochrome; mask <= maskEnd; mask <<= 1) {
            if (supportedFormats & mask)
                m_formats << mask; // Store mask of supported format
        }
    }
}

QString S60ImageCaptureSession::imageCaptureCodec()
{
    return m_currentCodec;
}
void S60ImageCaptureSession::setImageCaptureCodec(const QString &codecName)
{
    if (!codecName.isEmpty()) {
        if (supportedImageCaptureCodecs().contains(codecName, Qt::CaseInsensitive)) {
            m_currentCodec = codecName;
            m_currentFormat = selectFormatForCodec(m_currentCodec);
            return;
        }
    }
    setError(KErrNotSupported, QString("Requested image codec is not supported"));
}

QString S60ImageCaptureSession::imageCaptureCodecDescription(const QString &codecName)
{
    QString description = codecDescriptionMap().value(codecName);
    return description;
}

QtMultimediaKit::EncodingQuality S60ImageCaptureSession::captureQuality() const
{
    if (m_symbianImageQuality <= 1) {
        return QtMultimediaKit::VeryLowQuality;
    }

    return static_cast<QtMultimediaKit::EncodingQuality> (m_symbianImageQuality / KSymbianImageQualityCoefficient);
}

void S60ImageCaptureSession::setCaptureQuality(const QtMultimediaKit::EncodingQuality &quality)
{
    switch (quality) {
        case QtMultimediaKit::VeryLowQuality:
            m_symbianImageQuality = 1;
            break;
        default:
            m_symbianImageQuality = quality * KSymbianImageQualityCoefficient;
            break;
    }
}

qreal S60ImageCaptureSession::maximumZoom()
{
    qreal maxZoomFactor = 1.0;

    if (queryCurrentCameraInfo()) {
        maxZoomFactor = m_cameraInfo->iMaxZoomFactor;

        if (maxZoomFactor == 0.0 || maxZoomFactor == 1.0) {
            return 1.0; // Not supported
        } else {
            return maxZoomFactor;
        }
    } else {
        return 1.0;
    }
}

qreal S60ImageCaptureSession::minZoom()
{
    qreal minZoomValue = 1.0;

    if (queryCurrentCameraInfo()) {
        minZoomValue = m_cameraInfo->iMinZoomFactor;
        if (minZoomValue == 0.0 || minZoomValue == 1.0)
            return 1.0; // Macro Zoom is not supported
        else {
            return minZoomValue;
        }

    } else {
        return 1.0;
    }
}

qreal S60ImageCaptureSession::maxDigitalZoom()
{
    qreal maxDigitalZoomFactor = 1.0;

    if (queryCurrentCameraInfo()) {
        maxDigitalZoomFactor = m_cameraInfo->iMaxDigitalZoomFactor;
        return maxDigitalZoomFactor;
    } else {
        return 1.0;
    }
}

void S60ImageCaptureSession::doSetZoomFactorL(qreal optical, qreal digital)
{
    // Convert Zoom Factor to Zoom Value if AdvSettings are not available
#if !defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER) & !defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER)
    // Convert Zoom Factors to Symbian Zoom Values
    int opticalSymbian = (optical * m_cameraInfo->iMaxZoom) / maximumZoom();
    int digitalSymbian = (digital * m_cameraInfo->iMaxDigitalZoom) / maxDigitalZoom();
#endif // !USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER & !USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER

    if (m_cameraEngine && !m_cameraEngine->IsCameraReady()) {
        return;
    }

    if (m_cameraEngine && queryCurrentCameraInfo()) {
        CCamera *camera = m_cameraEngine->Camera();
        if (camera) {
            // Optical Zoom
            if (optical != opticalZoomFactor()) {
                if (optical >= m_cameraInfo->iMinZoomFactor && optical <= m_cameraInfo->iMaxZoomFactor) {
#if defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER) | defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER)
                    m_advancedSettings->setOpticalZoomFactorL(optical); // Using Zoom Factor
#else // No advanced settigns
                    camera->SetZoomFactorL(opticalSymbian); // Using Zoom Value
#endif // USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER
                } else {
                    setError(KErrNotSupported, QString("Requested optical zoom factor is not supported."));
                    return;
                }
            }

            // Digital Zoom (Smooth Zoom - Zoom value set in steps)
            if (digital != digitalZoomFactor()) {
                if (digital >= 1 && digital <= m_cameraInfo->iMaxDigitalZoomFactor) {
#if defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER) | defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER)
                    qreal currentZoomFactor = m_advancedSettings->digitalZoomFactorL();

                    QList<qreal> smoothZoomSetValues;
                    QList<qreal> *factors = m_advancedSettings->supportedDigitalZoomFactors();
                    if (currentZoomFactor < digital) {
                        for (int i = 0; i < factors->count(); ++i) {
                            if (factors->at(i) > currentZoomFactor && factors->at(i) < digital)
                                smoothZoomSetValues << factors->at(i);
                        }

                        for (int i = 0; i < smoothZoomSetValues.count(); i = i + KSmoothZoomStep) {
                            m_advancedSettings->setDigitalZoomFactorL(smoothZoomSetValues[i]); // Using Zoom Factor
                        }

                    } else {
                        for (int i = 0; i < factors->count(); ++i) {
                            if (factors->at(i) < currentZoomFactor && factors->at(i) > digital)
                                smoothZoomSetValues << factors->at(i);
                        }

                        for (int i = (smoothZoomSetValues.count() - 1); i >= 0; i = i - KSmoothZoomStep) {
                            m_advancedSettings->setDigitalZoomFactorL(smoothZoomSetValues[i]); // Using Zoom Factor
                        }
                    }

                    // Set final value
                    m_advancedSettings->setDigitalZoomFactorL(digital); // Using Zoom Factor
#else // No advanced settigns
                    // Define zoom steps
                    int currentZoomFactor = camera->DigitalZoomFactor();
                    int difference = abs(currentZoomFactor - digitalSymbian);
                    int midZoomValue = currentZoomFactor;

                    if (currentZoomFactor < digitalSymbian) {
                        while (midZoomValue < (digitalSymbian - KSmoothZoomStep)) {
                            midZoomValue = midZoomValue + KSmoothZoomStep;
                            camera->SetDigitalZoomFactorL(midZoomValue);
                        }
                    } else {
                        while (midZoomValue > (digitalSymbian + KSmoothZoomStep)) {
                            midZoomValue = midZoomValue - KSmoothZoomStep;
                            camera->SetDigitalZoomFactorL(midZoomValue);
                        }
                    }

                    // Set final and emit signal
                    camera->SetDigitalZoomFactorL(digitalSymbian);
#endif // USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER
                } else {
                    setError(KErrNotSupported, QString("Requested digital zoom factor is not supported."));
                    return;
                }
            }

        }
    } else {
        setError(KErrGeneral, QString("Unexpected camera error."));
    }
}

qreal S60ImageCaptureSession::opticalZoomFactor()
{
    qreal factor = 1.0;

#if defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER) | defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER)
    if (m_advancedSettings) {
        TRAPD(err, factor = m_advancedSettings->opticalZoomFactorL());
        if (err)
            return 1.0;
    }
#else // No advanced settigns
    if (m_cameraEngine) {
        if (m_cameraEngine->Camera())
            factor = m_cameraEngine->Camera()->ZoomFactor();
	}
#endif // USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER

    if (factor == 0.0) // If not supported
        factor = 1.0;

    return factor;
}

qreal S60ImageCaptureSession::digitalZoomFactor()
{
    qreal factor = 1.0;

#if defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER) | defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER)
    if (m_advancedSettings) {
        TRAPD(err, factor = m_advancedSettings->digitalZoomFactorL());
        if (err)
            return 1.0;
    }
#else // No advanced settigns
    if (m_cameraEngine) {
        if (m_cameraEngine->Camera())
            factor = m_cameraEngine->Camera()->DigitalZoomFactor();
	}
#endif // USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER

    if (factor == 0.0)
        factor = 1.0;

    return factor;
}

void S60ImageCaptureSession::setFlashMode(QCameraExposure::FlashModes mode)
{
    TRAPD(err, doSetFlashModeL(mode));
    setError(err, QString("Failed to set flash mode."));
}

void S60ImageCaptureSession::doSetFlashModeL(QCameraExposure::FlashModes mode)
{
    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        switch(mode) {
            case QCameraExposure::FlashOff:
                camera->SetFlashL(CCamera::EFlashNone);
                break;
            case QCameraExposure::FlashAuto:
                camera->SetFlashL(CCamera::EFlashAuto);
                break;
            case QCameraExposure::FlashOn:
                camera->SetFlashL(CCamera::EFlashForced);
                break;
            case QCameraExposure::FlashRedEyeReduction:
                camera->SetFlashL(CCamera::EFlashRedEyeReduce);
                break;
            case QCameraExposure::FlashFill:
                camera->SetFlashL(CCamera::EFlashFillIn);
                break;

            default:
                setError(QCamera::NotSupportedFeatureError, QString("Requested flash mode is not suported"));
                break;
        }
    }
    else
        setError(KErrNotReady, QString("Unexpected camera error."));
}

QCameraExposure::FlashMode S60ImageCaptureSession::flashMode()
{
    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        switch(camera->Flash()) {
            case CCamera::EFlashAuto:
                return QCameraExposure::FlashAuto;
            case CCamera::EFlashForced:
                return QCameraExposure::FlashOn;
            case CCamera::EFlashRedEyeReduce:
                return QCameraExposure::FlashRedEyeReduction;
            case CCamera::EFlashFillIn:
                return QCameraExposure::FlashFill;
            case CCamera::EFlashNone:
                return QCameraExposure::FlashOff;

            default:
                return QCameraExposure::FlashAuto; // Most probable default
        }
    }
    else
        setError(KErrNotReady, QString("Unexpected camera error."));

    return QCameraExposure::FlashOff;
}

QCameraExposure::FlashModes S60ImageCaptureSession::supportedFlashModes()
{
    QCameraExposure::FlashModes modes = QCameraExposure::FlashOff;

    if (queryCurrentCameraInfo()) {
        TInt supportedModes = m_cameraInfo->iFlashModesSupported;

        if (supportedModes == 0)
            return modes;

        if (supportedModes & CCamera::EFlashManual) {
             modes |= QCameraExposure::FlashOff;
        }
        if (supportedModes & CCamera::EFlashForced) {
             modes |= QCameraExposure::FlashOn;
        }
        if (supportedModes & CCamera::EFlashAuto) {
             modes |= QCameraExposure::FlashAuto;
        }
        if (supportedModes & CCamera::EFlashFillIn) {
             modes |= QCameraExposure::FlashFill;
        }
        if (supportedModes & CCamera::EFlashRedEyeReduce) {
             modes |= QCameraExposure::FlashRedEyeReduction;
        }
    }

    return modes;
}

QCameraExposure::ExposureMode S60ImageCaptureSession::exposureMode()
{
    if (m_cameraEngine) {
        CCamera* camera = m_cameraEngine->Camera();
        switch(camera->Exposure()) {
            case CCamera::EExposureManual:
                return QCameraExposure::ExposureManual;
            case CCamera::EExposureAuto:
                return QCameraExposure::ExposureAuto;
            case CCamera::EExposureNight:
                return QCameraExposure::ExposureNight;
            case CCamera::EExposureBacklight:
                return QCameraExposure::ExposureBacklight;
            case CCamera::EExposureSport:
                return QCameraExposure::ExposureSports;
            case CCamera::EExposureSnow:
                return QCameraExposure::ExposureSnow;
            case CCamera::EExposureBeach:
                return QCameraExposure::ExposureBeach;

            default:
                return QCameraExposure::ExposureAuto;
        }
    }

    return QCameraExposure::ExposureAuto;
}

bool S60ImageCaptureSession::isExposureModeSupported(QCameraExposure::ExposureMode mode) const
{
    TInt supportedModes = m_cameraInfo->iExposureModesSupported;

    if (supportedModes == 0)
        return false;

    switch (mode) {
        case QCameraExposure::ExposureManual:
            if(supportedModes & CCamera::EExposureManual)
                return true;
            else
                return false;
        case QCameraExposure::ExposureAuto:
            if(supportedModes & CCamera::EExposureAuto)
                return true;
            else
                return false;
        case QCameraExposure::ExposureNight:
            if(supportedModes & CCamera::EExposureNight)
                return true;
            else
                return false;
        case QCameraExposure::ExposureBacklight:
            if(supportedModes & CCamera::EExposureBacklight)
                return true;
            else
                return false;
        case QCameraExposure::ExposureSports:
            if(supportedModes & CCamera::EExposureSport)
                return true;
            else
                return false;
        case QCameraExposure::ExposureSnow:
            if(supportedModes & CCamera::EExposureSnow)
                return true;
            else
                return false;
        case QCameraExposure::ExposureBeach:
            if(supportedModes & CCamera::EExposureBeach)
                return true;
            else
                return false;

        default:
            return false;
    }
}

void S60ImageCaptureSession::setExposureMode(QCameraExposure::ExposureMode mode)
{
    TRAPD(err, doSetExposureModeL(mode));
    setError(err, QString("Failed to set exposure mode."));
}

void S60ImageCaptureSession::doSetExposureModeL( QCameraExposure::ExposureMode mode)
{
    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        switch(mode) {
            case QCameraExposure::ExposureManual:
                camera->SetExposureL(CCamera::EExposureManual);
                break;
            case QCameraExposure::ExposureAuto:
                camera->SetExposureL(CCamera::EExposureAuto);
                break;
            case QCameraExposure::ExposureNight:
                camera->SetExposureL(CCamera::EExposureNight);
                break;
            case QCameraExposure::ExposureBacklight:
                camera->SetExposureL(CCamera::EExposureBacklight);
                break;
            case QCameraExposure::ExposureSports:
                camera->SetExposureL(CCamera::EExposureSport);
                break;
            case QCameraExposure::ExposureSnow:
                camera->SetExposureL(CCamera::EExposureSnow);
                break;
            case QCameraExposure::ExposureBeach:
                camera->SetExposureL(CCamera::EExposureBeach);
                break;
            case QCameraExposure::ExposureLargeAperture:
            case QCameraExposure::ExposureSmallAperture:
                break;
            case QCameraExposure::ExposurePortrait:
            case QCameraExposure::ExposureSpotlight:
            default:
                setError(QCamera::NotSupportedFeatureError, QString("Requested exposure mode is not suported"));
                break;
        }
    }
    else
        setError(KErrNotReady, QString("Unexpected camera error."));
}

int S60ImageCaptureSession::contrast() const
{
    if (m_cameraEngine) {
        return m_cameraEngine->Camera()->Contrast();
    } else {
        return 0;
    }
}

void S60ImageCaptureSession::setContrast(int value)
{
    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->Camera()->SetContrastL(value));
        setError(err, QString("Failed to set contrast."));
    }
    else
        setError(KErrNotReady, QString("Unexpected camera error."));
}

int S60ImageCaptureSession::brightness() const
{
    if (m_cameraEngine) {
        return m_cameraEngine->Camera()->Brightness();
    } else {
        return 0;
    }
}

void S60ImageCaptureSession::setBrightness(int value)
{
    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->Camera()->SetBrightnessL(value));
        setError(err, QString("Failed to set brightness."));
    }
    else
        setError(KErrNotReady, QString("Unexpected camera error."));
}

 QCameraImageProcessing::WhiteBalanceMode S60ImageCaptureSession::whiteBalanceMode()
{
    if (m_cameraEngine) {
        CCamera::TWhiteBalance mode = m_cameraEngine->Camera()->WhiteBalance();
        switch(mode) {
            case CCamera::EWBAuto:
                return  QCameraImageProcessing::WhiteBalanceAuto;
            case CCamera::EWBDaylight:
                return  QCameraImageProcessing::WhiteBalanceSunlight;
            case CCamera::EWBCloudy:
                return  QCameraImageProcessing::WhiteBalanceCloudy;
            case CCamera::EWBTungsten:
                return  QCameraImageProcessing::WhiteBalanceTungsten;
            case CCamera::EWBFluorescent:
                return  QCameraImageProcessing::WhiteBalanceFluorescent;
            case CCamera::EWBFlash:
                return  QCameraImageProcessing::WhiteBalanceFlash;
            case CCamera::EWBBeach:
                return  QCameraImageProcessing::WhiteBalanceSunset;
            case CCamera::EWBManual:
                return  QCameraImageProcessing::WhiteBalanceManual;
            case CCamera::EWBShade:
                return  QCameraImageProcessing::WhiteBalanceShade;

            default:
                return  QCameraImageProcessing::WhiteBalanceAuto;
        }
    }

    return  QCameraImageProcessing::WhiteBalanceAuto;
}

void S60ImageCaptureSession::setWhiteBalanceMode( QCameraImageProcessing::WhiteBalanceMode mode)
{
    TRAPD(err, doSetWhiteBalanceModeL(mode));
    setError(err, QString("Failed to set white balance mode."));
}

void S60ImageCaptureSession::doSetWhiteBalanceModeL( QCameraImageProcessing::WhiteBalanceMode mode)
{
    if (m_cameraEngine) {
        CCamera* camera = m_cameraEngine->Camera();
        switch(mode) {
            case  QCameraImageProcessing::WhiteBalanceAuto:
                camera->SetWhiteBalanceL(CCamera::EWBAuto);
                break;
            case  QCameraImageProcessing::WhiteBalanceSunlight:
                camera->SetWhiteBalanceL(CCamera::EWBDaylight);
                break;
            case  QCameraImageProcessing::WhiteBalanceCloudy:
                camera->SetWhiteBalanceL(CCamera::EWBCloudy);
                break;
            case  QCameraImageProcessing::WhiteBalanceTungsten:
                camera->SetWhiteBalanceL(CCamera::EWBTungsten);
                break;
            case  QCameraImageProcessing::WhiteBalanceFluorescent:
                camera->SetWhiteBalanceL(CCamera::EWBFluorescent);
                break;
            case  QCameraImageProcessing::WhiteBalanceFlash:
                camera->SetWhiteBalanceL(CCamera::EWBFlash);
                break;
            case  QCameraImageProcessing::WhiteBalanceSunset:
                camera->SetWhiteBalanceL(CCamera::EWBBeach);
                break;
            case  QCameraImageProcessing::WhiteBalanceManual:
                camera->SetWhiteBalanceL(CCamera::EWBManual);
                break;
            case  QCameraImageProcessing::WhiteBalanceShade:
                camera->SetWhiteBalanceL(CCamera::EWBShade);
                break;

            default:
                setError(QCamera::NotSupportedFeatureError, QString("Requested white balance mode is not suported"));
                break;
        }
    }
    else
        setError(KErrNotReady, QString("Unexpected camera error."));
}

bool S60ImageCaptureSession::isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceMode mode) const
{
    if (m_cameraEngine) {
        TInt supportedModes = m_cameraInfo->iWhiteBalanceModesSupported;
        switch (mode) {
            case QCameraImageProcessing::WhiteBalanceManual:
                if (supportedModes & CCamera::EWBManual)
                    return true;
                else
                    return false;
            case QCameraImageProcessing::WhiteBalanceAuto:
                if (supportedModes & CCamera::EWBAuto)
                    return true;
                else
                    return false;
            case QCameraImageProcessing::WhiteBalanceSunlight:
                if (supportedModes & CCamera::EWBDaylight)
                    return true;
                else
                    return false;
            case QCameraImageProcessing::WhiteBalanceCloudy:
                if (supportedModes & CCamera::EWBCloudy)
                    return true;
                else
                    return false;
            case QCameraImageProcessing::WhiteBalanceShade:
                if (supportedModes & CCamera::EWBShade)
                    return true;
                else
                    return false;
            case QCameraImageProcessing::WhiteBalanceTungsten:
                if (supportedModes & CCamera::EWBTungsten)
                    return true;
                else
                    return false;
            case QCameraImageProcessing::WhiteBalanceFluorescent:
                if (supportedModes & CCamera::EWBFluorescent)
                    return true;
                else
                    return false;
            case QCameraImageProcessing::WhiteBalanceIncandescent: // Not available in Symbian
                    return false;
            case QCameraImageProcessing::WhiteBalanceFlash:
                if (supportedModes & CCamera::EWBFlash)
                    return true;
                else
                    return false;
            case QCameraImageProcessing::WhiteBalanceSunset:
                if (supportedModes & CCamera::EWBBeach)
                    return true;
                else
                    return false;

            default:
                return false;
        }

    }

    return false;
}

/*
 * ====================
 * S60 3.1 AutoFocosing
 * ====================
 */
bool S60ImageCaptureSession::isFocusSupported() const
{
    return m_cameraEngine->IsAutoFocusSupported();
}

void S60ImageCaptureSession::startFocus()
{
    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->StartFocusL());
        setError(err, QString("Failed to start focusing."));
    }
    else
        setError(KErrNotReady, QString("Unexpected camera error."));
}

void S60ImageCaptureSession::cancelFocus()
{
    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->FocusCancel());
        setError(err, QString("Failed to cancel focusing."));
    }
    else
        setError(KErrNotReady, QString("Unexpected camera error."));
}

// End of file

