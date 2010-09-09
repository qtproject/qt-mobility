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

#include "qtelephonycalllist_linux_p.h"
#include "qtelephonycallinfo_p.h"
#include "telepathylistener_p.h"
#include "message_p.h"
#include <QtCore/qshareddata.h>
#include <QDebug>

QT_USE_NAMESPACE

QTM_BEGIN_NAMESPACE

////////
QTelephonyCallListPrivate::QTelephonyCallListPrivate(QTelephonyCallList *parent)
    : QObject(parent)
    , p(parent)
    , ptelepathyListener(0)
{
    qDebug() << "QTelephonyCallListPrivate::QTelephonyCallListPrivate(...) for linux";
    ptelepathyListener = new TelepathyListener(this);
    connect(ptelepathyListener
            , SIGNAL(NewChannels(ChannelsArray))
            , SLOT(newChannelsSlot(ChannelsArray)));
    connect(ptelepathyListener
            , SIGNAL(StatusChangedSlot(QString))
            , SLOT(statusChangedSlot(QString)));
}

QTelephonyCallListPrivate::~QTelephonyCallListPrivate()
{
    qDebug() << "QTelephonyCallListPrivate::~QTelephonyCallListPrivate() for linux";
    if(ptelepathyListener)
        delete ptelepathyListener;
}

void QTelephonyCallListPrivate::emitActiveCallStatusChanged(QTelephonyCallInfoPrivate& call)
{
    QTelephonyCallInfo callinfo;
    callinfo.d = QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(&call);
    emit p->activeCallStatusChanged(callinfo);
}

void QTelephonyCallListPrivate::emitActiveCallRemoved(QTelephonyCallInfoPrivate& call)
{
    QTelephonyCallInfo callinfo;
    callinfo.d = QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(&call);
    emit p->activeCallRemoved(callinfo);
}

void QTelephonyCallListPrivate::emitActiveCallAdded(QTelephonyCallInfoPrivate& call)
{
    QTelephonyCallInfo callinfo;
    callinfo.d = QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(&call);
    emit p->activeCallAdded(callinfo);
    if(callInfoList.count() == 1)
        emit p->hasActiveCalls(true);
}

void QTelephonyCallListPrivate::newChannelsSlot(const ChannelsArray& channelsarray)
{
    //create a QTelephonyCallInfoPrivate
    if(channelsarray.channelslist.count() > 0){
        Channels chs = channelsarray.channelslist.at(0);
        if(chs.map.contains("org.freedesktop.Telepathy.Channel.InitiatorID")){
            QVariant var = chs.map.value("org.freedesktop.Telepathy.Channel.InitiatorID");
            QTelephonyCallInfoPrivate* cip = new QTelephonyCallInfoPrivate();
            cip->remotePartyIdentifier = var.toString();
            cip->type = QTelephony::Voice;
            cip->direction = QTelephony::Received;
            callInfoList.append(QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(cip));
            emitActiveCallAdded(*cip);
        }
    }
}

void QTelephonyCallListPrivate::statusChangedSlot(const QString& status)
{
    qDebug() << "TelephonyCallListPrivate::statusChangedSlot";

    QTelephony::CallStatus newstatus = QTelephony::Idle;
    if(status == "IDLE")
        newstatus = QTelephony::Idle;
    else if(status == "Dialing"){
        newstatus = QTelephony::Dialing;
    }
    else if(status == "Alerting"){
        newstatus = QTelephony::Alerting;
    }
    else if(status == "Connected"){
        newstatus = QTelephony::Connected;
    }
    else if(status == "Disconnecting"){
        newstatus = QTelephony::Disconnecting;
    }

    for(int i = 0; i < callInfoList.count(); i++){
        callInfoList[i]->status = newstatus;
        emit emitActiveCallStatusChanged(*callInfoList[i].data());
    }

    // check if we need to remove the calls
    if(newstatus == QTelephony::Disconnecting){
        for(int i = 0; i < callInfoList.count(); i++){
            emit emitActiveCallRemoved(*callInfoList[i].data());
        }
        callInfoList.clear();
        emit p->hasActiveCalls(false);
    }
}

QList<QTelephonyCallInfo> QTelephonyCallListPrivate::activeCalls(const QTelephony::CallType& calltype) const
{
    QList<QTelephonyCallInfo> ret;

    //call copy constructor so the caller has to delete the QTelephonyCallInfo pointers
    for( int i = 0; i < callInfoList.count(); i++){
        if(callInfoList.at(i).data()->type == QTelephony::Any
            || callInfoList.at(i).data()->type == calltype)
        {
            QTelephonyCallInfo callinfo;
            callinfo.d = callInfoList.at(i);
            ret.push_back(callinfo);
        }
    }
    return ret;
}

int QTelephonyCallListPrivate::activeCallCount() const
{
    return callInfoList.count();
}

#include "moc_qtelephonycalllist_linux_p.cpp"

QTM_END_NAMESPACE
