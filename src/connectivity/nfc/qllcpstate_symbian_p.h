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

#ifndef QLLCPSOCKETSTATE_P_H
#define QLLCPSOCKETSTATE_P_H

#include <qmobilityglobal.h>
#include "qllcpsocket.h"

#include <QtCore/QObject>

class CLlcpSocketType1;
class CLlcpSocketType2;

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QLlcpSocketPrivate;

/*
 *     enum LLCPSocketState {
        UnconnectedState,
        ConnectingState,
        ConnectedState,
        BoundState,
        ListeningState,
        ClosingState
 */
/*!
    \QLLCPSocketState
*/
class QLLCPSocketState
{
public:
    explicit QLLCPSocketState(QLlcpSocketPrivate*);

public:
    // all the following default implementation will emit errors.
    virtual qint64 ReadDatagram(char *data, qint64 maxSize,
                        QNearFieldTarget **target = 0, quint8 *port = 0);
    virtual qint64 WriteDatagram(const char *data, qint64 size,
                                 QNearFieldTarget *target, quint8 port);
    virtual qint64 WriteDatagram(const char *data, qint64 size);

    virtual bool Bind(quint8 port);
    virtual void ConnectToService(QNearFieldTarget *target, const QString &serviceUri);
    virtual void DisconnectFromService();
    virtual void ConnectToServiceComplete();
    virtual bool WaitForBytesWritten(int); 
    virtual bool WaitForReadyRead(int);
    virtual bool WaitForConnected(int);
    virtual bool WaitForDisconnected(int);

public:
    QLlcpSocketPrivate* m_socket;
};

/*!
    \QLLCPUnconnected
*/
class QLLCPUnconnected: public QLLCPSocketState
{
public:
    explicit QLLCPUnconnected(QLlcpSocketPrivate*);

public: // from base class
     bool Bind(quint8 port);
     void ConnectToService(QNearFieldTarget *target, const QString &serviceUri);
     void DisconnectFromService();
    // simple return, already unconnected
     void DisconnectFromHost();
     qint64 WriteDatagram(const char *data, qint64 size,
                          QNearFieldTarget *target, quint8 port);
     bool WaitForBytesWritten(int);     
};


/*!
    \QLLCPConnecting
*/
class QLLCPConnecting: public QLLCPSocketState
{
public:
    explicit QLLCPConnecting(QLlcpSocketPrivate*);

public:  
     QLLCPSocketState* Instance(QLlcpSocketPrivate* aSocket);
  
public: // from base class
    void ConnectToService(QNearFieldTarget *target, const QString &serviceUri);
    void DisconnectFromHost() {};
    void DisconnectFromService();
    void ConnectToServiceComplete();
    bool WaitForConnected(int);
};


/*!
    \QLLCPConnecting
*/
class QLLCPConnected: public QLLCPSocketState
{
public:
    explicit QLLCPConnected(QLlcpSocketPrivate*);

public: // from base class
    void ConnectToService(QNearFieldTarget *target, const QString &serviceUri);
    void DisconnectFromHost();
    void DisconnectFromService();
   qint64 WriteDatagram(const char *data, qint64 size);
   qint64 ReadDatagram(char *data, qint64 maxSize);
   bool WaitForBytesWritten(int msecs);
   bool WaitForReadyRead(int msecs);
   bool WaitForDisconnected(int);
private:
};


/*!
    \QLLCPClosing
*/
class QLLCPClosing: public QLLCPSocketState
{
public:
    explicit QLLCPClosing(QLlcpSocketPrivate*);
public: // from base class
    void ConnectToService(QNearFieldTarget *target, const QString &serviceUri);
    void DisconnectFromService();   
};

/*!
    \QLLCPBind
*/
class QLLCPBind: public QLLCPSocketState
    {  
public:
    explicit QLLCPBind(QLlcpSocketPrivate*);
public://from base class
    qint64 WriteDatagram(const char *data, qint64 size,QNearFieldTarget *target, quint8 port);
    qint64 ReadDatagram(char *data, qint64 maxSize,QNearFieldTarget **target = 0, quint8 *port = 0);
    bool WaitForBytesWritten(int msecs);
    bool WaitForReadyRead(int msecs);
    };


QTM_END_NAMESPACE
QT_END_HEADER

#endif //QLLCPSTATE_P_H
