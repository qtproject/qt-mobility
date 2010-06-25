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

#include <qcameracontrol.h>
#include  "qmediacontrol_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCameraControl

    \preliminary

    \brief The QCameraControl class is an abstract base class for
    classes that control still cameras or video cameras.

    \ingroup camera

    This service is provided by a QMediaService object via
    QMediaService::control().  It is used by QCamera.

    The interface name of QCameraControl is \c com.nokia.Qt.QCameraControl/1.0 as
    defined in QCameraControl_iid.


The Camera API of Qt Mobility is still in \bold Technology Preview. It has
not undergone the same level of review and testing as the rest of the APIs.

The API exposed by the classes in this component are not stable, and will
undergo modification or removal prior to the final release of Qt Mobility.

    \sa QMediaService::requestControl(), QCamera
*/

/*!
    \macro QCameraControl_iid

    \c com.nokia.Qt.QCameraControl/1.0

    Defines the interface name of the QCameraControl class.

    \relates QCameraControl
*/

/*!
    Constructs a camera control object with \a parent.
*/

QCameraControl::QCameraControl(QObject *parent):
    QMediaControl(*new QMediaControlPrivate, parent)
{
}

/*!
    Destruct the camera control object.
*/

QCameraControl::~QCameraControl()
{
}

/*!
    \fn QCameraControl::state() const

    Returns the state of the camera service.

    \sa QCamera::state
*/

/*!
    \fn QCameraControl::setState(QCamera::State state)

    Sets the camera \a state.
    This operation is asynchronous, with state chages notified with stateChanged() signal.

    \sa QCamera::State
*/

/*!
    \fn void QCameraControl::stateChanged(QCamera::State state)

    Signal emitted when the camera \a state changes.
*/

/*!
    \fn void QCameraControl::error(int error, const QString &errorString)

    Signal emitted when an error occurs with error code \a error and
    a description of the error \a errorString.
*/

/*!
    \fn Camera::CaptureMode QCameraControl::captureMode() const = 0

    Returns the current capture mode.
*/

/*!
    \fn void QCameraControl::setCaptureMode(QCamera::CaptureMode mode) = 0;

    Sets the current capture \a mode.

    Setting the capture mode also triggers asynchronous camera state change to QCamera::Idle,
    to allow capture settings configuration before the camera is activated.
*/

/*!
    \fn bool QCameraControl::isCaptureModeSupported(QCamera::CaptureMode mode) const = 0;

    Returns true if the capture \a mode is suported.
    QCamera::CaptureDisabled mode is always considered supported and not checked by backend.

    Backend should return supported modes even in Stopped state.
*/

/*!
    \fn QCameraControl::captureModeChanged(QCamera::CaptureMode mode)

    Signal emitted when the camera capture \a mode changes.
 */

#include "moc_qcameracontrol.cpp"
QT_END_NAMESPACE
