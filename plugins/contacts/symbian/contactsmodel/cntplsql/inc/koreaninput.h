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
*
*/

#ifndef KOREANINPUT_H
#define KOREANINPUT_H

class QStringList;
class QString;
class QChar;

class KoreanInput
{
public:
    KoreanInput();
    ~KoreanInput();
    QStringList Tokenize(QString sourcestring);
    
private:
    int countConsecutive( QString stringToCount, QChar keychar, int currentpos );
    int findEndchar( QString sourcestring, QChar startchar, int startpoint );
};

#endif // KOREANINPUT_H
