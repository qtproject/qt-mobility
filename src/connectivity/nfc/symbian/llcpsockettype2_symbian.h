

#ifndef LLCPSOCKETTYPE2_SYMBIAN_H_
#define LLCPSOCKETTYPE2_SYMBIAN_H_

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
class COwnLlcpConnOriented;

#include <qmobilityglobal.h>
#include "../qllcpsocket_symbian_p.h"

/*!
 *  CLASS DECLARATION for CLlcpSocketType2 (ConnectOriented Transportation).
 */
class CLlcpSocketType2 : public CBase
   {
public:
   /*!
    * Creates a new CLlcpSocketType2 object.
    */
   static CLlcpSocketType2* NewL(QtMobility::QLlcpSocketPrivate* aCallback = NULL);
   
   /*!
    * Creates a new CLlcpSocketType2 object.
    */
   static CLlcpSocketType2* NewLC(QtMobility::QLlcpSocketPrivate* aCallback = NULL);
   
   /*!
    * Destructor
    */
   ~CLlcpSocketType2();
   
public:    
   void ConnectToService( const TDesC8& aServiceName);
   void DisconnectFromService();
   TInt StartWriteDatagram(const TDesC8& aData);
   TInt StartReadDatagram(TInt64 aMaxSize);
   bool TransferCompleted();
   bool ReceiveCompleted();
   bool ReceiveData(TDesC8& aData);
   
private:
    // Constructor
    CLlcpSocketType2(QtMobility::QLlcpSocketPrivate* aCallback = NULL);
    // Second phase constructor
    void ConstructL();   
    void Cleanup();
 
public:
   TInt CreateRemoteConnection(MLlcpConnOrientedTransporter* aConnection);
   TInt CreateLocalConnection(const TDesC8& aServiceName);

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
   COwnLlcpConnOriented* iRemoteConnection;   
   
   QtMobility::QLlcpSocketPrivate*  iCallback; // not own 
   
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

public: 
   /*!
    * Disonnect with remote peer .
    */    
   void Disconnect();
   
   /*!
    * Connect to remote peer as given service uri.
    */
   void Connect(const TDesC8& aServiceName);
   
   /*!
        Transfer given data to remote device.
    */
   TInt Transfer( const TDesC8& aData );
   
   /*!
        Cancels COwnLlcpConnOriented::Tranfer() request.
    */ 
   void TransferCancel();
   
   /*!
        Transfer given data to remote device.
    */
   TInt Receive(TInt64 aMaxSize); 
   
   /*!
        Cancels COwnLlcpConnection::Tranfer() request.
    */ 
   void ReceiveCancel();
   
   bool ReceiveCompeleted();
   bool TransferCompleted();
   
   const TDesC8& ReceiveData() const;
   
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
 




#endif /* LLCPSOCKETTYPE2_SYMBIAN_H_ */
