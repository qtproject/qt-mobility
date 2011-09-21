/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "s60imagecapturesettings.h"
#include "s60imagecapturesession.h"
#include "s60cameraconstants.h"
#include "s60cameraadvsettings.h"

using namespace S60CameraConstants;

static QVideoFrame::PixelFormat symbian2QVideoFrameFormat(const CCamera::TFormat format)
{
    switch (format) {
    case CCamera::EFormatExif:
        return QVideoFrame::Format_Jpeg;
    default:
        return QVideoFrame::Format_Invalid;
    }
}

static CCamera::TFormat qVideoFrame2SymbianFormat(const QVideoFrame::PixelFormat format)
{
    switch (format) {
    case QVideoFrame::Format_Jpeg:
        return CCamera::EFormatExif;
    default:
        return CCamera::EFormatExif; // Default
    }
}

S60ImageCaptureSettings::S60ImageCaptureSettings(S60ImageCaptureSession *session,
                                                 QObject *parent):
    QObject(parent),
    m_session(session),
    m_cameraEngine(0),
    m_advancedSettings(0),
    m_cameraInfo(0),
    m_activeDeviceIndex(KDefaultCameraDevice),
    m_captureDestination(KDefaultCaptureDestination),
    m_currentCodec(QString()),
    m_captureSize(QSize()),
    m_symbianImageQuality(QtMultimediaKit::HighQuality * KSymbianImageQualityCoefficient),
    m_bufferCaptureFormat(KDefaultBufferCaptureQtFormat),
    m_captureSettingsSet(false),
    m_requestedFocusMode(KDefaultFocusMode),
    m_requestedDigitalZoomFactor(KDefaultDigitalZoom),
    m_requestedFlashMode(KDefaultFlashMode),
    m_requestedExposureMode(KDefaultExposureMode),
    m_requestedWhiteBalanceMode(KDefaultWhiteBalanceMode),
    m_requestedContrast(KDefaultProcessingParameter),
    m_requestedBrightness(KDefaultProcessingParameter),
    m_requestedSaturation(KDefaultProcessingParameter),
    m_requestedSharpening(KDefaultProcessingParameter),
    m_requestedDenoising(KDefaultProcessingParameter),
    m_requestedIsoSensitivity(KDefaultExposureParameter),
    m_requestedAperture(KDefaultExposureParameter),
    m_requestedShutterSpeed(KDefaultExposureParameter),
    m_reauestedExposureCompensation(KDefaultExposureParameter)
{
    // Define supported image codecs
    m_supportedImageCodecs << "image/jpeg";
    // Define supported buffer capture formats
    m_supportedBufferCaptureFormats << QVideoFrame::Format_Jpeg;

    initializeImageCaptureSettings();
}

S60ImageCaptureSettings::~S60ImageCaptureSettings()
{
    // Delete AdvancedSettings (Should already be destroyed by CameraControl)
    deleteAdvancedSettings();

    m_formats.clear();
    m_supportedImageCodecs.clear();
}

CCamera::TFormat S60ImageCaptureSettings::defaultImageFormat()
{
    // Primary Camera
    if (m_activeDeviceIndex == 0)
        return KDefaultImageFormatPrimaryCam;
    // Secondary Camera or other
    else
        return KDefaultImageFormatSecondaryCam;
}

void S60ImageCaptureSettings::deleteAdvancedSettings()
{
    if (m_advancedSettings) {
        delete m_advancedSettings;
        m_advancedSettings = 0;
    }
}

void S60ImageCaptureSettings::setCurrentDevice(TInt deviceindex)
{
    m_activeDeviceIndex = deviceindex;
}

void S60ImageCaptureSettings::notifySettingsSet()
{
    m_captureSettingsSet = true;
}

void S60ImageCaptureSettings::resetSettings(CCameraEngine *engine, const bool errorHandling)
{
    deleteAdvancedSettings();

    m_cameraEngine = engine;

    int err = KErrNone;
    m_advancedSettings = S60CameraAdvSettings::New(err, this, m_cameraEngine);
    if (err == KErrNotSupported) {
        m_advancedSettings = 0;
#ifndef S60_31_PLATFORM // Post S60 3.1 Platform
        // Adv. settings may not be supported for other than the Primary Camera
        if (m_cameraEngine->CurrentCameraIndex() == 0)
            m_session->setError(err, tr("Unexpected camera error."));
#endif // !S60_31_PLATFORM
    } else if (err != KErrNone) { // Other errors
        m_advancedSettings = 0;
        qWarning("Failed to create camera settings handler.");
        if (errorHandling)
            emit  m_session->setError(KErrNotFound, tr("Failed to recover from error."));
        else
            m_session->setError(err, tr("Unexpected camera error."));
        return;
    }

    if (m_advancedSettings) {
        if (m_cameraEngine)
            m_cameraEngine->SetAdvancedObserver(m_advancedSettings);
        else
            m_session->setError(KErrNotReady, tr("Unexpected camera error."));

        connect(m_advancedSettings, SIGNAL(error(int, const QString&)),
            m_session, SIGNAL(cameraError(int, const QString&)));
        connect(m_advancedSettings, SIGNAL(flashReady(bool)), this, SIGNAL(flashReady(bool)));
        connect(m_advancedSettings, SIGNAL(focusStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason)),
            this, SIGNAL(focusStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason)));
        connect(m_advancedSettings, SIGNAL(apertureChanged()), this, SIGNAL(apertureChanged()));
        connect(m_advancedSettings, SIGNAL(apertureRangeChanged()), this, SIGNAL(apertureRangeChanged()));
        connect(m_advancedSettings, SIGNAL(shutterSpeedChanged()), this, SIGNAL(shutterSpeedChanged()));
        connect(m_advancedSettings, SIGNAL(isoSensitivityChanged()), this, SIGNAL(isoSensitivityChanged()));
        connect(m_advancedSettings, SIGNAL(evChanged()), this, SIGNAL(evChanged()));
    }

    updateImageCaptureFormats();
}

void S60ImageCaptureSettings::applyCameraSettings()
{
    // Zoom Factor
    if (!qFuzzyCompare(m_requestedDigitalZoomFactor, qreal(1.0))) {
        TRAPD(err, doSetZoomFactorL(KDefaultOpticalZoom, m_requestedDigitalZoomFactor));
        if (err)
            m_session->setError(err, tr("Failed to apply zoom factor."));
    }

    // FocusMode
    if (focusMode() != m_requestedFocusMode &&
        isFocusModeSupported(m_requestedFocusMode))
        setFocusMode(m_requestedFocusMode);

    // FlashMode
    if (flashMode() != m_requestedFlashMode &&
        supportedFlashModes() & m_requestedFlashMode)
        setFlashMode(m_requestedFlashMode);

    // ExposureMode
    if (exposureMode() != m_requestedExposureMode &&
        isExposureModeSupported(m_requestedExposureMode))
        setExposureMode(m_requestedExposureMode);

    // WhiteBalanceMode
    if (whiteBalanceMode() != m_requestedWhiteBalanceMode &&
        isWhiteBalanceModeSupported(m_requestedWhiteBalanceMode))
        setWhiteBalanceMode(m_requestedWhiteBalanceMode);

    // Contrast
    if (contrast() != m_requestedContrast && areContrastAndBrightnessSupported())
        setContrast(m_requestedContrast);

    // Brightness
    if (brightness() != m_requestedBrightness && areContrastAndBrightnessSupported())
        setBrightness(m_requestedBrightness);

    // Saturation
    if (saturation() != m_requestedSaturation && isSaturationSupported())
        setSaturation(m_requestedSaturation);

    // Sharpening
    if (sharpening() != m_requestedSharpening && isSharpeningSupported())
        setSharpening(m_requestedSharpening);

    // Denoising
    if (denoising() != m_requestedDenoising && isDenoisingSupported())
        setDenoising(m_requestedDenoising);

    // ISO Sensitivity
    if (m_requestedIsoSensitivity != 0 &&
        isoSensitivity() != m_requestedIsoSensitivity &&
        supportedIsoSensitivities().contains(m_requestedIsoSensitivity))
        setIsoSensitivity(m_requestedIsoSensitivity);

    // Aperture
    if (aperture() != m_requestedAperture &&
        supportedApertures().contains(m_requestedAperture))
        setAperture(m_requestedAperture);

    // ShutterSpeed
    if (shutterSpeed() != m_requestedShutterSpeed &&
        supportedShutterSpeeds().contains(m_requestedShutterSpeed))
        setShutterSpeed(m_requestedShutterSpeed);

    // ExposureCompensation
    if (exposureCompensation() != m_reauestedExposureCompensation &&
        supportedExposureCompensations().contains(m_reauestedExposureCompensation))
        setExposureCompensation(m_reauestedExposureCompensation);
}

void S60ImageCaptureSettings::initializeImageCaptureSettings()
{
    if (m_captureSettingsSet)
        return;

    m_currentCodec = KDefaultImageCodec;
    m_captureSize = QSize(-1, -1);
    m_currentFormat = defaultImageFormat();

    // Resolution
    if (m_cameraEngine) {
        QList<QSize> resolutions = supportedImageResolutionsForCodec(imageCodec());
        foreach (QSize reso, resolutions) {
            if ((reso.width() * reso.height()) > (m_captureSize.width() * m_captureSize.height()))
                m_captureSize = reso;
        }
    } else {
        m_captureSize = KDefaultImageResolution;
    }

    m_symbianImageQuality = KJpegQualityDefault;
}

/*
 * This function selects proper format to be used for the captured image based
 * on the requested image codec.
 */
CCamera::TFormat S60ImageCaptureSettings::selectFormatForCodec(const QString &codec)
{
    CCamera::TFormat format = CCamera::EFormatMonochrome;

    if (codec == "image/jpg" || codec == "image/jpeg") {
        // Primary Camera
        if (m_activeDeviceIndex == 0)
            format = KDefaultImageFormatPrimaryCam;
        // Secondary Camera or other
        else
            format = KDefaultImageFormatSecondaryCam;

        return format;
    }

    m_session->setError(KErrNotSupported, tr("Failed to select color format to be used with image codec."));
    return format;
}

/*
 * Queries camera properties
 * Results are returned to member variable m_info
 *
 * @return boolean indicating if querying the info was a success
 */
bool S60ImageCaptureSettings::queryCurrentCameraInfo()
{
    if (m_cameraEngine) {
        m_cameraInfo = m_cameraEngine->CameraInfo();
        return true;
    }
    return false;
}

void S60ImageCaptureSettings::setCaptureDestination(const QCameraImageCapture::CaptureDestinations destination)
{
    // Reset image format if needed
    if (destination & QCameraImageCapture::CaptureToFile) {
        if (m_currentFormat != defaultImageFormat()) {
            m_currentFormat = defaultImageFormat();
            emit bufferCaptureFormatChanged(symbian2QVideoFrameFormat(m_currentFormat));
        }
    }

    if (symbian2QVideoFrameFormat(m_currentFormat) == QVideoFrame::Format_Invalid &&
        destination & QCameraImageCapture::CaptureToBuffer) {
        if (destination == QCameraImageCapture::CaptureToBuffer) {
            // Only buffer capture requested
#ifdef SYMBIAN_3_PLATFORM
            // Change buffer format to supported
            m_currentFormat = KDefaultBufferCaptureSymbianFormat;
            emit bufferCaptureFormatChanged(symbian2QVideoFrameFormat(m_currentFormat));
#else // Older platforms
            // Notify buffer capture is not supported (camera can only provide bitmaps)
            // Future improvement: Implement bitmap encoding to a compressed image buffer
            m_session->setError(KErrNotSupported, tr("Buffer image capture is not supported with this setup."));
#endif // SYMBIAN_3_PLATFORM
        } else {
            // Both file and buffer requested, unset BufferCapture flag
            m_captureDestination = destination & ~QCameraImageCapture::CaptureToBuffer;
            emit destinationChanged(m_captureDestination);
            m_session->setError(KErrNotSupported, tr("Buffer image capture is not supported with this setup."));
        }
    } else {
        m_captureDestination = destination;
        emit destinationChanged(m_captureDestination);
    }
}

QCameraImageCapture::CaptureDestinations S60ImageCaptureSettings::destination() const
{
    return m_captureDestination;
}

QSize S60ImageCaptureSettings::imageResolution() const
{
    return m_captureSize;
}

void S60ImageCaptureSettings::setImageResolution(const QSize &size)
{
    QList<QSize> supportedSizes = supportedImageResolutionsForCodec(m_currentCodec);
    if (!size.isEmpty()) {
        if (!supportedSizes.isEmpty()) {
            if (!supportedSizes.contains(size)) {
                m_session->setError(KErrNotSupported, tr("Requested resolution is not supported."), true);
                return;
            }
        }
        m_captureSize = size;

    } else {
        // Quality defines the resolution
        switch (m_symbianImageQuality) {
        case KJpegQualityVeryLow:
            m_captureSize = supportedSizes.last();
            break;
        case KJpegQualityLow:
            if (supportedSizes.count() > 3)
                m_captureSize = supportedSizes.at(supportedSizes.count() - 2);
            else
                m_captureSize = supportedSizes.last();
            break;
        case KJpegQualityNormal:
            m_captureSize = supportedSizes.at(supportedSizes.count() / 2);
            break;
        case KJpegQualityHigh:
            if (supportedSizes.count() > 3)
                m_captureSize = supportedSizes.at(1);
            else
                m_captureSize = supportedSizes.first();
            break;
        case KJpegQualityVeryHigh:
            m_captureSize = supportedSizes.first();
            break;
        default:
            // Best as default
            m_captureSize = supportedSizes.first();
            break;
        }
    }
}

QList<QSize> S60ImageCaptureSettings::sortResolutions(QList<QSize> resolutions)
{
    if (resolutions.count() < 2)
        return resolutions;

    // Sort resolutions
    QList<QSize> smaller;
    QList<QSize> larger;

    // Naive midpoint selection
    QSize mid = resolutions.at(resolutions.count() / 2);
    int midMultiplied = mid.width() * mid.height();

    foreach (QSize resolution, resolutions) {
        if (resolution.width() * resolution.height() <= midMultiplied) {
            if (resolution != mid)
                smaller << resolution;
        } else {
            larger << resolution;
        }
    }

    // Sort to descending order
    return (sortResolutions(larger) << mid) + sortResolutions(smaller);
}

QList<QSize> S60ImageCaptureSettings::supportedImageResolutionsForCodec(const QString &codecName)
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
        if (camera) {
            for (int i = 0; i < m_cameraInfo->iNumImageSizesSupported; i++) {
                camera->EnumerateCaptureSizes(imageSize, i, format);
                list << QSize(imageSize.iWidth, imageSize.iHeight); // Add resolution to the list
            }
        }
    }

#ifdef Q_CC_NOKIAX86 // Emulator
    // Add some for testing purposes
    list << QSize(50, 50);
    list << QSize(100, 100);
    list << QSize(800,600);
#endif

    // Sort list (from large to small resolution)
    if (list.count() > 1) {
        // First check if resolutions already are in correct order
        bool isDescending = true;
        for (int i = 0; i < list.count() - 1; ++i) {
            if (list.at(i).width() * list.at(i).height() <
                list.at(i+1).width() * list.at(i+1).height())
                isDescending = false;
        }
        if (!isDescending)
            list = sortResolutions(list);
    }

    return list;
}

QMap<QString, QString> S60ImageCaptureSettings::codecDescriptionMap()
{
    QMap<QString, QString> formats;
    formats.insert("image/jpg", "JPEG image codec");
    return formats;
}

QStringList S60ImageCaptureSettings::supportedImageCodecs()
{
    return m_supportedImageCodecs;
}

void S60ImageCaptureSettings::updateImageCaptureFormats()
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

QString S60ImageCaptureSettings::imageCodec()
{
    return m_currentCodec;
}

void S60ImageCaptureSettings::setImageCodec(const QString &codecName)
{
    if (!codecName.isEmpty()) {
        if (supportedImageCodecs().contains(codecName, Qt::CaseInsensitive) ||
            codecName == QLatin1String("image/jpg")) {
            m_currentCodec = codecName;
            m_currentFormat = selectFormatForCodec(m_currentCodec);
        } else {
            m_session->setError(KErrNotSupported, tr("Requested image codec is not supported"), true);
        }
    } else {
        m_currentCodec = KDefaultImageCodec;
        m_currentFormat = selectFormatForCodec(m_currentCodec);
    }
}

QString S60ImageCaptureSettings::imageCodecDescription(const QString &codecName)
{
    QString description = codecDescriptionMap().value(codecName);
    return description;
}

QtMultimediaKit::EncodingQuality S60ImageCaptureSettings::imageQuality() const
{
    switch (m_symbianImageQuality) {
    case KJpegQualityVeryLow:
        return QtMultimediaKit::VeryLowQuality;
    case KJpegQualityLow:
        return QtMultimediaKit::LowQuality;
    case KJpegQualityNormal:
        return QtMultimediaKit::NormalQuality;
    case KJpegQualityHigh:
        return QtMultimediaKit::HighQuality;
    case KJpegQualityVeryHigh:
        return QtMultimediaKit::VeryHighQuality;
    default:
        // Return normal as default
        return QtMultimediaKit::NormalQuality;
    }
}

void S60ImageCaptureSettings::setImageQuality(const QtMultimediaKit::EncodingQuality &quality)
{
    // Use sensible presets
    switch (quality) {
    case QtMultimediaKit::VeryLowQuality:
        m_symbianImageQuality = KJpegQualityVeryLow;
        break;
    case QtMultimediaKit::LowQuality:
        m_symbianImageQuality = KJpegQualityLow;
        break;
    case QtMultimediaKit::NormalQuality:
        m_symbianImageQuality = KJpegQualityNormal;
        break;
    case QtMultimediaKit::HighQuality:
        m_symbianImageQuality = KJpegQualityHigh;
        break;
    case QtMultimediaKit::VeryHighQuality:
        m_symbianImageQuality = KJpegQualityVeryHigh;
        break;
    default:
        // Accept integers between 0-100 which are cast to
        // QtMultimediaKit::EncodingQuality since image quality actually
        // defines Jpeg compression quality. Thus it makes sense to let user
        // define specific Jpeg compression qualitys in this way.
        if (quality > 0 && quality <= 100) {
            m_symbianImageQuality = int(quality);
        } else {
            m_session->setError(KErrNotSupported, tr("Requested image quality is not supported"), true);
        }
        break;
    }
}

void S60ImageCaptureSettings::commitJpegQuality() const
{
    if (m_cameraEngine->Camera())
        m_cameraEngine->Camera()->SetJpegQuality(m_symbianImageQuality);
    else
        m_session->setError(KErrNotReady, tr("Setting image quality failed."), true);
}

int S60ImageCaptureSettings::jpegQuality() const
{
    return m_symbianImageQuality;
}

QList<QVideoFrame::PixelFormat> S60ImageCaptureSettings::supportedBufferCaptureFormats() const
{
    return m_supportedBufferCaptureFormats;
}

void S60ImageCaptureSettings::setBufferCaptureFormat(const QVideoFrame::PixelFormat format)
{
    CCamera::TFormat symbianCaptureFormat = qVideoFrame2SymbianFormat(format);

    if (m_currentFormat != symbianCaptureFormat) {
        if (m_captureDestination != QCameraImageCapture::CaptureToBuffer) {
            // Unset FileCapture flag
            m_captureDestination = m_captureDestination & ~QCameraImageCapture::CaptureToFile;
            emit destinationChanged(m_captureDestination);
        }
    }

    if (m_bufferCaptureFormat != format) {
        m_bufferCaptureFormat = format;
        emit bufferCaptureFormatChanged(m_bufferCaptureFormat);
    }
}

CCamera::TFormat S60ImageCaptureSettings::currentSymbianFormat() const
{
    return m_currentFormat;
}

void S60ImageCaptureSettings::resetSymbianFormat()
{
    m_currentFormat = defaultImageFormat();
}

qreal S60ImageCaptureSettings::maximumZoom()
{
    qreal maxZoomFactor = 1.0;

    if (queryCurrentCameraInfo()) {
        maxZoomFactor = m_cameraInfo->iMaxZoomFactor;

        if (maxZoomFactor == 0.0 || maxZoomFactor == 1.0)
            return 1.0; // Not supported
        else
            return maxZoomFactor;
    } else {
        return 1.0;
    }
}

qreal S60ImageCaptureSettings::minZoom()
{
    qreal minZoomValue = 1.0;

    if (queryCurrentCameraInfo()) {
        minZoomValue = m_cameraInfo->iMinZoomFactor;
        if (minZoomValue == 0.0 || minZoomValue == 1.0)
            return 1.0; // Macro Zoom is not supported
        else
            return minZoomValue;
    } else {
        return 1.0;
    }
}

qreal S60ImageCaptureSettings::maxDigitalZoom()
{
    qreal maxDigitalZoomFactor = 1.0;

    if (queryCurrentCameraInfo()) {
        maxDigitalZoomFactor = m_cameraInfo->iMaxDigitalZoomFactor;
        return maxDigitalZoomFactor;
    } else {
        return 1.0;
    }
}

void S60ImageCaptureSettings::doSetZoomFactorL(qreal optical, qreal digital)
{
#if !defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER) & !defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER)
    // Convert Zoom Factor to Zoom Value if AdvSettings are not available
    int digitalSymbian = (digital * m_cameraInfo->iMaxDigitalZoom) / maxDigitalZoom();
    if (m_cameraInfo->iMaxDigitalZoom != 0 && digital == 1.0)
        digitalSymbian = 1; // Make sure zooming out to initial value if requested
#endif // !USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER & !USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER

    if (m_cameraEngine && !m_cameraEngine->IsCameraReady()) {
        if ((digital < qreal(1.0) && !qFuzzyCompare(digital, qreal(1.0))) ||
            (m_cameraInfo && digital > m_cameraInfo->iMaxDigitalZoomFactor &&
                !qFuzzyCompare(digital, m_cameraInfo->iMaxDigitalZoomFactor))) {
            m_session->setError(KErrNotSupported, tr("Requested digital zoom factor is not supported."));
            return;
        }

        // Save the requested zoom factor
        m_requestedDigitalZoomFactor = digital;
        return;
    }

    if (m_cameraEngine && queryCurrentCameraInfo()) {
        CCamera *camera = m_cameraEngine->Camera();
        if (camera) {

            // Optical Zoom
            if (!qFuzzyCompare(optical, qreal(1.0)) && !qFuzzyCompare(optical, qreal(0.0))) {
                m_session->setError(KErrNotSupported, tr("Requested optical zoom factor is not supported."));
                return;
            }

            // Digital Zoom (Smooth Zoom - Zoom value set in steps)
            if (digital != digitalZoomFactor()) {
                if ((digital > 1.0 || qFuzzyCompare(digital, qreal(1.0))) &&
                    digital <= m_cameraInfo->iMaxDigitalZoomFactor) {
#if defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER) | defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER)
                    if (m_advancedSettings) {
                        qreal currentZoomFactor = m_advancedSettings->digitalZoomFactorL();

                        QList<qreal> smoothZoomSetValues;
                        QList<qreal> factors = m_advancedSettings->supportedDigitalZoomFactors();
                        if (currentZoomFactor < digital) {
                            for (int i = 0; i < factors.count(); ++i) {
                                if (factors.at(i) > currentZoomFactor && factors.at(i) < digital)
                                    smoothZoomSetValues << factors.at(i);
                            }

                            for (int i = 0; i < smoothZoomSetValues.count(); i = i + KSmoothZoomStep)
                                m_advancedSettings->setDigitalZoomFactorL(smoothZoomSetValues[i]); // Using Zoom Factor

                        } else {
                            for (int i = 0; i < factors.count(); ++i) {
                                if (factors.at(i) < currentZoomFactor && factors.at(i) > digital)
                                    smoothZoomSetValues << factors.at(i);
                            }

                            for (int i = (smoothZoomSetValues.count() - 1); i >= 0; i = i - KSmoothZoomStep)
                                m_advancedSettings->setDigitalZoomFactorL(smoothZoomSetValues[i]); // Using Zoom Factor
                        }

                        // Set final value
                        m_advancedSettings->setDigitalZoomFactorL(digital);
                        m_requestedDigitalZoomFactor = digital;
                    } else {
                        m_session->setError(KErrNotReady, tr("Zooming failed."));
                    }
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
                    m_session->setError(KErrNotSupported, tr("Requested digital zoom factor is not supported."));
                    return;
                }
            }
        }
    } else {
        m_session->setError(KErrGeneral, tr("Unexpected camera error."));
    }
}

qreal S60ImageCaptureSettings::opticalZoomFactor()
{
    qreal factor = KDefaultOpticalZoom;

#if defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER) | defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER)
    if (m_advancedSettings && m_cameraEngine && m_cameraEngine->IsCameraReady()) {
        TRAPD(err, factor = m_advancedSettings->opticalZoomFactorL());
        if (err)
            return KDefaultOpticalZoom;
    }
#else // No advanced settigns
    if (m_cameraEngine && m_cameraInfo) {
        if (m_cameraEngine->Camera()) {
            if (m_cameraInfo->iMaxZoom != 0)
                factor = (m_cameraEngine->Camera()->ZoomFactor()* maximumZoom()) / m_cameraInfo->iMaxZoom;
            else
                factor = KDefaultOpticalZoom;
        }
    }
#endif // USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER

    if (factor == 0.0) // I.e. Not supported
        factor = KDefaultOpticalZoom;

    return factor;
}

qreal S60ImageCaptureSettings::digitalZoomFactor()
{
    qreal factor = KDefaultDigitalZoom;

#if defined(USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER) | defined(USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER)
    if (m_advancedSettings && m_cameraEngine && m_cameraEngine->IsCameraReady()) {
        TRAPD(err, factor = m_advancedSettings->digitalZoomFactorL());
        if (err)
            return KDefaultDigitalZoom;
    } else {
        return m_requestedDigitalZoomFactor;
    }
#else // No advanced settigns
    if (m_cameraEngine && m_cameraInfo) {
        if (m_cameraEngine->Camera()) {
            if (m_cameraInfo->iMaxDigitalZoom != 0)
                factor = (m_cameraEngine->Camera()->DigitalZoomFactor()* maxDigitalZoom()) / m_cameraInfo->iMaxDigitalZoom;
            else
                factor = KDefaultDigitalZoom;
        }
    }
#endif // USE_S60_32_ECAM_ADVANCED_SETTINGS_HEADER | USE_S60_50_ECAM_ADVANCED_SETTINGS_HEADER

    if (factor == 0.0) // I.e. Not supported
        factor = KDefaultDigitalZoom;

    return factor;
}

void S60ImageCaptureSettings::startFocusing()
{
    if (m_advancedSettings)
        m_advancedSettings->startFocusing();
    else
        emit focusStatusChanged(QCamera::Unlocked, QCamera::LockFailed);
}

void S60ImageCaptureSettings::cancelFocusing()
{
    if (m_advancedSettings)
        m_advancedSettings->cancelFocusing();
    else
        emit focusStatusChanged(QCamera::Unlocked, QCamera::LockFailed);
}

QCameraFocus::FocusMode S60ImageCaptureSettings::focusMode()
{
    return (m_advancedSettings && m_cameraEngine && m_cameraEngine->IsCameraReady()) ?
        m_advancedSettings->focusMode() : m_requestedFocusMode;
}

void S60ImageCaptureSettings::setFocusMode(QCameraFocus::FocusMode mode)
{
    if (m_advancedSettings && m_cameraEngine && m_cameraEngine->IsCameraReady())
        m_advancedSettings->setFocusMode(mode);
    m_requestedFocusMode = mode;
}

bool S60ImageCaptureSettings::isFocusModeSupported(QCameraFocus::FocusMode mode) const
{
    QCameraFocus::FocusModes supportedFocusModes;
    if (m_advancedSettings)
        supportedFocusModes = m_advancedSettings->supportedFocusModes();
    return supportedFocusModes & mode;
}

void S60ImageCaptureSettings::setFlashMode(QCameraExposure::FlashModes mode)
{
    TRAPD(err, doSetFlashModeL(mode));
    if (err == KErrInUse) // Camera not reserved yet
        m_session->setError(KErrNotSupported, tr("Cannot set flash mode before camera is started."));
    else
        m_session->setError(err, tr("Failed to set flash mode."));
}

void S60ImageCaptureSettings::doSetFlashModeL(QCameraExposure::FlashModes mode)
{
    if (m_cameraEngine && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady()) {
        switch(mode) {
        case QCameraExposure::FlashOff:
            m_cameraEngine->Camera()->SetFlashL(CCamera::EFlashNone);
            break;
        case QCameraExposure::FlashAuto:
            m_cameraEngine->Camera()->SetFlashL(CCamera::EFlashAuto);
            break;
        case QCameraExposure::FlashOn:
            m_cameraEngine->Camera()->SetFlashL(CCamera::EFlashForced);
            break;
        case QCameraExposure::FlashRedEyeReduction:
            m_cameraEngine->Camera()->SetFlashL(CCamera::EFlashRedEyeReduce);
            break;
        case QCameraExposure::FlashFill:
            m_cameraEngine->Camera()->SetFlashL(CCamera::EFlashFillIn);
            break;
#ifndef S60_31_PLATFORM
        case QCameraExposure::FlashTorch:
            m_cameraEngine->Camera()->SetFlashL(CCamera::EFlashVideoLight);
            break;
#endif // S60_31_PLATFORM
        case QCameraExposure::FlashSlowSyncFrontCurtain:
            m_cameraEngine->Camera()->SetFlashL(CCamera::EFlashSlowFrontSync);
            break;
        case QCameraExposure::FlashSlowSyncRearCurtain:
            m_cameraEngine->Camera()->SetFlashL(CCamera::EFlashSlowRearSync);
            break;
        case QCameraExposure::FlashManual:
            m_cameraEngine->Camera()->SetFlashL(CCamera::EFlashManual);
            break;
        default:
            m_session->setError(KErrNotSupported, tr("Requested flash mode is not suported"));
            break;
        }
    }

    m_requestedFlashMode = mode;
}

QCameraExposure::FlashModes S60ImageCaptureSettings::flashMode()
{
    if (m_cameraEngine && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady()) {
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
        case CCamera::EFlashSlowFrontSync:
            return QCameraExposure::FlashSlowSyncFrontCurtain;
        case CCamera::EFlashSlowRearSync:
            return QCameraExposure::FlashSlowSyncRearCurtain;
#ifndef S60_31_PLATFORM
        case CCamera::EFlashVideoLight:
            return QCameraExposure::FlashTorch;
#endif // S60_31_PLATFORM
        case CCamera::EFlashManual:
            return QCameraExposure::FlashManual;
        default:
            return QCameraExposure::FlashAuto; // Most probable default
        }
    } else {
        return m_requestedFlashMode;
    }
}

QCameraExposure::FlashModes S60ImageCaptureSettings::supportedFlashModes()
{
    QCameraExposure::FlashModes modes = QCameraExposure::FlashOff;

    if (queryCurrentCameraInfo()) {
        TInt supportedModes = m_cameraInfo->iFlashModesSupported;

        if (supportedModes == 0)
            return modes;

        // Flash off always supported
        modes |= QCameraExposure::FlashOff;
        if (supportedModes & CCamera::EFlashForced)
             modes |= QCameraExposure::FlashOn;
        if (supportedModes & CCamera::EFlashAuto)
             modes |= QCameraExposure::FlashAuto;
        if (supportedModes & CCamera::EFlashFillIn)
             modes |= QCameraExposure::FlashFill;
        if (supportedModes & CCamera::EFlashRedEyeReduce)
             modes |= QCameraExposure::FlashRedEyeReduction;
        if (supportedModes & CCamera::EFlashSlowFrontSync)
             modes |= QCameraExposure::FlashSlowSyncFrontCurtain;
        if (supportedModes & CCamera::EFlashSlowRearSync)
             modes |= QCameraExposure::FlashSlowSyncRearCurtain;
#ifndef S60_31_PLATFORM
        if (supportedModes & CCamera::EFlashVideoLight)
             modes |= QCameraExposure::FlashTorch;
#endif // S60_31_PLATFORM
        if (supportedModes & CCamera::EFlashManual)
             modes |= QCameraExposure::FlashManual;
    }

    return modes;
}

bool S60ImageCaptureSettings::isFlashReady() const
{
    return m_advancedSettings ? m_advancedSettings->isFlashReady() : false;
}

QCameraExposure::ExposureMode S60ImageCaptureSettings::exposureMode()
{
    if (m_cameraEngine && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady()) {
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
    } else {
        return m_requestedExposureMode;
    }
}

bool S60ImageCaptureSettings::isExposureModeSupported(QCameraExposure::ExposureMode mode) const
{
    TInt supportedModes = m_cameraInfo->iExposureModesSupported;

    if (supportedModes == 0)
        return false;

    switch (mode) {
    case QCameraExposure::ExposureManual:
        if (supportedModes & CCamera::EExposureManual)
            return true;
        else
            return false;
    case QCameraExposure::ExposureAuto:
        return true; // Always supported
    case QCameraExposure::ExposureNight:
        if (supportedModes & CCamera::EExposureNight)
            return true;
        else
            return false;
    case QCameraExposure::ExposureBacklight:
        if (supportedModes & CCamera::EExposureBacklight)
            return true;
        else
            return false;
    case QCameraExposure::ExposureSports:
        if (supportedModes & CCamera::EExposureSport)
            return true;
        else
            return false;
    case QCameraExposure::ExposureSnow:
        if (supportedModes & CCamera::EExposureSnow)
            return true;
        else
            return false;
    case QCameraExposure::ExposureBeach:
        if (supportedModes & CCamera::EExposureBeach)
            return true;
        else
            return false;
    default:
        return false;
    }
}

void S60ImageCaptureSettings::setExposureMode(QCameraExposure::ExposureMode mode)
{
    TRAPD(err, doSetExposureModeL(mode));
    m_session->setError(err, tr("Failed to set exposure mode."));
}

void S60ImageCaptureSettings::doSetExposureModeL( QCameraExposure::ExposureMode mode)
{
    if (m_cameraEngine && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady()) {
        switch(mode) {
        case QCameraExposure::ExposureManual:
            m_cameraEngine->Camera()->SetExposureL(CCamera::EExposureManual);
            break;
        case QCameraExposure::ExposureAuto:
            m_cameraEngine->Camera()->SetExposureL(CCamera::EExposureAuto);
            break;
        case QCameraExposure::ExposureNight:
            m_cameraEngine->Camera()->SetExposureL(CCamera::EExposureNight);
            break;
        case QCameraExposure::ExposureBacklight:
            m_cameraEngine->Camera()->SetExposureL(CCamera::EExposureBacklight);
            break;
        case QCameraExposure::ExposureSports:
            m_cameraEngine->Camera()->SetExposureL(CCamera::EExposureSport);
            break;
        case QCameraExposure::ExposureSnow:
            m_cameraEngine->Camera()->SetExposureL(CCamera::EExposureSnow);
            break;
        case QCameraExposure::ExposureBeach:
            m_cameraEngine->Camera()->SetExposureL(CCamera::EExposureBeach);
            break;
        case QCameraExposure::ExposureLargeAperture:
        case QCameraExposure::ExposureSmallAperture:
            break;
        case QCameraExposure::ExposurePortrait:
        case QCameraExposure::ExposureSpotlight:
        default:
            m_session->setError(KErrNotSupported, tr("Requested exposure mode is not suported"));
            break;
        }
    }

    m_requestedExposureMode = mode;
}

QCameraExposure::MeteringMode S60ImageCaptureSettings::meteringMode()
{
    return QCameraExposure::MeteringSpot;
}

void S60ImageCaptureSettings::setMeteringMode(QCameraExposure::MeteringMode mode)
{
    m_session->setError(KErrNotSupported, tr("Setting metering mode is not supported."));
}

bool S60ImageCaptureSettings::isMeteringModeSupported(QCameraExposure::MeteringMode mode) const
{
    return false;
}

bool S60ImageCaptureSettings::areContrastAndBrightnessSupported() const
{
#ifdef SYMBIAN_3_PLATFORM
    return true;
#else // Pre-Symbian^3
    if (m_cameraEngine && m_cameraEngine->CurrentCameraIndex() == 0)
        return true;
    else
        return false;
#endif // SYMBIAN_3_PLATFORM
}

int S60ImageCaptureSettings::contrast() const
{
    if (m_cameraEngine && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        return m_cameraEngine->Camera()->Contrast();
    else
        return m_requestedContrast;
}

void S60ImageCaptureSettings::setContrast(int value)
{
    if (m_cameraEngine && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady()) {
        TRAPD(err, m_cameraEngine->Camera()->SetContrastL(value));
        m_session->setError(err, tr("Failed to set contrast."));
    }

    m_requestedContrast = value;
}

int S60ImageCaptureSettings::brightness() const
{
    if (m_cameraEngine && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        return m_cameraEngine->Camera()->Brightness();
    else
        return m_requestedBrightness;
}

void S60ImageCaptureSettings::setBrightness(int value)
{
    if (m_cameraEngine && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady()) {
        TRAPD(err, m_cameraEngine->Camera()->SetBrightnessL(value));
        m_session->setError(err, tr("Failed to set brightness."));
    }

    m_requestedBrightness = value;
}

bool S60ImageCaptureSettings::isSaturationSupported() const
{
    return m_advancedSettings ? m_advancedSettings->isSaturationSupported() : false;
}

int S60ImageCaptureSettings::saturation() const
{
    return (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        ? m_advancedSettings->saturation() : m_requestedSaturation;
}

void S60ImageCaptureSettings::setSaturation(int value)
{
    if (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        m_advancedSettings->setSaturation(value);
    m_requestedSaturation = value;
}

bool S60ImageCaptureSettings::isSharpeningSupported() const
{
    return m_advancedSettings ? m_advancedSettings->isSharpeningSupported() : false;
}

int S60ImageCaptureSettings::sharpening() const
{
    return (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        ? m_advancedSettings->sharpeningLevel() : m_requestedSharpening;
}

void S60ImageCaptureSettings::setSharpening(int value)
{
    if (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        m_advancedSettings->setSharpeningLevel(value);
    m_requestedSharpening = value;
}

bool S60ImageCaptureSettings::isDenoisingSupported() const
{
    return m_advancedSettings ? m_advancedSettings->isDenoisingSupported() : false;
}

int S60ImageCaptureSettings::denoising() const
{
    return (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        ? m_advancedSettings->denoising() : m_requestedDenoising;
}

void S60ImageCaptureSettings::setDenoising(int value)
{
    if (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        m_advancedSettings->setDenoising(value);
    m_requestedDenoising = value;
}

 QCameraImageProcessing::WhiteBalanceMode S60ImageCaptureSettings::whiteBalanceMode()
{
    if (m_cameraEngine && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady()) {
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
    } else {
        return m_requestedWhiteBalanceMode;
    }
}

void S60ImageCaptureSettings::setWhiteBalanceMode( QCameraImageProcessing::WhiteBalanceMode mode)
{
    TRAPD(err, doSetWhiteBalanceModeL(mode));
    m_session->setError(err, tr("Failed to set white balance mode."));
}

void S60ImageCaptureSettings::doSetWhiteBalanceModeL( QCameraImageProcessing::WhiteBalanceMode mode)
{
    if (m_cameraEngine && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady()) {
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
            m_session->setError(KErrNotSupported, tr("Requested white balance mode is not suported"));
            break;
        }
    }

    m_requestedWhiteBalanceMode = mode;
}

bool S60ImageCaptureSettings::isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceMode mode) const
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
            // Always supported
            return true;
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

int S60ImageCaptureSettings::isoSensitivity() const
{
    return (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        ? m_advancedSettings->isoSensitivity() : m_requestedIsoSensitivity;
}

bool S60ImageCaptureSettings::setIsoSensitivity(int iso)
{
    if (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        m_advancedSettings->setManualIsoSensitivity(iso);
    m_requestedIsoSensitivity = iso;
    return true;
}

void S60ImageCaptureSettings::setAutoIsoSensitivity()
{
    if (m_advancedSettings)
        m_advancedSettings->setAutoIsoSensitivity();
}

QList<int> S60ImageCaptureSettings::supportedIsoSensitivities() const
{
    QList<int> supportedIsoValues;
    TRAPD(err, supportedIsoValues = m_advancedSettings->supportedIsoSensitivitiesL());
    Q_UNUSED(err); // Do not notify error while querying supported values
    return supportedIsoValues;
}

qreal S60ImageCaptureSettings::aperture() const
{
    return (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        ? m_advancedSettings->aperture() : m_requestedAperture;
}

bool S60ImageCaptureSettings::setAperture(qreal aperture)
{
    if (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        m_advancedSettings->setManualAperture(aperture);
    m_requestedAperture = aperture;
    return true;
}

QList<qreal> S60ImageCaptureSettings::supportedApertures() const
{
    return m_advancedSettings->supportedApertures();
}

qreal S60ImageCaptureSettings::shutterSpeed() const
{
    return (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        ? m_advancedSettings->shutterSpeed() : m_requestedShutterSpeed;
}

bool S60ImageCaptureSettings::setShutterSpeed(qreal seconds)
{
    if (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        m_advancedSettings->setManualShutterSpeed(seconds);
    m_requestedShutterSpeed = seconds;
    return true;
}

QList<qreal> S60ImageCaptureSettings::supportedShutterSpeeds() const
{
    return m_advancedSettings->supportedShutterSpeeds();
}

qreal S60ImageCaptureSettings::exposureCompensation() const
{
    return (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        ? m_advancedSettings->exposureCompensation() : m_reauestedExposureCompensation;
}

bool S60ImageCaptureSettings::setExposureCompensation(qreal ev)
{
    if (m_advancedSettings && m_cameraEngine->Camera() && m_cameraEngine->IsCameraReady())
        m_advancedSettings->setExposureCompensation(ev);
    m_reauestedExposureCompensation = ev;
    return true;
}

QList<qreal> S60ImageCaptureSettings::supportedExposureCompensations() const
{
    return m_advancedSettings->supportedExposureCompensationValues();
}

/*
 * ====================
 * S60 3.1 AutoFocosing
 * ====================
 */
bool S60ImageCaptureSettings::isFocusSupported() const
{
    return m_cameraEngine->IsAutoFocusSupported();
}

void S60ImageCaptureSettings::startFocus()
{
    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->StartFocusL());
        if (!err)
            emit focusStatusChanged(QCamera::Searching, QCamera::UserRequest);
        else
            emit focusStatusChanged(QCamera::Unlocked, QCamera::LockFailed);
    } else {
        m_session->setError(KErrNotReady, tr("Unexpected camera error."));
    }
}

void S60ImageCaptureSettings::cancelFocus()
{
    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->FocusCancel());
        if (!err)
            emit focusStatusChanged(QCamera::Unlocked, QCamera::UserRequest);
        else
            emit focusStatusChanged(QCamera::Unlocked, QCamera::LockFailed);
    } else {
        m_session->setError(KErrNotReady, tr("Unexpected camera error."));
    }
}

void S60ImageCaptureSettings::notifyFocusComplete()
{
    emit focusStatusChanged(QCamera::Locked, QCamera::LockAcquired);
}

// End of file

