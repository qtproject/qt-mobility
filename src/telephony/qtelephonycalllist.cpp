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
#include "qtelephonycallinfo_p.h"


#if defined(Q_WS_MAEMO5)
# include "qtelephonycalllist_maemo_p.h"
#else
# ifdef Q_OS_LINUX
#  include "qtelephonycalllist_linux_p.h"
# endif
# ifdef Q_OS_WIN
#  include "qtelephonycalllist_win_p.h"
# endif
# ifdef Q_OS_SYMBIAN
#  include "qtelephonycalllist_symbian_p.h"
# endif
#endif

QTM_BEGIN_NAMESPACE

/*!
    \fn void QTelephonyCallList::activeCallStatusChanged(const QTelephonyCallInfo& call);
    \a call The QTelephonyCallInfo object that changed its status.

    This signal is emitted whenever the status of a active call was changed.
*/

/*!
    \fn void QTelephonyCallList::activeCallRemoved(const QTelephonyCallInfo& call);
    \a call The QTelephonyCallInfo object that was removed from the active call list.

    This signal is emitted whenever a call was removed from the active call list.
*/

/*!
    \fn void QTelephonyCallList::activeCallAdded(const QTelephonyCallInfo& call);
    \a call The QTelephonyCallInfo object that was added to the active call list.

    This signal is emitted whenever a call was added to the active call list.
*/

/*!
    \class QTelephonyCallList

    \ingroup telephony

    \brief The QTelephonyCallList class contains a list of active calls. This object can notify other applications about call changes.

    QTelephonyCallList supports calls event notification for other application and an access of the list of the
    existing calls.
*/

/*!
    \fn QTelephonyCallList::QTelephonyCallList(QObject *parent)
    \a parent The parent of this object. Default is 0.

    Constructor for the QTelephonyCallList object
*/
QTelephonyCallList::QTelephonyCallList(QObject *parent)
    : QObject(parent)
{
    d = new QTelephonyCallListPrivate(this);
}

/*!
    \fn QTelephonyCallList::~QTelephonyCallList()

    Destructor of QTelephonyCallList.
*/
QTelephonyCallList::~QTelephonyCallList()
{
    if(d)
        delete d;
}

/*!
    \fn QList<QTelephonyCallInfo> QTelephonyCallList::activeCalls(const QTelephonyCallInfo::CallType& calltype) const
    \a calltype All calls in the list have this type.

    Gives back a list of calls from type of calltype.
*/
QList<QTelephonyCallInfo> QTelephonyCallList::activeCalls(const QTelephonyCallInfo::CallType& calltype) const
{
    if(d)
        return d->activeCalls(calltype);
    return QList<QTelephonyCallInfo>();
}

#include "moc_qtelephonycalllist.cpp"

QTM_END_NAMESPACE
