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
        connect(d->control, SIGNAL(stateChanged(QCamera::State)), this, SIGNAL(stateChanged(QCamera::State)));
        connect(d->control, SIGNAL(flashReady(bool)), this, SIGNAL(flashReady(bool)));
        connect(d->control, SIGNAL(focusStatusChanged(QCamera::FocusStatus)), this, SIGNAL(focusStatusChanged(QCamera::FocusStatus)));
        connect(d->control, SIGNAL(zoomValueChanged(double)), this, SIGNAL(zoomValueChanged(double)));
        connect(d->control, SIGNAL(exposureLocked()), this, SIGNAL(exposureLocked()));
        connect(d->control, SIGNAL(focusLocked()), this, SIGNAL(focusLocked()));
    } else {
        d->service = 0;
        d->control = 0;
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

    if(d->control)
        d->control->start();
}

/*!
    Stop camera.
*/

void QCamera::stop()
{
    Q_D(QCamera);

    if(d->control)
        d->control->stop();
}

/*!
    Lock the exposure.
*/

void QCamera::lockExposure()
{
    Q_D(QCamera);

    if(d->control)
        d->control->lockExposure();
}

/*!
    Unlock the exposure.
*/

void QCamera::unlockExposure()
{
    Q_D(QCamera);

    if(d->control)
        d->control->unlockExposure();
}

/*!
    Lock the focus.
*/

void QCamera::lockFocus()
{
    Q_D(QCamera);

    if(d->control)
        d->control->lockFocus();
}

/*!
    Unlock the focus.
*/

void QCamera::unlockFocus()
{
    Q_D(QCamera);

    if(d->control)
        d->control->unlockFocus();
}

/*!
    Returns a list of camera device's available.
*/

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

/*!
    Set the camera device being used to \a device
*/

void QCamera::setDevice(const QByteArray& device)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setDevice(device);
}

/*!
    Returns the description of the \a device.
*/

QString QCamera::deviceDescription(const QByteArray &device)
{
    Q_UNUSED(device);
    return QString();
}

QCamera::State QCamera::state() const
{
    if(d_func()->control)
        return (QCamera::State)d_func()->control->state();

    return QCamera::StoppedState;
}

/*!
    Returns true if camera device available.
*/

bool QCamera::isValid() const
{
    return d_func()->control != NULL;
}

/*!
    Returns the flash mode being used.
*/

QCamera::FlashMode QCamera::flashMode() const
{
    return d_func()->control ? d_func()->control->flashMode() : QCamera::FlashOff;
}

/*!
    Set the flash mode to \a mode
*/

void QCamera::setFlashMode(QCamera::FlashMode mode)
{
    if (d_func()->control)
        d_func()->control->setFlashMode(mode);
}

/*!
    Returns the flash modes available.
*/

QCamera::FlashModes QCamera::supportedFlashModes() const
{
    return d_func()->control ? d_func()->control->supportedFlashModes() : QCamera::FlashOff;
}

/*!
    Returns true if flash is charged.
*/

bool QCamera::isFlashReady() const
{
    return d_func()->control ? d_func()->control->isFlashReady() : true;
}

/*!
    Returns the focus mode being used.
*/

QCamera::FocusMode QCamera::focusMode() const
{
    return d_func()->control ? d_func()->control->focusMode() : QCamera::AutoFocus;
}

/*!
    Set the focus mode to \a mode
*/

void QCamera::setFocusMode(QCamera::FocusMode mode)
{
    if (d_func()->control)
        d_func()->control->setFocusMode(mode);
}

/*!
    Returns the focus modes available.
*/

QCamera::FocusModes QCamera::supportedFocusModes() const
{
    return d_func()->control ? d_func()->control->supportedFocusModes() : QCamera::AutoFocus;
}

/*!
    Returns the focus status
*/

QCamera::FocusStatus QCamera::focusStatus() const
{
    return d_func()->control ? d_func()->control->focusStatus() : QCamera::FocusDisabled;
}

/*!
    Returns true if macro focusing enabled.
*/

bool QCamera::macroFocusingEnabled() const
{
    return d_func()->control ? d_func()->control->macroFocusingEnabled() : false;
}

/*!
    Returns true if macro focusing is supported.
*/

bool QCamera::isMacroFocusingSupported() const
{
    return d_func()->control ? d_func()->control->isMacroFocusingSupported() : false;
}

/*!
    Set macro focusing to \a enabled.
*/

void QCamera::setMacroFocusingEnabled(bool enabled)
{
    if (d_func()->control)
        d_func()->control->setMacroFocusingEnabled(enabled);
}

/*!
    Returns the exposure mode being used.
*/

QCamera::ExposureMode QCamera::exposureMode() const
{
    return d_func()->control ? d_func()->control->exposureMode() : QCamera::ExposureAuto;
}

/*!
    Set exposure mode to \a mode
*/

void QCamera::setExposureMode(QCamera::ExposureMode mode)
{
    if (d_func()->control)
        d_func()->control->setExposureMode(mode);
}

/*!
    Return the exposure modes available.
*/

QCamera::ExposureModes QCamera::supportedExposureModes() const
{
    return d_func()->control ? d_func()->control->supportedExposureModes() : QCamera::ExposureAuto;
}

/*!
    Returns the exposure compensation.
*/

double QCamera::exposureCompensation() const
{
    return d_func()->control ? d_func()->control->exposureCompensation() : 0;
}

/*!
    Sets the exposure compensation to \a ev
*/

void QCamera::setExposureCompensation(double ev)
{
    if (d_func()->control)
        d_func()->control->setExposureCompensation(ev);
}

/*!
    Returns the metering mode being used.
*/

QCamera::MeteringMode QCamera::meteringMode() const
{
    return d_func()->control ? d_func()->control->meteringMode() : QCamera::MeteringMatrix;
}

/*!
    Sets the metering mode to \a mode.
*/

void QCamera::setMeteringMode(QCamera::MeteringMode mode)
{
    if (d_func()->control)
        d_func()->control->setMeteringMode(mode);
}

/*!
    Returns the metering modes available.
*/

QCamera::MeteringModes QCamera::supportedMeteringModes() const
{
    return d_func()->control ? d_func()->control->supportedMeteringModes() : QCamera::MeteringMatrix;
}

/*!
    Returns the white balance mode being used.
*/

QCamera::WhiteBalanceMode QCamera::whiteBalanceMode() const
{
    return d_func()->control ? d_func()->control->whiteBalanceMode() : QCamera::WhiteBalanceAuto;
}

/*!
    Sets the white balance to \a mode.
*/

void QCamera::setWhiteBalanceMode(QCamera::WhiteBalanceMode mode)
{
    if (d_func()->control)
        d_func()->control->setWhiteBalanceMode(mode);
}

/*!
    Returns the white balance modes available.
*/

QCamera::WhiteBalanceModes QCamera::supportedWhiteBalanceModes() const
{
    return d_func()->control ? d_func()->control->supportedWhiteBalanceModes() : QCamera::WhiteBalanceAuto;
}

/*!
    Returns ?
*/

int QCamera::manualWhiteBalance() const
{
    return d_func()->control ? d_func()->control->manualWhiteBalance() : -1;
}

/*!
    Sets manual white balance to \a colorTemperature
*/

void QCamera::setManualWhiteBalance(int colorTemperature)
{
    if (d_func()->control)
        d_func()->control->setManualWhiteBalance(colorTemperature);
}

/*!
    Returns the sensitivity
*/

int QCamera::isoSensitivity() const
{
    return d_func()->control ? d_func()->control->isoSensitivity() : -1;
}

/*!
    Returns the sensitivity ranges available.
*/

QPair<int, int> QCamera::supportedIsoSensitivityRange() const
{
    return d_func()->control ? d_func()->control->supportedIsoSensitivityRange() : qMakePair<int,int>(-1,-1);
}

/*!
    Sets the manual sensitivity to \a iso
*/

void QCamera::setManualIsoSensitivity(int iso)
{
    if (d_func()->control)
        d_func()->control->setManualIsoSensitivity(iso);
}

/*!
     Turn on auto sensitivity
*/

void QCamera::setAutoIsoSensitivity()
{
    if (d_func()->control)
        d_func()->control->setAutoIsoSensitivity();
}

/*!
    Returns the current aperture.
*/

double QCamera::aperture() const
{
    return d_func()->control ? d_func()->control->aperture() : -1.0;
}

/*!
    Returns the supported aperture ranges available.
*/

QPair<double, double> QCamera::supportedApertureRange() const
{
    return d_func()->control ? d_func()->control->supportedApertureRange() : qMakePair<double,double>(-1,-1);
}

/*!
    Sets the aperture to \a aperture
*/

void QCamera::setManualAperture(double aperture)
{
    if (d_func()->control)
        d_func()->control->setManualAperture(aperture);
}

/*!
    Turn on auto aperture
*/

void QCamera::setAutoAperture()
{
    if (d_func()->control)
        d_func()->control->setAutoAperture();
}

/*!
    Return the current shutter speed.
*/

double QCamera::shutterSpeed() const
{
    return d_func()->control ? d_func()->control->shutterSpeed() : -1;
}

/*!
    Return the shutter speed ranges available.
*/

QPair<double, double> QCamera::supportedShutterSpeedRange() const
{
    return d_func()->control ? d_func()->control->supportedShutterSpeedRange() : qMakePair<double,double>(-1,-1);
}

/*!
    Set the shutter speed to \a seconds
*/

void QCamera::setManualShutterSpeed(double seconds)
{
    if (d_func()->control)
        d_func()->control->setManualShutterSpeed(seconds);
}

/*!
    Turn on auto shutter speed
*/

void QCamera::setAutoShutterSpeed()
{
    if (d_func()->control)
        d_func()->control->setAutoShutterSpeed();
}

/*!
    Returns the maximum optical zoom
*/

double QCamera::maximumOpticalZoom() const
{
    return d_func()->control ? d_func()->control->maximumOpticalZoom() : 1.0;
}

/*!
    Returns the maximum digital zoom
*/

double QCamera::maximumDigitalZoom() const
{
    return d_func()->control ? d_func()->control->maximumDigitalZoom() : 1.0;
}

/*!
    Returns the current zoom.
*/

double QCamera::zoomValue() const
{
    return d_func()->control ? d_func()->control->zoomValue() : 1.0;
}

/*!
    Set the zoom to \a value.
*/

void QCamera::zoomTo(int value)
{
    if (d_func()->control)
        d_func()->control->zoomTo(value);
}

/*!
    Return true if exposure locked.
*/

bool QCamera::isExposureLocked() const
{
    return d_func()->control ? d_func()->control->isExposureLocked() : true;
}

/*!
    Return true if focus locked.
*/

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

/*!
    Creates a camera service with parent \a provider
*/

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

/*!
    \enum QCamera::State
    \value ActiveState  The camera has been started and can produce data.
    \value StoppedState The camera has been stopped.
*/

/*!
    \enum QCamera::FlashMode

    \value FlashOff             Flash is Off.
    \value FlashOn              Flash is On.
    \value FlashAuto            Automatic flash.
    \value FlashRedEyeReduction Red eye reduction flash.
    \value FlashFill            Use flash to fillin shadows.
*/

/*!
    \enum QCamera::FocusMode

    \value ManualFocus          Manual focus mode.
    \value AutoFocus            One-shot auto focus mode.
    \value ContinuousFocus      Continuous auto focus mode.
*/

/*!
    \enum QCamera::FocusStatus

    \value FocusDisabled        Manual focus mode used or auto focus is not available.
    \value FocusRequested       Focus request is in progress.
    \value FocusReached         Focus has been reached.
    \value FocusLost            Focus has been lost.
    \value FocusUnableToReach   Unable to achieve focus.
*/

/*!
    \enum QCamera::ExposureMode

    \value ExposureManual        Manual mode.
    \value ExposureAuto          Automatic mode.
    \value ExposureNight         Night mode.
    \value ExposureBacklight     Backlight exposure mode.
    \value ExposureSpotlight     Spotlight exposure mode.
    \value ExposureSports        Spots exposure mode.
    \value ExposureSnow          Snow exposure mode.
    \value ExposureBeach         Beach exposure mode.
    \value ExposureLargeAperture Use larger aperture with small depth of field.
    \value ExposureSmallAperture Use smaller aperture.
    \value ExposurePortrait      Portrait exposure mode.
*/

/*!
    \enum QCamera::ExposureStatus

    \value CorrectExposure       The exposure is correct.
    \value UnderExposure         The photo will be underexposed.
    \value OverExposure          The photo will be overexposed.
*/

/*!
    \enum QCamera::MeteringMode

    \value MeteringAverage       Center weighted average metering mode.
    \value MeteringSpot          Spot metering mode.
    \value MeteringMatrix        Matrix metering mode.
*/

/*!
    \enum QCamera::WhiteBalanceMode

    \value WhiteBalanceManual       Manual white balance. In this mode the white balance should be set with
                                    setManualWhiteBalance()
    \value WhiteBalanceAuto         Auto white balance mode.
    \value WhiteBalanceSunlight     Sunlight white balance mode.
    \value WhiteBalanceCloudy       Cloudy white balance mode.
    \value WhiteBalanceShade        Shade white balance mode.
    \value WhiteBalanceTungsten     Tungsten white balance mode.
    \value WhiteBalanceFluorescent  Fluorescent white balance mode.
    \value WhiteBalanceIncandescent Incandescent white balance mode.
    \value WhiteBalanceFlash        Flash white balance mode.
    \value WhiteBalanceSunset       Sunset white balance mode.
*/

/*!
    \property QCamera::state
    \brief The current state of the camera object.
*/

/*!
    \fn void QCamera::stateChanged(State state)

    Signal emitted when \a state of the Camera object has changed.
*/

/*!
    \fn void QCamera::exposureLocked()

    Signal emitted when exposure locked.
*/

/*!
    \fn void QCamera::focusLocked()

    Signal emitted when focus is locked.
*/

/*!
    \fn void QCamera::focusStatusChanged(FocusStatus status)

    Signal emitted when focus \a status changed.
*/

/*!
    \fn void QCamera::flashReady(bool ready)

    Signal emitted when flash status changed, flash is ready if \a ready true.
*/

/*!
    \fn void QCamera::zoomValueChanged(double value)

    Signal emitted when zoom value changes to new \a value.
*/

