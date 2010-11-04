

#ifndef LLCPSOCKETTYPE1_SYMBIAN_H_
#define LLCPSOCKETTYPE1_SYMBIAN_H_

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

class COwnLlcpConnLess;

#include <qmobilityglobal.h>
#include "../qllcpsocket_symbian_p.h"

class MLlcpReadWriteCb
    {  
public:

    /**
     * Empty Destructor.
     */
    virtual ~MLlcpReadWriteCb() {};
    
    /**
     * Called 
     */
    virtual void ReceiveComplete() = 0;
    virtual void WriteComplete( TInt) = 0;
        
    };      

/*!
 *  CLASS DECLARATION for CLlcpSocketType1 (ConnectLess Tran).
 */
class CLlcpSocketType1 : public CBase,
                         public MLlcpConnLessListener,
                         public MLlcpReadWriteCb
   {
public:
   /*!
    * Creates a new CLlcpSocketType1 object.
    */
   static CLlcpSocketType1* NewL(QtMobility::QLlcpSocketPrivate&);
   
   /*!
    * Creates a new CLlcpSocketType1 object.
    */
   static CLlcpSocketType1* NewLC(QtMobility::QLlcpSocketPrivate&);
   
   /*!
    * Destructor
    */
   ~CLlcpSocketType1();
   
public:    
   TInt StartWriteDatagram(const TDesC8& aData,TInt8 portNum);
   TInt StartReadDatagram(TInt64 aMaxSize);
   bool Bind(TInt8 portNum);
   
   TInt64 ReceiveData(TPtr8 &aTPtr);
   
   /*!
       Returns true if at least one datagram is waiting to be read;
       otherwise returns false.
   */
   bool HasPendingDatagrams() const;  
   TInt64 PendingDatagramSize() const;
    
private:  // from  MLlcpReadWriteCb
     void ReceiveComplete( );
     void WriteComplete( TInt );
     
private: // From MLlcpConnLessListener
    void FrameReceived( MLlcpConnLessTransporter* aConnection );
     
private:
    // Constructor
    CLlcpSocketType1(QtMobility::QLlcpSocketPrivate&);
    
    // Second phase constructor
    void ConstructL();  
    void Cleanup();   
    
    TInt CreateLocalConnection(TInt8 portNum);
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
   COwnLlcpConnLess* iRemoteConnection;   
   
   bool iConnLessStarted;
   
   QtMobility::QLlcpSocketPrivate& iCallback;
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
   static COwnLlcpConnLess* NewL( MLlcpConnLessTransporter*);
   
   /*!
    * Creates a new COwnLlcpConnection object.
    */
   static COwnLlcpConnLess* NewLC(MLlcpConnLessTransporter*);
   
   /*!
    * Destructor.
    */
   ~COwnLlcpConnLess();

public: 
   /*!
    * Transfer given data to remote device.
    */
   TInt Transfer(MLlcpReadWriteCb&, const TDesC8& aData );
   
   /*!
    * Cancels COwnLlcpConnection::Tranfer() request.
    */ 
   void TransferCancel();
   
   /*!
    * Starts receive data from ConnLess.
    */
   TInt Receive(MLlcpReadWriteCb&,TInt64 aMaxSize);
   
   /*!
    * Cancels COwnLlcpConnection::Receive() request.
    */ 
   void ReceiveCancel();
    
   void ReceiveDataFromBuf(TPtr8&);
   
   bool HasPendingDatagrams() const;
   
   TInt64 PendingDatagramSize() const;
   
public: // From CActive
    void RunL();
    void DoCancel();

private:

    // Constructor
    COwnLlcpConnLess( MLlcpConnLessTransporter*);
    
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
    
    MLlcpReadWriteCb* iLlcpReadWriteCb; // not own
    
    };

#endif /* LLCPSOCKETTYPE1_SYMBIAN_H_ */
