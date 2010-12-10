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
#include <nfctype1connection.h>
#include <nfctype2connection.h>
#include <nfctype3connection.h>
#include <iso14443connection.h>
#include <ndefconnection.h>
#include <nfctag.h>
#include <qglobal.h>
#include "nearfieldtag_symbian.h"
#include "nearfieldndeftarget_symbian.h"
#include "nearfieldtagndefoperationcallback_symbian.h"

CNearFieldNdefTarget::CNearFieldNdefTarget(MNfcTag * aNfcTag, RNfcServer& aNfcServer) : iNfcTag(aNfcTag),
                                                                                        iNfcServer(aNfcServer)
    {
    }

CNearFieldNdefTarget* CNearFieldNdefTarget::NewLC(MNfcTag * aNfcTag, RNfcServer& aNfcServer)
    {
    CNearFieldNdefTarget* self = new (ELeave) CNearFieldNdefTarget(aNfcTag, aNfcServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CNearFieldNdefTarget* CNearFieldNdefTarget::NewL(MNfcTag * aNfcTag, RNfcServer& aNfcServer)
    {
    CNearFieldNdefTarget* self = CNearFieldNdefTarget::NewLC(aNfcTag, aNfcServer);
    CleanupStack::Pop(); // self;
    return self;
    }

void CNearFieldNdefTarget::ConstructL()
    {
    }

void CNearFieldNdefTarget::SetRealTarget(MNearFieldTarget * aRealTarget)
    {
    iTagConnection = aRealTarget;
    }

CNearFieldNdefTarget::~CNearFieldNdefTarget()
    {
    if (ERead == iCurrentOperation)
        {
        iNdefConnection->CancelRead();
        }
    else if (EWrite == iCurrentOperation)
        {
        iNdefConnection->CancelWrite();
        }
    if (iTagConnection)
        {
        delete iTagConnection;
        }
    else
        {
        delete iNfcTag;
        }
    }

CNearFieldTag * CNearFieldNdefTarget::CastToTag()
    {
    if (IsConnectionOpened())
        {
        CloseConnection();
        }
    return iTagConnection ? iTagConnection->CastToTag() : reinterpret_cast<CNearFieldTag *>(0);
    }

CNearFieldNdefTarget * CNearFieldNdefTarget::CastToNdefTarget()
    {
    TInt error = KErrNone;
    if (iTagConnection)
        {
        if (iTagConnection->IsConnectionOpened())
            {
            iTagConnection->CloseConnection();
            }
        }
    
    if (!IsConnectionOpened())
        {
        error = OpenConnection();
        }
    return (error == KErrNone) ? const_cast<CNearFieldNdefTarget *>(this)
                               : reinterpret_cast<CNearFieldNdefTarget *>(0);
    }

TInt CNearFieldNdefTarget::OpenConnection()
    {
    return iNfcTag->OpenConnection(*iNdefConnection);
    }

void CNearFieldNdefTarget::CloseConnection()
    {
    return iNfcTag->CloseConnection(*iNdefConnection);
    }

TBool CNearFieldNdefTarget::IsConnectionOpened()
    {
    return iNdefConnection->IsActivated();
    }

const TDesC8& CNearFieldNdefTarget::Uid() const
    {
    return iNfcTag->Uid();
    }

void CNearFieldNdefTarget::ReadComplete( CNdefRecord* /*aRecord*/, CNdefRecord::TNdefMessagePart /*aPart*/ )
    {
    }

void CNearFieldNdefTarget::ReadComplete( CNdefMessage* aMessage )
    {
    if (iCallback)
        {
        TInt errIgnore = KErrNone;
        TInt err = KErrNone;
        if (iMessages)
            {
            err = iMessages->Append(aMessage);
            }
        iMessages = 0;
        QT_TRYCATCH_ERROR(errIgnore, iCallback->ReadComplete(err));
        }
    iCurrentOperation = ENull;
    }

void CNearFieldNdefTarget::WriteComplete()
    {
    if (iCallback)
        {
        TInt errIgnore = KErrNone;
        QT_TRYCATCH_ERROR(errIgnore, iCallback->WriteComplete(KErrNone));
        }
    iCurrentOperation = ENull;
    }

void CNearFieldNdefTarget::HandleError( TInt aError )
    {
    if (iCallback)
        {
        iMessages = 0;
        if (ERead == iCurrentOperation)
            {
            iCallback->ReadComplete(aError);
            }
        else if (EWrite == iCurrentOperation)
            {
            iCallback->WriteComplete(aError);
            }
        }
    }   

TBool CNearFieldNdefTarget::hasNdefMessage()
    {
    TBool result = EFalse;
    if (!IsConnectionOpened())
        {
        result = (KErrNone == OpenConnection());
        }
    if (result)
        {
        TRAPD(err, result = iNdefConnection->IsEmptyL());
        result = (KErrNone == err) ? result : EFalse;
        }
    return result;
    }

TInt CNearFieldNdefTarget::ndefMessages(RPointerArray<CNdefMessage>& aMessages)
    {
    TInt error = KErrNone;
    if (iCurrentOperation != ENull)
        {
        error = KErrInUse;
        }
    else
        {
        if (!IsConnectionOpened())
            {
            error = OpenConnection();
            }
        if (KErrNone == error)
            {
            iMessages = &aMessages;
            error = iNdefConnection->ReadMessage();
            iCurrentOperation = (KErrNone == error) ? ERead : ENull;
            }
        }
    return error;
    }

TInt CNearFieldNdefTarget::setNdefMessages(const RPointerArray<CNdefMessage>& aMessages)
    {
    TInt error = KErrNone;
    CNdefMessage * message;
    if (iCurrentOperation != ENull)
        {
        error = KErrInUse;
        }
    else
        {
        if (aMessages.Count() > 0)
            {
            // current only support single ndef message
            message = aMessages[0];
            if (!IsConnectionOpened())
                {
                error = OpenConnection();
                }
            if (KErrNone == error)
                {
                error = iNdefConnection->WriteMessage(*message);
                iCurrentOperation = (KErrNone == error) ? EWrite : ENull;
                }
            }
        }
    return error;
    }

void CNearFieldNdefTarget::SetNdefOperationCallback(MNearFieldNdefOperationCallback * const aCallback)
    {
    iCallback = aCallback;
    }

MNearFieldNdefOperationCallback * CNearFieldNdefTarget::NdefOperationCallback()
    {
    return iCallback;
    }
