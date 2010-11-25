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

#ifndef QNEARFIELDTAGIMPL_H
#define QNEARFIELDTAGIMPL_H

#include <qnearfieldtarget.h>
#include "nearfieldtag_symbian.h"
#include "nearfieldndeftarget_symbian.h"
#include "nearfieldutility_symbian.h"
#include "nearfieldtagoperationcallback_symbian.h"

QTM_USE_NAMESPACE
class MNearFieldTarget;

template <typename TAGTYPE>
class QNearFieldTagImpl : public MNearFieldTagOperationCallback
{
public:
    QNearFieldTagImpl(MNearFieldTarget *tag);
    bool _hasNdefMessage();
    QList<QNdefMessage> _ndefMessages();
    void _setNdefMessages(const QList<QNdefMessage> &messages);

    void _setAccessMethods(const QNearFieldTarget::AccessMethods& accessMethods)
    {
        mAccessMethods = accessMethods;
    }

    QNearFieldTarget::AccessMethods _accessMethods() const
    {
        return mAccessMethods;
    }

    QByteArray _sendCommand(const QByteArray &command, int timeout, int reponseSize);

    template<int N>
    QByteArray _sendCommand(const QByteArray &command, int timeout);

    QByteArray _uid() const;

private:
    TInt CommandComplete(){};
    TInt NdefOperationComplete(){};

protected:
    MNearFieldTarget * mTag;
    QNearFieldTarget::AccessMethods mAccessMethods;
    mutable QByteArray mUid;
};

template<typename TAGTYPE>
template<int N>
QByteArray QNearFieldTagImpl<TAGTYPE>::_sendCommand(const QByteArray &command, int timeout)
{
    CNearFieldTag * tag = mTag->CastToTag();
    QByteArray result;
    if (tag)
    {
        TPtrC8 cmd = QNFCNdefUtility::FromQByteArrayToTPtrC8(command);
        TBuf8<N> response;
        if (KErrNone == tag->RawModeAccess(cmd, response, TTimeIntervalMicroSeconds32(timeout)))
        {
            result = QNFCNdefUtility::FromTDesCToQByteArray(response);
        }
    }
    return result;
}

template<typename TAGTYPE>
QNearFieldTagImpl<TAGTYPE>::QNearFieldTagImpl(MNearFieldTarget *tag) : mTag(tag)
{
}

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::_hasNdefMessage()
{
    bool result = false;

    CNearFieldNdefTarget * ndefTarget = mTag->CastToNdefTarget();
    if (ndefTarget)
    {
        result = ndefTarget->hasNdefMessage();
    }
    
    return result; 
}

template<typename TAGTYPE>
QList<QNdefMessage> QNearFieldTagImpl<TAGTYPE>::_ndefMessages()
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

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::_setNdefMessages(const QList<QNdefMessage> &messages)
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

template<typename TAGTYPE>
QByteArray QNearFieldTagImpl<TAGTYPE>::_sendCommand(const QByteArray &command, int timeout, int responseSize)
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

template<typename TAGTYPE>
QByteArray QNearFieldTagImpl<TAGTYPE>::_uid() const
{
    if (mUid.isEmpty())
    {
        CNearFieldTag * tag = mTag->CastToTag();
        if (tag)
        {
            mUid = QNFCNdefUtility::FromTDesCToQByteArray(tag->Uid());
        }
    }
    return mUid;
}
#endif // QNEARFIELDTAGIMPL_H
