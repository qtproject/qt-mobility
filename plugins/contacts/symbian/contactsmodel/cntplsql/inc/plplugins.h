/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/


#ifndef __PLPLUGINS_H__
#define __PLPLUGINS_H__

#include "persistencelayer.h"

//Forward declarations
class CContactPhoneNumberParser;

/** 
Base class for Contacts Model plugins.
*/
class NONSHARED CContactPlugin : public CBase
	{
public:
	~CContactPlugin();

protected:
	virtual void SetPluginL(TLibraryFunction aGateFunction) = 0;
	TUid LoadEComPluginFromInterface(TUid aInterfaceUid);
	CContactPlugin();

private:
	RLibrary iDll;
	};

	
/** 
Contact phone number parser plugin. 
*/
class NONSHARED CContactPhoneParser : public CContactPlugin
	{
public:
	static CContactPhoneParser* NewL();
	~CContactPhoneParser();

	inline CContactPhoneNumberParser* Parser() const;
	
	// From CContactPlugin.
	virtual void SetPluginL(TLibraryFunction aGateFunction);

private:
	void LoadEComPluginL();

private:
	CContactPhoneNumberParser* iParser;
	};


inline CContactPhoneNumberParser* CContactPhoneParser::Parser() const 
	{ return iParser; }


#endif // __PLPLUGINS_H__
