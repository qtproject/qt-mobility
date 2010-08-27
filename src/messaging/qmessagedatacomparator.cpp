/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

/*!
    \namespace QMessageDataComparator
    \ingroup messaging

    \brief The QMessageDataComparator namespace contains types used in specifying the comparison
    of MessageStore objects with user-defined values.
*/

/*!
    \enum QMessageDataComparator::EqualityComparator

    Defines the comparison operations that can be used to compare data elements 
    of QMessageManager objects for equality and inequality.

    \value Equal Represents the '==' operator.
    \value NotEqual Represents the '!=' operator.
*/

/*!
    \enum QMessageDataComparator::InclusionComparator

    Defines the comparison operations that can be used to compare data elements 
    of QMessageManager objects for inclusion or exclusion.

    \value Includes Represents an operation in which an associated property is tested to 
                    determine whether it includes a value.
    \value Excludes Represents an operation in which an associated property is tested to 
                    determine whether it is equal to the supplied value.
*/

/*!
    \enum QMessageDataComparator::RelationComparator

    Defines the comparison operations that can be used to compare data elements 
    of QMessageManager objects, according to a specific relation.

    \value LessThan Represents the '<' operator.
    \value LessThanEqual Represents the '<=' operator.
    \value GreaterThan Represents the '>' operator.
    \value GreaterThanEqual Represents the '>= operator'.
*/

/*!
    \enum QMessageDataComparator::MatchFlag

    This enum describes additional matching criteria when performing a search.
    
    \value MatchCaseSensitive   The key should be case sensitive. Not supported on QMF platform.
    \value MatchFullWord        The key should only match a complete word. Not supported on Windows and QMF platforms.
*/
