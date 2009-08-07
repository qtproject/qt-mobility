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

#include <QDebug>

#include "qcamera.h"

#include "qabstractmediaobject_p.h"
#include "qcameracontrol.h"
#include "qcameraservice.h"
#include "qmediarecordercontrol.h"

/*!
    \class QCamera
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QCameraPrivate : public QAbstractMediaObjectPrivate
{
public:
    QAbstractMediaService* service;
    QMediaRecorderControl* media;
    QCameraControl* control;
};

/*!
    Construct a QCamera from \a service with \a parent.
*/

QCamera::QCamera(QAbstractMediaService *service, QObject *parent)
    : QAbstractMediaObject(*new QCameraPrivate, parent)
{
    Q_D(QCamera);

    if(service) {
        d->service = service;
        d->control = qobject_cast<QCameraControl *>(service->control(QCameraControl_iid));
        d->media   = qobject_cast<QMediaRecorderControl *>(service->control(QMediaRecorderControl_iid));
        //connect(d->media,SIGNAL(stateChanged(QCamera::State)),this,SLOT(statusChange(int)));
    } else {
        d->service = 0;
        d->control = 0;
        d->media   = 0;
    }
}

/*!
    Destroys the camera object.
*/

QCamera::~QCamera()
{
}

/*!
    Start camera.
*/

void QCamera::start()
{
    Q_D(QCamera);

    if(d->media)
        d->media->record();
}

/*!
    Stop camera.
*/

void QCamera::stop()
{
    Q_D(QCamera);

    if(d->media)
        d->media->stop();
}

void QCamera::lockExposure()
{
    Q_D(QCamera);

    if(d->control)
        d->control->lockExposure();
}

void QCamera::unlockExposure()
{
    Q_D(QCamera);

    if(d->control)
        d->control->unlockExposure();
}

void QCamera::lockFocus()
{
    Q_D(QCamera);

    if(d->control)
        d->control->lockFocus();
}

void QCamera::unlockFocus()
{
    Q_D(QCamera);

    if(d->control)
        d->control->unlockFocus();
}

QList<QByteArray> QCamera::deviceList()
{
    Q_D(QCamera);

    QList<QByteArray> list;

    if(d->service) {
        QCameraService* serv = qobject_cast<QCameraService*>(d->service);
        if (serv)
            list << serv->deviceList();
    }

    return list;
}

void QCamera::setDevice(const QByteArray& device)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setDevice(device);
}

QString QCamera::deviceDescription(const QByteArray &device)
{
    Q_UNUSED(device);
    return QString();
}

QCamera::State QCamera::state() const
{
    if(d_func()->media)
        return (QCamera::State)d_func()->media->state();

    return QCamera::StoppedState;
}

bool QCamera::isValid() const
{
    return d_func()->control != NULL;
}

QCamera::FlashMode QCamera::flashMode() const
{
    return d_func()->control ? d_func()->control->flashMode() : QCamera::FlashOff;
}

void QCamera::setFlashMode(QCamera::FlashMode mode)
{
    if (d_func()->control)
        d_func()->control->setFlashMode(mode);
}

QCamera::FlashModes QCamera::supportedFlashModes() const
{
    return d_func()->control ? d_func()->control->supportedFlashModes() : QCamera::FlashOff;
}

bool QCamera::isFlashReady() const
{
    return d_func()->control ? d_func()->control->isFlashReady() : true;
}

QCamera::FocusMode QCamera::focusMode() const
{
    return d_func()->control ? d_func()->control->focusMode() : QCamera::AutoFocus;
}

void QCamera::setFocusMode(QCamera::FocusMode mode)
{
    if (d_func()->control)
        d_func()->control->setFocusMode(mode);
}

QCamera::FocusModes QCamera::supportedFocusModes() const
{
    return d_func()->control ? d_func()->control->supportedFocusModes() : QCamera::AutoFocus;
}

QCamera::FocusStatus QCamera::focusStatus() const
{
    return d_func()->control ? d_func()->control->focusStatus() : QCamera::FocusDisabled;
}

bool QCamera::macroFocusingEnabled() const
{
    return d_func()->control ? d_func()->control->macroFocusingEnabled() : false;
}

bool QCamera::isMacroFocusingSupported() const
{
    return d_func()->control ? d_func()->control->isMacroFocusingSupported() : false;
}

void QCamera::setMacroFocusingEnabled(bool enabled)
{
    if (d_func()->control)
        d_func()->control->setMacroFocusingEnabled(enabled);
}

QCamera::ExposureMode QCamera::exposureMode() const
{
    return d_func()->control ? d_func()->control->exposureMode() : QCamera::ExposureAuto;
}

void QCamera::setExposureMode(QCamera::ExposureMode mode)
{
    if (d_func()->control)
        d_func()->control->setExposureMode(mode);
}

QCamera::ExposureModes QCamera::supportedExposureModes() const
{
    return d_func()->control ? d_func()->control->supportedExposureModes() : QCamera::ExposureAuto;
}

double QCamera::exposureCompensation() const
{
    return d_func()->control ? d_func()->control->exposureCompensation() : 0;
}

void QCamera::setExposureCompensation(double ev)
{
    if (d_func()->control)
        d_func()->control->setExposureCompensation(ev);
}

QCamera::MeteringMode QCamera::meteringMode() const
{
    return d_func()->control ? d_func()->control->meteringMode() : QCamera::MeteringMatrix;
}

void QCamera::setMeteringMode(QCamera::MeteringMode mode)
{
    if (d_func()->control)
        d_func()->control->setMeteringMode(mode);
}

QCamera::MeteringModes QCamera::supportedMeteringModes() const
{
    return d_func()->control ? d_func()->control->supportedMeteringModes() : QCamera::MeteringMatrix;
}

QCamera::WhiteBalanceMode QCamera::whiteBalanceMode() const
{
    return d_func()->control ? d_func()->control->whiteBalanceMode() : QCamera::WhiteBalanceAuto;
}

void QCamera::setWhiteBalanceMode(QCamera::WhiteBalanceMode mode)
{
    if (d_func()->control)
        d_func()->control->setWhiteBalanceMode(mode);
}

QCamera::WhiteBalanceModes QCamera::supportedWhiteBalanceModes() const
{
    return d_func()->control ? d_func()->control->supportedWhiteBalanceModes() : QCamera::WhiteBalanceAuto;
}

int QCamera::manualWhiteBalance() const
{
    return d_func()->control ? d_func()->control->manualWhiteBalance() : -1;
}

void QCamera::setManualWhiteBalance(int colorTemperature)
{
    if (d_func()->control)
        d_func()->control->setManualWhiteBalance(colorTemperature);
}

int QCamera::isoSensitivity() const
{
    return d_func()->control ? d_func()->control->isoSensitivity() : -1;
}
QPair<int, int> QCamera::supportedIsoSensitivityRange() const
{
    return d_func()->control ? d_func()->control->supportedIsoSensitivityRange() : qMakePair<int,int>(-1,-1);
}

void QCamera::setManualIsoSensitivity(int iso)
{
    if (d_func()->control)
        d_func()->control->setManualIsoSensitivity(iso);
}

void QCamera::setAutoIsoSensitivity()
{
    if (d_func()->control)
        d_func()->control->setAutoIsoSensitivity();
}

double QCamera::aperture() const
{
    return d_func()->control ? d_func()->control->aperture() : -1.0;
}
QPair<double, double> QCamera::supportedApertureRange() const
{
    return d_func()->control ? d_func()->control->supportedApertureRange() : qMakePair<double,double>(-1,-1);
}

void QCamera::setManualAperture(double aperture)
{
    if (d_func()->control)
        d_func()->control->setManualAperture(aperture);
}

void QCamera::setAutoAperture()
{
    if (d_func()->control)
        d_func()->control->setAutoAperture();
}

double QCamera::shutterSpeed() const
{
    return d_func()->control ? d_func()->control->shutterSpeed() : -1;
}

QPair<double, double> QCamera::supportedShutterSpeedRange() const
{
    return d_func()->control ? d_func()->control->supportedShutterSpeedRange() : qMakePair<double,double>(-1,-1);
}

void QCamera::setManualShutterSpeed(double seconds)
{
    if (d_func()->control)
        d_func()->control->setManualShutterSpeed(seconds);
}

void QCamera::setAutoShutterSpeed()
{
    if (d_func()->control)
        d_func()->control->setAutoShutterSpeed();
}

double QCamera::maximumOpticalZoom() const
{
    return d_func()->control ? d_func()->control->maximumOpticalZoom() : 1.0;
}

double QCamera::maximumDigitalZoom() const
{
    return d_func()->control ? d_func()->control->maximumDigitalZoom() : 1.0;
}

double QCamera::zoomValue() const
{
    return d_func()->control ? d_func()->control->zoomValue() : 1.0;
}

void QCamera::zoomTo(int value)
{
    if (d_func()->control)
        d_func()->control->zoomTo(value);
}

bool QCamera::isExposureLocked() const
{
    return d_func()->control ? d_func()->control->isExposureLocked() : true;
}

bool QCamera::isFocusLocked() const
{
    return d_func()->control ? d_func()->control->isFocusLocked() : true;
}


/*!
    Returns the session object being controlled by this recorder.
*/

QAbstractMediaService *QCamera::service() const
{
    return d_func()->service;
}

QAbstractMediaService* createCameraService(QMediaServiceProvider *provider)
{
    QObject *object = provider ? provider->createObject("com.nokia.qt.Camera/1.0") : 0;

    if (object) {
        QAbstractMediaService *service = qobject_cast<QAbstractMediaService *>(object);

        if (service)
            return service;

        delete object;
    }
    return 0;
}

