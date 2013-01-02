/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "simulatorcameraservice.h"
#include "simulatorcamerasession.h"
#include "simulatorcameracontrol.h"
#include "simulatorcameraimagecapturecontrol.h"
#include "simulatorcameraexposurecontrol.h"

#include "simulatorvideoinputdevicecontrol.h"
#include "simulatorvideorenderercontrol.h"
#include "../qsimulatormultimediaconnection_p.h"

#include <qmediaserviceprovider.h>
#include <QtCore/QDebug>

QTM_USE_NAMESPACE;
using namespace Simulator;

SimulatorCameraService::SimulatorCameraService(const QString &service, MultimediaConnection *multimediaConnection,
                             QObject *parent):
    QMediaService(parent)
{
    Q_UNUSED(service)
    mCaptureSession = new SimulatorCameraSession(this);
    mCameraControl = new SimulatorCameraControl(mCaptureSession);
    mVideoInputDeviceControl = new QSimulatorVideoInputDeviceControl(mCaptureSession);
    mVideoInputDeviceControl->updateDeviceList(get_qtCameraData());
    mVideoRendererControl = new SimulatorVideoRendererControl(mCaptureSession, this);
    mImageCaptureControl = new SimulatorCameraImageCaptureControl(mCaptureSession, this);
    mExposureControl = new SimulatorCameraExposureControl(mCaptureSession, this);

    connect(multimediaConnection, SIGNAL(cameraDataChanged(QtMobility::QCameraData)),
            SLOT(updateCameraData(QtMobility::QCameraData)));
    connect(multimediaConnection, SIGNAL(cameraAdded(QString,QtMobility::QCameraData::QCameraDetails)),
            mVideoInputDeviceControl, SLOT(addDevice(QString,QtMobility::QCameraData::QCameraDetails)));
    connect(multimediaConnection, SIGNAL(cameraRemoved(QString)),
            mVideoInputDeviceControl, SLOT(removeDevice(QString)));
    connect(multimediaConnection, SIGNAL(cameraChanged(QString,QtMobility::QCameraData::QCameraDetails)),
            mVideoInputDeviceControl, SLOT(changeDevice(QString,QtMobility::QCameraData::QCameraDetails)));
    connect(multimediaConnection, SIGNAL(cameraChanged(QString,QtMobility::QCameraData::QCameraDetails)),
            SLOT(changeCamera(QString,QtMobility::QCameraData::QCameraDetails)));
    connect(mCameraControl, SIGNAL(startCamera()),
            mVideoRendererControl, SLOT(showImage()));
    connect(mCameraControl, SIGNAL(stopCamera()),
            mVideoRendererControl, SLOT(stop()));
    connect(mVideoInputDeviceControl, SIGNAL(selectedDeviceChanged(QString)),
            SLOT(updateCameraPicture(QString)));
    connect(mCaptureSession->settings(), SIGNAL(isoSensitivityChanged()), mVideoRendererControl, SLOT(showImage()));
    connect(mCaptureSession->settings(), SIGNAL(apertureChanged()), mVideoRendererControl, SLOT(showImage()));
    connect(mCaptureSession->settings(), SIGNAL(shutterSpeedChanged()), mVideoRendererControl, SLOT(showImage()));
    connect(mCaptureSession->settings(), SIGNAL(exposureCompensationChanged()), mVideoRendererControl, SLOT(showImage()));
    mCaptureSession->setImage(mVideoRendererControl->image());
    mVideoInputDeviceControl->setSelectedDevice(mVideoInputDeviceControl->defaultDevice());
}

SimulatorCameraService::~SimulatorCameraService()
{
}

QMediaControl *SimulatorCameraService::requestControl(const char *name)
{
    if (!mCaptureSession)
        return 0;

    if (qstrcmp(name,QCameraControl_iid) == 0)
        return mCameraControl;

    if (qstrcmp(name,QVideoDeviceControl_iid) == 0)
        return mVideoInputDeviceControl;

    if (qstrcmp(name, QVideoRendererControl_iid) == 0)
        return mVideoRendererControl;

    if (qstrcmp(name, QCameraImageCaptureControl_iid) == 0)
        return mImageCaptureControl;

    if (qstrcmp(name, QCameraExposureControl_iid) == 0)
        return mExposureControl;

    return 0;
}

void SimulatorCameraService::releaseControl(QMediaControl *control)
{
    Q_UNUSED(control)
}

void SimulatorCameraService::updateCameraData(const QtMobility::QCameraData &data)
{
    mVideoInputDeviceControl->updateDeviceList(data);
    QString currentDevice = mVideoInputDeviceControl->deviceName(mVideoInputDeviceControl->selectedDevice());
    if (!data.cameras.contains(currentDevice))
        return;

    updateCurrentDeviceImage(data.cameras.value(currentDevice).imagePath);
}

void SimulatorCameraService::changeCamera(const QString &name, const QtMobility::QCameraData::QCameraDetails &details)
{
    QString currentDevice = mVideoInputDeviceControl->deviceName(mVideoInputDeviceControl->selectedDevice());
    if (currentDevice != name)
        return;

    updateCurrentDeviceImage(details.imagePath);
}

void SimulatorCameraService::updateCameraPicture(const QString &name)
{
    QtMobility::QCameraData data = QtMobility::get_qtCameraData();
    if (!data.cameras.contains(name))
        return;

    updateCurrentDeviceImage(data.cameras.value(name).imagePath);
}

void SimulatorCameraService::updateCurrentDeviceImage(const QString &imagePath)
{
    mVideoRendererControl->setImagePath(imagePath);
    mCaptureSession->setImage(mVideoRendererControl->image());
}

#include "moc_simulatorcameraservice.cpp"
