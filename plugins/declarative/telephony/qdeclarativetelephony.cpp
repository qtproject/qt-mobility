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
    \enum QTelephonyCallInfo::CallStatus

    This enum type is used to describe the status of a call.
    A QTelephonyCallInfo object can have a status of:

    \value Idle          The call status is not defined.
    \value Dialing       The status of the call is dialing.
    \value Alerting      The status of the call is alerting.
    \value Connected     The status of the call is connected.
    \value Disconnecting The status of the call is disconnected.
    \value Incomming     The status of the call is incomming.
    \value OnHold        The status of the call is on hold.
*/

/*!
    \enum QTelephonyCallInfo::CallType

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
    \fn  QTelephonyCallInfoWrapper::QTelephonyCallInfoWrapper()

    Constructor of a QTelephonyCallInfoWrapper object.
*/
QTelephonyCallInfoWrapper::QTelephonyCallInfoWrapper()
    : QObject(0)
{
    d = new QTelephonyCallInfo();
}

/*!
    \fn  QTelephonyCallInfoWrapper::QTelephonyCallInfoWrapper(const QTelephonyCallInfo& other)
    \a QTelephonyCallInfo Object which needs to be copied from.

    Constructor of a QTelephonyCallInfoWrapper object.
*/
QTelephonyCallInfoWrapper::QTelephonyCallInfoWrapper(const QTelephonyCallInfo& other)
    : QObject(0)
{
   // d = new QTelephonyCallInfo(other);
}

/*!
    \fn  QTelephonyCallInfoWrapper::QTelephonyCallInfoWrapper(const QTelephonyCallInfoWrapper& other)
    \a other Object which needs to be copied from.

    Copy constructor of a QTelephonyCallInfoWrapper object.
*/
QTelephonyCallInfoWrapper::QTelephonyCallInfoWrapper(const QTelephonyCallInfoWrapper& other)
    : QObject(0)
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
    \fn  QTelephonyCallInfoWrapper& QTelephonyCallInfoWrapper::operator=(const QTelephonyCallInfoWrapper& other)

    Assigns this relationship to be equal to \a other.
*/
QTelephonyCallInfoWrapper& QTelephonyCallInfoWrapper::operator=(const QTelephonyCallInfoWrapper& other)
{
    d = other.d;
    return *this;
}

/*!
    \property QTelephonyCallInfoWrapper::remotePartyIdentifier
    \brief the id of a call.

    The remotePartyIdentifier contains the value dependend on the call CallType.
    It can be the phone number, IP address or something else.
*/
QString QTelephonyCallInfoWrapper::remotePartyIdentifier() const
{
    return d->remotePartyIdentifier();
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
    \fn  QString QTelephonyCallInfoWrapper::subTyp() const

    Gives back the sub type of the call type.
    A subtype of a Voip call can be Skype.
    You can use this property to get the sub type of the call.
*/
QString QTelephonyCallInfoWrapper::subTyp() const
{
    return d->subTyp();
}

/*!
    \fn  QVariant QTelephonyCallInfoWrapper::value(const QString& param) const
    \a param Parameter for reading additional values.

    Gives back a variant value.
    This function is for reading special values dependce on the param parameter.
*/
QVariant QTelephonyCallInfoWrapper::value(const QString& param) const
{
    return d->value(param);
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
    : QObject(0)
{
    d = new QTelephonyCallList(this);
    connect(d, SIGNAL(activeCallStatusChanged(const QTelephonyCallInfo))
        , this , SIGNAL(activeCallStatusChanged(const QTelephonyCallInfo)));
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

