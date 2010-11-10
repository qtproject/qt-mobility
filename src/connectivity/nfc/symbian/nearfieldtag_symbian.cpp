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
#include <nfctag.h>
#include <nfcconnection.h>
#include "nearfieldtag_symbian.h"

/*!
    \class CNearFieldTag
    \brief The CNearFieldTag class provides ways to access tag type1

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
*/

CNearFieldTag::CNearFieldTag(MNfcTag * aNfcTag, RNfcServer& aNfcServer) : iNfcTag(aNfcTag),
                                                                          iNfcServer(aNfcServer)
    {
    }

CNearFieldTag* CNearFieldTag::NewLC(MNfcTag * aNfcTag, RNfcServer& aNfcServer)
    {
    CNearFieldTag* self = new (ELeave) CNearFieldTag(aNfcTag, aNfcServer);
    CleanupStack::PushL(self);
    return self;
    }

CNearFieldTag* CNearFieldTag::NewL(MNfcTag * aNfcTag, RNfcServer& aNfcServer)
    {
    CNearFieldTag* self = CNearFieldTag::NewLC(aNfcTag, aNfcServer);
    CleanupStack::Pop(); // self;
    return self;
    }

CNearFieldTag::~CNearFieldTag()
    {
    delete iTagConnection;
    delete iNfcTag;
    }

CNearFieldTag * CNearFieldTag::CastToTag()
    {
    TInt error = KErrNone;
    
    if (!IsConnectionOpened())
        {
        error = OpenConnection();
        }
    return (error == KErrNone) ? const_cast<CNearFieldTag *>(this) 
                               : reinterpret_cast<CNearFieldTag *>(0);
    }

TInt CNearFieldTag::OpenConnection()
    {
    return iNfcTag->OpenConnection(*iTagConnection);
    }

void CNearFieldTag::CloseConnection()
    {
    return iNfcTag->CloseConnection(*iTagConnection);
    }

TBool CNearFieldTag::IsConnectionOpened()
    {
    return iTagConnection->IsActivated();
    }

TInt CNearFieldTag::RawModeAccess(const TDesC8& aCommand, TDes8& aResponse, const TTimeIntervalMicroSeconds32& aTimeout)
    {
    return (IsConnectionOpened()) ? iTagConnection->RawModeAccess(aCommand, aResponse, aTimeout)
                                  : KErrInUse;
    }
