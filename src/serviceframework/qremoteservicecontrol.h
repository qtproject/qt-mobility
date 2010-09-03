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

#ifndef QREMOTESERVICECONTROL_H
#define QREMOTESERVICECONTROL_H

#include "qmobilityglobal.h"
#include "qremoteserviceclassregister.h"
#include <QObject>
#include <QQueue>


QTM_BEGIN_NAMESPACE

class QRemoteServiceControlPrivate;
class Q_SERVICEFW_EXPORT QRemoteServiceControl : public QObject
{
    Q_OBJECT
public:

    enum InstanceType {
        SharedInstanceT = 0,  //every new request for service gets same service instance
        UniqueInstanceT       //every new request for service gets new service instance
    };

    class Q_SERVICEFW_EXPORT Entry {
    public:
        Entry();
        Entry(const Entry &);
        Entry &operator=(const Entry &);

        bool operator==(const Entry &) const;
        bool operator!=(const Entry &) const;

        bool isValid() const;

        QString interfaceName() const;
        QString serviceName() const;
        QString version() const;

        const QMetaObject* metaObject() const;
        void setInstanciationType(QRemoteServiceClassRegister::InstanceType t);
        QRemoteServiceClassRegister::InstanceType instaciationType() const;


    private:
        //TODO do we need a private d-pointer?
        QString iface;
        QString service;
        QString ifaceVersion;
        const QMetaObject* meta;
        QRemoteServiceClassRegister::CreateServiceFunc cptr;
        QRemoteServiceClassRegister::InstanceType instanceType;


        friend class QRemoteServiceControl;
    };


    QRemoteServiceControl(QObject* parent = 0);
    ~QRemoteServiceControl();

    template <typename T>
        Entry createServiceEntry(const QString& serviceName, const QString& interfaceName, const QString& version);
    void registerService(const Entry& entry);


    void publishServices(const QString& ident ); //TODO To be removed
    void publishServices();

private:
    void registerServiceHelper(const QMetaObject* meta,
                QRemoteServiceClassRegister::CreateServiceFunc func,
                const QRemoteServiceIdentifier& identifier,
                QRemoteServiceClassRegister::InstanceType type);



    QRemoteServiceControlPrivate* d;
};

template <typename T>
QObject* qServiceTypeConstructHelper()
{
    return new T;
}


template <typename T>
QRemoteServiceControl::Entry QRemoteServiceControl::createServiceEntry(const QString& serviceName, const QString& interfaceName, const QString& version)
{
    if (serviceName.isEmpty()
            || interfaceName.isEmpty()
            || version.isEmpty() ) {
        qWarning() << "QRemoteServiceControl::registerService: service name, interface name and version must be specified";
        return Entry();
    }

    QRemoteServiceClassRegister::CreateServiceFunc cptr = qServiceTypeConstructHelper<T>;

    Entry e;
    e.service = serviceName;
    e.iface = interfaceName;
    e.ifaceVersion = version;
    e.cptr = cptr;
    e.meta = &T::staticMetaObject;

    return e;
}


QTM_END_NAMESPACE
#endif //QREMOTESERVICECONTROL_H
