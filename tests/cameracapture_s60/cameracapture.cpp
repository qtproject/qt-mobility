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

#include "cameracapture.h"
#include "ui_cameracapture.h"
#include "settings.h"
#ifdef Q_OS_SYMBIAN
#include "stillsettings.h"
#endif

#include <qmediaservice.h>
#include <qmediarecorder.h>
#include <experimental/qcamera.h>
#include <qvideowidget.h>

#include <qmessagebox.h>

#include <QtGui>

CameraCapture::CameraCapture(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CameraCapture),
    camera(0),
    imageCapture(0),
    mediaRecorder(0),
    audioSource(0),
    videoWidget(0)
{
    ui->setupUi(this);
    #if defined(Q_OS_SYMBIAN)
    outputDirVideo = QDir::rootPath(); // this defaults to C:\Data\Videos in symbian
    outputDirVideo.cd("Videos");
    outputDirImage = QDir::rootPath(); // this defaults to C:\Data\Images in symbian
    outputDirImage.cd("Images");
    #else
    outputDirVideo = QDir::currentPath();
    outputDirImage = QDir::currentPath();
    #endif

    //camera devices
    QByteArray cameraDevice;

    ui->actionCamera->setMenu(new QMenu(this));
    QActionGroup *videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);
    foreach(const QByteArray &deviceName, QCamera::availableDevices()) {
        QString description = deviceName+" "+camera->deviceDescription(deviceName);
        QAction *videoDeviceAction = new QAction(description, videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant(deviceName));
        if (cameraDevice.isEmpty()) {
            cameraDevice = deviceName;
            videoDeviceAction->setChecked(true);
        }
        ui->actionCamera->menu()->addAction(videoDeviceAction);
    }

    connect(videoDevicesGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateCameraDevice(QAction*)));
    connect(ui->actionFlash_On_2, SIGNAL(triggered()), this, SLOT(setFlashOn()));
    connect(ui->actionFlash_Off, SIGNAL(triggered()), this, SLOT(setFlashOff()));
    connect(ui->actionFlash_auto, SIGNAL(triggered()), this, SLOT(setFlashAuto()));
    connect(ui->actionRed_eye, SIGNAL(triggered()), this, SLOT(setFlashRed()));
    connect(ui->actionFlash_FillIn, SIGNAL(triggered()), this, SLOT(setFlashFillIn()));
    
    connect(ui->action_Focus_On, SIGNAL(triggered()), this, SLOT(setFocusOn()));   
    connect(ui->action_Focus_Off, SIGNAL(triggered()), this, SLOT(setFocusOff()));
    
    connect(ui->actionNight, SIGNAL(triggered()), this, SLOT(setExposureNight()));   
    connect(ui->actionBacklight, SIGNAL(triggered()), this, SLOT(setExposureBacklight()));
    connect(ui->actionSport, SIGNAL(triggered()), this, SLOT(setExposureSport())); 
    connect(ui->actionBeach, SIGNAL(triggered()), this, SLOT(setExposureBeach()));
    
    connect(ui->actionAuto, SIGNAL(triggered()), this, SLOT(setWBAuto()));   
    connect(ui->actionSunlight, SIGNAL(triggered()), this, SLOT(setWBSunlight()));
    connect(ui->actionCloudy, SIGNAL(triggered()), this, SLOT(setWBCloudy())); 
    connect(ui->actionTungsten, SIGNAL(triggered()), this, SLOT(setWBTungsten()));
    
    connect(ui->actionHard, SIGNAL(triggered()), this, SLOT(setSharpnessHard()));
    connect(ui->actionNormal, SIGNAL(triggered()), this, SLOT(setSharpnessNormal()));
    connect(ui->actionSoft, SIGNAL(triggered()), this, SLOT(setSharpnessSoft()));
    
    ui->actionAudio->setMenu(new QMenu(this));

    setCamera(cameraDevice);
    
    mediaKeysObserver = new MediaKeysObserver(this);
    connect(mediaKeysObserver, SIGNAL(mediaKeyPressed(MediaKeysObserver::MediaKeys)), this, SLOT(handleMediaKeyEvent(MediaKeysObserver::MediaKeys)));
}

CameraCapture::~CameraCapture()
{
}

void CameraCapture::setFlashOn()
{
    camera->setFlashMode(QCamera::FlashOn);
}

void CameraCapture::setFlashOff()
{
    camera->setFlashMode(QCamera::FlashOff);
}

void CameraCapture::setFlashAuto()
{
    camera->setFlashMode(QCamera::FlashAuto);
}

void CameraCapture::setFlashRed()
{
    camera->setFlashMode(QCamera::FlashRedEyeReduction);
}

void CameraCapture::setFlashFillIn()
{
    camera->setFlashMode(QCamera::FlashFill);
}

void CameraCapture::setFocusOn()
{
    camera->setFocusMode(QCamera::AutoFocus);
}

void CameraCapture::setFocusOff()
{
    camera->setFocusMode(QCamera::ManualFocus);
}

void CameraCapture::setExposureNight()
{
    camera->setExposureMode(QCamera::ExposureNight);
}

void CameraCapture::setExposureBacklight()
{
    camera->setExposureMode(QCamera::ExposureBacklight);
}

void CameraCapture::setExposureSport()
{
    camera->setExposureMode(QCamera::ExposureSports);
}

void CameraCapture::setExposureBeach()
{
    camera->setExposureMode(QCamera::ExposurePortrait);
}

void CameraCapture::setWBAuto()
{
    camera->setWhiteBalanceMode(QCamera::WhiteBalanceAuto);
}

void CameraCapture::setWBSunlight()
{
    camera->setWhiteBalanceMode(QCamera::WhiteBalanceSunlight);
}

void CameraCapture::setWBCloudy()
{
    camera->setWhiteBalanceMode(QCamera::WhiteBalanceCloudy);
}

void CameraCapture::setWBTungsten()
{
    camera->setWhiteBalanceMode(QCamera::WhiteBalanceTungsten);
}

void CameraCapture::setSharpnessHard()
{
    // no qcamera implementation
}

void CameraCapture::setSharpnessNormal()
{
    // no qcamera implementation
}

void CameraCapture::setSharpnessSoft()
{
    // no qcamera implementation
}

void CameraCapture::setCamera(const QByteArray &cameraDevice)
{
    delete imageCapture;
    delete mediaRecorder;
    delete videoWidget;
    delete camera;

    //qDebug() << "CameraCapture::setCamera cameraDevice.isEmpty()=" << cameraDevice.isEmpty();
    if (cameraDevice.isEmpty())
        camera = new QCamera(this);
    else       
        camera = new QCamera(cameraDevice, this);

    connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(updateCameraState(QCamera::State)));
    connect(camera, SIGNAL(focusStatusChanged(QCamera::FocusStatus)), this, SLOT(focusStatusChanged(QCamera::FocusStatus)));
    connect(camera, SIGNAL(opticalZoomChanged(qreal)), this, SLOT(zoomValueChanged(qreal)));
    connect(camera, SIGNAL(digitalZoomChanged(qreal)), this, SLOT(zoomValueChanged(qreal)));
    connect(camera, SIGNAL(error(QCamera::Error)), this, SLOT(error(QCamera::Error)));

    mediaRecorder = new QMediaRecorder(camera, this);
    connect(mediaRecorder, SIGNAL(stateChanged(QMediaRecorder::State)), this, SLOT(updateRecorderState(QMediaRecorder::State)));

    imageCapture = new QStillImageCapture(camera, this);

    audioSource = new QAudioCaptureSource(camera,this);
    connect(audioSource, SIGNAL(availableAudioInputsChanged()), SLOT(updateAudioDevices()));

    mediaRecorder->setOutputLocation(QUrl("test.mkv"));

    connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(updateRecordTime()));
    connect(mediaRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayErrorMessage()));

    camera->setMetaData(QtMedia::Title, QVariant(QLatin1String("Test Title")));

    videoWidget = new QVideoWidget(this);
    videoWidget->setMediaObject(camera);
    ui->stackedWidget->addWidget(videoWidget);

    updateCameraState(camera->state());
    updateRecorderState(mediaRecorder->state());
    updateAudioDevices();
    
    ui->recordButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    ui->stopButton->setEnabled(false);

    connect(imageCapture, SIGNAL(readyForCaptureChanged(bool)), ui->takeImageButton, SLOT(setEnabled(bool)));
    connect(imageCapture, SIGNAL(imageCaptured(QString,QImage)), this, SLOT(processCapturedImage(QString,QImage)));

}

void CameraCapture::updateAudioDevices()
{
    ui->actionAudio->menu()->clear();
    QActionGroup *audioDevicesGroup = new QActionGroup(this);
    audioDevicesGroup->setExclusive(true);

    if (audioSource->isAvailable()) {
        QList<QString> devices = audioSource->audioInputs();
        for (int i=0; i<devices.size(); i++) {
            QString description = audioSource->audioDescription(devices.at(i));
            QAction *audioDeviceAction = new QAction(devices.at(i)+" "+description, audioDevicesGroup);
            audioDeviceAction->setData(devices.at(i));
            audioDeviceAction->setCheckable(true);

            ui->actionAudio->menu()->addAction(audioDeviceAction);

            if (devices.at(i) == audioSource->activeAudioInput())
                audioDeviceAction->setChecked(true);
        }
    } else {
        qWarning() << "No audio device for camera service available";
    }

    connect(audioDevicesGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateAudioDevice(QAction*)));
}

void CameraCapture::updateRecordTime()
{
    QString str = QString("Recorded %1 sec").arg(mediaRecorder->duration()/1000);
    ui->statusbar->showMessage(str);
}

void CameraCapture::processCapturedImage(const QString& fname, const QImage& img)
{
    ui->lastImagePreviewLabel->setPixmap( QPixmap::fromImage(img.scaledToWidth(128)) );
    qDebug() << "image captured:" << fname;
}

void CameraCapture::settings()
{    
    Settings settingsDialog(mediaRecorder);    
    settingsDialog.setAudioSettings(mediaRecorder->audioSettings());
    settingsDialog.setVideoSettings(mediaRecorder->videoSettings());
    settingsDialog.setFormat(mediaRecorder->containerMimeType());
    
    if (settingsDialog.exec() == QDialog::Accepted) {
        mediaRecorder->setEncodingSettings(
                settingsDialog.audioSettings(),
                settingsDialog.videoSettings(),
                settingsDialog.format());
    }
}

#ifdef Q_OS_SYMBIAN
void CameraCapture::stillSettings()
{    
    StillSettings settingsDialog(imageCapture);    
    settingsDialog.setImageSettings(imageCapture->encodingSettings());
    
    if (settingsDialog.exec() == QDialog::Accepted) {
        imageCapture->setEncodingSettings(settingsDialog.imageSettings());
    }
}
#endif

void CameraCapture::record()
{
    int lastImage = 0;
    foreach( QString fileName, outputDirVideo.entryList(QStringList() << "clip_*.mpg") ) {
        int imgNumber = fileName.mid(5, fileName.size()-9).toInt();
        lastImage = qMax(lastImage, imgNumber);
    }
    
    QUrl location(QDir::toNativeSeparators(outputDirVideo.canonicalPath()+
        QString("/clip_%1.mpg").arg(lastImage+1,4,10,QLatin1Char('0'))));

    mediaRecorder->setOutputLocation(location);
    
    mediaRecorder->record();
    updateRecordTime();
}

void CameraCapture::pause()
{
    mediaRecorder->pause();
}

void CameraCapture::stop()
{
    mediaRecorder->stop();
}

void CameraCapture::takeImage()
{
    if (camera->focusMode() == QCamera::AutoFocus) {
        qDebug() << "CameraCapture::takeImage: if (camera->focusMode() == QCamera::AutoFocus)"; 
        m_takeImage = true;
        camera->startFocusing();
    } else {
        qDebug() << "CameraCapture::takeImage: else";
        int lastImage = 0;
        foreach( QString fileName, outputDirImage.entryList(QStringList() << "img_*.jpg") ) {
            int imgNumber = fileName.mid(4, fileName.size()-8).toInt();
            lastImage = qMax(lastImage, imgNumber);
        }        
        imageCapture->capture(QString("img_%1.jpg").arg(lastImage+1,
                                                        4, //fieldWidth
                                                        10,
                                                        QLatin1Char('0')));
    }  
}

void CameraCapture::toggleCamera()
{
    if (camera->state() == QCamera::ActiveState){
        camera->stop();
    }
    else
        camera->start();
}

void CameraCapture::updateCameraState(QCamera::State state)
{
    qDebug() << "CameraCapture::updateCameraState(), state="<<state;
    if (state == QCamera::ActiveState) {
        ui->actionCamera->setEnabled(false);
        ui->actionAudio->setEnabled(false);
        ui->actionSettings->setEnabled(true);

        ui->startCameraButton->setText(tr("Stop Camera"));
        ui->startCameraButton->setChecked(true);
        //ui->imageCaptureBox->setEnabled(true);
        //ui->videoCaptureBox->setEnabled(true);
        ui->recordButton->setEnabled(true);
        
    } else {
        ui->actionCamera->setEnabled(true);
        ui->actionAudio->setEnabled(true);
        ui->actionSettings->setEnabled(true);

        ui->startCameraButton->setText(tr("Start Camera"));
        ui->startCameraButton->setChecked(false);
        ui->takeImageButton->setEnabled(false);
        
        ui->recordButton->setEnabled(false);
        ui->pauseButton->setEnabled(false);
        ui->stopButton->setEnabled(false);
        
        //ui->imageCaptureBox->setEnabled(false);
        //ui->videoCaptureBox->setEnabled(false);
    }

    if (camera->isAvailable()) {
        ui->startCameraButton->setEnabled(true);
    } else {
        ui->startCameraButton->setEnabled(false);
        ui->startCameraButton->setText(tr("Camera is not available"));
    }
}

void CameraCapture::updateRecorderState(QMediaRecorder::State state)
{
    qDebug() << "CameraCapture::updateRecorderState";
    switch (state) {
    case QMediaRecorder::StoppedState:
        ui->recordButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->stopButton->setEnabled(false);
        break;
    case QMediaRecorder::PausedState:
        ui->recordButton->setEnabled(true);
        ui->pauseButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
        break;
    case QMediaRecorder::RecordingState:
        ui->recordButton->setEnabled(false);
        ui->pauseButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
        break;
    }
}


void CameraCapture::displayErrorMessage()
{
    QMessageBox::warning(this, "Capture error", mediaRecorder->errorString());
}

void CameraCapture::updateCameraDevice(QAction *action)
{
    qDebug() << "CameraCapture::updateCameraDevice(), action="<<action;
    qDebug() << "CameraCapture::updateCameraDevice(), device="<<action->data().toByteArray();
    setCamera(action->data().toByteArray());
    if (action->data().toByteArray() == "Secondary camera")
        setFocusOff();
    else
        setFocusOn();
}

void CameraCapture::updateAudioDevice(QAction *action)
{
    audioSource->setAudioInput(action->data().toString());
}

void CameraCapture::focusStatusChanged(QCamera::FocusStatus status)
{
    qDebug() << "CameraCapture::focusStatusChanged: "<<status;
    if (status == QCamera::FocusReached && m_takeImage) {
        int lastImage = 0;
        foreach( QString fileName, outputDirImage.entryList(QStringList() << "img_*.jpg") ) {
            int imgNumber = fileName.mid(4, fileName.size()-8).toInt();
            lastImage = qMax(lastImage, imgNumber);
        }
    
        imageCapture->capture(QString("img_%1.jpg").arg(lastImage+1,
                                                        4, //fieldWidth
                                                        10,
                                                        QLatin1Char('0')));
        m_takeImage = false;
    }
}

void CameraCapture::zoomValueChanged(qreal value)
{
	qDebug() << "CameraCapture zoom value changed to: " << value;
}

void CameraCapture::handleMediaKeyEvent(MediaKeysObserver::MediaKeys key)
{
    switch (key) {
        case MediaKeysObserver::EVolIncKey: 
            camera->zoomTo(0, camera->digitalZoom() + 5);
            break;
        case MediaKeysObserver::EVolDecKey:
            camera->zoomTo(0, camera->digitalZoom() - 5);
            break;
        default:
        break;
    }
}

void CameraCapture::error(QCamera::Error aError)
{
    qDebug() << "CameraCapture error: " << aError;
}
