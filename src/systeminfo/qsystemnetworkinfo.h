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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSYSTEMNETWORKINFO_H
#define QSYSTEMNETWORKINFO_H

#include "qmobilityglobal.h"

#include <QObject>
#include <QNetworkInterface>

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QSystemNetworkInfoPrivate;

class  Q_SYSINFO_EXPORT QSystemNetworkInfo : public QObject
{
    Q_OBJECT
    Q_ENUMS(NetworkStatus)
    Q_ENUMS(NetworkMode)
    Q_PROPERTY(int cellId READ cellId)
    Q_PROPERTY(int locationAreaCode READ locationAreaCode)
    Q_PROPERTY(QString currentMobileCountryCode READ currentMobileCountryCode NOTIFY currentMobileCountryCodeChanged)
    Q_PROPERTY(QString currentMobileNetworkCode READ currentMobileNetworkCode NOTIFY currentMobileNetworkCodeChanged)
    Q_PROPERTY(QString homeMobileCountryCode READ homeMobileCountryCode CONSTANT)
    Q_PROPERTY(QString homeMobileNetworkCode READ homeMobileNetworkCode CONSTANT)
    Q_PROPERTY(QSystemNetworkInfo::NetworkMode currentMode READ currentMode CONSTANT)

public:

    explicit QSystemNetworkInfo(QObject *parent = 0);
    ~QSystemNetworkInfo();

    enum NetworkStatus {
        UndefinedStatus = 0,
        NoNetworkAvailable,
        EmergencyOnly,
        Searching,
        Busy,
        Connected,
        HomeNetwork,
        Denied,
        Roaming
    };

    enum NetworkMode {
        UnknownMode=0,
        GsmMode,
        CdmaMode,
        WcdmaMode,
        WlanMode,
        EthernetMode,
        BluetoothMode,
        WimaxMode
    };

    Q_INVOKABLE QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);
    Q_INVOKABLE static int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    Q_INVOKABLE QString macAddress(QSystemNetworkInfo::NetworkMode mode);
    Q_INVOKABLE QSystemNetworkInfo::NetworkMode currentMode();

    int cellId();
    int locationAreaCode();

    QString currentMobileCountryCode();
    QString currentMobileNetworkCode();
    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();
    Q_INVOKABLE static QString networkName(QSystemNetworkInfo::NetworkMode mode);
    QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);


Q_SIGNALS:
   void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
   void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int);
   void currentMobileCountryCodeChanged(const QString &);
   void currentMobileNetworkCodeChanged(const QString &);
   void networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &);
   void networkModeChanged(QSystemNetworkInfo::NetworkMode);

protected:
    virtual void connectNotify(const char *signal);
    virtual void disconnectNotify(const char *signal);

private:
       QSystemNetworkInfoPrivate *d;

};


QTM_END_NAMESPACE

QT_END_HEADER

#endif // QSYSTEMNETWORKINFO_H
