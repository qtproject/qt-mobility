

#ifndef LLCPSOCKET_SYMBIAN_H_
#define LLCPSOCKET_SYMBIAN_H_

#include <e32base.h>
#include <nfcserver.h>                      // RNfcServer
#include <llcpprovider.h>                   // CLlcpProvider
#include <llcpconnlesstransporter.h>        // MLlcpConnLessTransporter
#include <llcpconnorientedtransporter.h>    // MLlcpConnOrientedTransporter
#include <llcpconnlesslistener.h>           // MLlcpConnLessListener
#include <llcpconnorientedlistener.h>       // MLlcpConnOrientedListener
#include <llcplinklistener.h>               // MLlcpLinkListener
#include <typeinfo>

/*!
 *   FORWARD DECLARATIONS
 */
class AbstractConnection;
class COwnLlcpConnLess;
class COwnLlcpConnOriented;

/*!
 *  CLASS DECLARATION for CLlcpSocketPrivate.
 */
class CLlcpSocketPrivate : public CBase,
                           public MLlcpConnLessListener,
                           public MLlcpConnOrientedListener
   {
public:
   /*!
    * Creates a new CLlcpSocketPrivate object.
    */
   static CLlcpSocketPrivate* NewL();
   
   /*!
    * Creates a new CLlcpSocketPrivate object.
    */
   static CLlcpSocketPrivate* NewLC();
   
   /*!
    * Destructor
    */
   ~CLlcpSocketPrivate();
   
public:    
   void ConnectToService( const TDesC8& aServiceName);
   void DisconnectFromService();
   TInt WriteDatagram(const TDesC8& aData);
   
private: // From MLlcpConnLessListener
    void FrameReceived( MLlcpConnLessTransporter* aConnection );
    
private: // From MLlcpConnOrientedListener   
    void RemoteConnectRequest( MLlcpConnOrientedTransporter* aConnection );    
    
private:
    // Constructor
    CLlcpSocketPrivate();
    
    // Second phase constructor
    void ConstructL();
    
    void Cleanup();
    
    
private:
    enum ConnType {
        ConnectionLess = 1,
        ConnectionOriented = 2    
    };
    void CreateLocalConnection(ConnType type);
    void CreateRemoteConnection(MLlcpConnLessTransporter* aConnection);
    void CreateRemoteConnection(MLlcpConnOrientedTransporter* aConnection);
    
private:
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
   
   /*!
    * Pointer to MLlcpConnLessTransporter object.
    * Own.
    *
    * This is used to send data to local device.
    */ 
   AbstractConnection* iLocalConnection;
   
   /*!
    * Pointer to MLlcpConnLessTransporter object.
    * Own.
    *
    * This is used to send data to local device.
    */
   //COwnLlcpConnLess* iRemoteConnless;
   
   AbstractConnection* iRemoteConnection;   
   
   /*!
    * Boolean variables, which tell whether Connenctionless listen started or not.
    */ 
   TBool iConnLessStarted;
      
   /*!
    * Boolean variables, which tell whether ConnectionOriented listen started or not.
    */ 
   TBool iConnOrientedStarted;   
   
   };
    
class AbstractConnection
    {

public:
    /*!
     * Transfer given data to remote device.
     */
    virtual TInt Transfer( const TDesC8& aData ) = 0;
     
    /*!
     * Starts receive data from ConnLess.
     */
    virtual TInt Receive() = 0;    
    
    /*!
     * Cancels Tranfer() request.
     */ 
    virtual void TransferCancel() = 0;
     
    /*!
     * Cancels Receive() request.
     */ 
    virtual void ReceiveCancel() = 0;          
    };



/*!
 *  CLASS DECLARATION for COwnLlcpConnLess.
 *
 */    
class COwnLlcpConnLess : public CActive,
                         public AbstractConnection
    {
public:

   /*!
    * Creates a new COwnLlcpConnection object.
    */
   static COwnLlcpConnLess* NewL( MLlcpConnLessTransporter* aConnection );
   
   /*!
    * Creates a new COwnLlcpConnection object.
    */
   static COwnLlcpConnLess* NewLC( MLlcpConnLessTransporter* aConnection );
   
   /*!
    * Destructor.
    */
   ~COwnLlcpConnLess();

public: //  From AbstractConnection
   /*!
    * Transfer given data to remote device.
    */
   TInt Transfer( const TDesC8& aData );
   
   /*!
    * Cancels COwnLlcpConnection::Tranfer() request.
    */ 
   void TransferCancel();
   
   /*!
    * Starts receive data from ConnLess.
    */
   TInt Receive();
   
   /*!
    * Cancels COwnLlcpConnection::Receive() request.
    */ 
   void ReceiveCancel();
   
public: // From CActive

    void RunL();
    
    void DoCancel();
   
private:

    // Constructor
    COwnLlcpConnLess( MLlcpConnLessTransporter* aConnection );
    
    // Second phase constructor
    void ConstructL();
    
private:
    
    enum TActionState
        {
        EIdle,
        EReceiving,
        ETransmitting
        };
    
    /*!
         Pointer to MLlcpConnLessTransporter object.
     */ 
    MLlcpConnLessTransporter* iConnection;

    /*!
         Buffered data for transmitting data.
     */ 
    RBuf8 iTransmitBuf;
    
    /*!
     * Buffered data for receiving data
     */
    RBuf8 iReceiveBuf;
    
    TActionState iActionState;
    };


/*!
 *  CLASS DECLARATION for COwnLlcpConnOriented.
 *
 */    
class COwnLlcpConnOriented : public CActive,
                             public AbstractConnection
    {
public:

   /*!
    * Creates a new COwnLlcpConnection object.
    */
   static COwnLlcpConnOriented* NewL( MLlcpConnOrientedTransporter* aConnection );
   
   /*!
    * Creates a new COwnLlcpConnection object.
    */
   static COwnLlcpConnOriented* NewLC( MLlcpConnOrientedTransporter* aConnection );
   
   /*!
    * Destructor.
    */
   ~COwnLlcpConnOriented();

public: //  From AbstractConnection
   /*!
        Transfer given data to remote device.
    */
   TInt Transfer( const TDesC8& aData );
   
   /*!
        Cancels COwnLlcpConnection::Tranfer() request.
    */ 
   void TransferCancel();
   
   /*!
        Transfer given data to remote device.
    */
   TInt Receive(); // Replace StartReceive
   
   /*!
        Cancels COwnLlcpConnection::Tranfer() request.
    */ 
   void ReceiveCancel();
   
public: // From CActive

    void RunL();
    
    void DoCancel();
   
private:

    // Constructor
    COwnLlcpConnOriented( MLlcpConnOrientedTransporter* aConnection );
    
    // Second phase constructor
    void ConstructL();
    
private:
    
    enum TConnectionState
        {
        ENotConnected,
        EConnecting,
        EConnected
        };
    
    enum TActionState
        {
        EIdle,
        EReceiving,
        ETransmitting
        };
    
    /*!
          Pointer to MLlcpConnOrientedTransporter object.
     */ 
    MLlcpConnOrientedTransporter* iConnection;
    
    
    /*!
     * Pointer to TRequestStatus object.
     * Not own.
     */
    TRequestStatus* iClientStatus;
    
    /*!
     * Buffered data for transmitting data.
     */ 
    RBuf8 iTransmitBuf;
    
    /*!
          Buffered data for receiving data
     */
    RBuf8 iReceiveBuf;
    
    /*!
          State of LLCP connection object.
     */
    TConnectionState iConnState;
    
    TActionState iActionState;
    };
  

#endif /* LLCPSOCKET_SYMBIAN_H_ */
