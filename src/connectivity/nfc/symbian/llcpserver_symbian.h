

#ifndef LLCPSERVER_SYMBIAN_H_
#define LLCPSERVER_SYMBIAN_H_

#include <e32base.h>
#include <nfcserver.h>                      // RNfcServer
#include <llcpprovider.h>                   // CLlcpProvider
#include <llcpconnorientedlistener.h>       // MLlcpConnOrientedListener

class CLlcpSocketType2;

#include <qmobilityglobal.h>
#include "../qllcpserver_symbian_p.h"

/*!
 *  CLASS DECLARATION for CLlcpSocketPrivate.
 */
class CLlcpServer : public CBase,
                           public MLlcpConnOrientedListener
   {
public:
   /*!
    * Creates a new CLlcpServer object.
    */
   static CLlcpServer* NewL(QtMobility::QLlcpServerPrivate&);
   
   /*!
    * Creates a new CLlcpServer object.
    */
   static CLlcpServer* NewLC(QtMobility::QLlcpServerPrivate&);
   
   /*!
    * Destructor
    */
   ~CLlcpServer();
   
public:    
   bool Listen( const TDesC8& aServiceName);
   void StopListening();
   bool isListening() const;
   CLlcpSocketType2 *nextPendingConnection();
   bool hasPendingConnections() const;
   const TDesC8& serviceUri() const;
    
private: // From MLlcpConnOrientedListener   
    void RemoteConnectRequest( MLlcpConnOrientedTransporter* aConnection ); 
    
private:
    // Constructor
    CLlcpServer(QtMobility::QLlcpServerPrivate&);
    
    // Second phase constructor
    void ConstructL();
    
private:
   
    RPointerArray<CLlcpSocketType2>  iLlcpSocketArray;
    //CLlcpSocketType2 *iLlcpSocket;
    
   /*!
    * Handle to NFC-server.
    * Own.
    */ 
   RNfcServer iNfcServer;
   
   /*!
    * Pointer to CLlcpProvider object.
    * Own.
    */
   CLlcpProvider* iLlcp;
   
   TBool iSocketListening;
   
   RBuf8 iServiceName;
   
   QtMobility::QLlcpServerPrivate& iCallback;
   };
    
#endif /* LLCPSERVER_SYMBIAN_H_ */
