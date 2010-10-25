

#ifndef LLCPSERVER_SYMBIAN_H_
#define LLCPSERVER_SYMBIAN_H_

#include <e32base.h>
#include <nfcserver.h>                      // RNfcServer
#include <llcpprovider.h>                   // CLlcpProvider
#include <llcpconnlesslistener.h>           // MLlcpConnLessListener
#include <llcpconnorientedlistener.h>       // MLlcpConnOrientedListener
#include <llcplinklistener.h>               // MLlcpLinkListener

class CLlcpSocketType2;

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
   static CLlcpServer* NewL();
   
   /*!
    * Creates a new CLlcpServer object.
    */
   static CLlcpServer* NewLC();
   
   /*!
    * Destructor
    */
   ~CLlcpServer();
   
public:    
   void Listen( const TDesC8& aServiceName);
    
private: // From MLlcpConnOrientedListener   
    void RemoteConnectRequest( MLlcpConnOrientedTransporter* aConnection ); 
    void CreateLocalConnection();
    
private:
    // Constructor
    CLlcpServer();
    
    // Second phase constructor
    void ConstructL();
    
private:
   
    RPointerArray<CLlcpSocketType2>  iPendingConnections;
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
   
   };
    
#endif /* LLCPSERVER_SYMBIAN_H_ */
