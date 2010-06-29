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

#include <qcameralockscontrol.h>
#include  "qmediacontrol_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCameraLocksControl

    \preliminary

    \brief The QCameraLocksControl class is an abstract base class for
    classes that control still cameras or video cameras.

    \ingroup camera

    This service is provided by a QMediaService object via
    QMediaService::control().  It is used by QCamera.

    The interface name of QCameraLocksControl is \c com.nokia.Qt.QCameraLocksControl/1.0 as
    defined in QCameraLocksControl_iid.


The Camera API of Qt Mobility is still in \bold Technology Preview. It has
not undergone the same level of review and testing as the rest of the APIs.

The API exposed by the classes in this component are not stable, and will
undergo modification or removal prior to the final release of Qt Mobility.

    \sa QMediaService::requestControl(), QCamera
*/

/*!
    \macro QCameraLocksControl_iid

    \c com.nokia.Qt.QCameraLocksControl/1.0

    Defines the interface name of the QCameraLocksControl class.

    \relates QCameraLocksControl
*/

/*!
    Constructs a camera locks control object with \a parent.
*/

QCameraLocksControl::QCameraLocksControl(QObject *parent):
    QMediaControl(*new QMediaControlPrivate, parent)
{
}

/*!
    Destruct the camera locks control object.
*/

QCameraLocksControl::~QCameraLocksControl()
{
}

/*!
    \fn QCameraLocksControl::supportedLocks() const

    Returns the lock types, the camera supports.
*/

/*!
    \fn QCameraLocksControl::lockStatus(QCamera::LockType lock) const

    Returns the camera \a lock status.

    \sa QCamera::lockStatus(QCamera::LockType)
*/

/*!
    \fn QCameraLocksControl::searchAndLock(QCamera::LockTypes locks)

    Request camera \a locks.

    \sa QCamera::searchAndLock(QCamera::LockTypes)
*/

/*!
    \fn QCameraLocksControl::unlock(QCamera::LockTypes locks)

    Unlock camera \a locks.

    \sa QCamera::unlock(QCamera::LockTypes)
*/

/*!
    \fn QCameraLocksControl::lockStatusChanged(QCamera::LockType lock, QCamera::LockStatus status, QCamera::LockChangeReason reason)

    Signals the \a lock \a status was changed with a specified \a reason.

    \sa QCamera::lockStatusChanged
*/



#include "moc_qcameralockscontrol.cpp"
QT_END_NAMESPACE
