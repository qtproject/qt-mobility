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
#include <qvideowidget.h>


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
    

    void testCameraExposure();
    void testCameraFocus();
    void testCameraCapture();
    void testImageSettings();
    
private:
    QCamera *sharedCamera;
    QVideoWidget *videoWidget;
};

void tst_QCamera::initTestCase()
{
    qRegisterMetaType<QCamera::State>("QCamera::State");
    qRegisterMetaType<QCamera::Error>("QCamera::Error");
    sharedCamera = new QCamera(QCamera::availableDevices().first(), this);
    videoWidget = new QVideoWidget();
    videoWidget->setMediaObject(sharedCamera);
    videoWidget->show();
}

void tst_QCamera::cleanupTestCase()
{
    delete sharedCamera;
    delete videoWidget;
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
    QSignalSpy spyAdd(sharedCamera, SIGNAL(stateChanged(QCamera::State)));
    QVERIFY(sharedCamera->service() != NULL);

    QCOMPARE(sharedCamera->state(), QCamera::StoppedState);
    sharedCamera->start();
    QTRY_COMPARE(spyAdd.count(), 1);
    
    QCOMPARE(sharedCamera->state(), QCamera::ActiveState);
    sharedCamera->stop();
    QTRY_COMPARE(spyAdd.count(), 2);
    QCOMPARE(sharedCamera->state(), QCamera::StoppedState);

    sharedCamera->deleteLater();
}

void tst_QCamera::testSimpleCameraWhiteBalance()
{
    QTimer timer;
    QSignalSpy timerSpy(&timer, SIGNAL(timeout()));
    sharedCamera->start();
    timer.start(5000);
    
    do {
        QTest::qWait(500);
    } while( sharedCamera->state() != QCamera::ActiveState && !timerSpy.count());

    //supported e.g. in 5800XpressMusic: sunlight,cloudy,tungsted,fluorcent.
    QVERIFY(sharedCamera->supportedWhiteBalanceModes() & QCamera::WhiteBalanceAuto |QCamera::WhiteBalanceCloudy);
    QCOMPARE(sharedCamera->whiteBalanceMode(), QCamera::WhiteBalanceAuto);
    if (sharedCamera->state() == QCamera::ActiveState) {
        sharedCamera->setWhiteBalanceMode(QCamera::WhiteBalanceCloudy);
        QCOMPARE(sharedCamera->whiteBalanceMode(), QCamera::WhiteBalanceCloudy);
        QCOMPARE(sharedCamera->manualWhiteBalance(), -1);
        sharedCamera->setManualWhiteBalance(5000);
        QCOMPARE(sharedCamera->manualWhiteBalance(), -1);
    }
    sharedCamera->stop();
}

void tst_QCamera::testSimpleCameraExposure()
{
    QTimer timer;
    QSignalSpy timerSpy(&timer, SIGNAL(timeout()));
    sharedCamera->start();
    timer.start(5000);
    
    do {
        QTest::qWait(500);
    } while( sharedCamera->state() != QCamera::ActiveState && !timerSpy.count());
    
    QVERIFY(sharedCamera->supportedExposureModes() & QCamera::ExposureAuto | QCamera::ExposureNight);
    QCOMPARE(sharedCamera->exposureMode(), QCamera::ExposureAuto);
    sharedCamera->setExposureMode(QCamera::ExposureNight);
    QCOMPARE(sharedCamera->exposureMode(), QCamera::ExposureNight);
    
    QVERIFY(sharedCamera->supportedFlashModes() & QCamera::FlashAuto | QCamera::FlashRedEyeReduction);
    QCOMPARE(sharedCamera->flashMode(), QCamera::FlashOff);
    QCOMPARE(sharedCamera->isFlashReady(), false);

    sharedCamera->setFlashMode(QCamera::FlashOn);
    QCOMPARE(sharedCamera->flashMode(), QCamera::FlashOn);
    //QCOMPARE(sharedCamera->isFlashReady(), true); ??
    
    // Meteringmodes are not supported 5800 XpressMusic
    QVERIFY(sharedCamera->supportedMeteringModes() == 0);
    /*QCOMPARE(sharedCamera->meteringMode(), QCamera::MeteringMatrix);
    sharedCamera->setMeteringMode(QCamera::MeteringSpot);
    QCOMPARE(sharedCamera->meteringMode(), QCamera::MeteringMatrix);*/
    
    QCOMPARE(sharedCamera->exposureCompensation(), 0.0);
    sharedCamera->setExposureCompensation(2.0);
    QCOMPARE(sharedCamera->exposureCompensation(), 2.0);

    QCOMPARE(sharedCamera->isoSensitivity(), 0);
    // //supported isosensitivities in 5800XpressMusic: 100, 200, 400 and 800
    sharedCamera->setManualIsoSensitivity(100);
    QCOMPARE(sharedCamera->isoSensitivity(), 100);
    sharedCamera->setManualIsoSensitivity(800);
    QCOMPARE(sharedCamera->isoSensitivity(), 800);
    //not supported in 5800XpressMusic
    /*sharedCamera->setAutoIsoSensitivity();
    QCOMPARE(sharedCamera->isoSensitivity(), -1);*/

    //not supported in 5800XpressMusic
    QVERIFY(sharedCamera->aperture() == 0);
    QVERIFY(sharedCamera->supportedApertures().isEmpty());
    sharedCamera->setAutoAperture();
    QVERIFY(sharedCamera->aperture() == 0);
    sharedCamera->setManualAperture(5.6);
    QVERIFY(sharedCamera->aperture() == 0);
    
    //not supported in 5800XpressMusic
    QVERIFY(sharedCamera->shutterSpeed() == 0);
    QVERIFY(sharedCamera->supportedShutterSpeeds().isEmpty());
    sharedCamera->setAutoShutterSpeed();
    QVERIFY(sharedCamera->shutterSpeed() == 0);
    sharedCamera->setManualShutterSpeed(1/128.0);
    QVERIFY(sharedCamera->shutterSpeed() == 0);
    
    // not supported
    /*sharedCamera->lockExposure();
    QCOMPARE(sharedCamera->isExposureLocked(), true);*/
    sharedCamera->stop();
}

void tst_QCamera::testSimpleCameraFocus()
{
    QTimer timer;
    QSignalSpy timerSpy(&timer, SIGNAL(timeout()));
    sharedCamera->start();
    timer.start(5000);
    
    do {
        QTest::qWait(500);
    } while( sharedCamera->state() != QCamera::ActiveState && !timerSpy.count());

    QVERIFY(sharedCamera->supportedFocusModes() & QCamera::AutoFocus);
    QCOMPARE(sharedCamera->focusMode(), QCamera::AutoFocus);
    sharedCamera->setFocusMode(QCamera::ContinuousFocus);
    QCOMPARE(sharedCamera->focusMode(), QCamera::ContinuousFocus);
    QCOMPARE(sharedCamera->focusStatus(), QCamera::FocusInitial);

    QVERIFY(!sharedCamera->isMacroFocusingSupported());
    QVERIFY(!sharedCamera->macroFocusingEnabled());
    // macro focusing not supported
    //sharedCamera->setMacroFocusingEnabled(true);
    //QVERIFY(!sharedCamera->macroFocusingEnabled());

    QCOMPARE(sharedCamera->maximumOpticalZoom(), 0.0); // these should be check model ny model
    QCOMPARE(sharedCamera->maximumDigitalZoom(), 60.0); // these should be check model ny model
    QCOMPARE(sharedCamera->digitalZoom(), 0.0);
    sharedCamera->zoomTo(0,10.0);
    QCOMPARE(sharedCamera->digitalZoom(), 10.0);
    QCOMPARE(sharedCamera->opticalZoom(), 0.0);
    
    sharedCamera->stop();
}

void tst_QCamera::testSimpleCameraCapture()
{
    // This tests that taking a picture works with default setup
    QStillImageCapture *imageCapture = new QStillImageCapture(sharedCamera, NULL);
    QSignalSpy cameraState(sharedCamera, SIGNAL(stateChanged(QCamera::State)));
    QSignalSpy capturedSignal(imageCapture, SIGNAL(imageCaptured(QString,QImage)));
    QSignalSpy errorSignal(imageCapture, SIGNAL(error(QStillImageCapture::Error)));
    QSignalSpy imageSavedSignal(imageCapture, SIGNAL(imageSaved(const QString&)));
     
    QTimer timer;
    QSignalSpy timerSpy(&timer, SIGNAL(timeout()));
    sharedCamera->start();
    timer.start(5000);
    
    do {
        QTest::qWait(500);
    } while( sharedCamera->state() != QCamera::ActiveState && !timerSpy.count());
    
    QCOMPARE(sharedCamera->state(), QCamera::ActiveState);
    QVERIFY(imageCapture->isAvailable());
    QVERIFY(imageCapture->isReadyForCapture());
    
    QCOMPARE(imageCapture->error(), QStillImageCapture::NoError);
    QVERIFY(imageCapture->errorString().isEmpty());
    
    imageCapture->capture(capturePath(QDir::rootPath()));
    QTRY_COMPARE(capturedSignal.count(), 1);
    QTRY_COMPARE(imageSavedSignal.count(), 1);
    
    QCOMPARE(errorSignal.size(), 0);
        
    sharedCamera->stop();
    delete imageCapture;
}

void tst_QCamera::testCameraCapture()
{
    QStillImageCapture *imageCapture = new QStillImageCapture(sharedCamera, NULL);
    // check we have some controls and devices
    QSignalSpy cameraState(sharedCamera, SIGNAL(stateChanged(QCamera::State)));
    QSignalSpy capturedSignal(imageCapture, SIGNAL(imageCaptured(QString,QImage)));
    QSignalSpy errorSignal(imageCapture, SIGNAL(error(QStillImageCapture::Error)));
    QSignalSpy cameraErrorSignal(sharedCamera, SIGNAL(error(QCamera::Error)));
    QSignalSpy imageSavedSignal(imageCapture, SIGNAL(imageSaved(const QString&)));
    
    // case where we have not started the camera
    // we shoul receive NotReadyToCaptureError
    imageCapture->capture(QString::fromLatin1("/dev/null"));
    QCOMPARE(capturedSignal.size(), 0);
    
    //qDebug()<<"Camera errorNumber=" <<sharedCamera->error();
    //qDebug()<<"Camera errorText="  <<sharedCamera->errorString();
    
    QCOMPARE(cameraErrorSignal.size(), 0);
    QCOMPARE(sharedCamera->error(), QCamera::NotReadyToCaptureError);
    QCOMPARE(errorSignal.size(), 1);
    QCOMPARE(imageCapture->error(), QStillImageCapture::NotReadyError);
    QCOMPARE(sharedCamera->state(), QCamera::StoppedState);

    // Phase 2 test taking picture to a real path
    errorSignal.clear();
    cameraErrorSignal.clear();
    cameraState.clear();
    capturedSignal.clear();
    imageSavedSignal.clear();
    
    QTimer timer;
    QSignalSpy timerSpy(&timer, SIGNAL(timeout()));
    sharedCamera->start(); //we need to start camera before taking picture
    timer.start(5000);
    
    do {
        QTest::qWait(500);
    } while( sharedCamera->state() != QCamera::ActiveState && !timerSpy.count());
    
    QTRY_COMPARE (errorSignal.count(), 0); // We should not have any error code.
    QTRY_COMPARE (cameraState.count(), 1); // wait for camera to initialize itself
    QCOMPARE(sharedCamera->state(), QCamera::ActiveState);
    QVERIFY(imageCapture->isReadyForCapture());
    imageCapture->capture(QString::fromLatin1("/dev/null"));
    // we should receive captured signal, but no imageSavedSignal()
    QCOMPARE(capturedSignal.size(), 1);
    QCOMPARE(errorSignal.size(), 0);
    QCOMPARE(cameraErrorSignal.size(), 0);
    // as this is no good place to save image we shouldn't have imageSaved signal
    QCOMPARE(imageSavedSignal.size(), 0);
    QCOMPARE(imageCapture->error(), QStillImageCapture::ResourceError);
    
    sharedCamera->stop();
    delete imageCapture;
}

void tst_QCamera::testCameraExposure()
{
    QTimer timer;
    QSignalSpy timerSpy(&timer, SIGNAL(timeout()));
    sharedCamera->start();
    timer.start(5000);
    
    do {
        QTest::qWait(500);
    } while( sharedCamera->state() != QCamera::ActiveState && !timerSpy.count());

    QVERIFY(sharedCamera->supportedExposureModes() & QCamera::ExposureAuto | QCamera::ExposureBacklight);   
    sharedCamera->setExposureMode(QCamera::ExposureAuto);
    QCOMPARE(sharedCamera->exposureMode(), QCamera::ExposureAuto);
    sharedCamera->setExposureMode(QCamera::ExposureBacklight);
    QCOMPARE(sharedCamera->exposureMode(), QCamera::ExposureBacklight);

    //QCOMPARE(sharedCamera->flashMode(), QCamera::FlashOn); // we set this previous test function..
    sharedCamera->setFlashMode(QCamera::FlashAuto);
    QCOMPARE(sharedCamera->flashMode(), QCamera::FlashAuto);
    //QCOMPARE(sharedCamera->isFlashReady(), true); ???
    sharedCamera->setFlashMode(QCamera::FlashRedEyeReduction);
    QCOMPARE(sharedCamera->flashMode(), QCamera::FlashRedEyeReduction);

    // Not supported in 5800XpressMusic
    /*QCOMPARE(sharedCamera->meteringMode(), QCamera::MeteringMatrix);
    sharedCamera->setMeteringMode(QCamera::MeteringAverage);
    QCOMPARE(sharedCamera->meteringMode(), QCamera::MeteringAverage);
    sharedCamera->setMeteringMode(QCamera::MeteringSpot);
    QCOMPARE(sharedCamera->meteringMode(), QCamera::MeteringAverage);*/
    
    QCOMPARE(sharedCamera->exposureCompensation(), 0.0);
    sharedCamera->setExposureCompensation(2.0);
    QCOMPARE(sharedCamera->exposureCompensation(), 2.0);

    int minIso = sharedCamera->supportedIsoSensitivities().first();
    int maxIso = sharedCamera->supportedIsoSensitivities().last();
    QVERIFY(minIso > 0);
    QVERIFY(maxIso > 0);
    sharedCamera->setManualIsoSensitivity(minIso);
    QCOMPARE(sharedCamera->isoSensitivity(), minIso);
    sharedCamera->setManualIsoSensitivity(maxIso*10);
    QCOMPARE(sharedCamera->isoSensitivity(), maxIso);
    sharedCamera->setManualIsoSensitivity(-10);
    QCOMPARE(sharedCamera->isoSensitivity(), minIso);
    sharedCamera->setAutoIsoSensitivity();
    QCOMPARE(sharedCamera->isoSensitivity(), 100);

    //not supported in 5800XpressMusic
    if (!sharedCamera->supportedApertures().isEmpty()) {
        qreal minAperture = sharedCamera->supportedApertures().first();
        qreal maxAperture = sharedCamera->supportedApertures().last();
        sharedCamera->setManualAperture(280);
        QVERIFY(minAperture > 0);
        QVERIFY(maxAperture > 0);
        QVERIFY(sharedCamera->aperture() >= minAperture);
        QVERIFY(sharedCamera->aperture() <= maxAperture);
        sharedCamera->setAutoAperture();
        QVERIFY(sharedCamera->aperture() >= minAperture);
        QVERIFY(sharedCamera->aperture() <= maxAperture);
        sharedCamera->setManualAperture(10.0);
        QCOMPARE(sharedCamera->aperture(), 10.0);
        sharedCamera->setManualAperture(10000);
        QCOMPARE(sharedCamera->aperture(), maxAperture);
    }
    
    //not supported in 5800XpressMusic
    qreal minShutterSpeed = sharedCamera->supportedShutterSpeeds().first();
    qreal maxShutterSpeed = sharedCamera->supportedShutterSpeeds().last();
    if (!sharedCamera->supportedShutterSpeeds().empty()) {
        QVERIFY(minShutterSpeed > 0);
        QVERIFY(maxShutterSpeed > 0);
        QVERIFY(sharedCamera->shutterSpeed() >= minShutterSpeed);
        QVERIFY(sharedCamera->shutterSpeed() <= maxShutterSpeed);
        
        sharedCamera->setAutoShutterSpeed();
        QVERIFY(sharedCamera->shutterSpeed() >= minShutterSpeed);
        QVERIFY(sharedCamera->shutterSpeed() <= maxShutterSpeed);
    
        sharedCamera->setManualShutterSpeed(0);
        QCOMPARE(sharedCamera->shutterSpeed(), minShutterSpeed);

        sharedCamera->setManualShutterSpeed(10000);
        QCOMPARE(sharedCamera->shutterSpeed(), maxShutterSpeed);
    }
    sharedCamera->stop();
}

void tst_QCamera::testCameraFocus()
{
    QTimer timer;
    QSignalSpy timerSpy(&timer, SIGNAL(timeout()));
    sharedCamera->start();
    timer.start(5000);
    
    do {
        QTest::qWait(500);
    } while( sharedCamera->state() != QCamera::ActiveState && !timerSpy.count());

    QVERIFY(sharedCamera->supportedFocusModes() & QCamera::AutoFocus);    

    sharedCamera->setFocusMode(QCamera::AutoFocus);
    QCOMPARE(sharedCamera->focusMode(), QCamera::AutoFocus);

    //QCOMPARE(sharedCamera->focusStatus(), QCamera::FocusInitial);

    // macro focusing not supported
    QVERIFY(!sharedCamera->isMacroFocusingSupported());
    QVERIFY(!sharedCamera->macroFocusingEnabled());

    QVERIFY(sharedCamera->maximumOpticalZoom() == 0.0); // in 5800XpressMusic
    QVERIFY(sharedCamera->maximumDigitalZoom() >= 60.0); // in 5800XpressMusic
    QCOMPARE(sharedCamera->digitalZoom(), 0.0);
    sharedCamera->zoomTo(1.0,15.0);
    QCOMPARE(sharedCamera->digitalZoom(), 15.0);
    sharedCamera->zoomTo(1.0,2.0);
    QCOMPARE(sharedCamera->digitalZoom(), 2.0);
    sharedCamera->zoomTo(1.0,2000000.0);
    QCOMPARE(sharedCamera->digitalZoom(), 2.0);
    
    /* TODO: focusing has changed 
    sharedCamera->lockFocus();
    QCOMPARE(sharedCamera->isFocusLocked(), true);
    sharedCamera->unlockFocus();
    QCOMPARE(sharedCamera->isFocusLocked(), false);
    */
    
    sharedCamera->stop();
}

void tst_QCamera::testImageSettings()
{
    QStillImageCapture *imageCapture = new QStillImageCapture(sharedCamera, NULL);
    QSignalSpy cameraState(sharedCamera, SIGNAL(stateChanged(QCamera::State)));
    QTimer timer;
    QSignalSpy timerSpy(&timer, SIGNAL(timeout()));
    sharedCamera->start();
    timer.start(5000);
    
    do {
        QTest::qWait(500);
    } while( sharedCamera->state() != QCamera::ActiveState && !timerSpy.count());
    
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
    sharedCamera->stop();
    delete imageCapture;
}

QTEST_MAIN(tst_QCamera)

#include "tst_qcamera_s60.moc"
