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
        connect(d->control,SIGNAL(stateChanged(QCamera::State)),this,SIGNAL(stateChanged(QCamera::State)));
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
    return QList<QByteArray>();
}

QString QCamera::deviceDescription(const QByteArray &device)
{
    Q_UNUSED(device);
    return QString();
}

QCamera::State QCamera::state() const
{
    if(d_func()->control)
        return d_func()->control->state();

    return QCamera::StoppedState;
}

bool QCamera::isValid() const
{
    return d_func()->control != NULL;
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

