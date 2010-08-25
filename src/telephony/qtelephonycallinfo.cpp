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

#include <QtCore/qglobal.h>

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
# include "maemo/qtelephonycallinfo_maemo_p.h"
#else
# include "qtelephonycallinfo_p.h"
#endif

#include "qmobilityglobal.h"
#include "qtelephony.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QTelephonyCallInfo
    \ingroup telephonyapi
    \inmodule QtTelephony
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
    \fn  QString QTelephonyCallInfo::remotePartyIdentifier() const

    Gives back the id of a call.
    The remotePartyIdentifier contains the value dependend on the call CallType.
    It can be the phone number, IP address or something else.
*/
QString QTelephonyCallInfo::remotePartyIdentifier() const
{
    if(d)
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
        return d->remotePartyIdentifier();
#else
        return d->remotePartyIdentifier;
#endif
    return QString();
}

/*!
    \fn  QTelephony::CallType QTelephonyCallInfo::type() const

    Gives back the type of the call.
    A call can be a VOIP, Video Voice and so on.
    You can use this property to get the type of the call.
*/
QTelephony::CallType QTelephonyCallInfo::type() const
{
    if(d)
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
        return d->type();
#else
        return d->type;
#endif
    return Other;
}

/*!
    \fn  QString QTelephonyCallInfo::subType() const

    Gives back the sub type of the call type (see \l QTelephonyCallInfo::type()).
    A subtype of a Voip call can be Skype.
    You can use this property to get the sub type of the call .
*/
QString QTelephonyCallInfo::subType() const
{
    if(d)
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
        return d->subType();
#else
        return d->subType;
#endif
    return QString();
}

/*!
    \fn  QTelephony::CallStatus QTelephonyCallInfo::status() const

    Gives back the status of the call.
    A call can be in different conditions like ringing, dropped and so on.
    You can use this property if you want to know about the current condition of the call.
*/
QTelephony::CallStatus QTelephonyCallInfo::status() const
{
    if(d)
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
        return d->status();
#else
        return d->status;
#endif
    return Idle;
}

/*!
    \fn  QVariant QTelephonyCallInfo::value(const QString& key) const
    The \a key select the value in which the user is interrested to read.

    Gives back a variant value.
    This function is for reading additional values dependent on the key parameter.
*/
QVariant QTelephonyCallInfo::value(const QString& key) const
{
    if(d && d->values.contains(key)){
        return d->values.value(key);
    }
    return QVariant();
}

QTM_END_NAMESPACE
