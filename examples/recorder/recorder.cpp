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
#include "qaudiodeviceendpoint.h"

#include <QtGui>

Recorder::Recorder()
{
    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    deviceBox = new QComboBox(this);

    audioCapture = new QMediaCapture;
    audioCapture->setSink(QMediaSink(QUrl("/tmp/test.ogg")));
    audioDevice = audioCapture->service()->createEndpoint<QAudioDeviceEndpoint*>();

    if (audioDevice) {
        audioCapture->service()->setAudioInput(audioDevice);
        audioDevice->setDirectionFilter(QAudioDeviceEndpoint::InputDevice);
        for (int i=0; i<audioDevice->deviceCount(); i++) {
            deviceBox->addItem(audioDevice->description(i));
        }
    } else
        deviceBox->setEnabled(false);

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

void Recorder::deviceChanged(int idx)
{
    if (audioDevice)
        audioDevice->setSelectedDevice(idx);
}

void Recorder::toggleRecord()
{
    if (!audioCapture) return;

    if (!active) {
        recTime->setText("0 sec");
        currentTime = 0;
        audioCapture->record();

        button->setText(tr("Click to stop recording"));
        active = true;
    } else {
        button->setText(tr("Click to start recording"));
        audioCapture->stop();
        active = false;
    }
}
