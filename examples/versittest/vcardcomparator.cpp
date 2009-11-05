/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
        if (!mOutputLines->contains(mInputLines->at(i),Qt::CaseInsensitive)) {
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
        QString line = QString::fromAscii(device.readLine());
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
