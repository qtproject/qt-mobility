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


#include "qtelephonycalllist.h"
#include "qtelephonycallinfo.h"

#ifdef Q_OS_LINUX
# include "qtelephonycalllist_linux_p.h"
#endif
#ifdef Q_OS_WIN
# include "qtelephonycalllist_win_p.h"
#endif
#ifdef Q_OS_SYMBIAN
# include "qtelephonycalllist_s60_p.h"
#endif

#include "qtelephonycallinfoproperty.h"

QTM_BEGIN_NAMESPACE

/*! 
    \fn void QTelephonyCallList::callstatusChanged(const QTelephonyCallInfo::CallStatus& status);

    This signal is emitted whenever the status of the current active call is changed.
    The new status of the current call is specified by \a status.
*/

/*! 
    \fn void QTelephonyCallList::callsChanged();

    This signal is emitted whenever a change in the calls occured.
*/


/*!
    \class QTelephonyCallList

    \ingroup telephony

    \brief The QTelephonyCallList class contains a list of active calls. This object can notify other applications about call changes.

    QTelephonyCallList supports calls event notification for other application and an access of the list of the 
    existing calls.
*/


Q_GLOBAL_STATIC(QTelephonyCallListPrivate, telephonycalllistprivate)

/*!
    \fn QTelephonyCallList::QTelephonyCallList(QObject *parent)
    \a parent The parent of this object. Default is 0.

    Constructor for the QTelephonyCallList object 
*/
QTelephonyCallList::QTelephonyCallList(QObject *parent)
    : QObject(parent), d(telephonycalllistprivate())
{
    connect(d, SIGNAL(callstatusChanged(const QTelephonyCallInfo::CallStatus))
        , this, SIGNAL(callstatusChanged(const QTelephonyCallInfo::CallStatus)));
    connect(d, SIGNAL(callsChanged())
        , this, SIGNAL(callsChanged()));
}

/*!
    \fn QTelephonyCallList::~QTelephonyCallList()

    Destructor of QTelephonyCallList.
*/
QTelephonyCallList::~QTelephonyCallList()
{
}

/*!
    \fn QList<QTelephonyCallInfo* > QTelephonyCallList::calls() const

    Gives back a list of all active calls.
    Note that the caller retains ownership of the QTelephonyCallInfo pointers.
*/
QList<QTelephonyCallInfo* > QTelephonyCallList::calls() const
{
    QList<QTelephonyCallInfo* > calllist;
    QList<QTelephonyCallInfo* > ret;
    if(d)
        calllist = d->calls();

    //call copy constructor so the caller has to delete the QTelephonyCallInfo pointers
    for( int i = 0; i < calllist.count(); i++){
        ret.push_back(new QTelephonyCallInfo(*calllist.at(i)));
    }
    return ret;
}

/*!
    \fn  QList<QTelephonyCallInfoProperty* > QTelephonyCallList::callsProperty() const

    Gives back a list of all active calls.
    Note that the caller retains ownership of the QTelephonyCallInfoProperty pointers.
*/
/*!
    \property QTelephonyCallList::calls

    Gives back a list of all active calls.
    Note that the caller retains ownership of the QTelephonyCallInfoProperty pointers.
*/
QList<QTelephonyCallInfoProperty* > QTelephonyCallList::callsProperty() const
{
    QList<QTelephonyCallInfoProperty* > ret;

    //with calls() we have already copy of QTelephonyCallInfo pointers.
    QList<QTelephonyCallInfo* > calllist = calls();
    for( int i = 0; i < calllist.count(); i++){
        ret.push_back(new QTelephonyCallInfoProperty(*calllist.at(i)));
    }
    return ret;
}

/*!
    \fn QList<QTelephonyCallInfo* > QTelephonyCallList::calls(const QTelephonyCallInfo::CallType& calltype) const
    \a calltype All calls in the list have this type.

    Gives back a list of calls from type of calltype.
    Note that the caller retains ownership of the QTelephonyCallInfo pointers.
*/
QList<QTelephonyCallInfo* > QTelephonyCallList::calls(const QTelephonyCallInfo::CallType& calltype) const
{
    QList<QTelephonyCallInfo* > ret;
    for( int i = 0; i < calls().count(); i++){
        if(calls().at(i)->type() == calltype)
            ret.push_back(calls().at(i));
    }
    return ret; 
}

/*!
    \fn QList<QTelephonyCallInfo* > QTelephonyCallList::calls(const QTelephonyCallInfo::CallStatus& callstatus) const
    \a callstatus All calls in the list have this status.

    Gives back a list of calls from status of callstatus.
    Note that the caller retains ownership of the QTelephonyCallInfo pointers.
*/
QList<QTelephonyCallInfo* > QTelephonyCallList::calls(const QTelephonyCallInfo::CallStatus& callstatus) const
{
    QList<QTelephonyCallInfo* > ret;
    for( int i = 0; i < calls().count(); i++){
        if(calls().at(i)->status() == callstatus)
            ret.push_back(calls().at(i));
    }
    return ret; 
}

/*!
    \fn QList<QTelephonyCallInfo* > QTelephonyCallList::calls(const QTelephonyCallInfo::CallType& calltype, const QTelephonyCallInfo::CallStatus& callstatus) const
    \a calltype All calls in the list have this type.
    \a callstatus All calls in the list have this status.

    Gives back a list of calls from status of callstatus and type of calltype.
    Note that the caller retains ownership of the QTelephonyCallInfo pointers.
*/
QList<QTelephonyCallInfo* > QTelephonyCallList::calls(const QTelephonyCallInfo::CallType& calltype
                                            , const QTelephonyCallInfo::CallStatus& callstatus) const
{
    QList<QTelephonyCallInfo* > ret;
    for( int i = 0; i < calls().count(); i++){
        if(calls().at(i)->type() == calltype && calls().at(i)->status() == callstatus)
            ret.push_back(calls().at(i));
    }
    return ret; 
}

/*!
    \fn QTelephonyCallInfo* QTelephonyCallList::currentCall() const

    \brief Gves back the current call.
    Note that the caller retains ownership of the QTelephonyCallInfo pointer
*/
QTelephonyCallInfo* QTelephonyCallList::currentCall() const
{
    if(d)
        return d->currentCall();
    return 0; 
}

/*!
    \fn  QTelephonyCallInfoProperty* QTelephonyCallList::currentCallProperty() const

    Gives back the current call.
    Note that the caller retains ownership of the QTelephonyCallInfoProperty pointer
*/
/*!
    \property QTelephonyCallList::currentCall const

    \brief Gives back the current call.
    Note that the caller retains ownership of the QTelephonyCallInfoProperty pointer
*/
QTelephonyCallInfoProperty* QTelephonyCallList::currentCallProperty() const
{
    QTelephonyCallInfo* pti = 0;
    if(d)
        pti = d->currentCall();
    if(pti)
        return new QTelephonyCallInfoProperty(*pti);
    return 0; 
}

#include "moc_qtelephonycalllist.cpp"

QTM_END_NAMESPACE
