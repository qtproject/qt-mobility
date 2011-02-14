/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
***********************************/
#ifndef QDECLARATIVENETWORKINFO_H
#define QDECLARATIVENETWORKINFO_H

#include <QObject>
#include "qsystemnetworkinfo.h"

QT_BEGIN_HEADER
QTM_USE_NAMESPACE

class QDeclarativeNetworkInfo : public QSystemNetworkInfo
{
    Q_OBJECT
    Q_PROPERTY(QString networkStatus READ networkStatus NOTIFY statusChanged)
    Q_PROPERTY(QString networkName READ networkName NOTIFY nameChanged)
    Q_PROPERTY(int networkSignalStrength READ networkSignalStrength NOTIFY signalStrengthChanged)
    Q_PROPERTY(QString macAddress READ macAddress CONSTANT)
    Q_PROPERTY(QSystemNetworkInfo::NetworkMode useMode READ useMode WRITE useThisMode)
    Q_PROPERTY(QList<QSystemNetworkInfo::NetworkMode> availableModes READ availableModes)

    Q_PROPERTY(bool monitorStatusChanges READ monitorStatusChanges WRITE startStatusChanged CONSTANT)
    Q_PROPERTY(bool monitorSignalStrengthChanges READ monitorSignalStrengthChanges WRITE startSignalStrengthChanged CONSTANT)
    Q_PROPERTY(bool monitorNameChanges READ monitorNameChanges WRITE startNameChanged CONSTANT)
    Q_PROPERTY(bool monitorModeChanges READ monitorModeChanges WRITE startModeChanged CONSTANT)
    Q_PROPERTY(bool monitorCurrentMobileCountryCodeChanges READ monitorCurrentMobileCountryCodeChanges WRITE startCurrentMobileCountryCodeChanged CONSTANT)
    Q_PROPERTY(bool monitorCurrentMobileNetworkCodeChanges READ monitorCurrentMobileNetworkCodeChanges WRITE startCurrentMobileNetworkCodeChanged CONSTANT)

public:
    explicit QDeclarativeNetworkInfo(QObject *parent = 0);
    virtual ~QDeclarativeNetworkInfo();
    int networkSignalStrength();
    QString networkName();
    QString macAddress();
    QString networkStatus();
    QSystemNetworkInfo::NetworkMode useMode();

    bool monitorStatusChanges();
    void startStatusChanged(bool on);

    bool monitorSignalStrengthChanges();
    void startSignalStrengthChanged(bool on);

    bool monitorNameChanges();
    void startNameChanged(bool on);

    bool monitorModeChanges();
    void startModeChanged(bool on);

    bool monitorCurrentMobileCountryCodeChanges();
    void startCurrentMobileCountryCodeChanged(bool on);

    bool monitorCurrentMobileNetworkCodeChanges();
    void startCurrentMobileNetworkCodeChanged(bool on);

    void useThisMode(QSystemNetworkInfo::NetworkMode curmode);

public slots:

    QNetworkInterface interfaceForMode();
    QList<QSystemNetworkInfo::NetworkMode> availableModes();

Q_SIGNALS:
   void statusChanged(const QString &newStatus);
   void signalStrengthChanged(int strength);
   void nameChanged(const QString & name);
   void modeChanged();
   void currentMobileCountryCodeChanged(const QString &newMcc);
   void currentMobileNetworkCodeChanged(const QString &newMnc);

private:
    QSystemNetworkInfo::NetworkMode m_mode;
    QString statusToString(QSystemNetworkInfo::NetworkStatus);
    QString modeToString(QSystemNetworkInfo::NetworkMode);

    bool monitoringStatusChanges;
    bool monitoringSignalStrengthChanges;
    bool monitoringNameChanges;
    bool monitoringModeChanges;
    bool monitoringCurrentMobileCountryCodeChanges;
    bool monitoringCurrentMobileNetworkCodeChanges;

private slots:
    void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
    void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int);
    void networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &);
    void networkModeChanged(QSystemNetworkInfo::NetworkMode);
};


QT_END_NAMESPACE

//QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeNetworkInfo))

QT_END_HEADER
#endif // QDECLARATIVENETWORKINFO_H
