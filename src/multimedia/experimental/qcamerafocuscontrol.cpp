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

#include <experimental/qcamerafocuscontrol.h>
#include  "qmediacontrol_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QCameraFocusControl
    \ingroup multimedia

    \preliminary
    \brief The QCameraFocusControl class supplies control for
    focusing related camera parameters.

    The interface name of QCameraFocusControl is \c com.nokia.Qt.QCameraFocusControl/1.0 as
    defined in QCameraFocusControl_iid.

    \sa QMediaService::control(), QCamera
*/

/*!
    \macro QCameraFocusControl_iid

    \c com.nokia.Qt.QCameraFocusControl/1.0

    Defines the interface name of the QCameraFocusControl class.

    \relates QCameraFocusControl
*/

/*!
    Constructs a camera control object with \a parent.
*/

QCameraFocusControl::QCameraFocusControl(QObject *parent):
    QMediaControl(*new QMediaControlPrivate, parent)
{
}

/*!
    Destruct the camera control object.
*/

QCameraFocusControl::~QCameraFocusControl()
{
}


/*!
  \fn void QCameraFocusControl::lockFocus()

  Lock the focus.
*/

/*!
  \fn void QCameraFocusControl::unlockFocus()
  Unlock the focus.
*/

/*!
  \fn QCamera::FocusMode QCameraFocusControl::focusMode() const

  Returns the focus mode being used.
*/


/*!
  \fn void QCameraFocusControl::setFocusMode(QCamera::FocusMode mode)

  Set the focus mode to \a mode.
*/


/*!
  \fn QCamera::FocusModes QCameraFocusControl::supportedFocusModes() const

  Returns focus modes available.
*/


/*!
  \fn QCamera::FocusStatus QCameraFocusControl::focusStatus() const

  Returns the focus status.
*/


/*!
  \fn bool QCameraFocusControl::macroFocusingEnabled() const

  Returns true if macro focusing enabled.
*/


/*!
  \fn bool QCameraFocusControl::isMacroFocusingSupported() const

  Returns true if macro focusing is available.
*/


/*!
  \fn void QCameraFocusControl::setMacroFocusingEnabled(bool enable)

  Set macro focusing to \a enable
*/



/*!
  \fn qreal QCameraFocusControl::maximumOpticalZoom() const

  Returns the maximum optical zoom
*/


/*!
  \fn qreal QCameraFocusControl::maximumDigitalZoom() const

  Return the maximum digital zoom
*/


/*!
  \fn qreal QCameraFocusControl::zoomValue() const

  Return the zoom value.
*/


/*!
  \fn void QCameraFocusControl::zoomTo(qreal value)

  Set the zoom value to \a value
*/


/*!
  \fn bool QCameraFocusControl::isFocusLocked() const

  Return true if focus locked.
*/

/*!
    \fn void QCameraFocusControl::focusLocked()

    Signal emitted when focus is locked.
*/

/*!
    \fn void QCameraFocusControl::focusStatusChanged(QCamera::FocusStatus status)

    Signal emitted when focus \a status changed.
*/


/*!
    \fn void QCameraFocusControl::zoomValueChanged(qreal zoom)

    Signal emitted when \a zoom value changed.
*/

#include "moc_qcamerafocuscontrol.cpp"
QTM_END_NAMESPACE

