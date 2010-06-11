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

#include "qdeclarativetelephony.h"

/*!
    \class QTelephonyCallInfoWrapper
    \ingroup telephony
    \brief The QTelephonyCallInfoWrapper class for a QTelephonyCallInfo object. This wrapper class can be used in QML to work with telephony.
*/

/*!
    \enum QTelephonyCallInfoWrapper::CallStatus

    This enum type is used to describe the status of a call.
    A QTelephonyCallInfoWrapper object can have a status of:

    \value UnknownStatus The call status is not defined.
    \value NoCall        The status of the call is not active.
    \value Ringing       The status of the call is ringing.
    \value InProgress    The status of the call is in progress.
    \value OnHold        The status of the call is on hold.
    \value Dropped       The call is dropped.
*/

/*!
    \enum QTelephonyCallInfoWrapper::CallType

    This enum decribes the type of the call.
    A QTelephonyCallInfoWrapper object can be a type of:

    \value UnknownType The call type is not defined.
    \value VOIP        The call is a VOIP call.
    \value Voice       The call is a Voice call.
    \value Video       The call is a Video call.
*/

/*!
    \fn  QTelephonyCallInfoWrapper::QTelephonyCallInfoWrapper()

    Constructor of a QTelephonyCallInfoWrapper object.
*/
QTelephonyCallInfoWrapper::QTelephonyCallInfoWrapper()
{
    d = new QTelephonyCallInfo();
}

/*!
    \fn  QTelephonyCallInfoWrapper::QTelephonyCallInfoWrapper(const QTelephonyCallInfo& other)
    \a QTelephonyCallInfo Object which needs to be copied from.

    Constructor of a QTelephonyCallInfoWrapper object.
*/
QTelephonyCallInfoWrapper::QTelephonyCallInfoWrapper(const QTelephonyCallInfo& other)
{
    d = new QTelephonyCallInfo(other);
}

/*!
    \fn  QTelephonyCallInfoWrapper::QTelephonyCallInfoWrapper(const QTelephonyCallInfoWrapper& other)
    \a other Object which needs to be copied from.

    Copy constructor of a QTelephonyCallInfoWrapper object.
*/
QTelephonyCallInfoWrapper::QTelephonyCallInfoWrapper(const QTelephonyCallInfoWrapper& other)
{
    d = new QTelephonyCallInfo(*other.d);
}

/*!
    \fn  QTelephonyCallInfoWrapper::~QTelephonyCallInfoWrapper()
    
    Destructor of a call info object.
*/
QTelephonyCallInfoWrapper::~QTelephonyCallInfoWrapper()
{
    delete d;
}

/*!
    \property QTelephonyCallInfoWrapper::callIdentifier
    \brief the id of a call.

    The callIdentifier contains the value dependend on the call CallType.
    It can be the phone number, IP address or something else.
*/
QString QTelephonyCallInfoWrapper::callIdentifier() const
{
    return d->callIdentifier();
}

/*!
    \property QTelephonyCallInfoWrapper::contacts
    \brief a list of the contacts ids (can be used in QContact).

    A contact id represent an id to find the contact in the system.
    If you want to find the contact information for a QTelephonyCallInfoWrapper object then call this function.
*/
QList<quint32> QTelephonyCallInfoWrapper::contacts() const
{
    return d->contacts();
}

/*!
    \property QTelephonyCallInfoWrapper::type
    \brief  the type of the call.

    A call can be a VOIP, Video Voice and so on.
    You can use this property to get the type of the call.
*/
QTelephonyCallInfoWrapper::CallType QTelephonyCallInfoWrapper::type() const
{
    return (QTelephonyCallInfoWrapper::CallType)d->type();
}

/*!
    \property QTelephonyCallInfoWrapper::status
    \brief the status of the call.

    A call can be in different conditions like ringing, dropped and so on.
    You can use this property if you want to know about the current condition of the call.
*/
QTelephonyCallInfoWrapper::CallStatus QTelephonyCallInfoWrapper::status() const
{
    return (QTelephonyCallInfoWrapper::CallStatus)d->status();
}


/*!
    \class QTelephonyCallListWrapper
    \ingroup telephony
    \brief The QTelephonyCallListWrapper class for a QTelephonyCallList object. This wrapper class can be used in QML to work with telephony.
*/

/*!
    \fn QTelephonyCallListWrapper::QTelephonyCallListWrapper(QObject *parent)
    \a parent The parent of this object. Default is 0.

    Constructor for the QTelephonyCallListWrapper object 
*/
QTelephonyCallListWrapper::QTelephonyCallListWrapper()
{
    d = new QTelephonyCallList(this);
    connect(d, SIGNAL(activeCallStatusChanged(const QTelephonyCallInfo))
        , this, SIGNAL(activeCallStatusChanged(const QTelephonyCallInfo)));
    connect(d, SIGNAL(activeCallRemoved(const QTelephonyCallInfo))
        , this, SIGNAL(activeCallRemoved(const QTelephonyCallInfo)));
    connect(d, SIGNAL(activeCallAdded(const QTelephonyCallInfo))
        , this, SIGNAL(activeCallAdded(const QTelephonyCallInfo)));
}

/*!
    \fn QTelephonyCallListWrapper::~QTelephonyCallListWrapper()

    Destructor of QTelephonyCallListWrapper.
*/
QTelephonyCallListWrapper::~QTelephonyCallListWrapper()
{
    delete d;
}

/*!
    \fn QList<QTelephonyCallListWrapper> QTelephonyCallListWrapper::activeCalls(const CallType& calltype) const
    \a calltype All calls in the list have this type.

    Gives back a list of calls from type of calltype.
*/
QList<QTelephonyCallInfoWrapper> QTelephonyCallListWrapper::activeCalls(const QTelephonyCallInfoWrapper::CallType& calltype) const
{
    
    QList<QtMobility::QTelephonyCallInfo> calllist;
    QList<QTelephonyCallInfoWrapper> ret;
    if(d)
        calllist = d->activeCalls((QTelephonyCallInfo::CallType)calltype);

    //call copy constructor so the caller has to delete the QTelephonyCallInfo pointers
    for( int i = 0; i < calllist.count(); i++){
        ret.push_back(QTelephonyCallInfoWrapper(calllist.at(i)));
    }
    return ret;
}

/*! 
    \fn void QTelephonyCallListWrapper::activeCallStatusChanged(const QTelephonyCallInfo& call);
    \a call The QTelephonyCallInfo object that changed its status.

    This Slot is called whenever a call changed its status.
*/
void QTelephonyCallListWrapper::activeCallStatusChanged(const QTelephonyCallInfo& call)
{
    emit activeCallStatusChanged(QTelephonyCallInfoWrapper(call));
}

/*! 
    \fn void QTelephonyCallListWrapper::activeCallRemoved(const QTelephonyCallInfo& call);
    \a call The QTelephonyCallInfo object that was removed from the active call list.

    This Slot is called whenever a call was removed from the active call list of QTelephonyCallList.
*/
void QTelephonyCallListWrapper::activeCallRemoved(const QTelephonyCallInfo& call)
{
    emit activeCallRemoved(QTelephonyCallInfoWrapper(call));
}

/*! 
    \fn void QTelephonyCallListWrapper::activeCallAdded(const QTelephonyCallInfo& call);
    \a call The QTelephonyCallInfo object that was added to the active call list.

    This Slot is called whenever a call was added to the active call list of QTelephonyCallList.
*/
void QTelephonyCallListWrapper::activeCallAdded(const QTelephonyCallInfo& call)
{
    emit activeCallAdded(QTelephonyCallInfoWrapper(call));
}

