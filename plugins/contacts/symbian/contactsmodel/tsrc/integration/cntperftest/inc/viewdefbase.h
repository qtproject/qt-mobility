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

#ifndef __VIEWDEF_BASE_H__
#define __VIEWDEF_BASE_H__

#include "cntbasestep.h"

class CViewDefBase : public CCntBaseStep
	{
public:
	CViewDefBase(const TInt aContacts);

protected:
	virtual void InitializeL();
	virtual void Cleanup();
	void FieldsInView(	const CContactItemViewDef &aView, TInt &aRetNormal, 
						TInt &aRetHidden, const TBool aHasEmpty = EFalse);
	TInt FindInFieldSet(const CContactItemViewDef &aView, const TInt &aFrom);
	void AddMultipleUidsL( CContactItemViewDef &aView, TInt &aPos, const TInt &aMany );
protected:
	CContactItemViewDef *iViewII;
	CContactItemViewDef *iViewIM;
	CContactItemViewDef *iViewMI;
	CContactItemViewDef *iViewMM;
	CContactItemViewDef* iDefView;
	};

#endif
