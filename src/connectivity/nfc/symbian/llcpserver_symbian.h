

#ifndef LLCPSERVER_SYMBIAN_H_
#define LLCPSERVER_SYMBIAN_H_

#include <e32base.h>
#include <nfcserver.h>                      // RNfcServer
#include <llcpprovider.h>                   // CLlcpProvider
#include <llcpconnorientedlistener.h>       // MLlcpConnOrientedListener

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
   bool isListening() const;
   CLlcpSocketType2 *nextPendingConnection();
   bool hasPendingConnections() const;
    
private: // From MLlcpConnOrientedListener   
    void RemoteConnectRequest( MLlcpConnOrientedTransporter* aConnection ); 
    
private:
    // Constructor
    CLlcpServer();
    
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
   };
    
#endif /* LLCPSERVER_SYMBIAN_H_ */
