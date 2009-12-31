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

#ifndef DBUSSERVER_H
#define DBUSSERVER_H

#include <QtDBus/QDBusAbstractAdaptor>
#include "dbuscomm.h"

#include "qgeopositioninfo.h"
#include "qgeopositioninfosource.h"

class DBusComm;
class DBusServer: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.nokia.qlocation.updates")

public:
    DBusServer(QObject *obj);
    void setHandlerObject(DBusComm* p);

private:
    DBusComm* handler;

public slots:
    Q_NOREPLY void positionUpdate(const QByteArray &update);
    Q_NOREPLY void currentSettings(const QGeoPositionInfoSource::PositioningMethod methods,
                                   const int interval);
};

#endif
