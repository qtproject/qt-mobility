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


#ifndef T_VIEWSTORE_H_
#define T_VIEWSTORE_H_

/*
  Simple helper class used to create pre-defined view definitions
*/
class CContactViewDefinitionGenerator : public CBase
	{
public:
	enum TViewDescription
		{
		ETechviewContactsAppView = 0,
		EValidLFCView,
		EValidLFPView,
		ENumValidViewDescriptions,

		EInvalidViewNameView = ENumValidViewDescriptions,
		EInvalidSortPluginNameView,
		
		ENumViewDescriptions
		};
public:

	static CContactViewDefinitionGenerator* NewLC();
	CContactDefaultViewDefinition* CreateDefinitionLC(TViewDescription aDesc);
	
private:
	void ConstructL();
	};

#endif // T_VIEWSTORE_H_
