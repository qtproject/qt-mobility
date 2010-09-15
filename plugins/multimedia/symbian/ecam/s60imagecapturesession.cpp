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

#include <qglobal.h>
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
    m_imageQuality(QtMultimediaKit::NormalQuality*KSymbianImageQualityCoefficient),
    m_captureSize(QSize()),
    m_activeDeviceIndex(KDefaultCameraDevice),
    m_error(NoError),
    m_currentImageId(0)
{
    // Set defaults so that camera works with all devices
    m_currentCodec = defaultCodec();
}

S60ImageCaptureSession::~S60ImageCaptureSession()
{
    // Delete AdvancedSettings
    deleteAdvancedSettings();
}

CCamera::TFormat S60ImageCaptureSession::defaultCodec()
{
    if (m_activeDeviceIndex == 0)
        return CCamera::EFormatExif; // Primary Camera
    else
        return CCamera::EFormatFbsBitmapColor64K; // Secondary Camera or other
}

bool S60ImageCaptureSession::isDeviceReady()
{
#ifdef Q_CC_NOKIAX86 // Emulator
    return true;
#endif

    if (m_cameraEngine)
        return m_cameraEngine->IsCameraReady();

    return EFalse;
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
    m_cameraEngine = camerahandle;
    resetSession();
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
    m_currentCodec = defaultCodec();
    TRAPD(err, m_advancedSettings = new (ELeave) S60CameraSettings(this, m_cameraEngine));
    setError(err);

    if (m_cameraEngine)
        m_cameraEngine->SetAdvancedObserver(m_advancedSettings);
    else
        setError(KErrNotReady);

    updateImageCaptureCodecs();
    emit advancedSettingCreated();
}

S60CameraSettings* S60ImageCaptureSession::advancedSettings()
{
    return m_advancedSettings;
}

/*
 * This function can be used both internally and from Control classes using this session.
 * The error notification will go to client application through QCameraImageCapture error signal.
 */
void S60ImageCaptureSession::setError(TInt aError)
{
    if (aError == KErrNone)
        return;

    m_error = aError;
    QCamera::Error cameraError = fromSymbianErrorToQtMultimediaError(aError);

    QString errorDescription;
    switch (cameraError) {
        case QCamera::ServiceMissingError:
            errorDescription = "Camera service is missing";
            break;
        case QCamera::NotSupportedFeatureError:
            errorDescription = "The requested camera feature is not supported";
            break;
        case QCamera::CameraError:
            errorDescription = "General camera error occurred";
            break;

        default:
            errorDescription = "Unexpected camera error occurred";
            break;
    }

    emit error(cameraError, errorDescription);
}

QCamera::Error S60ImageCaptureSession::fromSymbianErrorToQtMultimediaError(int aError)
{
    switch(aError) {
        case KErrNone:
            return QCamera::NoError; // No errors have occurred
        case KErrNotSupported:
        case KErrECamSettingNotSupported:
        case KErrECamParameterNotInRange:
            return QCamera::NotSupportedFeatureError; // The feature is not supported
        case KErrNoMemory:
        case KErrNotFound:
        case KErrBadHandle:
            return QCamera::ServiceMissingError; // No camera service available

        default:
            return QCamera::CameraError; // An error has occurred (i.e. General Error)
    }
}

int S60ImageCaptureSession::currentImageId() const
{
    return m_currentImageId;
}

int S60ImageCaptureSession::prepareImageCapture()
{
    int symbianError = KErrNone;
    TSize captureSize = TSize(m_captureSize.width(), m_captureSize.height());
    TRAP(symbianError, m_cameraEngine->PrepareL(captureSize, m_currentCodec));

    return symbianError;
}

int S60ImageCaptureSession::capture(const QString &fileName)
{
    // Give new ID for the new image
    m_currentImageId += 1;

    m_error = KErrNone;
    emit readyForCaptureChanged(false);

    if (m_stillCaptureFileName.isNull() || m_stillCaptureFileName.isEmpty() ) {
        m_stillCaptureFileName = "C:\\Data\\Images\\image.jpg";
    } else {
        m_stillCaptureFileName = fileName;
    }

    // Check capture size - uses the default one if not set
    setCaptureSize(m_captureSize);

    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->CaptureL());
        setError(err);
    } else {
        setError(KErrNotReady);
    }

#ifdef Q_CC_NOKIAX86 // Emulator
    QImage *snapImage = new QImage(QLatin1String("C:/Data/testimage.jpg"));
    emit imageCaptured(m_currentImageId, *snapImage);
    emit imageSaved(m_currentImageId, m_stillCaptureFileName);
#endif

    return m_currentImageId;
}

void S60ImageCaptureSession::cancelCapture()
{
}

void S60ImageCaptureSession::MceoCameraReady()
{
    // Engine calls iObserver (S60CameraControl) for CameraReady
}

void S60ImageCaptureSession::MceoFocusComplete()
{
    emit focusStatusChanged(QCamera::Locked, QCamera::LockAcquired);
}

void S60ImageCaptureSession::MceoCapturedDataReady(TDesC8* aData)
{
    // Create snapshot
    QImage snapImage = QImage::fromData((const uchar *)aData->Ptr(), aData->Length());
    emit imageCaptured(m_currentImageId, snapImage);

    TFileName path = imagePath();

    // Try to save image and inform if it was succcesful
    TRAPD(err, saveImageL(aData, path));
    setError(err);
    emit imageSaved(m_currentImageId, m_stillCaptureFileName);

    // Release image resources
    releaseImageBuffer();
}

TFileName S60ImageCaptureSession::imagePath()
{
    TFileName path = KNullDesC();
    QFileInfo fileInfo(m_stillCaptureFileName);
    QString absolutePath = fileInfo.path();
    QDir dir(absolutePath);
    if (!dir.exists()) {
        emit error(QCameraImageCapture::ResourceError, QLatin1String("Folder does not exist"));
        return path;
    }

    // Convert to Symbian path
    TPtrC16 attachmentPath(KNullDesC);
    if (absolutePath.length() > 1) {
        // Path is already included in filename
        attachmentPath.Set(reinterpret_cast<const TUint16*>(QDir::toNativeSeparators(m_stillCaptureFileName).utf16()));
        path.Append(attachmentPath);
    } else {
        // Use default path
        path = PathInfo::PhoneMemoryRootPath();
        path.Append(PathInfo::ImagesPath());
        attachmentPath.Set(reinterpret_cast<const TUint16*>(m_stillCaptureFileName.utf16()));
        path.Append(attachmentPath);
        m_stillCaptureFileName.prepend(QDesktopServices::storageLocation(QDesktopServices::PicturesLocation)+"/");
    }

    return path;
}

void S60ImageCaptureSession::saveImageL(TDesC8* aData, TFileName aPath)
{
    if (aPath.Size() > 0) {
        RFs fs;
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);

        RFile file;
        User::LeaveIfError(file.Create(fs, aPath, EFileWrite));
        CleanupClosePushL(file);
        User::LeaveIfError(file.Write(*aData));

        CleanupStack::PopAndDestroy(&file);
        CleanupStack::PopAndDestroy(&fs);
    } else {
        setError(KErrPathNotFound);
    }
}

void S60ImageCaptureSession::releaseImageBuffer()
{
    if (m_cameraEngine)
        m_cameraEngine->ReleaseImageBuffer();
    else
        setError(KErrNotReady);

    // Inform that we can continue taking more pictures
    emit readyForCaptureChanged(true);
}

void S60ImageCaptureSession::MceoCapturedBitmapReady(CFbsBitmap* aBitmap)
{
    if(aBitmap)
    {
        TSize size = aBitmap->SizeInPixels();
        TUint32 sizeInWords = size.iHeight * CFbsBitmap::ScanLineLength(size.iWidth, aBitmap->DisplayMode()) / sizeof( TUint32 );
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
                setError(KErrNotSupported);
                break;
        }

        QImage *snapImage = new QImage(
                (uchar*)pixelData,
                size.iWidth,
                size.iHeight,
                CFbsBitmap::ScanLineLength(size.iWidth, aBitmap->DisplayMode()),
                format);
        emit imageCaptured(m_currentImageId, *snapImage);

        TFileName path = imagePath();
        TInt saveError = aBitmap->Save(path);
        if (saveError == saveError)
            emit imageSaved(m_currentImageId, m_stillCaptureFileName);
        else {
            setError(saveError);
        }
        aBitmap = NULL;
        releaseImageBuffer();

    } else {
        setError(KErrBadHandle);
    }
}

void S60ImageCaptureSession::MceoViewFinderFrameReady(CFbsBitmap& aFrame)
{
    Q_UNUSED(aFrame);
}

void S60ImageCaptureSession::MceoHandleError(TCameraEngineError aErrorType, TInt aError)
{
    Q_UNUSED(aErrorType);
    setError(aError);
}

/*
 * Queries all kinds of camera properties
 * Results are returned to member variable m_info
 *
 * @return boolean indicating if querying the info was a success
 */
bool S60ImageCaptureSession::queryCurrentCameraInfo()
{
    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        if (camera) {
            camera->CameraInfo(m_info);
        }
        return true;
    }

    return false;
}

QSize S60ImageCaptureSession::captureSize() const
{
    return m_captureSize;
}

QSize S60ImageCaptureSession::minimumCaptureSize()
{
    return supportedCaptureSizesForCodec(formatMap().key(m_currentCodec)).first();
}
QSize S60ImageCaptureSession::maximumCaptureSize()
{
    return supportedCaptureSizesForCodec(formatMap().key(m_currentCodec)).last();
}

void S60ImageCaptureSession::setCaptureSize(const QSize &size)
{
    if (m_captureSize.isNull() || size.isEmpty()) {
        // An empty QSize indicates the encoder should make an optimal choice based on what is
        // available from the image source and the limitations of the codec.
        m_captureSize = supportedCaptureSizesForCodec(formatMap().key(m_currentCodec)).last();

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
            format = defaultCodec();
        else
            format = static_cast<CCamera::TFormat>( formatMap().value(codecName) );

        CCamera *camera = m_cameraEngine->Camera();
        for (int i = 0; i < m_info.iNumImageSizesSupported; i++) {
            TSize size;
            camera->EnumerateVideoFrameSizes(size, i, format);
            list << QSize(size.iWidth, size.iHeight); // Add resolution to the list
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
    formats.insert("Monochrome", 0x0001);
    formats.insert("16bitRGB444", 0x0002);
    formats.insert("16BitRGB565", 0x0004);
    formats.insert("32BitRGB888", 0x0008);
    formats.insert("Jpeg", 0x0010);
    formats.insert("Exif", 0x0020);
    formats.insert("FbsBitmapColor4K", 0x0040);
    formats.insert("FbsBitmapColor64K", 0x0080);
    formats.insert("FbsBitmapColor16M", 0x0100);
    formats.insert("UserDefined", 0x0200);
    formats.insert("YUV420Interleaved", 0x0400);
    formats.insert("YUV420Planar", 0x0800);
    formats.insert("YUV422", 0x1000);
    formats.insert("YUV422Reversed", 0x2000);
    formats.insert("YUV444", 0x4000);
    formats.insert("YUV420SemiPlanar", 0x8000);
    formats.insert("FbsBitmapColor16MU", 0x00010000);
    formats.insert("MJPEG", 0x00020000);
    formats.insert("EncodedH264", 0x00040000);

    return formats;
}

QMap<QString, int> S60ImageCaptureSession::formatDescMap()
{
    QMap<QString, int> formats;

    // E.g. Y1Y2Y3Y4...U1U2...V1V2...etc.
    formats.insert("Monochrome", 0x0001);
    formats.insert("16bitRGB444", 0x0002);
    formats.insert("16BitRGB565", 0x0004);
    formats.insert("32BitRGB888", 0x0008);
    formats.insert("Jpeg", 0x0010);
    formats.insert("Exif", 0x0020);
    formats.insert("FbsBitmapColor4K", 0x0040);
    formats.insert("FbsBitmapColor64K", 0x0080);
    formats.insert("FbsBitmapColor16M", 0x0100);
    formats.insert("UserDefined", 0x0200);
    formats.insert("YUV420Interleaved", 0x0400);
    formats.insert("YUV420Planar", 0x0800);
    formats.insert("YUV422", 0x1000);
    formats.insert("YUV422Reversed", 0x2000);
    formats.insert("YUV444", 0x4000);
    formats.insert("YUV420SemiPlanar", 0x8000);
    formats.insert("FbsBitmapColor16MU", 0x00010000);
    formats.insert("MJPEG", 0x00020000);
    formats.insert("EncodedH264", 0x00040000);

    return formats;
}

QStringList S60ImageCaptureSession::supportedImageCaptureCodecs()
{
    QStringList list;

#ifdef Q_CC_NOKIAX86 // Emulator
    return formatMap().keys();
#endif

    for (int i = 0; i < m_formats.length(); i++) {
        list << formatMap().key(m_formats.at(i));
    }

    return list;
}

void S60ImageCaptureSession::updateImageCaptureCodecs()
{
    m_formats.clear();
    if (m_cameraEngine && queryCurrentCameraInfo()) {
        TUint32 supportedFormats = m_info.iImageFormatsSupported;

#ifdef PRE_S60_50_PLATFORM // S60 3.1 & 3.2
        int maskEnd = CCamera::EFormatFbsBitmapColor16MU;
#else
        int maskEnd = CCamera::EFormatEncodedH264;
#endif

        for (int mask = CCamera::EFormatMonochrome; mask <= maskEnd; mask <<= 1) {
            if (supportedFormats & mask)
                m_formats << mask; // Store mask of supported format
        }
    }
}

QString S60ImageCaptureSession::imageCaptureCodec()
{
    return formatMap().key(m_currentCodec);
}
void S60ImageCaptureSession::setImageCaptureCodec(const QString &codecName)
{
    m_currentCodec = static_cast<CCamera::TFormat>( formatMap().value(codecName) );
}

QString S60ImageCaptureSession::imageCaptureCodecDescription(const QString &codecName)
{
    return formatDescMap().key(formatMap().value(codecName));
}

QtMultimediaKit::EncodingQuality S60ImageCaptureSession::captureQuality() const
{
    if (m_imageQuality <= 1) {
        return QtMultimediaKit::VeryLowQuality;
    }

    return static_cast<QtMultimediaKit::EncodingQuality> (m_imageQuality / KSymbianImageQualityCoefficient);
}

void S60ImageCaptureSession::setCaptureQuality(QtMultimediaKit::EncodingQuality quality)
{
    switch (quality) {
        case QtMultimediaKit::VeryLowQuality:
            m_imageQuality = 1;
            break;
        default:
            m_imageQuality = quality * KSymbianImageQualityCoefficient;
            break;
    }

    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        if (camera)
            camera->SetJpegQuality(m_imageQuality);
        else
            setError(KErrNotReady);
    }
    else
        setError(KErrNotReady);

}

void S60ImageCaptureSession::setZoomFactor(qreal optical, qreal digital)
{
    if (m_cameraEngine && !m_cameraEngine->IsCameraReady()) {
        return;
    }

    if (m_cameraEngine && queryCurrentCameraInfo()) {
        CCamera *camera = m_cameraEngine->Camera();
        if (camera) {

            // Digital Zoom
            if (digital > m_info.iMaxZoom && digital <= m_info.iMaxDigitalZoom) {
                TRAPD(err, camera->SetDigitalZoomFactorL(digital));
                setError(err);
                if (err == KErrNone)
                    emit digitalZoomChanged(digital);

            // Optical Zoom
            } else if (optical >= m_info.iMinZoom && optical <= m_info.iMaxZoom) {
                TRAPD(err2, camera->SetZoomFactorL(optical));
                setError(err2);
                if (err2 == KErrNone)
                    emit opticalZoomChanged(optical);
            }
        }
    }
}

int S60ImageCaptureSession::zoomFactor()
{
    int factor = 0;

    if (m_cameraEngine)
        factor = m_cameraEngine->Camera()->ZoomFactor();

    return factor;
}

int S60ImageCaptureSession::digitalZoomFactor()
{
    int factor = 0;

    if (m_cameraEngine)
        factor = m_cameraEngine->Camera()->DigitalZoomFactor();

    return factor;
}

bool S60ImageCaptureSession::isFocusSupported()
{
    return m_cameraEngine->IsAutoFocusSupported();
}

void S60ImageCaptureSession::startFocus()
{
    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->StartFocusL());
        setError(err);
    }
    else
        setError(KErrNotReady);
}

void S60ImageCaptureSession::cancelFocus()
{
    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->FocusCancel());
        setError(err);
    }
    else
        setError(KErrNotReady);
}

int S60ImageCaptureSession::maximumZoom()
{
    if (queryCurrentCameraInfo()) {
        return m_info.iMaxZoom;
    } else {
        return 0;
    }
}

int S60ImageCaptureSession::minZoom()
{
    if (queryCurrentCameraInfo()) {
        return m_info.iMinZoom;
    } else {
        return 0;
    }
}

int S60ImageCaptureSession::maxDigitalZoom()
{
    if (queryCurrentCameraInfo()) {
        return m_info.iMaxDigitalZoom;
    } else {
        return 0;
    }
}

void S60ImageCaptureSession::setFlashMode(QCameraExposure::FlashModes mode)
{
    TRAPD(err, setFlashModeL(mode));
    setError(err);
}

void S60ImageCaptureSession::setFlashModeL(QCameraExposure::FlashModes mode)
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
                break;
        }
    }
    else
        setError(KErrNotReady);
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

            default:
                return QCameraExposure::FlashAuto; // Most probable default
        }
    }
    else
        setError(KErrNotReady);

    return QCameraExposure::FlashOff;
}

QCameraExposure::FlashModes S60ImageCaptureSession::supportedFlashModes()
{
    QCameraExposure::FlashModes modes = QCameraExposure::FlashOff;

    if (queryCurrentCameraInfo()) {
        TInt supportedModes = m_info.iFlashModesSupported;

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
    TInt supportedModes = m_info.iExposureModesSupported;

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
    TRAPD(err, setExposureModeL(mode));
    setError(err);
}

void S60ImageCaptureSession::setExposureModeL( QCameraExposure::ExposureMode mode)
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
                break;
        }
    }
    else
        setError(KErrNotReady);
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
        setError(err);
    }
    else
        setError(KErrNotReady);
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
    TRAPD(err, setWhiteBalanceModeL(mode));
    setError(err);
}

void S60ImageCaptureSession::setWhiteBalanceModeL( QCameraImageProcessing::WhiteBalanceMode mode)
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
            case  QCameraImageProcessing::WhiteBalanceIncandescent:
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
                break;
        }
    }
    else
        setError(KErrNotReady);
}

bool S60ImageCaptureSession::isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceMode mode) const
{
    if (m_cameraEngine) {
        TInt supportedModes = m_info.iWhiteBalanceModesSupported;
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
            case QCameraImageProcessing::WhiteBalanceIncandescent:
                if (supportedModes & CCamera::EWBTungsten)
                    return true;
                else
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

// End of file

