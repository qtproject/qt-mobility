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

#ifndef QLLCPSOCKET_P_H
#define QLLCPSOCKET_P_H

#include <qmobilityglobal.h>
#include "qllcpsocket.h"

#include <QtCore/QObject>

class CLlcpSocketType1;
class CLlcpSocketType2;

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QLLCPState;

class QLlcpSocketPrivate : public QObject
{
    Q_OBJECT
    
public:
    QLlcpSocketPrivate();
    ~QLlcpSocketPrivate();

    void connectToService(QNearFieldTarget *target, const QString &serviceUri);
    void connectToHost(QNearFieldTarget *target, const quint8 portNum);
    void disconnectFromService();
    void disconnectFromHost();

    bool bind(quint8 port);

    bool hasPendingDatagrams() const;
    qint64 pendingDatagramSize() const;

    qint64 writeDatagram(const char *data, qint64 size);
    qint64 writeDatagram(const QByteArray &datagram);

    qint64 readDatagram(char *data, qint64 maxSize,
                        QNearFieldTarget **target = 0, quint8 *port = 0);
    qint64 writeDatagram(const char *data, qint64 size,
                         QNearFieldTarget *target, quint8 port);
    qint64 writeDatagram(const QByteArray &datagram, QNearFieldTarget *target, quint8 port);

    QLlcpSocket::Error error() const;
    QLlcpSocket::State state() const;

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

    bool waitForReadyRead(int msecs);
    bool waitForBytesWritten(int msecs);
    bool waitForConnected(int msecs);
    bool waitForDisconnected(int msecs);
private:
    void socketType1Check() const;
    void socketType2Check() const;

    
public: 
    CLlcpSocketType2* socketType2Handler() {return m_symbianSocketType2;}
    
private:
    enum SocketType 
    {
       connectionType1 = 1, // ConnectionLess mode
       connectionType2 = 2, // ConnectionOriented mode
       connectionUnknown = -1
    };
    
    CLlcpSocketType1* m_symbianSocketType1;
    CLlcpSocketType2* m_symbianSocketType2;
    SocketType m_socketType;
   
    int m_port;

public:
     void invokeBytesWritten(qint64 bytes);
     void invokeReadyRead();
     void invokeError();
     void invokeStateChange(QLlcpSocket::State socketState);
     
signals:
     void readyRead();
     void bytesWritten(qint64 bytes);
     void error(QLlcpSocket::Error socketError) const;
     void stateChanged(QLlcpSocket::State socketState);
     /*  From QLlcpSocket
     signals:
         void connected();
         void disconnected();
         void error(QLlcpSocket::Error socketError);
         void stateChanged(QLlcpSocket::State socketState);
     */
 
// state machine part
    
public:
     void ChangeState(QLLCPState* state);
private:
     QLLCPState* m_state;
};



//////////

/*
 *     enum SocketState {
        UnconnectedState,
        HostLookupState,
        ConnectingState,
        ConnectedState,
        BoundState,
        ListeningState,
        ClosingState
 * 
 */
/*!
    \QLLCPState
*/
class QLLCPState
{
public:
    QLLCPState(QLlcpSocketPrivate*);
public:
    // all the default implementation will emit errors. 
    virtual void Transmit(const char *data, qint64 size,QNearFieldTarget *target, quint8 port) {}
    virtual qint64 readDatagram(char *data, qint64 maxSize,
                        QNearFieldTarget **target = 0, quint8 *port = 0) { return -1;}  
    
    virtual void Bind(quint8 port) {};
    virtual void DisconnectFromHost() {};
    
    virtual void ChangeState(QLlcpSocketPrivate*t,QLLCPState *s );
    
protected:
    QLlcpSocketPrivate *m_socket;
};


/*!
    \QLLCPType1Unconnected
*/
class QLLCPType1Unconnected: public QLLCPState
{
public:
    ~QLLCPType1Unconnected() {delete m_instance;}

public:  
    static QLLCPState* Instance(QLlcpSocketPrivate* aSocket);
    virtual void Bind(quint8 port);
    // simple return, already unconnected
    virtual void DisconnectFromHost();
   
private:
    QLLCPType1Unconnected(QLlcpSocketPrivate*);
    
private:
    static QLLCPState* m_instance;
};

/*!
    \QLLCPType1LookUp
*/
class QLLCPType1LookUp: public QLLCPState
{
public:
    ~QLLCPType1LookUp() {delete m_instance;}

public:  
    static QLLCPState* Instance(QLlcpSocketPrivate* aSocket);

public: // from base class
    // pending close? do sth
    virtual void DisconnectFromHost() {};
private:
    QLLCPType1LookUp(QLlcpSocketPrivate*);
    
private:
    static QLLCPState* m_instance;
};

/*!
    \QLLCPType1Connecting
*/
class QLLCPType1Connecting: public QLLCPState
{
public:
    ~QLLCPType1Connecting() {delete m_instance;}

public:  
    static QLLCPState* Instance(QLlcpSocketPrivate* aSocket);
  
public: // from base class
    // pending close? do sth
    virtual void DisconnectFromHost() {};
    
private:
    QLLCPType1Connecting(QLlcpSocketPrivate*);
    
private:
    static QLLCPState* m_instance;
};


/*!
    \QLLCPType1Connecting
*/
class QLLCPType1Connected: public QLLCPState
{
public:
    ~QLLCPType1Connected() {delete m_instance;}

public:  
    static QLLCPState* Instance(QLlcpSocketPrivate* aSocket);

public: // from base class
    virtual void DisconnectFromHost();
    
private:
    QLLCPType1Connected(QLlcpSocketPrivate*);
    
private:
    static QLLCPState* m_instance;
};


/*!
    \QLLCPType1Closing
*/
class QLLCPType1Closing: public QLLCPState
{
public:
    ~QLLCPType1Closing() {delete m_instance;}

public:  
    static QLLCPState* Instance(QLlcpSocketPrivate* aSocket);

public: // from base class
    virtual void DisconnectFromHost();
    
private:
    QLLCPType1Closing(QLlcpSocketPrivate*);
    
private:
    static QLLCPState* m_instance;
};

/*!
    \QLLCPType1Bind
*/
class QLLCPType1Bind: public QLLCPState
    {  
public:  
    static QLLCPState* Instance(QLlcpSocketPrivate* aSocket);   
    ~QLLCPType1Bind() {delete m_instance;}

public: //from base class
    virtual void Transmit(const char *data, qint64 size,QNearFieldTarget *target, quint8 port);
    qint64 readDatagram(char *data, qint64 maxSize,
                        QNearFieldTarget **target = 0, quint8 *port = 0);    
    
private:
    QLLCPType1Bind(QLlcpSocketPrivate* aSocket);
    static QLLCPState* m_instance;
    };




QTM_END_NAMESPACE
QT_END_HEADER

#endif // QLLCPSOCKET_P_H
