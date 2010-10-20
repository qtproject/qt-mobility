#include "qdeclarativeorganizeritemfetchhint_p.h"
/*!
   \qmlclass FetchHint QDeclarativeOrganizerItemFetchHint
   \brief The FetchHint element provides hints to the manager about which organizer item
  information needs to be retrieved in an asynchronous fetch request or a synchronous
  function call.

   \ingroup qml-organizer

   This element is part of the \bold{QtMobility.organizer 1.1} module.

   \sa {QOrganizerItemFetchHint}
 */

/*!
  \qmlproperty FetchHint::OptimizationHints  FetchHint::optimizationHints

  This property holds the optimization hint flags specified by the client.
  These hints may be ignored by the backend, in which case it will return
  the full set of information accessible in a organizer item.
  */
