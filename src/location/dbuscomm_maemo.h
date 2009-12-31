/*
 * This file is part of libqtlocation
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: Martti Virtanen <martti.e.virtanen@nokia.com>
 *
 * This software, including documentation, is protected by copyright
 * controlled by Nokia Corporation. All rights are reserved. Copying,
 * including reproducing, storing, adapting or translating, any or all of
 * this material requires the prior written consent of Nokia Corporation.
 * This material also contains confidential information which may not be
 * disclosed to others without the prior written consent of Nokia.
 */

/*
 * This file is part of libqtlocation
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: Martti Virtanen <martti.e.virtanen@nokia.com>
 *
 * This software, including documentation, is protected by copyright
 * controlled by Nokia Corporation. All rights are reserved. Copying,
 * including reproducing, storing, adapting or translating, any or all of
 * this material requires the prior written consent of Nokia Corporation.
 * This material also contains confidential information which may not be
 * disclosed to others without the prior written consent of Nokia.
 */

#ifndef DBUSCOMM_H
#define DBUSCOMM_H

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>

#include "qgeopositioninfo.h"
#include "qgeopositioninfosource.h"
#include "dbusserver.h"

class DBusServer;
class DBusComm: public QObject
{
    Q_OBJECT

private:
    QDBusInterface *positioningdProxy;
    DBusServer* dbusServer;
    QObject serverObj;

    QString positioningdService;
    QString positioningdPath;
    QString positioningdInterface;
    QString myService;
    QString myPath;
    QString myInterface;
    int     clientId;

    void createUniqueName();

public:
    enum cmds {CmdStart = 1, CmdStop = 2, CmdOneShot = 3, CmdSetMethods = 4, CmdSetInterval = 8};

    DBusComm();
    int  init();
    bool sendDBusRegister();
    int  sessionConfigRequest(const int command, const int method,
                              const int interval) const;
    int receiveDBusMessage(const QByteArray &message); // called by D-Bus server
    int receivePositionUpdate(const QGeoPositionInfo &update); // called by D-Bus server
    int receiveSettings(const QGeoPositionInfoSource::PositioningMethod methods,
                        const int interval);
signals:
    void receivedMessage(const QByteArray &msg);
    void receivedPositionUpdate(const QGeoPositionInfo &update);
};

#endif
