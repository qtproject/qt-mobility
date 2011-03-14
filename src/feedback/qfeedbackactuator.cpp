/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qfeedbackactuator.h"
#include "qfeedbackplugininterfaces.h"

#include <QtCore/QVariant>

QTM_BEGIN_NAMESPACE

/*!
    \class QFeedbackActuator
    \brief The QFeedbackActuator class describes actuators for tactile feedback.
    \inmodule QtFeedback

    An actuator knows how to play a \l{QFeedbackEffect}{tactile
    effect}. The class gives access to a specified actuator.

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
    actuator it represents. You can query if the actuator \l{isEnabled()}{is enabled}
    and if it \l{isValid()}{is valid }. Whether an actuator is ready to play an
    effect can be queried by checking the actuator's state(). The
    \l{QFeedbackActuator::}{State} enum describes the states and
    actuator can have. You can also get a human readable name for the actuator with the
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
    \value Unknown The actuator is in an unknown state.
*/


/*!
    Constructs a QFeedbackActuator, passing \a parent to the QObject constructor.

    The object will represent the default actuator on the system.
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
    Constructs a QFeedbackActuator with id \a id, passing \a parent to the QObject constructor.

    This is used by plugins to represents their actuators.

    \sa isValid()
*/
QFeedbackActuator::QFeedbackActuator(QObject *parent, int id) : QObject(parent), m_id(id)
{
}

/*!
    \property QFeedbackActuator::id
    \brief id of the feedback actuator.
*/

/*!
    Returns the id of the actuator.
*/
int QFeedbackActuator::id() const
{
    return m_id;
}

/*!
    \property QFeedbackActuator::valid
    \brief validity of the feedback actuator
*/

/*!
    Returns true if the actuator is valid.
*/
bool QFeedbackActuator::isValid() const
{
    return m_id >= 0;
}

/*!
    \property QFeedbackActuator::name
    \brief name of the feedback actuator
*/

/*!
    Returns the name of the actuator.
*/
QString QFeedbackActuator::name() const
{
    return QFeedbackHapticsInterface::instance()->actuatorProperty(*this, QFeedbackHapticsInterface::Name).toString();
}

/*!
    \property QFeedbackActuator::state
    \brief state of the feedback actuator
*/

/*!
    Returns the state of the actuator.
*/
QFeedbackActuator::State QFeedbackActuator::state() const
{
    return QFeedbackActuator::State(QFeedbackHapticsInterface::instance()->actuatorProperty(*this, QFeedbackHapticsInterface::State).toInt());
}

/*!
    Returns if the actuator supports the supplied \a capability.
*/
bool QFeedbackActuator::isCapabilitySupported(Capability capability) const
{
    return QFeedbackHapticsInterface::instance()->isActuatorCapabilitySupported(*this, capability);
}

/*!
    \property QFeedbackActuator::enabled
    \brief whether the feedback actuator is enabled
*/

/*!
    Returns true if you can use this actuator to start effects.
*/
bool QFeedbackActuator::isEnabled() const
{
    return QFeedbackHapticsInterface::instance()->actuatorProperty(*this, QFeedbackHapticsInterface::Enabled).toBool();
}

/*!
    Allows you to enable or disable a actuator.  If \a enabled is true, the actuator will be enabled,
    and otherwise it will be disabled.

    \note Some systems may not allow you to change whether an actuator is enabled.
*/
void QFeedbackActuator::setEnabled(bool enabled)
{
    if (isEnabled() != enabled) {
        QFeedbackHapticsInterface::instance()->setActuatorProperty(*this, QFeedbackHapticsInterface::Enabled, enabled);
        emit enabledChanged();
    }
}

/*!
    \fn void QFeedbackActuator::enabledChanged()

    This signal is emitted when the actuator is requested to enable or disable itself.

    \sa isEnabled()
*/

/*!
    \fn QFeedbackActuator::actuators()

    Returns the list of actuators available on the system.
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
