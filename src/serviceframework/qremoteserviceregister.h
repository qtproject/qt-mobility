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

#ifndef QREMOTESERVICEREGISTER_H
#define QREMOTESERVICEREGISTER_H

#include "qmobilityglobal.h"
#include <QObject>
#include <QQueue>
#include <QHash>
#include <QDebug>


QTM_BEGIN_NAMESPACE

class QRemoteServiceRegisterPrivate;
class Q_SERVICEFW_EXPORT QRemoteServiceRegister : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool quitOnLastInstanceClosed READ quitOnLastInstanceClosed WRITE setQuitOnLastInstanceClosed)
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
        void setInstanciationType(QRemoteServiceRegister::InstanceType t);
        QRemoteServiceRegister::InstanceType instanciationType() const;


    private:
        //TODO do we need a private d-pointer?
        QString iface;
        QString service;
        QString ifaceVersion;
        const QMetaObject* meta;
        QRemoteServiceRegister::CreateServiceFunc cptr;
        QRemoteServiceRegister::InstanceType instanceType;


        friend class QRemoteServiceRegister;
        friend class InstanceManager;
        friend class QServiceManager;
#ifndef QT_NO_DATASTREAM
        friend Q_SERVICEFW_EXPORT QDataStream &operator<<(QDataStream &, const QRemoteServiceRegister::Entry &);
        friend Q_SERVICEFW_EXPORT QDataStream &operator>>(QDataStream &, QRemoteServiceRegister::Entry &);
#endif
    };


    QRemoteServiceRegister(QObject* parent = 0);
    ~QRemoteServiceRegister();

    template <typename T>
    Entry createEntry(const QString& serviceName,
                    const QString& interfaceName, const QString& version);
    void registerEntry(const Entry& entry);


    void publishEntries(const QString& ident );

    bool quitOnLastInstanceClosed() const;
    void setQuitOnLastInstanceClosed(const bool quit);

    typedef bool (*securityFilter)(const void *message);
    securityFilter setSecurityFilter(securityFilter filter);

Q_SIGNALS:
    void lastInstanceClosed();
private:
    QRemoteServiceRegisterPrivate* d;
};

struct QRemoteServiceRegisterLocalSocketCred {
    int fd;
    int pid;
    int uid;
    int gid;
};


inline uint qHash(const QRemoteServiceRegister::Entry& e) {
    //Only consider version, iface and service name -> needs to sync with operator==
    return ( qHash(e.serviceName()) + qHash(e.interfaceName()) + qHash(e.version()) );
}

#ifndef QT_NO_DATASTREAM
inline QDataStream& operator>>(QDataStream& s, QRemoteServiceRegister::Entry& entry) {
    //for now we only serialize version, iface and service name
    //neds to sync with qHash and operator==
    s >> entry.service >> entry.iface >> entry.ifaceVersion;
    return s;
}

inline QDataStream& operator<<(QDataStream& s, const QRemoteServiceRegister::Entry& entry) {
    //for now we only serialize version, iface and service name
    //neds to sync with qHash and operator==
    s << entry.service << entry.iface << entry.ifaceVersion;
    return s;
}
#endif

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator<<(QDebug dbg, const QRemoteServiceRegister::Entry& entry) {
    dbg.nospace() << "QRemoteServiceRegister::Entry("
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
QRemoteServiceRegister::Entry QRemoteServiceRegister::createEntry(const QString& serviceName, const QString& interfaceName, const QString& version)
{
    if (serviceName.isEmpty()
            || interfaceName.isEmpty()
            || version.isEmpty() ) {
        qWarning() << "QRemoteServiceRegister::registerService: service name, interface name and version must be specified";
        return Entry();
    }

    QRemoteServiceRegister::CreateServiceFunc cptr = qServiceTypeConstructHelper<T>;

    Entry e;
    e.service = serviceName;
    e.iface = interfaceName;
    e.ifaceVersion = version;
    e.cptr = cptr;
    e.meta = &T::staticMetaObject;

    return e;
}


QTM_END_NAMESPACE
#endif //QREMOTESERVICEREGISTER_H
