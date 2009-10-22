/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
#include <qcamera.h>
#include <qvideowidget.h>

#include <QtGui>

CameraCapture::CameraCapture(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CameraCapture),
    mediaRecorder(0),
    camera(0),
    service(0),
    videoWidget(0)
{
    ui->setupUi(this);

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

    service = camera->service();
    mediaRecorder = new QMediaRecorder(camera);

    //audio devices
    ui->actionAudio->menu()->clear();
    QActionGroup *audioDevicesGroup = new QActionGroup(this);
    audioDevicesGroup->setExclusive(true);

    foreach(const QString deviceName, service->supportedEndpoints(QMediaService::AudioDevice)) {
        QString description = service->endpointDescription(QMediaService::AudioDevice, deviceName);
        QAction *audioDeviceAction = new QAction(deviceName+" "+description, audioDevicesGroup);
        audioDeviceAction->setData(QVariant(deviceName));
        audioDeviceAction->setCheckable(true);

        ui->actionAudio->menu()->addAction(audioDeviceAction);

        if (service->activeEndpoint(QMediaService::AudioDevice) == deviceName)
            audioDeviceAction->setChecked(true);
    }

    connect(audioDevicesGroup, SIGNAL(triggered(QAction*)), this, SLOT(updateAudioDevice(QAction*)));


    mediaRecorder->setSink(QUrl("test.mkv"));

    connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(updateRecordTime()));
    connect(mediaRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayErrorMessage()));

    camera->setMetaData(QtMedia::Title, QVariant(QLatin1String("Test Title")));

    videoWidget = new QVideoWidget(mediaRecorder);
    ui->stackedWidget->addWidget(videoWidget);
    ui->previewCamera->setChecked(false);
}


void CameraCapture::updateRecordTime()
{
    QString str = QString("Recorded %1 sec").arg(mediaRecorder->duration()/1000);
    ui->statusbar->showMessage(str);
}

void CameraCapture::settings()
{
    Settings settingsDialog(mediaRecorder);

    settingsDialog.setAudioSettings(mediaRecorder->audioSettings());
    settingsDialog.setVideoSettings(mediaRecorder->videoSettings());
    settingsDialog.setFormat(mediaRecorder->format());

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

void CameraCapture::enablePreview(bool enabled)
{
    if (enabled)
        camera->start();
    else
        camera->stop();
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
    service->setActiveEndpoint(QMediaService::AudioDevice, action->data().toString());
}
