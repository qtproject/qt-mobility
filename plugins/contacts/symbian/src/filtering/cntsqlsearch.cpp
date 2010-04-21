/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QStringList>

#include "cntsqlsearch.h"

CntSqlSearch::CntSqlSearch()
{


}

QString CntSqlSearch::CreatePredictiveSearch(const QString &pattern)
{  
    if (pattern.length() == 1)
        {
        return "SELECT contact_id FROM " + SelectTableView(pattern) + " ORDER BY first_name_as_number ASC;";
        }
    else
        {
        return "SELECT contact_id FROM " + SelectTableView(pattern) + " WHERE " +
            CreateSubStringSearch(pattern) + " ORDER BY first_name_as_number ASC;";
        }
}

QString CntSqlSearch::SelectTableView(const QString &pattern)
{
    QString view;
    QString firstNumber = pattern.at(0);
    int num = firstNumber.toInt();
    switch (num)
        {

        case 0:
            {
            view = QString("view0");
            }
        break;
        case 1:
            {
            view = QString("view1");
            }
        break;
        case 2:
            {
            view = QString("view2");
            }
        break;
        case 3:
            {
            view = QString("view3");
            }
        break;
        case 4:
            {
            view = QString("view4");
            }
        break;
        case 5:
            {
            view = QString("view5");
            }
        break;
        case 6:
            {
            view = QString("view6");
            }
        break;
        case 7:
            {
            view = QString("view7");
            }
        break;
        case 8:
            {
            view = QString("view8");
            }
        break;
        case 9:
            {
            view = QString("view9");
            }
        break;
        }
return view;
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
numbers = GetNumber(pattern);

if (IsSubStringSearch(pattern) && numbers.count() > 1 )
    {
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
queryString = "(first_name_as_number LIKE '% " + pattern +
        "%') OR (last_name_as_number LIKE '% " + pattern + "%')";
return queryString;
}

QString CntSqlSearch::CreateSpaceStringSearch(QStringList numbers)
{
QString queryString;
QString queryItem;

for( int i = 0; i < numbers.count(); i++ )
    {
    if ((numbers.count() == 1) || ((numbers.count() - 1) == i ))
        {
        queryItem += CreateSpaceString(numbers.at(i));
        }
    else
        {
        queryItem += CreateSpaceString(numbers.at(i)) + " OR ";
        }
    }

return queryString  = " OR " + queryItem;
}

QString CntSqlSearch::CreateSpaceString(QString number)
{
return QString("((first_name_as_number LIKE '% " + number + "%') AND (first_name_as_number LIKE '%_ " + number + "%')) OR ((last_name_as_number LIKE '% " + number + "%') AND (last_name_as_number LIKE '%_ " + number + "%'))");
}


