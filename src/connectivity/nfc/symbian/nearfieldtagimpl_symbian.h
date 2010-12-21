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
#include <qnearfieldtarget_p.h>
#include "nearfieldtag_symbian.h"
#include "nearfieldndeftarget_symbian.h"
#include "nearfieldutility_symbian.h"
#include "nearfieldtarget_symbian.h"
#include "nearfieldtagasyncrequest_symbian.h"
#include "nearfieldtargetoperation_symbian.h"

#include "nearfieldtagoperationcallback_symbian.h"
#include "nearfieldtagndefoperationcallback_symbian.h"

#include "nearfieldtagndefrequest_symbian.h"
#include "nearfieldtagcommandrequest_symbian.h"
#include "nearfieldtagcommandsrequest_symbian.h"
#include "debug.h"

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
    bool IssueNextRequest(QNearFieldTarget::RequestId aId);
    void RemoveRequestFromQueue(QNearFieldTarget::RequestId aId);
    QNearFieldTarget::RequestId AllocateRequestId();
    bool HandleResponse(const QNearFieldTarget::RequestId &id, const QByteArray &command, const QByteArray &response);

    void EmitNdefMessageRead(const QNdefMessage &message);
    void EmitNdefMessagesWritten();
    void EmitRequestCompleted(const QNearFieldTarget::RequestId &id);
    void EmitError(int error, const QNearFieldTarget::RequestId &id);
    
    void DoCancelSendCommand();
    void DoCancelNdefAccess();

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
    QNearFieldTarget::RequestId _sendCommands(const QList<QByteArray> &command);
    bool _waitForRequestCompleted(const QNearFieldTarget::RequestId &id, int msecs = 5000);

    QByteArray _uid() const;

    bool _isProcessingRequest() const;

protected:
    QNearFieldTarget::Error SymbianError2QtError(int error);
protected:
    MNearFieldTarget * mTag;
    QNearFieldTarget::AccessMethods mAccessMethods;
    mutable QByteArray mUid;
    RPointerArray<CNdefMessage> mMessageList;
    RBuf8 mResponse;

protected:
    // own each async request in the list
    QList<MNearFieldTagAsyncRequest *> mPendingRequestList;
    MNearFieldTagAsyncRequest * mCurrentRequest;
};

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::DoReadNdefMessages(MNearFieldNdefOperationCallback * const aCallback)
{
    BEGIN
    int error = KErrGeneral;
    CNearFieldNdefTarget * ndefTarget = mTag->CastToNdefTarget();
    if (ndefTarget)
    {
        LOG("switched to ndef connection");
        ndefTarget->SetNdefOperationCallback(aCallback);        
        // TODO: consider re-entry, since signal is emit one by one
        mMessageList.Reset();
        error = ndefTarget->ndefMessages(mMessageList);
        LOG("error code is"<<error);
    }

    if (KErrNone != error)
    {
        // This means the aysnc request doesn't issued. Directly invoke callback with the errore
        aCallback->ReadComplete(error, 0);
    }
    END
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::DoSetNdefMessages(const QList<QNdefMessage> &messages, MNearFieldNdefOperationCallback * const aCallback)
{
    BEGIN
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
    END
}

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::DoHasNdefMessages()
{
    BEGIN
    if (
       (mCurrentRequest) && (MNearFieldTagAsyncRequest::ENdefRequest == mCurrentRequest->Type()) ||
       (!mCurrentRequest) 
       )

    {
        LOG("create ndefTarget connection");
        CNearFieldNdefTarget * ndefTarget = mTag->CastToNdefTarget();
        END
        return ndefTarget ? ndefTarget->hasNdefMessage() : false;
    }
    else
    {
        LOG("currentRequest is ongoing, use async request to check ndef message");
        NearFieldTagNdefRequest * readNdefRequest = new NearFieldTagNdefRequest;

        QNearFieldTarget::RequestId requestId;

        if (readNdefRequest)
        {
            readNdefRequest->SetRequestId(requestId);
            readNdefRequest->SetNdefRequestType(NearFieldTagNdefRequest::EReadRequest);
            readNdefRequest->SetOperator(this);
            int index = mPendingRequestList.indexOf(mCurrentRequest);
            if ((index < 0) || (index == mPendingRequestList.count() - 1))
            {
                // no next request
                mPendingRequestList.append(readNdefRequest);
            }
            else
            {
                mPendingRequestList.insert(index+1, readNdefRequest);
            }
            readNdefRequest->WaitRequestCompleted(5000);
            if (mMessageList.Count() == 0)
            {
                END
                return false;
            }
            else
            {
                mMessageList.Reset();
                END
                return true;
            }
        }
        else
        {
            // TODO: is that proper way?
            LOG("unexpect error to create async request");
            END
            return false;
        }
    }
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::DoSendCommand(const QByteArray& command, MNearFieldTagOperationCallback * const aCallback)
{
    BEGIN
    int error = KErrGeneral;

    if (command.count() > 0)
    {
        CNearFieldTag * tag = mTag->CastToTag();

        if (tag)
        {
            tag->SetTagOperationCallback(aCallback);
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
    END
}

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::IssueNextRequest(QNearFieldTarget::RequestId aId)
{
    BEGIN
    // find the request after current request
    int index = mPendingRequestList.indexOf(mCurrentRequest);
    LOG("index = "<<index);
    if ((index < 0) || (index == mPendingRequestList.count() - 1))
    {
        // no next request
        mCurrentRequest = 0;
        END
        return false;
    }
    else
    {
        if (aId == mCurrentRequest->GetRequestId())
        {
            mCurrentRequest = mPendingRequestList.at(index + 1);
            mCurrentRequest->IssueRequest();
        }
        else
        {
            LOG("re-entry happened");
        }

        END
        return true;
    }
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::RemoveRequestFromQueue(QNearFieldTarget::RequestId aId)
{
    BEGIN
    for(int i = 0; i < mPendingRequestList.count(); ++i)
    {
        MNearFieldTagAsyncRequest * request = mPendingRequestList.at(i);
        if (request->GetRequestId() == aId)
        {
            LOG("remove request id");
            mPendingRequestList.removeAt(i);
            break;
        }
    }
    END
} 

template<typename TAGTYPE>
QNearFieldTarget::RequestId QNearFieldTagImpl<TAGTYPE>::AllocateRequestId()
{
    BEGIN
    QNearFieldTarget::RequestIdPrivate * p = new QNearFieldTarget::RequestIdPrivate;
    QNearFieldTarget::RequestId id(p);
    END
    return id;
} 

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::HandleResponse(const QNearFieldTarget::RequestId &id, const QByteArray &command, const QByteArray &response)
{
    BEGIN
    TAGTYPE * tag = static_cast<TAGTYPE *>(this);
    END
    return tag->handleTagOperationResponse(id, command, response);
}


template<typename TAGTYPE>
QNearFieldTagImpl<TAGTYPE>::QNearFieldTagImpl(MNearFieldTarget *tag) : mTag(tag)
{
    mCurrentRequest = 0;
}

template<typename TAGTYPE>
QNearFieldTagImpl<TAGTYPE>::~QNearFieldTagImpl()
{
    BEGIN
    for (int i = 0; i < mPendingRequestList.count(); ++i)
    {
        delete mPendingRequestList[i];
    }
    mPendingRequestList.clear();
    mCurrentRequest = 0;
    delete mTag;
    mMessageList.Close();
    mResponse.Close();
    END
}
    
template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::_hasNdefMessage()
{
    return DoHasNdefMessages();
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::_ndefMessages()
{
    BEGIN
    NearFieldTagNdefRequest * readNdefRequest = new NearFieldTagNdefRequest;
    QNearFieldTarget::RequestId requestId = AllocateRequestId();

    if (readNdefRequest)
    {
        LOG("read ndef request created");
        readNdefRequest->SetRequestId(requestId);
        readNdefRequest->SetNdefRequestType(NearFieldTagNdefRequest::EReadRequest);
        readNdefRequest->SetOperator(this);

        if (!_isProcessingRequest())
        {
            LOG("the request will be issued at once");
            // issue the request
            mCurrentRequest = readNdefRequest;
            readNdefRequest->IssueRequest();
        }
        mPendingRequestList.append(readNdefRequest);
    }
    END
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::_setNdefMessages(const QList<QNdefMessage> &messages)
{
    BEGIN
    NearFieldTagNdefRequest * writeNdefRequest = new NearFieldTagNdefRequest;
    QNearFieldTarget::RequestId requestId = AllocateRequestId();

    if (writeNdefRequest)
    {
        LOG("write ndef request created");
        writeNdefRequest->SetRequestId(requestId);
        writeNdefRequest->SetNdefRequestType(NearFieldTagNdefRequest::EWriteRequest);
        writeNdefRequest->SetInputNdefMessages(messages);
        writeNdefRequest->SetOperator(this);

        if (!_isProcessingRequest())
        {
            // issue the request
            LOG("the request will be issued at once");
            mCurrentRequest = writeNdefRequest;
            writeNdefRequest->IssueRequest();
        }
        mPendingRequestList.append(writeNdefRequest);
    }
    // TODO: consider else
    END
}

template<typename TAGTYPE>
QNearFieldTarget::RequestId QNearFieldTagImpl<TAGTYPE>::_sendCommand(const QByteArray &command)
{
    BEGIN
    NearFieldTagCommandRequest * rawCommandRequest = new NearFieldTagCommandRequest;
    QNearFieldTarget::RequestId requestId = AllocateRequestId();

    if (rawCommandRequest)
    {
        LOG("send command request created");
        rawCommandRequest->SetInputCommand(command);
        rawCommandRequest->SetRequestId(requestId);
        rawCommandRequest->SetResponseBuffer(mResponse);
        rawCommandRequest->SetOperator(this);

        if (!_isProcessingRequest())
        {
            // issue the request
            LOG("the request will be issued at once");
            mCurrentRequest = rawCommandRequest;
            rawCommandRequest->IssueRequest();
        }
        mPendingRequestList.append(rawCommandRequest);
    }
    // TODO: consider else
    END
    return requestId;
}

template<typename TAGTYPE>
QNearFieldTarget::RequestId QNearFieldTagImpl<TAGTYPE>::_sendCommands(const QList<QByteArray> &commands)
{
    BEGIN
    NearFieldTagCommandsRequest * rawCommandsRequest = new NearFieldTagCommandsRequest;
    QNearFieldTarget::RequestId requestId = AllocateRequestId();

    if (rawCommandsRequest)
    {
        LOG("send commands request created");
        rawCommandsRequest->SetInputCommands(commands);
        rawCommandsRequest->SetRequestId(requestId);
        rawCommandsRequest->SetOperator(this);
        rawCommandsRequest->SetResponseBuffer(&mResponse);

        if (!_isProcessingRequest())
        {
            // issue the request
            LOG("the request will be issued at once");
            mCurrentRequest = rawCommandsRequest;
            rawCommandsRequest->IssueRequest();
        }
        mPendingRequestList.append(rawCommandsRequest);
    }
    // TODO: consider else
    END
    return requestId;
}

template<typename TAGTYPE>
QByteArray QNearFieldTagImpl<TAGTYPE>::_uid() const
{
    BEGIN
    if (mUid.isEmpty())
    {
        mUid = QNFCNdefUtility::FromTDesCToQByteArray(mTag->Uid());
        LOG(mUid);
    }
    END
    return mUid;
}

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::_isProcessingRequest() const
{
    BEGIN
    bool result = mPendingRequestList.count() > 0;
    LOG(result);
    END
    return result;

}

template<typename TAGTYPE>
bool QNearFieldTagImpl<TAGTYPE>::_waitForRequestCompleted(const QNearFieldTarget::RequestId &id, int msecs)
{
    BEGIN
    int index = -1;
    for (int i = 0; i < mPendingRequestList.count(); ++i)
    {
        if (id == mPendingRequestList.at(i)->GetRequestId())
        {
            index = i;
            break;
        }
    }

    if (index < 0)
    {
        // request ID is not in pending list. So either it may not be issued, or has already completed
        END
        return false; 
    }

    MNearFieldTagAsyncRequest * request = mPendingRequestList.at(index);
    LOG("get the request from pending request list");
    END
    return request->WaitRequestCompleted(msecs);
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::EmitNdefMessageRead(const QNdefMessage &message)
{
    BEGIN
    TAGTYPE * tag = static_cast<TAGTYPE *>(this);
    emit tag->ndefMessageRead(message);
    END
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::EmitNdefMessagesWritten()
{
    BEGIN
    TAGTYPE * tag = static_cast<TAGTYPE *>(this);
    emit tag->ndefMessagesWritten();
    END
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::EmitRequestCompleted(const QNearFieldTarget::RequestId &id)
{
    BEGIN
    TAGTYPE * tag = static_cast<TAGTYPE *>(this);
    emit tag->requestCompleted(id);
    END
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::EmitError(int error, const QNearFieldTarget::RequestId &id)
{
    BEGIN
    TAGTYPE * tag = static_cast<TAGTYPE *>(this);
    emit tag->error(SymbianError2QtError(error), id);
    END
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::DoCancelSendCommand()
{
    BEGIN
    CNearFieldTag * tag = mTag->CastToTag();
    if (tag)
    {
        LOG("Cancel raw command operation");
        tag->SetTagOperationCallback(0);
        tag->Cancel();
    }
    END
}

template<typename TAGTYPE>
void QNearFieldTagImpl<TAGTYPE>::DoCancelNdefAccess()
{
    BEGIN
    CNearFieldNdefTarget * ndefTarget = mTag->CastToNdefTarget();
    if(ndefTarget)
    {
        LOG("Cancel ndef operation");
        ndefTarget->SetNdefOperationCallback(0);
        ndefTarget->Cancel();
    }
    END
}

template<typename TAGTYPE>
QNearFieldTarget::Error QNearFieldTagImpl<TAGTYPE>::SymbianError2QtError(int error)
{
    BEGIN
    // TODO: refactor. 
    QNearFieldTarget::Error qtError;
    switch(error)
    {    
        case KErrNone:
        {
            qtError = QNearFieldTarget::NoError;
            break;
        }
        case KErrNotSupported:
        {
            qtError = QNearFieldTarget::UnsupportedError;
            break;
        }
        case KErrTimedOut:
        {
            qtError = QNearFieldTarget::NoResponseError;
            break;
        }
        case KErrAccessDenied:
        case KErrEof:
        case KErrServerTerminated:
        {
            if (mCurrentRequest)
            {
                if(mCurrentRequest->Type() == MNearFieldTagAsyncRequest::ENdefRequest)
                {
                    NearFieldTagNdefRequest * req = static_cast<NearFieldTagNdefRequest *>(mCurrentRequest);
                    if (req->GetNdefRequestType() == NearFieldTagNdefRequest::EReadRequest)
                    {
                        qtError = QNearFieldTarget::NdefReadError;
                        break;
                    }
                    else
                    {
                        qtError = QNearFieldTarget::InvalidParametersError;
                        break;
                    }
                }
                else
                {
                    qtError = QNearFieldTarget::InvalidParametersError;
                    break;
                }
            }
            else
            {
                qtError = QNearFieldTarget::InvalidParametersError;
                break;
            }
        }
        case KErrTooBig:
        {
            qtError = QNearFieldTarget::TargetOutOfRangeError;
            break;
        } 
        default:
        {
            qtError = QNearFieldTarget::InvalidParametersError;
            break;
        }
    }
    LOG(qtError);
    END
    return qtError;
}
QTM_END_NAMESPACE
#endif // QNEARFIELDTAGIMPL_H
