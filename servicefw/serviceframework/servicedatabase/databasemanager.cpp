/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "databasemanager.h"
#include <qserviceinterfacedescriptor_p.h>
#include <QFileSystemWatcher>
#include <QHash>

QT_BEGIN_NAMESPACE

DatabaseFileWatcher::DatabaseFileWatcher(DatabaseManager *parent)
    : QObject(parent),
      m_manager(parent),
      m_watcher(0)
{
}

QString DatabaseFileWatcher::closestExistingParent(const QString &path)
{
    if (QFile::exists(path))
        return path;

    int lastSep = path.lastIndexOf(QDir::separator());
    if (lastSep < 0)
        return QString();
    return closestExistingParent(path.mid(0, lastSep));
}

void DatabaseFileWatcher::restartDirMonitoring(const QString &dbPath, const QString &previousDirPath)
{
    if (m_watcher->files().contains(dbPath))
        return;

    QString existing = closestExistingParent(dbPath);
    if (existing.isEmpty()) {
        qWarning() << "QServiceManager: can't find existing directory for path to database" << dbPath
            << "serviceAdded() and serviceRemoved() will not be emitted";
        return;
    }
    if (existing == dbPath) {
        ServiceDatabase *db = 0;
        DatabaseManager::DbScope scope;
        if (dbPath == m_manager->m_userDb->databasePath()) {
            db = m_manager->m_userDb;
            scope = DatabaseManager::UserOnlyScope;
        } else if (dbPath == m_manager->m_systemDb->databasePath()) {
            db = m_manager->m_systemDb;
            scope = DatabaseManager::SystemScope;
        }

        if (db) {
            if (!previousDirPath.isEmpty())
                m_watcher->removePath(previousDirPath);
            QMutableListIterator<QString> i(m_monitoredDbPaths);
            while (i.hasNext()) {
                if (i.next() == dbPath)
                    i.remove();
            }

            QStringList newServices = m_manager->getServiceNames(QString(), scope);
            for (int i=0; i<newServices.count(); i++)
                emit m_manager->serviceAdded(newServices[i], scope);
            setEnabled(db, true);
        }
    } else {
        if (previousDirPath != existing) {
            if (!previousDirPath.isEmpty())
                m_watcher->removePath(previousDirPath);
            if (!m_watcher->directories().contains(existing))
                m_watcher->addPath(existing);
            if (!m_monitoredDbPaths.contains(dbPath))
                m_monitoredDbPaths << dbPath;
        }
    }
}

void DatabaseFileWatcher::setEnabled(ServiceDatabase *database, bool enabled)
{
    if (!m_watcher) {
        m_watcher = new QFileSystemWatcher(this);
        connect(m_watcher, SIGNAL(fileChanged(QString)),
            SLOT(databaseChanged(QString)));
        connect(m_watcher, SIGNAL(directoryChanged(QString)),
            SLOT(databaseDirectoryChanged(QString)));
    }

    QString path = database->databasePath();
    if (enabled) {
        if (QFile::exists(path)) {
            m_knownServices[path] = database->getServiceNames(QString());
            m_watcher->addPath(path);
        } else {
            restartDirMonitoring(path, QString());
        }
    } else {
        m_watcher->removePath(path);
        m_knownServices.remove(path);
    }
}

void DatabaseFileWatcher::databaseDirectoryChanged(const QString &path)
{
    for (int i=0; i<m_monitoredDbPaths.count(); i++) {
        if (m_monitoredDbPaths[i].contains(path))
            restartDirMonitoring(m_monitoredDbPaths[i], path);
    }
}

void DatabaseFileWatcher::databaseChanged(const QString &path)
{
    if (path == m_manager->m_userDb->databasePath())
        notifyChanges(m_manager->m_userDb, DatabaseManager::UserScope);
    else if (path == m_manager->m_systemDb->databasePath())
        notifyChanges(m_manager->m_systemDb, DatabaseManager::SystemScope);

    // if database was deleted, the path may have been dropped
    if (!m_watcher->files().contains(path) && QFile::exists(path))
        m_watcher->addPath(path);
}

void DatabaseFileWatcher::notifyChanges(ServiceDatabase *database, DatabaseManager::DbScope scope)
{
    QString dbPath = database->databasePath();
    if (!QFile::exists(dbPath)) {
        m_knownServices.remove(dbPath);
        restartDirMonitoring(dbPath, QString());
        return;
    }

    bool ok = false;
    QStringList currentServices = database->getServiceNames(QString(), &ok);
    if (!ok) {
        qWarning("QServiceManager: failed to get current service names for serviceAdded() and serviceRemoved() signals");
        return;
    }

    const QStringList &knownServicesRef = m_knownServices[dbPath];

    QSet<QString> currentServicesSet = currentServices.toSet();
    QSet<QString> knownServicesSet = knownServicesRef.toSet();
    if (currentServicesSet == knownServicesSet)
        return;

    QStringList newServices;
    for (int i=0; i<currentServices.count(); i++) {
        if (!knownServicesSet.contains(currentServices[i]))
            newServices << currentServices[i];
    }

    QStringList removedServices;
    for (int i=0; i<knownServicesRef.count(); i++) {
        if (!currentServicesSet.contains(knownServicesRef[i]))
            removedServices << knownServicesRef[i];
    }

    m_knownServices[dbPath] = currentServices;
    for (int i=0; i<newServices.count(); i++)
        emit m_manager->serviceAdded(newServices[i], scope);
    for (int i=0; i<removedServices.count(); i++)
        emit m_manager->serviceRemoved(removedServices[i], scope);
}

bool lessThan(const QServiceInterfaceDescriptor &d1,
                                        const QServiceInterfaceDescriptor &d2)
{
        return (d1.majorVersion() < d2.majorVersion())
                || ( d1.majorVersion() == d2.majorVersion()
                && d1.minorVersion() < d2.minorVersion());
}

DatabaseManager::DatabaseManager()
    : m_userDb(new ServiceDatabase),
      m_systemDb(new ServiceDatabase),
      m_fileWatcher(0)
{
    initDbPath(UserScope);
    initDbPath(SystemScope);
}

DatabaseManager::~DatabaseManager()
{
    close();
}

void DatabaseManager::initDbPath(DbScope scope)
{
    QSettings::Scope settingsScope;
    QString dbIdentifier;
    ServiceDatabase *db;
    if (scope == SystemScope) {
        settingsScope = QSettings::SystemScope;
        dbIdentifier = "_system";
        db = m_systemDb;
    } else {
        settingsScope = QSettings::UserScope;
        dbIdentifier = "_user";
        db = m_userDb;
    }

    QSettings settings(QSettings::IniFormat, settingsScope,
            QLatin1String("Nokia"), QLatin1String("QtServiceFramework"));
    QFileInfo fi(settings.fileName());
    QDir dir = fi.dir();
    QString qtVersion(qVersion());
    qtVersion = qtVersion.left(qtVersion.size() -2); //strip off patch version
    QString dbName = QString("QtServiceFW_") + qtVersion + dbIdentifier + QLatin1String(".db");
    db->setDatabasePath(dir.path() + QDir::separator() + dbName);
}

void DatabaseManager::close()
{
    delete m_userDb;
    m_userDb = 0;
    delete m_systemDb;
    m_systemDb = 0;
}

bool DatabaseManager::registerService(ServiceMetaData &service, DbScope scope)
{
    if (scope == DatabaseManager::SystemScope) {
        if(!openDb(DatabaseManager::SystemScope)) {
            return false;
        }  else {
            if (!m_systemDb->registerService(service)) {
                m_lastError = m_systemDb->lastError();
                return false;
            } else { //must be successful registration
                m_lastError.setError(DBError::NoError);
                return true;
            }
        }
    } else { //must registering service at user scope
        if (!openDb(DatabaseManager::UserScope)) {
            return false;
        } else {
            if (!m_userDb->registerService(service)) {
                m_lastError = m_userDb->lastError();
                return false;
            } else { //must be successful registration
                m_lastError.setError(DBError::NoError);
                return true;
            }
        }
    }
}

bool DatabaseManager::unregisterService(const QString &serviceName, DbScope scope)
{
    if(scope == DatabaseManager::SystemScope) {
        if (!openDb(DatabaseManager::SystemScope))
            return false;
   else {
            if (!m_systemDb->unregisterService(serviceName)) {
                m_lastError = m_systemDb->lastError();
                return false;
            } else { //must be successful unregistration
                m_lastError.setError(DBError::NoError);
                return true;
            }
        }
    } else {
        if (!openDb(DatabaseManager::UserScope)) {
            return false;
        } else {
            if (!m_userDb->unregisterService(serviceName)){
                m_lastError = m_userDb->lastError();
                return false;
            } else { //must be successful unregistration
                m_lastError.setError(DBError::NoError);
                return true;
            }
        }
    }
}

QList<QServiceInterfaceDescriptor>  DatabaseManager::getInterfaces(const QServiceFilter &filter, DbScope scope)
{
    QList<QServiceInterfaceDescriptor> descriptors;

    bool ok = false;
    int userDescriptorCount = 0;
    if (scope == UserScope) {
        if (!openDb(UserScope))
            return descriptors;
        descriptors =  m_userDb->getInterfaces(filter, &ok);

        if (ok == false) {
            descriptors.clear();
            m_lastError = m_userDb->lastError();
            return descriptors;
        }

        userDescriptorCount = descriptors.count();
        for (int i=0; i < userDescriptorCount; ++i) {
            descriptors[i].d->systemScope = false;
        }
    }

    if (openDb(SystemScope)) {
        descriptors.append(m_systemDb->getInterfaces(filter, &ok));
        if (ok == false) {
            descriptors.clear();
            m_lastError = m_systemDb->lastError();
            return descriptors;
        }

        for (int i = userDescriptorCount; i < descriptors.count(); ++i) {
            descriptors[i].d->systemScope = true;
        }
        m_lastError.setError(DBError::NoError);
    }

    if (!m_systemDb->isOpen() && scope == SystemScope) {
        //openDb() should already have handled lastError
        descriptors.clear();
        return descriptors;
    }

    //Note:if the system database could not be opened at user scope,
    //we continue on and just use the results for the user database
    return descriptors;
}

QStringList DatabaseManager::getServiceNames(const QString &interfaceName, DatabaseManager::DbScope scope)
{
    QStringList serviceNames;
    bool ok = false;
    if (scope == UserScope || scope == UserOnlyScope) {
        if(!openDb(DatabaseManager::UserScope))
            return serviceNames;
        serviceNames = m_userDb->getServiceNames(interfaceName, &ok);
        if(!ok) {
            serviceNames.clear();
            m_lastError = m_userDb->lastError();
            return serviceNames;
        }
        if (scope == UserOnlyScope) {
            m_lastError.setError(DBError::NoError);
            return serviceNames;
        }
    }

    if(openDb(DatabaseManager::SystemScope)) {
        QStringList systemServiceNames;
        systemServiceNames = m_systemDb->getServiceNames(interfaceName, &ok);
        if(!ok) {
            serviceNames.clear();
            m_lastError = m_systemDb->lastError();
            return serviceNames;
        }
        foreach(const QString &systemServiceName, systemServiceNames) {
            if(!serviceNames.contains(systemServiceName, Qt::CaseInsensitive))
                serviceNames.append(systemServiceName);
        }

        m_lastError.setError(DBError::NoError);
    }

    if (!m_systemDb->isOpen() && scope == SystemScope) {
        //openDb() should have already handled lastError
        serviceNames.clear();
        return serviceNames;
    }

    //Note:if the system database could not be opened at user scope,
    //we continue on and just use the results for the user database
    return serviceNames;
}

QServiceInterfaceDescriptor DatabaseManager::defaultServiceInterface(const QString &interfaceName, DbScope scope)
{
    QServiceInterfaceDescriptor descriptor;
    if (scope == UserScope) {
        if (!openDb(UserScope))
            return QServiceInterfaceDescriptor();
        QString interfaceID;
        descriptor = m_userDb->defaultServiceInterface(interfaceName, &interfaceID);

        if (m_userDb->lastError().errorCode() == DBError::NoError) {
            descriptor.d->systemScope = false;
            return descriptor;
        } else if (m_userDb->lastError().errorCode() == DBError::ExternalIfaceIDFound) {
            //default hasn't been found in user db, but we have found an ID
            //that may refer to an interface implementation in the system db
            if (!openDb(SystemScope)) {
                QString errorText("No default service found for interface: \"%1\"");
                m_lastError.setError(DBError::NotFound, errorText.arg(interfaceName));
                return QServiceInterfaceDescriptor();
            }

            descriptor = m_systemDb->getInterface(interfaceID);
            //found the service from the system database
            if (m_systemDb->lastError().errorCode() == DBError::NoError) {
                m_lastError.setError(DBError::NoError);
                descriptor.d->systemScope = true;
                return descriptor;
            } else if(m_systemDb->lastError().errorCode() == DBError::NotFound){
                //service implementing interface doesn't exist in the system db
                //so the user db must contain a stale entry so remove it
                m_userDb->removeExternalDefaultServiceInterface(interfaceID);
                QString errorText("No default service found for interface: \"%1\"");
                m_lastError.setError(DBError::NotFound, errorText.arg(interfaceName));
                return QServiceInterfaceDescriptor();
            } else {
                m_lastError.setError(DBError::NoError);
                return QServiceInterfaceDescriptor();
            }
        } else if (m_userDb->lastError().errorCode() == DBError::NotFound) {
            //do nothing, the search for a default in the system db continues
            //further down
        } else { //error occurred at user db level, so return
            m_lastError = m_userDb->lastError();
            return QServiceInterfaceDescriptor();
        }
    }

    //search at system scope because we haven't found a default at user scope
    //or because we're specifically only querying at system scope
    if (!openDb(SystemScope)) {
        if (scope == SystemScope) {
            m_lastError = m_systemDb->lastError();
            return QServiceInterfaceDescriptor();
        } else if (scope == UserScope && m_userDb->lastError().errorCode() == DBError::NotFound) {
            m_lastError = m_userDb->lastError();
            return QServiceInterfaceDescriptor();
        }
    } else {
        descriptor = m_systemDb->defaultServiceInterface(interfaceName);
        if (m_systemDb->lastError().errorCode() == DBError::NoError) {
            descriptor.d->systemScope = true;
            return descriptor;
        } else if (m_systemDb->lastError().errorCode() == DBError::NotFound) {
            m_lastError = m_systemDb->lastError();
            return QServiceInterfaceDescriptor();
        } else {
            m_lastError = m_systemDb->lastError();
            return QServiceInterfaceDescriptor();
        }
    }

    //should not be possible to reach here
    m_lastError.setError(DBError::UnknownError);
    return QServiceInterfaceDescriptor();
}

bool DatabaseManager::setDefaultService(const QString &serviceName, const QString &interfaceName, DbScope scope)
{
    QList<QServiceInterfaceDescriptor> descriptors;
    int userDescriptorCount = 0;
    bool ok;
    QServiceFilter filter;
    filter.setServiceName(serviceName);
    filter.setInterface(interfaceName);

    descriptors = getInterfaces(filter, scope);
    if (m_lastError.errorCode() != DBError::NoError)
        return false;

    if (descriptors.count() == 0) {
        QString errorText("No implementation for interface \"%1\" "
                "found for service \"%2\"");
        m_lastError.setError(DBError::NotFound,
                errorText.arg(interfaceName)
                .arg(serviceName));
        return false;
    }

    //find the descriptor with the latest version
    int latestIndex = 0;
        for (int i = 1; i < descriptors.count(); ++i) {
            if (lessThan(descriptors[latestIndex], descriptors[i]))
                latestIndex = i;
    }

    return setDefaultService(descriptors[latestIndex], scope);
}

bool DatabaseManager::setDefaultService(const QServiceInterfaceDescriptor &descriptor, DbScope scope)
{
    if (scope == UserScope) {
        if (!openDb(UserScope))
            return false;
        if (!descriptor.inSystemScope()) { //if a user scope descriptor, just set it in the user db
            if(m_userDb->setDefaultService(descriptor)) {
                m_lastError.setError(DBError::NoError);
                return true;
            } else {
                m_lastError = m_userDb->lastError();
                return false;
            }
        } else { //otherwise we need to get the interfaceID from the system db and set this
                 //as an external default interface ID in the user db
            if (!openDb(SystemScope))
                return false;

            QString interfaceDescriptorID = m_systemDb->getInterfaceID(descriptor);
            if (m_systemDb->lastError().errorCode() == DBError::NoError) {
                if(m_userDb->setDefaultService(descriptor, interfaceDescriptorID)) {
                    m_lastError.setError(DBError::NoError);
                    return true;
                } else {
                    m_lastError = m_userDb->lastError();
                    return false;
                }
            } else {
                m_lastError = m_systemDb->lastError();
                return false;
            }
        }
    } else {  //scope == SystemScope
        if (!descriptor.inSystemScope()) {
            QString errorText("Cannot set default service at system scope with a user scope "
                                "interface descriptor");
            m_lastError.setError(DBError::InvalidDescriptorScope, errorText);
            return false;
        } else {
            if (!openDb(SystemScope)) {
                return false;
            } else {
                if (m_systemDb->setDefaultService(descriptor)) {
                    m_lastError.setError(DBError::NoError);
                    return true;
                } else {
                    m_lastError = m_systemDb->lastError();
                    return false;
                }
            }
        }
    }
}

bool DatabaseManager::openDb(DbScope scope)
{
    if (scope == SystemScope && m_systemDb->isOpen() && !QFile::exists(m_systemDb->databasePath())) {
        delete m_systemDb;
        m_systemDb = new ServiceDatabase;
        initDbPath(SystemScope);
    } else if (scope != SystemScope && m_userDb->isOpen() && !QFile::exists(m_userDb->databasePath())) {
        delete m_userDb;
        m_userDb = new ServiceDatabase;
        initDbPath(UserScope);
    }

    ServiceDatabase *db;
    if (scope == SystemScope)
        db = m_systemDb;
    else
        db = m_userDb;

    if (db->isOpen())
        return true;

    bool isOpen = db->open();
    if (!isOpen) {
        QString errorText("Unable to open service framework database: %1");
        if (scope == SystemScope)
            m_lastError.setError(DBError::CannotOpenSystemDb,
                                errorText.arg(db->databasePath()));
        else
            m_lastError.setError(DBError::CannotOpenUserDb,
                                errorText.arg(db->databasePath()));

#ifdef QT_SFW_SERVICEDATABASE_DEBUG
        qWarning() << "DatabaseManger::openDb():-"
                    << "Problem:" << qPrintable(m_lastError.text());
#endif
        return false;
    }

    if (scope == SystemScope && m_userDb->isOpen()) {
        QStringList interfaceIDs = m_userDb->externalDefaultInterfaceIDs();
        QServiceInterfaceDescriptor interface;
        foreach( const QString &interfaceID, interfaceIDs ) {
            interface = m_userDb->getInterface(interfaceID);
            if (m_userDb->lastError().errorCode() == DBError::NotFound)
                m_userDb->removeExternalDefaultServiceInterface(interfaceID);
        }
    }

    m_lastError.setError(DBError::NoError);
    return true;
}

void DatabaseManager::setChangeNotificationsEnabled(DbScope scope, bool enabled)
{
    if (!m_fileWatcher)
        m_fileWatcher = new DatabaseFileWatcher(this);
    m_fileWatcher->setEnabled(scope == SystemScope ? m_systemDb : m_userDb, enabled);
}

QT_END_NAMESPACE
