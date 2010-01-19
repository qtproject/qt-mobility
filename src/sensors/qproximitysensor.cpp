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

#include <qproximitysensor.h>
#include "qproximitysensor_p.h"

QTM_BEGIN_NAMESPACE

IMPLEMENT_READING(QProximityReading)

/*!
    \class QProximityReading
    \ingroup sensors

    \preliminary
    \brief The QProximityReading class represents one reading from the
           proximity sensor.

    The proximity sensor returns an indication of how far the user is
    away from the device using the pre-defined values found in the
    QProximityReading::Proximity enum.
*/

/*!
    \enum QProximityReading::Proximity

    This enum represents the proximity of the user.

    \value Undefined  The proximity is unknown.
    \value Close      The user is close to the device.
    \value NotClose   The user is not close to the device.
*/

/*!
    \property QProximityReading::proximity
    \brief foo
*/

/*!
    Foo
*/
QProximityReading::Proximity QProximityReading::proximity() const
{
    return static_cast<QProximityReading::Proximity>(d->proximity);
}

/*!
    Foo
*/
void QProximityReading::setProximity(QProximityReading::Proximity proximity)
{
    d->proximity = proximity;
}

// =====================================================================

/*!
    \class QProximitySensor
    \ingroup sensors

    \preliminary
    \brief The QProximitySensor class reports on user proximity.

    The proximity sensor returns an indication of how far the user is
    away from the device using the pre-defined values found in the
    QProximityReading::Proximity enum.

    \sa QProximityReading
*/


#include "moc_qproximitysensor.cpp"
QTM_END_NAMESPACE

