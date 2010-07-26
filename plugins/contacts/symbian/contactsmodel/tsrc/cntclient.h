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


#include <cntitem.h>

class RTest;
class CCntClient : public CBase , public MContactDbObserver
	{
public:
	CCntClient(RTest &aTest, const TDesC &aDatabaseName);
	CContactDatabase* CreateDatabaseL();
	CContactDatabase* OpenDatabaseL();
	void CloseDatabase();
	
	~CCntClient();
	CContactDatabase* Db() {return(iDb);}
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);

private:
	CContactDatabase* iDb;
	TFileName iDatabaseName;
	RTest* iTest;
	CContactChangeNotifier* iNotifier;
	static TInt iObservers; //this variable tells how many CCntClient objects have registered for a database event notification
	};
