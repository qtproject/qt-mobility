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

#include "qtelephonycallinfoproperty.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QTelephonyCallInfoProperty

    \ingroup telephony

    \brief The QTelephonyCallInfoProperty class enhance the QTelephonyCallInfo to
    access the QTelephonyCallInfo data by using properties.

    User can use this class to access datas of a QTelephonyCallInfo object by using QML.
*/

/*!
    \enum QTelephonyCallInfoProperty::CallStatus

    This enum type is used to describe the status of a call.
    A QTelephonyCallInfoProperty object can have a status of:

    \value UnknownStatus The call status is not defined.
    \value NoCall        The status of the call is not active.
    \value Ringing       The status of the call is ringing.
    \value InProgress    The status of the call is in progress.
    \value OnHold        The status of the call is on hold.
    \value Dropped       The call is dropped.
*/

/*!
    \enum QTelephonyCallInfoProperty::CallType

    This enum decribes the type of the call.
    A QTelephonyCallInfoProperty object can be a type of:

    \value UnknownType The call type is not defined.
    \value VOIP        The call is a VOIP call.
    \value Voice       The call is a Voice call.
    \value Video       The call is a Video call.
*/

/*!
    \fn QTelephonyCallInfoProperty::QTelephonyCallInfoProperty(const QTelephonyCallInfo& info)

    \a info The QTelephonyCallInfo object to copy.

    Copy constructor for the QTelephonyCallInfoProperty object 
*/
QTelephonyCallInfoProperty::QTelephonyCallInfoProperty(const QTelephonyCallInfo& info)
{
    pqtelephonyCallInfo = new QTelephonyCallInfo(info);
}

/*!
    \fn QTelephonyCallInfoProperty::~QTelephonyCallInfoProperty()
    
    Destructor of QTelephonyCallInfoProperty.
*/
QTelephonyCallInfoProperty::~QTelephonyCallInfoProperty()
{
    if(pqtelephonyCallInfo)
        delete pqtelephonyCallInfo;
}

/*!
    \property QTelephonyCallInfoProperty::callIdentifier
    \brief the id of a call.

    The callIdentifier contains the value dependend on the call CallType.
    It can be the phone number, IP address or something else.
*/
QString QTelephonyCallInfoProperty::callIdentifier() const
{
    return pqtelephonyCallInfo->callIdentifier();
}

/*!
    \property QTelephonyCallInfoProperty::contacts
    \brief a list of the contacts ids (can be used in QContact).

    A contact id represent an id to find the contact in the system.
    If you want to find the contact information for a QTelephonyCallInfo object then call this function.
*/
QList<quint32> QTelephonyCallInfoProperty::contacts() const
{
    return pqtelephonyCallInfo->contacts();
}

/*!
    \property QTelephonyCallInfoProperty::type
    \brief  the type of the call.

    A call can be a VOIP, Video Voice and so on.
    You can use this property to get the type of the call.
*/
QTelephonyCallInfoProperty::CallType QTelephonyCallInfoProperty::type() const
{
    return (QTelephonyCallInfoProperty::CallType)pqtelephonyCallInfo->type();
}

/*!
    \property QTelephonyCallInfoProperty::status
    \brief the status of the call.

    A call can be in different conditions like ringing, dropped and so on.
    You can use this property if you want to know about the current condition of the call.
*/
QTelephonyCallInfoProperty::CallStatus QTelephonyCallInfoProperty::status() const
{
    return (QTelephonyCallInfoProperty::CallStatus)pqtelephonyCallInfo->status();
}

#include "moc_qtelephonycallinfoproperty.cpp"

QTM_END_NAMESPACE
