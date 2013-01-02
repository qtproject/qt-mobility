/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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
#include "qdeclarativefeedbackactuator_p.h"
/*!
    \qmlclass Actuator QFeedbackActuator
    \brief The Actuator element represents a feedback actuator.
    \ingroup qml-feedback-api
    \since Mobility 1.1

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
    \since Mobility 1.1
*/
int QDeclarativeFeedbackActuator::actuatorId() const
{
    return d->id();
}

/*!
    \qmlproperty bool Actuator::valid

    This property is true if the actuator is valid.
    This property is read only.
    \since Mobility 1.1
*/
bool QDeclarativeFeedbackActuator::isValid() const
{
    return d->isValid();
}
/*!
   \qmlproperty  string Actuator::name
    This property holds the name of the feedback actuator.
    This property is read only.
    \since Mobility 1.1
*/
QString QDeclarativeFeedbackActuator::name() const
{
    return d->name();
}

/*!
    \qmlproperty enumeration Actuator::state
    This property holds the state of the feedback actuator.
    This property is read only.
    \since Mobility 1.1
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
    \since Mobility 1.1
*/
bool QDeclarativeFeedbackActuator::isCapabilitySupported(QFeedbackActuator::Capability capbility) const
{
    return d->isCapabilitySupported(capbility);
}
/*!
    \qmlproperty bool Actuator::enabled
    This property is true if the feedback actuator is enabled.
    \since Mobility 1.1
*/

bool QDeclarativeFeedbackActuator::isEnabled() const
{
    return d->isEnabled();
}
void QDeclarativeFeedbackActuator::setEnabled(bool v)
{
    d->setEnabled(v);
}
