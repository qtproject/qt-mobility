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
    QCameraService* service;
    QCameraControl* control;
};

/*!
    Construct a QCamera from \a service with \a parent.
*/

QCamera::QCamera(QCameraService *service, QObject *parent)
    : QAbstractMediaObject(*new QCameraPrivate, parent)
{
    Q_D(QCamera);

    if(service) {
        d->service = service;
        d->control = qobject_cast<QCameraControl *>(service->control("camera"));
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
#ifdef VIDEOSERVICES

QList<QVideoFrame::Type> QCamera::supportedColorFormats()
{
    Q_D(QCamera);

    if(d->control)
        return d->control->supportedColorFormats();
    else {
        QList<QVideoFrame::Type> list;
        return list;
    }
}

QList<QSize> QCamera::supportedResolutions(QVideoFrame::Type fmt)
{
    Q_D(QCamera);

    if(d->control)
        return d->control->supportedResolutions(fmt);
    else {
        QList<QSize> sizes;
        return sizes;
    }
}

QVideoFormat QCamera::format() const
{
    if(d_func()->control)
        return d_func()->control->format();

    return QVideoFormat();
}

void QCamera::setFormat(const QVideoFormat &format)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setFormat(format);
}

void QCamera::start()
{
    Q_D(QCamera);

    if(d->control)
        d->control->start();
}

void QCamera::stop()
{
    Q_D(QCamera);

    if(d->control)
        d->control->stop();
}

/*!
    Returns the current framerate

    Return value of -1 indicates not supported
*/

int QCamera::framerate() const
{
    if(d_func()->control)
        return d_func()->control->framerate();

    return 0;
}

/*!
    Sets the framrerate

    Note: If supported, the device will set the framerate to the closest value of \a value
*/

void QCamera::setFrameRate(int rate)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setFrameRate(rate);
}

/*!
    Returns the brightness level

    Return value of -1 indicates not supported.
*/

int QCamera::brightness() const
{
    if(d_func()->control)
        return d_func()->control->brightness();

    return 0;
}

/*!
    Sets the brightness to \a value
    note: ranges 0-100
*/

void QCamera::setBrightness(int b)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setBrightness(b);
}

/*!
    Returns the contrast

    Return of -1 indicates this is not supported
*/

int QCamera::contrast() const
{
    if(d_func()->control)
        return d_func()->control->contrast();

    return 0;
}

/*
    Sets the contrast to \a value

    Note: ranges 0-100
*/

void QCamera::setContrast(int c)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setContrast(c);
}

/*!
    Returns the saturation

    Return value of -1 indicates that this is not supported
*/

int QCamera::saturation() const
{
    if(d_func()->control)
        return d_func()->control->saturation();

    return 0;
}

/*!
    Sets the saturation to \a value

    Note: ranges 0-100
*/

void QCamera::setSaturation(int s)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setSaturation(s);
}

/*!
    Sets the hue

    note: ranges 0-100
*/

int QCamera::hue() const
{
    if(d_func()->control)
        return d_func()->control->hue();

    return 0;
}

/*!
    Returns  current hue

    Return value of -1 indicates that this is not supported
*/

void QCamera::setHue(int h)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setHue(h);
}

/*!
    Returns current sharpness

    Return value of -1 indicates that this is not supported
*/

int QCamera::sharpness() const
{
    if(d_func()->control)
        return d_func()->control->sharpness();

    return 0;
}

/*!
    Sets current sharpness to \a value

    Note: ranges 01-00
*/

void QCamera::setSharpness(int s)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setSharpness(s);
}

/*!
    Returns the zoom level

    Return of -1 indicates not supported
*/

int QCamera::zoom() const
{
    if(d_func()->control)
        return d_func()->control->zoom();

    return 0;
}

/*!
    Sets the zoom

    Note: ranges 0-100
*/

void QCamera::setZoom(int z)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setZoom(z);
}

/*!
    Return true if backlight compensation is on
    false otherwise
*/

bool QCamera::backlightCompensation() const
{
    if(d_func()->control)
        return d_func()->control->backlightCompensation();

    return false;
}

/*!
    Enables/disables backlight compensation with \a value
*/

void QCamera::setBacklightCompensation(bool b)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setBacklightCompensation(b);
}

/*
    Returns the whitelevel

    Return -1 indicates not supported
*/

int QCamera::whitelevel() const
{
    if(d_func()->control)
        return d_func()->control->whitelevel();

    return 0;
}

/*!
    Set the white level

    note: ranges 0-100
*/

void QCamera::setWhitelevel(int w)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setWhitelevel(w);
}

/*!
    Returns the current rotation

    Returns -1 if not impleneted
*/

int QCamera::rotation() const
{
    if(d_func()->control)
        return d_func()->control->rotation();

    return 0;
}

/*!
    Sets the rotation to \a value
*/

void QCamera::setRotation(int r)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setRotation(r);
}

/*!
    Returns true if flash is active false otherwise
    Returns false if not implemented
*/

bool QCamera::flash() const
{
    if(d_func()->control)
        return d_func()->control->flash();

    return 0;
}

/*!
    Set \a value to true to enable flash
    Set \a value to false to disable
*/

void QCamera::setFlash(bool f)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setFlash(f);
}

/*!
    Returns true if auto focus is active false otherwise
    Returns false if not implemented
*/

bool QCamera::autofocus() const
{
    if(d_func()->control)
        return d_func()->control->autofocus();

    return false;
}

/*!
    Set \a value to true to enable auto focus
    set \a value to false to disable
*/

void QCamera::setAutofocus(bool f)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setAutofocus(f);
}

/*!
    Set the camera device to \a device.
*/

void QCamera::setDevice(const QByteArray &device)
{
    Q_D(QCamera);

    if(d->control)
        d->control->setDevice(device);
}
#endif
bool QCamera::isValid() const
{
    if(d_func()->control)
        return d_func()->control->isValid();

    return false;
}

/*!
    Returns the session object being controlled by this recorder.
*/

QAbstractMediaService *QCamera::service() const
{
    return d_func()->service;
}

QCameraService* createCameraService(QMediaServiceProvider *provider)
{
    QObject *object = provider ? provider->createObject("com.nokia.qt.Camera/1.0") : 0;

    if (object) {
        QCameraService *service = qobject_cast<QCameraService *>(object);

        if (service)
            return service;

        delete service;
    }
    return 0;
}

