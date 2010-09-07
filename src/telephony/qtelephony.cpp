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

/*!
    \namespace QTelephony

    \inmodule QtTelephony

    \ingroup telephonyapienum

    \brief The QTelephony namespace contains all enumerators for the Telephony Events API.
*/

/*!
    \enum  QTelephony::CallStatus

    This enum type is used to describe the status of a call.
    A QTelephonyCallInfo object can have a status of:

    \value Idle          The call status is not defined.
    \value Dialing       The status of the call is dialing.
    \value Alerting      The status of the call is alerting.
    \value Connected     The status of the call is connected.
    \value Disconnecting The status of the call is disconnecting.
    \value Incoming      The status of the call is incoming.
    \value OnHold        The status of the call is on hold.
*/

/*!
    \enum QTelephony::CallType

    This enum decribes the type of the call.
    A QTelephonyCallInfo object can be a type of:

    \value Any         The call type can be any type.
    \value Text        The call is a text base call.
    \value Data        The call is a data based call.
    \value Video       The call is a video based call.
    \value Voice       The call is a voice based call.
    \value Other       The call is a an unknown base call.
*/

/*!
    \enum QTelephony::Direction

    This enum decribes the direction of the call.
    A QTelephonyCallInfo object can have a direction of:

    \value Received    The call is received.
    \value Dialed      The call is dialed.
*/
