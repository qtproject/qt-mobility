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

#ifndef CNTSQLKOREANITUT_H_
#define CNTSQLKOREANITUT_H_

#include <QString>
#include "cntsqlsearchbase.h"

class CntSqlKoreanItuT : public CntSqlSearchBase 
{
 

public:
    CntSqlKoreanItuT();
    
    ~CntSqlKoreanItuT();
    
public: //from CCntSqlSearchBase

    QString createInputSpecificSearch(const QString &pattern);
    
    QueryType getQueryType();
    

private:
    
    
};
//#endif

#endif /* CNTSQLKOREANITUT_H_ */
