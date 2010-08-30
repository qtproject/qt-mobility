/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

/*!
    \namespace QTelephonyEvents

    \inmodule QtTelephony

    \ingroup telephonyapienum

    \brief The QTelephonyEvents namespace contains all enumerators for the Telephony Events API.
*/

/*!
    \enum  QTelephonyEvents::CallStatus

    This enum type is used to describe the status of a call.
    A QTelephonyCallInfo object can have a status of:

    \value Idle          The call status is not defined.
    \value Dialing       The status of the call is dialing.
    \value Alerting      The status of the call is alerting.
    \value Connected     The status of the call is connected.
    \value Disconnecting The status of the call is disconnecting.
    \value Incomming     The status of the call is incomming.
    \value OnHold        The status of the call is on hold.
*/

/*!
    \enum QTelephonyEvents::CallType

    This enum decribes the type of the call.
    A QTelephonyCallInfo object can be a type of:

    \value Any         The call type can be any type.
    \value Text        The call is a text base call.
    \value Data        The call is a data based call.
    \value Video       The call is a video based call.
    \value Voice       The call is a voice based call.
    \value Other       The call is a an unknown base call.
*/
