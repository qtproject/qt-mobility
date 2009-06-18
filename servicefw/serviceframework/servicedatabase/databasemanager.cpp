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

#include "databasemanager.h"
#include <qserviceinterfacedescriptor_p.h>

#include <QFileSystemWatcher>
#include <QHash>

QT_BEGIN_NAMESPACE

DatabaseFileWatcher::DatabaseFileWatcher(DatabaseManager *parent)
    : QObject(parent),
      manager(parent),
      watcher(0)
{
}

void DatabaseFileWatcher::setEnabled(ServiceDatabase *database, bool enabled)
{
    if (!watcher) {
        watcher = new QFileSystemWatcher(this);
        connect(watcher, SIGNAL(fileChanged(QString)),
            SLOT(databaseChanged(QString)));
        connect(watcher, SIGNAL(directoryChanged(QString)),
            SLOT(databaseDirectoryChanged(QString)));
    }

    if (enabled) {
        QString path = database->databasePath();
        if (!QFile::exists(path)) {
            watcher->addPath(QFileInfo(path).absolutePath());   //directory
        } else {
            knownServices[path] = database->getServiceNames(QString());
            watcher->addPath(path);
        }
    } else {
        watcher->removePath(database->databasePath());
    }
}

void DatabaseFileWatcher::databaseDirectoryChanged(const QString &path)
{
    ServiceDatabase *db = 0;
    DatabaseManager::DbScope scope;
    if (path == QFileInfo(manager->m_userDb.databasePath()).absolutePath()
            && QFile::exists(manager->m_userDb.databasePath())) {
        db = &manager->m_userDb;
        scope = DatabaseManager::UserOnlyScope;
    } else if (path == QFileInfo(manager->m_systemDb.databasePath()).absolutePath()
            && QFile::exists(manager->m_systemDb.databasePath())) {
        db = &manager->m_systemDb;
        scope = DatabaseManager::SystemScope;
    }

    if (db) {
        setEnabled(db, true);
        watcher->removePath(path);
        QStringList newServices = manager->getServiceNames(QString(), scope);
        for (int i=0; i<newServices.count(); i++)
            emit manager->serviceAdded(newServices[i], scope);
    }
}

void DatabaseFileWatcher::databaseChanged(const QString &path)
{
    if (path == manager->m_userDb.databasePath())
        notifyChanges(&manager->m_userDb, DatabaseManager::UserScope);
    else if (path == manager->m_systemDb.databasePath())
        notifyChanges(&manager->m_systemDb, DatabaseManager::SystemScope);
}

void DatabaseFileWatcher::notifyChanges(ServiceDatabase *database, DatabaseManager::DbScope scope)
{
    bool ok = false;
    QStringList currentServices = database->getServiceNames(QString(), &ok);
    if (!ok) {
            qWarning("QServiceManager: failed to get current service names for serviceAdded() and serviceRemoved() signals");
        return;
    }

    QString dbPath = database->databasePath();
    const QStringList &knownServicesRef = knownServices[dbPath];

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

    knownServices[dbPath] = currentServices;
    for (int i=0; i<newServices.count(); i++)
        emit manager->serviceAdded(newServices[i], scope);
    for (int i=0; i<removedServices.count(); i++)
        emit manager->serviceRemoved(removedServices[i], scope);
}



DatabaseManager::DatabaseManager()
    : m_fileWatcher(0)
{
    initDbPaths();
}

DatabaseManager::~DatabaseManager()
{
    close();
}

void DatabaseManager::initDbPaths()
{
    //setup system database path
    QSettings systemSettings(QSettings::IniFormat, QSettings::SystemScope,
            QLatin1String("Nokia"), QLatin1String("QtServiceFramework"));
    QFileInfo fi(systemSettings.fileName());
    QDir dir = fi.dir();

    QString qtVersion(qVersion());
    qtVersion = qtVersion.left(qtVersion.size() -2); //strip off patch version
    QString dbName = QString("QtServiceFW_") + qtVersion + QLatin1String("_system.db");
    m_systemDb.setDatabasePath(dir.path() + QDir::separator() + dbName);

    //setup user database path
    QSettings userSettings(QSettings::IniFormat, QSettings::UserScope,
            QLatin1String("Nokia"), QLatin1String("QtServiceFramework"));
    fi.setFile(userSettings.fileName());
    dir = fi.dir();
    dbName.replace(QLatin1String("system"), QLatin1String("user"));
    m_userDb.setDatabasePath(dir.path() + QDir::separator()  + dbName);
}

void DatabaseManager::close()
{
    m_systemDb.close();
    m_userDb.close();
}

bool DatabaseManager::registerService(ServiceMetaData &service, DbScope scope)
{
    if (scope == DatabaseManager::SystemScope) {
        if(!openDb(DatabaseManager::SystemScope)) {
            return false;
        }  else {
            if (!m_systemDb.registerService(service)) {
                m_lastError = m_systemDb.lastError();
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
            if (!m_userDb.registerService(service)) {
                m_lastError = m_userDb.lastError();
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
            if (!m_systemDb.unregisterService(serviceName)) {
                m_lastError = m_systemDb.lastError();
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
            if (!m_userDb.unregisterService(serviceName)){
                m_lastError = m_userDb.lastError();
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
        descriptors =  m_userDb.getInterfaces(filter, &ok);
        if (ok == false) {
            descriptors.clear();
            m_lastError = m_userDb.lastError();
            return descriptors;
        }

        userDescriptorCount = descriptors.count();
        for (int i=0; i < userDescriptorCount; ++i) {
            descriptors[i].d->systemScope = false;
        }
    }

    if (openDb(SystemScope)) {
        descriptors.append(m_systemDb.getInterfaces(filter, &ok));
        if (ok == false) {
            descriptors.clear();
            m_lastError = m_systemDb.lastError();
            return descriptors;
        }

        for (int i = userDescriptorCount; i < descriptors.count(); ++i) {
            descriptors[i].d->systemScope = true;
        }
        m_lastError.setError(DBError::NoError);
    }

    if (!m_systemDb.isOpen() && scope == SystemScope) {
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
        serviceNames = m_userDb.getServiceNames(interfaceName, &ok);
        if(!ok) {
            serviceNames.clear();
            m_lastError = m_userDb.lastError();
            return serviceNames;
        }
        if (scope == UserOnlyScope) {
            m_lastError.setError(DBError::NoError);
            return serviceNames;
        }
    }

    if(openDb(DatabaseManager::SystemScope)) {
        QStringList systemServiceNames;
        systemServiceNames = m_systemDb.getServiceNames(interfaceName, &ok);
        if(!ok) {
            serviceNames.clear();
            m_lastError = m_systemDb.lastError();
            return serviceNames;
        }
        foreach(const QString &systemServiceName, systemServiceNames) {
            if(!serviceNames.contains(systemServiceName, Qt::CaseInsensitive))
                serviceNames.append(systemServiceName);
        }

        m_lastError.setError(DBError::NoError);
    }

    if (!m_systemDb.isOpen() && scope == SystemScope) {
        //openDb() should have already handled lastError
        serviceNames.clear();
        return serviceNames;
    }

    //Note:if the system database could not be opened at user scope,
    //we continue on and just use the results for the user database
    return serviceNames;
}

bool DatabaseManager::openDb(DbScope scope)
{
    ServiceDatabase *db;
    if (scope == SystemScope)
        db = &m_systemDb;
    else
        db = &m_userDb;

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

    m_lastError.setError(DBError::NoError);
    return true;
}

void DatabaseManager::setChangeNotificationsEnabled(DbScope scope, bool enabled)
{
    if (!m_fileWatcher)
        m_fileWatcher = new DatabaseFileWatcher(this);
    m_fileWatcher->setEnabled(scope == SystemScope ? &m_systemDb : &m_userDb, enabled);
}

QT_END_NAMESPACE
