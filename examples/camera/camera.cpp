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

#include "camera.h"

#include <qabstractmediaservice.h>
#include <qmediarecorder.h>
#include <qaudiodeviceendpoint.h>
#include <qaudioencodecontrol.h>
#include <qvideoencodecontrol.h>
#include <qcameracontrol.h>
#include "qmediawidgetendpoint.h"

#include <QtGui>

Camera::Camera()
{
    camera = new QCamera;
    if(camera->service())
        capture = new QMediaRecorder(camera);
    else
        capture = new QMediaRecorder;

    capture->setSink(QMediaSink(QUrl("test.tmp")));

    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    if(capture->service()) {
        // Video Input
        QWidget *videoWidget = capture->service()->createEndpoint<QMediaWidgetEndpoint*>();
        if(videoWidget) {
            videoWidget->setBaseSize(QSize(320,240));
            capture->service()->setVideoOutput(videoWidget);
            layout->addWidget(videoWidget);
            videoWidget->show();
        }
        // Audio Input
        audioDevice = capture->service()->createEndpoint<QAudioDeviceEndpoint*>();
        if(audioDevice) {
            capture->service()->setAudioInput(audioDevice);
            audioDevice->setDirectionFilter(QAudioDeviceEndpoint::InputDevice);
        }
        // Audio Encode Control
        audioEncodeControl = capture->service()->control<QAudioEncodeControl*>();

        connect(capture, SIGNAL(durationChanged(qint64)), this, SLOT(updateProgress(qint64)));
        connect(capture,SIGNAL(stateChanged(QMediaRecorder::State)),this,SLOT(stateChanged(QMediaRecorder::State)));
        //connect(capture, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(displayErrorMessage()));
    }

    deviceBox = new QComboBox(this);
    QList<QByteArray> devices = camera->deviceList();
    for(int i = 0; i < devices.size(); ++i) {
        deviceBox->addItem(devices.at(i));
    }

    connect(deviceBox,SIGNAL(activated(int)),SLOT(deviceChanged(int)));
    layout->addWidget(deviceBox);

    button = new QPushButton(this);
    button->setText(tr("Click to start video"));
    connect(button,SIGNAL(clicked()),SLOT(togglePlay()));
    layout->addWidget(button);

    recTime = new QLabel;
    recTime->setText("0 sec");
    layout->addWidget(recTime);

    window->setLayout(layout);
    setCentralWidget(window);
    window->show();

    active = false;
    framerate = 25;
}

Camera::~Camera()
{
}

void Camera::updateProgress(qint64 pos)
{
    currentTime = pos;
    if(currentTime == 0) currentTime = 1;
    QString text = QString("%1 secs").arg(currentTime/1000);
    recTime->setText(text);
}

void Camera::stateChanged(QMediaRecorder::State state)
{
    qWarning()<<"stateChanged() "<<state;
}

void Camera::deviceChanged(int idx)
{
    QByteArray device;
    device = deviceBox->itemText(idx).toLocal8Bit().constData();
    camera->setDevice(device);
}

void Camera::togglePlay()
{
    if(!camera) return;

    if(!active) {
        recTime->setText("0 sec");
        currentTime = 0;
        capture->record();

        button->setText(tr("Click to stop"));
        active = true;
    } else {
        capture->stop();
        button->setText(tr("Click to start"));
        active = false;
    }
}
