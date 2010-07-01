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

#include <qcameraexposure.h>
#include <qcamera.h>

#include <qmediaobject_p.h>
#include <qcameraexposurecontrol.h>
#include <qmetaobject.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCameraExposure

    \preliminary
    \brief The QCameraExposure class provides interface for exposure related camera settings.

    \inmodule QtMultimediaKit
    \ingroup camera

The Camera API of Qt Mobility is still in \bold Technology Preview. It has
not undergone the same level of review and testing as the rest of the APIs.

The API exposed by the classes in this component are not stable, and will
undergo modification or removal prior to the final release of Qt Mobility.
*/

#define DEBUG_EXPOSURE_CHANGES 1

#ifdef DEBUG_EXPOSURE_CHANGES
#define ENUM_NAME(c,e,v) (c::staticMetaObject.enumerator(c::staticMetaObject.indexOfEnumerator(e)).valueToKey((v)))
#endif


class QCameraExposurePrivate
{
    Q_DECLARE_NON_CONST_PUBLIC(QCameraExposure)
public:
    void initControls();
    QCameraExposure *q_ptr;

    QCamera *camera;
    QCameraExposureControl *exposureControl;

    void _q_exposureParameterChanged(int parameter);
    void _q_exposureParameterRangeChanged(int parameter);
};

void QCameraExposurePrivate::initControls()
{
    Q_Q(QCameraExposure);

    QMediaService *service = camera->service();
    exposureControl = 0;
    if (service)
        exposureControl = qobject_cast<QCameraExposureControl *>(service->requestControl(QCameraExposureControl_iid));

    if (exposureControl) {
        q->connect(exposureControl, SIGNAL(flashReady(bool)), q, SIGNAL(flashReady(bool)));

        q->connect(exposureControl, SIGNAL(exposureParameterChanged(int)),
                   q, SLOT(_q_exposureParameterChanged(int)));
        q->connect(exposureControl, SIGNAL(exposureParameterRangeChanged(int)),
                   q, SLOT(_q_exposureParameterRangeChanged(int)));
    }
}

void QCameraExposurePrivate::_q_exposureParameterChanged(int parameter)
{
    Q_Q(QCameraExposure);

#if DEBUG_EXPOSURE_CHANGES
    qDebug() << "Exposure parameter changed:"
             << ENUM_NAME(QCameraExposureControl, "ExposureParameter", parameter)
             << exposureControl->exposureParameter(QCameraExposureControl::ExposureParameter(parameter));
#endif

    switch (parameter) {
    case QCameraExposureControl::ISO:
        emit q->isoSensitivityChanged(q->isoSensitivity());
        break;
    case QCameraExposureControl::Aperture:
        emit q->apertureChanged(q->aperture());
        break;
    case QCameraExposureControl::ShutterSpeed:
        emit q->shutterSpeedChanged(q->shutterSpeed());
        break;
    }
}

void QCameraExposurePrivate::_q_exposureParameterRangeChanged(int parameter)
{
    Q_Q(QCameraExposure);

    switch (parameter) {
    case QCameraExposureControl::Aperture:
        emit q->apertureRangeChanged();
        break;
    case QCameraExposureControl::ShutterSpeed:
        emit q->shutterSpeedRangeChanged();
        break;
    }
}

/*!
    Construct a QCameraExposure from service \a provider and \a parent.
*/

QCameraExposure::QCameraExposure(QCamera *parent):
    QObject(parent), d_ptr(new QCameraExposurePrivate)
{
    Q_D(QCameraExposure);
    d->camera = parent;
    d->q_ptr = this;
    d->initControls();
}


/*!
    Destroys the camera exposure object.
*/

QCameraExposure::~QCameraExposure()
{
    Q_D(QCameraExposure);
    if (d->exposureControl)
        d->camera->service()->releaseControl(d->exposureControl);
}

/*!
    Returns true if exposure settings are supported by this camera.
*/
bool QCameraExposure::isAvailable() const
{
    return d_func()->exposureControl != 0;
}


/*!
    Returns the flash mode being used.
*/

QCameraExposure::FlashModes QCameraExposure::flashMode() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->flashMode() : QCameraExposure::FlashOff;
}

/*!
    Set the flash mode to \a mode.

    Usually the single QCameraExposure::FlashMode flag is used,
    but some non conflicting flags combination are also allowed,
    like QCameraExposure::FlashManual | QCameraExposure::FlashSlowSyncRearCurtain.
*/

void QCameraExposure::setFlashMode(QCameraExposure::FlashModes mode)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setFlashMode(mode);
}

/*!
    Returns true if the flash \a mode is supported.
*/

bool QCameraExposure::isFlashModeSupported(QCameraExposure::FlashModes mode) const
{
    return d_func()->exposureControl ? d_func()->exposureControl->isFlashModeSupported(mode) : false;
}

/*!
    Returns true if flash is charged.
*/

bool QCameraExposure::isFlashReady() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->isFlashReady() : false;
}


/*!
    Returns the exposure mode being used.
*/

QCameraExposure::ExposureMode QCameraExposure::exposureMode() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->exposureMode() : QCameraExposure::ExposureAuto;
}

/*!
    Set exposure mode to \a mode
*/

void QCameraExposure::setExposureMode(QCameraExposure::ExposureMode mode)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setExposureMode(mode);
}

/*!
    Returns true if the exposure \a mode is supported.
*/

bool QCameraExposure::isExposureModeSupported(QCameraExposure::ExposureMode mode) const
{
    return d_func()->exposureControl ?
            d_func()->exposureControl->isExposureModeSupported(mode) : false;
}

/*!
    Returns the exposure compensation.
*/

qreal QCameraExposure::exposureCompensation() const
{
    if (d_func()->exposureControl)
        return d_func()->exposureControl->exposureParameter(QCameraExposureControl::ExposureCompensation).toReal();
    else
        return 0;
}

/*!
    Sets the exposure compensation to \a ev
*/

void QCameraExposure::setExposureCompensation(qreal ev)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setExposureParameter(QCameraExposureControl::ExposureCompensation, QVariant(ev));
}

/*!
    Returns the metering mode being used.
*/

QCameraExposure::MeteringMode QCameraExposure::meteringMode() const
{
    return d_func()->exposureControl ? d_func()->exposureControl->meteringMode() : QCameraExposure::MeteringMatrix;
}

/*!
    Sets the metering mode to \a mode.
*/

void QCameraExposure::setMeteringMode(QCameraExposure::MeteringMode mode)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setMeteringMode(mode);
}

/*!
    Returns true if the metering \a mode is supported.
*/
bool QCameraExposure::isMeteringModeSupported(QCameraExposure::MeteringMode mode) const
{
    return d_func()->exposureControl ? d_func()->exposureControl->isMeteringModeSupported(mode) : false;
}

/*!
    Returns the ISO sensitivity.
*/

int QCameraExposure::isoSensitivity() const
{
    if (d_func()->exposureControl)
        return d_func()->exposureControl->exposureParameter(QCameraExposureControl::ISO).toInt();

    return -1;
}

/*!
    Returns the list of ISO senitivities camera supports.

    If the camera supports arbitrary ISO sensitivities within the supported range,
    *\a continuous is set to true, otherwise *\a continuous is set to false.
*/
QList<int> QCameraExposure::supportedIsoSensitivities(bool *continuous) const
{
    QList<int> res;
    QCameraExposureControl *control = d_func()->exposureControl;

    if (!control)
        return res;

    foreach (const QVariant &value,
             control->supportedParameterRange(QCameraExposureControl::ISO)) {
        bool ok = false;
        int intValue = value.toInt(&ok);
        if (ok)
            res.append(intValue);
        else
            qWarning() << "Incompatible ISO value type, int is expected";
    }

    if (continuous)
        *continuous = control->exposureParameterFlags(QCameraExposureControl::ISO) &
                      QCameraExposureControl::ContinuousRange;

    return res;
}

/*!
    Sets the manual sensitivity to \a iso
*/

void QCameraExposure::setManualIsoSensitivity(int iso)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setExposureParameter(QCameraExposureControl::ISO, QVariant(iso));
}

/*!
     Turn on auto sensitivity
*/

void QCameraExposure::setAutoIsoSensitivity()
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setExposureParameter(QCameraExposureControl::ISO, QVariant());
}

/*!
    \property QCameraExposure::shutterSpeed
    \brief The effective length of time the shutter is open in seconds.
*/

/*!
    \fn QCameraExposure::shutterSpeedChanged(qreal speed)

    Signals that a camera's shutter \a speed has changed.
*/

/*!
    \property QCameraExposure::isoSensitivity
    \brief The sensor ISO sensitivity.
*/

/*!
    \property QCameraExposure::aperture
    \brief Lens aperture is specified as an F number, the ratio of the focal length to effective aperture diameter.
*/

/*!
    \property QCameraExposure::exposureCompensation
    \brief Camera exposure compensation, in EV.
*/


qreal QCameraExposure::aperture() const
{
    if (d_func()->exposureControl)
        return d_func()->exposureControl->exposureParameter(QCameraExposureControl::Aperture).toReal();

    return -1.0;
}

/*!
    Returns the list of aperture values camera supports.
    The apertures list can change depending on the focal length,
    in such a case the apertureRangeChanged() signal is emited.

    If the camera supports arbitrary aperture values within the supported range,
    *\a continuous is set to true, otherwise *\a continuous is set to false.
*/
QList<qreal> QCameraExposure::supportedApertures(bool * continuous) const
{
    QList<qreal> res;
    QCameraExposureControl *control = d_func()->exposureControl;

    if (!control)
        return res;

    foreach (const QVariant &value,
             control->supportedParameterRange(QCameraExposureControl::Aperture)) {
        bool ok = false;
        qreal realValue = value.toReal(&ok);
        if (ok)
            res.append(realValue);
        else
            qWarning() << "Incompatible aperture value type, qreal is expected";
    }

    if (continuous)
        *continuous = control->exposureParameterFlags(QCameraExposureControl::Aperture) &
                      QCameraExposureControl::ContinuousRange;

    return res;
}

/*!
    Sets the aperture to \a aperture
*/

void QCameraExposure::setManualAperture(qreal aperture)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setExposureParameter(QCameraExposureControl::Aperture, QVariant(aperture));
}

/*!
    Turn on auto aperture
*/

void QCameraExposure::setAutoAperture()
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setExposureParameter(QCameraExposureControl::Aperture, QVariant());
}

/*!
    Return the current shutter speed in seconds.
*/

qreal QCameraExposure::shutterSpeed() const
{
    if (d_func()->exposureControl)
        return d_func()->exposureControl->exposureParameter(QCameraExposureControl::ShutterSpeed).toReal();

    return -1.0;
}

/*!
    Returns the list of shutter speed values in seconds camera supports.

    If the camera supports arbitrary shutter speed values within the supported range,
    *\a continuous is set to true, otherwise *\a continuous is set to false.
*/
QList<qreal> QCameraExposure::supportedShutterSpeeds(bool *continuous) const
{
    QList<qreal> res;

    QCameraExposureControl *control = d_func()->exposureControl;
    if (!control)
        return res;

    foreach (const QVariant &value,
             control->supportedParameterRange(QCameraExposureControl::ShutterSpeed)) {
        bool ok = false;
        qreal realValue = value.toReal(&ok);
        if (ok)
            res.append(realValue);
        else
            qWarning() << "Incompatible shutter speed value type, qreal is expected";
    }

    if (continuous)
        *continuous = control->exposureParameterFlags(QCameraExposureControl::ShutterSpeed) &
                      QCameraExposureControl::ContinuousRange;

    return res;
}

/*!
    Set the shutter speed to \a seconds
*/

void QCameraExposure::setManualShutterSpeed(qreal seconds)
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setExposureParameter(QCameraExposureControl::ShutterSpeed, QVariant(seconds));
}

/*!
    Turn on auto shutter speed
*/

void QCameraExposure::setAutoShutterSpeed()
{
    if (d_func()->exposureControl)
        d_func()->exposureControl->setExposureParameter(QCameraExposureControl::ShutterSpeed, QVariant());
}


/*!
    \enum QCameraExposure::FlashMode

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
    \enum QCameraExposure::ExposureMode

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
    \enum QCameraExposure::MeteringMode

    \value MeteringAverage       Center weighted average metering mode.
    \value MeteringSpot          Spot metering mode.
    \value MeteringMatrix        Matrix metering mode.
*/

/*!
    \fn void QCameraExposure::flashReady(bool ready)

    Signal emitted when flash status changed, flash is ready if \a ready true.
*/

/*!
    \fn void QCameraExposure::apertureChanged(qreal value)

    Signal emitted when aperature changes to \a value.
*/

/*!
    \fn void QCameraExposure::apertureRangeChanged()

    Signal emitted when aperature range has changed.
*/


/*!
    \fn void QCameraExposure::shutterSpeedRangeChanged()

    Signal emitted when the shutter speed range has changed.
*/


/*!
    \fn void QCameraExposure::isoSensitivityChanged(int value)

    Signal emitted when sensitivity changes to \a value.
*/

#include "moc_qcameraexposure.cpp"
QT_END_NAMESPACE
