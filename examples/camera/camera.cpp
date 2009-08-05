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
#include <qcameracontrol.h>

#include <QtGui>

Camera::Camera()
{
    camera = new QCamera;
    capture = new QMediaRecorder(camera);

    destination.setDataLocation("file:///tmp/test.avi");
    capture->setSink(destination);

    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

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

void Camera::stateChanged(QCamera::State state)
{
    qWarning()<<"stateChanged() "<<state;
}
/*
void Camera::frameReady(QVideoFrame const &frame)
{
    qWarning()<<"frameReady";

    currentTime++;
    QString str = QString("%1 sec").arg(currentTime/framerate);
    recTime->setText(str);
}
*/
void Camera::deviceChanged(int idx)
{
    QByteArray device;
    device = deviceBox->itemText(idx).toLocal8Bit().constData();
    camera->setDevice(device);
    /*
    QList<QVideoFrame::Type> fmts = camera->supportedColorFormats();
    qWarning()<<"fmts = "<<fmts;
    QList<QSize> sizes = camera->supportedResolutions(fmts.first());
    qWarning()<<"sizes = "<<sizes;
    format = QVideoFormat(sizes.first(),fmts.first());
    camera->setFormat(format);
*/
    // Change a camera property
    camera->setBrightness(camera->brightness());
    framerate = camera->framerate();
    if(framerate == 0) framerate = 25;

    connect(camera,SIGNAL(stateChanged(QCamera::State)),this,SLOT(stateChanged(QCamera::State)));
    //connect(camera,SIGNAL(frameReady(QVideoFrame const&)),this,SLOT(frameReady(QVideoFrame const&)));
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
