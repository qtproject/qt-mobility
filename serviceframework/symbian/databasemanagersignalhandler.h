#ifndef DATABASEMANAGERSIGNALHANDLER_H
#define DATABASEMANAGERSIGNALHANDLER_H

#include <QObject>
#include "databasemanager_p.h"
#include "databasemanagersession.h"

class DatabaseManagerSignalHandler : public QObject
    {
    Q_OBJECT
    
    public:
        DatabaseManagerSignalHandler(CDatabaseManagerServerSession& databaseManagerServerSession);
        ~DatabaseManagerSignalHandler();

    public Q_SLOTS:
        void ServiceAdded(const QString& aServiceName, DatabaseManager::DbScope aScope);
        void ServiceRemoved(const QString& aServiceName, DatabaseManager::DbScope aScope);
        
    public:
        CDatabaseManagerServerSession& iDatabaseManagerServerSession;
    };

#endif

// End of File

