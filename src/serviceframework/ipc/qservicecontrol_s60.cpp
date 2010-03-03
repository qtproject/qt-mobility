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

#include "qservicecontrol_s60_p.h"
#include "ipcendpoint_p.h"
#include "objectendpoint_p.h"

/* IPC based on Symbian Client-Server framework
 * This module implements the Symbian specific IPC mechanisms and related control.
 * IPC is based on Symbian Client-Server architecture.
 */

QTM_BEGIN_NAMESPACE

class SymbianClientServerEndPoint: public QServiceIpcEndPoint
{
    Q_OBJECT
public:
    SymbianClientServerEndPoint(QObject* parent = 0)
        : QServiceIpcEndPoint(parent)
    {
    qDebug() << "Symbian IPC endpoint created.";
    }

    ~SymbianClientServerEndPoint()
    {
    qDebug() << "Symbian IPC endpoint destroyed.";
    }
};


QServiceControlPrivate::QServiceControlPrivate(QObject *parent)
    : QObject(parent)
{
}

void QServiceControlPrivate::publishServices(const QString &ident)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "QSCP::publishServices for ident: " << ident;
#endif
}

QObject* QServiceControlPrivate::proxyForService(const QServiceTypeIdent &typeId, const QString &location)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "QSCP::proxyForService for location: " << location;
#endif
    // location format: protocol:address
    RServiceSession *session = new RServiceSession(location.section(':', 1, 1));
    // zzz
}



RServiceSession::RServiceSession(QString address)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "RServiceSession() for address: " << address;
#endif
}

void RServiceSession::Close()
{
    qDebug() << "RServiceSession close()";
}

TInt RServiceSession::Connect()
{
    qDebug() << "RServiceSession connect()";
}

TVersion RServiceSession::Version() const
{
    qDebug() << "RServiceSession Version()";
}


#include "moc_qservicecontrol_s60_p.cpp"
// #include "qservicecontrol_p.moc"
QTM_END_NAMESPACE
