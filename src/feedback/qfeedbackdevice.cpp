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

#include "qfeedbackdevice.h"
#include "qfeedbackplugin.h"

QTM_BEGIN_NAMESPACE

        /*!
            \class QFeedbackDevice
            \brief The QFeedbackDevice class is describing devices for tactile feedback.

            The class gives access to a specified device for tactile feedback.
            It allows the program to know the kind of device, its state and can be used
            in combination with QFeedbackEffect.

            You can query the devices available on your system.

            \sa QFeedbackEffect
        */

        /*!
            \enum QFeedbackDevice::Type

            \value Vibra The device represents a Vibra motor on your system.
            \value Touch The device represents touch feedback (piezzo...).
        */

        /*!
            \enum QFeedbackDevice::Type

            \value Vibra The device represents a Vibra motor on your system.
            \value Touch The device represents touch feedback (piezzo...).
        */

        /*!
            \enum QFeedbackDevice::Capability

            \value Envelope Capacity defining the wave type with attack/fade times and levels.
        */

        /*!
            \enum QFeedbackDevice::State

            \value Busy    The device is busy.
            \value Ready   The device is ready to play an effect.
            \value Unknown The device is in an anknown state.
        */

        /*!
            \fn int QFeedbackDevice::id()

            returns the id of the device
        */

        /*!
            \fn bool QFeedbackDevice::isValid()

            returns true if the device is valid. Default constructed devices are invalid.
        */


        /*!
            \fn QString QFeedbackDevice::name()

            returns the name of the device.
        */

        /*!
            \fn QFeedbackDevice::State QFeedbackDevice::state()

            returns the state of the device.
        */

        /*!
            \fn QFeedbackDevice QFeedbackDevice::defaultDevice()

            returns the default device.
        */

        /*!
            \fn QFeedbackDevice QFeedbackDevice::defaultDevice(Type t)

            returns the default device of the specified type.
        */

        /*!
            \fn QList<QFeedbackDevice> QFeedbackDevice::devices()

            returns the list of devices available on the system.
        */

        /*!
            \fn  bool QFeedbackDevice::isEnabled()

            returns true if you can use this device to start effects.
        */

        /*!
            \fn  void QFeedbackDevice::setEnabled()

            Allows to enable or disable a device.
        */

QFeedbackDevice::QFeedbackDevice() : m_id(-1)
{
}

int QFeedbackDevice::id() const
{
    return m_id;
}

bool QFeedbackDevice::isValid() const
{
    return m_id >= 0;
}

QFeedbackDevice QFeedbackDevice::defaultDevice()
{
    QList<QFeedbackDevice> ret = devices();
    if (ret.isEmpty())
        return QFeedbackDevice();

    return ret.first();
}


QString QFeedbackDevice::name() const
{
    return QFeedbackInterface::instance()->deviceName(*this);
}

QFeedbackDevice::State QFeedbackDevice::state() const
{
    return QFeedbackInterface::instance()->deviceState(*this);
}

QFeedbackDevice::Capabilities QFeedbackDevice::supportedCapabilities() const
{
    return QFeedbackInterface::instance()->supportedCapabilities(*this);
}

bool QFeedbackDevice::isEnabled() const
{
    return QFeedbackInterface::instance()->isEnabled(*this);
}

void QFeedbackDevice::setEnabled(bool enabled)
{
    QFeedbackInterface::instance()->setEnabled(*this, enabled);
}

QList<QFeedbackDevice> QFeedbackDevice::devices()
{
    return QFeedbackInterface::instance()->devices();
}


#include "moc_qfeedbackdevice.cpp"

QTM_END_NAMESPACE
