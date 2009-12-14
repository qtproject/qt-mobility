#include "RSatSession_etel_test_server.h"
#include "RSatRefresh_etel_test_server.h"

class RSatRefresh::CNotifier : public CActive
    {
    public:
        CNotifier( MSatRefreshObserver& aObserver, const RTelServer& aTelServer );
        ~CNotifier();
        void RunL();
        void DoCancel();
        void Activate();
        void Activate( const TSatRefreshFiles& aObservedFiles );
    private:
        MSatRefreshObserver& iObserver;
        const RTelServer& iTelServer;
        TSatRefreshFiles iFiles;
        TSatRefreshType iType;
        TPtr8 iTypePtr;
    };

RSatRefresh::CNotifier::CNotifier( MSatRefreshObserver& aObserver,
    const RTelServer& aTelServer )
    :   CActive( EPriorityStandard ),
        iObserver( aObserver ),
        iTelServer( aTelServer ),
        iTypePtr( NULL,0,0 )
    {
    CActiveScheduler::Add( this );
    }

RSatRefresh::CNotifier::~CNotifier()
    {
    Cancel();
    }

void RSatRefresh::CNotifier::RunL()
    {
    if ( iStatus == KErrNone )
        {
        if ( iObserver.AllowRefresh( iType, iFiles ) )
            {
            iObserver.Refresh( iType, iFiles );
            }
        }
    else
        {
        User::LeaveIfError( iStatus.Int() );
        }
    }

void RSatRefresh::CNotifier::DoCancel()
    {
    iTelServer.CancelSatRefreshNotification();
    }

void RSatRefresh::CNotifier::Activate()
    {
    TPckg<TSatRefreshType> pckg( iType );
    iTypePtr.Set( pckg );
    iTelServer.NotifySatRefresh( iStatus, iFiles, iTypePtr );
    SetActive();
    }

void RSatRefresh::CNotifier::Activate( const TSatRefreshFiles& aObservedFiles )
    {
    TPckg<TSatRefreshType> pckg( iType );
    iTypePtr.Set( pckg );
    iTelServer.NotifySatRefresh( iStatus, iFiles, iTypePtr, aObservedFiles );
    SetActive();
    }

RSatSession::RSatSession()
    {
    }

void RSatSession::ConnectL()
    {
    User::LeaveIfError( iTelServer.Connect() );
    }

void RSatSession::Close()
    {
    iTelServer.Close();
    RSessionBase::Close();
    }

TVersion RSatSession::Version() const
    {
    return TVersion( 1,0,0 );
    }

RSatRefresh::RSatRefresh( MSatRefreshObserver& aObserver ) :
iObserver( &aObserver )
    {
    }

void RSatRefresh::OpenL( const RSatSession& aSession )
    {
    iSatSession = &aSession;
    if ( !iNotifier && iObserver )
        {
        iNotifier = new( ELeave ) CNotifier( *iObserver, iSatSession->TelServer() );
        }
    }

void RSatRefresh::NotifyFileChangeL()
    {
    if ( iNotifier && !iNotifier->IsActive() )
        {
        iNotifier->Activate();
        }
    }

void RSatRefresh::NotifyFileChangeL( const TSatRefreshFiles& aObservedFiles )
    {
    if ( iNotifier && !iNotifier->IsActive() )
        {
        iNotifier->Activate( aObservedFiles );
        }
    }

void RSatRefresh::Close()
    {
    delete iNotifier;
    iNotifier = NULL;
    }

void RSatRefresh::Cancel() const
    {
    if ( iNotifier )
        {
        iNotifier->Cancel();
        }
    }

void RSatRefresh::RefreshEFRead( TBool /*aAdditionEFRead*/ )
    {
    // Not needed to send to server
    }

void RSatRefresh::LaunchNotification( TSatRefreshType aType,
    TSatRefreshFiles& aFiles )
    {
    iSatSession->TelServer().MakeSatRefresh( aFiles, aType );
    }


