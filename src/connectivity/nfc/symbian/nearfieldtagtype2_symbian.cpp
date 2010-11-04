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
#include <nfctype2connection.h>
#include <nfctype2address.h>
#include <nfctag.h>
#include "nearfieldtagtype2_symbian.h"

/*!
    \class CNearFieldTagType2
    \brief The CNearFieldTagType2 class provides ways to access tag type1

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
*/

static const TInt KBlockSize = 16;
static const TInt KCRCSize = 1;

CNearFieldTagType2::CNearFieldTagType2(MNfcTag * aNfcTag, RNfcServer& aNfcServer) : CActive(EPriorityStandard), 
                                                                                    iNfcTag(aNfcTag),
                                                                                    iNfcServer(aNfcServer)
    {
    }

CNearFieldTagType2* CNearFieldTagType2::NewLC(MNfcTag * aNfcTag, RNfcServer& aNfcServer)
    {
    CNearFieldTagType2* self = new (ELeave) CNearFieldTagType2(aNfcTag, aNfcServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CNearFieldTagType2* CNearFieldTagType2::NewL(MNfcTag * aNfcTag, RNfcServer& aNfcServer)
    {
    CNearFieldTagType2* self = CNearFieldTagType2::NewLC(aNfcTag, aNfcServer);
    CleanupStack::Pop(); // self;
    return self;
    }

void CNearFieldTagType2::ConstructL()
    {
    iNfcType2Connection = CNfcType2Connection::NewL(iNfcServer);
    iWait = new(ELeave)CActiveSchedulerWait();
    
    CActiveScheduler::Add(this); // Add to scheduler
    }

CNearFieldTagType2::~CNearFieldTagType2()
    {
    Cancel(); // Cancel any request, if outstanding
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    
    if (iNfcType2Connection->IsActivated())
        {
        iNfcTag->CloseConnection(*iNfcType2Connection);
        }
    
    delete iWait;
    delete iNfcType2Connection;
    delete iNfcTag;
    iNfcServer.Close();
    }

void CNearFieldTagType2::DoCancel()
    {
    if (ERead == iCurrentOperation)
        {
        iNfcType2Connection->ReadCancel();
        }
    else if (EWrite == iCurrentOperation)
        {
        iNfcType2Connection->WriteCancel();
        }
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    iOperationError = KErrCancel;
    iCurrentOperation = ENull;
    }

void CNearFieldTagType2::RunL()
    {
    iOperationError = iStatus.Int();
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    }

TInt CNearFieldTagType2::RunError(TInt aError)
    {
    return aError;
    }

CNearFieldTagType2 * CNearFieldTagType2::CastToTagType2()
    {
    TInt error = KErrNone;
    
    if (!IsConnectionOpened())
        {
        error = OpenConnection();
        }
    return (error == KErrNone) ? const_cast<CNearFieldTagType2 *>(this) 
                               : reinterpret_cast<CNearFieldTagType2 *>(0);
    }

TInt CNearFieldTagType2::OpenConnection()
    {
    return iNfcTag->OpenConnection(*iNfcType2Connection);
    }

void CNearFieldTagType2::CloseConnection()
    {
    return iNfcTag->CloseConnection(*iNfcType2Connection);
    }

TBool CNearFieldTagType2::IsConnectionOpened()
    {
    return iNfcType2Connection->IsActivated();
    }

TInt CNearFieldTagType2::ReadBlock(TUint8 aBlockAddress, TDes8& aResponse)
    {
    TInt error = KErrNone;
    if (!IsConnectionOpened())
        {
        error = OpenConnection();
        }
    if (KErrNone == error)
        {
        if (!IsActive())
            {
            if (!iWait->IsStarted())
                {
                iOperationError = KErrNone;
                iNfcType2Connection->Read(iStatus, aResponse, KBlockSize + KCRCSize, TNfcType2Address(0, aBlockAddress));
                SetActive();
                iCurrentOperation =  ERead;
                iWait->Start();
                error = iOperationError;
                iCurrentOperation = ENull;
                }
            else
                {
                error = KErrInUse;
                }
            }
        else
            {
            error = KErrInUse;
            }
        }
    return error;
    }

TInt CNearFieldTagType2::WriteBlock(TUint8 aBlockAddress, const TDesC8& aData)
    {
    TInt error = KErrNone;
    if (!IsConnectionOpened())
        {
        error = OpenConnection();
        }
    if (KErrNone == error)
        {
        if (!IsActive())
            {
            if (!iWait->IsStarted())
                {
                iOperationError = KErrNone;
                iNfcType2Connection->Write(iStatus, aData, aBlockAddress);
                SetActive();
                iCurrentOperation =  EWrite;
                iWait->Start();
                error = iOperationError;
                iCurrentOperation = ENull;
                }
            else
                {
                error = KErrInUse;
                }
            }
        else
            {
            error = KErrInUse;
            }
        }
    return error;
    }
    
TInt CNearFieldTagType2::SelectSector(TUint8 sector)
    {
    return KErrNotSupported;
    }
