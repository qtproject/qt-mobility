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
** ensure the GNU Lesser General Public License version 2.1 requirements ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
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


#ifndef QNFCNDEFUTILITY_SYMBIAN_H_
#define QNFCNDEFUTILITY_SYMBIAN_H_
#include <qndefmessage.h>
#include <e32base.h>

class CNdefMessage;

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

template <class T>
class CleanupQDelete
    {
public:
    inline static void PushL(T* aPtr);
private:
    static void Delete(TAny *aPtr);
    };
template <class T> inline void CleanupQDelete<T>::PushL(T* aRef)
    {
    CleanupStack::PushL(TCleanupItem(&Delete,aRef));
    }

template <class T> void CleanupQDelete<T>::Delete(TAny *aPtr)
    {
    delete (static_cast<T*>(aPtr));
    }

template <class T> inline void CleanupQDeletePushL(T* aRef)
    {
    CleanupQDelete<T>::PushL(aRef);
    }

QTM_BEGIN_NAMESPACE
class QNdefMessage;
class QNFCNdefUtility
{
public:

    /**
     * Maps between CNdefMessage and QNdefMessage
     *
     */
    static CNdefMessage* QNdefMsg2CNdefMsgL( const QNdefMessage& msg );
    static QNdefMessage CNdefMsg2QNdefMsgL( const CNdefMessage& msg );

    static TPtrC8 QByteArray2TPtrC8(const QByteArray& qbytearray);
    static void QByteArray2TDes8(const QByteArray& qbytearray, TDes8& buf);
    static QByteArray TDesC2QByteArray( const TDesC8& des);

    static HBuf8* QString2HBuf8(const QString& qstring);
    static QString TDesC82QString(const TDesC8&);

};

QTM_END_NAMESPACE
#endif /* QNFCNDEFUTILITY_SYMBIAN_H_ */
