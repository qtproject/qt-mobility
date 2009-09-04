#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "databasemanagerserver.h"
#include "clientservercommon.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    
    qDebug("Attempting to create CDatabaseManagerServer");
    
    CDatabaseManagerServer* server = new CDatabaseManagerServer;
    TInt err = server->Start(KDatabaseManagerServerName);
    if (err != KErrNone)
    {
        CDatabaseManagerServer::PanicServer(ESvrStartServer);
    }
    RProcess::Rendezvous(err);

    return app.exec();
}

