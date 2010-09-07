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

#include <qmobilityglobal.h>
#include <QtCore/qshareddata.h>
#include <QDebug>
#include <QtCore/QPair>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>

#include "qtelephony.h"
#include "maemo/qtelephonycalllist_maemo_p.h"
#include "maemo/qtelephonycallinfo_maemo_p.h"
#include "maemo/constants.h"
#include "maemo/types.h"
#include "maemo/connection.h"
#include "maemo/channel.h"
#include "maemo/accountmanager.h"

QT_USE_NAMESPACE

QTM_BEGIN_NAMESPACE

QTelephonyCallListPrivate::QTelephonyCallListPrivate(QTelephonyCallList *parent)
    : p(parent)
    , accountManager(0)
{
    DBus::registerTypes();
    qDebug() << "QTelephonyCallListPrivate::QTelephonyCallListPrivate(QTelephonyCallList *parent) for maemo";

    qDebug() << "create Account Manager";
    accountManager = new AccountManager(QDBusConnection::sessionBus(), TELEPATHY_ACCOUNT_MANAGER_BUS_NAME, TELEPATHY_ACCOUNT_MANAGER_OBJECT_PATH, this);
}

QTelephonyCallListPrivate::~QTelephonyCallListPrivate()
{
    qDebug() << "QTelephonyCallListPrivate::~QTelephonyCallListPrivate() for maemo";
    callInfoList.clear();
    if(accountManager)
        delete accountManager;
}

int QTelephonyCallListPrivate::activeCallCount() const
{
    return callInfoList.count();
}

QList<QTelephonyCallInfo> QTelephonyCallListPrivate::activeCalls(const QTelephony::CallType& calltype) const 
{ 
    QList<QTelephonyCallInfo> ret;

    //call copy constructor so the caller has to delete the QTelephonyCallInfo pointers
    for( int i = 0; i < callInfoList.count(); i++){
        if(calltype == QTelephony::Any || callInfoList.at(i).data()->type() == calltype)
        {
            QTelephonyCallInfo callinfo;
            callinfo.d = callInfoList.at(i);
            ret.push_back(callinfo);
        }
    }
    return ret;
}

void QTelephonyCallListPrivate::emitActiveCallStatusChanged(QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>& call)
{
    if(p->enableCallStatusChangeNotify){
        QTelephonyCallInfo callinfo;
        callinfo.d = call;
        emit p->activeCallStatusChanged(callinfo);
    }
}

void QTelephonyCallListPrivate::emitActiveCallRemoved(QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>& call)
{
    if(p->enableCallRemovedNotify){
        QTelephonyCallInfo callinfo;
        callinfo.d = call;
        emit p->activeCallRemoved(callinfo);
    }
    if(p->enableHasActiveCallsNotify){
        if(callInfoList.count() == 0 && p->enableHasActiveCallsNotify)
            emit p->hasActiveCalls(false);
    }
}

void QTelephonyCallListPrivate::emitActiveCallAdded(QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>& call)
{
    if(p->enableCallAddedNotify){
        QTelephonyCallInfo callinfo;
        callinfo.d = call;
        emit p->activeCallAdded(callinfo);
    }
    if(callInfoList.count() == 1 && p->enableHasActiveCallsNotify)
        emit p->hasActiveCalls(true);
}

void QTelephonyCallListPrivate::newChannels(DBus::ChannelPtr channelptr)
{
    QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate> callinfo = QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(new QTelephonyCallInfoPrivate(channelptr));
    callInfoList.append(callinfo);
    emitActiveCallAdded(callinfo);
}

void QTelephonyCallListPrivate::channelStatusChanged(DBus::ChannelPtr channel)
{
    int found = -1;
    for(int index = 0; index < callInfoList.count(); index++)
    {
        if(callInfoList[index]->telepathychannel.data() == channel.data()){
            found = index;
            break;
        }
    }
    if(found >= 0 && found < callInfoList.count()){
        emitActiveCallStatusChanged(callInfoList[found]);
        //check if we need remove the call
        if(callInfoList[found]->status() == QTelephony::Disconnecting){
            QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate> callinfo = callInfoList[found];
            callInfoList.removeAt(found);
            emitActiveCallRemoved(callinfo);
        }
    }
}

#include "moc_qtelephonycalllist_maemo_p.cpp"

QTM_END_NAMESPACE
