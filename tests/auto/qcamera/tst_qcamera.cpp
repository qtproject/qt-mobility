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

#include <QtTest/QtTest>
#include <QDebug>

#include <qcameracontrol.h>
#include <qcameralockscontrol.h>
#include <qcameraexposurecontrol.h>
#include <qcamerafocuscontrol.h>
#include <qcameraimagecapturecontrol.h>
#include <qimageencodercontrol.h>
#include <qcameraimageprocessingcontrol.h>
#include <qmediaservice.h>
#include <qcamera.h>
#include <qcameraimagecapture.h>

QT_USE_NAMESPACE
class MockCaptureControl;

class MockCameraControl : public QCameraControl
{
    friend class MockCaptureControl;
    Q_OBJECT
public:
    MockCameraControl(QObject *parent = 0):
            QCameraControl(parent),
            m_state(QCamera::StoppedState),
            m_captureMode(QCamera::CaptureStillImage)
    {
    }

    ~MockCameraControl() {}

    void start() { m_state = QCamera::ActiveState; }
    virtual void stop() { m_state = QCamera::StoppedState; }
    QCamera::State state() const { return m_state; }
    void setState(QCamera::State state) {
        if (m_state != state) {
            if (state == QCamera::ActiveState || state == QCamera::StoppedState)
                emit stateChanged(m_state = state);
            else
                emit error(QCamera::NotSupportedFeatureError, "State not supported.");
        }
    }

    QCamera::CaptureMode captureMode() const { return m_captureMode; }
    void setCaptureMode(QCamera::CaptureMode mode)
    {
        if (m_captureMode != mode) {
            m_captureMode = mode;
            emit captureModeChanged(mode);
        }
    }

    bool isCaptureModeSupported(QCamera::CaptureMode mode) const
    {
        return mode == QCamera::CaptureStillImage || mode == QCamera::CaptureVideo;
    }

    QCamera::LockTypes supportedLocks() const
    {
        return QCamera::LockExposure | QCamera::LockFocus | QCamera::LockWhiteBalance;
    }

    QCamera::State m_state;
    QCamera::CaptureMode m_captureMode;
};


class MockCameraLocksControl : public QCameraLocksControl
{
    Q_OBJECT
public:
    MockCameraLocksControl(QObject *parent = 0):
            QCameraLocksControl(parent),
            m_focusLock(QCamera::Unlocked),
            m_exposureLock(QCamera::Unlocked)
    {
    }

    ~MockCameraLocksControl() {}

    QCamera::LockTypes supportedLocks() const
    {
        return QCamera::LockExposure | QCamera::LockFocus;
    }

    QCamera::LockStatus lockStatus(QCamera::LockType lock) const
    {
        switch (lock) {
        case QCamera::LockExposure:
            return m_exposureLock;
        case QCamera::LockFocus:
            return m_focusLock;
        default:
            return QCamera::Unlocked;
        }
    }

    void searchAndLock(QCamera::LockTypes locks)
    {
        if (locks & QCamera::LockExposure) {
            QCamera::LockStatus newStatus = locks & QCamera::LockFocus ? QCamera::Searching : QCamera::Locked;

            if (newStatus != m_exposureLock)
                emit lockStatusChanged(QCamera::LockExposure,
                                       m_exposureLock = newStatus,
                                       QCamera::UserRequest);
        }

        if (locks & QCamera::LockFocus) {
            emit lockStatusChanged(QCamera::LockFocus,
                                   m_focusLock = QCamera::Searching,
                                   QCamera::UserRequest);

            QTimer::singleShot(5, this, SLOT(focused()));
        }
    }

    void unlock(QCamera::LockTypes locks) {
        if (locks & QCamera::LockFocus && m_focusLock != QCamera::Unlocked) {
            emit lockStatusChanged(QCamera::LockFocus,
                                   m_focusLock = QCamera::Unlocked,
                                   QCamera::UserRequest);
        }

        if (locks & QCamera::LockExposure && m_exposureLock != QCamera::Unlocked) {
            emit lockStatusChanged(QCamera::LockExposure,
                                   m_exposureLock = QCamera::Unlocked,
                                   QCamera::UserRequest);
        }
    }

private slots:
    void focused()
    {
        if (m_focusLock == QCamera::Searching) {
            emit lockStatusChanged(QCamera::LockFocus,
                                   m_focusLock = QCamera::Locked,
                                   QCamera::UserRequest);
        }

        if (m_exposureLock == QCamera::Searching) {
            emit lockStatusChanged(QCamera::LockExposure,
                                   m_exposureLock = QCamera::Locked,
                                   QCamera::UserRequest);
        }
    }


private:
    QCamera::LockStatus m_focusLock;
    QCamera::LockStatus m_exposureLock;
};

class MockCaptureControl : public QCameraImageCaptureControl
{
    Q_OBJECT
public:
    MockCaptureControl(MockCameraControl *cameraControl, QObject *parent = 0)
        :QCameraImageCaptureControl(parent), m_cameraControl(cameraControl), m_captureRequest(0), m_ready(true), m_captureCanceled(false)
    {
    }

    ~MockCaptureControl()
    {
    }

    QCameraImageCapture::DriveMode driveMode() const { return QCameraImageCapture::SingleImageCapture; }
    void setDriveMode(QCameraImageCapture::DriveMode) {}

    bool isReadyForCapture() const { return m_ready && m_cameraControl->state() == QCamera::ActiveState; }

    int capture(const QString &fileName)
    {
        if (isReadyForCapture()) {
            m_fileName = fileName;
            m_captureRequest++;
            emit readyForCaptureChanged(m_ready = false);
            QTimer::singleShot(5, this, SLOT(captured()));
            return m_captureRequest;
        } else {
            emit error(QCameraImageCapture::NotReadyError,
                       QLatin1String("Could not capture in stopped state"));
        }

        return -1;
    }

    void cancelCapture()
    {
        m_captureCanceled = true;
    }

private Q_SLOTS:
    void captured()
    {
        if (!m_captureCanceled)
            emit imageCaptured(m_captureRequest, QImage());

        if (!m_ready)
            emit readyForCaptureChanged(m_ready = true);

        if (!m_captureCanceled)
            emit imageSaved(m_captureRequest, m_fileName);

        m_captureCanceled = false;
    }

private:
    MockCameraControl *m_cameraControl;
    QString m_fileName;
    int m_captureRequest;
    bool m_ready;
    bool m_captureCanceled;
};

class MockCameraExposureControl : public QCameraExposureControl
{
    Q_OBJECT
public:
    MockCameraExposureControl(QObject *parent = 0):
        QCameraExposureControl(parent),        
        m_aperture(2.8),
        m_shutterSpeed(0.01),
        m_isoSensitivity(100),
        m_meteringMode(QCameraExposure::MeteringMatrix),
        m_exposureCompensation(0),
        m_exposureMode(QCameraExposure::ExposureAuto),
        m_flashMode(QCameraExposure::FlashAuto)
    {
    }

    ~MockCameraExposureControl() {}

    QCameraExposure::FlashModes flashMode() const
    {
        return m_flashMode;
    }

    void setFlashMode(QCameraExposure::FlashModes mode)
    {
        if (isFlashModeSupported(mode)) {
            m_flashMode = mode;
        }
    }

    bool isFlashModeSupported(QCameraExposure::FlashModes mode) const
    {
        return mode & (QCameraExposure::FlashAuto | QCameraExposure::FlashOff | QCameraExposure::FlashOn);
    }

    bool isFlashReady() const
    {
        return true;
    }

    QCameraExposure::ExposureMode exposureMode() const
    {
        return m_exposureMode;
    }

    void setExposureMode(QCameraExposure::ExposureMode mode)
    {
        if (isExposureModeSupported(mode))
            m_exposureMode = mode;
    }

    bool isExposureModeSupported(QCameraExposure::ExposureMode mode) const
    {
        return mode == QCameraExposure::ExposureAuto ||
               mode == QCameraExposure::ExposureManual;
    }

    bool isParameterSupported(ExposureParameter parameter) const
    {
        switch (parameter) {
        case QCameraExposureControl::ExposureCompensation:
        case QCameraExposureControl::ISO:
        case QCameraExposureControl::Aperture:
        case QCameraExposureControl::ShutterSpeed:
            return true;
        default:
            return false;
        }
    }

    QVariant exposureParameter(ExposureParameter parameter) const
    {
        switch (parameter) {
        case QCameraExposureControl::ExposureCompensation:
            return QVariant(m_exposureCompensation);
        case QCameraExposureControl::ISO:
            return QVariant(m_isoSensitivity);
        case QCameraExposureControl::Aperture:
            return QVariant(m_aperture);
        case QCameraExposureControl::ShutterSpeed:
            return QVariant(m_shutterSpeed);
        default:
            return QVariant();
        }
    }

    QVariantList supportedParameterRange(ExposureParameter parameter, bool *continuous) const
    {
        QVariantList res;
        if (continuous)
            *continuous = true;

        switch (parameter) {
        case QCameraExposureControl::ExposureCompensation:
            res << -2.0 << 2.0;
            break;
        case QCameraExposureControl::ISO:
            if (continuous)
                *continuous = false;
            res << 100 << 200 << 400 << 800;
            break;
        case QCameraExposureControl::Aperture:
            res << 2.8 << 4.0 << 5.6 << 8.0 << 11.0 << 16.0;
            break;

        case QCameraExposureControl::ShutterSpeed:
            res << 0.001 << 0.01 << 0.1 << 1.0;
            break;
        default:
            if (continuous)
                *continuous = false;
        }

        return res;
    }

    bool setExposureParameter(ExposureParameter parameter, const QVariant& value)
    {
        switch (parameter) {
        case QCameraExposureControl::ExposureCompensation:
            m_exposureCompensation = qBound<qreal>(-2.0, value.toReal(), 2.0);
            break;
        case QCameraExposureControl::ISO:
            m_isoSensitivity = 100*qRound(qBound(100, value.toInt(), 800)/100.0);
            break;
        case QCameraExposureControl::Aperture:
            m_aperture = qBound<qreal>(2.8, value.toReal(), 16.0);
            break;
        case QCameraExposureControl::ShutterSpeed:
            m_shutterSpeed = qBound<qreal>(0.001, value.toReal(), 1.0);
            break;
        default:
            return false;
        }

        return true;
    }

    QString extendedParameterName(ExposureParameter)
    {
        return QString();
    }

    QCameraExposure::MeteringMode meteringMode() const
    {
        return m_meteringMode;
    }

    void setMeteringMode(QCameraExposure::MeteringMode mode)
    {
        if (isMeteringModeSupported(mode))
            m_meteringMode = mode;
    }

    bool isMeteringModeSupported(QCameraExposure::MeteringMode mode) const
    {
        return mode == QCameraExposure::MeteringAverage
                || mode == QCameraExposure::MeteringMatrix;
    }

private:
    qreal m_aperture;
    qreal m_shutterSpeed;
    int m_isoSensitivity;
    QCameraExposure::MeteringMode m_meteringMode;
    qreal m_exposureCompensation;
    QCameraExposure::ExposureMode m_exposureMode;
    QCameraExposure::FlashModes m_flashMode;
};

class MockCameraFocusControl : public QCameraFocusControl
{
    Q_OBJECT
public:
    MockCameraFocusControl(QObject *parent = 0):
        QCameraFocusControl(parent),
        m_opticalZoom(1.0),
        m_digitalZoom(1.0),
        m_focusMode(QCameraFocus::AutoFocus),
        m_focusPointMode(QCameraFocus::FocusPointAuto),
        m_focusPoint(0.5, 0.5)
    {
    }

    ~MockCameraFocusControl() {}

    QCameraFocus::FocusMode focusMode() const
    {
        return m_focusMode;
    }

    void setFocusMode(QCameraFocus::FocusMode mode)
    {
        if (isFocusModeSupported(mode))
            m_focusMode = mode;
    }

    bool isFocusModeSupported(QCameraFocus::FocusMode mode) const
    {
        return mode == QCameraFocus::AutoFocus || mode == QCameraFocus::ContinuousFocus;
    }

    qreal maximumOpticalZoom() const
    {
        return 3.0;
    }

    qreal maximumDigitalZoom() const
    {
        return 4.0;
    }

    qreal opticalZoom() const
    {
        return m_opticalZoom;
    }

    qreal digitalZoom() const
    {
        return m_digitalZoom;
    }

    void zoomTo(qreal optical, qreal digital)
    {
        optical = qBound<qreal>(1.0, optical, maximumOpticalZoom());
        digital = qBound<qreal>(1.0, digital, maximumDigitalZoom());

        if (!qFuzzyCompare(digital, m_digitalZoom)) {
            m_digitalZoom = digital;
            emit digitalZoomChanged(m_digitalZoom);
        }

        if (!qFuzzyCompare(optical, m_opticalZoom)) {
            m_opticalZoom = optical;
            emit opticalZoomChanged(m_opticalZoom);
        }
    }

    QCameraFocus::FocusPointMode focusPointMode() const
    {
        return m_focusPointMode;
    }

    void setFocusPointMode(QCameraFocus::FocusPointMode mode)
    {
        if (isFocusPointModeSupported(mode))
            m_focusPointMode = mode;
    }

    bool isFocusPointModeSupported(QCameraFocus::FocusPointMode mode) const
    {
        switch (mode) {
        case QCameraFocus::FocusPointAuto:
        case QCameraFocus::FocusPointCenter:
        case QCameraFocus::FocusPointCustom:
            return true;
        default:
            return false;
        }
    }

    QPointF customFocusPoint() const
    {
        return m_focusPoint;
    }

    void setCustomFocusPoint(const QPointF &point)
    {
        m_focusPoint = point;
    }

    QCameraFocusZoneList focusZones() const { return QCameraFocusZoneList() << QCameraFocusZone(QRectF(0.45, 0.45, 0.1, 0.1)); }


private:
    qreal m_opticalZoom;
    qreal m_digitalZoom;
    QCameraFocus::FocusMode m_focusMode;
    QCameraFocus::FocusPointMode m_focusPointMode;
    QPointF m_focusPoint;
};

class MockImageProcessingControl : public QCameraImageProcessingControl
{
    Q_OBJECT
public:
    MockImageProcessingControl(QObject *parent = 0)
        : QCameraImageProcessingControl(parent)
    {
        m_supportedWhiteBalance.insert(QCameraImageProcessing::WhiteBalanceAuto);
    }

    QCameraImageProcessing::WhiteBalanceMode whiteBalanceMode() const { return m_whiteBalanceMode; }
    void setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceMode mode) { m_whiteBalanceMode = mode; }

    bool isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceMode mode) const {
        return m_supportedWhiteBalance.contains(mode); }

    void setSupportedWhiteBalanceModes(QSet<QCameraImageProcessing::WhiteBalanceMode> modes) {
        m_supportedWhiteBalance = modes; }

    int manualWhiteBalance() const { return m_manualWhiteBalance; }
    void setManualWhiteBalance(int colorTemperature) { m_manualWhiteBalance = colorTemperature; }

    bool isProcessingParameterSupported(ProcessingParameter parameter) const
    {
        return parameter == Contrast ||  parameter == Sharpening;
    }
    QVariant processingParameter(ProcessingParameter parameter) const
    {
        switch (parameter) {
        case Contrast:
            return m_contrast;
        case Sharpening:
            return m_sharpeningLevel;
        default:
            return QVariant();
        }
    }
    void setProcessingParameter(ProcessingParameter parameter, QVariant value)
    {
        switch (parameter) {
        case Contrast:
            m_contrast = value;
            break;
        case Sharpening:
            m_sharpeningLevel = value;
            break;
        default:
            break;
        }
    }

    QList<QByteArray> supportedPresets() const
    {
        return m_presets;
    }

    QString presetDescription(const QByteArray &preset) const
    {
        return QString::fromLatin1(preset);
    }

    QByteArray preset() const
    {
        return m_preset;
    }
    void setPreset(const QByteArray &preset)
    {
        if (m_presets.contains(preset))
            m_preset = preset;
        else
            m_preset.clear();
    }


private:
    QCameraImageProcessing::WhiteBalanceMode m_whiteBalanceMode;
    QSet<QCameraImageProcessing::WhiteBalanceMode> m_supportedWhiteBalance;
    int m_manualWhiteBalance;
    QVariant m_contrast;
    QVariant m_sharpeningLevel;

    QList<QByteArray> m_presets;
    QByteArray m_preset;
};

class MockImageEncoderControl : public QImageEncoderControl
{
public:
    MockImageEncoderControl(QObject *parent = 0)
        : QImageEncoderControl(parent)
    {
    }

    QList<QSize> supportedResolutions(const QImageEncoderSettings & = QImageEncoderSettings(),
                                      bool *continuous = 0) const
    {
        if (continuous)
            *continuous = true;

        return m_supportedResolutions;
    }

    void setSupportedResolutions(const QList<QSize> &resolutions) {
        m_supportedResolutions = resolutions; }

    QStringList supportedImageCodecs() const { return m_supportedCodecs; }
    void setSupportedImageCodecs(const QStringList &codecs) { m_supportedCodecs = codecs; }

    QString imageCodecDescription(const QString &codecName) const {
        return m_codecDescriptions.value(codecName); }
    void setImageCodecDescriptions(const QMap<QString, QString> &descriptions) {
        m_codecDescriptions = descriptions; }

    QImageEncoderSettings imageSettings() const { return m_settings; }
    void setImageSettings(const QImageEncoderSettings &settings) { m_settings = settings; }

private:
    QImageEncoderSettings m_settings;

    QList<QSize> m_supportedResolutions;
    QStringList m_supportedCodecs;
    QMap<QString, QString> m_codecDescriptions;
};

class MockSimpleCameraService : public QMediaService
{
    Q_OBJECT

public:
    MockSimpleCameraService(): QMediaService(0)
    {
        mockControl = new MockCameraControl(this);
    }

    ~MockSimpleCameraService()
    {
    }

    QMediaControl* requestControl(const char *iid)
    {
        if (qstrcmp(iid, QCameraControl_iid) == 0)
            return mockControl;
        return 0;
    }

    void releaseControl(QMediaControl*) {}

    MockCameraControl *mockControl;
};

class MockCameraService : public QMediaService
{
    Q_OBJECT

public:
    MockCameraService(): QMediaService(0)
    {
        mockControl = new MockCameraControl(this);
        mockLocksControl = new MockCameraLocksControl(this);
        mockExposureControl = new MockCameraExposureControl(this);
        mockFocusControl = new MockCameraFocusControl(this);
        mockCaptureControl = new MockCaptureControl(mockControl, this);
        mockImageProcessingControl = new MockImageProcessingControl(this);
        mockImageEncoderControl = new MockImageEncoderControl(this);
    }

    ~MockCameraService()
    {
    }

    QMediaControl* requestControl(const char *iid)
    {
        if (qstrcmp(iid, QCameraControl_iid) == 0)
            return mockControl;

        if (qstrcmp(iid, QCameraLocksControl_iid) == 0)
            return mockLocksControl;

        if (qstrcmp(iid, QCameraExposureControl_iid) == 0)
            return mockExposureControl;

        if (qstrcmp(iid, QCameraFocusControl_iid) == 0)
            return mockFocusControl;

        if (qstrcmp(iid, QCameraImageCaptureControl_iid) == 0)
            return mockCaptureControl;

        if (qstrcmp(iid, QCameraImageProcessingControl_iid) == 0)
            return mockImageProcessingControl;

        if (qstrcmp(iid, QImageEncoderControl_iid) == 0)
            return mockImageEncoderControl;

        return 0;
    }

    void releaseControl(QMediaControl*) {}

    MockCameraControl *mockControl;
    MockCameraLocksControl *mockLocksControl;
    MockCaptureControl *mockCaptureControl;
    MockCameraExposureControl *mockExposureControl;
    MockCameraFocusControl *mockFocusControl;
    MockImageProcessingControl *mockImageProcessingControl;
    MockImageEncoderControl *mockImageEncoderControl;
};

class MockProvider : public QMediaServiceProvider
{
public:
    QMediaService *requestService(const QByteArray &, const QMediaServiceProviderHint &)
    {
        return service;
    }

    void releaseService(QMediaService *) {}

    QMediaService *service;
};


class tst_QCamera: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void testAvailableDevices();
    void testDeviceDescription();
    void testCtorWithDevice();
    void testSimpleCamera();
    void testSimpleCameraWhiteBalance();
    void testSimpleCameraExposure();
    void testSimpleCameraFocus();
    void testSimpleCameraCapture();
    void testSimpleCameraLock();

    void testCameraWhiteBalance();
    void testCameraExposure();
    void testCameraFocus();
    void testCameraCapture();
    void testImageSettings();
    void testCameraLock();
    void testCameraLockCancel();

private:
    MockSimpleCameraService  *mockSimpleCameraService;
    MockProvider *provider;
};

void tst_QCamera::initTestCase()
{
    provider = new MockProvider;
    mockSimpleCameraService = new MockSimpleCameraService;
    provider->service = mockSimpleCameraService;
}

void tst_QCamera::cleanupTestCase()
{
    delete mockSimpleCameraService;
    delete provider;
}

void tst_QCamera::testAvailableDevices()
{
    int deviceCount = QMediaServiceProvider::defaultServiceProvider()->devices(QByteArray(Q_MEDIASERVICE_CAMERA)).count();

    QVERIFY(QCamera::availableDevices().count() == deviceCount);
}

void tst_QCamera::testDeviceDescription()
{
    int deviceCount = QMediaServiceProvider::defaultServiceProvider()->devices(QByteArray(Q_MEDIASERVICE_CAMERA)).count();

    if (deviceCount == 0)
        QVERIFY(QCamera::deviceDescription(QByteArray("random")).isNull());
    else {
        foreach (const QByteArray &device, QCamera::availableDevices())
            QVERIFY(QCamera::deviceDescription(device).length() > 0);
    }
}

void tst_QCamera::testCtorWithDevice()
{
    int deviceCount = QMediaServiceProvider::defaultServiceProvider()->devices(QByteArray(Q_MEDIASERVICE_CAMERA)).count();
    QCamera *camera = 0;

    if (deviceCount == 0) {
        camera = new QCamera("random");
        QVERIFY(camera->error() == QCamera::ServiceMissingError);
    }
    else {
        camera = new QCamera(QCamera::availableDevices().first());
        QVERIFY(camera->error() == QCamera::NoError);
    }

    delete camera;
}

void tst_QCamera::testSimpleCamera()
{
    QCamera camera(0, provider);
    QCOMPARE(camera.service(), (QMediaService*)mockSimpleCameraService);

    QCOMPARE(camera.state(), QCamera::StoppedState);
    camera.start();
    QCOMPARE(camera.state(), QCamera::ActiveState);
    camera.stop();
    QCOMPARE(camera.state(), QCamera::StoppedState);
}

void tst_QCamera::testSimpleCameraWhiteBalance()
{
    QCamera camera(0, provider);

    //only WhiteBalanceAuto is supported
    QVERIFY(!camera.imageProcessing()->isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceAuto));
    QVERIFY(!camera.imageProcessing()->isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceCloudy));
    QCOMPARE(camera.imageProcessing()->whiteBalanceMode(), QCameraImageProcessing::WhiteBalanceAuto);
    camera.imageProcessing()->setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceCloudy);
    QCOMPARE(camera.imageProcessing()->whiteBalanceMode(), QCameraImageProcessing::WhiteBalanceAuto);
    QCOMPARE(camera.imageProcessing()->manualWhiteBalance(), -1);
    camera.imageProcessing()->setManualWhiteBalance(5000);
    QCOMPARE(camera.imageProcessing()->manualWhiteBalance(), -1);
}

void tst_QCamera::testSimpleCameraExposure()
{
    QCamera camera(0, provider);
    QCameraExposure *cameraExposure = camera.exposure();
    QVERIFY(cameraExposure != 0);

    QVERIFY(!cameraExposure->isExposureModeSupported(QCameraExposure::ExposureAuto));
    QCOMPARE(cameraExposure->exposureMode(), QCameraExposure::ExposureAuto);
    cameraExposure->setExposureMode(QCameraExposure::ExposureManual);//should be ignored
    QCOMPARE(cameraExposure->exposureMode(), QCameraExposure::ExposureAuto);

    QVERIFY(!cameraExposure->isFlashModeSupported(QCameraExposure::FlashOff));
    QCOMPARE(cameraExposure->flashMode(), QCameraExposure::FlashOff);
    QCOMPARE(cameraExposure->isFlashReady(), false);
    cameraExposure->setFlashMode(QCameraExposure::FlashOn);
    QCOMPARE(cameraExposure->flashMode(), QCameraExposure::FlashOff);

    QVERIFY(!cameraExposure->isMeteringModeSupported(QCameraExposure::MeteringAverage));
    QVERIFY(!cameraExposure->isMeteringModeSupported(QCameraExposure::MeteringSpot));
    QVERIFY(!cameraExposure->isMeteringModeSupported(QCameraExposure::MeteringMatrix));
    QCOMPARE(cameraExposure->meteringMode(), QCameraExposure::MeteringMatrix);
    cameraExposure->setMeteringMode(QCameraExposure::MeteringSpot);
    QCOMPARE(cameraExposure->meteringMode(), QCameraExposure::MeteringMatrix);

    QCOMPARE(cameraExposure->exposureCompensation(), 0.0);
    cameraExposure->setExposureCompensation(2.0);
    QCOMPARE(cameraExposure->exposureCompensation(), 0.0);

    QCOMPARE(cameraExposure->isoSensitivity(), -1);
    QVERIFY(cameraExposure->supportedIsoSensitivities().isEmpty());
    cameraExposure->setManualIsoSensitivity(100);
    QCOMPARE(cameraExposure->isoSensitivity(), -1);
    cameraExposure->setAutoIsoSensitivity();
    QCOMPARE(cameraExposure->isoSensitivity(), -1);

    QVERIFY(cameraExposure->aperture() < 0);
    QVERIFY(cameraExposure->supportedApertures().isEmpty());
    cameraExposure->setAutoAperture();
    QVERIFY(cameraExposure->aperture() < 0);
    cameraExposure->setManualAperture(5.6);
    QVERIFY(cameraExposure->aperture() < 0);

    QVERIFY(cameraExposure->shutterSpeed() < 0);
    QVERIFY(cameraExposure->supportedShutterSpeeds().isEmpty());
    cameraExposure->setAutoShutterSpeed();
    QVERIFY(cameraExposure->shutterSpeed() < 0);
    cameraExposure->setManualShutterSpeed(1/128.0);
    QVERIFY(cameraExposure->shutterSpeed() < 0);   
}

void tst_QCamera::testSimpleCameraFocus()
{
    QCamera camera(0, provider);

    QCameraFocus *cameraFocus = camera.focus();
    QVERIFY(cameraFocus != 0);

    QVERIFY(!cameraFocus->isFocusModeSupported(QCameraFocus::AutoFocus));
    QVERIFY(!cameraFocus->isFocusModeSupported(QCameraFocus::ContinuousFocus));
    QVERIFY(!cameraFocus->isFocusModeSupported(QCameraFocus::InfinityFocus));

    QCOMPARE(cameraFocus->focusMode(), QCameraFocus::AutoFocus);
    QTest::ignoreMessage(QtWarningMsg, "Focus points mode selection is not supported");
    cameraFocus->setFocusMode(QCameraFocus::ContinuousFocus);
    QCOMPARE(cameraFocus->focusMode(), QCameraFocus::AutoFocus);    

    QCOMPARE(cameraFocus->maximumOpticalZoom(), 1.0);
    QCOMPARE(cameraFocus->maximumDigitalZoom(), 1.0);
    QCOMPARE(cameraFocus->opticalZoom(), 1.0);
    QCOMPARE(cameraFocus->digitalZoom(), 1.0);

    QTest::ignoreMessage(QtWarningMsg, "The camera doesn't support zooming.");
    cameraFocus->zoomTo(100.0, 100.0);
    QCOMPARE(cameraFocus->opticalZoom(), 1.0);
    QCOMPARE(cameraFocus->digitalZoom(), 1.0);


    QVERIFY(!cameraFocus->isFocusPointModeSupported(QCameraFocus::FocusPointAuto));
    QCOMPARE(cameraFocus->focusPointMode(), QCameraFocus::FocusPointAuto);


    cameraFocus->setFocusPointMode( QCameraFocus::FocusPointCenter );
    QCOMPARE(cameraFocus->focusPointMode(), QCameraFocus::FocusPointAuto);

    QCOMPARE(cameraFocus->customFocusPoint(), QPointF(0.5, 0.5));
    QTest::ignoreMessage(QtWarningMsg, "Focus points selection is not supported");
    cameraFocus->setCustomFocusPoint(QPointF(1.0, 1.0));
    QCOMPARE(cameraFocus->customFocusPoint(), QPointF(0.5, 0.5));
}

void tst_QCamera::testSimpleCameraCapture()
{
    QCamera camera(0, provider);
    QCameraImageCapture imageCapture(&camera);

    QVERIFY(!imageCapture.isReadyForCapture());
    QVERIFY(!imageCapture.isAvailable());

    QCOMPARE(imageCapture.error(), QCameraImageCapture::NoError);
    QVERIFY(imageCapture.errorString().isEmpty());

    QSignalSpy errorSignal(&imageCapture, SIGNAL(error(QCameraImageCapture::Error)));
    imageCapture.capture(QString::fromLatin1("/dev/null"));
    QCOMPARE(errorSignal.size(), 1);
    QCOMPARE(imageCapture.error(), QCameraImageCapture::NotSupportedFeatureError);
    QVERIFY(!imageCapture.errorString().isEmpty());
}

void tst_QCamera::testSimpleCameraLock()
{
    QCamera camera(0, provider);
    QCOMPARE(camera.lockStatus(), QCamera::Unlocked);
    QCOMPARE(camera.lockStatus(QCamera::LockExposure), QCamera::Unlocked);
    QCOMPARE(camera.lockStatus(QCamera::LockFocus), QCamera::Unlocked);
    QCOMPARE(camera.lockStatus(QCamera::LockWhiteBalance), QCamera::Unlocked);

    QSignalSpy lockedSignal(&camera, SIGNAL(locked()));
    QSignalSpy lockFailedSignal(&camera, SIGNAL(lockFailed()));
    QSignalSpy lockStatusChangedSignal(&camera, SIGNAL(lockStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason)));

    camera.searchAndLock();
    QCOMPARE(camera.lockStatus(), QCamera::Locked);
    QCOMPARE(camera.lockStatus(QCamera::LockExposure), QCamera::Locked);
    QCOMPARE(camera.lockStatus(QCamera::LockFocus), QCamera::Locked);
    QCOMPARE(camera.lockStatus(QCamera::LockWhiteBalance), QCamera::Locked);
    QCOMPARE(lockedSignal.count(), 1);
    QCOMPARE(lockFailedSignal.count(), 0);
    QCOMPARE(lockStatusChangedSignal.count(), 1);

    lockedSignal.clear();
    lockFailedSignal.clear();
    lockStatusChangedSignal.clear();

    camera.unlock();
    QCOMPARE(camera.lockStatus(), QCamera::Unlocked);
    QCOMPARE(camera.lockStatus(QCamera::LockExposure), QCamera::Unlocked);
    QCOMPARE(camera.lockStatus(QCamera::LockFocus), QCamera::Unlocked);
    QCOMPARE(camera.lockStatus(QCamera::LockWhiteBalance), QCamera::Unlocked);

    QCOMPARE(lockedSignal.count(), 0);
    QCOMPARE(lockFailedSignal.count(), 0);
    QCOMPARE(lockStatusChangedSignal.count(), 1);
}

void tst_QCamera::testCameraCapture()
{
    MockCameraService service;
    provider->service = &service;
    QCamera camera(0, provider);
    QCameraImageCapture imageCapture(&camera);


    QVERIFY(!imageCapture.isReadyForCapture());

    QSignalSpy capturedSignal(&imageCapture, SIGNAL(imageCaptured(int,QImage)));
    QSignalSpy errorSignal(&imageCapture, SIGNAL(error(QCameraImageCapture::Error)));

    imageCapture.capture(QString::fromLatin1("/dev/null"));
    QCOMPARE(capturedSignal.size(), 0);
    QCOMPARE(errorSignal.size(), 1);
    QCOMPARE(imageCapture.error(), QCameraImageCapture::NotReadyError);

    errorSignal.clear();

    camera.start();
    QVERIFY(imageCapture.isReadyForCapture());
    QCOMPARE(errorSignal.size(), 0);

    imageCapture.capture(QString::fromLatin1("/dev/null"));

    for (int i=0; i<100 && capturedSignal.isEmpty(); i++)
        QTest::qWait(10);

    QCOMPARE(capturedSignal.size(), 1);
    QCOMPARE(errorSignal.size(), 0);
    QCOMPARE(imageCapture.error(), QCameraImageCapture::NoError);
}


void tst_QCamera::testCameraWhiteBalance()
{
    QSet<QCameraImageProcessing::WhiteBalanceMode> whiteBalanceModes;
    whiteBalanceModes << QCameraImageProcessing::WhiteBalanceAuto;
    whiteBalanceModes << QCameraImageProcessing::WhiteBalanceFlash;
    whiteBalanceModes << QCameraImageProcessing::WhiteBalanceIncandescent;

    MockCameraService service;
    service.mockImageProcessingControl->setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceFlash);
    service.mockImageProcessingControl->setSupportedWhiteBalanceModes(whiteBalanceModes);
    service.mockImageProcessingControl->setManualWhiteBalance(34);

    MockProvider provider;
    provider.service = &service;

    QCamera camera(0, &provider);
    QCameraImageProcessing *cameraImageProcessing = camera.imageProcessing();

    QCOMPARE(cameraImageProcessing->whiteBalanceMode(), QCameraImageProcessing::WhiteBalanceFlash);
    QVERIFY(camera.imageProcessing()->isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceAuto));
    QVERIFY(camera.imageProcessing()->isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceFlash));
    QVERIFY(camera.imageProcessing()->isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceIncandescent));
    QVERIFY(!camera.imageProcessing()->isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceCloudy));

    cameraImageProcessing->setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceIncandescent);
    QCOMPARE(cameraImageProcessing->whiteBalanceMode(), QCameraImageProcessing::WhiteBalanceIncandescent);

    cameraImageProcessing->setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceManual);
    QCOMPARE(cameraImageProcessing->whiteBalanceMode(), QCameraImageProcessing::WhiteBalanceManual);
    QCOMPARE(cameraImageProcessing->manualWhiteBalance(), 34);

    cameraImageProcessing->setManualWhiteBalance(432);
    QCOMPARE(cameraImageProcessing->manualWhiteBalance(), 432);
}

void tst_QCamera::testCameraExposure()
{
    MockCameraService service;
    provider->service = &service;
    QCamera camera(0, provider);

    QCameraExposure *cameraExposure = camera.exposure();
    QVERIFY(cameraExposure != 0);

    QVERIFY(cameraExposure->isExposureModeSupported(QCameraExposure::ExposureAuto));
    QCOMPARE(cameraExposure->exposureMode(), QCameraExposure::ExposureAuto);
    cameraExposure->setExposureMode(QCameraExposure::ExposureManual);
    QCOMPARE(cameraExposure->exposureMode(), QCameraExposure::ExposureManual);

    QCOMPARE(cameraExposure->flashMode(), QCameraExposure::FlashAuto);
    QCOMPARE(cameraExposure->isFlashReady(), true);
    cameraExposure->setFlashMode(QCameraExposure::FlashOn);
    QCOMPARE(cameraExposure->flashMode(), QCameraExposure::FlashOn);

    cameraExposure->setFlashMode(QCameraExposure::FlashRedEyeReduction); // not expected to be supported
    QCOMPARE(cameraExposure->flashMode(), QCameraExposure::FlashOn);

    QCOMPARE(cameraExposure->meteringMode(), QCameraExposure::MeteringMatrix);
    cameraExposure->setMeteringMode(QCameraExposure::MeteringAverage);
    QCOMPARE(cameraExposure->meteringMode(), QCameraExposure::MeteringAverage);
    cameraExposure->setMeteringMode(QCameraExposure::MeteringSpot);
    QCOMPARE(cameraExposure->meteringMode(), QCameraExposure::MeteringAverage);


    QCOMPARE(cameraExposure->exposureCompensation(), 0.0);
    cameraExposure->setExposureCompensation(2.0);
    QCOMPARE(cameraExposure->exposureCompensation(), 2.0);

    int minIso = cameraExposure->supportedIsoSensitivities().first();
    int maxIso = cameraExposure->supportedIsoSensitivities().last();
    QVERIFY(cameraExposure->isoSensitivity() > 0);
    QVERIFY(minIso > 0);
    QVERIFY(maxIso > 0);
    cameraExposure->setManualIsoSensitivity(minIso);
    QCOMPARE(cameraExposure->isoSensitivity(), minIso);
    cameraExposure->setManualIsoSensitivity(maxIso*10);
    QCOMPARE(cameraExposure->isoSensitivity(), maxIso);
    cameraExposure->setManualIsoSensitivity(-10);
    QCOMPARE(cameraExposure->isoSensitivity(), minIso);
    cameraExposure->setAutoIsoSensitivity();
    QCOMPARE(cameraExposure->isoSensitivity(), 100);

    qreal minAperture = cameraExposure->supportedApertures().first();
    qreal maxAperture = cameraExposure->supportedApertures().last();
    QVERIFY(minAperture > 0);
    QVERIFY(maxAperture > 0);
    QVERIFY(cameraExposure->aperture() >= minAperture);
    QVERIFY(cameraExposure->aperture() <= maxAperture);

    cameraExposure->setAutoAperture();
    QVERIFY(cameraExposure->aperture() >= minAperture);
    QVERIFY(cameraExposure->aperture() <= maxAperture);

    cameraExposure->setManualAperture(0);
    QCOMPARE(cameraExposure->aperture(), minAperture);

    cameraExposure->setManualAperture(10000);
    QCOMPARE(cameraExposure->aperture(), maxAperture);


    qreal minShutterSpeed = cameraExposure->supportedShutterSpeeds().first();
    qreal maxShutterSpeed = cameraExposure->supportedShutterSpeeds().last();
    QVERIFY(minShutterSpeed > 0);
    QVERIFY(maxShutterSpeed > 0);
    QVERIFY(cameraExposure->shutterSpeed() >= minShutterSpeed);
    QVERIFY(cameraExposure->shutterSpeed() <= maxShutterSpeed);

    cameraExposure->setAutoShutterSpeed();
    QVERIFY(cameraExposure->shutterSpeed() >= minShutterSpeed);
    QVERIFY(cameraExposure->shutterSpeed() <= maxShutterSpeed);

    cameraExposure->setManualShutterSpeed(0);
    QCOMPARE(cameraExposure->shutterSpeed(), minShutterSpeed);

    cameraExposure->setManualShutterSpeed(10000);
    QCOMPARE(cameraExposure->shutterSpeed(), maxShutterSpeed);
}

void tst_QCamera::testCameraFocus()
{
    MockCameraService service;
    provider->service = &service;
    QCamera camera(0, provider);

    QCameraFocus *cameraFocus = camera.focus();
    QVERIFY(cameraFocus != 0);

    QVERIFY(cameraFocus->isFocusModeSupported(QCameraFocus::AutoFocus));
    QVERIFY(cameraFocus->isFocusModeSupported(QCameraFocus::ContinuousFocus));
    QVERIFY(!cameraFocus->isFocusModeSupported(QCameraFocus::InfinityFocus));

    QCOMPARE(cameraFocus->focusMode(), QCameraFocus::AutoFocus);
    cameraFocus->setFocusMode(QCameraFocus::ManualFocus);
    QCOMPARE(cameraFocus->focusMode(), QCameraFocus::AutoFocus);
    cameraFocus->setFocusMode(QCameraFocus::ContinuousFocus);
    QCOMPARE(cameraFocus->focusMode(), QCameraFocus::ContinuousFocus);

    QVERIFY(cameraFocus->maximumOpticalZoom() >= 1.0);
    QVERIFY(cameraFocus->maximumDigitalZoom() >= 1.0);
    QCOMPARE(cameraFocus->opticalZoom(), 1.0);
    QCOMPARE(cameraFocus->digitalZoom(), 1.0);
    cameraFocus->zoomTo(0.5, 1.0);
    QCOMPARE(cameraFocus->opticalZoom(), 1.0);
    QCOMPARE(cameraFocus->digitalZoom(), 1.0);
    cameraFocus->zoomTo(2.0, 0.5);
    QCOMPARE(cameraFocus->opticalZoom(), 2.0);
    QCOMPARE(cameraFocus->digitalZoom(), 1.0);
    cameraFocus->zoomTo(2.0, 2.5);
    QCOMPARE(cameraFocus->opticalZoom(), 2.0);
    QCOMPARE(cameraFocus->digitalZoom(), 2.5);
    cameraFocus->zoomTo(2000000.0, 1000000.0);
    QVERIFY(qFuzzyCompare(cameraFocus->opticalZoom(), cameraFocus->maximumOpticalZoom()));
    QVERIFY(qFuzzyCompare(cameraFocus->digitalZoom(), cameraFocus->maximumDigitalZoom()));

    QVERIFY(cameraFocus->isFocusPointModeSupported(QCameraFocus::FocusPointAuto));
    QVERIFY(cameraFocus->isFocusPointModeSupported(QCameraFocus::FocusPointCenter));
    QVERIFY(cameraFocus->isFocusPointModeSupported(QCameraFocus::FocusPointCustom));
    QCOMPARE(cameraFocus->focusPointMode(), QCameraFocus::FocusPointAuto);

    cameraFocus->setFocusPointMode( QCameraFocus::FocusPointCenter );
    QCOMPARE(cameraFocus->focusPointMode(), QCameraFocus::FocusPointCenter);

    cameraFocus->setFocusPointMode( QCameraFocus::FocusPointFaceDetection );
    QCOMPARE(cameraFocus->focusPointMode(), QCameraFocus::FocusPointCenter);

    QCOMPARE(cameraFocus->customFocusPoint(), QPointF(0.5, 0.5));
    cameraFocus->setCustomFocusPoint(QPointF(1.0, 1.0));
    QCOMPARE(cameraFocus->customFocusPoint(), QPointF(1.0, 1.0));
}

void tst_QCamera::testImageSettings()
{
    QImageEncoderSettings settings;
    QVERIFY(settings.isNull());
    QVERIFY(settings == QImageEncoderSettings());

    QCOMPARE(settings.codec(), QString());
    settings.setCodec(QLatin1String("codecName"));
    QCOMPARE(settings.codec(), QLatin1String("codecName"));
    QVERIFY(!settings.isNull());
    QVERIFY(settings != QImageEncoderSettings());

    settings = QImageEncoderSettings();
    QCOMPARE(settings.quality(), QtMultimediaKit::NormalQuality);
    settings.setQuality(QtMultimediaKit::HighQuality);
    QCOMPARE(settings.quality(), QtMultimediaKit::HighQuality);
    QVERIFY(!settings.isNull());

    settings = QImageEncoderSettings();
    QCOMPARE(settings.resolution(), QSize());
    settings.setResolution(QSize(320,240));
    QCOMPARE(settings.resolution(), QSize(320,240));
    settings.setResolution(800,600);
    QCOMPARE(settings.resolution(), QSize(800,600));
    QVERIFY(!settings.isNull());

    settings = QImageEncoderSettings();
    QVERIFY(settings.isNull());
    QCOMPARE(settings.codec(), QString());
    QCOMPARE(settings.quality(), QtMultimediaKit::NormalQuality);
    QCOMPARE(settings.resolution(), QSize());

    {
        QImageEncoderSettings settings1;
        QImageEncoderSettings settings2;
        QCOMPARE(settings2, settings1);

        settings2 = settings1;
        QCOMPARE(settings2, settings1);
        QVERIFY(settings2.isNull());

        settings1.setQuality(QtMultimediaKit::HighQuality);

        QVERIFY(settings2.isNull());
        QVERIFY(!settings1.isNull());
        QVERIFY(settings1 != settings2);
    }

    {
        QImageEncoderSettings settings1;
        QImageEncoderSettings settings2(settings1);
        QCOMPARE(settings2, settings1);

        settings2 = settings1;
        QCOMPARE(settings2, settings1);
        QVERIFY(settings2.isNull());

        settings1.setQuality(QtMultimediaKit::HighQuality);

        QVERIFY(settings2.isNull());
        QVERIFY(!settings1.isNull());
        QVERIFY(settings1 != settings2);
    }

    QImageEncoderSettings settings1;
    QImageEncoderSettings settings2;

    settings1 = QImageEncoderSettings();
    settings1.setResolution(800,600);
    settings2 = QImageEncoderSettings();
    settings2.setResolution(QSize(800,600));
    QVERIFY(settings1 == settings2);
    settings2.setResolution(QSize(400,300));
    QVERIFY(settings1 != settings2);

    settings1 = QImageEncoderSettings();
    settings1.setCodec("codec1");
    settings2 = QImageEncoderSettings();
    settings2.setCodec("codec1");
    QVERIFY(settings1 == settings2);
    settings2.setCodec("codec2");
    QVERIFY(settings1 != settings2);

    settings1 = QImageEncoderSettings();
    settings1.setQuality(QtMultimediaKit::NormalQuality);
    settings2 = QImageEncoderSettings();
    settings2.setQuality(QtMultimediaKit::NormalQuality);
    QVERIFY(settings1 == settings2);
    settings2.setQuality(QtMultimediaKit::LowQuality);
    QVERIFY(settings1 != settings2);
}

void tst_QCamera::testCameraLock()
{
    MockCameraService service;
    provider->service = &service;
    QCamera camera(0, provider);

    camera.focus()->setFocusMode(QCameraFocus::AutoFocus);

    QCOMPARE(camera.lockStatus(), QCamera::Unlocked);

    QSignalSpy lockedSignal(&camera, SIGNAL(locked()));
    QSignalSpy lockFailedSignal(&camera, SIGNAL(lockFailed()));
    QSignalSpy lockStatusChangedSignal(&camera, SIGNAL(lockStatusChanged(QCamera::LockStatus,QCamera::LockChangeReason)));

    camera.searchAndLock();
    QCOMPARE(camera.lockStatus(), QCamera::Searching);
    QCOMPARE(lockedSignal.count(), 0);
    QCOMPARE(lockFailedSignal.count(), 0);
    QCOMPARE(lockStatusChangedSignal.count(), 1);

    lockedSignal.clear();
    lockFailedSignal.clear();
    lockStatusChangedSignal.clear();

    for (int i=0; i<200 && camera.lockStatus() == QCamera::Searching; i++)
        QTest::qWait(10);

    QCOMPARE(camera.lockStatus(), QCamera::Locked);
    QCOMPARE(lockedSignal.count(), 1);
    QCOMPARE(lockFailedSignal.count(), 0);
    QCOMPARE(lockStatusChangedSignal.count(), 1);

    lockedSignal.clear();
    lockFailedSignal.clear();
    lockStatusChangedSignal.clear();

    camera.searchAndLock();
    QCOMPARE(camera.lockStatus(), QCamera::Searching);
    QCOMPARE(lockedSignal.count(), 0);
    QCOMPARE(lockFailedSignal.count(), 0);
    QCOMPARE(lockStatusChangedSignal.count(), 1);

    lockedSignal.clear();
    lockFailedSignal.clear();
    lockStatusChangedSignal.clear();

    camera.unlock();
    QCOMPARE(camera.lockStatus(), QCamera::Unlocked);
    QCOMPARE(lockedSignal.count(), 0);
    QCOMPARE(lockFailedSignal.count(), 0);
    QCOMPARE(lockStatusChangedSignal.count(), 1);
}

void tst_QCamera::testCameraLockCancel()
{
    MockCameraService service;
    provider->service = &service;
    QCamera camera(0, provider);

    camera.focus()->setFocusMode(QCameraFocus::AutoFocus);

    QCOMPARE(camera.lockStatus(), QCamera::Unlocked);

    QSignalSpy lockedSignal(&camera, SIGNAL(locked()));
    QSignalSpy lockFailedSignal(&camera, SIGNAL(lockFailed()));
    QSignalSpy lockStatusChangedSignal(&camera, SIGNAL(lockStatusChanged(QCamera::LockStatus, QCamera::LockChangeReason)));
    camera.searchAndLock();
    QCOMPARE(camera.lockStatus(), QCamera::Searching);
    QCOMPARE(lockedSignal.count(), 0);
    QCOMPARE(lockFailedSignal.count(), 0);
    QCOMPARE(lockStatusChangedSignal.count(), 1);

    lockedSignal.clear();
    lockFailedSignal.clear();
    lockStatusChangedSignal.clear();

    camera.unlock();
    QCOMPARE(camera.lockStatus(), QCamera::Unlocked);
    QCOMPARE(lockedSignal.count(), 0);
    QCOMPARE(lockFailedSignal.count(), 0);
    QCOMPARE(lockStatusChangedSignal.count(), 1);
}


QTEST_MAIN(tst_QCamera)

#include "tst_qcamera.moc"
