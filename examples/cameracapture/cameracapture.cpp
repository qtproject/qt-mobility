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

#include "cameracapture.h"
#include "ui_cameracapture.h"
#include "settings.h"

#include <multimedia/qmediaservice.h>
#include <multimedia/qmediarecorder.h>
#include <multimedia/qcamera.h>
#include <multimedia/qvideowidget.h>

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
    ui->actionCamera->setMenu(new QMenu(this));
    foreach(const QByteArray deviceName, QCamera::devices()) {
        QString description = deviceName+" "+camera->deviceDescription(deviceName);
        ui->actionCamera->menu()->addAction(description);
    }

    ui->actionAudio->setMenu(new QMenu(this));

    setCamera(QByteArray());
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

    foreach(const QString deviceName, service->supportedEndpoints(QMediaService::AudioInput)) {
        QString description = service->endpointDescription(QMediaService::AudioInput, deviceName);
        QAction *audioDeviceAction = new QAction(deviceName+" "+description, audioDevicesGroup);
        audioDeviceAction->setData(QVariant(deviceName));
        audioDeviceAction->setCheckable(true);

        ui->actionAudio->menu()->addAction(audioDeviceAction);

        if (!service->activeEndpoints(QMediaService::AudioInput).isEmpty() &&
            service->activeEndpoints(QMediaService::AudioInput).first() == deviceName)
            audioDeviceAction->setChecked(true);
    }


    mediaRecorder->setSink(QUrl("test.mkv"));

    connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(updateRecordTime()));
    connect(mediaRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayErrorMessage()));

    camera->setMetaData(QtMedia::Title, QVariant(QLatin1String("Test Title")));

    videoWidget = new QVideoWidget(mediaRecorder);
    ui->stackedWidget->addWidget(videoWidget);
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
