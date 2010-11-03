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
#include <iso14443connection.h>
#include <nfctag.h>
#include "nearfieldtagtype4_symbian.h"

/*!
    \class CNearFieldTagType4
    \brief The CNearFieldTagType4 class provides ways to access tag type3

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
*/

CNearFieldTagType4::CNearFieldTagType4(MNfcTag * aNfcTag, RNfcServer& aNfcServer) : CActive(EPriorityStandard), 
                                                                                    iNfcTag(aNfcTag),
                                                                                    iNfcServer(aNfcServer)
    {
    }

CNearFieldTagType4* CNearFieldTagType4::NewLC(MNfcTag * aNfcTag, RNfcServer& aNfcServer)
    {
    CNearFieldTagType4* self = new (ELeave) CNearFieldTagType4(aNfcTag, aNfcServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CNearFieldTagType4* CNearFieldTagType4::NewL(MNfcTag * aNfcTag, RNfcServer& aNfcServer)
    {
    CNearFieldTagType4* self = CNearFieldTagType4::NewLC(aNfcTag, aNfcServer);
    CleanupStack::Pop(); // self;
    return self;
    }

void CNearFieldTagType4::ConstructL()
    {
    iNfcType4Connection = CIso14443Connection::NewL(iNfcServer);
    iWait = new(ELeave)CActiveSchedulerWait();
    
    CActiveScheduler::Add(this); // Add to scheduler
    }

CNearFieldTagType4::~CNearFieldTagType4()
    {
    Cancel(); // Cancel any request, if outstanding
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    
    if (iNfcType4Connection->IsActivated())
        {
        iNfcTag->CloseConnection(*iNfcType4Connection);
        }
    
    delete iWait;
    delete iNfcType4Connection;
    delete iNfcTag;
    }

void CNearFieldTagType4::DoCancel()
    {
    iNfcType4Connection->ExchangeDataCancel();
    }

void CNearFieldTagType4::RunL()
    {
    iOperationError = iStatus.Int();
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    }

TInt CNearFieldTagType4::RunError(TInt aError)
    {
    return aError;
    }

CNearFieldTagType4 * CNearFieldTagType4::CastToTagType4()
    {
    TInt error = KErrNone;
    
    if (!IsConnectionOpened())
        {
        error = OpenConnection();
        }
    return (error == KErrNone) ? const_cast<CNearFieldTagType4 *>(this) 
                               : reinterpret_cast<CNearFieldTagType4 *>(0);
    }

TInt CNearFieldTagType4::OpenConnection()
    {
    return iNfcTag->OpenConnection(*iNfcType4Connection);
    }

void CNearFieldTagType4::CloseConnection()
    {
    return iNfcTag->CloseConnection(*iNfcType4Connection);
    }

TBool CNearFieldTagType4::IsConnectionOpened()
    {
    return iNfcType4Connection->IsActivated();
    }

TInt CNearFieldTagType4::SendAPDUCommand(const TDesC8& aCommand, TDes8& aResponse)
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
                iNfcType4Connection->ExchangeData(iStatus, aCommand, aResponse);
                SetActive();
                iWait->Start();
                error = iOperationError;
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
