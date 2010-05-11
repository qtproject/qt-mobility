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


#include "qtelephony.h"
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
    \class QTelephonyEvent

    \ingroup telephony

    \brief The QTelephonyEvent class represents the call list container and the notifier about call changes.

    QTelephonyEvent supports calls event notification for other application and an access of the list of the 
    existing calls.

\table
\header
    \o Class
\row
    \o QTelephonyEvent signal callStatusChanges
\row
    \o QTelephonyEvent signal callListChanges
\row
    \o QTelephonyEvent::calllist (+ 3 overloaded)
\endtable
*/


Q_GLOBAL_STATIC(QTelephonyEventPrivate, telephonyeventPrivate)

/*!
    \fn QTelephonyEvent::QTelephonyEvent(QObject *parent)
    
    Constructor for the Telephony Event object 
*/
QTelephonyEvent::QTelephonyEvent(QObject *parent)
    : QObject(parent), d(telephonyeventPrivate())
{
    connect(d, SIGNAL(callstatusChange(CallStatus status)), this, SIGNAL(callstatusChange(CallStatus status)));
    connect(d, SIGNAL(calllistChange()), this, SIGNAL(calllistChange()));
}

/*!
  Destroys the QTelephonyEvent object
*/
QTelephonyEvent::~QTelephonyEvent()
{
}

/*!
    \fn QTelephonyEvent::calllist()
    
    Gives back a list of calls.
*/
QList<QCallInfo*> QTelephonyEvent::calllist()
{
    return calls;
}

/*!
    \fn QTelephonyEvent::calllist(CallType calltype)
    
    Gives back a list of calls from type of calltype.
*/
QList<QCallInfo*> QTelephonyEvent::calllist(CallType calltype)
{
    QList<QCallInfo*> ret;
    for( int i = 0; i < calls.count(); i++){
        if(calls[i]->callType() == calltype)
            ret.push_back(calls[i]);
    }
    return calls; 
}

/*!
    \fn QTelephonyEvent::calllist(CallStatus callstatus)
    
    Gives back a list of calls from status of callstatus.
*/
QList<QCallInfo*> QTelephonyEvent::calllist(CallStatus callstatus)
{
    QList<QCallInfo*> ret;
    for( int i = 0; i < calls.count(); i++){
        if(calls[i]->callStatus() == callstatus)
            ret.push_back(calls[i]);
    }
    return calls; 
}

/*!
    \fn QTelephonyEvent::calllist(CallType calltype, CallStatus callstatus)
    
    Gives back a list of calls from status of callstatus and type of calltype.
*/
QList<QCallInfo*> QTelephonyEvent::calllist(CallType calltype, CallStatus callstatus)
{
    QList<QCallInfo*> ret;
    for( int i = 0; i < calls.count(); i++){
        if(calls[i]->callType() == calltype && calls[i]->callStatus() == callstatus)
            ret.push_back(calls[i]);
    }
    return calls; 
}

/*!
    \fn QTelephonyEvent::currentStatus()
    
    Gives back the current telephony status.
*/
CallStatus QTelephonyEvent::currentStatus()
{
    if(d)
        d->currentStatus();
    return CallStatus::UnknownStatus;
}

#include "moc_qtelephony.cpp"

QTM_END_NAMESPACE
