/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Retrieves the character map for each of the numeric keys.
*/

#ifndef CNTSQLSEARCHINTERFACE_H_
#define CNTSQLSEARCHINTERFACE_H_

#include <QString>

class CPcsKeyMap;

class CntSqlSearchInterface 
{
public:
    enum QueryType { NA, KoreaITUT };
    
public:
    
    virtual ~CntSqlSearchInterface() {};
    
public: //abstracts

    virtual QString createInputSpecificSearch(const QString &pattern) = 0;
    
    virtual QueryType getQueryType() = 0;
    
};
//#endif
#endif /* CNTSQLSEARCHINTERFACE_H_ */
