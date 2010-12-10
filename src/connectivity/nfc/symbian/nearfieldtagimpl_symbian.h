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
#include "nearfieldtarget_symbian.h"
#include "nearfieldtagasyncrequest_symbian.h"
#include "nearfieldtargetoperation_symbian.h"

#include "nearfieldtagoperationcallback_symbian.h"
#include "nearfieldtagndefoperationcallback_symbian.h"


QTM_BEGIN_NAMESPACE
class QNearFieldTagType1Symbian;
class QNearFieldTagType2Symbian;
class QNearFieldTagType3Symbian;

template<typename TAGTYPE>
struct TagConstValue
{
    enum { MaxResponseSize = 4096, Timeout = 100 * 1000 };
};

template<>
struct TagConstValue<QNearFieldTagType1Symbian>
{
    enum { MaxResponseSize = 131, Timeout = 5 * 1000 };
};

template<>
struct TagConstValue<QNearFieldTagType2Symbian>
{
    enum { MaxResponseSize = 18, Timeout = 5 * 1000 };
};

template<>
struct TagConstValue<QNearFieldTagType3Symbian>
{
    enum { MaxResponseSize = 256, Timeout = 500 * 1000 };
};

template <typename TAGTYPE>
class QNearFieldTagImpl : public MNearFieldTargetOperation
{
public: // From MNearFieldTargetOperation
    void DoReadNdefMessages(MNearFieldNdefOperationCallback * const aCallback);
    void DoSetNdefMessages(const QList<QNdefMessage> &messages, MNearFieldNdefOperationCallback * const aCallback);
    bool DoHasNdefMessages();
    void DoSendCommand(const QByteArray& command, MNearFieldTagOperationCallback * const aCallback);

public:
    QNearFieldTagImpl(MNearFieldTarget *tag);
    virtual ~QNearFieldTagImpl();
    bool _hasNdefMessage();
    void _ndefMessages();
    void _setNdefMessages(const QList<QNdefMessage> &messages);

    void _setAccessMethods(const QNearFieldTarget::AccessMethods& accessMethods)
    {
        mAccessMethods = accessMethods;
    }

    QNearFieldTarget::AccessMethods _accessMethods() const
    {
        return mAccessMethods;
    }

    QNearFieldTarget::RequestId _sendCommand(const QByteArray &command);
    bool _waitForRequestCompleted(const QNearFieldTarget::RequestId &id, int msecs = 5000);

    QByteArray _uid() const;

    bool _isProcessingRequest() const;

protected:
    MNearFieldTarget * mTag;
    QNearFieldTarget::AccessMethods mAccessMethods;
    mutable QByteArray mUid;
    RPointerArray<CNdefMessage> mMessageList;
    RBuf8 mResponse;

protected:
    QList<MNearFieldTagAsyncRequest *> mPendingRequestList;
    MNearFieldTagAsyncRequest * mCurrentRequest;
};

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::DoReadNdefMessages(MNearFieldNdefOperationCallback * const aCallback)
{
    int error = KErrGeneral;
    CNearFieldNdefTarget * ndefTarget = mTag->CastToNdefTarget();
    if (ndefTarget)
    {
        ndefTarget->SetNdefOperationCallback(aCallback);        
        mMessageList.Reset();
        error = ndefTarget->ndefMessages(mMessageList);
    }

    if (KErrNone != error)
    {
        // This means the aysnc request doesn't issued. Directly invoke callback with the errore
        aCallback->ReadComplete(error);
    }
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::DoSetNdefMessages(const QList<QNdefMessage> &messages, MNearFieldNdefOperationCallback * const aCallback)
{
    int error = KErrGeneral;
    CNearFieldNdefTarget * ndefTarget = mTag->CastToNdefTarget();

    if (ndefTarget)
    {
        ndefTarget->SetNdefOperationCallback(aCallback);
        if (ndefTarget)
        {
            RPointerArray<CNdefMessage> result;
            TRAP( error, 
                for (int i = 0; i < messages.count(); ++i)
                {
                    result.Append(QNFCNdefUtility::FromQNdefMsgToCNdefMsgL(messages.at(i)));
                }
            )
            
            if (error == KErrNone)
            {
                ndefTarget->setNdefMessages(result);
            }
            
            result.Close();
        }
    }

    if (KErrNone != error)
    {
        aCallback->WriteComplete(error);
    }
}

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::DoHasNdefMessages()
{
    CNearFieldNdefTarget * ndefTarget = 0;
    if (MNearFieldTagAsyncRequest::ENdefRequest == mCurrentRequest->Type())
    {
        CNearFieldNdefTarget * ndefTarget = mTag->CastToNdefTarget();
        return ndefTarget ? ndefTarget->hasNdefMessage() : false;
    }
    else
    {
        // TODO:
        // create a new request for HasNdefMessages Operation, and INSERT it behind
        // ongoing request so that it can be run immediately when current request
        // finished. And Wait the request then return result.
    }
        
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::DoSendCommand(const QByteArray& command, MNearFieldTagOperationCallback * const aCallback)
{
    int error = KErrGeneral;

    if (command.count() > 0)
    {
        CNearFieldTag * tag = mTag->CastToTag();

        if (tag)
        {
            TPtrC8 cmd = QNFCNdefUtility::FromQByteArrayToTPtrC8(command);
            TRAP( error, 
                // Lazy creation
                if (mResponse.MaxLength() == 0)
                {
                    // the response is not created yet.
                    mResponse.CreateL(TagConstValue<TAGTYPE>::MaxResponseSize);
                }
                else
                {
                    mResponse.Zero();
                }
                
                User::LeaveIfError(tag->RawModeAccess(cmd, mResponse, TagConstValue<TAGTYPE>::Timeout));
            )
        }
    }
    if (error != KErrNone)
    {
        aCallback->CommandComplete(error);
    }
}

template<typename TAGTYPE>
QNearFieldTagImpl<TAGTYPE>::QNearFieldTagImpl(MNearFieldTarget *tag) : mTag(tag)
{
}

template<typename TAGTYPE>
QNearFieldTagImpl<TAGTYPE>::~QNearFieldTagImpl()
{
    delete mTag;
    mMessageList.Close();
    mResponse.Close();
}
    
template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::_hasNdefMessage()
{
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::_ndefMessages()
{
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::_setNdefMessages(const QList<QNdefMessage> &messages)
{
}

template<typename TAGTYPE>
QNearFieldTarget::RequestId QNearFieldTagImpl<TAGTYPE>::_sendCommand(const QByteArray &command)
{
}

template<typename TAGTYPE>
QByteArray QNearFieldTagImpl<TAGTYPE>::_uid() const
{
    if (mUid.isEmpty())
    {
        mUid = QNFCNdefUtility::FromTDesCToQByteArray(mTag->Uid());
    }
    return mUid;
}

QTM_END_NAMESPACE
#endif // QNEARFIELDTAGIMPL_H
