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

#include "nearfieldmanager_symbian.h"
#include "nearfieldtargetfactory_symbian.h"
#include "../qnearfieldmanager_symbian_p.h"
#include "nearfieldutility_symbian.h"

#include <ndefmessage.h>

/*!
    \class CNearFieldManager
    \brief The Symbain class provides symbian backend implementation to access NFC service.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity

    A Symbian implementation class to support symbian NFC backend.
*/

/*!
    Constructs a CNearFieldManager.
*/
void CNearFieldManager::ConstructL()
    {
    User::LeaveIfError(iServer.Open());

    //create LLCP provider api
    iLlcpProvider = CLlcpProvider::NewL( iServer );
    iLlcpProvider->AddLlcpLinkListenerL( *this );

    }

/*!
    Start listening all type tags.
*/
void CNearFieldManager::StartTargetDetectionL(const QList<QNearFieldTarget::Type> &aTargetTypes)
    {
    if (aTargetTypes.size() > 0)
        {
        if (!iNfcTagDiscovery)
            {
            iNfcTagDiscovery = CNfcTagDiscovery::NewL( iServer );
            }
        else
            {
            iNfcTagDiscovery->RemoveTagConnectionListener();
            }
        User::LeaveIfError(iNfcTagDiscovery->AddTagConnectionListener( *this ));
        if (!iTagSubscription)
            {
            iTagSubscription = CNfcTagSubscription::NewL();
            }
        else
            {
            iTagSubscription->RemoveAllConnectionModes();
            }
        for (int i = 0; i < aTargetTypes.size(); ++i)
            {
            switch(aTargetTypes[i])
                {
                case QNearFieldTarget::NfcTagType1:
                    iTagSubscription->RemoveConnectionMode(TNfcConnectionInfo::ENfcType1);
                    iTagSubscription->AddConnectionModeL( TNfcConnectionInfo::ENfcType1 );
                    break;
                case QNearFieldTarget::NfcTagType2:
                    iTagSubscription->RemoveConnectionMode(TNfcConnectionInfo::ENfcType2);
                    iTagSubscription->AddConnectionModeL( TNfcConnectionInfo::ENfcType2 );
                    break;
                case QNearFieldTarget::NfcTagType3:
                    iTagSubscription->RemoveConnectionMode(TNfcConnectionInfo::ENfcType3);
                    iTagSubscription->AddConnectionModeL( TNfcConnectionInfo::ENfcType3 );
                    break;
                case QNearFieldTarget::NfcTagType4:
                    iTagSubscription->RemoveConnectionMode(TNfcConnectionInfo::ENfc14443P4);
                    iTagSubscription->AddConnectionModeL( TNfcConnectionInfo::ENfc14443P4 );
                    break;
                case QNearFieldTarget::MifareTag:
                    iTagSubscription->RemoveConnectionMode(TNfcConnectionInfo::ENfcMifareStd);
                    iTagSubscription->AddConnectionModeL( TNfcConnectionInfo::ENfcMifareStd );
                    break;
                case QNearFieldTarget::AnyTarget:
                    iTagSubscription->RemoveAllConnectionModes();
                    iTagSubscription->AddConnectionModeL( TNfcConnectionInfo::ENfcType1 );
                    iTagSubscription->AddConnectionModeL( TNfcConnectionInfo::ENfcType2 );
                    iTagSubscription->AddConnectionModeL( TNfcConnectionInfo::ENfcType3 );
                    iTagSubscription->AddConnectionModeL( TNfcConnectionInfo::ENfc14443P4 );
                    iTagSubscription->AddConnectionModeL( TNfcConnectionInfo::ENfcMifareStd );
                    break;
                case QNearFieldTarget::ProprietaryTag:
                    break;
                }
            }
        }
        iNfcTagDiscovery->AddTagSubscriptionL( *iTagSubscription );
    }

/*!
    Stop listening all type tags.
*/
void CNearFieldManager::stopTargetDetection()
    {
    if (iNfcTagDiscovery)
        {
        iNfcTagDiscovery->RemoveTagConnectionListener();
        iNfcTagDiscovery->RemoveTagSubscription();
        delete iNfcTagDiscovery;
        iNfcTagDiscovery = NULL;
        if (iTagSubscription)
            {
            delete iTagSubscription;
            iTagSubscription = NULL;
            }
        }
    }

/*!
    Register interested TNF NDEF message to NFC server.
*/
TInt CNearFieldManager::AddNdefSubscription( const QNdefRecord::TypeNameFormat aTnf,
                                       const QByteArray& aType )
    {
    TInt err = KErrNone;
    if ( !iNdefDiscovery )
        {
        TRAP(err, iNdefDiscovery = CNdefDiscovery::NewL( iServer ));
        if (err != KErrNone)
            {
            return err;
            }
        err = iNdefDiscovery->AddNdefMessageListener( *this );
        if (err != KErrNone)
            {
            return err;
            }

        }
    TPtrC8 type(QNFCNdefUtility::FromQByteArrayToTPtrC8(aType));
    err = iNdefDiscovery->AddNdefSubscription( (CNdefRecord::TNdefRecordTnf)aTnf, type );
    return err;
    }

/*!
    Unregister interested TNF NDEF message to NFC server.
*/
void CNearFieldManager::RemoveNdefSubscription( const QNdefRecord::TypeNameFormat aTnf,
                                          const QByteArray& aType )
    {
    if ( iNdefDiscovery )
        {
        TPtrC8 type(QNFCNdefUtility::FromQByteArrayToTPtrC8(aType));
        iNdefDiscovery->RemoveNdefSubscription( (CNdefRecord::TNdefRecordTnf)aTnf, type );
        }
    }

/*!
    Callback function when the tag found by NFC symbain services.
*/
void CNearFieldManager::TagDetected( MNfcTag* aNfcTag )
    {
    TRAP_IGNORE(
        if (aNfcTag)
            {
            QNearFieldTarget* tag = TNearFieldTargetFactory::CreateTargetL(aNfcTag, iServer, &iCallback);
            CleanupStack::PushL(tag);
            QT_TRYCATCH_LEAVING( iCallback.targetFound(tag) );
            CleanupStack::Pop(tag);
            }
            );
    }

/*!
    Callback function when the tag lost event found by NFC symbain services.
*/
void CNearFieldManager::TagLost()
    {
    TRAP_IGNORE(
            QT_TRYCATCH_LEAVING(iCallback.targetDisconnected());
        );
    }

/*!
    Callback function when the LLCP peer found by NFC symbain services.
*/
void CNearFieldManager::LlcpRemoteFound()
    {
    TRAP_IGNORE(
        QNearFieldTarget* tag = TNearFieldTargetFactory::CreateTargetL(NULL, iServer, &iCallback);
        CleanupStack::Pop(tag);
        QT_TRYCATCH_LEAVING( iCallback.targetFound(tag) );
        CleanupStack::Pop(tag);
        );
    }

/*!
    Callback function when the LLCP peer lost event found by NFC symbain services.
*/
void CNearFieldManager::LlcpRemoteLost()
    {
    TRAP_IGNORE(
            QT_TRYCATCH_LEAVING(iCallback.targetDisconnected());
        );
    }

/*!
    Callback function when the registerd NDEF message found by NFC symbain services.
*/
void CNearFieldManager::MessageDetected( CNdefMessage* aMessage )
    {
    if ( aMessage )
        {
        TRAP_IGNORE(
                QNdefMessage msg = QNFCNdefUtility::FromCNdefMsgToQndefMsgL( *aMessage);
           QT_TRYCATCH_LEAVING(iCallback.invokeTargetDetectedHandler(msg));
           delete aMessage;
        );
        }
    }

/*!
    New a CNearFieldManager instance.
*/
CNearFieldManager::CNearFieldManager( QtMobility::QNearFieldManagerPrivateImpl& aCallback)
    : iCallback(aCallback)
    {
    }

/*!
    Create a new instance of this class.
*/
CNearFieldManager* CNearFieldManager::NewL( QtMobility::QNearFieldManagerPrivateImpl& aCallback)
    {
    CNearFieldManager* self = NewLC(aCallback);
    CleanupStack::Pop( self );
    return self;
    }

/*!
    Create a new instance of this class and push to cleanup stack.
*/
CNearFieldManager* CNearFieldManager::NewLC( QtMobility::QNearFieldManagerPrivateImpl& aCallback)
    {
    CNearFieldManager* self = new (ELeave) CNearFieldManager(aCallback);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/*!
    Destructor.
*/
CNearFieldManager::~CNearFieldManager()
    {
    if ( iNfcTagDiscovery )
        {
        iNfcTagDiscovery->RemoveTagConnectionListener();
        iNfcTagDiscovery->RemoveTagSubscription();
        }

    delete iTagSubscription;
    delete iNfcTagDiscovery;

    if (iLlcpProvider)
        {
        iLlcpProvider->RemoveLlcpLinkListener();
        }
    delete iLlcpProvider;

    if (iNdefDiscovery)
        {
        iNdefDiscovery->RemoveAllNdefSubscription();
        }
    delete iNdefDiscovery;

    iServer.Close();
    }
