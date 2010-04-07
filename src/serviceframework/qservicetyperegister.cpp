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

#include "qservicetyperegister.h"
#include "instancemanager_p.h"

#include <QMutexLocker>
#include <QMetaObject>
#include <QMetaClassInfo>
#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \typedef QServiceTypeIdent
    \relates QServiceTypeRegister

    Qt-style synonym for QPair<QByteArray, QByteArray>.
*/

/*!
    \class QServiceTypeRegister
    \ingroup servicefw
    \brief The QServiceTypeRegister class manages the various remote service classes
    which can be instanciated.

*/

/*!
    \enum QServiceTypeRegister::InstanceType

    Defines the possible service instanciation methodologies.

    \value SharedInstance Each new request for this service receives a reference to the same service object instance. This can be compared with a singleton pattern.
    \value UniqueInstance A new service object instance is created for each new client requesting a reference to the service.
*/

/*!
    \fn bool QServiceTypeRegister::registerType(QServiceTypeRegister::InstanceType type)

    This template function registers the provided template class as remote service and returns true if the registration
    was successfull. The \a type specifies the method used for service class instanciation. 

    \code
        QServiceTypeRegister::registerType<ServiceClass>();
    \endcode

    Every service class must be marked via the \l Q_SERVICE() macro.
*/

/*!
    \macro Q_SERVICE(T, interface, version)
    \relates QServiceTypeRegister

    This macro marks a class as remote service and should be placed next to the Q_OBJECT
    macro. \a T represents the name of the class implementing the service, \a interface
    specifies the service framework interface name and \a version represents the interface version. Version
    and interface parameter must match the interface information provided by the associated
    service XML file.

    \code
        class ServiceClass : public QObject
        {
            Q_OBJECT
            Q_SERVICE(ServiceClass, "com.nokia.qt.interfacename", "1.2");
        public:
            ServiceClass(QObject* parent) : QObject(parent) {};
        public slots:
            void serviceCall();
        };
    \endcode

*/

/*!
    \internal
*/
bool QServiceTypeRegister::registerType( const QMetaObject* meta,
        CreateServiceFunc func, TypeIdentFunc tfunc,  QServiceTypeRegister::InstanceType t)
{
    Q_ASSERT(InstanceManager::instance());
    return InstanceManager::instance()->addType(meta, func, tfunc, t);
}

/*!
    \internal
*/
const QMetaObject* QServiceTypeRegister::metaObject(const QServiceTypeIdent& ident) const
{
    Q_ASSERT(InstanceManager::instance());
    return InstanceManager::instance()->metaObject(ident);
}

/*!
    Returns the list of all registered service types.
*/
QList<QServiceTypeIdent> QServiceTypeRegister::types() const
{
    Q_ASSERT(InstanceManager::instance());
    return InstanceManager::instance()->allIdents();
}

QTM_END_NAMESPACE
