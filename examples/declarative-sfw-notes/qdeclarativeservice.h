/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
***************************************************************************/

#ifndef QDECLARATIVESERVICE_H
#define QDECLARATIVESERVICE_H

#include <QtCore>
#include <qserviceinterfacedescriptor.h>
#include <qservicemanager.h>
#include <qdeclarative.h>
#include <qdeclarativelist.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QServiceInterfaceDescriptor)

class QServiceWrapper : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString interfaceName READ interfaceName WRITE setInterfaceName);
    Q_PROPERTY(QString serviceName READ serviceName);
    Q_PROPERTY(QString versionNumber READ versionNumber);
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged);
    Q_PROPERTY(QObject* serviceObject READ serviceObject NOTIFY serviceObjectChanged);

public:
    QServiceWrapper();
    ~QServiceWrapper();

    void setInterfaceDesc(const QServiceInterfaceDescriptor& desc);
    QServiceInterfaceDescriptor interfaceDesc() const;

    void setInterfaceName(const QString& interface);
    QString interfaceName() const;
    QString serviceName() const;
    QString versionNumber() const;

    bool isValid() const;
    QObject* serviceObject();

Q_SIGNALS:
    void validChanged();
    void serviceObjectChanged();

private:
    QServiceManager* serviceManager;

    QServiceInterfaceDescriptor m_descriptor;
    QObject* serviceInstance;
};

QML_DECLARE_TYPE(QServiceWrapper);

class QServiceListWrapper : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString interfaceName READ interfaceName WRITE setInterfaceName);
    Q_PROPERTY(QString minVersion READ minVersion WRITE setMinVersion);
    Q_PROPERTY(QDeclarativeListProperty<QServiceWrapper> services READ services NOTIFY servicesChanged);

public:
    QServiceListWrapper();
    ~QServiceListWrapper();

    QDeclarativeListProperty<QServiceWrapper> services();
    
    void setInterfaceName(const QString& interface);
    QString interfaceName() const;

    void setMinVersion(const QString& interface);
    QString minVersion() const;
    
private:
    QServiceManager* serviceManager;
    QList<QServiceWrapper *> m_services;
    QString m_interface;
    QString m_version;

Q_SIGNALS:
    void servicesChanged(const QDeclarativeListProperty<QServiceWrapper>&);
};

QML_DECLARE_TYPE(QServiceListWrapper)

#endif

