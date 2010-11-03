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
#include <nfctype3connection.h>
#include <nfctag.h>
#include "nearfieldtagtype3_symbian.h"

/*!
    \class CNearFieldTagType3
    \brief The CNearFieldTagType3 class provides ways to access tag type3

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
*/

CNearFieldTagType3::CNearFieldTagType3(MNfcTag * aNfcTag, RNfcServer& aNfcServer) : CActive(EPriorityStandard), 
                                                                                    iNfcTag(aNfcTag),
                                                                                    iNfcServer(aNfcServer)
    {
    }

CNearFieldTagType3* CNearFieldTagType3::NewLC(MNfcTag * aNfcTag, RNfcServer& aNfcServer)
    {
    CNearFieldTagType3* self = new (ELeave) CNearFieldTagType3(aNfcTag, aNfcServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CNearFieldTagType3* CNearFieldTagType3::NewL(MNfcTag * aNfcTag, RNfcServer& aNfcServer)
    {
    CNearFieldTagType3* self = CNearFieldTagType3::NewLC(aNfcTag, aNfcServer);
    CleanupStack::Pop(); // self;
    return self;
    }

void CNearFieldTagType3::ConstructL()
    {
    iNfcType3Connection = CNfcType3Connection::NewL(iNfcServer);
    iWait = new(ELeave)CActiveSchedulerWait();
    
    CActiveScheduler::Add(this); // Add to scheduler
    }

CNearFieldTagType3::~CNearFieldTagType3()
    {
    Cancel(); // Cancel any request, if outstanding
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    
    if (iNfcType3Connection->IsActivated())
        {
        iNfcTag->CloseConnection(*iNfcType3Connection);
        }
    
    delete iWait;
    delete iNfcType3Connection;
    delete iNfcTag;
    }

void CNearFieldTagType3::DoCancel()
    {
    }

void CNearFieldTagType3::RunL()
    {
    
    SetActive(); // Tell scheduler a request is active
    }

TInt CNearFieldTagType3::RunError(TInt aError)
    {
    return aError;
    }

CNearFieldTagType3 * CNearFieldTagType3::CastToTagType3()
    {
    TInt error = KErrNone;
    
    if (!IsConnectionOpened())
        {
        error = OpenConnection();
        }
    return (error == KErrNone) ? const_cast<CNearFieldTagType3 *>(this) 
                               : reinterpret_cast<CNearFieldTagType3 *>(0);
    }

TInt CNearFieldTagType3::OpenConnection()
    {
    return iNfcTag->OpenConnection(*iNfcType3Connection);
    }

void CNearFieldTagType3::CloseConnection()
    {
    return iNfcTag->CloseConnection(*iNfcType3Connection);
    }

TBool CNearFieldTagType3::IsConnectionOpened()
    {
    return iNfcType3Connection->IsActivated();
    }
