

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
class CLlcpConnecterAO;
class CLlcpSenderAO;
class CLlcpReceiverAO;

#include <qmobilityglobal.h>
#include "../qllcpsocket_symbian_p.h"
#include "../qllcpsocket.h"

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
   
   static CLlcpSocketType2* NewL(MLlcpConnOrientedTransporter* aTransporter, QtMobility::QLlcpSocketPrivate* aCallback = NULL);
    
   /*!
    * Destructor
    */
   ~CLlcpSocketType2();
   
public:    
   void ConnectToServiceL( const TDesC8& aServiceName);
   void DisconnectFromService();
   
   TInt StartWriteDatagramL(const TDesC8& aData);
   bool ReceiveData(TDesC8& aData);

   //for qt signals
   void Error(QtMobility::QLlcpSocket::Error socketError);
   void StateChanged(QtMobility::QLlcpSocket::State socketState);
   void ReadyRead();
   void BytesWritten(qint64 bytes);
private:
    // Constructor
    CLlcpSocketType2(MLlcpConnOrientedTransporter* aTransporter = NULL,QtMobility::QLlcpSocketPrivate* aCallback = NULL);
    // Second phase constructor
    void ConstructL();   
    void Cleanup();
 
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
   
   MLlcpConnOrientedTransporter* iTransporter;
   
   CLlcpConnecterAO* iConnecter;
   CLlcpSenderAO* iSender;
   CLlcpReceiverAO* iReceiver;
   
   QtMobility::QLlcpSocketPrivate*  iCallback; // not own 
   
   };

class CLlcpConnecterAO : public CActive
	{
public:
   /*!
	* Creates a new CLlcpConnectAO object.
	*/
   static CLlcpConnecterAO* NewL( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket );
   
	 
   /*!
	* Destructor.
	*/
   ~CLlcpConnecterAO();
   
public: 
   /*!
    * Disonnect with remote peer .
    */    
   void Disconnect();
   
   /*!
    * Connect to remote peer as given service uri.
    */
   void Connect(const TDesC8& aServiceName);
   
public: // From CActive

    void RunL(); 
    void DoCancel();
    
private:

    // Constructor
    CLlcpConnecterAO( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket );
    
    // Second phase constructor
    void ConstructL();
    
private:
    
    enum TConnectionState
        {
        ENotConnected,
        EConnecting,
        EConnected
        };
    /*!
          Pointer to MLlcpConnOrientedTransporter object.
     */ 
    MLlcpConnOrientedTransporter& iConnection;//Not Own
    
    CLlcpSocketType2& iSocket;
    
    /*!
          State of LLCP connection object.
     */
    TConnectionState iConnState;
    
	};
class CLlcpSenderAO : public CActive
	{
public:
   /*!
	* Creates a new CLlcpSenderAO object.
	*/
   static CLlcpSenderAO* NewL( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket );
   
	 
   /*!
	* Destructor.
	*/
   ~CLlcpSenderAO();
   
public:
   /*!
        Transfer given data to remote device.
    */
   TInt Send( const TDesC8& aData );
   
public: // From CActive

    void RunL(); 
    void DoCancel();
    
private:

    // Constructor
   CLlcpSenderAO( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket );
    
    // Second phase constructor
    void ConstructL();
    
private:
   
   /*!
         Pointer to MLlcpConnOrientedTransporter object.
    */ 
   MLlcpConnOrientedTransporter& iConnection; //Not Own
   
   CLlcpSocketType2& iSocket;
   /*!
    * Buffered data for sending data.
    */ 
   RBuf8 iSendBuf;
	};
class CLlcpReceiverAO : public CActive
	{
public:
   /*!
	* Creates a new CLlcpReceiverAO object.
	*/
   static CLlcpReceiverAO* NewL( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket );
   
	 
   /*!
	* Destructor.
	*/
   ~CLlcpReceiverAO();
   
public:
   /*!
        Receive data from remote device.
    */
   
   TInt StartReceiveDatagram(TInt64 aMaxSize);

public: // From CActive

    void RunL(); 
    void DoCancel();
   
private:

    // Constructor
   CLlcpReceiverAO( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket );
    
    // Second phase constructor
    void ConstructL();
private:
   
   /*!
    * Pointer to MLlcpConnOrientedTransporter object.
    */ 
   MLlcpConnOrientedTransporter& iConnection; //Not Own
   
   CLlcpSocketType2& iSocket;
   /*!
    * Buffered data for receiving data.
    */ 
   RBuf8 iReceiveBuf;	
	};

#endif /* LLCPSOCKETTYPE2_SYMBIAN_H_ */
