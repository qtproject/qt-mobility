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

#include "recorder.h"
#include "ui_recorder.h"

#include <qabstractmediaservice.h>
#include <qmediacapture.h>
#include <qaudiodeviceendpoint.h>
#include <qaudioencodecontrol.h>

#include <QtGui>

Recorder::Recorder(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Recorder)
{
    ui->setupUi(this);

    audioCapture = new QMediaCapture;
    audioCapture->setSink(QMediaSink(QUrl("test.ogg")));
    audioDevice = audioCapture->service()->createEndpoint<QAudioDeviceEndpoint*>();

    connect(audioCapture, SIGNAL(positionChanged(qint64)), this, SLOT(updateRecordTime()));

    if (audioDevice) {
        audioCapture->service()->setAudioInput(audioDevice);
        audioDevice->setDirectionFilter(QAudioDeviceEndpoint::InputDevice);
        for (int i=0; i<audioDevice->deviceCount(); i++) {
             ui->inputDeviceBox->addItem(audioDevice->description(i));
        }
    } else
        ui->inputDeviceBox->setEnabled(false);

    encodeControl = qobject_cast<QAudioEncodeControl*>(
            audioCapture->service()->control("com.nokia.qt.AudioEncodeControl"));

    if (encodeControl) {
        encodeControl->setAudioCodec("lame");

        foreach(const QString &codecName, encodeControl->supportedAudioCodecs()) {
            QString description = encodeControl->codecDescription(codecName);
            ui->audioCodecBox->addItem(codecName+": "+description);
            if (codecName == encodeControl->audioCodec())
                ui->audioCodecBox->setCurrentIndex(ui->audioCodecBox->count()-1);
        }

        ui->qualitySlider->setValue( qRound(encodeControl->quality()));
    } else {
        ui->audioCodecBox->setEnabled(false);
        ui->qualitySlider->setEnabled(false);
    }
}

Recorder::~Recorder()
{
}

void Recorder::updateRecordTime()
{
    QString str = QString("Recorded %1 sec").arg(audioCapture->position()/1000);
    ui->statusbar->showMessage(str);
}

void Recorder::setInputDevice(int idx)
{
    if (audioDevice)
        audioDevice->setSelectedDevice(idx);
}

void Recorder::setCodec(int idx)
{
    if (encodeControl)
        encodeControl->setAudioCodec( encodeControl->supportedAudioCodecs()[idx]);
}

void Recorder::setQuality(int value)
{
    if (encodeControl)
        encodeControl->setQuality(value);    
}

void Recorder::record()
{
    if (audioCapture)
        audioCapture->record();
    updateRecordTime();
}

void Recorder::pause()
{
    if (audioCapture)
        audioCapture->pause();
}

void Recorder::stop()
{
    if (audioCapture)
        audioCapture->stop();
}
