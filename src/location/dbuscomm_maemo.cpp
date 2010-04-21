/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "dbuscomm_maemo_p.h"

#include <iostream>
using namespace std;

QTM_BEGIN_NAMESPACE

DBusComm::DBusComm()
{
    positioningdService   = QString("com.nokia.positioningd.client");
    positioningdPath      = QString("/com/nokia/positioningd/client");
    positioningdInterface = QString("com.nokia.positioningd.client");
}


int DBusComm::init()
{

    if (!QDBusConnection::sessionBus().isConnected()) {
        cerr << "Cannot connect to the D-BUS session bus." << endl;
        return -1;
    }

    // Application auto-start by dbus may take a while, so try
    // connecting a few times.

    int cnt = 3;
    do {
        cout << "Connecting to positioning daemon" << endl;
        positioningdProxy = new QDBusInterface(positioningdService,
                                               positioningdPath,
                                               positioningdInterface,
                                               QDBusConnection::sessionBus());
        usleep(500000);
        cnt--;
    } while (cnt && (positioningdProxy->isValid() == false));

    if (positioningdProxy->isValid() == false) {
        cerr << "DBus connection to positioning daemon failed." << endl;
        return -1;
    }

    createUniqueName();

    dbusServer = new DBusServer(&serverObj);
    dbusServer->setHandlerObject(this);

    QDBusConnection::sessionBus().registerObject(myPath, &serverObj);

    if (!QDBusConnection::sessionBus().registerService(myService)) {
        cerr << qPrintable(QDBusConnection::sessionBus().lastError().message()) << endl;
        return -1;
    }

    return 0;
}


int DBusComm::receiveDBusMessage(const QByteArray &message)
{
    emit receivedMessage(message);

    return 0;
}


int DBusComm::receivePositionUpdate(const QGeoPositionInfo &update)
{
    emit receivedPositionUpdate(update);

    return 0;
}


int DBusComm::receiveSettings(const QGeoPositionInfoSource::PositioningMethod methods,
                              const int interval)
{
    cout << "Interval confirmed to be :" << interval << "\n";
    // FIXME save these

    return 0;
}


bool DBusComm::sendDBusRegister()
{
    int n;
    QDBusReply<int> reply;

    reply = positioningdProxy->call("registerListener",
                                    myService.toAscii().constData(),
                                    myPath.toAscii().constData());
    if (reply.isValid()) {
        n = reply.value();
        clientId = n;
        cout << "Register client ID: " << n << endl;
    } else {
        cerr << endl << "DBus error:\n";
        cerr << reply.error().name().toAscii().constData() << endl;
        cerr << reply.error().message().toAscii().constData() << endl;
        return false;
    }

    return true;
}


int DBusComm::sessionConfigRequest(const int command, const int method,
                                   const int interval) const
{
    int n;
    QDBusReply<bool> reply;

    positioningdProxy->call("configSession", clientId, command, method, interval);
    cout << "sessionConfigRequest cmd: cmd:" << command << " method: ";
    cout << method << " interval: " << interval << "\n";
    if (reply.isValid()) {
        n = reply.value();
        cout << "sessionConfigRequest:Reply: " << n << endl;
    } else {
        cerr << endl << "DBus error:\n";
        cerr << reply.error().name().toAscii().constData() << endl;
        cerr << reply.error().message().toAscii().constData() << endl;
    }

    return 0;
}


void DBusComm::createUniqueName()
{
    QFile uuidfile("/proc/sys/kernel/random/uuid");
    if (!uuidfile.open(QIODevice::ReadOnly)) {
        cerr << "UUID file failed.";
        exit(0);
    }

    QTextStream in(&uuidfile);
    QString uuid = 'I' + in.readLine();
    uuid.replace('-', 'I');
    myService   = "com.nokia.qlocation." + uuid;
    myPath      = "/com/nokia/qlocation/" + uuid;
    myInterface = "com.nokia.qlocation.updates";

}

#include "moc_dbuscomm_maemo_p.cpp"
QTM_END_NAMESPACE

