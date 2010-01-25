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


#define QTRY_COMPARE(a,e)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if ((a) == (e)) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QCOMPARE(a, e)

#define QTRY_VERIFY(a)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if (a) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QVERIFY(a)

QTM_USE_NAMESPACE

class tst_QCamera: public QObject
{
    Q_OBJECT

private:
    
    QString capturePath(QDir dir);
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

};

void tst_QCamera::initTestCase()
{
    qRegisterMetaType<QCamera::State>("QCamera::State");
    qRegisterMetaType<QCamera::Error>("QCamera::Error");
}

void tst_QCamera::cleanupTestCase()
{
}
QString tst_QCamera::capturePath(QDir dir)
{
    int lastImage = 0;
    foreach( QString fileName, dir.entryList(QStringList() << "img_*.jpg") ) {
        int imgNumber = fileName.mid(4, fileName.size()-8).toInt();
        lastImage = qMax(lastImage, imgNumber);
    }

    return dir.absolutePath() + QDir::separator()  + QString("img_%1.jpg").arg(lastImage+1,
        4, //fieldWidth
        10,
        QLatin1Char('0'));
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
    QCamera camera(0);
    
    QSignalSpy spyAdd(&camera, SIGNAL(stateChanged(QCamera::State)));
    QVERIFY(camera.service() != NULL);

    QCOMPARE(camera.state(), QCamera::StoppedState);
    camera.start();
    QTRY_COMPARE(spyAdd.count(), 1);
    
    QCOMPARE(camera.state(), QCamera::ActiveState);
    camera.stop();
    QTRY_COMPARE(spyAdd.count(), 2);
    QCOMPARE(camera.state(), QCamera::StoppedState);
}

void tst_QCamera::testSimpleCameraWhiteBalance()
{
    QCamera camera(0);

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
    QCamera camera(0);

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
    QCamera camera(0);

    QCOMPARE(camera.supportedFocusModes(), QCamera::AutoFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    camera.setFocusMode(QCamera::ContinuousFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    QCOMPARE(camera.focusStatus(), QCamera::FocusInitial);

    QVERIFY(!camera.isMacroFocusingSupported());
    QVERIFY(!camera.macroFocusingEnabled());
    // macro focusing not supported
    //camera.setMacroFocusingEnabled(true);
    //QVERIFY(!camera.macroFocusingEnabled());

    QCOMPARE(camera.maximumOpticalZoom(), 1.0);
    QCOMPARE(camera.maximumDigitalZoom(), 60.0);
    QCOMPARE(camera.zoomValue(), 1.0);
    camera.zoomTo(100);
    QCOMPARE(camera.zoomValue(), 1.0);

}

void tst_QCamera::testSimpleCameraCapture()
{
    // This tests that taking a picture works with default setup
    QCamera camera(0);
    QStillImageCapture imageCapture(&camera);
    // we need to start camera before we take images.
    QSignalSpy cameraState(&camera, SIGNAL(stateChanged(QCamera::State)));
    QSignalSpy capturedSignal(&imageCapture, SIGNAL(imageCaptured(QString,QImage)));
    QSignalSpy errorSignal(&imageCapture, SIGNAL(error(QStillImageCapture::Error)));
    QSignalSpy imageSavedSignal(&imageCapture, SIGNAL(imageSaved(const QString&)));
   
    QCOMPARE(camera.state(), QCamera::StoppedState);
    camera.start(); //we need to start camera before taking picture
    QTRY_COMPARE(cameraState.count(), 1); // wait for camera to initialize itself
    QCOMPARE(camera.state(), QCamera::ActiveState);
    QVERIFY(imageCapture.isAvailable());
    QVERIFY(imageCapture.isReadyForCapture());
    
    QCOMPARE(imageCapture.error(), QStillImageCapture::NoError);
    QVERIFY(imageCapture.errorString().isEmpty());
    
    imageCapture.capture(capturePath(QDir::rootPath()));
    QTRY_COMPARE(capturedSignal.count(), 1);
    QTRY_COMPARE(imageSavedSignal.count(), 1);
    
    QCOMPARE(errorSignal.size(), 0);
        
//    camera.stop();
}

void tst_QCamera::testCameraCapture()
{
    QCamera camera(0);
    QStillImageCapture imageCapture(&camera);
    // check we have some controls and devices
    //QVERIFY(!imageCapture.isAvailable());
    QVERIFY(!imageCapture.isReadyForCapture());

    QSignalSpy cameraState(&camera, SIGNAL(stateChanged(QCamera::State)));
    QSignalSpy capturedSignal(&imageCapture, SIGNAL(imageCaptured(QString,QImage)));
    QSignalSpy errorSignal(&imageCapture, SIGNAL(error(QStillImageCapture::Error)));
    QSignalSpy cameraErrorSignal(&camera, SIGNAL(error(QCamera::Error)));
    QSignalSpy imageSavedSignal(&imageCapture, SIGNAL(imageSaved(const QString&)));
    
    // case where we have not started the camera
    // we shoul receive NotReadyToCaptureError
    imageCapture.capture(QString::fromLatin1("/dev/null"));
    QCOMPARE(capturedSignal.size(), 0);
    
    qDebug()<<"Camera errorNumber=" <<camera.error();
    qDebug()<<"Camera errorText="  <<camera.errorString();
    
    QCOMPARE(cameraErrorSignal.size(), 1);
    QCOMPARE(camera.error(), QCamera::NotReadyToCaptureError);
    QCOMPARE(errorSignal.size(), 1);
    QCOMPARE(imageCapture.error(), QStillImageCapture::NotReadyError);
    QCOMPARE(camera.state(), QCamera::StoppedState);

    // Phase 2 test taking picture to a real path
    errorSignal.clear();
    cameraErrorSignal.clear();
    cameraState.clear();
    capturedSignal.clear();
    imageSavedSignal.clear();
    
    //we need to start camera before taking picture. this is the difference between previous test 
    camera.start();
    QTRY_COMPARE (errorSignal.count(), 0); // We should not have any error code.
    QTRY_COMPARE (cameraState.count(), 1); // wait for camera to initialize itself
    QCOMPARE(camera.state(), QCamera::ActiveState);
    QVERIFY(imageCapture.isReadyForCapture());
    imageCapture.capture(QString::fromLatin1("/dev/null"));
    // we should receive captured signal, but no imageSavedSignal()
    QCOMPARE(capturedSignal.size(), 1);
    QCOMPARE(errorSignal.size(), 0);
    QCOMPARE(cameraErrorSignal.size(), 0);
    // as this is no good place to save image we shouldn't have imageSaved signal
    QCOMPARE(imageSavedSignal.size(), 0);
    QCOMPARE(imageCapture.error(), QStillImageCapture::ResourceError);
    
    camera.stop();
}

void tst_QCamera::testCameraWhiteBalance()
{
    QCamera::WhiteBalanceModes whiteBalanceModes
            = QCamera::WhiteBalanceAuto
            | QCamera::WhiteBalanceFlash
            | QCamera::WhiteBalanceIncandescent;

    QCamera camera(0);

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
    QCamera camera(0);

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
    QCamera camera(0);

    QCOMPARE(camera.supportedFocusModes(), QCamera::AutoFocus /*| QCamera::ContinuousFocus*/);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    camera.setFocusMode(QCamera::ManualFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    camera.setFocusMode(QCamera::ContinuousFocus);
    QCOMPARE(camera.focusMode(), QCamera::AutoFocus);
    QCOMPARE(camera.focusStatus(), QCamera::FocusInitial);

    QVERIFY(camera.isMacroFocusingSupported());
    QVERIFY(!camera.macroFocusingEnabled());
    // macro focusing not supported
    //camera.setMacroFocusingEnabled(true);
    //QVERIFY(camera.macroFocusingEnabled());

    QVERIFY(camera.maximumOpticalZoom() >= 1.0);
    QVERIFY(camera.maximumDigitalZoom() >= 1.0);
    QCOMPARE(camera.zoomValue(), 1.0);
    camera.zoomTo(0.5);
    QCOMPARE(camera.zoomValue(), 1.0);
    camera.zoomTo(2.0);
    QCOMPARE(camera.zoomValue(), 2.0);
    camera.zoomTo(2000000.0);
    QVERIFY(qFuzzyCompare(camera.zoomValue(), camera.maximumOpticalZoom()*camera.maximumDigitalZoom()));

    /* TODO: focusing has changed 
    camera.lockFocus();
    QCOMPARE(camera.isFocusLocked(), true);
    camera.unlockFocus();
    QCOMPARE(camera.isFocusLocked(), false);
    */
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
