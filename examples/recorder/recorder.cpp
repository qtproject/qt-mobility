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

#include <QtGui>

#ifdef AUDIOSERVICES
#include <QtMultimedia/QAudioDeviceInfo>
#endif

Recorder::Recorder()
{
    recorder = new QAudioCapture;

#ifdef AUDIOSERVICES
    QAudioFormat format;
    format.setFrequency(8000);
    format.setChannels(1);
    format.setSampleSize(8);
    format.setSampleType(QAudioFormat::UnSignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");

    recorder->setFormat(format);
#endif

    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    deviceBox = new QComboBox(this);
    QList<QByteArray> devices = recorder->service()->supportedEndpointInterfaces(QMediaEndpointInterface::Input);
    for(int i = 0; i < devices.size(); ++i) {
        deviceBox->addItem(devices.at(i), i);
    }
    connect(deviceBox,SIGNAL(activated(int)),SLOT(deviceChanged(int)));
    layout->addWidget(deviceBox);

    button = new QPushButton(this);
    button->setText(tr("Click to start recording"));
    connect(button,SIGNAL(clicked()),SLOT(toggleRecord()));
    layout->addWidget(button);

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
}

#ifdef AUDIOSERVICES
void Recorder::state(QAudio::State state)
{
    qWarning()<<" state="<<state;
}
#endif

void Recorder::deviceChanged(int idx)
{
#ifdef AUDIOSERVICES
    device = deviceBox->itemData(idx).value<QAudioDeviceId>();
    QObject* input = recorder->service()->createEndpoint(deviceBox->itemText(idx).toLocal8Bit().constData());
    QFile* output = qobject_cast<QFile*>(recorder->service()->createEndpoint("QFile"));
    if(output) {
        output->setFileName("/tmp/test.raw");
        output->open(QIODevice::WriteOnly);
        recorder->service()->setAudioInput(input);
        recorder->service()->setAudioOutput(qobject_cast<QObject*>(output));
    }
#endif
}

void Recorder::toggleRecord()
{
    if(!recorder) return;

    if(!active) {
        recorder->start();
        button->setText(tr("Click to stop recording"));
        active = true;
    } else {
        recorder->stop();
        button->setText(tr("Click to start recording"));
        QFile* output = qobject_cast<QFile*>(recorder->service()->audioOutput());
        output->close();
        active = false;
    }
}
