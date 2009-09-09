/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QDebug>
#include <qcameracontrol.h>
#include <qcameraexposurecontrol.h>
#include <qcamerafocuscontrol.h>
#include <qcameraservice.h>
#include <qcamera.h>


class MockCameraControl : public QCameraControl
{
    Q_OBJECT
public:
    MockCameraControl():QCameraControl(0), m_state(QCamera::StoppedState) {}

    void setDevice(const QString& device) { m_device = device; }

    void start() { m_state = QCamera::ActiveState; };
    virtual void stop() { m_state = QCamera::StoppedState; }
    QCamera::State state() const { return m_state; }

    QCamera::State m_state;
    QString m_device;
};

class MockCameraExposureControl : public QCameraExposureControl
{
    Q_OBJECT
public:
    MockCameraExposureControl():QCameraExposureControl(0),
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

    QCamera::FlashMode flashMode() const
    {
        return m_flashMode;
    }

    void setFlashMode(QCamera::FlashMode mode)
    {
        if (supportedFlashModes() & mode) {
            m_flashMode = mode;
        }
    }

    QCamera::FlashModes supportedFlashModes() const
    {
        return QCamera::FlashAuto | QCamera::FlashOff | QCamera::FlashOn;
    }

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

    QPair<int, int> supportedIsoSensitivityRange() const
    {
        return qMakePair<int,int>(50, 3200);
    }

    void setManualIsoSensitivity(int iso)
    {
        m_isoSensitivity = qBound(50, iso, 3200);
    }

    void setAutoIsoSensitivity()
    {
        m_isoSensitivity = 100;
    }

    qreal aperture() const
    {
        return m_aperture;
    }

    QPair<qreal, qreal> supportedApertureRange() const
    {
        return qMakePair<qreal,qreal>(2.8, 16.0);
    }
    void setManualAperture(qreal aperture)
    {
        m_aperture = qBound(2.8, aperture, 16.0);
    }

    void setAutoAperture()
    {
        m_aperture = 2.8;
    }

    qreal shutterSpeed() const
    {
        return m_shutterSpeed;
    }

    QPair<qreal, qreal> supportedShutterSpeedRange() const
    {
        return qMakePair<qreal,qreal>(0.001, 30.0);
    }
    void setManualShutterSpeed(qreal shutterSpeed)
    {
        m_shutterSpeed = qBound(0.001, shutterSpeed, 30.0);
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
    QCamera::FlashMode m_flashMode;
};

class MockSimpleCameraService : public QCameraService
{
    Q_OBJECT

public:
    MockSimpleCameraService(): QCameraService(0)
    {
        mockControl = new MockCameraControl;
    }

    QAbstractMediaControl* control(const char *iid) const
    {
        if (qstrcmp(iid, QCameraControl_iid) == 0)
            return mockControl;

        return 0;
    }

    MockCameraControl *mockControl;
};

class MockCameraService : public QCameraService
{
    Q_OBJECT

public:
    MockCameraService(): QCameraService(0)
    {
        mockControl = new MockCameraControl;
        mockExposureControl = new MockCameraExposureControl;
    }

    QAbstractMediaControl* control(const char *iid) const
    {
        if (qstrcmp(iid, QCameraControl_iid) == 0)
            return mockControl;

        if (qstrcmp(iid, QCameraExposureControl_iid) == 0)
            return mockExposureControl;

        return 0;
    }

    MockCameraControl *mockControl;
    MockCameraExposureControl *mockExposureControl;
};



class tst_QCamera: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();


private slots:
    void testSimpleCamera();
    void testSimpleCameraWhiteBalance();
    void testSimpleCameraExposure();
    void testSimpleCameraFocus();

    void testCameraExposure();

private:
    MockSimpleCameraService  *mockSimpleCameraService;
};

void tst_QCamera::initTestCase()
{
    mockSimpleCameraService = new MockSimpleCameraService;
}

void tst_QCamera::cleanupTestCase()
{
    delete mockSimpleCameraService;
}


void tst_QCamera::testSimpleCamera()
{
    QCamera camera(0, mockSimpleCameraService);

    QVERIFY(camera.isValid());
    QCOMPARE(camera.state(), QCamera::StoppedState);
    camera.start();
    QCOMPARE(camera.state(), QCamera::ActiveState);
}

void tst_QCamera::testSimpleCameraWhiteBalance()
{
    QCamera camera(0, mockSimpleCameraService);

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
    QCamera camera(0, mockSimpleCameraService);

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
    QCOMPARE(camera.supportedIsoSensitivityRange().first, -1);
    QCOMPARE(camera.supportedIsoSensitivityRange().second, -1);
    camera.setManualIsoSensitivity(100);
    QCOMPARE(camera.isoSensitivity(), -1);

    QVERIFY(camera.aperture() < 0);
    QVERIFY(camera.supportedApertureRange().first < 0);
    QVERIFY(camera.supportedApertureRange().second < 0);
    camera.setAutoAperture();
    QVERIFY(camera.aperture() < 0);
    camera.setManualAperture(5.6);
    QVERIFY(camera.aperture() < 0);

    QVERIFY(camera.shutterSpeed() < 0);
    QVERIFY(camera.supportedShutterSpeedRange().first < 0);
    QVERIFY(camera.supportedShutterSpeedRange().second < 0);
    camera.setAutoShutterSpeed();
    QVERIFY(camera.shutterSpeed() < 0);
    camera.setManualShutterSpeed(1/128.0);
    QVERIFY(camera.shutterSpeed() < 0);

    QCOMPARE(camera.isExposureLocked(), true);

}

void tst_QCamera::testSimpleCameraFocus()
{
    QCamera camera(0, mockSimpleCameraService);

    QCOMPARE(camera.supportedFocusModes(), QCamera::AutoFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    camera.setFocusMode(QCamera::ContinuousFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    QCOMPARE(camera.focusStatus(), QCamera::FocusDisabled);

    QVERIFY(!camera.isMacroFocusingSupported());
    QVERIFY(!camera.macroFocusingEnabled());
    camera.setMacroFocusingEnabled(true);
    QVERIFY(!camera.macroFocusingEnabled());

    QCOMPARE(camera.maximumOpticalZoom(), 1.0);
    QCOMPARE(camera.maximumDigitalZoom(), 1.0);
    QCOMPARE(camera.zoomValue(), 1.0);
    camera.zoomTo(100);
    QCOMPARE(camera.zoomValue(), 1.0);

    QCOMPARE(camera.isFocusLocked(), true);
}

void tst_QCamera::testCameraExposure()
{
    QCamera camera(0, new MockCameraService);

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

    int minIso = camera.supportedIsoSensitivityRange().first;
    int maxIso = camera.supportedIsoSensitivityRange().second;
    QVERIFY(camera.isoSensitivity() > 0);
    QVERIFY(minIso > 0);
    QVERIFY(maxIso > 0);
    camera.setManualIsoSensitivity(100);
    QCOMPARE(camera.isoSensitivity(), 100);
    camera.setManualIsoSensitivity(maxIso*10);
    QCOMPARE(camera.isoSensitivity(), maxIso);
    camera.setManualIsoSensitivity(-10);
    QCOMPARE(camera.isoSensitivity(), minIso);

    qreal minAperture = camera.supportedApertureRange().first;
    qreal maxAperture = camera.supportedApertureRange().second;
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


    qreal minShutterSpeed = camera.supportedShutterSpeedRange().first;
    qreal maxShutterSpeed = camera.supportedShutterSpeedRange().second;
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


QTEST_MAIN(tst_QCamera)

#include "tst_qcamera.moc"
