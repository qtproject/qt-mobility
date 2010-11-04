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
#include "nearfieldtagtype1_symbian.h"
#include "nearfieldtagtype2_symbian.h"
#include "nearfieldtagtype3_symbian.h"
#include "nearfieldtagtype4_symbian.h"
#include "nearfieldndeftarget_symbian.h"

/*!
    \class CNearFieldNdefTarget
    \brief The CNearFieldNdefTarget class provides ways to access tag type1

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
*/

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
    iWait = new (ELeave) CActiveSchedulerWait;
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

CNearFieldTagType1 * CNearFieldNdefTarget::CastToTagType1()
    {
    if (IsConnectionOpened())
        {
        CloseConnection();
        }
    return iTagConnection ? iTagConnection->CastToTagType1() : reinterpret_cast<CNearFieldTagType1 *>(0);
    }

CNearFieldTagType2 * CNearFieldNdefTarget::CastToTagType2()
    {
    return iTagConnection ? iTagConnection->CastToTagType2() : reinterpret_cast<CNearFieldTagType2 *>(0);
    }
CNearFieldTagType3 * CNearFieldNdefTarget::CastToTagType3()
    {
    return iTagConnection ? iTagConnection->CastToTagType3() : reinterpret_cast<CNearFieldTagType3 *>(0);
    }

CNearFieldTagType4 * CNearFieldNdefTarget::CastToTagType4()
    {
    return iTagConnection ? iTagConnection->CastToTagType4() : reinterpret_cast<CNearFieldTagType4 *>(0);
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

void CNearFieldNdefTarget::ReadComplete( CNdefRecord* aRecord, CNdefRecord::TNdefMessagePart aPart )
    {
    }

void CNearFieldNdefTarget::ReadComplete( CNdefMessage* aMessage )
    {
    iMessage = aMessage;
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    }

void CNearFieldNdefTarget::WriteComplete()
    {
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    }

void CNearFieldNdefTarget::HandleError( TInt aError )
    {
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

void CNearFieldNdefTarget::ndefMessages(RPointerArray<CNdefMessage>& aMessages)
    {
    TInt error = KErrNone;
    if (!IsConnectionOpened())
        {
        error = OpenConnection();
        }
    if (KErrNone == error)
        {
        if (!iWait->IsStarted())
            {
            error = iNdefConnection->ReadMessage();
            if (KErrNone == error)
                {
                iCurrentOperation = ERead;
                iWait->Start();
                iCurrentOperation = ENull;
                aMessages.Append(iMessage);
                }
            }
        else
            {
            error = KErrInUse;
            }
        }
    }

void CNearFieldNdefTarget::setNdefMessages(const RPointerArray<CNdefMessage>& aMessages)
    {
    CNdefMessage * message;
    if (aMessages.Count() > 0)
        {
        // current only support single ndef message
        message = aMessages[0];
        TInt error = KErrNone;
        if (!IsConnectionOpened())
            {
            error = OpenConnection();
            }
        if (KErrNone == error)
            {
            if (!iWait->IsStarted())
                {
                error = iNdefConnection->WriteMessage(*message);
                if (KErrNone == error)
                    {
                    iCurrentOperation = EWrite;
                    iWait->Start();
                    iCurrentOperation = ENull;
                    }
                }
            }
        else
            {
            error = KErrInUse;
            }
        }
    }
