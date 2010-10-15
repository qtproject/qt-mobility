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

#include "qfeedbackactuator.h"
#include "qfeedbackplugininterfaces.h"

#include <QtCore/QVariant>

QTM_BEGIN_NAMESPACE

/*!
    \class QFeedbackActuator
    \brief The QFeedbackActuator class is describing actuators for tactile feedback.
    \inmodule QtFeedback

    An actuator knows how to play a \l{QFeedbackEffect}{tactile
    effect}.The class gives access to a specified actuator. 

    An actuator can be used to play \l{QFeedbackHapticsEffect}s using
    \l{QFeedbackHapticsEffect::}{setActuator()}. Usually, you will not
    have to set an actuator directly on a QFeedbackHapticsEffect.
    QFeedbackHapticsEffect and QFeedbackFileEffect uses an appropriate
    actuator by default. However, you can query which actuators
    are available with actuators().

    \code
     QFeedbackActuator actuator; // default system actuator
     QList<QFeedbackActuator> actuators = QFeedbackActuator::actuators();
     foreach (const QFeedbackActuator& temp, actuators) {
         if (temp.name() == "ExampleActuatorName") {
             actuator = temp;
         }
     }
    \endcode

    The QFeedbackActuator class gives access to information about the
    actuator it represents. You can query if the actuator isEnabled()
    and if it isValid(). Whether an actuator is ready to play an
    effect can be queried by checking the actuator's state(). The
    \l{QFeedbackActuator::}{State} enum describes the states and
    actuator can be in, which is either Ready, Busy, or Unknown. You
    can also get a human readable name for the actuator with the
    name() function.

    \sa QFeedbackHapticsEffect QFeedbackFileEffect QFeedbackEffect
*/

/*!
    \enum QFeedbackActuator::Capability

    \value Envelope Capability defining the wave type with attack/fade times and levels.
    \value Period   Capability defining that the device can play periodic effects.
*/

/*!
    \enum QFeedbackActuator::State

    \value Busy    The actuator is busy.
    \value Ready   The actuator is ready to play an effect.
    \value Unknown The actuator is in an known state.
*/


/*!
    Constructs a QFeedbackActuator. It will represent the default actuator on the system.

    If there are no actuators attached to the system, isValid() will return false.

    \sa isValid()
*/
QFeedbackActuator::QFeedbackActuator(QObject *parent) : QObject(parent), m_id(-1)
{
    QList<QFeedbackActuator*> list = actuators();
    if  (!list.isEmpty()) {
        QFeedbackActuator* defaultActuator = list.first();
        m_id = defaultActuator->id();
    }
}

/*!
    Constructs a QFeedbackActuator with id id.

    This will be used by plugins to represents their actuators.

    \sa isValid()
*/
QFeedbackActuator::QFeedbackActuator(QObject *parent, int id) : QObject(parent), m_id(id)
{
}


/*!
    \fn QFeedbackActuator::id() const

    returns the id of the actuator
*/
int QFeedbackActuator::id() const
{
    return m_id;
}

/*!
    \fn QFeedbackActuator::isValid() const

    returns true if the actuator is valid.
*/
bool QFeedbackActuator::isValid() const
{
    return m_id >= 0;
}

/*!
    \fn QFeedbackActuator::name() const

    returns the name of the actuator.
*/
QString QFeedbackActuator::name() const
{
    return QFeedbackHapticsInterface::instance()->actuatorProperty(*this, QFeedbackHapticsInterface::Name).toString();
}

/*!
    \fn QFeedbackActuator::state() const

    returns the state of the actuator.
*/
QFeedbackActuator::State QFeedbackActuator::state() const
{
    return QFeedbackActuator::State(QFeedbackHapticsInterface::instance()->actuatorProperty(*this, QFeedbackHapticsInterface::State).toInt());
}

/*!
    \fn bool QFeedbackActuator::isCapabilitySupported(Capability capability) const

    returns if the actuator supports the supplied \a capability.
*/
bool QFeedbackActuator::isCapabilitySupported(Capability capability) const
{
    return QFeedbackHapticsInterface::instance()->isActuatorCapabilitySupported(*this, capability);
}

/*!
    \fn  QFeedbackActuator::isEnabled() const

    returns true if you can use this actuator to start effects.
*/
bool QFeedbackActuator::isEnabled() const
{
    return QFeedbackHapticsInterface::instance()->actuatorProperty(*this, QFeedbackHapticsInterface::Enabled).toBool();
}

/*!
    \fn  QFeedbackActuator::setEnabled(bool enabled)

    Allows to enable or disable a actuator to \a enabled.
*/
void QFeedbackActuator::setEnabled(bool enabled)
{
    if (isEnabled() != enabled) {
        QFeedbackHapticsInterface::instance()->setActuatorProperty(*this, QFeedbackHapticsInterface::Enabled, enabled);
        emit enabledChanged();
    }
}

/*!
    \fn QFeedbackActuator::actuators()

    returns the list of actuators available on the system.
*/
QList<QFeedbackActuator*> QFeedbackActuator::actuators()
{
    return QFeedbackHapticsInterface::instance()->actuators();
}

/*!
    \fn QFeedbackActuator::operator==(const QFeedbackActuator &other) const

    Returns true if this actuator is equal to \a other.
*/
bool QFeedbackActuator::operator==(const QFeedbackActuator &other) const
{
    return m_id == other.m_id;
}

#include "moc_qfeedbackactuator.cpp"

QTM_END_NAMESPACE
