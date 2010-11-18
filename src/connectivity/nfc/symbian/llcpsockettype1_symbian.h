

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
class CLlcpSenderType1;
class CLlcpReceiverType1;
class CLlcpTimer;

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
    virtual void ReceiveComplete(TInt aError) = 0;
    virtual void WriteComplete( TInt aError, TInt aSize) = 0;
        
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
   TInt ReadDatagram(TPtr8 &aTPtr);
   bool Bind(TInt8 portNum);
   
   /*!
       Returns true if at least one datagram is waiting to be read;
       otherwise returns false.
   */
   bool HasPendingDatagrams() const;  
   TInt64 PendingDatagramSize() const;
   TBool WaitForBytesWritten(TInt aMilliSeconds);
 
private:
   enum TWaitStatus
       {
       ENone,
       EWaitForBytesWritten
       };
    
   CActiveSchedulerWait * iWait;
   TWaitStatus iWaitStatus;
   CLlcpTimer * iTimer;
   
private:
   TBool WaitForOperationReadyL(TWaitStatus aWaitStatus,TInt aMilliSeconds);  
   void StopWaitNow(TWaitStatus aWaitStatus);

private:  // from  MLlcpReadWriteCb
     void ReceiveComplete(TInt aError );
     void WriteComplete( TInt aError, TInt aSize);
     
private: // From MLlcpConnLessListener
    void FrameReceived( MLlcpConnLessTransporter* aConnection );
     
private:
    // Constructor
    CLlcpSocketType1(QtMobility::QLlcpSocketPrivate&);
    
    // Second phase constructor
    void ConstructL();  
    void Cleanup();   
    
    TInt CreateConnection(TInt8 portNum);
    TInt CreateConnection(MLlcpConnLessTransporter* aConnection);
    
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
   COwnLlcpConnLess* iConnection;
  
   
   bool iConnLessStarted;
   
   QtMobility::QLlcpSocketPrivate& iCallback;
   };

/*!
 *  CLASS DECLARATION for COwnLlcpConnLess.
 *
 */   


class COwnLlcpConnLess : public CBase
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

private:

    // Constructor
    COwnLlcpConnLess( MLlcpConnLessTransporter*);   
    // Second phase constructor
    void ConstructL();
  
private:
    MLlcpConnLessTransporter* iConnection;  
    CLlcpSenderType1*      iSenderAO;
    CLlcpReceiverType1*    iReceiverAO;  
    };

    
class CLlcpSenderType1 : public CActive
    {
public:
     static CLlcpSenderType1* NewL(MLlcpConnLessTransporter* aConnection);
    ~CLlcpSenderType1();

public:    
    /*!
     * Transfer given data to remote device.
     */
    void Transfer(MLlcpReadWriteCb&, const TDesC8& aData );
    
    /*!
     * Cancels COwnLlcpConnection::Tranfer() request.
     */ 
    void TransferCancel();   
 
public: // From CActive
    void RunL();
    void DoCancel();
 
private:
    CLlcpSenderType1(MLlcpConnLessTransporter* aConnection);
private: 
    /*!
         Buffered data for transmitting data.
     */ 
    RBuf8 iTransmitBuf;
    MLlcpReadWriteCb* iSendObserver; // not own
    MLlcpConnLessTransporter* iConnection; // not own
    };

class CLlcpReceiverType1 : public CActive
    {
public:
    static CLlcpReceiverType1* NewL(MLlcpConnLessTransporter* aConnection);  
    ~CLlcpReceiverType1();
        
public:   
    /*!
     * Starts receive data from ConnLess.
     */
    void Receive(MLlcpReadWriteCb&,TInt64 aMaxSize);
    
    /*!
     * Cancels COwnLlcpConnection::Receive() request.
     */ 
    void ReceiveCancel();
    void ReceiveDataFromBuf(TPtr8 &aTPtr);
    
    bool HasPendingDatagrams() const;
    TInt64 PendingDatagramSize() const;    
    
public: // From CActive
    void RunL();
    void DoCancel();      
    
private:
     CLlcpReceiverType1(MLlcpConnLessTransporter* aConnection);    
    
private:
    RBuf8 iReceiveBuf;
    MLlcpReadWriteCb* iReceiveObserver; // not own
    MLlcpConnLessTransporter* iConnection; // not own
    };
#endif /* LLCPSOCKETTYPE1_SYMBIAN_H_ */
