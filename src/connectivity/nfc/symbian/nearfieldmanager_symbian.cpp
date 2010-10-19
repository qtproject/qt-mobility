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
#include "../qnearfieldmanager_symbian_p.h"

void CNearFieldManager::ConstructL()
    {
    User::LeaveIfError(iServer.Open());
    
    //create Tag discovery api
    iNfcTagDiscovery = CNfcTagDiscovery::NewL( iServer );
    iNfcTagDiscovery->AddTagConnectionListener( *this );
    
    iTagSubscription = CNfcTagSubscription::NewL();   
    
    //create LLCP provider api
    iLlcpProvider = CLlcpProvider::NewL( iServer );
    
//    //create NDEF discovery api
//    iNdefDiscovery = CNdefDiscovery::NewL( iServer );
//    iNdefDiscovery->AddNdefMessageListener( *this );
    }

TInt CNearFieldManager::InitConnectionL( MNfcConnection* aConnection,
    TNfcConnectionInfo::TNfcConnectionMode aMode )
    {
    iTagSubscription->AddConnectionModeL( aMode );
    iNfcTagDiscovery->AddTagSubscriptionL( *iTagSubscription );
    
    iLlcpProvider->AddLlcpLinkListenerL(*this);
    
    return KErrNone; 
    }
TInt CNearFieldManager::AddNdefSubscription( const CNdefRecord::TNdefRecordTnf aTnf, 
                                       const TDesC8& aType )
	{
	return iNdefDiscovery->AddNdefSubscription( aTnf, aType );
	}
void CNearFieldManager::RemoveNdefSubscription( const CNdefRecord::TNdefRecordTnf aTnf, 
                                          const TDesC8& aType )
	{
	iNdefDiscovery->RemoveNdefSubscription( aTnf, aType );
	}
void CNearFieldManager::AddTagSubscription( TNfcConnectionInfo::TNfcConnectionMode aMode )
	{
	if (iTagSubscription)
		{
		RArray<TNfcConnectionInfo::TNfcConnectionMode> array;
		iTagSubscription->GetConnectionModes(array);
		if( array.Find(aMode) != KErrNotFound)
			{
			iTagSubscription->AddConnectionModeL(aMode);
			}
		}
	}
void CNearFieldManager::TagDetected( MNfcTag* aNfcTag )
    {
	if (aNfcTag)
		{
		//TODO emit NearFieldTargetDetected signal
//		iCallback.targetFound(new QNearFieldTarget());
		}
    }

void CNearFieldManager::TagLost()
    {
	//TODO emit NearFieldTargetLost signal
    }
void CNearFieldManager::LlcpRemoteFound()
	{
    //TODO emit NearFieldTargetDetected signal
//	iCallback.targetFound(new QNearFieldTarget());
	}
void CNearFieldManager::LlcpRemoteLost()
	{
	//TODO emit NearFieldTargetLost signal
	}

/*void CNearFieldManager::MessageDetected( CNdefMessage* aMessage )
    {
    if ( aMessage )
        {
        //
        //TODO NDEF handling code here...
        //
        
        delete aMessage;  
        }
    }*/
CNearFieldManager::CNearFieldManager(QNearFieldManagerPrivateImpl& aCallback)
	: iCallback(aCallback)
    {    
    }

CNearFieldManager* CNearFieldManager::NewL(QNearFieldManagerPrivateImpl& aCallback)
    {
    CNearFieldManager* self = NewLC(aCallback);
    CleanupStack::Pop( self );  
    return self;
    }

CNearFieldManager* CNearFieldManager::NewLC(QNearFieldManagerPrivateImpl& aCallback)
    {
    CNearFieldManager* self = new (ELeave) CNearFieldManager(aCallback);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

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
