#ifndef CDATABASEMANAGERSESSION_H_
#define CDATABASEMANAGERSESSION_H_

#include <e32base.h>
#include <QObject>
#include "databasemanager_p.h"

class CDatabaseManagerServer;
class DatabaseManagerSignalHandler;

typedef TPckgBuf<TInt> TError;

class CDatabaseManagerServerSession : public CSession2
    {
    public:
        static CDatabaseManagerServerSession* NewL();
        static CDatabaseManagerServerSession* NewLC();
        virtual ~CDatabaseManagerServerSession();
    
        void ServiceL(const RMessage2& aMessage);
        void DispatchMessageL(const RMessage2& aMessage);
        
        TInt RegisterServiceL(const RMessage2& aMessage);
        TInt UnregisterServiceL(const RMessage2& aMessage);
        TInt InterfacesL(const RMessage2& aMessage);
        TInt ServiceNamesL(const RMessage2& aMessage);
        TInt InterfaceDefaultL(const RMessage2& aMessage);
        TInt SetInterfaceDefaultL(const RMessage2& aMessage);
        TInt SetInterfaceDefault2L(const RMessage2& aMessage);
        void NotifyServiceSignal(const RMessage2& aMessage);
        TInt CancelNotifyServiceSignal(const RMessage2& aMessage);
        void SetChangeNotificationsEnabled(const RMessage2& aMessage);
        TInt InterfaceDefaultSize(const RMessage2& aMessage);
        TInt InterfacesSizeL(const RMessage2& aMessage);
        TInt ServiceNamesSizeL(const RMessage2& aMessage);
        
        void ServiceRemoved(const QString& aServiceName, DatabaseManager::DbScope aScope);
        void ServiceAdded(const QString& aServiceName, DatabaseManager::DbScope aScope);
        
    private:
        CDatabaseManagerServerSession();
        void ConstructL();
        TError LastErrorCode();
            
    protected:
        void PanicClient(const RMessage2& aMessage, TInt aPanic) const;
        
    private:
        QByteArray* iByteArray;
        TBool iWaitingAsyncRequest;
        RMessage2 iMsg;
        DatabaseManager* iDatabaseManager;
        DatabaseManagerSignalHandler* iDatabaseManagerSignalHandler;
    };

#endif


// End of File
