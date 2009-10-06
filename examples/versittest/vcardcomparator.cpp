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

#include "vcardcomparator.h" 


// -----------------------------------------------------------------------------
// VCardComparator::VCardComparator
// -----------------------------------------------------------------------------
//
VCardComparator::VCardComparator(
    QIODevice& input,
    QIODevice& output,
    const QStringList& excludedFields) : mInputLines(0), mOutputLines(0)
{
    mInputLines = readLines(input,excludedFields);
    mOutputLines = readLines(output,excludedFields);
}

// -----------------------------------------------------------------------------
// VCardComparator::~VCardComparator
// -----------------------------------------------------------------------------
//
VCardComparator::~VCardComparator()
{
    delete mOutputLines;
    delete mInputLines;
}

// -----------------------------------------------------------------------------
// VCardComparator::firstNonMatchingLine
// -----------------------------------------------------------------------------
//
QString VCardComparator::nonMatchingLines() const
{   
    QString nonMatchingLines;
    for( int i=0;i<mInputLines->count();i++) {
        if (!mOutputLines->contains(mInputLines->at(i))) {
            nonMatchingLines.append(mInputLines->at(i));
        }
    }
    return nonMatchingLines;
}

// -----------------------------------------------------------------------------
// VCardComparator::readLines
// -----------------------------------------------------------------------------
//
QStringList* VCardComparator::readLines(
    QIODevice& device,
    const QStringList& excludedFields)
{
    QStringList* lines = new QStringList;
    while (!device.atEnd()) {
        QString line(device.readLine());
        bool addLine = true;
        foreach(QString excludedField, excludedFields) {
            if (line.startsWith(excludedField)) {
                addLine = false;
            }
        }
        if (addLine) {
            lines->append(line);
        }
    }
    return lines;
}

// End of File.
