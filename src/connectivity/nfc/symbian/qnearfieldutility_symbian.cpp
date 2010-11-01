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
#include <ndefmessage.h>
#include <QtCore/QList>
#include <e32base.h>
#include "../qndefmessage.h"
#include "qnearfieldutility_symbian.h"

QTM_BEGIN_NAMESPACE

QNFCNdefUtility::QNFCNdefUtility()
    {
    }

QNFCNdefUtility::~QNFCNdefUtility()
    {
    }

CNdefMessage* QNFCNdefUtility::FromQNdefMsgToCNdefMsgL( const QNdefMessage& msg )
    {
        QByteArray payload = msg.toByteArray();
        CNdefMessage* cmsg = CNdefMessage::NewL();
        TPtrC8 rawData(reinterpret_cast<const TUint8*>(payload.constData()), payload.size());
        cmsg->ImportRawDataL(rawData, 0);
        return cmsg;
    }


QNdefMessage QNFCNdefUtility::FromCNdefMsgToQndefMsgL( const CNdefMessage& msg )
    {
        QNdefMessage result;
        HBufC8* newBuf = HBufC8::NewL(msg.SizeL());
        RBuf8 buf;
        buf.Assign(newBuf);
        buf.CleanupClosePushL();
        msg.ExportRawDataL(buf,0);
        QByteArray qtArray;
        qtArray.fromRawData(reinterpret_cast<const char*>(newBuf->Ptr()),newBuf->Size());
        result.fromByteArray(qtArray);
        CleanupStack::Pop(&buf);
        return result;
    }

TPtrC8 QNFCNdefUtility::FromQByteArrayToTPtrC8( QByteArray& qbytearray)
    {
    TPtrC8 ptr(reinterpret_cast<const TUint8*>(qbytearray.constData()), qbytearray.size());
    return ptr;
    }

QTM_END_NAMESPACE
