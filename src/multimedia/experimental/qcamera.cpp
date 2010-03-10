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
    \brief The QCamera class provides interface for system camera devices.

    \ingroup camera

    QCamera can be used with QVideoWidget for viewfinder display
    and QMediaRecorder for video recording.

    \code
        camera = new QCamera;
        viewFinder = new QVideoWidget(camera);
        viewFinder->show();

        recorder = new QMediaRecorder(camera);
        imageCapture = new QStillImageCapture(camera);

        camera->start();
    \endcode

The Camera API of Qt Mobility is still in \bold Technology Preview. It has
not undergone the same level of review and testing as the rest of the APIs.

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
    void _q_updateFocusStatus(QCamera::FocusStatus);
    void unsetError() { error = QCamera::NoError; errorString.clear(); }
};

void QCameraPrivate::_q_error(int error, const QString &errorString)
{
    Q_Q(QCamera);

    this->error = QCamera::Error(error);
    this->errorString = errorString;

    emit q->error(this->error);
}

void QCameraPrivate::_q_updateFocusStatus(QCamera::FocusStatus status)
{
    Q_Q(QCamera);

    emit q->focusStatusChanged(status);

    if (status == QCamera::FocusReached)
        emit q->focusReached();
    else if (status == QCamera::FocusUnableToReach)
        emit q->focusUnableToReach();
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
            q->connect(control, SIGNAL(captureModeChanged(QCamera::CaptureMode)),
                       q, SIGNAL(captureModeChanged(QCamera::CaptureMode)));
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
                q, SLOT(_q_updateFocusStatus(QCamera::FocusStatus)));
        q->connect(focusControl, SIGNAL(opticalZoomChanged(qreal)), q, SIGNAL(opticalZoomChanged(qreal)));
        q->connect(focusControl, SIGNAL(digitalZoomChanged(qreal)), q, SIGNAL(digitalZoomChanged(qreal)));
    }

    if (imageControl) {
        q->connect(imageControl, SIGNAL(whiteBalanceLocked()),
                   q, SIGNAL(whiteBalanceLocked()));
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
    Returne true if the camera service is ready to use.
*/
bool QCamera::isAvailable() const
{
    if (d_func()->control != NULL)
        return true;
    else
        return false;
}

/*!
    Returns the error state of the camera service.
*/

QtMedia::AvailabilityError QCamera::availabilityError() const
{
    if (d_func()->control != NULL) {
        if (d_func()->error == QCamera::NoError)
            return QtMedia::NoError;
        else
            return QtMedia::ResourceError;
    } else
        return QtMedia::ServiceMissingError;
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
    Returns the supported capture modes.
*/
QCamera::CaptureModes QCamera::supportedCaptureModes() const
{
    return d_func()->control ? d_func()->control->supportedCaptureModes() : QCamera::CaptureDisabled;
}

/*!
  \property QCamera::captureMode

  The type of media (video or still images),
  the camera is configured to capture.
*/

QCamera::CaptureMode QCamera::captureMode() const
{
    return d_func()->control ? d_func()->control->captureMode() : QCamera::CaptureDisabled;
}

void QCamera::setCaptureMode(QCamera::CaptureMode mode)
{
    Q_D(QCamera);
    if (d->control)
        d->control->setCaptureMode(mode);
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
    Lock the white balance.
*/

void QCamera::lockWhiteBalance()
{
    Q_D(QCamera);

    d->unsetError();

    if(d->imageControl)
        d->imageControl->lockWhiteBalance();
    else
        d->_q_error(NotSupportedFeatureError, tr("White balance locking is not supported"));
}

/*!
    Unlock the white balance.
*/

void QCamera::unlockWhiteBalance()
{
    Q_D(QCamera);

    d->unsetError();

    if(d->imageControl)
        d->imageControl->unlockWhiteBalance();
}

/*!
    Starts single or continuous autofocus.

    Does nothing in hyperfocal or infinity focus modes.

    If supported by camera, startFocusing() turns on the manual focusing notifications,
    otherwise it does nothing in manual mode.
*/

void QCamera::startFocusing()
{
    Q_D(QCamera);

    d->unsetError();

    if(d->focusControl)
        d->focusControl->startFocusing();
    else
        d->_q_error(NotSupportedFeatureError, tr("Focus locking is not supported"));
}

/*!
  Cancels the single autofocus request or stops continuous focusing.

  Does nothing in hyperfocal or infinity focus modes.

  If supported by camera, startFocusing() turns off the manual focusing notifications,
  otherwise it does nothing in manual mode.
*/

void QCamera::cancelFocusing()
{
    Q_D(QCamera);

    if(d->focusControl)
        d->focusControl->cancelFocusing();
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

QCamera::FlashModes QCamera::flashMode() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->flashMode() : QCamera::FlashOff;
}

/*!
    Set the flash mode to \a mode.

    Usually the single QCamera::FlashMode flag is used,
    but some non conflicting flags combination are also allowed,
    like QCamera::FlashManual | QCamera::FlashSlowSyncRearCurtain.
*/

void QCamera::setFlashMode(QCamera::FlashModes mode)
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
    Returns the focus status.
*/

QCamera::FocusStatus QCamera::focusStatus() const
{
    return d_func()->focusControl ? d_func()->focusControl->focusStatus() : QCamera::FocusInitial;
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
  \property QCamera::focusPointMode
  The camera focus point selection mode.
*/

QCamera::FocusPointMode QCamera::focusPointMode() const
{
    return d_func()->focusControl ?
            d_func()->focusControl->focusPointMode() :
            QCamera::FocusPointAuto;
}

void QCamera::setFocusPointMode(QCamera::FocusPointMode mode)
{
    if (d_func()->focusControl)
        d_func()->focusControl->setFocusPointMode(mode);
    else if ( mode != QCamera::FocusPointAuto ) {
        d_func()->_q_error(NotSupportedFeatureError, tr("Focus points mode selection is not supported"));
    }
}

/*!
  Returns supported focus selection modes.
 */
QCamera::FocusPointModes QCamera::supportedFocusPointModes() const
{
    return d_func()->focusControl ?
            d_func()->focusControl->supportedFocusPointModes() :
            QCamera::FocusPointAuto;

}

/*!
  \property QCamera::customFocusPoint

  Position of custom focus point, in relative frame coordinates:
  QPointF(0,0) points to the left top frame point, QPointF(0.5,0.5) points to the frame center.

  Custom focus point is used only in FocusPointCustom focus mode.
 */

QPointF QCamera::customFocusPoint() const
{
    return d_func()->focusControl ?
            d_func()->focusControl->customFocusPoint() :
            QPointF(0.5,0.5);
}

void QCamera::setCustomFocusPoint(const QPointF &point)
{
    if (d_func()->focusControl)
        d_func()->focusControl->setCustomFocusPoint(point);
    else {
        d_func()->_q_error(NotSupportedFeatureError, tr("Focus points selection is not supported"));
    }
}

/*!
  \property QCamera::focusZones

  The list of zones, the camera is actually focused on.
 */
QList<QRectF> QCamera::focusZones() const
{
    return d_func()->focusControl ?
            d_func()->focusControl->focusZones() :
            QList<QRectF>();
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
    Returns the current optical zoom value.

    \sa QCamera::opticalZoomChanged(qreal), QCamera::digitalZoom()
*/

qreal QCamera::opticalZoom() const
{
    return d_func()->focusControl ? d_func()->focusControl->opticalZoom() : 1.0;
}

/*!
    Returns the current digital zoom value.

    \sa QCamera::digitalZoomChanged(qreal), QCamera::opticalZoom()
*/
qreal QCamera::digitalZoom() const
{
    return d_func()->focusControl ? d_func()->focusControl->digitalZoom() : 1.0;
}


/*!
    Set the camera \a optical and \a digital zoom values.
*/
void QCamera::zoomTo(qreal optical, qreal digital)
{
    if (d_func()->focusControl)
        d_func()->focusControl->zoomTo(optical, digital);
    else
        d_func()->_q_error(NotSupportedFeatureError, tr("The camera doesn't support zooming."));

}

/*!
    Returns true if exposure is locked.
*/

bool QCamera::isExposureLocked() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->isExposureLocked() : true;
}

/*!
    Returns true if white balance is locked.
*/

bool QCamera::isWhiteBalanceLocked() const
{
    return d_func()->imageControl ? d_func()->imageControl->isWhiteBalanceLocked() : false;
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
    \value FlashTorch           Constant light source, useful for focusing and video capture.
    \value FlashSlowSyncFrontCurtain 
                                Use the flash in conjunction with a slow shutter speed. 
                                This mode allows better exposure of distant objects and/or motion blur effect.
    \value FlashSlowSyncRearCurtain 
                                The similar mode to FlashSlowSyncFrontCurtain but flash is fired at the end of exposure.
    \value FlashManual          Flash power is manualy set.
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
    \enum QCamera::FocusPointMode

    \value FocusPointAuto       Automaticaly select one or multiple focus points.
    \value FocusPointCenter     Focus to the frame center.
    \value FocusPointFaceDetection Focus on faces in the frame.
    \value FocusPointCustom     Focus to the custom point, defined by QCamera::customFocusPoint property.
*/


/*!
    \enum QCamera::FocusStatus

    \value FocusIntial          The initial focus state.
    \value FocusRequested       Focus request is in progress.
    \value FocusCanceled        The focus request was canceled.
    \value FocusReached         Focus has been reached.
    \value FocusLost            Focus has been lost.
    \value FocusUnableToReach   Unable to achieve focus.

    When the QCamera::InfinityFocus or QCamera::HyperfocalFocus mode
    is requested, the focus status changes to QCamera::FocusRequested
    until the requested state is reached
    and stays in the QCamera::FocusReached status after.

    In manual focusing mode the focus stays in the QCamera::FocusReached status,
    or, when supported by camera, QCamera::FocusLost and QCamera::FocusReached values
    could be used for manual focus notifications.

    In autofocus mode, on focus request the status changes to QCamera::FocusRequested,
    and then depending on request results the focus status
    changes to QCamera::FocusReached, QCamera::FocusUnableToReach or QCamera::FocusCanceled.

    In countinous autofocus mode, the status changes to QCamera::FocusRequested,
    and since the focus is reached is stays in QCamera::FocusReached and QCamera::FocusLost states
    until continous focusing is canceled with cancelAutofocus() or setFocusMode().
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


/*!
    \fn void QCamera::focusReached()

    Signals the focus was reached.
    This signal is emited after focusStatus changes to QCamera::FocusReached state.
*/

/*!
    \fn void QCamera::focusUnableToReach()

    Signals the focus was unable to reach.
    This signal is emited after focusStatus changes to QCamera::FocusUnableToReach state.
*/

#include "moc_qcamera.cpp"
QTM_END_NAMESPACE
