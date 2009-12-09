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

#include <qmediaservice.h>
#include <qmediarecorder.h>
#include <experimental/qcamera.h>
#include <qvideowidget.h>

#include <QtGui>

CameraCapture::CameraCapture(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CameraCapture),
    camera(0),
    mediaRecorder(0),
    audioSource(0),
    videoWidget(0)
{
    ui->setupUi(this);

    outputDir = QDir::currentPath();

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

    ui->actionAudio->setMenu(new QMenu(this));

    setCamera(cameraDevice);
}

CameraCapture::~CameraCapture()
{
    delete mediaRecorder;
    delete videoWidget;
    delete camera;
}

void CameraCapture::setCamera(const QByteArray &cameraDevice)
{
    delete mediaRecorder;
    delete videoWidget;
    delete camera;

    if (cameraDevice.isEmpty())
        camera = new QCamera;
    else
        camera = new QCamera(cameraDevice);

    connect(camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(updateCameraState(QCamera::State)));

    mediaRecorder = new QMediaRecorder(camera);
    connect(mediaRecorder, SIGNAL(stateChanged(QMediaRecorder::State)), this, SLOT(updateRecorderState(QMediaRecorder::State)));

    audioSource = new QAudioCaptureSource(camera);
    connect(audioSource, SIGNAL(devicesChanged()), SLOT(updateAudioDevices()));

    mediaRecorder->setOutputLocation(QUrl("test.mkv"));

    connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(updateRecordTime()));
    connect(mediaRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayErrorMessage()));

    camera->setMetaData(QtMedia::Title, QVariant(QLatin1String("Test Title")));

    videoWidget = new QVideoWidget;
    videoWidget->setMediaObject(camera);
    ui->stackedWidget->addWidget(videoWidget);

    updateCameraState(camera->state());
    updateRecorderState(mediaRecorder->state());
    updateAudioDevices();

    connect(camera, SIGNAL(readyForCaptureChanged(bool)), ui->imageCaptureBox, SLOT(setEnabled(bool)));
    connect(camera, SIGNAL(imageCaptured(QString,QImage)), this, SLOT(processCapturedImage(QString,QImage)));

}

void CameraCapture::updateAudioDevices()
{
    ui->actionAudio->menu()->clear();
    QActionGroup *audioDevicesGroup = new QActionGroup(this);
    audioDevicesGroup->setExclusive(true);

    if (audioSource->isAvailable()) {
        for (int i=0; i<audioSource->deviceCount(); i++) {
            QString deviceName = audioSource->deviceName(i);
            QString description = audioSource->deviceDescription(i);

            QAction *audioDeviceAction = new QAction(deviceName+" "+description, audioDevicesGroup);
            audioDeviceAction->setData(QVariant(i));
            audioDeviceAction->setCheckable(true);

            ui->actionAudio->menu()->addAction(audioDeviceAction);

            audioDeviceAction->setChecked(audioSource->selectedDevice() == i);
        }
    } else {
        qWarning() << "Camera service is not available";
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

    if (settingsDialog.exec()) {
        mediaRecorder->setEncodingSettings(
                settingsDialog.audioSettings(),
                settingsDialog.videoSettings(),
                settingsDialog.format());
    }
}

void CameraCapture::record()
{
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
    int lastImage = 0;
    foreach( QString fileName, outputDir.entryList(QStringList() << "img_*.jpg") ) {
        int imgNumber = fileName.mid(4, fileName.size()-8).toInt();
        lastImage = qMax(lastImage, imgNumber);
    }

    camera->capture(QString("img_%1.jpg").arg(lastImage+1,
                                              4, //fieldWidth
                                              10,
                                              QLatin1Char('0')));
}

void CameraCapture::toggleCamera()
{
    if (camera->state() == QCamera::ActiveState)
        camera->stop();
    else
        camera->start();
}

void CameraCapture::updateCameraState(QCamera::State state)
{
    if (state == QCamera::ActiveState) {
        ui->actionCamera->setEnabled(false);
        ui->actionAudio->setEnabled(false);
        ui->actionSettings->setEnabled(false);

        ui->startCameraButton->setText(tr("Stop Camera"));
        ui->startCameraButton->setChecked(true);
        ui->videoCaptureBox->setEnabled(true);
    } else {
        ui->actionCamera->setEnabled(true);
        ui->actionAudio->setEnabled(true);
        ui->actionSettings->setEnabled(true);

        ui->startCameraButton->setText(tr("Start Camera"));
        ui->startCameraButton->setChecked(false);
        ui->imageCaptureBox->setEnabled(false);
        ui->videoCaptureBox->setEnabled(false);
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
    switch (state) {
    case QMediaRecorder::StoppedState:
        ui->recordButton->setEnabled(true);
        ui->pauseButton->setEnabled(true);
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
    setCamera(action->data().toByteArray());
}

void CameraCapture::updateAudioDevice(QAction *action)
{
    audioSource->setSelectedDevice(action->data().toInt());
}
