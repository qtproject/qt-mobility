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

#ifndef S60IMAGECAPTURESETTINGS_H
#define S60IMAGECAPTURESETTINGS_H

#include <qcamera.h>
#include <qcamerafocus.h>
#include <qcameraimagecapture.h>
#include <qvideoframe.h>

#include <ecam.h>

QT_USE_NAMESPACE

QT_FORWARD_DECLARE_CLASS(S60ImageCaptureSession)
QT_FORWARD_DECLARE_CLASS(CCameraEngine)
QT_FORWARD_DECLARE_CLASS(S60CameraAdvSettings)

/*
 * Class handling all image capture and basic camera settings.
 */
class S60ImageCaptureSettings : public QObject
{
    Q_OBJECT

public: // Constructor & Destructor

    S60ImageCaptureSettings(S60ImageCaptureSession *session,
                            QObject *parent = 0);
    ~S60ImageCaptureSettings();

public: // Methods

    void setCameraHandle(CCameraEngine* camerahandle);
    void setCurrentDevice(TInt deviceindex);
    void notifySettingsSet();
    void initializeImageCaptureSettings();
    void applyCameraSettings();

    // Ecam Advanced Settings
    void resetSettings(CCameraEngine *engine, const bool errorHandling);
    void deleteAdvancedSettings();

    // Capture Destination
    void setCaptureDestination(const QCameraImageCapture::CaptureDestinations destination);
    QCameraImageCapture::CaptureDestinations destination() const;

    // Image Resolution
    QSize imageResolution() const;
    QList<QSize> supportedImageResolutionsForCodec(const QString &codecName);
    void setImageResolution(const QSize &size);

    // Image Codec
    QStringList supportedImageCodecs();
    QString imageCodec();
    void setImageCodec(const QString &codecName);
    QString imageCodecDescription(const QString &codecName);

    // Image Quality
    QtMultimediaKit::EncodingQuality imageQuality() const;
    void setImageQuality(const QtMultimediaKit::EncodingQuality &quality);
    void commitJpegQuality() const;
    int jpegQuality() const;

    // Image Format (Buffer Capture)
    QList<QVideoFrame::PixelFormat> supportedBufferCaptureFormats() const;
    void setBufferCaptureFormat(const QVideoFrame::PixelFormat format);
    CCamera::TFormat currentSymbianFormat() const;
    void resetSymbianFormat();

    // S60 3.1 Focus Control (S60 3.2 and later via S60CameraAdvSettings class)
    bool isFocusSupported() const;
    void startFocus();
    void cancelFocus();
    void notifyFocusComplete();

    // Zoom Control
    qreal maximumZoom();
    qreal minZoom();
    qreal maxDigitalZoom();
    void doSetZoomFactorL(qreal optical, qreal digital);
    qreal opticalZoomFactor();
    qreal digitalZoomFactor();

    // Focus Control
    void startFocusing();
    void cancelFocusing();

    // Focus Mode Control
    QCameraFocus::FocusMode focusMode();
    void setFocusMode(QCameraFocus::FocusMode mode);
    bool isFocusModeSupported(QCameraFocus::FocusMode mode) const;

    // Exposure Mode Control
    QCameraExposure::ExposureMode exposureMode();
    void setExposureMode(QCameraExposure::ExposureMode mode);
    bool isExposureModeSupported(QCameraExposure::ExposureMode mode) const;

    // Metering Mode Control
    QCameraExposure::MeteringMode meteringMode();
    void setMeteringMode(QCameraExposure::MeteringMode mode);
    bool isMeteringModeSupported(QCameraExposure::MeteringMode mode) const;

    // Flash Mode Control
    QCameraExposure::FlashModes flashMode();
    void setFlashMode(QCameraExposure::FlashModes mode);
    QCameraExposure::FlashModes supportedFlashModes();
    bool isFlashReady() const;

    // Contrast Control
    int contrast() const;
    void setContrast(int value);

    // Brightness Control
    int brightness() const;
    void setBrightness(int value);

    bool areContrastAndBrightnessSupported() const;

    // Saturation Control
    bool isSaturationSupported() const;
    int saturation() const;
    void setSaturation(int value);

    // Sharpening Control
    bool isSharpeningSupported() const;
    int sharpening() const;
    void setSharpening(int value);

    // Denoising Control
    bool isDenoisingSupported() const;
    int denoising() const;
    void setDenoising(int value);

    // White Balance Mode Control
    QCameraImageProcessing::WhiteBalanceMode whiteBalanceMode();
    void setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceMode mode);
    bool isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceMode mode) const;

    // ISO Sensitivity
    int isoSensitivity() const;
    bool setIsoSensitivity(int iso);
    void setAutoIsoSensitivity();
    QList<int> supportedIsoSensitivities() const;

    // Aperture
    qreal aperture() const;
    bool setAperture(qreal aperture);
    QList<qreal> supportedApertures() const;

    // Shutter Speed
    qreal shutterSpeed() const;
    bool setShutterSpeed(qreal seconds);
    QList<qreal> supportedShutterSpeeds() const;

    // Exposure Compensation
    qreal exposureCompensation() const;
    bool setExposureCompensation(qreal ev);
    QList<qreal> supportedExposureCompensations() const;

private: // Internal

    CCamera::TFormat selectFormatForCodec(const QString &codec);
    CCamera::TFormat defaultImageFormat();
    bool queryCurrentCameraInfo();
    QMap<QString, QString> codecDescriptionMap();
    void updateImageCaptureFormats();

    void doSetWhiteBalanceModeL(QCameraImageProcessing::WhiteBalanceMode mode);
    void doSetFlashModeL(QCameraExposure::FlashModes mode);
    void doSetExposureModeL(QCameraExposure::ExposureMode mode);
    QList<QSize> sortResolutions(QList<QSize> resolutions);

signals: // Notifications

    void destinationChanged(const QCameraImageCapture::CaptureDestinations);
    void bufferCaptureFormatChanged(const QVideoFrame::PixelFormat);
    void flashReady(bool);
    void focusStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason);
    void apertureChanged();
    void apertureRangeChanged();
    void shutterSpeedChanged();
    void isoSensitivityChanged();
    void evChanged();

private: // Data

    S60ImageCaptureSession  *m_session;
    CCameraEngine           *m_cameraEngine;
    S60CameraAdvSettings    *m_advancedSettings;
    mutable TCameraInfo     *m_cameraInfo;
    TInt                    m_activeDeviceIndex;
    QCameraImageCapture::CaptureDestinations m_captureDestination;
    QStringList             m_supportedImageCodecs;
    QList<QVideoFrame::PixelFormat> m_supportedBufferCaptureFormats;
    QString                 m_currentCodec;
    CCamera::TFormat        m_currentFormat;
    QSize                   m_captureSize;
    int                     m_symbianImageQuality;
    QVideoFrame::PixelFormat m_bufferCaptureFormat;
    bool                    m_captureSettingsSet;
    QList<uint>             m_formats;
    QCameraFocus::FocusMode m_requestedFocusMode;
    qreal                   m_requestedDigitalZoomFactor;
    QCameraExposure::FlashModes m_requestedFlashMode;
    QCameraExposure::ExposureMode m_requestedExposureMode;
    QCameraImageProcessing::WhiteBalanceMode m_requestedWhiteBalanceMode;
    int                     m_requestedContrast;
    int                     m_requestedBrightness;
    int                     m_requestedSaturation;
    int                     m_requestedSharpening;
    int                     m_requestedDenoising;
    int                     m_requestedIsoSensitivity;
    qreal                   m_requestedAperture;
    qreal                   m_requestedShutterSpeed;
    qreal                   m_reauestedExposureCompensation;
};

#endif // S60IMAGECAPTURESESETTINGS_H
