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
