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
# include "qtelephony_linux_p.h"
#endif
#ifdef Q_OS_WIN
# include "qtelephony_win_p.h"
#endif
#ifdef Q_OS_SYMBIAN
# include "qtelephony_s60_p.h"
#endif

QTM_BEGIN_NAMESPACE

/*!
    \class QTelephonyCallList

    \ingroup telephony

    \brief The QTelephonyCallList class represents the call list container and the notifier about call changes.

    QTelephonyCallList supports calls event notification for other application and an access of the list of the 
    existing calls.

\table
\header
    \o Class
\row
    \o QTelephonyCallList signal callstatusChanged
\row
    \o QTelephonyCallList signal callsChanged
\row
    \o QTelephonyCallList::calls (+ 3 overloaded)
\endtable
*/


Q_GLOBAL_STATIC(QTelephonyCallListPrivate, telephonycalllistprivate)

/*!
    \fn QTelephonyCallList::QTelephonyCallList(QObject *parent)
    
    Constructor for the Telephony Event object 
*/
QTelephonyCallList::QTelephonyCallList(QObject *parent)
    : QObject(parent), d(telephonycalllistprivate())
{
    connect(d, SIGNAL(callstatusChanged(const QCallInfo::CallStatus status))
        , this, SIGNAL(callstatusChanged(const QCallInfo::CallStatus status)));
    connect(d, SIGNAL(callsChanged()), this, SIGNAL(callsChanged()));
}

/*!
  Destroys the QTelephonyCallList object
*/
QTelephonyCallList::~QTelephonyCallList()
{
}

/*!
    \fn QTelephonyCallList::calls()
    
    Gives back a list of calls.
*/
QList<QCallInfo*> QTelephonyCallList::calls() const
{
    QList<QCallInfo*> ret;
    if(d)
        ret = d->calls();
    return ret;
}

/*!
    \fn QTelephonyCallList::calls(const QCallInfo::CallType calltype)
    
    Gives back a list of calls from type of calltype.
*/
QList<QCallInfo*> QTelephonyCallList::calls(const QCallInfo::CallType& calltype) const
{
    QList<QCallInfo*> ret;
    for( int i = 0; i < calls().count(); i++){
        if(calls().at(i)->type() == calltype)
            ret.push_back(calls().at(i));
    }
    return ret; 
}

/*!
    \fn QTelephonyCallList::calls(const QCallInfo::CallStatus& callstatus)
    
    Gives back a list of calls from status of callstatus.
*/
QList<QCallInfo*> QTelephonyCallList::calls(const QCallInfo::CallStatus& callstatus) const
{
    QList<QCallInfo*> ret;
    for( int i = 0; i < calls().count(); i++){
        if(calls().at(i)->status() == callstatus)
            ret.push_back(calls().at(i));
    }
    return ret; 
}

/*!
    \fn QTelephonyCallList::calllist(const QCallInfo::CallType& calltype, const QCallInfo::CallStatus& callstatus)
    
    Gives back a list of calls from status of callstatus and type of calltype.
*/
QList<QCallInfo*> QTelephonyCallList::calls(const QCallInfo::CallType& calltype
                                            , const QCallInfo::CallStatus& callstatus) const
{
    QList<QCallInfo*> ret;
    for( int i = 0; i < calls().count(); i++){
        if(calls().at(i)->type() == calltype && calls().at(i)->status() == callstatus)
            ret.push_back(calls().at(i));
    }
    return ret; 
}

/*!
    \fn QTelephonyCallList::currentCall()
    
    Gives back the current active call.
*/
QCallInfo* QTelephonyCallList::currentCall()
{
    if(d)
        return d->currentCall();
    return 0; 
}

/*!
    \fn QTelephonyCallList::currentCallStatus()
    
    Gives back the current telephony status.
*/
QCallInfo::CallStatus QTelephonyCallList::currentCallStatus()
{
    if(d)
        d->currentCallStatus();
    return QCallInfo::UnknownStatus;
}

#include "moc_qtelephony.cpp"

QTM_END_NAMESPACE
