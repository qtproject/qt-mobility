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

#ifndef DATABASEMANAGER_H_
#define DATABASEMANAGER_H_

#include "servicedatabase.h"
#include <QObject>
#include "qserviceglobal.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class DatabaseFileWatcher;

class Q_SFW_EXPORT DatabaseManager : public QObject
{
    Q_OBJECT

    public:
        enum DbScope{UserScope, SystemScope, UserOnlyScope};
        DatabaseManager(void);
        virtual ~DatabaseManager();

        bool registerService(ServiceMetaData &service, DbScope scope);
        bool unregisterService(const QString &serviceName, DbScope scope);

        QList<QServiceInterfaceDescriptor> getInterfaces(const QServiceFilter &filter, DbScope scope);
        QStringList getServiceNames(const QString &interfaceName, DbScope scope);

      QServiceInterfaceDescriptor defaultServiceInterface(const QString &interfaceName, DbScope scope);
//      bool setDefaultService(const QString &serviceName, const QString &interfaceName);
      bool setDefaultService(const QServiceInterfaceDescriptor &interface, DbScope scope);

        DBError lastError(){ return m_lastError;}

        void setChangeNotificationsEnabled(DbScope scope, bool enabled);

    signals:
        void serviceAdded(const QString &serviceName, DatabaseManager::DbScope scope);
        void serviceRemoved(const QString &serviceName, DatabaseManager::DbScope scope);

    private:
        void initDbPaths();
        bool openDb(DbScope scope);
        void close();

        ServiceDatabase m_userDb;
        ServiceDatabase m_systemDb;
        DBError m_lastError;

        friend class DatabaseFileWatcher;
        DatabaseFileWatcher *m_fileWatcher;
};


class Q_SFW_EXPORT DatabaseFileWatcher : public QObject
{
    Q_OBJECT
public:
    DatabaseFileWatcher(DatabaseManager *parent = 0);

    void setEnabled(ServiceDatabase *database, bool enabled);

private slots:
    void databaseChanged(const QString &path);
    void databaseDirectoryChanged(const QString &path);

private:
    void notifyChanges(ServiceDatabase *database, DatabaseManager::DbScope scope);

    DatabaseManager *manager;
    QFileSystemWatcher *watcher;
    QHash<QString, QStringList> knownServices;
};

QT_END_NAMESPACE
QT_END_HEADER

#endif
