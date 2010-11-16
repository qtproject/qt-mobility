/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Content handler for data that is text type
*
*/



#include "ndefmessage.h"
#include "ndefrecord.h"
#include "nfcqtadaptionplugin.h"


CNfcQtAdaptionPlugin* CNfcQtAdaptionPlugin::NewL()
    {
    qDebug() << "CNfcQtAdaptionPlugin::NewL begin" << endl;
    CNfcQtAdaptionPlugin* self = new (ELeave) CNfcQtAdaptionPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    qDebug() << "CNfcQtAdaptionPlugin::NewL end" << endl;
    return self;
    }


CNfcQtAdaptionPlugin::CNfcQtAdaptionPlugin():
    iServerStatus( NULL )
    {
    qDebug() << "CNfcQtAdaptionPlugin::CNfcQtAdaptionPlugin" << endl;
    }

void CNfcQtAdaptionPlugin::ConstructL()
    {
    qDebug() <<"CNfcQtAdaptionPlugin::ConstructL " << endl;
    }

CNfcQtAdaptionPlugin::~CNfcQtAdaptionPlugin()
    {
    qDebug() << "CNfcQtAdaptionPlugin::~CNfcQtAdaptionPlugin begin" << endl;
      
    if ( iServerStatus )
        {
        User::RequestComplete( iServerStatus, KErrAbort );
        }
    else
        {
        //empty
        }
    
   qDebug() << "CNfcQtAdaptionPlugin::~CNfcQtAdaptionPlugin end"  << endl;
    }


// ---------------------------------------------------------------------------
// From class CNfcCH.
// ---------------------------------------------------------------------------
//
void CNfcQtAdaptionPlugin::HandleNdefMessage(
    TRequestStatus& aRequestStatus,
    CNdefMessage* aMessage )
    {
    qDebug() << "CNfcQtAdaptionPlugin::HandleNdefMessage begin" << endl;

    aRequestStatus = KRequestPending;
    iServerStatus = &aRequestStatus;

    CNdefRecord* record = NULL;
    record = &aMessage->GetNdefRecordL( 0 );
       
    TInt err = KErrNone;
    QServiceManager manager;
    
    QServiceFilter filter("com.nokia.symbian.NdefMessageHandler");
    filter.setCustomAttribute("tnf" , QString::number(record->GetTnf()));
    filter.setCustomAttribute("type", QString::fromRawData(reinterpret_cast<const QChar*>(record->GetType().Ptr()),
            record->GetType().Length()));
    
    QList<QServiceInterfaceDescriptor> list = manager.findInterfaces(filter);
    
    if (list.count()>0)
        {
        //only invoke the first one
        QServiceInterfaceDescriptor desc = list[0];
        if (!desc.isValid()) {            
        qWarning() << "tnf: %d, type: %s service dataprovider not found";
        }
        
        QObject *service = manager.loadInterface(desc);
        if (!service) 
            {  
             qWarning() << "unable to connect service data provider";
            }
        }
    else
        {
    
        }
    delete aMessage;
    aMessage = NULL;
        
    qDebug() << "CNfcQtAdaptionPlugin::HandleNdefMessage end" << endl;
    }

// ---------------------------------------------------------------------------
// From class CNfcCH.
// ---------------------------------------------------------------------------
//
void CNfcQtAdaptionPlugin::HandleNdefMessageCancel()
    {       
    if ( iServerStatus )
        {
        User::RequestComplete( iServerStatus, KErrCancel );
        }
    else
        {
        //empty
        }
    }


