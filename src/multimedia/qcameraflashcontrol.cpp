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

#include <qcameraflashcontrol.h>
#include  "qmediacontrol_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCameraFlashControl

    \preliminary
    \brief The QCameraFlashControl class supplies a camera flash control.

    \ingroup camera

    The interface name of QCameraFlashControl is \c com.nokia.Qt.QCameraFlashControl/1.0 as
    defined in QCameraFlashControl_iid.

The Camera API of Qt Mobility is still in \bold Technology Preview. It has not undergone
the same level of review and testing as the rest of the APIs.

The API exposed by the classes in this component are not stable, and will
undergo modification or removal prior to the final release of Qt Mobility.

    \sa QCamera
*/

/*!
    \macro QCameraFlashControl_iid

    \c com.nokia.Qt.QCameraFlashControl/1.0

    Defines the interface name of the QCameraFlashControl class.

    \relates QCameraFlashControl
*/

/*!
    Constructs a camera flash control object with \a parent.
*/

QCameraFlashControl::QCameraFlashControl(QObject *parent):
    QMediaControl(*new QMediaControlPrivate, parent)
{
}

/*!
    Destruct the camera control object.
*/

QCameraFlashControl::~QCameraFlashControl()
{
}

/*!
  \fn QCamera::FlashModes QCameraFlashControl::flashMode() const

  Returns the current flash mode.
*/


/*!
  \fn void QCameraFlashControl::setFlashMode(QCameraExposure::FlashModes mode)

  Set the current flash \a mode.

  Usually the single QCameraExposure::FlashMode flag is used,
  but some non conflicting flags combination are also allowed,
  like QCameraExposure::FlashManual | QCameraExposure::FlashSlowSyncRearCurtain.
*/


/*!
  \fn QCameraFlashControl::isFlashModeSupported(QCameraExposure::FlashModes mode) const

  Return true if reqested flash \a mode is supported.
  Some QCameraExposure::FlashMode values can be combined,
  for example QCameraExposure::FlashManual | QCameraExposure::FlashSlowSyncRearCurtain
*/

/*!
  \fn bool QCameraFlashControl::isFlashReady() const

  Returns true if flash is charged.
*/

/*!
    \fn void QCameraFlashControl::flashReady(bool ready)

    Signal emitted when flash state changes to \a ready.
*/


#include "moc_qcameraflashcontrol.cpp"
QT_END_NAMESPACE

