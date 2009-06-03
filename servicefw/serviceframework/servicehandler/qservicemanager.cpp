/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
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

#include "qservicemanager.h"
#include "qserviceplugininterface.h"
#include "qabstractsecuritysession.h"
#include "servicedatabase.h"

#include <QObject>
#include <QPluginLoader>
#include <QBuffer>
#include <QFile>
#include <QDebug>

QT_BEGIN_NAMESPACE


static QString qservicemanager_resolveLibraryPath(const QString &libNameOrPath)
{
    if (QFile::exists(libNameOrPath))
        return libNameOrPath;

    // try to find plug-in via QLibrary
    const QStringList paths = QCoreApplication::libraryPaths();
    for (int i=0; i<paths.count(); i++) {
        QLibrary lib(paths[i] + QDir::separator() + libNameOrPath);
        if (lib.load()) {
            lib.unload();
            return lib.fileName();
        }
    }
    return QString();
}


class QServicePluginCleanup : public QObject
{
    Q_OBJECT
public:
    QServicePluginCleanup(QPluginLoader *loader, QServicePluginInterface *pluginInterface, QObject *parent = 0)
        : QObject(parent),
          m_pluginInterface(pluginInterface),
          m_loader(loader)
    {
    }

    ~QServicePluginCleanup()
    {
        delete m_pluginInterface;
        if (m_loader) {
            m_loader->unload();
            delete m_loader;
        }
    }



    QServicePluginInterface *m_pluginInterface;
    QPluginLoader *m_loader;
};

class QServiceManagerPrivate
{
public:
    ServiceDatabase *database;
};

/*!
    \class QServiceManager
    \brief The QServiceManager class enables the loading of service plugins 
    and the (de)registration of services.

    A service is a stand-alone component that can be used by multiple clients. 
    Each service implementation must derive from QObject. Clients request a 
    reference to a service via \l loadInterface() or \l getInterface().

    Services are separate deliveries in the form of plug-ins. New services can be (de)registered 
    at any time via \l addService() and \l removeService() respectively. Such an event is 
    published via the \l serviceAdded() and \l serviceRemoved() signal.
    Each service plug-in must implement QServicePluginInterface.

    Each plug-in may support multiple interfaces and may even provide multiple implementations 
    for the same interface. Individual implementations are identified via 
    QServiceInterfaceDescriptor. For a more detailed explanation of services and how they relate to
    interface and their implementations please see QServiceInterfaceDescriptor.  
 
    \sa QServicePluginInterface, QServiceContext, QAbstractSecuritySession
*/

/*!
    \fn void QServiceManager::serviceAdded(const QString& serviceName)

    This signal is emited whenever a new service with the given 
    \a serviceName has been registered with the service manager.

    \sa addService()
*/

/*!
    \fn void QServiceManager::serviceRemoved(const QString& serviceName)

    This signal is emited whenever a service with the given 
    \a serviceName has been deregistered with the service manager.

    \sa removeService()
*/

/*!
    Creates a service manager with the given \a parent.
*/
QServiceManager::QServiceManager(QObject* parent)
    : QObject(parent),
      d(new QServiceManagerPrivate)
{
    d->database = new ServiceDatabase;
    connect(d->database, SIGNAL(serviceAdded(QString)), SIGNAL(serviceAdded(QString)));
    connect(d->database, SIGNAL(serviceRemoved(QString)), SIGNAL(serviceRemoved(QString)));

    if (!d->database->open())
        qWarning("QServiceManager: unable to open services database");
}

/*!
    Destroys the service manager.
*/
QServiceManager::~QServiceManager()
{
    d->database->close();
    delete d->database;
    delete d;
}

/*!
    Returns a list of the services that provide the interface specified by
    \a interfaceName.
*/
QStringList QServiceManager::findServices(const QString& interfaceName) const
{
    QStringList services;
    QList<ServiceInfo> serviceInfoList;
    if (!d->database->isOpen())
        return services;
    services = d->database->getServiceNames(interfaceName);
    return services;
}

/*!
    Returns a list of the interfaces that match the specified \a filter.
*/
QList<QServiceInterfaceDescriptor> QServiceManager::findInterfaces(const QServiceFilter& filter) const
{
    if (!d->database->isOpen())
        return QList<QServiceInterfaceDescriptor>();
    bool ok = false;
    QList<QServiceInterfaceDescriptor> descriptors = d->database->getInterfaces(filter, &ok);
    if (!ok)
        return QList<QServiceInterfaceDescriptor>();
    return descriptors;
}

/*!
    Returns a list of the interfaces provided by the service named
    \a serviceName.
*/
QList<QServiceInterfaceDescriptor> QServiceManager::findInterfaces(const QString& serviceName) const
{
    QServiceFilter filter;
    if (!serviceName.isEmpty())
        filter.setServiceName(serviceName);
    return findInterfaces(filter);
}

/*!
    Loads and returns the interface specified by \a interfaceName, as
    provided by the default service for this interface, using the given
    \a context and \a session. \a context and \a session object are owned 
    by the caller of this function.

    The caller takes ownership of the returned pointer.
    
    This function returns a null pointer if the requested service cannot be found.
*/
QObject* QServiceManager::loadInterface(const QString& interfaceName, QServiceContext* context, QAbstractSecuritySession* session)
{
    return loadInterface(defaultServiceInterface(interfaceName), context, session);
}

/*!
    Loads and returns the interface specified by \a descriptor using the
    given \a context and \a session. \a context and \a session object are owned 
    by the caller of this function.

    The caller takes ownership of the returned pointer.

    This function returns a null pointer if the requested service cannot be found.
*/
QObject* QServiceManager::loadInterface(const QServiceInterfaceDescriptor& descriptor, QServiceContext* context, QAbstractSecuritySession* session)
{
    if (!descriptor.isValid()) {
        qWarning() << "QServiceManager::loadInterface() given invalid descriptor";
        return 0;
    }

    const QStringList serviceCaps = descriptor.property(QServiceInterfaceDescriptor::Capabilities).toStringList();
    if ( session && !session->isAllowed(serviceCaps) )
        return 0;  //TODO set error state on context object, if it exists

    QString serviceFilePath = qservicemanager_resolveLibraryPath(
            descriptor.property(QServiceInterfaceDescriptor::Location).toString());
    if (serviceFilePath.isEmpty()) {
        qWarning() << "QServiceManager::loadInterface() cannot locate library file for plugin:"
                << descriptor.property(QServiceInterfaceDescriptor::Location).toString();
        return 0;
    }

    QPluginLoader *loader = new QPluginLoader(serviceFilePath);
    QServicePluginInterface *pluginIFace = qobject_cast<QServicePluginInterface *>(loader->instance());

    if (pluginIFace) {
        QObject *obj = pluginIFace->createInstance(descriptor, context, session);
        if (obj) {
            QServicePluginCleanup *cleanup = new QServicePluginCleanup(loader, pluginIFace);
            QObject::connect(obj, SIGNAL(destroyed()), cleanup, SLOT(deleteLater()));
            return obj;
        }
        delete pluginIFace;
    }

    qWarning() << "QServiceManager::loadInterface() cannot load plugin at"
            << serviceFilePath << ":" << loader->errorString();
    loader->unload();
    delete loader;

    return 0;
}

/*!
    \fn T* QServiceManager::getInterface(const QString& interfaceName, QServiceContext* context, QAbstractSecuritySession* session)

    Loads the service object implementing \a interfaceName,
    as provided by the default service for this interface, using the given
    \a context and \a session. \a context and \a session object are owned 
    by the caller of this function. The template class must be derived from QObject.

    If \a interfaceName is not a known interface the returned pointer will be null.

    The caller takes ownership of the returned pointer.
*/


/*!
    \fn T* QServiceManager::getInterface(const QServiceInterfaceDescriptor& serviceDescriptor, QServiceContext* context, QAbstractSecuritySession* session)

    Loads the service object identified by \a serviceDescriptor
    using the given \a context and \a session. \a context and \a session object are owned 
    by the caller of this function. The template class must be derived from QObject.


    If the \a serviceDescriptor is not valid the returned pointer will be null.

    The caller takes ownership of the returned pointer.
*/

/*!
    Registers the service defined by the XML file at \a xmlFilePath.
    Returns true if the registration succeeded, and false otherwise.

    If a previously unkown interface is added the newly registered service automatically
    becomes the new default service provider for the new interface.

    A service plugin cannot be added if another service is already registered
    with the same plugin file path.  A service plugin also cannot be added if
    the service is already registered and implements any of the same interface
    versions that the new plugin implements.

    \sa setDefaultServiceForInterface()
*/
bool QServiceManager::addService(const QString& xmlFilePath)
{
    QFile *f = new QFile(xmlFilePath);
    bool b = addService(f);
    delete f;
    return b;
}

/*!
    Registers the service defined by the XML data from the given \a device.
    Returns true if the registration succeeded, and false otherwise. If a
    previously unkown interface is added the newly registered service
    automatically becomes the new default service provider for the new
    interface.

    Registering a service also causes QServicePluginInterface::installService()
    to be called on the service. If the service plugin is not accessible
    (e.g. if the plugin file is not found) and \c installService() cannot
    be invoked on the service, the registration fails and this method returns
    false.

    A service plugin cannot be added if another service is already registered
    with the same plugin file path.  A service plugin also cannot be added if
    the service is already registered and implements any of the same interface
    versions that the new plugin implements.

    \sa setDefaultServiceForInterface()
*/
bool QServiceManager::addService(QIODevice *device)
{
    if (!d->database->isOpen())
        return false;
    ServiceMetaData data(device);
    if (!data.extractMetadata())
        return false;

    bool result = d->database->registerService(data);
    if (result) {
        QPluginLoader *loader = new QPluginLoader(qservicemanager_resolveLibraryPath(data.location()));
        QServicePluginInterface *pluginIFace = qobject_cast<QServicePluginInterface *>(loader->instance());
        if (pluginIFace) {
            pluginIFace->installService();
        } else {
            result = false;
            d->database->unregisterService(data.name());
        }
        loader->unload();
        delete loader;
    }
    return result;
}

/*!
    Unregisters the service specified by \a serviceName.

    Returns true if the unregistration succeeded, and false otherwise.

    If a default service implementation is removed and there are other implementations
    for the same interface, the service manager chooses the implementation with the
    highest version number as the new default.  If there is more than one serivce 
    with the same version number, the service manager makes a random choice with 
    regards to the new default implementation. If this is 
    not the desired behaviour the default selection should be updated
    via setDefaultServiceForInterface(). 
*/
bool QServiceManager::removeService(const QString& serviceName)
{
    if (!d->database->isOpen())
        return false;
    if (serviceName.isEmpty())
        return false;

    // Call QServicePluginInterface::uninstallService() on all plugins that
    // match this service

    QSet<QString> pluginPathsSet;
    QList<QServiceInterfaceDescriptor> descriptors = findInterfaces(serviceName);
    for (int i=0; i<descriptors.count(); i++)
        pluginPathsSet << descriptors[i].property(QServiceInterfaceDescriptor::Location).toString();

    QList<QString> pluginPaths = pluginPathsSet.toList();
    for (int i=0; i<pluginPaths.count(); i++) {
        QPluginLoader *loader = new QPluginLoader(qservicemanager_resolveLibraryPath(pluginPaths[i]));
        QServicePluginInterface *pluginIFace = qobject_cast<QServicePluginInterface *>(loader->instance());
        if (pluginIFace)
            pluginIFace->uninstallService();
        else
            qWarning() << "QServiceManager: unable to invoke uninstallService() on removed service";
        loader->unload();
        delete loader;
    }

    return d->database->unregisterService(serviceName);
}

/*!
    Sets the default interface implementation for \a interfaceName to the
    matching interface implementation provided by \a service.

    If \a service provides more than one interface implementation for
    \a interfaceName, the newest version of the interface is set as the
    default.

    Returns true if the operation succeeded, and false otherwise.
*/
bool QServiceManager::setDefaultServiceForInterface(const QString &service, const QString &interfaceName)
{
    if (!d->database->isOpen())
        return false;
    if (interfaceName.isEmpty() || service.isEmpty())
        return false;
    return d->database->setDefaultService(service, interfaceName);
}

/*!
    \overload

    Sets the interface implementation specified by \a descriptor to be the
    default implementation for the particular interface specified in the
    descriptor.

    Returns true if the operation succeeded, and false otherwise.
*/
bool QServiceManager::setDefaultServiceForInterface(const QServiceInterfaceDescriptor& descriptor)
{
    if (!d->database->isOpen())
        return false;
    if (!descriptor.isValid())
        return false;
    return d->database->setDefaultService(descriptor);
}

/*!
    Returns the default interface for the given \a interfaceName.
*/
QServiceInterfaceDescriptor QServiceManager::defaultServiceInterface(const QString& interfaceName) const
{
    if (!d->database->isOpen())
        return QServiceInterfaceDescriptor();
    bool ok = false;
    QServiceInterfaceDescriptor info;
    info = d->database->defaultServiceInterface(interfaceName, &ok);
    if (!ok)
        return QServiceInterfaceDescriptor();

    return info;
}

QT_END_NAMESPACE

#include "qservicemanager.moc"
