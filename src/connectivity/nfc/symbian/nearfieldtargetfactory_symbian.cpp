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
#include "nearfieldtargetfactory_symbian.h"
#include <nfcserver.h>
#include <nfctag.h>
#include <nfctype1connection.h>
#include <nfctype2connection.h>
#include <nfctype3connection.h>
#include <iso14443connection.h>
#include <mifareclassicconnection.h>
#include "nearfieldtag_symbian.h"
#include "nearfieldndeftarget_symbian.h"
#include "qnearfieldtagtype1_symbian_p.h"
#include "qnearfieldtagtype2_symbian_p.h"
#include "qnearfieldtagtype3_symbian_p.h"
#include "qnearfieldtagtype4_symbian_p.h"
#include "qnearfieldtagmifare_symbian_p.h"
#include "qnearfieldllcpdevice_symbian_p.h"
/*!
    \class TNearFieldTargetFactory
    \brief The TNearFieldTargetFactory class creates detected target instance according
           to the input tag infomation

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
*/
/*!
    Create target instance according to the tag infomation in \a aNfcTag and assign 
    the \a aParent as target's parent. 
*/
QNearFieldTarget * TNearFieldTargetFactory::CreateTargetL(MNfcTag * aNfcTag, RNfcServer& aNfcServer, QObject * aParent)
    {
    QNearFieldTarget * tag = 0;
    if (aNfcTag->HasConnectionMode(TNfcConnectionInfo::ENfcType1))
        {
        tag = CreateTagTypeL<CNfcType1Connection, QNearFieldTagType1Symbian>(aNfcTag, aNfcServer, aParent);
        }
    else if (aNfcTag->HasConnectionMode(TNfcConnectionInfo::ENfcType2))
        {
        tag = CreateTagTypeL<CNfcType2Connection, QNearFieldTagType2Symbian>(aNfcTag, aNfcServer, aParent);
        }
    else if (aNfcTag->HasConnectionMode(TNfcConnectionInfo::ENfcType3))
        {
        tag = CreateTagTypeL<CNfcType3Connection, QNearFieldTagType3Symbian>(aNfcTag, aNfcServer, aParent); 
        }
    else if (aNfcTag->HasConnectionMode(TNfcConnectionInfo::ENfc14443P4))
        {
        tag = CreateTagTypeL<CIso14443Connection, QNearFieldTagType4Symbian>(aNfcTag, aNfcServer, aParent);
        }
    else if (aNfcTag->HasConnectionMode(TNfcConnectionInfo::ENfcMifareStd))
        {
        tag = CreateTagTypeL<CMifareClassicConnection, QNearFieldTagMifareSymbian>(aNfcTag, aNfcServer, aParent);
        }
    else if (!aNfcTag)
        {
        tag = new (ELeave)QNearFieldLlcpDeviceSymbian(aNfcServer, aParent); 
        }
    return tag;
    }

/*!
    Create tag type instance according to the tag infomation in \a aNfcTag and assign 
    the \a aParent as target's parent. 
*/
template <typename CTAGCONNECTION, typename QTAGTYPE>
QNearFieldTarget * TNearFieldTargetFactory::CreateTagTypeL(MNfcTag * aNfcTag, RNfcServer& aNfcServer, QObject * aParent)
    {
    // ownership of aNfcTag transferred.
    CTAGCONNECTION * connection = CTAGCONNECTION::NewLC(aNfcServer);
    CNearFieldTag * tagType = CNearFieldTag::NewLC(aNfcTag, aNfcServer);
    tagType->SetConnection(connection);
    QTAGTYPE * tag= new(ELeave)QTAGTYPE(WrapNdefAccessL(aNfcTag, aNfcServer, tagType), aParent);
    tag->setAccessMethods(ConnectionMode2AccessMethods(aNfcTag));
    CleanupStack::Pop(tagType);
    CleanupStack::Pop(connection);
    return tag;
    }

   
MNearFieldTarget * TNearFieldTargetFactory::WrapNdefAccessL(MNfcTag * aNfcTag, RNfcServer& aNfcServer, MNearFieldTarget * aTarget)
    {
    if (aNfcTag->HasConnectionMode(TNfcConnectionInfo::ENdefConnection))
        {
        CNearFieldNdefTarget * ndefTarget = CNearFieldNdefTarget::NewLC(aNfcTag, aNfcServer);
        ndefTarget->SetRealTarget(aTarget);
        CleanupStack::Pop(ndefTarget);
        return ndefTarget;
        }
    else
        {
        return aTarget;
        }
    }
           
QNearFieldTarget::AccessMethods TNearFieldTargetFactory::ConnectionMode2AccessMethods(MNfcTag * aNfcTag)
    {
    QNearFieldTarget::AccessMethods accessMethod;
    if (aNfcTag->HasConnectionMode(TNfcConnectionInfo::ENdefConnection))
        {
        accessMethod |= QNearFieldTarget::NdefAccess;
        }
    if (!aNfcTag->HasConnectionMode(TNfcConnectionInfo::ENfcUnknownConnectionMode))
        {
        accessMethod |= QNearFieldTarget::TagTypeSpecificAccess;
        }
    return accessMethod;
    }
