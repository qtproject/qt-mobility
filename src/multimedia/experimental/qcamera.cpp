/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDebug>

#include <experimental/qcamera.h>

#include <qmediaobject_p.h>
#include <experimental/qcameracontrol.h>
#include <experimental/qcameraexposurecontrol.h>
#include <experimental/qcamerafocuscontrol.h>
#include <qmediarecordercontrol.h>
#include <experimental/qimageprocessingcontrol.h>
#include <experimental/qimagecapturecontrol.h>
#include <qvideodevicecontrol.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QCamera

    \preliminary
    \brief The QCamera class provides interface for system
    camera devices.

    QCamera can be used with QVideoWidget for viewfinder display
    and QMediaRecorder for video recording.

    \code
        camera = new QCamera;
        viewFinder = new QVideoWidget(camera);
        viewFinder->show();

        recorder = QMediaRecorder(camera);

        camera->start();
    \endcode

The Camera API of Qt Mobility is still in \bold ALPHA. It has not undergone
the same level of review and testing as the rest of the APIs.

The API exposed by the classes in this component are not stable, and will
undergo modification or removal prior to the final release of Qt Mobility.
*/


class QCameraPrivate : public QMediaObjectPrivate
{
    Q_DECLARE_NON_CONST_PUBLIC(QCamera)
public:
    void initControls();

    QCameraControl *control;
    QCameraExposureControl *exposureControl;
    QCameraFocusControl *focusControl;
    QImageProcessingControl *imageControl;
    QImageCaptureControl *captureControl;

    QCamera::Error error;
    QString errorString;

    void _q_error(int error, const QString &errorString);
    void unsetError() { error = QCamera::NoError; errorString.clear(); };
};

void QCameraPrivate::_q_error(int error, const QString &errorString)
{
    Q_Q(QCamera);

    this->error = QCamera::Error(error);
    this->errorString = errorString;

    emit q->error(this->error);
}

void QCameraPrivate::initControls()
{
    Q_Q(QCamera);

    if (service) {
        control = qobject_cast<QCameraControl *>(service->control(QCameraControl_iid));
        exposureControl = qobject_cast<QCameraExposureControl *>(service->control(QCameraExposureControl_iid));
        focusControl = qobject_cast<QCameraFocusControl *>(service->control(QCameraFocusControl_iid));
        imageControl = qobject_cast<QImageProcessingControl *>(service->control(QImageProcessingControl_iid));
        captureControl = qobject_cast<QImageCaptureControl *>(service->control(QImageCaptureControl_iid));

        if (control) {
            q->connect(control, SIGNAL(stateChanged(QCamera::State)), q, SIGNAL(stateChanged(QCamera::State)));
            q->connect(control, SIGNAL(error(int,QString)), q, SLOT(_q_error(int,QString)));
        }

        error = QCamera::NoError;
    } else {
        control = 0;
        exposureControl = 0;
        focusControl = 0;
        imageControl = 0;
        captureControl = 0;

        error = QCamera::ServiceMissingError;
        errorString = QCamera::tr("The camera service is missing");
    }

    if (exposureControl) {
        q->connect(exposureControl, SIGNAL(flashReady(bool)), q, SIGNAL(flashReady(bool)));
        q->connect(exposureControl, SIGNAL(exposureLocked()), q, SIGNAL(exposureLocked()));

        q->connect(exposureControl, SIGNAL(apertureChanged(qreal)),
                q, SIGNAL(apertureChanged(qreal)));
        q->connect(exposureControl, SIGNAL(apertureRangeChanged()),
                q, SIGNAL(apertureRangeChanged()));
        q->connect(exposureControl, SIGNAL(shutterSpeedChanged(qreal)),
                q, SIGNAL(shutterSpeedChanged(qreal)));
        q->connect(exposureControl, SIGNAL(isoSensitivityChanged(int)),
                q, SIGNAL(isoSensitivityChanged(int)));
    }

    if (focusControl) {
        q->connect(focusControl, SIGNAL(focusStatusChanged(QCamera::FocusStatus)),
                q, SIGNAL(focusStatusChanged(QCamera::FocusStatus)));
        q->connect(focusControl, SIGNAL(zoomValueChanged(qreal)), q, SIGNAL(zoomValueChanged(qreal)));
        q->connect(focusControl, SIGNAL(focusLocked()), q, SIGNAL(focusLocked()));
    }

    if (captureControl) {
        q->connect(captureControl, SIGNAL(imageCaptured(QString,QImage)),
                q, SIGNAL(imageCaptured(QString,QImage)));
        q->connect(captureControl, SIGNAL(imageSaved(QString)),
                        q, SIGNAL(imageSaved(QString)));
        q->connect(captureControl, SIGNAL(readyForCaptureChanged(bool)),
                q, SIGNAL(readyForCaptureChanged(bool)));
    }
}

/*!
    Construct a QCamera from service \a provider and \a parent.
*/

QCamera::QCamera(QObject *parent, QMediaServiceProvider *provider):
    QMediaObject(*new QCameraPrivate, parent, provider->requestService(Q_MEDIASERVICE_CAMERA))
{
    Q_D(QCamera);
    d->initControls();
}

/*!
    Construct a QCamera from device name \a device and \a parent.
*/

QCamera::QCamera(const QByteArray& device, QObject *parent):
    QMediaObject(*new QCameraPrivate, parent,
                  QMediaServiceProvider::defaultServiceProvider()->requestService(Q_MEDIASERVICE_CAMERA, QMediaServiceProviderHint(device)))
{
    Q_D(QCamera);
    d->initControls();

    if (d->service != 0) {
        //pass device name to service
        QVideoDeviceControl *deviceControl =
                qobject_cast<QVideoDeviceControl*>(d->service->control(QVideoDeviceControl_iid));

        if (deviceControl) {
            QString deviceName(device);

            for (int i=0; i<deviceControl->deviceCount(); i++) {
                if (deviceControl->deviceName(i) == deviceName) {
                    deviceControl->setSelectedDevice(i);
                    break;
                }
            }
        }
    }
}

/*!
    Destroys the camera object.
*/

QCamera::~QCamera()
{
}


/*!
    Returns true if the camera is supported, otherwise returns false.
*/
bool QCamera::isAvailable() const
{
    return d_func()->control != NULL;
}


/*!
    Returns the error state of the object.
*/

QCamera::Error QCamera::error() const
{
    return d_func()->error;
}

/*!
    Returns a string describing a camera's error state.
*/
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

    d->unsetError();

    if (d->control)
        d->control->start();
    else {
        d->errorString = tr("The camera service is missing");
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                                    Q_ARG(QCamera::Error, QCamera::ServiceMissingError));
    }
}

/*!
    Stops the camera.
*/

void QCamera::stop()
{
    Q_D(QCamera);

    d->unsetError();

    if(d->control)
        d->control->stop();
    else
        d->_q_error(QCamera::ServiceMissingError, tr("The camera service is missing"));
}

/*!
    Lock the exposure.
*/

void QCamera::lockExposure()
{
    Q_D(QCamera);

    d->unsetError();

    if(d->exposureControl)
        d->exposureControl->lockExposure();
    else
        d->_q_error(NotSupportedFeatureError, tr("Exposure locking is not supported"));
}

/*!
    Unlock the exposure.
*/

void QCamera::unlockExposure()
{
    Q_D(QCamera);

    d->unsetError();

    if(d->exposureControl)
        d->exposureControl->unlockExposure();    
}

/*!
    Lock the focus.
*/

void QCamera::lockFocus()
{
    Q_D(QCamera);

    d->unsetError();

    if(d->focusControl)
        d->focusControl->lockFocus();
    else
        d->_q_error(NotSupportedFeatureError, tr("Focus locking is not supported"));
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
    Returns a list of camera device's available from the default service provider.
*/

QList<QByteArray> QCamera::availableDevices()
{
    return QMediaServiceProvider::defaultServiceProvider()->devices(QByteArray(Q_MEDIASERVICE_CAMERA));
}

/*!
    Returns the description of the \a device.
*/

QString QCamera::deviceDescription(const QByteArray &device)
{
    return QMediaServiceProvider::defaultServiceProvider()->deviceDescription(QByteArray(Q_MEDIASERVICE_CAMERA), device);
}

QCamera::State QCamera::state() const
{
    if(d_func()->control)
        return (QCamera::State)d_func()->control->state();

    return QCamera::StoppedState;
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
    Returns the list of ISO senitivities camera supports.

    If the camera supports arbitrary ISO sensitivities within the supported range,
    *\a continuous is set to true, otherwise *\a continuous is set to false.
*/
QList<int> QCamera::supportedIsoSensitivities(bool *continuous) const
{
    if (continuous)
        *continuous = 0;

    return d_func()->exposureControl ? d_func()->exposureControl->supportedIsoSensitivities(continuous)
                                     : QList<int>();
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
    \property QCamera::shutterSpeed
    \brief The effective length of time the shutter is open in seconds.
*/

/*!
    \fn QCamera::shutterSpeedChanged(qreal speed)

    Signals that a camera's shutter \a speed has changed.
*/

/*!
    \property QCamera::isoSensitivity
    \brief The sensor ISO sensitivity. Lower sensitivity, the noise is lower, but more light is needed.
*/

/*!
    \property QCamera::aperture
    \brief Lens aperture is specified as an F number, the ratio of the focal length to effective aperture diameter.
*/

qreal QCamera::aperture() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->aperture() : -1.0;
}

/*!
    Returns the list of aperture values camera supports.
    The apertures list can change depending on the focal length,
    in such a case the apertureRangeChanged() signal is emited.

    If the camera supports arbitrary aperture values within the supported range,
    *\a continuous is set to true, otherwise *\a continuous is set to false.
*/
QList<qreal> QCamera::supportedApertures(bool * continuous) const
{
    if (continuous)
        *continuous = 0;

    return d_func()->exposureControl ? \
           d_func()->exposureControl->supportedApertures(continuous) : QList<qreal>();
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
    Return the current shutter speed in seconds.
*/

qreal QCamera::shutterSpeed() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->shutterSpeed() : -1;
}

/*!
    Returns the list of shutter speed values in seconds camera supports.

    If the camera supports arbitrary shutter speed values within the supported range,
    *\a continuous is set to true, otherwise *\a continuous is set to false.
*/
QList<qreal> QCamera::supportedShutterSpeeds(bool *continuous) const
{
    if (continuous)
        *continuous = false;

    return d_func()->exposureControl ?
            d_func()->exposureControl->supportedShutterSpeeds(continuous) : QList<qreal>();
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
    \fn QCamera::readyForCaptureChanged(bool ready)

    Signals that a camera's \a ready for capture state has changed.
*/

/*!
    Capture the image and save it to \a file.
    This operation is asynchronous in majority of cases,
    followed by signal QCamera::imageCaptured() or error()
*/
void QCamera::capture(const QString &file)
{
    Q_D(QCamera);

    d->unsetError();

    if (d->captureControl) {
        d->captureControl->capture(file);
    } else {
        d->error = NotSupportedFeatureError;
        d->errorString = tr("Device does not support images capture.");

        emit error(d->error);
    }
}


/*!
    \enum QCamera::State
    \value ActiveState  The camera has been started and can produce data.
    \value SuspendedState The camera is temporary not available,
           usually as a result of higher priority client requested
           the camera device.
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
    \value InfinityFocus        Focus strictly to infinity.
    \value HyperfocalFocus      Focus to hyperfocal distance, with with the maximum depth of field achieved.
                                All objects at distances from half of this
                                distance out to infinity will be acceptably sharp.
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
    \fn void QCamera::apertureChanged(qreal value)

    Signal emitted when aperature changes to \a value.
*/

/*!
    \fn void QCamera::apertureRangeChanged()

    Signal emitted when aperature range has changed.
*/

/*!
    \fn QCamera::imageCaptured(const QString &fileName, const QImage &preview)

    Signals that an image intendec to be saved to to \a fileName
    has been captured and a \a preview is available.
*/


/*!
    \fn QCamera::imageSaved(const QString &fileName)

    Signals that an captured image has been saved to \a fileName.
*/


/*!
    \fn void QCamera::isoSensitivityChanged(int value)

    Signal emitted when sensitivity changes to \a value.
*/

/*!
    \enum QCamera::Error

    \value  NoError      No errors have occurred.
    \value  CameraError  An error has occurred.
    \value  NotReadyToCaptureError System resource not available.
    \value  InvalidRequestError System resource doesn't support functionality.
    \value  ServiceMissingError No service available.
    \value  NotSupportedFeatureError The feature is not supported.
*/

/*!
    \fn void QCamera::error(QCamera::Error value)

    Signal emitted when error state changes to \a value.
*/


#include "moc_qcamera.cpp"
QTM_END_NAMESPACE

