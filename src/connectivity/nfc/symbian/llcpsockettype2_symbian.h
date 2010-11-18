/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


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
class CLlcpTimer;

#include <qmobilityglobal.h>
#include "../qllcpsocket_symbian_p.h"
#include "../qllcpsocket.h"

/*!
 *  CLASS DECLARATION for CLlcpSocketType2 (ConnectOriented Transportation).
 */
class CLlcpSocketType2 : public CBase
{
public:
    static CLlcpSocketType2* NewL(QtMobility::QLlcpSocketPrivate* aCallback = NULL);

    static CLlcpSocketType2* NewL(MLlcpConnOrientedTransporter* aTransporter, QtMobility::QLlcpSocketPrivate* aCallback = NULL);
    ~CLlcpSocketType2();

public:
    void ConnectToServiceL( const TDesC8& aServiceName);
    void DisconnectFromService();

    TInt StartWriteDatagram(const TDesC8& aData);
    TBool ReceiveData(TDes8& aData);

    TInt64 BytesAvailable();

    //for qt signals
    void Error(QtMobility::QLlcpSocket::Error socketError);
    void StateChangedL(QtMobility::QLlcpSocket::State socketState);
    void ReadyRead();
    void BytesWritten(qint64 bytes);

    TBool WaitForReadyRead(TInt aMilliSeconds);
    TBool WaitForBytesWritten(TInt aMilliSeconds);
    TBool WaitForConnected(TInt aMilliSeconds);
    TBool WaitForDisconnected(TInt aMilliSeconds);

    RPointerArray<HBufC8>& GetAndLockBuffer();
    void UnlockBuffer();
private:
    // Constructor
    CLlcpSocketType2(MLlcpConnOrientedTransporter* aTransporter = NULL,QtMobility::QLlcpSocketPrivate* aCallback = NULL);
    // Second phase constructor
    void ConstructL();
    void Cleanup();
    enum TWaitStatus
        {
        ENone,
        EWaitForReadyRead,
        EWaitForBytesWritten,
        EWaitForConnected,
        EWaitForDisconnected
        };
    TBool WaitForOperationReadyL(TWaitStatus aWaitStatus,TInt aSeconds);
    void StopWaitNow(TWaitStatus aWaitStatus);
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

    RPointerArray<HBufC8> iReceiveBufArray;
    TInt iBufferOffset;

    CActiveSchedulerWait * iWait;
    TWaitStatus iWaitStatus;
    CLlcpTimer * iTimer;

    QtMobility::QLlcpSocketPrivate*  iCallback; // not own

};

class CLlcpTimer : public CTimer
{
public:
    static CLlcpTimer* NewL(CActiveSchedulerWait & aWait);
    virtual ~CLlcpTimer();
    void Start(TInt aMSecs);

private: // From CTimer
    void RunL();

private:

    CLlcpTimer(CActiveSchedulerWait & aWait);
    void ConstructL();

private:

    CActiveSchedulerWait& iWait; //not own
};

class CLlcpConnecterAO : public CActive
{
public:
    static CLlcpConnecterAO* NewL( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket );
    ~CLlcpConnecterAO();

public:
    /*!
    * Disonnect with remote peer .
    */
    void DisconnectL();

    /*!
    * Connect to remote peer as given service uri.
    */
    void ConnectL(const TDesC8& aServiceName);

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
   static CLlcpSenderAO* NewL( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket );
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
    static CLlcpReceiverAO* NewL( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket );
    ~CLlcpReceiverAO();

public:
    /*!
    Receive data from remote device.
    */
    TInt StartReceiveDatagram();

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
