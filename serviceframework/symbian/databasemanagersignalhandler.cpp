#include "databasemanagersignalhandler.h"
#include "clientservercommon.h"

DatabaseManagerSignalHandler::DatabaseManagerSignalHandler(CDatabaseManagerServerSession& databaseManagerServerSession) 
    : iDatabaseManagerServerSession(databaseManagerServerSession)
{
}

DatabaseManagerSignalHandler::~DatabaseManagerSignalHandler()
{
}

void DatabaseManagerSignalHandler::ServiceAdded(const QString& aServiceName, DatabaseManager::DbScope aScope)
{
    iDatabaseManagerServerSession.ServiceAdded(aServiceName, aScope);
}

void DatabaseManagerSignalHandler::ServiceRemoved(const QString& aServiceName, DatabaseManager::DbScope aScope)
{
    iDatabaseManagerServerSession.ServiceRemoved(aServiceName, aScope);
}

