

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

/*!
 *   FORWARD DECLARATIONS
 */
class AbstractConnection;
class COwnLlcpConnLess;
class COwnLlcpConnOriented;

/*!
 *  CLASS DECLARATION for CLlcpSocketType1 (ConnectLess Tran).
 */
class CLlcpSocketType1 : public CBase,
                         public MLlcpConnLessListener
   {
public:
   /*!
    * Creates a new CLlcpSocketType1 object.
    */
   static CLlcpSocketType1* NewL();
   
   /*!
    * Creates a new CLlcpSocketType1 object.
    */
   static CLlcpSocketType1* NewLC();
   
   /*!
    * Destructor
    */
   ~CLlcpSocketType1();
   
public:    
   TInt WriteDatagram(const TDesC8& aData,TInt8 portNum);
   TInt ReadDatagram(TInt64 aMaxSize);
   bool Bind(TInt8 portNum);
   
private: // From MLlcpConnLessListener
    void FrameReceived( MLlcpConnLessTransporter* aConnection );
    
private:
    // Constructor
    CLlcpSocketType1();
    
    // Second phase constructor
    void ConstructL();  
    void Cleanup();   
    
    void CreateLocalConnection(TInt8 portNum);
    void CreateRemoteConnection(MLlcpConnLessTransporter* aConnection);
    
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
   COwnLlcpConnLess* iLocalConnection;
   
   /*!
    * Pointer to MLlcpConnLessTransporter object.
    * Own.
    *
    * This is used to send data to local device.
    */
   //COwnLlcpConnLess* iRemoteConnless;
   
   COwnLlcpConnLess* iRemoteConnection;   
   
   bool iConnLessStarted;
   
   
   };



/*!
 *  CLASS DECLARATION for CLlcpSocketType1 (ConnectLess Tran).
 */
class CLlcpSocketType2 : public CBase
   {
public:
   /*!
    * Creates a new CLlcpSocketType1 object.
    */
   static CLlcpSocketType2* NewL();
   
   /*!
    * Creates a new CLlcpSocketType1 object.
    */
   static CLlcpSocketType2* NewLC();
   
   /*!
    * Destructor
    */
   ~CLlcpSocketType2();
   
public:    
   void ConnectToService( const TDesC8& aServiceName);
   void DisconnectFromService();
   TInt WriteDatagram(const TDesC8& aData);
   TInt ReadDatagram(TInt64 aMaxSize);
   
public:
   COwnLlcpConnOriented* RemoteConnection() const;
   void CreateRemoteConnection(MLlcpConnOrientedTransporter* aConnection);
   
private:
    // Constructor
   CLlcpSocketType2();
    
    // Second phase constructor
    void ConstructL();
    
    void Cleanup();   
    
    void CreateLocalConnection(const TDesC8& aServiceName);

    
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
   COwnLlcpConnOriented* iLocalConnection;
   
   /*!
    * Pointer to MLlcpConnLessTransporter object.
    * Own.
    *
    * This is used to send data to local device.
    */
   //COwnLlcpConnLess* iRemoteConnless;
   
   COwnLlcpConnOriented* iRemoteConnection;   
   
   };
 

/*!
 *  CLASS DECLARATION for COwnLlcpConnLess.
 *
 */    
class COwnLlcpConnLess : public CActive
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

public: 
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
   TInt Receive(TInt64 aMaxSize);
   
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
class COwnLlcpConnOriented : public CActive
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
   
   TInt Connect(const TDesC8& aServiceName);
   
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
   TInt Receive(TInt64 aMaxSize); // Replace StartReceive
   
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
