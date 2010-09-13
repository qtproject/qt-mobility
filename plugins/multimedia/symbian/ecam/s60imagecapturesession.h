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

#ifndef S60IMAGECAPTURESESSION_H
#define S60IMAGECAPTURESESSION_H

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qdatetime.h> // QTime
#include <QtGui/qicon.h>
#include <QtMultimedia/qvideoframe.h>

#include <qcamera.h>
#include <qcamerafocus.h>
#include <qcameraimagecapture.h>

#include <e32base.h>

#include "s60camerasettings.h"
#include "s60cameraengine.h"
#include "s60cameraengineobserver.h"

QT_USE_NAMESPACE

class S60CameraService;

class S60ImageCaptureSession : public QObject, public MCameraEngineObserver
{
    Q_OBJECT

public: // Enums

    enum Error {
        NoError = 0,
        OutOfMemoryError,
        InUseError,
        NotReadyError,
        UnknownError = -1
    };

    enum EcamErrors {
        KErrECamCameraDisabled = -12100,        // The camera has been disabled, hence calls do not succeed
        KErrECamSettingDisabled = -12101,       // This parameter or operation is supported, but presently is disabled.
        KErrECamParameterNotInRange = -12102,   // This value is out of range.
        KErrECamSettingNotSupported = -12103,   // This parameter or operation is not supported.
        KErrECamNotOptimalFocus = -12104        // The optimum focus is lost
    };

public: // Constructor & Destructor

    S60ImageCaptureSession(QObject *parent = 0);
    ~S60ImageCaptureSession();

public: // Methods

    void setError(TInt aError);
    int currentImageId() const;

    bool isDeviceReady();
    CCamera::TFormat defaultCodec();
    void setCameraHandle(CCameraEngine* camerahandle);
    void setCurrentDevice(TInt deviceindex);

    // Ecam Advanced Settings
    S60CameraSettings* advancedSettings();
    void deleteAdvancedSettings();

    // Controls
    int prepareImageCapture();
    int capture(const QString &fileName);
    void cancelCapture();
    void releaseImageBuffer();

    // Image Encoder Control
    QSize captureSize() const;
    QSize minimumCaptureSize();
    QSize maximumCaptureSize();
    QList<QSize> supportedCaptureSizesForCodec(const QString &codecName);
    void setCaptureSize(const QSize &size);
    QStringList supportedImageCaptureCodecs();
    QString imageCaptureCodec();
    void setImageCaptureCodec(const QString &codecName);
    QString imageCaptureCodecDescription(const QString &codecName);
    QtMultimediaKit::EncodingQuality captureQuality() const;
    void setCaptureQuality(QtMultimediaKit::EncodingQuality);
    void updateImageCaptureCodecs();

    // S60 3.1 Focus Control (S60 3.2 and later via S60CameraSettings class)
    bool isFocusSupported();
    void startFocus();
    void cancelFocus();

    // Zoom Control
    int maximumZoom();
    int minZoom();
    int maxDigitalZoom();
    void setZoomFactor(qreal optical, qreal digital);
    int zoomFactor();
    int digitalZoomFactor();

    // Exposure Control
    QCameraExposure::ExposureMode exposureMode();
    void setExposureMode(QCameraExposure::ExposureMode mode);
    bool isExposureModeSupported(QCameraExposure::ExposureMode mode) const;

    // Flash Control
    QCameraExposure::FlashMode flashMode();
    void setFlashMode(QCameraExposure::FlashModes mode);
    QCameraExposure::FlashModes supportedFlashModes();

    // Contrast Control
    int contrast() const;
    void setContrast(int value);

    // White Balance Mode Control
    QCameraImageProcessing::WhiteBalanceMode whiteBalanceMode();
    void setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceMode mode);
    bool isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceMode mode) const;

protected: // MCameraEngineObserver

    void MceoCameraReady();
    void MceoFocusComplete();
    void MceoCapturedDataReady(TDesC8* aData);
    void MceoCapturedBitmapReady(CFbsBitmap* aBitmap);
    void MceoViewFinderFrameReady(CFbsBitmap& aFrame);
    void MceoHandleError(TCameraEngineError aErrorType, TInt aError);

private: // Internal

    QCamera::Error fromSymbianErrorToQtMultimediaError(int aError);

    bool queryCurrentCameraInfo();
    QMap<QString, int> formatMap();
    QMap<QString, int> formatDescMap();

    void setWhiteBalanceModeL(QCameraImageProcessing::WhiteBalanceMode mode);
    void resetSession();
    void setFlashModeL(QCameraExposure::FlashModes mode);
    void setExposureModeL(QCameraExposure::ExposureMode mode);
    void saveImageL(TDesC8* aData, TFileName aPath);
    TFileName imagePath();

Q_SIGNALS: // Notifications

    void stateChanged(QCamera::State);
    void advancedSettingCreated();
    // Capture notifications
    void error(int error, const QString &errorString);
    void readyForCaptureChanged(bool);
    void imageCaptured(const int id, const QImage &preview);
    void imageSaved(const int id, const QString &fileName);
    // Focus notifications
    void focusStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason);
    // Zoom notifications
    void opticalZoomChanged(qreal opticalZoom);
    void digitalZoomChanged(qreal digitalZoom);

private: // Data

    CCameraEngine       *m_cameraEngine;
    S60CameraSettings   *m_advancedSettings;
    int                 m_imageQuality;
    QSize               m_captureSize;
    TInt                m_activeDeviceIndex;
    mutable int         m_error;
    CCamera::TFormat    m_currentCodec;
    QList<uint>         m_formats;
    QString             m_stillCaptureFileName;
    mutable TCameraInfo m_info;
    mutable int         m_currentImageId;
};

#endif // S60IMAGECAPTURESESSION_H
