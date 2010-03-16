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

    \preliminary
    \brief The QCameraFocusControl class supplies control for
    focusing related camera parameters.

    \ingroup camera

    The interface name of QCameraFocusControl is \c com.nokia.Qt.QCameraFocusControl/1.0 as
    defined in QCameraFocusControl_iid.

The Camera API of Qt Mobility is still in \bold Technology Preview. It has not undergone
the same level of review and testing as the rest of the APIs.

The API exposed by the classes in this component are not stable, and will
undergo modification or removal prior to the final release of Qt Mobility.

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
  \fn void QCameraFocusControl::startFocusing()

    Starts single or continuous autofocus.

    Does nothing in hyperfocal or infinity focus modes.

    If supported by camera, startFocusing() turns on the manual focusing notifications,
    otherwise it does nothing in manual mode.
*/

/*!
  \fn void QCameraFocusControl::cancelFocusing()

  Cancels the single autofocus request or stops continuous focusing.

  Does nothing in hyperfocal or infinity focus modes.

  If supported by camera, startFocusing() turns off the manual focusing notifications,
  otherwise it does nothing in manual mode.
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

  Returns the maximum optical zoom value, or 1.0 if optical zoom is not supported.
*/


/*!
  \fn qreal QCameraFocusControl::maximumDigitalZoom() const

  Returns the maximum digital zoom value, or 1.0 if digital zoom is not supported.
*/


/*!
  \fn qreal QCameraFocusControl::opticalZoom() const

  Return the current optical zoom value.
*/

/*!
  \fn qreal QCameraFocusControl::digitalZoom() const

  Return the current digital zoom value.
*/


/*!
  \fn void QCameraFocusControl::zoomTo(qreal opticalZoom, qreal digitalZoom)

  Set the zoom value to \a value
*/

/*!
  \fn QCameraFocusControl::focusPointMode() const

  Returns the camera focus point selection mode.
*/

/*!
  \fn QCameraFocusControl::setFocusPointMode(QCamera::FocusPointMode mode)

  Sets the camera focus point selection mode.
*/

/*!
  \fn QCameraFocusControl::supportedFocusPointModes() const

  Returns supported focus selection modes.
*/

/*!
  \fn QCameraFocusControl::customFocusPoint() const

  Return the position of custom focus point, in relative frame coordinates:
  QPointF(0,0) points to the left top frame point, QPointF(0.5,0.5) points to the frame center.

  Custom focus point is used only in FocusPointCustom focus mode.
*/

/*!
  \fn QCameraFocusControl::setCustomFocusPoint(const QPointF &point)

  Sets the custom focus point.

  If camera supports fixed set of focus points,
  it should use the nearest supported focus point,
  and return the actual focus point with QCameraFocusControl::focusZones().

  \sa QCameraFocusControl::customFocusPoint(), QCameraFocusControl::focusZones()
*/

/*!
  \fn QCameraFocusControl::focusZones() const

  Returns the list of zones, the camera is actually focused on.

  \sa QCameraFocusControl::focusZonesChanged
*/

/*!
    \fn void QCameraFocusControl::focusStatusChanged(QCamera::FocusStatus status)

    Signal emitted when focus \a status changed.
*/


/*!
    \fn void QCameraFocusControl::opticalZoomChanged(qreal zoom)

    Signal emitted when the optical \a zoom value changed.
*/

/*!
    \fn void QCameraFocusControl::digitalZoomChanged(qreal zoom)

    Signal emitted when the digital \a zoom value changed.
*/

/*!
  \fn QCameraFocusControl::focusZonesChanged(const QList<QRectF> &)

  Signal is emited when the set of zones, camera focused on is changed.

  Usually the zones list is changed when the camera is focused.
*/



#include "moc_qcamerafocuscontrol.cpp"
QTM_END_NAMESPACE

