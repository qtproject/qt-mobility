/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qdeclarativefeedbackactuator_p.h"
/*!
    \qmlclass Actuator QFeedbackActuator
    \brief The Actuator element represents a feedback actuator.
    \ingroup qml-feedback-api

    This element is part of the \bold{QtMobility.feedback 1.1} module.

    The Actuator class maps directly to the QFeedbackActuator C++ class, and
    can be used with HapticsEffect elements.

    There are several predefined enumerations and constants provided in this object:

    1. Capability
    \list
    \o Envelope - Value specifying that the device can bound its intensity by an Envelope.
    \o Period  - Value specifying that the device can play periodic effects.
    \endlist

    2. State
    \list
    \o Busy    - The actuator is busy.
    \o Ready   - The actuator is ready to play an effect.
    \o Unknown - The actuator is in an unknown state.
    \endlist

    \sa HapticsEffect, {QFeedbackActuator}
*/
QDeclarativeFeedbackActuator::QDeclarativeFeedbackActuator(QObject *parent)
    :QObject(parent)
{
    d = new QFeedbackActuator(this);
    connect(d, SIGNAL(enabledChanged()), this, SIGNAL(enabledChanged()));
}

QDeclarativeFeedbackActuator::QDeclarativeFeedbackActuator(QObject *parent, QFeedbackActuator* actuator)
    :QObject(parent)
{
    d = actuator;
    connect(d, SIGNAL(enabledChanged()), this, SIGNAL(enabledChanged()));
}

QFeedbackActuator* QDeclarativeFeedbackActuator::feedbackActuator() const
{
    return d;
}

/*!
    \qmlproperty int Actuator::actuatorId
    This property holds the id of the feedback actuator.
    This property is read only.
*/
int QDeclarativeFeedbackActuator::actuatorId() const
{
    return d->id();
}

/*!
    \qmlproperty bool Actuator::valid

    This property is true if the actuator is valid.
    This property is read only.
*/
bool QDeclarativeFeedbackActuator::isValid() const
{
    return d->isValid();
}
/*!
   \qmlproperty  string Actuator::name
    This property holds the name of the feedback actuator.
    This property is read only.
*/
QString QDeclarativeFeedbackActuator::name() const
{
    return d->name();
}

/*!
    \qmlproperty enumeration Actuator::state
    This property holds the state of the feedback actuator.
    This property is read only.
*/
QFeedbackActuator::State QDeclarativeFeedbackActuator::state() const
{
    return d->state();
}

/*!
    \qmlmethod bool Actuator::isCapabilitySupported(enumeration capability)
    Returns if the actuator supports the supplied \a capability, available capabilities are:
    \list
    \o Envelope - Value specifying that the device can bound its intensity by an Envelope.
    \o Period   - Value specifying that the device can play periodic effects.
    \endlist
*/
bool QDeclarativeFeedbackActuator::isCapabilitySupported(QFeedbackActuator::Capability capbility) const
{
    return d->isCapabilitySupported(capbility);
}
/*!
    \qmlproperty bool Actuator::enabled
    This property is true if the feedback actuator is enabled.
*/

bool QDeclarativeFeedbackActuator::isEnabled() const
{
    return d->isEnabled();
}
void QDeclarativeFeedbackActuator::setEnabled(bool v)
{
    d->setEnabled(v);
}
