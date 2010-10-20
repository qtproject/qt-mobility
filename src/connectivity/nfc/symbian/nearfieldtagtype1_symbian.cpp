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
#include <nfctag.h>
#include "nearfieldtagtype1_symbian.h"

CNearFieldTagType1::CNearFieldTagType1(MNfcTag * aNfcTag) : iNfcTag(aNfcTag), CActive(EPriorityStandard)
    {
    }

CNearFieldTagType1* CNearFieldTagType1::NewLC(MNfcTag * aNfcTag)
    {
    CNearFieldTagType1* self = new (ELeave) CNearFieldTagType1(aNfcTag);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CNearFieldTagType1* CNearFieldTagType1::NewL(MNfcTag * aNfcTag)
    {
    CNearFieldTagType1* self = CNearFieldTagType1::NewLC(aNfcTag);
    CleanupStack::Pop(); // self;
    return self;
    }

void CNearFieldTagType1::ConstructL()
    {
    iNfcType1Connection = CNfcType1Connection::NewL(iNfcServer);
    iWait = new(ELeave)CActiveSchedulerWait();
    
    User::LeaveIfError(iNfcTag->OpenConnection(*iNfcType1Connection));
    CActiveScheduler::Add(this); // Add to scheduler
    }

CNearFieldTagType1::~CNearFieldTagType1()
    {
    Cancel(); // Cancel any request, if outstanding
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    
    if (iNfcType1Connection->IsActivated())
        {
        iNfcTag->CloseConnection(*iNfcType1Connection);
        }
    
    delete iWait;
    delete iNfcType1Connection;
    delete iNfcTag;
    iNfcServer.Close();
    }

void CNearFieldTagType1::DoCancel()
    {
    }

void CNearFieldTagType1::RunL()
    {
    
    SetActive(); // Tell scheduler a request is active
    }

TInt CNearFieldTagType1::RunError(TInt aError)
    {
    return aError;
    }

void CNearFieldTagType1::ReadIdentificationL(TDes8& aData)
    {
    
    }
    
void CNearFieldTagType1::ReadAllL(TDes8& aData)
    {
    
    }

void CNearFieldTagType1::WriteByteEraseL(TUint8 aAddress, const TDesC8& aData)
    {
    
    }

void CNearFieldTagType1::WriteByteNoEraseL(TUint8 aAddress, const TDesC8& aData)
    {
    
    }

void CNearFieldTagType1::ReadByteL(TUint8 aAddress, TDes8& aData)
    {
    
    }

void CNearFieldTagType1::ReadSegmentL(TUint aSegmentAddress, TDes8& aData)
    {
    
    }

void CNearFieldTagType1::ReadBlockL(TUint aBlockAddress, TDes8& aData)
    {
    
    }

void CNearFieldTagType1::WriteBlockEraseL(TUint aBlockAddress, const TDesC8& aData)
    {
    
    }

void CNearFieldTagType1::WriteBlockNoEraseL(TUint aBlockAddress, const TDesC8& aData)
    {
    
    }

TNfcType1Address CNearFieldTagType1::AddOperand(TUint8 aAddress) const
    {
    const TUint8 staticBlockMask = 0x78;
    const TUint8 byteMask = 0x07;
    
    TNfcType1Address address;
    address.SetBlock((aAddress&staticBlockMask)>>3);
    address.SetByte(aAddress&byteMask);
    
    return address;
    }

TNfcType1Address CNearFieldTagType1::AddsOperand(TUint8 aSegmentAddress) const
    {
    
    }

TNfcType1Address CNearFieldTagType1::Add8Operand(TUint8 aBlockAddress) const
    {
    
    }
