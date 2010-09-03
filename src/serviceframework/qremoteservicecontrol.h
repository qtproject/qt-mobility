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
#include <QObject>
#include <QQueue>
#include <QHash>
#include <QDebug>


QTM_BEGIN_NAMESPACE

class QRemoteServiceControlPrivate;
class Q_SERVICEFW_EXPORT QRemoteServiceControl : public QObject
{
    Q_OBJECT
public:

    enum InstanceType {
        SharedInstance = 0,  //every new request for service gets same service instance
        UniqueInstance       //every new request for service gets new service instance
    };

    typedef QObject *(*CreateServiceFunc)();

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
        void setInstanciationType(QRemoteServiceControl::InstanceType t);
        QRemoteServiceControl::InstanceType instanciationType() const;


    private:
        //TODO do we need a private d-pointer?
        QString iface;
        QString service;
        QString ifaceVersion;
        const QMetaObject* meta;
        QRemoteServiceControl::CreateServiceFunc cptr;
        QRemoteServiceControl::InstanceType instanceType;


        friend class QRemoteServiceControl;
        friend class InstanceManager;
        friend class QServiceManager;
#ifndef QT_NO_DATASTREAM
        friend Q_SERVICEFW_EXPORT QDataStream &operator<<(QDataStream &, const QRemoteServiceControl::Entry &);
        friend Q_SERVICEFW_EXPORT QDataStream &operator>>(QDataStream &, QRemoteServiceControl::Entry &);
#endif
    };


    QRemoteServiceControl(QObject* parent = 0);
    ~QRemoteServiceControl();

    template <typename T>
    Entry createServiceEntry(const QString& serviceName,
                    const QString& interfaceName, const QString& version);
    void registerService(const Entry& entry);


    void publishServices(const QString& ident );

private:
    QRemoteServiceControlPrivate* d;
};

inline uint qHash(const QRemoteServiceControl::Entry& e) {
    //Only consider version, iface and service name -> needs to sync with operator==
    return ( qHash(e.serviceName()) + qHash(e.interfaceName()) + qHash(e.version()) );
}

#ifndef QT_NO_DATASTREAM
inline QDataStream& operator>>(QDataStream& s, QRemoteServiceControl::Entry& entry) {
    //for now we only serialize version, iface and service name
    //neds to sync with qHash and operator==
    s >> entry.service >> entry.iface >> entry.ifaceVersion;
    return s;
}

inline QDataStream& operator<<(QDataStream& s, const QRemoteServiceControl::Entry& entry) {
    //for now we only serialize version, iface and service name
    //neds to sync with qHash and operator==
    s << entry.service << entry.iface << entry.ifaceVersion;
    return s;
}
#endif

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator<<(QDebug dbg, const QRemoteServiceControl::Entry& entry) {
    dbg.nospace() << "QRemoteServiceControl::Entry("
                  << entry.serviceName() << ", "
                  << entry.interfaceName() << ", "
                  << entry.version() << ")";
    return dbg.space();
}
#endif

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

    QRemoteServiceControl::CreateServiceFunc cptr = qServiceTypeConstructHelper<T>;

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
