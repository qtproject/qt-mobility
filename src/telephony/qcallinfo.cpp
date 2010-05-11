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


#include "qcallinfo.h"

#ifdef Q_OS_LINUX
# include "qtelephonyevent_linux_p.h"
#endif
#ifdef Q_OS_WIN
# include "qtelephonyevent_win_p.h"
#endif
#ifdef Q_OS_SYMBIAN
# include "qtelephonyevent_s60_p.h"
#endif

QTM_BEGIN_NAMESPACE

/*!
    \class QCallInfo

    \ingroup telephony

    \brief The QCallInfo class represents a call including the type, status and other information of the call.

\table
\header
    \o Class
\row
    \o QCallInfo::phoneNumber
\row
    \o QCallInfo::sipID
\row
    \o QCallInfo::contacts
\row
    \o QCallInfo::querryData
\row
    \o QCallInfo::callType
\row
    \o QCallInfo::callStatus
\endtable
*/

Q_GLOBAL_STATIC(QCallInfoPrivate, callinfoprivate)

/*!
    \fn QCallInfo::QCallInfo(QObject *parent)
    
    Constructor of a call info object.
*/
QCallInfo::QCallInfo(QObject *parent)
    : QObject(parent), d(callinfoprivate())
{
}

/*!
    \fn QCallInfo::~QCallInfo()
    
    Destructor of a call info object.
*/
QCallInfo::~QCallInfo()
{
}

/*!
    \fn QCallInfo::phoneNumber()
    
    Gives back the phone number of a call.
*/
QString QCallInfo::phoneNumber()
{
    if(d)
        return d->phoneNumber();
    return "unknown";
}

/*!
    \fn QCallInfo::sipID()
    
    Gives back the SIP ID of a call.
*/
unsigned long QCallInfo::sipID()
{
    if(d)
        d->sipID();
    return 0;
}

/*!
    \fn QCallInfo::sipID()
    
    Gives back a list of contact id (used in QContact).
*/
QList<QContactLocalId> QCallInfo::contacts()
{
    QList<QContactLocalId> ret;
    if(d)
        ret = d->contacts();
    return ret;
}

/*!
    \fn QCallInfo::querryData( PrivateDataType datatype, const QObject& param, void** value)
    
    Function that allows to querry special data out from the private QCallInfo implementation.
*/
bool QCallInfo::querryData( PrivateDataType datatype, const QObject& param, void** value)
{
    if(d)
        return d->querryData( datatype, param, value);
    return false;
}

/*!
    \fn QCallInfo::callType()
    
    Gives back the type of the call.
*/
CallType QCallInfo::callType()
{
    if(d)
        return d->callType();
    return CallType::UnknownType;
}

/*!
    \fn QCallInfo::callType()
    
    Gives back the status of the call.
*/
CallStatus QCallInfo::callStatus()
{
    if(d)
        return d->callStatus();
    return CallStatus::UnknownStatus;
}

#include "moc_qcallinfo.cpp"

QTM_END_NAMESPACE
