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

#include <QtCore/qshareddata.h>
#include <QDebug>
#include <QtCore/QPair>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include "maemo/qtelephonycalllist_maemo_p.h"
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
# include "maemo/qtelephonycallinfo_maemo_p.h"
#else
# include "qtelephonycallinfo_p.h"
#endif
#include "maemo/cli-constants.h"
#include "maemo/types.h"
#include "maemo/connection.h"
#include "maemo/channel.h"

QT_USE_NAMESPACE

QTM_BEGIN_NAMESPACE

QTelephonyCallListPrivate::QTelephonyCallListPrivate(QTelephonyCallList *parent)
    : p(parent)
{
    Tp::registerTypes();
    qDebug() << "QTelephonyCallListPrivate::QTelephonyCallListPrivate(QTelephonyCallList *parent)";
    //now Create Connection Request interface
    QString path = "/org/freedesktop/Telepathy/Connection/ring/tel/ring";
    connection = ConnectionPtr(new Connection(QDBusConnection::sessionBus(), path.mid(1).replace(QLatin1String("/"), QLatin1String(".")), path, this));
}

QTelephonyCallListPrivate::~QTelephonyCallListPrivate()
{
    qDebug() << "QTelephonyCallListPrivate::~QTelephonyCallListPrivate() for maemo";
    callInfoList.clear();
}

QList<QTelephonyCallInfo> QTelephonyCallListPrivate::activeCalls(const QTelephonyCallInfo::CallType& calltype) const 
{ 
    QList<QTelephonyCallInfo> e; 
    return e; 
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

void QTelephonyCallListPrivate::emitActiveCallAdded(QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>& call)
{
    qDebug() << "QTelephonyCallListPrivate::emitActiveCallAdded(QTelephonyCallInfoPrivate& call)";
    QTelephonyCallInfo callinfo;
    callinfo.d = call;
    emit p->activeCallAdded(callinfo);
}

QTelephonyCallInfo::CallType QTelephonyCallListPrivate::convertToCallType(QString channeltelephatytype)
{
    if(channeltelephatytype == "StreamedMedia")
        return QTelephonyCallInfo::Voice;
    if(channeltelephatytype == "Text")
        return QTelephonyCallInfo::Text;

    return QTelephonyCallInfo::Other;
}

void QTelephonyCallListPrivate::newChannels(Tp::ChannelPtr channelptr)
{
    QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate> callinfo = QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(new QTelephonyCallInfoPrivate(channelptr));
    callInfoList.append(callinfo);
    emitActiveCallAdded(callinfo);
}

void QTelephonyCallListPrivate::channelStatusChanged(Tp::ChannelPtr channel)
{
    int index = 0;
    for(index = 0; index < callInfoList.count(); index++)
    {
        if(callInfoList[index]->telepathychannel.data() == channel.data())
            break;
    }
    if(index < callInfoList.count()){
        QTelephonyCallInfo callinfo;
        callinfo.d = callInfoList[index];
        emit p->activeCallStatusChanged(callinfo);

//        emit emitActiveCallStatusChanged(*found->data());
        //check if channel must be removed from callist
        if(callInfoList[index]->status() == QTelephonyCallInfo::Disconnecting)
            callInfoList.removeAt(index);
    }
}

#include "moc_qtelephonycalllist_maemo_p.cpp"

QTM_END_NAMESPACE
