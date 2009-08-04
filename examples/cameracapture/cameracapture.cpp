/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cameracapture.h"
#include "ui_cameracapture.h"

#include <qabstractmediaservice.h>
#include <qmediarecorder.h>
#include <qaudiodeviceendpoint.h>
#include <qmediawidgetendpoint.h>
#include <qaudioencodecontrol.h>
#include <qvideoencodecontrol.h>
#include <qmediaformatcontrol.h>
#include <qcamera.h>

#ifdef AUDIOSERVICES
#include <QtMultimedia/qaudioformat.h>
#endif

#include <QtGui>

CameraCapture::CameraCapture(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CameraCapture)
{
    ui->setupUi(this);

    camera = new QCamera;

    mediaRecorder = new QMediaRecorder(camera->service());
    mediaRecorder->setSink(QMediaSink(QUrl("test.ogg")));
    audioDevice = mediaRecorder->service()->createEndpoint<QAudioDeviceEndpoint*>();

    connect(mediaRecorder, SIGNAL(positionChanged(qint64)), this, SLOT(updateRecordTime()));
    connect(mediaRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayErrorMessage()));

    if (audioDevice) {
        mediaRecorder->service()->setAudioInput(audioDevice);
        audioDevice->setDirectionFilter(QAudioDeviceEndpoint::InputDevice);
        for (int i=0; i<audioDevice->deviceCount(); i++) {
             ui->audioInputDeviceBox->addItem(audioDevice->description(i));
        }
    } else
        ui->audioInputDeviceBox->setEnabled(false);

    audioEncodeControl = qobject_cast<QAudioEncodeControl*>(
            mediaRecorder->service()->control(QAudioEncodeControl_iid));

    if (audioEncodeControl) {
        foreach(const QString &codecName, audioEncodeControl->supportedAudioCodecs()) {
            QString description = audioEncodeControl->codecDescription(codecName);
            ui->audioCodecBox->addItem(codecName+": "+description);
            if (codecName == audioEncodeControl->audioCodec())
                ui->audioCodecBox->setCurrentIndex(ui->audioCodecBox->count()-1);
        }

        ui->audioQualitySlider->setValue(qRound(audioEncodeControl->quality()));
    } else {
        ui->audioCodecBox->setEnabled(false);
        ui->audioQualitySlider->setEnabled(false);
    }

    videoEncodeControl = qobject_cast<QVideoEncodeControl*>(
            mediaRecorder->service()->control(QVideoEncodeControl_iid));

    if (videoEncodeControl) {
        foreach(const QString &codecName, videoEncodeControl->supportedVideoCodecs()) {
            QString description = videoEncodeControl->videoCodecDescription(codecName);
            ui->videoCodecBox->addItem(codecName+": "+description);
            if (codecName == videoEncodeControl->videoCodec())
                ui->videoCodecBox->setCurrentIndex(ui->videoCodecBox->count()-1);
        }

        ui->videoQualitySlider->setValue(qRound(videoEncodeControl->quality()));
    } else {
        ui->videoCodecBox->setEnabled(false);
        ui->videoQualitySlider->setEnabled(false);
    }

    formatControl = qobject_cast<QMediaFormatControl*>(
            mediaRecorder->service()->control(QMediaFormatControl_iid));

    if (formatControl) {
        foreach(const QString &formatName, formatControl->supportedFormats()) {
            QString description = formatControl->formatDescription(formatName);
            ui->containerFormatBox->addItem(formatName+": "+description);
            if (formatName == formatControl->format())
                ui->containerFormatBox->setCurrentIndex(ui->containerFormatBox->count()-1);
        }
    } else {
        ui->containerFormatBox->setEnabled(false);
    }

    QWidget *videoWidget = mediaRecorder->service()->createEndpoint<QMediaWidgetEndpoint *>();

    if (videoWidget) {
        qDebug() << "service supports video widgets, nice";
        mediaRecorder->service()->setVideoOutput(videoWidget);
    }

    videoWidget->show();
}

CameraCapture::~CameraCapture()
{
}

void CameraCapture::updateRecordTime()
{
    QString str = QString("Recorded %1 sec").arg(mediaRecorder->position()/1000);
    ui->statusbar->showMessage(str);
}

void CameraCapture::setAudioInputDevice(int idx)
{
    audioDevice->setSelectedDevice(idx);
}

void CameraCapture::setCameraDevice(int idx)
{
    Q_UNUSED(idx);
}

void CameraCapture::setAudioCodec(int idx)
{
    if (audioEncodeControl) {
        QString codecName = audioEncodeControl->supportedAudioCodecs()[idx];
        audioEncodeControl->setAudioCodec(codecName);

        QAudioFormat audioFormat;
        //speex works better with 32kHz sample rate
        if (codecName == QLatin1String("speexenc")) {
            audioFormat.setFrequency(32000);
        }

        audioEncodeControl->setFormat(audioFormat);

    }
}

void CameraCapture::setVideoCodec(int idx)
{
    QString codecName = videoEncodeControl->supportedVideoCodecs()[idx];
    videoEncodeControl->setVideoCodec(codecName);
}

void CameraCapture::setAudioQuality(int value)
{
    audioEncodeControl->setQuality(value);
}

void CameraCapture::setVideoQuality(int value)
{
    videoEncodeControl->setQuality(value);
}

void CameraCapture::setContainerFormat(int idx)
{
    if (formatControl)
        formatControl->setFormat(formatControl->supportedFormats()[idx]);
}

void CameraCapture::record()
{
    if (mediaRecorder)
        mediaRecorder->record();
    updateRecordTime();
}

void CameraCapture::pause()
{
    if (mediaRecorder)
        mediaRecorder->pause();
}

void CameraCapture::stop()
{
    if (mediaRecorder)
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
