/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


#include <e32test.h>
#include "cntclient.h"

CCntClient::CCntClient(RTest &aTest, const TDesC &aDatabaseName)
	{
	iDatabaseName.Copy(aDatabaseName);
 	iTest=&aTest;
	iTest->Start(_L("New client"));
	}

CCntClient::~CCntClient()
	{
	CloseDatabase();
	iTest->End();
	}

CContactDatabase* CCntClient::CreateDatabaseL()
	{
	CloseDatabase();
	iDb=CContactDatabase::ReplaceL(iDatabaseName);
	iNotifier = CContactChangeNotifier::NewL(*iDb,this);
	iObservers++; //
	return(iDb);
	}

CContactDatabase* CCntClient::OpenDatabaseL()
	{
	if(!iDb)
		iDb=CContactDatabase::OpenL(iDatabaseName);
	if(!iNotifier)
		{
		iNotifier = CContactChangeNotifier::NewL(*iDb,this);
		iObservers++;//
		}
	return(iDb);
	}

void CCntClient::CloseDatabase()
	{
	if (iNotifier)
		{
		iObservers--;
		}
	delete iNotifier;
	iNotifier = NULL;
	delete iDb;
	iDb=NULL;
	}

void CCntClient::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
	{
	static TInt i=0;
	if(aEvent.iType == EContactDbObserverEventGroupAdded) 
		{
		i++;
		}

	if (i==iObservers)
		{
		// Stop the scheduler when all the clients have handled the event
		CActiveScheduler::Stop();
		i=0;
		}
	}
