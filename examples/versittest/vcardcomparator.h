/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef VCARDCOMPARATOR_H
#define VCARDCOMPARATOR_H

#include <QObject>
#include <QIODevice>
#include <QStringList>


class VCardComparator : public QObject
{
public: // Constructors and destructor

    VCardComparator(
        QIODevice& input,
        QIODevice& output,
        const QStringList& excludedFields);
    ~VCardComparator();    
    
public: // New functions
    QString nonMatchingLines() const;
    
private: // New functions
    QStringList* readLines(
        QIODevice& device,
        const QStringList& excludedFields);
    
private: // Data
    QStringList* mInputLines;
    QStringList* mOutputLines;
};

#endif // VCARDCOMPARATOR_H
