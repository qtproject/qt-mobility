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

#include <multimedia/qmediaservice.h>
#include <multimedia/qmediarecorder.h>
#include <multimedia/qaudiodevicecontrol.h>
#include <multimedia/qaudioencodercontrol.h>
#include <multimedia/qvideoencodercontrol.h>
#include <multimedia/qmediaformatcontrol.h>
#include <multimedia/qcamera.h>
#include <multimedia/qvideowidget.h>

#include <QtGui>

CameraCapture::CameraCapture(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CameraCapture),
    mediaRecorder(0),
    camera(0),
    service(0)
{
    ui->setupUi(this);

    camera = new QCamera;
    service = camera->service();
    mediaRecorder = new QMediaRecorder(camera);

    Q_ASSERT(service);

    mediaRecorder->setSink(QUrl("test.mkv"));

    connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(updateRecordTime()));
    connect(mediaRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayErrorMessage()));

    //audio devices
    foreach(const QString deviceName, service->supportedEndpoints(QMediaService::AudioInput)) {
        QString description = service->endpointDescription(QMediaService::AudioInput, deviceName);
        ui->audioInputDeviceBox->addItem(deviceName+" "+description, QVariant(deviceName));
    }

    //camera devices
    foreach(const QString deviceName, camera->devices()) {
        ui->cameraDeviceBox->addItem(deviceName+" "+camera->deviceDescription(deviceName), QVariant(deviceName));
    }

    //audio codecs
    foreach(const QString &codecName, mediaRecorder->supportedAudioCodecs()) {
        QString description = mediaRecorder->audioCodecDescription(codecName);
        ui->audioCodecBox->addItem(codecName+": "+description);
        if (codecName == mediaRecorder->audioCodec())
            ui->audioCodecBox->setCurrentIndex(ui->audioCodecBox->count()-1);
    }

    ui->audioQualitySlider->setRange(0, int(QMediaRecorder::VeryHighQuality));
    ui->audioQualitySlider->setValue(qRound(mediaRecorder->audioQuality()));

    //video codecs
    foreach(const QString &codecName, mediaRecorder->supportedVideoCodecs()) {
        QString description = mediaRecorder->videoCodecDescription(codecName);
        ui->videoCodecBox->addItem(codecName+": "+description);
        if (codecName == mediaRecorder->videoCodec())
            ui->videoCodecBox->setCurrentIndex(ui->videoCodecBox->count()-1);
    }

    ui->videoQualitySlider->setValue(qRound(mediaRecorder->videoQuality()));
    ui->videoQualitySlider->setRange(0, int(QMediaRecorder::VeryHighQuality));

    ui->videoResolutionBox->addItem(tr("Default"));
    QList<QSize> supportedResolutions = mediaRecorder->supportedResolutions();
    foreach(const QSize &resolution, supportedResolutions) {
        ui->videoResolutionBox->addItem(QString("%1x%2").arg(resolution.width()).arg(resolution.height()));
    }

    ui->videoFramerateBox->addItem(tr("Default"));
    QList< QPair<int,int> > supportedFrameRates = mediaRecorder->supportedFrameRates();
    QPair<int,int> rate;
    foreach(rate, supportedFrameRates) {
        ui->videoFramerateBox->addItem(QString("%1/%2").arg(rate.first).arg(rate.second));
    }

    //container format selection
    foreach(const QString &formatName, mediaRecorder->supportedFormats()) {
        QString description = mediaRecorder->formatDescription(formatName);
        ui->containerFormatBox->addItem(formatName+": "+description);
        if (formatName == mediaRecorder->format())
            ui->containerFormatBox->setCurrentIndex(ui->containerFormatBox->count()-1);
    }


    camera->setMetaData(QMediaObject::Title, QVariant(QLatin1String("Test Title")));

    QWidget *videoWidget = new QVideoWidget(mediaRecorder);
    videoWidget->resize(640,480);
    videoWidget->show();
}

CameraCapture::~CameraCapture()
{
}

void CameraCapture::updateRecordTime()
{
    QString str = QString("Recorded %1 sec").arg(mediaRecorder->duration()/1000);
    ui->statusbar->showMessage(str);
}

void CameraCapture::setAudioInputDevice(int idx)
{
    QString deviceName = ui->audioInputDeviceBox->itemData(idx).toString();
    service->setActiveEndpoint(QMediaService::AudioInput, deviceName );
}

void CameraCapture::setCameraDevice(int idx)
{
    camera->setDevice(ui->cameraDeviceBox->itemData(idx).toString());
}

void CameraCapture::setAudioCodec(int idx)
{
    QString codecName = mediaRecorder->supportedAudioCodecs()[idx];
    mediaRecorder->setAudioCodec(codecName);
}

void CameraCapture::setVideoCodec(int idx)
{
    QString codecName = mediaRecorder->supportedVideoCodecs()[idx];
    mediaRecorder->setVideoCodec(codecName);
}

void CameraCapture::setAudioQuality(int value)
{
    mediaRecorder->setAudioQuality(QMediaRecorder::EncodingQuality(value));
}

void CameraCapture::setVideoQuality(int value)
{
    mediaRecorder->setVideoQuality(QMediaRecorder::EncodingQuality(value));
}

void CameraCapture::setContainerFormat(int idx)
{
    if (mediaRecorder)
        mediaRecorder->setFormat(mediaRecorder->supportedFormats()[idx]);
}

void CameraCapture::setVideoResolution()
{
    QSize resolution;
    QStringList resolutionParts = ui->videoResolutionBox->currentText().split('x');
    if (resolutionParts.size() == 2) {
        resolution.setWidth(resolutionParts[0].toInt());
        resolution.setHeight(resolutionParts[1].toInt());
    }

    mediaRecorder->setResolution(resolution);
}

void CameraCapture::setVideoFramerate()
{
    QPair<int,int> frameRate = qMakePair<int,int>(-1,-1);
    QStringList rateParts = ui->videoFramerateBox->currentText().split('/');
    if (rateParts.size() == 2) {
        frameRate.first = rateParts[0].toInt();
        frameRate.second = rateParts[1].toInt();
    }

    mediaRecorder->setFrameRate(frameRate);
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
