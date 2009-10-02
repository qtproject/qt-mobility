/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDebug>

#include <multimedia/qcamera.h>

#include <multimedia/qmediaobject_p.h>
#include <multimedia/qcameracontrol.h>
#include <multimedia/qcameraexposurecontrol.h>
#include <multimedia/qcamerafocuscontrol.h>
#include <multimedia/qmediarecordercontrol.h>
#include <multimedia/qimageprocessingcontrol.h>
#include <multimedia/qimagecapturecontrol.h>

/*!
    \class QCamera
    \ingroup multimedia

    \preliminary
    \brief The QCamera class provides interface for system
    camera devices.
*/


class QCameraPrivate : public QMediaObjectPrivate
{
public:
    QCameraControl *control;
    QCameraExposureControl *exposureControl;
    QCameraFocusControl *focusControl;
    QImageProcessingControl *imageControl;
    QImageCaptureControl *captureControl;

    QCamera::Error error;
    QString errorString;
};

/*!
    Construct a QCamera from service \a provider and \a parent.
*/

QCamera::QCamera(QObject *parent, QMediaServiceProvider *provider):
    QMediaObject(*new QCameraPrivate, parent, provider->requestService("camera"))
{
    Q_D(QCamera);

    Q_ASSERT(d->service != 0);

    if (d->service) {
        d->control = qobject_cast<QCameraControl *>(d->service->control(QCameraControl_iid));
        d->exposureControl = qobject_cast<QCameraExposureControl *>(d->service->control(QCameraExposureControl_iid));
        d->focusControl = qobject_cast<QCameraFocusControl *>(d->service->control(QCameraFocusControl_iid));
        d->imageControl = qobject_cast<QImageProcessingControl *>(d->service->control(QImageProcessingControl_iid));
        d->captureControl = qobject_cast<QImageCaptureControl *>(d->service->control(QImageCaptureControl_iid));

        connect(d->control, SIGNAL(stateChanged(QCamera::State)), this, SIGNAL(stateChanged(QCamera::State)));
    } else {
        d->control = 0;
        d->exposureControl = 0;
        d->focusControl = 0;
        d->imageControl = 0;
        d->captureControl = 0;
    }

    if (d->exposureControl) {
        connect(d->exposureControl, SIGNAL(flashReady(bool)), this, SIGNAL(flashReady(bool)));
        connect(d->exposureControl, SIGNAL(exposureLocked()), this, SIGNAL(exposureLocked()));
    }

    if (d->focusControl) {
        connect(d->focusControl, SIGNAL(focusStatusChanged(QCamera::FocusStatus)),
                this, SIGNAL(focusStatusChanged(QCamera::FocusStatus)));
        connect(d->focusControl, SIGNAL(zoomValueChanged(qreal)), this, SIGNAL(zoomValueChanged(qreal)));
        connect(d->focusControl, SIGNAL(focusLocked()), this, SIGNAL(focusLocked()));
    }

    if (d->captureControl) {
        connect(d->captureControl, SIGNAL(imageCaptured(QString,QImage)),
                this, SIGNAL(imageCaptured(QString,QImage)));
        connect(d->captureControl, SIGNAL(readyForCaptureChanged(bool)),
                this, SIGNAL(readyForCaptureChanged(bool)));
    }
}

/*!
    Destroys the camera object.
*/

QCamera::~QCamera()
{
}

/*!
    Returns the error state of the object.
*/

QCamera::Error QCamera::error() const
{
    return d_func()->error;
}

QString QCamera::errorString() const
{
    return d_func()->errorString;
}

/*!
    Starts the camera.

    This can involve powering up the camera device and can be asynchronyous.

    State is changed to QCamera::ActiveState if camera is started
    succesfuly, otherwise error() signal is emited.
*/

void QCamera::start()
{
    Q_D(QCamera);

    if(d->control)
        d->control->start();
}

/*!
    Stops the camera.
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

    if(d->exposureControl)
        d->exposureControl->lockExposure();
    else
        emit exposureLocked();
}

/*!
    Unlock the exposure.
*/

void QCamera::unlockExposure()
{
    Q_D(QCamera);

    if(d->exposureControl)
        d->exposureControl->unlockExposure();
}

/*!
    Lock the focus.
*/

void QCamera::lockFocus()
{
    Q_D(QCamera);

    if(d->focusControl)
        d->focusControl->lockFocus();
    else
        emit focusLocked();
}

/*!
    Unlock the focus.
*/

void QCamera::unlockFocus()
{
    Q_D(QCamera);

    if(d->focusControl)
        d->focusControl->unlockFocus();
}

/*!
    Returns a list of camera device's available.
*/

QStringList QCamera::devices() const
{
    if (d_func()->service != 0)
        return d_func()->service->supportedEndpoints(QMediaService::VideoInput);

    return QStringList();
}

/*!
    Set the camera device being used to \a device
*/

void QCamera::setDevice(const QString& device)
{
    Q_D(QCamera);

    if (d->service)
        d->service->setActiveEndpoint(QMediaService::VideoInput, device);
}

/*!
    Returns the description of the \a device.
*/

QString QCamera::deviceDescription(const QString &device) const
{
    if (d_func()->service != 0)
        return d_func()->service->endpointDescription(QMediaService::VideoInput, device);
    else
        return device;
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
    return d_func()->exposureControl ? d_func()->exposureControl->flashMode() : QCamera::FlashOff;
}

/*!
    Set the flash mode to \a mode
*/

void QCamera::setFlashMode(QCamera::FlashMode mode)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setFlashMode(mode);
}

/*!
    Returns the flash modes available.
*/

QCamera::FlashModes QCamera::supportedFlashModes() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->supportedFlashModes() : QCamera::FlashOff;
}

/*!
    Returns true if flash is charged.
*/

bool QCamera::isFlashReady() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->isFlashReady() : false;
}

/*!
    Returns the focus mode being used.
*/

QCamera::FocusMode QCamera::focusMode() const
{
    return d_func()->focusControl ? d_func()->focusControl->focusMode() : QCamera::AutoFocus;
}

/*!
    Set the focus mode to \a mode
*/

void QCamera::setFocusMode(QCamera::FocusMode mode)
{
    if (d_func()->focusControl)
        d_func()->focusControl->setFocusMode(mode);
}

/*!
    Returns the focus modes available.
*/

QCamera::FocusModes QCamera::supportedFocusModes() const
{
    return d_func()->focusControl ? d_func()->focusControl->supportedFocusModes() : QCamera::AutoFocus;
}

/*!
    Returns the focus status
*/

QCamera::FocusStatus QCamera::focusStatus() const
{
    return d_func()->focusControl ? d_func()->focusControl->focusStatus() : QCamera::FocusDisabled;
}

/*!
    Returns true if macro focusing enabled.
*/

bool QCamera::macroFocusingEnabled() const
{
    return d_func()->focusControl ? d_func()->focusControl->macroFocusingEnabled() : false;
}

/*!
    Returns true if macro focusing is supported.
*/

bool QCamera::isMacroFocusingSupported() const
{
    return d_func()->focusControl ? d_func()->focusControl->isMacroFocusingSupported() : false;
}

/*!
    Set macro focusing to \a enabled.
*/

void QCamera::setMacroFocusingEnabled(bool enabled)
{
    if (d_func()->focusControl)
        d_func()->focusControl->setMacroFocusingEnabled(enabled);
}

/*!
    Returns the exposure mode being used.
*/

QCamera::ExposureMode QCamera::exposureMode() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->exposureMode() : QCamera::ExposureAuto;
}

/*!
    Set exposure mode to \a mode
*/

void QCamera::setExposureMode(QCamera::ExposureMode mode)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setExposureMode(mode);
}

/*!
    Return the exposure modes available.
*/

QCamera::ExposureModes QCamera::supportedExposureModes() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->supportedExposureModes() : QCamera::ExposureAuto;
}

/*!
    Returns the exposure compensation.
*/

qreal QCamera::exposureCompensation() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->exposureCompensation() : 0;
}

/*!
    Sets the exposure compensation to \a ev
*/

void QCamera::setExposureCompensation(qreal ev)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setExposureCompensation(ev);
}

/*!
    Returns the metering mode being used.
*/

QCamera::MeteringMode QCamera::meteringMode() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->meteringMode() : QCamera::MeteringMatrix;
}

/*!
    Sets the metering mode to \a mode.
*/

void QCamera::setMeteringMode(QCamera::MeteringMode mode)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setMeteringMode(mode);
}

/*!
    Returns the metering modes available.
*/

QCamera::MeteringModes QCamera::supportedMeteringModes() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->supportedMeteringModes() : QCamera::MeteringMatrix;
}

/*!
    Returns the white balance mode being used.
*/

QCamera::WhiteBalanceMode QCamera::whiteBalanceMode() const
{
    return d_func()->imageControl ? d_func()->imageControl->whiteBalanceMode() : QCamera::WhiteBalanceAuto;
}

/*!
    Sets the white balance to \a mode.
*/

void QCamera::setWhiteBalanceMode(QCamera::WhiteBalanceMode mode)
{
    if (d_func()->imageControl)
        d_func()->imageControl->setWhiteBalanceMode(mode);
}

/*!
    Returns the white balance modes available.
*/

QCamera::WhiteBalanceModes QCamera::supportedWhiteBalanceModes() const
{
    return d_func()->imageControl ? d_func()->imageControl->supportedWhiteBalanceModes() : QCamera::WhiteBalanceAuto;
}

/*!
    Returns the current color temperature if the
    manual white balance is active, otherwise the
    return value is undefined.
*/

int QCamera::manualWhiteBalance() const
{
    return d_func()->imageControl ? d_func()->imageControl->manualWhiteBalance() : -1;
}

/*!
    Sets manual white balance to \a colorTemperature
*/

void QCamera::setManualWhiteBalance(int colorTemperature)
{
    if (d_func()->imageControl)
        d_func()->imageControl->setManualWhiteBalance(colorTemperature);
}

/*!
    Returns the ISO sensitivity.
*/

int QCamera::isoSensitivity() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->isoSensitivity() : -1;
}

/*!
    Returns the sensitivity ranges available.
*/

QPair<int, int> QCamera::supportedIsoSensitivityRange() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->supportedIsoSensitivityRange() : qMakePair<int,int>(-1,-1);
}

/*!
    Sets the manual sensitivity to \a iso
*/

void QCamera::setManualIsoSensitivity(int iso)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setManualIsoSensitivity(iso);
}

/*!
     Turn on auto sensitivity
*/

void QCamera::setAutoIsoSensitivity()
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setAutoIsoSensitivity();
}

/*!
    Returns the current aperture.
*/

qreal QCamera::aperture() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->aperture() : -1.0;
}

/*!
    Returns the supported aperture ranges available.
*/

QPair<qreal, qreal> QCamera::supportedApertureRange() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->supportedApertureRange() : qMakePair<qreal,qreal>(-1,-1);
}

/*!
    Sets the aperture to \a aperture
*/

void QCamera::setManualAperture(qreal aperture)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setManualAperture(aperture);
}

/*!
    Turn on auto aperture
*/

void QCamera::setAutoAperture()
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setAutoAperture();
}

/*!
    Return the current shutter speed.
*/

qreal QCamera::shutterSpeed() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->shutterSpeed() : -1;
}

/*!
    Return the shutter speed ranges available.
*/

QPair<qreal, qreal> QCamera::supportedShutterSpeedRange() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->supportedShutterSpeedRange() : qMakePair<qreal,qreal>(-1,-1);
}

/*!
    Set the shutter speed to \a seconds
*/

void QCamera::setManualShutterSpeed(qreal seconds)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setManualShutterSpeed(seconds);
}

/*!
    Turn on auto shutter speed
*/

void QCamera::setAutoShutterSpeed()
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setAutoShutterSpeed();
}

/*!
    Returns the maximum optical zoom
*/

qreal QCamera::maximumOpticalZoom() const
{
    return d_func()->focusControl ? d_func()->focusControl->maximumOpticalZoom() : 1.0;
}

/*!
    Returns the maximum digital zoom
*/

qreal QCamera::maximumDigitalZoom() const
{
    return d_func()->focusControl ? d_func()->focusControl->maximumDigitalZoom() : 1.0;
}

/*!
    Returns the current zoom.
*/

qreal QCamera::zoomValue() const
{
    return d_func()->focusControl ? d_func()->focusControl->zoomValue() : 1.0;
}

/*!
    Set the zoom to \a value.
*/

void QCamera::zoomTo(qreal value)
{
    if (d_func()->focusControl)
        d_func()->focusControl->zoomTo(qBound<qreal>(1.0,value,maximumOpticalZoom()*maximumDigitalZoom()));
}

/*!
    Return true if exposure locked.
*/

bool QCamera::isExposureLocked() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->isExposureLocked() : true;
}

/*!
    Return true if focus locked.
*/

bool QCamera::isFocusLocked() const
{
    return d_func()->focusControl ? d_func()->focusControl->isFocusLocked() : true;
}

/*!
  \property QCamera::readyForCapture
   Indicates the camera is ready to capture an image immediately.
*/

bool QCamera::isReadyForCapture() const
{
    return d_func()->captureControl ? d_func()->captureControl->isReadyForCapture() : false;
}

/*!
    Capture the image and save it to file.
    This operation is asynchronous in majority of cases,
    followed by signal QCamera::imageCaptured() or error()
*/
void QCamera::capture(const QString &file)
{
    Q_D(QCamera);

    if (d->captureControl) {
        d->captureControl->capture(file);
    } else {
        d->error = NotReadyToCaptureError;
        d->errorString = tr("Device does not support images capture.");

        emit error(d->error);
    }
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
    \fn void QCamera::zoomValueChanged(qreal value)

    Signal emitted when zoom value changes to new \a value.
*/

/*!
    \enum QCamera::Error

    \value  NoError      No errors have occurred.
    \value  CameraError  An error has occurred.
*/

/*!
    \fn void QCamera::error(QCamera::Error value)

    Signal emitted when error state changes to \a value.
*/

