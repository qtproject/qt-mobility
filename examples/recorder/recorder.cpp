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

#include <qabstractmediaservice.h>
#include "qiodeviceendpoint.h"

#include <QtGui>

#ifdef AUDIOSERVICES
#include <QtMultimedia/QAudioDeviceInfo>
#endif

Recorder::Recorder()
{
    file = new QFile("/tmp/test.raw");
    audioCapture = new QAudioCapture;

#ifdef AUDIOSERVICES
    format.setFrequency(8000);
    format.setChannels(1);
    format.setSampleSize(8);
    format.setSampleType(QAudioFormat::UnSignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");
#endif

    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    deviceBox = new QComboBox(this);
#ifdef AUDIOSERVICES
    QList<QAudioDeviceId> devices = QAudioDeviceInfo::deviceList(QAudio::AudioInput);
    for(int i = 0; i < devices.size(); ++i) {
        deviceBox->addItem(QAudioDeviceInfo(devices.at(i)).deviceName(), qVariantFromValue(devices.at(i)));
    }
    audioInput = 0;
#endif
    connect(deviceBox,SIGNAL(activated(int)),SLOT(deviceChanged(int)));
    layout->addWidget(deviceBox);

    button = new QPushButton(this);
    button->setText(tr("Click to start recording"));
    connect(button,SIGNAL(clicked()),SLOT(toggleRecord()));
    layout->addWidget(button);

    recTime = new QLabel;
    recTime->setText("0 sec");
    layout->addWidget(recTime);

    window->setLayout(layout);
    setCentralWidget(window);
    window->show();

    active = false;
}

Recorder::~Recorder()
{
}

void Recorder::status()
{
    currentTime++;
    QString str = QString("%1 sec").arg(currentTime);
    recTime->setText(str);
    qWarning()<<"time: "<<currentTime;
}

#ifdef AUDIOSERVICES
void Recorder::state(QAudio::State state)
{
    if(state == QAudio::StopState)
        qWarning()<<" state = Stop";
    else if(state == QAudio::ActiveState)
        qWarning()<<" state = Active";
    else
        qWarning()<<" state = "<<state;
}
#endif

void Recorder::deviceChanged(int idx)
{
#ifdef AUDIOSERVICES
    if(audioInput)
        delete audioInput;

    device = deviceBox->itemData(idx).value<QAudioDeviceId>();
    audioInput = new QAudioInput(device,format,this);
    connect(audioInput,SIGNAL(stateChanged(QAudio::State)),this,SLOT(state(QAudio::State)));
    connect(audioInput,SIGNAL(notify()),this,SLOT(status()));

    if(!audioCapture) return;

    // Setup and connections
    audioCapture->setOutputDevice(file);
    audioCapture->setInputFormat(format);
    audioCapture->setOutputFormat(format);

#endif
}

void Recorder::toggleRecord()
{
    if(!audioCapture) return;
#ifdef AUDIOSERVICES
    if(!audioInput) return;
#endif
    if(!active) {
        recTime->setText("0 sec");
        currentTime = 0;
        file->open(QIODevice::WriteOnly);
#ifdef AUDIOSERVICES
        QIODevice* tmp = audioInput->start();
        audioCapture->setInputDevice(tmp);
#endif

        button->setText(tr("Click to stop recording"));
        active = true;
    } else {
#ifdef AUDIOSERVICES
        audioInput->stop();
#endif
        button->setText(tr("Click to start recording"));
        file->close();
        active = false;
    }
}
