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
* Description: 
*/

#ifndef CNTSQLSEARCHBASE_H_
#define CNTSQLSEARCHBASE_H_

#include "cntsqlsearchinterface.h"
#include <QString>

class CntSqlSearchBase : public CntSqlSearchInterface 
{
 
public:
    
    CntSqlSearchBase();
    
    ~CntSqlSearchBase();
    
public: //from CCntSqlSearchInterface

    virtual QString createInputSpecificSearch(const QString &pattern) = 0;
    
    virtual QueryType getQueryType() = 0;
};
    

#endif /* CNTSQLSEARCHBASE_H_ */
