/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
#include <QStringList>

#include "cntsqlsearch.h"

CntSqlSearch::CntSqlSearch()
{


}

QString CntSqlSearch ::CreatePredictiveSearch(const QString &pattern)
{
return "SELECT contact_id FROM predictivesearch WHERE " +
   CreateSubStringSearch(pattern) + " ORDER BY first_name_as_number ASC;";
}


bool CntSqlSearch::IsSubStringSearch(const QString &pattern)
{
const QChar zero('0');
if (pattern.count( "0", Qt::CaseSensitive ) == pattern.count() )
    {
    return false;
    }
else if (pattern.contains(zero))
    {
    return true;
    }
else
    {
    return false;
    }
}

QStringList CntSqlSearch::GetNumber(const QString &pattern)
{
const QChar zero('0');
return pattern.split(zero, QString::SkipEmptyParts);
}
QString CntSqlSearch::CreateSubStringSearch(const QString &pattern)
{
QString queryString;
QStringList numbers;
if (IsSubStringSearch(pattern))
    {
    numbers = GetNumber(pattern);
    queryString = CreateStringSearch(pattern) + CreateSpaceStringSearch(numbers);
    }
else
    {
    queryString = CreateStringSearch(pattern);
    }

return queryString;
}

QString CntSqlSearch::CreateStringSearch(const QString &pattern)
{
QString queryString;
queryString = "(first_name_as_number LIKE % " + pattern +
        "%) OR (last_name_as_number LIKE % " + pattern + "%)";
return queryString;
}

QString CntSqlSearch::CreateSpaceStringSearch(QStringList numbers)
{
QString queryString;
QString queryItem;

for( int i = 0; i < numbers.count(); i++ )
    {
    if ( numbers.count() == 1 )
        {
        queryItem += CreateSpaceString(numbers.at(i));
        }
    else if ( numbers.count() - 1 == i )
        {
        queryItem += CreateSpaceString(numbers.at(i));
        }
    else
        {
        queryItem += CreateSpaceString(numbers.at(i)) + " AND ";
        }
    }
if (numbers.count() == 1)
    {
    queryString = " OR " + queryItem;
    }
else
    {
    queryString = " OR (" + queryItem + ")";
    }

return queryString;
}

QString CntSqlSearch::CreateSpaceString(QString number)
{
QString queryString;
queryString = "((first_name_as_number LIKE % " + number +
        "%) OR (last_name_as_number LIKE % " + number + "%))";
return queryString;
}



