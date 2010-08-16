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
* Description: 
*
*/




/**
 @file
 @publishedAll
 @released
*/
#ifndef __CLIENT_SERVER_H__
#define __CLIENT_SERVER_H__

namespace SharedConstants
	{
	_LIT(KServerName,"CntPerfTestServer");
	_LIT(KCustomLabelUid,"CustomLabelUid");
	_LIT(KUnifiedSingular,"UnifiedSingular");
	_LIT(KViewDefAllFieldsTest,"ViewDefAllFieldsTest");
	_LIT(KSimpleViewDefTest,"SimpleViewDefTests");
	_LIT(KViewDefAnyFieldsTest,"ViewDefAnyFieldsTest");
	_LIT(KViewDefCustomFieldsTest,"ViewDefCustomFieldsTest");
	_LIT(KCustomTemplateTest,"CustomTemplateTest");	
	_LIT(KAccessCountTests,"AccessCountTests");
	_LIT(KContactViewTest,"ContactViewTest");
	_LIT(KCreateDataBase,			"createdatabase");
	_LIT(KDbName,					"dbname");
	_LIT(KDeleteDataBase,			"deletedatabase");
	_LIT(KListOfViews,				"listofviews");
	_LIT(KIndex,						"index");
	_LIT(KFileToBeImported, 			"importfilename");
	_LIT(KVCardVersionPBAP, "pbap");
	const TInt KMaxViewTypeLength = 256;	
	const TInt KMaxBufferLength = 32;			
	}
#endif
