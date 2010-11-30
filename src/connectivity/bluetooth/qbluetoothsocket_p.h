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

#ifndef QBLUETOOTHSOCKET_P_H
#define QBLUETOOTHSOCKET_P_H

#include "qbluetoothsocket.h"

#include <QtGlobal>

#ifndef QBLUETOOTHDEVICE_BUFFERSIZE
#define QBLUETOOTHDEVICE_BUFFERSIZE Q_INT64_C(16384)
#endif

QT_FORWARD_DECLARE_CLASS(QSocketNotifier)

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

// This is QIODevice's read buffer, optimised for read(), isEmpty() and getChar()
class QBluetoothPrivateLinearBuffer
{
public:
    QBluetoothPrivateLinearBuffer() : len(0), first(0), buf(0), capacity(0) {
    }
    ~QBluetoothPrivateLinearBuffer() {
        delete [] buf;
    }
    void clear() {
        first = buf;
        len = 0;
    }
    int size() const {
        return len;
    }
    bool isEmpty() const {
        return len == 0;
    }
    void skip(int n) {
        if (n >= len) {
            clear();
        } else {
            len -= n;
            first += n;
        }
    }
    int getChar() {
        if (len == 0)
            return -1;
        int ch = uchar(*first);
        len--;
        first++;
        return ch;
    }
    int read(char* target, int size) {
        int r = qMin(size, len);
        memcpy(target, first, r);
        len -= r;
        first += r;
        return r;
    }
    char* reserve(int size) {
        makeSpace(size + len, freeSpaceAtEnd);
        char* writePtr = first + len;
        len += size;
        return writePtr;
    }
    void chop(int size) {
        if (size >= len) {
            clear();
        } else {
            len -= size;
        }
    }
    QByteArray readAll() {
        char* f = first;
        int l = len;
        clear();
        return QByteArray(f, l);
    }
    int readLine(char* target, int size) {
        int r = qMin(size, len);
        char* eol = static_cast<char*>(memchr(first, '\n', r));
        if (eol)
            r = 1+(eol-first);
        memcpy(target, first, r);
        len -= r;
        first += r;
        return int(r);
        }
    bool canReadLine() const {
        return memchr(first, '\n', len);
    }
    void ungetChar(char c) {
        if (first == buf) {
            // underflow, the existing valid data needs to move to the end of the (potentially bigger) buffer
            makeSpace(len+1, freeSpaceAtStart);
        }
        first--;
        len++;
        *first = c;
    }
    void ungetBlock(const char* block, int size) {
        if ((first - buf) < size) {
            // underflow, the existing valid data needs to move to the end of the (potentially bigger) buffer
            makeSpace(len + size, freeSpaceAtStart);
        }
        first -= size;
        len += size;
        memcpy(first, block, size);
    }

private:
    enum FreeSpacePos {freeSpaceAtStart, freeSpaceAtEnd};
    void makeSpace(size_t required, FreeSpacePos where) {
        size_t newCapacity = qMax(capacity, size_t(QBLUETOOTHDEVICE_BUFFERSIZE));
        while (newCapacity < required)
            newCapacity *= 2;
        int moveOffset = (where == freeSpaceAtEnd) ? 0 : newCapacity - len;
        if (newCapacity > capacity) {
            // allocate more space
            char* newBuf = new char[newCapacity];
            memmove(newBuf + moveOffset, first, len);
            delete [] buf;
            buf = newBuf;
            capacity = newCapacity;
        } else {
            // shift any existing data to make space
            memmove(buf + moveOffset, first, len);
        }
        first = buf + moveOffset;
    }

private:
    // length of the unread data
    int len;
    // start of the unread data
    char* first;
    // the allocated buffer
    char* buf;
    // allocated buffer size
    size_t capacity;
};



class QBluetoothSocket;

class QBluetoothSocketPrivate : public QObject
{
    Q_OBJECT

public:

    QBluetoothSocketPrivate(QBluetoothSocket *parent);
    virtual ~QBluetoothSocketPrivate();

    virtual void connectToService(const QBluetoothAddress &address, quint16 port, QIODevice::OpenMode openMode);

    virtual bool ensureNativeSocket(QBluetoothSocket::SocketType type);

    static QBluetoothSocketPrivate *constructPrivate(QBluetoothSocket *parent);
    
public:
    QBluetoothPrivateLinearBuffer buffer;
    int socket;
    QBluetoothSocket::SocketType socketType;
    QBluetoothSocket::SocketState state;
    QBluetoothSocket::SocketError socketError;
    QString localName;
    QString peerName;
    QSocketNotifier *readNotifier;
    QSocketNotifier *connectWriteNotifier;


//    QByteArray rxBuffer;
//    qint64 rxOffset;
    QByteArray txBuffer;

    QBluetoothSocket *q;

    QString errorString;

Q_SIGNALS:
    void readyRead();
    void connected();
    void disconnected();
    void error(QBluetoothSocket::SocketError error);
    void stateChanged(QBluetoothSocket::SocketState state);

public Q_SLOTS:
    void _q_readNotify();

};


QTM_END_NAMESPACE

QT_END_HEADER

#endif
