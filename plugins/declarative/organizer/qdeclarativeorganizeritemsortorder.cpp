/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qdeclarativeorganizeritemsortorder_p.h"

/*!
   \qmlclass SortOrder QDeclarativeOrganizerItemSortOrder
   \brief The SortOrder element defines how a list of organizer item should be ordered according to some criteria.

   \ingroup qml-organizer

   This element is part of the \bold{QtMobility.organizer 1.1} module.

   \sa OrganizerModel, {QOrganizerItemSortOrder}
 */


/*!
  \qmlproperty string SortOrder::definitionName

  This property holds the detail definition name of the details which will be inspected to perform sorting.

  */

/*!
  \qmlproperty string SortOrder::fieldName

  This property holds the detail field name of the details which will be inspected to perform sorting.
  For each detail elements, there are predefined field types.
  */

/*!
  \qmlproperty enumeration SortOrder::blankPolicy
  This property enumerates the ways in which the sort order interprets blanks when sorting organizer.
  \list
  \o SortOrder.BlanksFirst - Considers blank values to evaluate to less than all other values in comparisons.
  \o SortOrder.BlanksLast - Considers blank values to evaluate to greater than all other values in comparisons.
  \endlist
 */

/*!
  \qmlproperty enumeration SortOrder::direction

  This property holds the direction of the sort order, the value can be one of:
  \list
  \o Qt.AscendingOrder - (default)
  \o Qt.DescendingOrder
  \endlist
  */

/*!
  \qmlproperty enumeration SortOrder::caseSensitivity

  This property holds the case sensitivity of the sort order, the value can be one of:
  \list
  \o Qt.CaseInsensitive
  \o Qt.CaseSensitive - (default)
  \endlist
  */
