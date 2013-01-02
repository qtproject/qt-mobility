/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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
    static QByteArray TDesC2QByteArray( const TDesC8& des);

    static HBufC8* QString2HBufC8L(const QString& qstring);
    static QString TDesC82QStringL(const TDesC8&);

};

QTM_END_NAMESPACE
#endif /* QNFCNDEFUTILITY_SYMBIAN_H_ */
