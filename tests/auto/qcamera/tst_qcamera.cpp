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

#include <experimental/qcameracontrol.h>
#include <experimental/qcameraexposurecontrol.h>
#include <experimental/qcamerafocuscontrol.h>
#include <experimental/qimagecapturecontrol.h>
#include <qimageencodercontrol.h>
#include <experimental/qimageprocessingcontrol.h>
#include <qmediaservice.h>
#include <experimental/qcamera.h>
#include <experimental/qstillimagecapture.h>

QTM_USE_NAMESPACE
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

    QCamera::CaptureMode captureMode() const { return m_captureMode; }
    void setCaptureMode(QCamera::CaptureMode mode)
    {
        if (m_captureMode != mode) {
            m_captureMode = mode;
            emit captureModeChanged(mode);
        }
    }

    QCamera::CaptureModes supportedCaptureModes() const
    {
        return QCamera::CaptureStillImage | QCamera::CaptureVideo;
    }


    QCamera::State m_state;
    QCamera::CaptureMode m_captureMode;
};

class MockCaptureControl : public QImageCaptureControl
{
    Q_OBJECT
public:
    MockCaptureControl(MockCameraControl *cameraControl, QObject *parent = 0)
        :QImageCaptureControl(parent), m_cameraControl(cameraControl)
    {
    }

    ~MockCaptureControl()
    {
    }

    bool isReadyForCapture() const { return m_cameraControl->state() == QCamera::ActiveState; }

    void capture(const QString &fileName)
    {
        if (isReadyForCapture())
            emit imageCaptured(fileName, QImage());
        else
            emit error(QStillImageCapture::NotReadyError,
                       QLatin1String("Could not capture in stopped state"));
    }

    void cancelCapture()
    {
    }

    MockCameraControl *m_cameraControl;

};

class MockCameraExposureControl : public QCameraExposureControl
{
    Q_OBJECT
public:
    MockCameraExposureControl(QObject *parent = 0):
        QCameraExposureControl(parent),
        m_exposureLocked(false),
        m_aperture(2.8),
        m_shutterSpeed(0.01),
        m_isoSensitivity(100),
        m_meteringMode(QCamera::MeteringMatrix),
        m_exposureCompensation(0),
        m_exposureMode(QCamera::ExposureAuto),
        m_flashMode(QCamera::FlashAuto)
    {
    }

    ~MockCameraExposureControl() {}

    QCamera::FlashModes flashMode() const
    {
        return m_flashMode;
    }

    void setFlashMode(QCamera::FlashModes mode)
    {
        if (supportedFlashModes() & mode) {
            m_flashMode = mode;
        }
    }

    QCamera::FlashModes supportedFlashModes() const
    {
        return QCamera::FlashAuto | QCamera::FlashOff | QCamera::FlashOn;
    }

    qreal flashCompensation() const { return 0; }
    void setFlashCompensation(qreal) {}

    qreal flashPower() const { return 0; }
    void setFlashPower(qreal) {}


    bool isFlashReady() const
    {
        return true;
    }

    QCamera::ExposureMode exposureMode() const
    {
        return m_exposureMode;
    }

    void setExposureMode(QCamera::ExposureMode mode)
    {
        if (supportedExposureModes() & mode)
            m_exposureMode = mode;
    }

    QCamera::ExposureModes supportedExposureModes() const
    {
        return QCamera::ExposureAuto | QCamera::ExposureManual;
    }

    qreal exposureCompensation() const
    {
        return m_exposureCompensation;
    }

    void setExposureCompensation(qreal ev)
    {
        m_exposureCompensation = ev;
    }

    QCamera::MeteringMode meteringMode() const
    {
        return m_meteringMode;
    }

    void setMeteringMode(QCamera::MeteringMode mode)
    {
        if (supportedMeteringModes() & mode)
            m_meteringMode = mode;
    }

    QCamera::MeteringModes supportedMeteringModes() const
    {
        return QCamera::MeteringAverage | QCamera::MeteringMatrix;
    }

    int isoSensitivity() const
    {
        return m_isoSensitivity;
    }

    QList<int> supportedIsoSensitivities(bool * continuous) const
    {
        if (continuous)
            *continuous = false;
        return QList<int>() << 100 << 200 << 400 << 800;
    }


    void setManualIsoSensitivity(int iso)
    {
        m_isoSensitivity = 100*qRound(qBound(100, iso, 800)/100.0);
    }

    void setAutoIsoSensitivity()
    {
        m_isoSensitivity = 100;
    }

    qreal aperture() const
    {
        return m_aperture;
    }

    QList<qreal> supportedApertures(bool *continuous) const
    {
        if (continuous)
            *continuous = true;
        return QList<qreal>() << 2.8 << 4 << 5.6 << 8 << 11 << 16;
    }

    void setManualAperture(qreal aperture)
    {
        m_aperture = qBound<qreal>(2.8, aperture, 16.0);
    }

    void setAutoAperture()
    {
        m_aperture = 2.8;
    }

    qreal shutterSpeed() const
    {
        return m_shutterSpeed;
    }

    QList<qreal> supportedShutterSpeeds(bool *continuous) const
    {
        if (continuous)
            *continuous = false;

        return QList<qreal>() << 0.001 << 0.01 << 0.1 << 1.0;
    }

    void setManualShutterSpeed(qreal shutterSpeed)
    {
        m_shutterSpeed = qBound<qreal>(0.001, shutterSpeed, 1.0);
    }

    void setAutoShutterSpeed()
    {
        m_shutterSpeed = 0.01;
    }

    bool isExposureLocked() const
    {
        return m_exposureLocked;
    }

public Q_SLOTS:
    void lockExposure()
    {
        if (!m_exposureLocked) {
            m_exposureLocked = true;
            emit exposureLocked();
        }
    }

    void unlockExposure()
    {
        m_exposureLocked = false;
    }


private:
    bool m_exposureLocked;
    qreal m_aperture;
    qreal m_shutterSpeed;
    int m_isoSensitivity;
    QCamera::MeteringMode m_meteringMode;
    qreal m_exposureCompensation;
    QCamera::ExposureMode m_exposureMode;
    QCamera::FlashModes m_flashMode;
};

class MockCameraFocusControl : public QCameraFocusControl
{
    Q_OBJECT
public:
    MockCameraFocusControl(QObject *parent = 0):
        QCameraFocusControl(parent),
        m_opticalZoom(1.0),
        m_digitalZoom(1.0),
        m_macroFocusingEnabled(false),
        m_focusMode(QCamera::AutoFocus),
        m_focusStatus(QCamera::FocusInitial),
        m_focusPointMode(QCamera::FocusPointAuto),
        m_focusPoint(0.5, 0.5)
    {
    }

    ~MockCameraFocusControl() {}

    QCamera::FocusMode focusMode() const
    {
        return m_focusMode;
    }

    void setFocusMode(QCamera::FocusMode mode)
    {
        if (supportedFocusModes() & mode)
            m_focusMode = mode;
    }

    QCamera::FocusModes supportedFocusModes() const
    {
        return QCamera::AutoFocus | QCamera::ContinuousFocus;
    }

    QCamera::FocusStatus focusStatus() const
    {
        return m_focusStatus;
    }

    bool macroFocusingEnabled() const
    {
        return m_macroFocusingEnabled;
    }

    bool isMacroFocusingSupported() const
    {
        return true;
    }

    void setMacroFocusingEnabled(bool flag)
    {
        if (isMacroFocusingSupported())
            m_macroFocusingEnabled = flag;
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

    QCamera::FocusPointMode focusPointMode() const
    {
        return m_focusPointMode;
    }

    void setFocusPointMode(QCamera::FocusPointMode mode)
    {
        if (mode & supportedFocusPointModes())
            m_focusPointMode = mode;
    }

    QCamera::FocusPointModes supportedFocusPointModes() const
    {
        return QCamera::FocusPointAuto | QCamera::FocusPointCenter | QCamera::FocusPointCustom;
    }

    QPointF customFocusPoint() const
    {
        return m_focusPoint;
    }

    void setCustomFocusPoint(const QPointF &point)
    {
        m_focusPoint = point;
    }

    QList<QRectF> focusZones() const { return QList<QRectF>() << QRectF(0.45, 0.45, 0.1, 0.1); }

public Q_SLOTS:
    void startFocusing()
    {
    }

    void cancelFocusing()
    {
    }

private:
    qreal m_opticalZoom;
    qreal m_digitalZoom;
    bool m_macroFocusingEnabled;
    QCamera::FocusMode m_focusMode;
    QCamera::FocusStatus m_focusStatus;
    QCamera::FocusPointMode m_focusPointMode;
    QPointF m_focusPoint;
};

class MockImageProcessingControl : public QImageProcessingControl
{
    Q_OBJECT
public:
    MockImageProcessingControl(QObject *parent = 0)
        : QImageProcessingControl(parent)
        , m_supportedWhiteBalance(QCamera::WhiteBalanceAuto)
        , m_contrast(0)
        , m_saturation(0)
        , m_sharpeningLevel(-1)
        , m_denoisingLevel(-1)
        , m_sharpeningSupported(false)
        , m_denoisingSupported(true)
        , m_whiteBalanceLocked(false)
    {
    }

    QCamera::WhiteBalanceMode whiteBalanceMode() const { return m_whiteBalanceMode; }
    void setWhiteBalanceMode(QCamera::WhiteBalanceMode mode) { m_whiteBalanceMode = mode; }

    QCamera::WhiteBalanceModes supportedWhiteBalanceModes() const {
        return m_supportedWhiteBalance; }
    void setSupportedWhiteBalanceModes(QCamera::WhiteBalanceModes modes) {
        m_supportedWhiteBalance = modes; }

    int manualWhiteBalance() const { return m_manualWhiteBalance; }
    void setManualWhiteBalance(int colorTemperature) { m_manualWhiteBalance = colorTemperature; }

    int contrast() const { return m_contrast; }
    void setContrast(int value) { m_contrast = value; }

    int saturation() const { return m_saturation; }
    void setSaturation(int value) { m_saturation = value; }

    bool isSharpeningSupported() const { return m_sharpeningSupported; }
    void setSharpendingSupported(bool supported) { m_sharpeningSupported = supported; }

    int sharpeningLevel() const { return m_sharpeningLevel; }
    void setSharpeningLevel(int value) { m_sharpeningLevel = value; }

    bool isDenoisingSupported() const { return m_denoisingSupported; }
    void setDenoisingSupported(bool supported) { m_denoisingSupported = supported; }
    int denoisingLevel() const { return m_denoisingLevel; }
    void setDenoisingLevel(int value) { m_denoisingLevel = value; }

    bool isWhiteBalanceLocked() const { return m_whiteBalanceLocked; }

public Q_SLOTS:
    void lockWhiteBalance() { m_whiteBalanceLocked = true; emit whiteBalanceLocked(); }
    void unlockWhiteBalance() { m_whiteBalanceLocked = false; }

private:
    QCamera::WhiteBalanceMode m_whiteBalanceMode;
    QCamera::WhiteBalanceModes m_supportedWhiteBalance;
    int m_manualWhiteBalance;
    int m_contrast;
    int m_saturation;
    int m_sharpeningLevel;
    int m_denoisingLevel;
    bool m_sharpeningSupported;
    bool m_denoisingSupported;
    bool m_whiteBalanceLocked;
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

    QMediaControl* control(const char *iid) const
    {
        if (qstrcmp(iid, QCameraControl_iid) == 0)
            return mockControl;
        return 0;
    }

    MockCameraControl *mockControl;
};

class MockCameraService : public QMediaService
{
    Q_OBJECT

public:
    MockCameraService(): QMediaService(0)
    {
        mockControl = new MockCameraControl(this);
        mockExposureControl = new MockCameraExposureControl(this);
        mockFocusControl = new MockCameraFocusControl(this);
        mockCaptureControl = new MockCaptureControl(mockControl, this);
        mockImageProcessingControl = new MockImageProcessingControl(this);
        mockImageEncoderControl = new MockImageEncoderControl(this);
    }

    ~MockCameraService()
    {
    }

    QMediaControl* control(const char *iid) const
    {
        if (qstrcmp(iid, QCameraControl_iid) == 0)
            return mockControl;

        if (qstrcmp(iid, QCameraExposureControl_iid) == 0)
            return mockExposureControl;

        if (qstrcmp(iid, QCameraFocusControl_iid) == 0)
            return mockFocusControl;

        if (qstrcmp(iid, QImageCaptureControl_iid) == 0)
            return mockCaptureControl;

        if (qstrcmp(iid, QImageProcessingControl_iid) == 0)
            return mockImageProcessingControl;

        if (qstrcmp(iid, QImageEncoderControl_iid) == 0)
            return mockImageEncoderControl;

        return 0;
    }

    MockCameraControl *mockControl;
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

    void testCameraWhiteBalance();
    void testCameraExposure();
    void testCameraFocus();
    void testCameraCapture();
    void testImageSettings();

private:
    MockSimpleCameraService  *mockSimpleCameraService;
    MockProvider *provider;
};

void tst_QCamera::initTestCase()
{
    provider = new MockProvider;
    mockSimpleCameraService = new MockSimpleCameraService;
    provider->service = mockSimpleCameraService;

    qRegisterMetaType<QCamera::Error>("QCamera::Error");
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
    QCOMPARE(camera.supportedWhiteBalanceModes(), QCamera::WhiteBalanceAuto);
    QCOMPARE(camera.whiteBalanceMode(), QCamera::WhiteBalanceAuto);
    camera.setWhiteBalanceMode(QCamera::WhiteBalanceCloudy);
    QCOMPARE(camera.supportedWhiteBalanceModes(), QCamera::WhiteBalanceAuto);
    QCOMPARE(camera.manualWhiteBalance(), -1);
    camera.setManualWhiteBalance(5000);
    QCOMPARE(camera.manualWhiteBalance(), -1);
}

void tst_QCamera::testSimpleCameraExposure()
{
    QCamera camera(0, provider);

    QCOMPARE(camera.supportedExposureModes(), QCamera::ExposureAuto);
    QCOMPARE(camera.exposureMode(), QCamera::ExposureAuto);
    camera.setExposureMode(QCamera::ExposureManual);//should be ignored
    QCOMPARE(camera.exposureMode(), QCamera::ExposureAuto);

    QCOMPARE(camera.supportedFlashModes(), QCamera::FlashOff);
    QCOMPARE(camera.flashMode(), QCamera::FlashOff);
    QCOMPARE(camera.isFlashReady(), false);
    camera.setFlashMode(QCamera::FlashOn);
    QCOMPARE(camera.flashMode(), QCamera::FlashOff);

    QCOMPARE(camera.supportedMeteringModes(), QCamera::MeteringMatrix);
    QCOMPARE(camera.meteringMode(), QCamera::MeteringMatrix);
    camera.setMeteringMode(QCamera::MeteringSpot);
    QCOMPARE(camera.meteringMode(), QCamera::MeteringMatrix);

    QCOMPARE(camera.exposureCompensation(), 0.0);
    camera.setExposureCompensation(2.0);
    QCOMPARE(camera.exposureCompensation(), 0.0);

    QCOMPARE(camera.isoSensitivity(), -1);    
    QVERIFY(camera.supportedIsoSensitivities().isEmpty());
    camera.setManualIsoSensitivity(100);
    QCOMPARE(camera.isoSensitivity(), -1);
    camera.setAutoIsoSensitivity();
    QCOMPARE(camera.isoSensitivity(), -1);

    QVERIFY(camera.aperture() < 0);
    QVERIFY(camera.supportedApertures().isEmpty());
    camera.setAutoAperture();
    QVERIFY(camera.aperture() < 0);
    camera.setManualAperture(5.6);
    QVERIFY(camera.aperture() < 0);

    QVERIFY(camera.shutterSpeed() < 0);
    QVERIFY(camera.supportedShutterSpeeds().isEmpty());
    camera.setAutoShutterSpeed();
    QVERIFY(camera.shutterSpeed() < 0);
    camera.setManualShutterSpeed(1/128.0);
    QVERIFY(camera.shutterSpeed() < 0);

    QCOMPARE(camera.isExposureLocked(), true);

}

void tst_QCamera::testSimpleCameraFocus()
{
    QCamera camera(0, provider);

    QCOMPARE(camera.supportedFocusModes(), QCamera::AutoFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    camera.setFocusMode(QCamera::ContinuousFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    QCOMPARE(camera.focusStatus(), QCamera::FocusInitial);

    QVERIFY(!camera.isMacroFocusingSupported());
    QVERIFY(!camera.macroFocusingEnabled());
    camera.setMacroFocusingEnabled(true);
    QVERIFY(!camera.macroFocusingEnabled());

    QCOMPARE(camera.maximumOpticalZoom(), 1.0);
    QCOMPARE(camera.maximumDigitalZoom(), 1.0);
    QCOMPARE(camera.opticalZoom(), 1.0);
    QCOMPARE(camera.digitalZoom(), 1.0);
    QSignalSpy errorSignal(&camera, SIGNAL(error(QCamera::Error)));
    camera.zoomTo(100.0, 100.0);
    QCOMPARE(camera.opticalZoom(), 1.0);
    QCOMPARE(camera.digitalZoom(), 1.0);
    QCOMPARE(errorSignal.count(), 1);
    QCOMPARE(camera.error(), QCamera::NotSupportedFeatureError);

    QCOMPARE(camera.supportedFocusPointModes(), QCamera::FocusPointAuto);
    QCOMPARE(camera.focusPointMode(), QCamera::FocusPointAuto);

    camera.setFocusPointMode( QCamera::FocusPointCenter );
    QCOMPARE(camera.focusPointMode(), QCamera::FocusPointAuto);

    QCOMPARE(camera.customFocusPoint(), QPointF(0.5, 0.5));
    camera.setCustomFocusPoint(QPointF(1.0, 1.0));
    QCOMPARE(camera.customFocusPoint(), QPointF(0.5, 0.5));
}

void tst_QCamera::testSimpleCameraCapture()
{
    QCamera camera(0, provider);
    QStillImageCapture imageCapture(&camera);

    QVERIFY(!imageCapture.isReadyForCapture());
    QVERIFY(!imageCapture.isAvailable());

    QCOMPARE(imageCapture.error(), QStillImageCapture::NoError);
    QVERIFY(imageCapture.errorString().isEmpty());

    QSignalSpy errorSignal(&imageCapture, SIGNAL(error(QStillImageCapture::Error)));
    imageCapture.capture(QString::fromLatin1("/dev/null"));
    QCOMPARE(errorSignal.size(), 1);
    QCOMPARE(imageCapture.error(), QStillImageCapture::NotSupportedFeatureError);
    QVERIFY(!imageCapture.errorString().isEmpty());
}

void tst_QCamera::testCameraCapture()
{
    MockCameraService service;
    provider->service = &service;
    QCamera camera(0, provider);
    QStillImageCapture imageCapture(&camera);


    QVERIFY(!imageCapture.isReadyForCapture());

    QSignalSpy capturedSignal(&imageCapture, SIGNAL(imageCaptured(QString,QImage)));
    QSignalSpy errorSignal(&imageCapture, SIGNAL(error(QStillImageCapture::Error)));

    imageCapture.capture(QString::fromLatin1("/dev/null"));
    QCOMPARE(capturedSignal.size(), 0);
    QCOMPARE(errorSignal.size(), 1);
    QCOMPARE(imageCapture.error(), QStillImageCapture::NotReadyError);

    errorSignal.clear();

    camera.start();
    QVERIFY(imageCapture.isReadyForCapture());
    QCOMPARE(errorSignal.size(), 0);

    imageCapture.capture(QString::fromLatin1("/dev/null"));
    QCOMPARE(capturedSignal.size(), 1);
    QCOMPARE(errorSignal.size(), 0);
    QCOMPARE(imageCapture.error(), QStillImageCapture::NoError);
}

void tst_QCamera::testCameraWhiteBalance()
{
    QCamera::WhiteBalanceModes whiteBalanceModes
            = QCamera::WhiteBalanceAuto
            | QCamera::WhiteBalanceFlash
            | QCamera::WhiteBalanceIncandescent;

    MockCameraService service;
    service.mockImageProcessingControl->setWhiteBalanceMode(QCamera::WhiteBalanceFlash);
    service.mockImageProcessingControl->setSupportedWhiteBalanceModes(whiteBalanceModes);
    service.mockImageProcessingControl->setManualWhiteBalance(34);

    MockProvider provider;
    provider.service = &service;

    QCamera camera(0, &provider);

    QCOMPARE(camera.whiteBalanceMode(), QCamera::WhiteBalanceFlash);
    QCOMPARE(camera.supportedWhiteBalanceModes(), whiteBalanceModes);

    camera.setWhiteBalanceMode(QCamera::WhiteBalanceIncandescent);
    QCOMPARE(camera.whiteBalanceMode(), QCamera::WhiteBalanceIncandescent);

    camera.setWhiteBalanceMode(QCamera::WhiteBalanceManual);
    QCOMPARE(camera.whiteBalanceMode(), QCamera::WhiteBalanceManual);
    QCOMPARE(camera.manualWhiteBalance(), 34);

    camera.setManualWhiteBalance(432);
    QCOMPARE(camera.manualWhiteBalance(), 432);
}

void tst_QCamera::testCameraExposure()
{
    MockCameraService service;
    provider->service = &service;
    QCamera camera(0, provider);

    QVERIFY(camera.supportedExposureModes() & QCamera::ExposureAuto);
    QCOMPARE(camera.exposureMode(), QCamera::ExposureAuto);
    camera.setExposureMode(QCamera::ExposureManual);
    QCOMPARE(camera.exposureMode(), QCamera::ExposureManual);

    QCOMPARE(camera.flashMode(), QCamera::FlashAuto);
    QCOMPARE(camera.isFlashReady(), true);
    camera.setFlashMode(QCamera::FlashOn);
    QCOMPARE(camera.flashMode(), QCamera::FlashOn);

    camera.setFlashMode(QCamera::FlashRedEyeReduction); // not expected to be supported
    QCOMPARE(camera.flashMode(), QCamera::FlashOn);

    QCOMPARE(camera.meteringMode(), QCamera::MeteringMatrix);
    camera.setMeteringMode(QCamera::MeteringAverage);
    QCOMPARE(camera.meteringMode(), QCamera::MeteringAverage);
    camera.setMeteringMode(QCamera::MeteringSpot);
    QCOMPARE(camera.meteringMode(), QCamera::MeteringAverage);


    QCOMPARE(camera.exposureCompensation(), 0.0);
    camera.setExposureCompensation(2.0);
    QCOMPARE(camera.exposureCompensation(), 2.0);

    int minIso = camera.supportedIsoSensitivities().first();
    int maxIso = camera.supportedIsoSensitivities().last();
    QVERIFY(camera.isoSensitivity() > 0);
    QVERIFY(minIso > 0);
    QVERIFY(maxIso > 0);
    camera.setManualIsoSensitivity(minIso);
    QCOMPARE(camera.isoSensitivity(), minIso);
    camera.setManualIsoSensitivity(maxIso*10);
    QCOMPARE(camera.isoSensitivity(), maxIso);
    camera.setManualIsoSensitivity(-10);
    QCOMPARE(camera.isoSensitivity(), minIso);
    camera.setAutoIsoSensitivity();
    QCOMPARE(camera.isoSensitivity(), 100);

    qreal minAperture = camera.supportedApertures().first();
    qreal maxAperture = camera.supportedApertures().last();
    QVERIFY(minAperture > 0);
    QVERIFY(maxAperture > 0);
    QVERIFY(camera.aperture() >= minAperture);
    QVERIFY(camera.aperture() <= maxAperture);

    camera.setAutoAperture();
    QVERIFY(camera.aperture() >= minAperture);
    QVERIFY(camera.aperture() <= maxAperture);

    camera.setManualAperture(0);
    QCOMPARE(camera.aperture(), minAperture);

    camera.setManualAperture(10000);
    QCOMPARE(camera.aperture(), maxAperture);


    qreal minShutterSpeed = camera.supportedShutterSpeeds().first();
    qreal maxShutterSpeed = camera.supportedShutterSpeeds().last();
    QVERIFY(minShutterSpeed > 0);
    QVERIFY(maxShutterSpeed > 0);
    QVERIFY(camera.shutterSpeed() >= minShutterSpeed);
    QVERIFY(camera.shutterSpeed() <= maxShutterSpeed);

    camera.setAutoShutterSpeed();
    QVERIFY(camera.shutterSpeed() >= minShutterSpeed);
    QVERIFY(camera.shutterSpeed() <= maxShutterSpeed);

    camera.setManualShutterSpeed(0);
    QCOMPARE(camera.shutterSpeed(), minShutterSpeed);

    camera.setManualShutterSpeed(10000);
    QCOMPARE(camera.shutterSpeed(), maxShutterSpeed);

    QCOMPARE(camera.isExposureLocked(), false);

    camera.lockExposure();
    QCOMPARE(camera.isExposureLocked(), true);

    camera.unlockExposure();
    QCOMPARE(camera.isExposureLocked(), false);
}

void tst_QCamera::testCameraFocus()
{
    MockCameraService service;
    provider->service = &service;
    QCamera camera(0, provider);

    QCOMPARE(camera.supportedFocusModes(), QCamera::AutoFocus | QCamera::ContinuousFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    camera.setFocusMode(QCamera::ManualFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    camera.setFocusMode(QCamera::ContinuousFocus);
    QCOMPARE(camera.focusMode(), QCamera::ContinuousFocus);
    QCOMPARE(camera.focusStatus(), QCamera::FocusInitial);

    QVERIFY(camera.isMacroFocusingSupported());
    QVERIFY(!camera.macroFocusingEnabled());
    camera.setMacroFocusingEnabled(true);
    QVERIFY(camera.macroFocusingEnabled());

    QVERIFY(camera.maximumOpticalZoom() >= 1.0);
    QVERIFY(camera.maximumDigitalZoom() >= 1.0);
    QCOMPARE(camera.opticalZoom(), 1.0);
    QCOMPARE(camera.digitalZoom(), 1.0);
    camera.zoomTo(0.5, 1.0);
    QCOMPARE(camera.opticalZoom(), 1.0);
    QCOMPARE(camera.digitalZoom(), 1.0);
    camera.zoomTo(2.0, 0.5);
    QCOMPARE(camera.opticalZoom(), 2.0);
    QCOMPARE(camera.digitalZoom(), 1.0);
    camera.zoomTo(2.0, 2.5);
    QCOMPARE(camera.opticalZoom(), 2.0);
    QCOMPARE(camera.digitalZoom(), 2.5);
    camera.zoomTo(2000000.0, 1000000.0);
    QVERIFY(qFuzzyCompare(camera.opticalZoom(), camera.maximumOpticalZoom()));
    QVERIFY(qFuzzyCompare(camera.digitalZoom(), camera.maximumDigitalZoom()));

    QCOMPARE(camera.supportedFocusPointModes(),
             QCamera::FocusPointAuto | QCamera::FocusPointCenter | QCamera::FocusPointCustom);
    QCOMPARE(camera.focusPointMode(), QCamera::FocusPointAuto);

    camera.setFocusPointMode( QCamera::FocusPointCenter );
    QCOMPARE(camera.focusPointMode(), QCamera::FocusPointCenter);

    camera.setFocusPointMode( QCamera::FocusPointFaceDetection );
    QCOMPARE(camera.focusPointMode(), QCamera::FocusPointCenter);

    QCOMPARE(camera.customFocusPoint(), QPointF(0.5, 0.5));
    camera.setCustomFocusPoint(QPointF(1.0, 1.0));
    QCOMPARE(camera.customFocusPoint(), QPointF(1.0, 1.0));
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
    QCOMPARE(settings.quality(), QtMedia::NormalQuality);
    settings.setQuality(QtMedia::HighQuality);
    QCOMPARE(settings.quality(), QtMedia::HighQuality);
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
    QCOMPARE(settings.quality(), QtMedia::NormalQuality);
    QCOMPARE(settings.resolution(), QSize());

    {
        QImageEncoderSettings settings1;
        QImageEncoderSettings settings2;
        QCOMPARE(settings2, settings1);

        settings2 = settings1;
        QCOMPARE(settings2, settings1);
        QVERIFY(settings2.isNull());

        settings1.setQuality(QtMedia::HighQuality);

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

        settings1.setQuality(QtMedia::HighQuality);

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
    settings1.setQuality(QtMedia::NormalQuality);
    settings2 = QImageEncoderSettings();
    settings2.setQuality(QtMedia::NormalQuality);
    QVERIFY(settings1 == settings2);
    settings2.setQuality(QtMedia::LowQuality);
    QVERIFY(settings1 != settings2);
}



QTEST_MAIN(tst_QCamera)

#include "tst_qcamera.moc"
