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

#include <qabstractmediaservice.h>
#include <qmediarecorder.h>
#include <qaudiodevicecontrol.h>
#include <qaudioencodercontrol.h>
#include <qvideoencodercontrol.h>
#include <qmediaformatcontrol.h>
#include <qcamera.h>
#include <qvideowidget.h>
#include <qmediametadata.h>

#ifndef QT_NO_MULTIMEDIA
#include <QtMultimedia/qaudioformat.h>
#else
#include <qaudioformat.h>
#endif

#include <QtGui>

CameraCapture::CameraCapture(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CameraCapture),
    mediaRecorder(0),
    camera(0),
    audioDevice(0)
{
    ui->setupUi(this);

    camera = new QCamera;

    if(camera->isValid())
        mediaRecorder = new QMediaRecorder(camera);
    else
        mediaRecorder = new QMediaRecorder();

    mediaRecorder->setSink(QUrl("test.mkv"));
    audioDevice = mediaRecorder->service()->control<QAudioDeviceControl*>();

    connect(mediaRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(updateRecordTime()));
    connect(mediaRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayErrorMessage()));

    if (audioDevice) {
        for (int i=0; i<audioDevice->deviceCount(); i++) {
             ui->audioInputDeviceBox->addItem(audioDevice->description(i));
        }
    } else
        ui->audioInputDeviceBox->setEnabled(false);

    //audio codecs
    foreach(const QString &codecName, mediaRecorder->supportedAudioCodecs()) {
        QString description = mediaRecorder->audioCodecDescription(codecName);
        ui->audioCodecBox->addItem(codecName+": "+description);
        if (codecName == mediaRecorder->audioCodec())
            ui->audioCodecBox->setCurrentIndex(ui->audioCodecBox->count()-1);
    }

    ui->audioQualitySlider->setValue(qRound(mediaRecorder->audioQuality()));

    //video codecs
    foreach(const QString &codecName, mediaRecorder->supportedVideoCodecs()) {
        QString description = mediaRecorder->videoCodecDescription(codecName);
        ui->videoCodecBox->addItem(codecName+": "+description);
        if (codecName == mediaRecorder->videoCodec())
            ui->videoCodecBox->setCurrentIndex(ui->videoCodecBox->count()-1);
    }

    ui->videoQualitySlider->setValue(qRound(mediaRecorder->videoQuality()));

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

    metadata = new QMediaMetadata(camera);
    metadata->setMetadata(QMediaMetadata::Title, QVariant(QLatin1String("Test Title")));

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
    audioDevice->setSelectedDevice(idx);
}

void CameraCapture::setCameraDevice(int idx)
{
    Q_UNUSED(idx);
}

void CameraCapture::setAudioCodec(int idx)
{

    QString codecName = mediaRecorder->supportedAudioCodecs()[idx];
    mediaRecorder->setAudioCodec(codecName);

    QAudioFormat audioFormat;
    //speex works better with 32kHz sample rate
    if (codecName == QLatin1String("speexenc")) {
        audioFormat.setFrequency(32000);
    }

    mediaRecorder->setAudioFormat(audioFormat);

}

void CameraCapture::setVideoCodec(int idx)
{
    QString codecName = mediaRecorder->supportedVideoCodecs()[idx];
    mediaRecorder->setVideoCodec(codecName);
}

void CameraCapture::setAudioQuality(int value)
{
    mediaRecorder->setAudioQuality(value);
}

void CameraCapture::setVideoQuality(int value)
{
    mediaRecorder->setVideoQuality(value);
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
