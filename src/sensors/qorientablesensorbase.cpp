/****************************************************************************
**
** Copyright (C) 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qorientablesensorbase.h"
#include <QObject>

QTM_BEGIN_NAMESPACE

class QOrientableSensorBasePrivate
{
public:
    QOrientableSensorBasePrivate()
        : axesOrientationMode(QOrientableSensorBase::FixedOrientation),
          currentOrientation(0),
          userOrientation(0)
    {
    }

    QOrientableSensorBase::AxesOrientationMode axesOrientationMode;
    int currentOrientation;
    int userOrientation;
};

/*!
    \class QOrientableSensorBase
    \ingroup sensors_main
    \inmodule QtSensors

    \brief The QOrientableSensorBase class is the base class for all sensors that react to screen
           orientation changes.

    All sensors based on this class support changing the reporting of the reading values based
    on the orientation of the screen. This is controlled by the axesOrientationMode property, which
    controls how the orientation affects the reading values.

    In the default mode, QOrientableSensorBase::FixedOrientation, the reading values remain
    unaffected by the orientation. In the QOrientableSensorBase::AutomaticOrientation mode, the reading
    values are automatically rotated by taking the current screen orientation into account. And
    finally, in the QOrientableSensorBase::UserOrientation mode, the reading values are rotated
    according to a user-specified orientation.

    The functionality of this is only available if it is supported by the backend, which can be
    checked by calling QSensor::isFeatureSupported() with the QSensor::AxesOrientation flag.

    The orientation here is always the screen orientation, not the device orientation. The orientation
    values range from 0 to 270 degrees. The orientation is applied in clockwise direction.
    Note that when rotating a device by 90 degrees counter-clockwise, the screen orientation compensates for
    that by rotating 90 degrees to the right.
*/

/*!
    \enum QOrientableSensorBase::AxesOrientationMode

    Describes how reading values are affected by the screen orientation.

    \value FixedOrientation No automatic rotation is applied to the reading values.

    \value AutomaticOrientation The reading values are automatically rotated based on the screen
                                orientation.

    \value UserOrientation The reading values are rotated based on the angle of the userOrientation property.

    \sa QOrientableSensorBase::axesOrientationMode
*/

/*!
   Constructs a QOrientableSensorBase.
 */
QOrientableSensorBase::QOrientableSensorBase()
    : d(new QOrientableSensorBasePrivate)
{
}

/*!
   Destroys a QOrientableSensorBase.
 */
QOrientableSensorBase::~QOrientableSensorBase()
{
    delete d;
    d = 0;
}

/*!
    \property QOrientableSensorBase::axesOrientationMode
    \brief The mode that affects how the screen orientation changes reading values.

    When set to FixedOrientation, which is the default mode, no automatic rotation is applied to
    the reading. This is the only mode available for backends that do not support the
    QSensor::AxesOrientation feature.

    When set to AutomaticOrientation, the reading values are automatically rotated when the
    screen orientation changes. In effect, the screen orientation is canceled out.

    As an example, assume the device is rotated by 180 degrees and therefore the screen orientation
    also is rotated by 180 degrees from the native orientation. Without automatic axes orientation,
    the reading values would now be changed: Both the X and the Y values would be negated, forcing
    an application developer to manually cancel out the negation in application code. Automatic
    axes orientation does this automatically, in this mode the X and Y values would be the same as
    with the default screen orientation.

    This automatic rotation of the axes is handy is some usecases, for example in a bubble level
    application that measures how level a surface is by looking at the X axis value of an
    accelerometer. When the device and screen orientation change by 90 degrees, an application
    developer does not need to change anything, he can continue using the X axis value even though
    the device is rotated. Without automatic axes orientation, the application developer would need
    to look at the Y values instead, thereby adding code to the application that reads from a
    different axis depending on the screen orientation.

    The UserOrientation mode is quite similar to AutomaticOrientation, only that the screen orientation
    is manually controlled instead of automatically determined. The angle of the userOrientation
    property is then used for rotating the reading values.

    Since the rotation of the reading values is based on the screen orientation, Z values will never
    change, as the Z axis is perpendicular to the screen.
    As screen orientation changes in 90 degree steps, rotating the reading values is also done in
    steps of 90 degrees.
*/

void QOrientableSensorBase::setAxesOrientationMode(QOrientableSensorBase::AxesOrientationMode axesOrientationMode)
{
    if (d->axesOrientationMode != axesOrientationMode) {
        d->axesOrientationMode = axesOrientationMode;
        emit axesOrientationModeChanged(axesOrientationMode);
    }
}

QOrientableSensorBase::AxesOrientationMode QOrientableSensorBase::axesOrientationMode() const
{
    return d->axesOrientationMode;
}

/*!
    \property QOrientableSensorBase::currentOrientation
    \brief The current orientation that is used for rotating the reading values.

    This might not be the same as the screen orientation. For example, in the FixedOrientation mode,
    the reading values are not rotated, and therefore the property is 0.

    In the UserOrientation mode, the readings are rotated based on the userOrientation property,
    and therefore this property is equal to the userOrientation property.

    In the AutomaticOrientation mode, the readings are rotated based on the screen orientation,
    and therefore this property will be equal to the current screen orientation.

    This property is set by the backend.
*/

int QOrientableSensorBase::currentOrientation() const
{
    return d->currentOrientation;
}

/*!
   Sets the current screen orientation to \a currentOrientation. This is to be called from the
   backend whenever the screen orientation or the userOrientation property changes.
 */
void QOrientableSensorBase::setCurrentOrientation(int currentOrientation)
{
    if (d->currentOrientation != currentOrientation) {
        d->currentOrientation = currentOrientation;
        emit currentOrientationChanged(currentOrientation);
    }
}

/*!
    \property QOrientableSensorBase::userOrientation
    \brief The angle used for rotating the reading values in the UserOrientation mode.

    When the axesOrientationMode property is set to UserOrientation, the angle for rotating the
    reading values is taken from this property. In other modes, the property has no effect.

    The default is 0. The only valid values are 0, 90, 180 and 270, as those are the only possible
    screen orientations.
*/

void QOrientableSensorBase::setUserOrientation(int userOrientation)
{
    if (d->userOrientation != userOrientation) {
        d->userOrientation = userOrientation;
        emit userOrientationChanged(userOrientation);
    }
}

int QOrientableSensorBase::userOrientation() const
{
    return d->userOrientation;
}

QTM_END_NAMESPACE
