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

#include "nearfieldtagimpl_symbian.h"
#include "nearfieldtag_symbian.h"
#include "nearfieldndeftarget_symbian.h"
#include "qnearfieldutility_symbian.h"

QNearFieldTagImpl::QNearFieldTagImpl(MNearFieldTarget *tag) : mTag(tag)
{
}

bool QNearFieldTagImpl::_hasNdefMessage()
{
    bool result = false;

    CNearFieldNdefTarget * ndefTarget = mTag->CastToNdefTarget();
    if (ndefTarget)
    {
        result = ndefTarget->hasNdefMessage();
    }
    
    return result; 
}

QList<QNdefMessage> QNearFieldTagImpl::_ndefMessages()
{
    CNearFieldNdefTarget * ndefTarget = mTag->CastToNdefTarget();
    QList<QNdefMessage> result;
    
    if (ndefTarget)
    {
        RPointerArray<CNdefMessage> messageList;
        ndefTarget->ndefMessages(messageList);
        // Convert every CNdefMessage to QNdefMessage and append it to the QList
        TRAPD( err, 
            for (int i = 0; i < messageList.Count(); ++i)
            {
                result.append(QNFCNdefUtility::FromCNdefMsgToQndefMsgL(*messageList[i]));
            }
        )
        if (err != KErrNone)
        {
            result.clear();
        }
        messageList.Close();
    }
    
    return result;
}

void QNearFieldTagImpl::_setNdefMessages(const QList<QNdefMessage> &messages)
{
    CNearFieldNdefTarget * ndefTarget = mTag->CastToNdefTarget();
    
    if (ndefTarget)
    {
        RPointerArray<CNdefMessage> result;
        TRAPD( err, 
            for (int i = 0; i < messages.count(); ++i)
            {
                result.Append(QNFCNdefUtility::FromQNdefMsgToCNdefMsgL(messages.at(i)));
            }
        )
        
        if (err == KErrNone)
        {
        ndefTarget->setNdefMessages(result);
        }
        
        result.Close();
    }
}

QByteArray QNearFieldTagImpl::_sendCommand(const QByteArray &command, int timeout, int responseSize)
{
    CNearFieldTag * tag = mTag->CastToTag();
    QByteArray result;
    if (tag)
    {
        TPtrC8 cmd = QNFCNdefUtility::FromQByteArrayToTPtrC8(command);
        TRAPD( err, 
            RBuf8 response;
            response.CleanupClosePushL();
            response.CreateL(responseSize);
            User::LeaveIfError(tag->RawModeAccess(cmd, response, TTimeIntervalMicroSeconds32(timeout)));
            result = QNFCNdefUtility::FromTDesCToQByteArray(response);
            CleanupStack::PopAndDestroy();
        )
    }
    return result;
}
