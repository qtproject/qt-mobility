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

/*!
    \class CNearFieldTagType1
    \brief The CNearFieldTagType1 class provides ways to access tag type1

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
*/

CNearFieldTagType1::CNearFieldTagType1(MNfcTag * aNfcTag, RNfcServer& aNfcServer) : CActive(EPriorityStandard), 
                                                                                    iNfcTag(aNfcTag),
                                                                                    iNfcServer(aNfcServer)
    {
    }

CNearFieldTagType1* CNearFieldTagType1::NewLC(MNfcTag * aNfcTag, RNfcServer& aNfcServer)
    {
    CNearFieldTagType1* self = new (ELeave) CNearFieldTagType1(aNfcTag, aNfcServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CNearFieldTagType1* CNearFieldTagType1::NewL(MNfcTag * aNfcTag, RNfcServer& aNfcServer)
    {
    CNearFieldTagType1* self = CNearFieldTagType1::NewLC(aNfcTag, aNfcServer);
    CleanupStack::Pop(); // self;
    return self;
    }

void CNearFieldTagType1::ConstructL()
    {
    iNfcType1Connection = CNfcType1Connection::NewL(iNfcServer);
    iWait = new(ELeave)CActiveSchedulerWait();
    
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

/*!
    Returns identification information read from the tag.

    The returned byte array contains HR0, HR1, UID0, UID1, UID2, UID3 in order. An empty byte array
    is returned if an error occurs.
*/

const TDesC8& CNearFieldTagType1::ReadIdentification()
    {
    return iNfcTag->Uid(); 
    }

/*!
    Reads and returns HR0, HR1 and all data in the static memory area of the tag.

    The returned byte array contains HR0, HR1 followed by 120 bytes of static data. An empty byte
    array is returned if an error occurs.
*/
    
TInt CNearFieldTagType1::ReadAll(TDes8& aData)
    {
#if 0
    return iNfcType1Connection->ReadAll(aData);    
#endif
    }

/*!
    Writes a single \a data byte to the linear byte \a address on the tag. If \a mode is
    EraseAndWrite the byte is erased before writing. If \a mode is WriteOnly the contents are not
    earsed before writing. This is equivelant to writing the result of the bitwise or of \a data
    and the original value.

    Returns true on success; otherwise returns false.
*/
TInt CNearFieldTagType1::WriteByteErase(TUint8 aAddress, TUint8 aData)
    {
#if 0
    return iNfcType1Connection->WriteByteErase(aAddress, aData); 
#endif
    }

/*!
    Writes a single \a data byte to the linear byte \a address on the tag. If \a mode is
    EraseAndWrite the byte is erased before writing. If \a mode is WriteOnly the contents are not
    earsed before writing. This is equivelant to writing the result of the bitwise or of \a data
    and the original value.

    Returns true on success; otherwise returns false.
*/
TInt CNearFieldTagType1::WriteByteNoErase(TUint8 aAddress, TUint8 aData)
    {
#if 0
    return iNfcType1Connection->WriteByteNoErase(aAddress, aData); 
#endif
    }


/*!
    Reads and returns a single byte from the static memory area of the tag. The \a address
    parameter specifices the linear byte address to read.
*/
TInt CNearFieldTagType1::ReadByte(TUint8 aAddress, TUint8& aData)
    {
#if 0
    return iNfcType1Connection->ReadByte(aAddress, aData); 
#endif
    }

/*!
    Reads and returns 120 bytes of data from the segment specified by \a segmentAddress. An empty
    byte array is returned if an error occurs.
*/
TInt CNearFieldTagType1::ReadSegment(TUint aSegmentAddress, TDes8& aData)
    {
#if 0
    return iNfcType1Connection->ReadSegment(aSegmentAddress, aData); 
#endif
    }

/*!
    Reads and returns 8 bytes of data from the block specified by \a blockAddress. An empty byte
    array is returned if an error occurs.
*/
TInt CNearFieldTagType1::ReadBlock(TUint aBlockAddress, TDes8& aData)
    {
#if 0
    return iNfcType1Connection->ReadBlock(aBlockAddress, aData); 
#endif
    }

/*!
    Writes 8 bytes of \a data to the block specified by \a blockAddress. If \a mode is
    EraseAndWrite the bytes are erased before writing. If \a mode is WriteOnly the contents are not
    earsed before writing. This is equivelant to writing the result of the bitwise or of \a data
    and the original value.

    Returns true on success; otherwise returns false.

*/
TInt CNearFieldTagType1::WriteBlockErase(TUint aBlockAddress, const TDesC8& aData)
    {
#if 0
    return iNfcType1Connection->WriteBlockErase(aBlockAddress, aData); 
#endif
    }


/*!
    Writes 8 bytes of \a data to the block specified by \a blockAddress. If \a mode is
    EraseAndWrite the bytes are erased before writing. If \a mode is WriteOnly the contents are not
    earsed before writing. This is equivelant to writing the result of the bitwise or of \a data
    and the original value.

    Returns true on success; otherwise returns false.

*/
TInt CNearFieldTagType1::WriteBlockNoErase(TUint aBlockAddress, const TDesC8& aData)
    {
#if 0
    return iNfcType1Connection->WriteBlockErase(aBlockAddress, aData); 
#endif
    }

/*!
    Convert \a aAddress to byte-block address
*/
TNfcType1Address CNearFieldTagType1::AddOperand(TUint8 aAddress) const
    {
    const TUint8 staticBlockMask = 0x78;
    const TUint8 byteMask = 0x07;
    
    TNfcType1Address address;
    address.SetBlock((aAddress&staticBlockMask)>>3);
    address.SetByte(aAddress&byteMask);
    
    return address;
    }

CNearFieldTagType1 * CNearFieldTagType1::CastToTagType1()
    {
    TInt error = KErrNone;
    
    if (!IsConnectionOpened())
        {
        error = OpenConnection();
        }
    return (error == KErrNone) ? const_cast<CNearFieldTagType1 *>(this) 
                               : reinterpret_cast<CNearFieldTagType1 *>(0);
    }

TInt CNearFieldTagType1::OpenConnection()
    {
    return iNfcTag->OpenConnection(*iNfcType1Connection);
    }

void CNearFieldTagType1::CloseConnection()
    {
    return iNfcTag->CloseConnection(*iNfcType1Connection);
    }

TBool CNearFieldTagType1::IsConnectionOpened()
    {
    return iNfcType1Connection->IsActivated();
    }
