/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdeclarativetelephony.h"

/*!
    \class QDeclarativeTelephonyCallInfo
    \ingroup telephonyapi
    \brief The QDeclarativeTelephonyCallInfo class for a QTelephonyCallInfo object. This wrapper class can be used in QML to work with telephony.
*/

/*!
    \enum QDeclarativeTelephonyCallInfo::CallStatus

    This enum type is used to describe the status of a call.
    A QDeclarativeTelephonyCallInfo object can have a status of:

    \value Idle          The call status is not defined.
    \value Dialing       The status of the call is dialing.
    \value Alerting      The status of the call is alerting.
    \value Connected     The status of the call is connected.
    \value Disconnecting The status of the call is disconnected.
    \value Incoming     The status of the call is incoming.
    \value OnHold        The status of the call is on hold.
*/

/*!
    \enum QDeclarativeTelephonyCallInfo::CallType

    This enum decribes the type of the call.
    A QDeclarativeTelephonyCallInfo object can be a type of:

    \value Any         The call type can be any type.
    \value Text        The call is a text base call.
    \value Data        The call is a data based call.
    \value Video       The call is a video based call.
    \value Voice       The call is a voice based call.
    \value Other       The call is a an unknown base call.
*/

/*!
    \enum QDeclarativeTelephonyCallInfo::Direction

    This enum decribes the direction of the call.
    A QTelephonyCallInfo object can have a direction of:

    \value Received    The call is received.
    \value Dialed      The call is dialed.
*/

/*!
    \fn  QDeclarativeTelephonyCallInfo::QDeclarativeTelephonyCallInfo()

    Constructor of a QDeclarativeTelephonyCallInfo object.
*/
QDeclarativeTelephonyCallInfo::QDeclarativeTelephonyCallInfo()
    : QObject(0)
{
    d = new QTelephonyCallInfo();
}

/*!
    \fn  QDeclarativeTelephonyCallInfo::QDeclarativeTelephonyCallInfo(const QTelephonyCallInfo& other)
    \a QTelephonyCallInfo Object which needs to be copied from.

    Constructor of a QDeclarativeTelephonyCallInfo object.
*/
QDeclarativeTelephonyCallInfo::QDeclarativeTelephonyCallInfo(const QTelephonyCallInfo& other)
    : QObject(0)
{
    d = new QTelephonyCallInfo(other);
}

/*!
    \fn  QDeclarativeTelephonyCallInfo::QDeclarativeTelephonyCallInfo(const QDeclarativeTelephonyCallInfo& other)
    \a other Object which needs to be copied from.

    Copy constructor of a QDeclarativeTelephonyCallInfo object.
*/
QDeclarativeTelephonyCallInfo::QDeclarativeTelephonyCallInfo(const QDeclarativeTelephonyCallInfo& other)
    : QObject(0)
{
    d = new QTelephonyCallInfo(*other.d);
}

/*!
    \fn  QDeclarativeTelephonyCallInfo::~QDeclarativeTelephonyCallInfo()
    
    Destructor of a call info object.
*/
QDeclarativeTelephonyCallInfo::~QDeclarativeTelephonyCallInfo()
{
    delete d;
}

/*!
    \fn  QDeclarativeTelephonyCallInfo& QDeclarativeTelephonyCallInfo::operator=(const QDeclarativeTelephonyCallInfo& other)

    Assigns this relationship to be equal to \a other.
*/
QDeclarativeTelephonyCallInfo& QDeclarativeTelephonyCallInfo::operator=(const QDeclarativeTelephonyCallInfo& other)
{
    d = other.d;
    return *this;
}

/*!
    \property QDeclarativeTelephonyCallInfo::remotePartyIdentifier
    \brief the id of a call.

    The remotePartyIdentifier contains the value dependend on the call CallType.
    It can be the phone number, IP address or something else.
*/
QString QDeclarativeTelephonyCallInfo::remotePartyIdentifier() const
{
    return d->remotePartyIdentifier();
}

/*!
    \fn  unsigned int QDeclarativeTelephonyCallInfo::type() const

    Returns the types of the call.
    A call can be a VOIP, Video Voice and so on or a combination of different types.
    You can use this property to get the types of the call by masking with QTelephony::CallType.
*/
unsigned int QDeclarativeTelephonyCallInfo::type() const
{
    return d->type();
}

/*!
    \property QDeclarativeTelephonyCallInfo::status
    \brief the status of the call.

    A call can be in different conditions like ringing, dropped and so on.
    You can use this property if you want to know about the current condition of the call.
*/
QDeclarativeTelephonyCallInfo::CallStatus QDeclarativeTelephonyCallInfo::status() const
{
    return (QDeclarativeTelephonyCallInfo::CallStatus)d->status();
}

/*!
    \fn  QString QDeclarativeTelephonyCallInfo::subType() const

    Gives back the sub type of the call type.
    A subtype of a Voip call can be Skype.
    You can use this property to get the sub type of the call.
*/
QString QDeclarativeTelephonyCallInfo::subType() const
{
    return d->subType();
}

/*!
    \fn  QVariant QDeclarativeTelephonyCallInfo::value(const QString& param) const
    \a param Parameter for reading additional values.

    Gives back a variant value.
    This function is for reading special values dependce on the param parameter.
*/
QVariant QDeclarativeTelephonyCallInfo::value(const QString& param) const
{
    return d->value(param);
}

/*!
    \fn  QVariant QDeclarativeTelephonyCallInfo::direction() const

    Gives back the direction of a call.
    This function is for reading the direction of a call (received or dialed).
*/
QDeclarativeTelephonyCallInfo::Direction QDeclarativeTelephonyCallInfo::direction() const
{
    return (QDeclarativeTelephonyCallInfo::Direction)d->direction();
}


/*!
    \class QDeclarativeTelephonyCallList
    \ingroup telephonyapi
    \brief The QDeclarativeTelephonyCallList class for a QTelephonyCallList object. This wrapper class can be used in QML to work with telephony.
*/

/*!
    \fn QDeclarativeTelephonyCallList::QDeclarativeTelephonyCallList(QObject *parent)
    \a parent The parent of this object. Default is 0.

    Constructor for the QDeclarativeTelephonyCallList object 
*/
QDeclarativeTelephonyCallList::QDeclarativeTelephonyCallList()
    : QObject(0)
{
    qDebug() << "QDeclarativeTelephonyCallList::QDeclarativeTelephonyCallList()";
    d = new QTelephonyCallList(this);

    connect(d, SIGNAL(activeCallStatusChanged(const QTelephonyCallInfo&))
        , this , SLOT(onCallStatusChanged(const QTelephonyCallInfo&)));
    connect(d, SIGNAL(activeCallRemoved(const QTelephonyCallInfo&))
        , this, SLOT(onCallRemoved(const QTelephonyCallInfo&)));
    connect(d, SIGNAL(activeCallAdded(const QTelephonyCallInfo&))
        , this, SLOT(onCallAdded(const QTelephonyCallInfo&)));
    connect(d, SIGNAL(activeCallCountChanged(const int&))
        , this, SLOT(onCallStatusChanged(QTelephonyCallInfo)));
}

/*!
    \fn QDeclarativeTelephonyCallList::~QDeclarativeTelephonyCallList()

    Destructor of QDeclarativeTelephonyCallList.
*/
QDeclarativeTelephonyCallList::~QDeclarativeTelephonyCallList()
{
    tmpcallinfolist.clear();
    delete d;
}

/*!
    \fn QList<QDeclarativeTelephonyCallList> QDeclarativeTelephonyCallList::activeCalls(const CallType& calltype) const
    \a calltype All calls in the list have this type.

    Gives back a list of calls from type of calltype.
*/
QList<QDeclarativeTelephonyCallInfo*> QDeclarativeTelephonyCallList::activeCalls(const QDeclarativeTelephonyCallInfo::CallType& calltype)
{
    tmpcallinfolist.clear();
    QList<QtMobility::QTelephonyCallInfo> calllist;
    if(d)
        calllist = d->activeCalls((QTelephony::CallType)calltype);

    //call copy constructor so the caller has to delete the QTelephonyCallInfo pointers
    for( int i = 0; i < calllist.count(); i++){
        tmpcallinfolist.push_back(QDeclarativeTelephonyCallInfo(calllist.at(i)));
    }
    //create list of pointer
    QList< QDeclarativeTelephonyCallInfo* > ret;
    for( int i = 0; i < tmpcallinfolist.count(); i++){
        ret.push_back(&tmpcallinfolist[i]);
    }
    return ret;
}

/*!
    \fn int QDeclarativeTelephonyCallList::activeCallCount() const

    Returns the number of current active calls.
*/
int QDeclarativeTelephonyCallList::activeCallCount() const
{
    if(d)
        return d->activeCallCount();
    return 0;
}

/*! 
    \fn void QDeclarativeTelephonyCallList::activeCallStatusChanged(const QTelephonyCallInfo& call);
    \a call The QTelephonyCallInfo object that changed its status.

    This Slot is called whenever a call changed its status.
*/
void QDeclarativeTelephonyCallList::onCallStatusChanged(const QTelephonyCallInfo& call)
{
    qDebug() << "QDeclarativeTelephonyCallList::onActiveCallStatusChanged(...)";
    QDeclarativeTelephonyCallInfo tciw(call);
    emit activeCallStatusChanged(&tciw);
}

/*! 
    \fn void QDeclarativeTelephonyCallList::activeCallRemoved(const QTelephonyCallInfo& call);
    \a call The QTelephonyCallInfo object that was removed from the active call list.

    This Slot is called whenever a call was removed from the active call list of QTelephonyCallList.
*/
void QDeclarativeTelephonyCallList::onCallRemoved(const QTelephonyCallInfo& call)
{
    qDebug() << "QDeclarativeTelephonyCallList::onActiveCallRemoved(...)";
    QDeclarativeTelephonyCallInfo tciw(call);
    emit activeCallRemoved(&tciw);
}

/*! 
    \fn void QDeclarativeTelephonyCallList::activeCallAdded(const QTelephonyCallInfo& call);
    \a call The QTelephonyCallInfo object that was added to the active call list.

    This Slot is called whenever a call was added to the active call list of QTelephonyCallList.
*/
void QDeclarativeTelephonyCallList::onCallAdded(const QTelephonyCallInfo& call)
{
    qDebug() << "QDeclarativeTelephonyCallList::onCallAdded(...)";
    QDeclarativeTelephonyCallInfo tciw(call);
    emit activeCallAdded(&tciw);
}

/*!
    \fn void QDeclarativeTelephonyCallList::onCallCountChanged(const int& value);
    \a count The current count of active calls.

    This Slot is called whenever the count of active calls was changed.
*/
void QDeclarativeTelephonyCallList::onCallCountChanged(const int& value)
{
    emit activeCallCountChanged(value);
}

