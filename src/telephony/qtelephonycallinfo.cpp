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

#include "qmobilityglobal.h"
#include "qtelephonycallinfo_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QTelephonyCallInfo
    \ingroup telephony
    \brief The QTelephonyCallInfo class represents a call including the type, status and other information of the call.
*/

/*!
    \enum QTelephonyCallInfo::CallStatus

    This enum type is used to describe the status of a call.
    A QTelephonyCallInfo object can have a status of:

    \value Undefined     The call status is not defined.
    \value NoCall        The status of the call is not active.
    \value Ringing       The status of the call is ringing.
    \value InProgress    The status of the call is in progress.
    \value OnHold        The status of the call is on hold.
    \value Dropped       The call is dropped.
*/

/*!
    \enum QTelephonyCallInfo::CallType

    This enum decribes the type of the call.
    A QTelephonyCallInfo object can be a type of:

    \value Unknown     The call type is not defined.
    \value Any         The call type can be any type.
    \value Voip        The call is a Voip call.
    \value Voice       The call is a Voice call.
    \value Video       The call is a Video call.
*/

/*!
    \class QTelephonyCallInfo

    \ingroup telephony

    \brief The QTelephonyCallInfo class represents a call including the type, status and other information of the call.
*/

/*!
    \fn  QTelephonyCallInfo::QTelephonyCallInfo()

    Constructor of a QTelephonyCallInfo object.
*/
QTelephonyCallInfo::QTelephonyCallInfo()
    : d(0)
{
}

/*!
    \fn  QTelephonyCallInfo::QTelephonyCallInfo(const QTelephonyCallInfo& other)
    \a other Object which needs to be copied from.

    Constructor of a QTelephonyCallInfo object using a const QTelephonyCallInfo.
*/
QTelephonyCallInfo::QTelephonyCallInfo(const QTelephonyCallInfo& other)
    : d(other.d)
{
}

/*!
    \fn  QTelephonyCallInfo::~QTelephonyCallInfo()

    Destructor of a call info object.
*/
QTelephonyCallInfo::~QTelephonyCallInfo()
{
}

/*!
    \fn  QString QTelephonyCallInfo::callIdentifier() const

    Gives back the id of a call.
    The callIdentifier contains the value dependend on the call CallType.
    It can be the phone number, IP address or something else.
*/
QString QTelephonyCallInfo::callIdentifier() const
{
    if(d)
        return d->callIdentifier;
    return QString();
}

/*!
    \fn  QList<quint32> QTelephonyCallInfo::contacts() const

    Gives back a list of contacts ids (can be used in QContact).
    A contact id represent an id to find the contact in the system.
    If you want to find the contact information for a QTelephonyCallInfo object then call this function.
*/
QList<quint32> QTelephonyCallInfo::contacts() const
{
    QList<quint32> ret;
    if(d)
        ret = d->contacts;
    return ret;
}

/*!
    \fn  QTelephonyCallInfo::CallType QTelephonyCallInfo::type() const

    Gives back the type of the call.
    A call can be a VOIP, Video Voice and so on.
    You can use this property to get the type of the call.
*/
QTelephonyCallInfo::CallType QTelephonyCallInfo::type() const
{
    if(d)
        return d->type;
    return Unknown;
}

/*!
    \fn  QString QTelephonyCallInfo::subTyp() const

    Gives back the sub type of the call type.
    A subtype of a Voip call can be Skype.
    You can use this property to get the sub type of the call.
*/
QString QTelephonyCallInfo::subTyp() const
{
    if(d)
        return d->subTyp;
    return QString();
}

/*!
    \fn  QTelephonyCallInfo::CallStatus QTelephonyCallInfo::status() const

    Gives back the status of the call.
    A call can be in different conditions like ringing, dropped and so on.
    You can use this property if you want to know about the current condition of the call.
*/
QTelephonyCallInfo::CallStatus QTelephonyCallInfo::status() const
{
    if(d)
        return d->status;
    return Undefined;
}

/*!
    \fn  QVariant QTelephonyCallInfo::value(const QString& param) const
    \a param Parameter for reading additional values.

    Gives back a variant value.
    This function is for reading special values dependce on the param parameter.
*/
QVariant QTelephonyCallInfo::value(const QString& param) const
{
    if(d && d->values.contains(param)){
        return d->values.value(param);
    }
    return QVariant();
}

QTM_END_NAMESPACE
