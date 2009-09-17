#ifndef DATABASEMANAGERSERVER_PAN_
#define DATABASEMANAGERSERVER_PAN_

// Panic category
_LIT(KDatabaseManagerServer, "DatabaseManagerServer");
_LIT(KUnknownOpCode, "Unknown op code");
_LIT(KBadDescriptor, "Bad descriptor");

enum TDatabaseManagerSerververPanic
{
    EBadRequest = 1,
    ESrvCreateServer = 2,
    ESvrStartServer = 3
};

#endif

// End of File
