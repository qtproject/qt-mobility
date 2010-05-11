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


#ifndef QTELEPHONYGLOBAL_H
#define QTELEPHONYGLOBAL_H

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

/*!
    \enum CallType

    \ingroup telephony

    This enum type is used to describe the type of a call.
    
    \value UnknownType The call type is not defined.
    \value VOIP        The call is a VOIP call.
    \value Voice       The call is a Voice call.
    \value Video       The call is a Video call.
*/
enum CallType {
    UnknownType = 0,
    VOIP = 1,
    Voice = 2,
    Video = 3
};

/*!
    \enum CallStatus

    \ingroup telephony

    This enum type is used to describe the status of a call.
    
    \value UnknownStatus The call status is not defined.
    \value NoCall        The status  of the call is no active call.
    \value Ringing       The status  of the call is ringing.
    \value InProgress    The status  of the call is in progress.
    \value OnHold        The status  of the call is on hold.
*/
enum CallStatus {
    UnknownStatus = 0,
    NoCall = 1,
    Ringing = 2,
    InProgress = 3,
    OnHold = 4
};

QTM_END_NAMESPACE
QT_END_HEADER

#endif