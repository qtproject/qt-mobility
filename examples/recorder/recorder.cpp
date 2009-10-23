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

#include "recorder.h"
#include "ui_recorder.h"

#include <qabstractmediaservice.h>
#include <qmediarecorder.h>
#include <qaudiodevicecontrol.h>
#include <qaudioencodercontrol.h>
#include <qmediaformatcontrol.h>

#include <QtGui>

Recorder::Recorder(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Recorder)
{
    ui->setupUi(this);

    audioRecorder = new QMediaRecorder;
    audioRecorder->setSink(QUrl("test.ogg"));

    connect(audioRecorder, SIGNAL(durationChanged(qint64)), this, SLOT(updateRecordTime()));
    connect(audioRecorder, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayErrorMessage()));

    if(audioRecorder->service()) {
        // Audio Input
        QList<QString> audioInputs;
        audioInputs = audioRecorder->service()->supportedEndpoints(QAbstractMediaService::AudioInput);
        if(audioInputs.size() > 0) {
            qWarning()<<"FOUND audioInputs: "<<audioInputs;
            audioRecorder->service()->setActiveEndpoint(QAbstractMediaService::AudioInput,audioInputs.first().toLocal8Bit().constData());
            for(int i = 0; i < audioInputs.size(); ++i) {
                ui->inputDeviceBox->addItem(audioInputs.at(i));
            }
        } else
            ui->inputDeviceBox->setEnabled(false);
    }

    audioDevice = qobject_cast<QAudioDeviceControl*>(
            audioRecorder->service()->control(QAudioDeviceControl_iid));

    //if (audioDevice) {
    //    audioRecorder->service()->setAudioInput(audioDevice);
    //    audioDevice->setDirectionFilter(QAudioDeviceEndpoint::InputDevice);
    //    for (int i=0; i<audioDevice->deviceCount(); i++) {
    //         ui->inputDeviceBox->addItem(audioDevice->description(i));
    //    }
    //} else
    //    ui->inputDeviceBox->setEnabled(false);

    encodeControl = qobject_cast<QAudioEncoderControl*>(
            audioRecorder->service()->control(QAudioEncoderControl_iid));

    if (encodeControl) {
        foreach(const QString &codecName, encodeControl->supportedAudioCodecs()) {
            QString description = encodeControl->codecDescription(codecName);
            ui->audioCodecBox->addItem(codecName+": "+description);
            if (codecName == encodeControl->audioCodec())
                ui->audioCodecBox->setCurrentIndex(ui->audioCodecBox->count()-1);
        }

        ui->qualitySlider->setValue(qRound(encodeControl->quality()));
    } else {
        ui->audioCodecBox->setEnabled(false);
        ui->qualitySlider->setEnabled(false);
    }

    formatControl = qobject_cast<QMediaFormatControl*>(
            audioRecorder->service()->control(QMediaFormatControl_iid));

    if (formatControl) {
        foreach(const QString &formatName, formatControl->supportedFormats()) {
            QString description = formatControl->formatDescription(formatName);
            //ui->containerFormatBox->addItem(formatName+": "+description);
            //if (formatName == formatControl->format())
            //    ui->containerFormatBox->setCurrentIndex(ui->containerFormatBox->count()-1);
        }
    //} else {
    //    ui->containerFormatBox->setEnabled(false);
    }

    if(audioRecorder->service()) {
        // Video Output
        QList<QString> videoOutputs;
        videoOutputs = audioRecorder->service()->supportedEndpoints(QAbstractMediaService::VideoOutput);

        // default video output
        if(videoOutputs.size() > 0)
            audioRecorder->service()->setActiveEndpoint(QAbstractMediaService::VideoOutput,videoOutputs.first().toLocal8Bit().constData());

        // try to find a specific one, videoWidget
        for(int i=0;i<videoOutputs.size();i++) {
            if(qstrcmp(videoOutputs.at(i).toLocal8Bit().constData(),"videoWidget") == 0) {
                audioRecorder->service()->setActiveEndpoint(QAbstractMediaService::VideoOutput,videoOutputs.at(i).toLocal8Bit().constData());
                break;
            }
        }
        // TODO: how do I show?
    }
}

Recorder::~Recorder()
{
}

void Recorder::updateRecordTime()
{
    QString str = QString("Recorded %1 sec").arg(audioRecorder->duration()/1000);
    ui->statusbar->showMessage(str);
}

void Recorder::setInputDevice(int idx)
{
    if (audioDevice)
        audioDevice->setSelectedDevice(idx);
}

void Recorder::setAudioCodec(int idx)
{
    if (encodeControl) {
        QString codecName = encodeControl->supportedAudioCodecs()[idx];
        encodeControl->setAudioCodec(codecName);
    }
}

void Recorder::setQuality(int value)
{
    if (encodeControl)
        encodeControl->setQuality(value);
}

void Recorder::setContainerFormat(int idx)
{
    if (formatControl)
        formatControl->setFormat(formatControl->supportedFormats()[idx]);
}

void Recorder::record()
{
    if (audioRecorder)
        audioRecorder->record();
    updateRecordTime();
}

void Recorder::pause()
{
    if (audioRecorder)
        audioRecorder->pause();
}

void Recorder::stop()
{
    if (audioRecorder)
        audioRecorder->stop();
}

void Recorder::displayErrorMessage()
{
    QMessageBox::warning(this, "Recorder error", audioRecorder->errorString());
}
