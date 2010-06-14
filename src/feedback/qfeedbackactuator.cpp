/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
#include "qfeedbackplugin.h"

#include <QtCore/QVariant>

QTM_BEGIN_NAMESPACE

        /*!
            \class QFeedbackActuator
            \brief The QFeedbackActuator class is describing actuators for tactile feedback.

            The class gives access to a specified actuator for tactile feedback.
            It allows the program to know its state and can be used
            in combination with QHapticsFeedbackEffect.

            You can query the actuators available on your system.

            \sa QHapticsFeedbackEffect
        */

        /*!
            \enum QFeedbackActuator::Capability

            \value Envelope Capacity defining the wave type with attack/fade times and levels.
        */

        /*!
            \enum QFeedbackActuator::State

            \value Busy    The actuator is busy.
            \value Ready   The actuator is ready to play an effect.
            \value Unknown The actuator is in an anknown state.
        */

        /*!
            \fn int QFeedbackActuator::id()

            returns the id of the actuator
        */

        /*!
            \fn bool QFeedbackActuator::isValid()

            returns true if the actuator is valid. Default constructed actuators are invalid.
        */


        /*!
            \fn QString QFeedbackActuator::name()

            returns the name of the actuator.
        */

        /*!
            \fn QFeedbackActuator::State QFeedbackActuator::state()

            returns the state of the actuator.
        */

        /*!
            \fn QFeedbackActuator QFeedbackActuator::defaultActuator()

            returns the default actuator.
        */

        /*!
            \fn QList<QFeedbackActuator> QFeedbackActuator::actuators()

            returns the list of actuators available on the system.
        */

        /*!
            \fn  bool QFeedbackActuator::isEnabled()

            returns true if you can use this actuator to start effects.
        */

        /*!
            \fn  void QFeedbackActuator::setEnabled()

            Allows to enable or disable a actuator.
        */

QFeedbackActuator::QFeedbackActuator() : m_id(-1)
{
}

int QFeedbackActuator::id() const
{
    return m_id;
}

bool QFeedbackActuator::isValid() const
{
    return m_id >= 0;
}

QFeedbackActuator QFeedbackActuator::defaultActuator()
{
    QList<QFeedbackActuator> ret = actuators();
    if (ret.isEmpty())
        return QFeedbackActuator();

    return ret.first();
}


QString QFeedbackActuator::name() const
{
    return QHapticsFeedbackInterface::instance()->actuatorProperty(*this, QHapticsFeedbackInterface::Name).toString();
}

QFeedbackActuator::State QFeedbackActuator::state() const
{
    return QFeedbackActuator::State(QHapticsFeedbackInterface::instance()->actuatorProperty(*this, QHapticsFeedbackInterface::Name).toInt());
}

QFeedbackActuator::Capabilities QFeedbackActuator::supportedCapabilities() const
{
  return  QFeedbackActuator::Capabilities(QHapticsFeedbackInterface::instance()->actuatorProperty(*this, QHapticsFeedbackInterface::SupportedCapabilities).toInt());
}

bool QFeedbackActuator::isEnabled() const
{
    return QHapticsFeedbackInterface::instance()->actuatorProperty(*this, QHapticsFeedbackInterface::Enabled).toBool();
}

void QFeedbackActuator::setEnabled(bool enabled)
{
    QHapticsFeedbackInterface::instance()->setActuatorProperty(*this, QHapticsFeedbackInterface::Enabled, enabled);
}

QList<QFeedbackActuator> QFeedbackActuator::actuators()
{
    return QHapticsFeedbackInterface::instance()->actuators();
}


#include "moc_qfeedbackactuator.cpp"

QTM_END_NAMESPACE
