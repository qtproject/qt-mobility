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
#include "databasemanager.h"

#include <QObject>
#include <QPluginLoader>
#include <QBuffer>
#include <QFile>
#include <QDebug>
#include <QFileSystemWatcher>

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

class QServiceManagerPrivate : public QObject
{
    Q_OBJECT
public:
    QServiceManager *manager;
    ServiceDatabase *database;
    DatabaseManager *dbManager;
    QServiceManager::Scope scope;
    QServiceManager::Error error;

    QServiceManagerPrivate(QServiceManager *parent = 0)
        : QObject(parent),
          manager(parent),
          database(new ServiceDatabase),
          dbManager(new DatabaseManager)
    {
        connect(dbManager, SIGNAL(serviceAdded(QString, DatabaseManager::DbScope)),
                SLOT(serviceAdded(QString, DatabaseManager::DbScope)));
        connect(dbManager, SIGNAL(serviceRemoved(QString, DatabaseManager::DbScope)),
                SLOT(serviceRemoved(QString, DatabaseManager::DbScope)));
    }

    ~QServiceManagerPrivate()
    {
        delete database;
        delete dbManager;
    }

    void init(QServiceManager::Scope databaseScope)
    {
        scope = databaseScope;

        if (!database->open()) {
            setError();
            qWarning("QServiceManager: unable to open services database");
        }
    }

    void setError(QServiceManager::Error err)
    {
        error = err;
    }

    void setError()
    {
        switch (database->lastError().errorCode()) {
            case DBError::NoError:
                error = QServiceManager::NoError;
                break;
            case DBError::DatabaseNotOpen:
            case DBError::InvalidDatabaseConnection:
                error = QServiceManager::StorageReadError;
                break;
            case DBError::ComponentAlreadyRegistered:
                error = QServiceManager::ServiceAlreadyExists;
                break;
            case DBError::IfaceImplAlreadyRegistered:
                error = QServiceManager::ImplementationAlreadyExists;
                break;
            case DBError::NotFound:
                error = QServiceManager::ComponentNotFound;
                break;
            case DBError::SqlError:
            case DBError::InvalidSearchCriteria:
            case DBError::CannotCloseDatabase:
            case DBError::UnknownError:
                error = QServiceManager::UnknownError;
                break;
        }
    }

private slots:
    void serviceAdded(const QString &service, DatabaseManager::DbScope dbScope)
    {
        QServiceManager::Scope s = (dbScope == DatabaseManager::SystemScope ?
                QServiceManager::SystemScope : QServiceManager::UserScope);
        emit manager->serviceAdded(service, s);
    }

    void serviceRemoved(const QString &service, DatabaseManager::DbScope dbScope)
    {
        QServiceManager::Scope s = (dbScope == DatabaseManager::SystemScope ?
                QServiceManager::SystemScope : QServiceManager::UserScope);
        emit manager->serviceRemoved(service, s);
    }
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
    \enum QServiceManager::Scope

    \value UserScope When adding and removing services, use a storage location
    specific to the current user (e.g. in the user's home directory). When
    searching for services and interface implementations, first search in the
    user-specific location; if the service or interface implementation
    is not found, search in the system-wide storage location.

    \value SystemScope When adding and removing services, use a system-wide
    storage location accessible to all users. When searching
    for services and interface implementations, search only in the system-wide
    storage location.
*/

/*!
    \enum QServiceManager::Error
    Defines the possible errors for the service manager.

    \value NoError No error occurred.
    \value StoragePermissionsError The service data storage cannot be accessed.
    \value StorageReadError The service data storage is not available.
    \value InvalidServiceLocation The service was not found at its specified \l{QServiceInterfaceDescriptor::Location}{location}.
    \value InvalidServiceXml The XML defining the service metadata is invalid.
    \value InvalidServiceInterfaceDescriptor The service interface descriptor is invalid.
    \value ServiceAlreadyExists Another service has previously been registered with the same \l{QServiceInterfaceDescriptor::Location}{location}.
    \value ImplementationAlreadyExists Another service that implements the same interface version has previously been registered.
    \value PluginLoadingFailed The service plugin cannot be loaded.
    \value ComponentNotFound The service or interface implementation has not been registered.
    \value ServiceCapabilityDenied The security session does not allow the service based on its capabilities.
    \value UnknownError An unknown error occurred.
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

    The scope will default to QServiceManager::UserScope.
*/
QServiceManager::QServiceManager(QObject *parent)
    : QObject(parent),
      d(new QServiceManagerPrivate(this))
{
    d->init(UserScope);
}

/*!
    Creates a service manager with the given \a scope and \a parent.
*/
QServiceManager::QServiceManager(Scope scope, QObject *parent)
    : QObject(parent),
      d(new QServiceManagerPrivate(this))
{
    d->init(scope);
}

/*!
    Destroys the service manager.
*/
QServiceManager::~QServiceManager()
{
    delete d;
}

/*!
    Returns the scope used for registering and searching of services.
*/
QServiceManager::Scope QServiceManager::scope() const
{
    return d->scope;
}

/*!
    Returns a list of the services that provide the interface specified by
    \a interfaceName.
*/
QStringList QServiceManager::findServices(const QString& interfaceName) const
{
    QStringList services;
    services = d->dbManager->getServiceNames(interfaceName,
            d->scope == SystemScope ? DatabaseManager::SystemScope : DatabaseManager::UserScope);
    d->setError();
    return services;
}

/*!
    Returns a list of the interfaces that match the specified \a filter.
*/
QList<QServiceInterfaceDescriptor> QServiceManager::findInterfaces(const QServiceFilter& filter) const
{
    QList<QServiceInterfaceDescriptor> descriptors = d->dbManager->getInterfaces(filter,
            d->scope == SystemScope ? DatabaseManager::SystemScope : DatabaseManager::UserScope);
    if (descriptors.isEmpty() && d->dbManager->lastError().errorCode() != DBError::NoError) {
        d->setError();
        return QList<QServiceInterfaceDescriptor>();
    }
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
        d->setError(InvalidServiceInterfaceDescriptor);
        return 0;
    }

    const QStringList serviceCaps = descriptor.property(QServiceInterfaceDescriptor::Capabilities).toStringList();
    if ( session && !session->isAllowed(serviceCaps) ) {
        d->setError(ServiceCapabilityDenied);
        return 0;  //TODO set error state on context object, if it exists
    }

    QString serviceFilePath = qservicemanager_resolveLibraryPath(
            descriptor.property(QServiceInterfaceDescriptor::Location).toString());
    if (serviceFilePath.isEmpty()) {
        d->setError(InvalidServiceLocation);
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

    loader->unload();
    delete loader;
    d->setError(PluginLoadingFailed);

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
    ServiceMetaData data(device);
    if (!data.extractMetadata()) {
        d->setError(InvalidServiceXml);
        return false;
    }

    DatabaseManager::DbScope scope = d->scope == UserScope ?
            DatabaseManager::UserOnlyScope : DatabaseManager::SystemScope;
    bool result = d->dbManager->registerService(data, scope);
    if (result) {
        QPluginLoader *loader = new QPluginLoader(qservicemanager_resolveLibraryPath(data.location()));
        QServicePluginInterface *pluginIFace = qobject_cast<QServicePluginInterface *>(loader->instance());
        if (pluginIFace) {
            pluginIFace->installService();
        } else {
            d->setError(PluginLoadingFailed);
            result = false;
            d->dbManager->unregisterService(data.name(), scope);
        }
        loader->unload();
        delete loader;
    } else {
        d->setError();
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
    if (serviceName.isEmpty()) {
        d->setError(ComponentNotFound);
        return false;
    }

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

    if (!d->dbManager->unregisterService(serviceName, d->scope == UserScope ?
            DatabaseManager::UserOnlyScope : DatabaseManager::SystemScope)) {
        d->setError();
        return false;
    }
    return true;
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
    if (service.isEmpty() || interfaceName.isEmpty()) {
        d->setError(ComponentNotFound);
        return false;
    }

    if (!d->database->setDefaultService(service, interfaceName)) {
        d->setError();
        return false;
    }
    return true;
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
    if (!d->database->isOpen()) {
        d->setError(StorageReadError);
        return false;
    }
    if (!descriptor.isValid()) {
        d->setError(InvalidServiceInterfaceDescriptor);
        return false;
    }
    if (!d->database->setDefaultService(descriptor)) {
        d->setError();
        return false;
    }
    return true;
}

/*!
    Returns the default interface for the given \a interfaceName.
*/
QServiceInterfaceDescriptor QServiceManager::defaultServiceInterface(const QString& interfaceName) const
{
    if (!d->database->isOpen()) {
        d->setError(StorageReadError);
        return QServiceInterfaceDescriptor();
    }
    bool ok = false;
    QServiceInterfaceDescriptor info;
    info = d->database->defaultServiceInterface(interfaceName, &ok);
    if (!ok) {
        d->setError();
        return QServiceInterfaceDescriptor();
    }

    return info;
}

/*!
    Returns the type of error that last occurred.
*/
QServiceManager::Error QServiceManager::error() const
{
    return d->error;
}

/*!
    \internal
*/
void QServiceManager::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(serviceAdded(QString,QServiceManager::Scope))
            || QLatin1String(signal) == SIGNAL(serviceRemoved(QString,QServiceManager::Scope))) {
        DatabaseManager::DbScope scope = d->scope == SystemScope ?
                DatabaseManager::SystemScope : DatabaseManager::UserScope;
        d->dbManager->setChangeNotificationsEnabled(scope, true);
    }
}

/*!
    \internal
*/
void QServiceManager::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(serviceAdded(QString,QServiceManager::Scope))
            || QLatin1String(signal) == SIGNAL(serviceRemoved(QString,QServiceManager::Scope))) {
        if (receivers(SIGNAL(serviceAdded(QString,QServiceManager::Scope))) == 0
                && receivers(SIGNAL(serviceRemoved(QString,QServiceManager::Scope))) == 0) {
            DatabaseManager::DbScope scope = d->scope == SystemScope ?
                    DatabaseManager::SystemScope : DatabaseManager::UserScope;
            d->dbManager->setChangeNotificationsEnabled(scope, false);
        }
    }
}

QT_END_NAMESPACE

#include "qservicemanager.moc"
