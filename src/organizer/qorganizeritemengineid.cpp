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

#include "qorganizeritemengineid.h"

/*!
  \class QOrganizerItemEngineId
  \relates QOrganizerItemId
  \brief The QOrganizerItemEngineId class uniquely identifies an item within a
  particular engine plugin.

  Clients of the Organizer API should never use this class.
  Every engine implementor must implement a class derived from
  QOrganizerItemEngineId.

  This class is provided so that engine implementors can implement their own
  id class (which may contain arbitrary data, and which may implement the
  required functions in an arbitrary manner).
 */

/*!
  \fn QOrganizerItemEngineId::~QOrganizerItemEngineId()
  Cleans up any memory in use by this engine item id.
 */

/*!
  \fn QOrganizerItemEngineId::isEqualTo(const QOrganizerItemEngineId* other) const
  Returns true if this id is equal to the \a other id; false otherwise.
  Note that when implementing this function, you do not have to check that the type is the same,
  since the function which calls this function (in QOrganizerItemId) does that check for you.
 */

/*!
  \fn QOrganizerItemEngineId::isLessThan(const QOrganizerItemEngineId* other) const
  Returns true if this id is less than the \a other id; false otherwise.
  Note that when implementing this function, you do not have to check that the type is the same,
  since the function which calls this function (in QOrganizerItemId) does that check for you.
 */

/*!
  \fn QOrganizerItemEngineId::engineIdType() const
  Returns an integer which identifies the type of the engine id.
  This is important because operators ==, < and != require ids of the same type.
  We suggest that engine implementors embed a globally unique UUID in their engine,
  and that this function should return the qHash() of that globally unique UUID;
  alternatively engine implementors can return a qHash() of their managerName.

  If the engine allows access to different datastores based on the construction parameters,
  the engine-specific implementation of QOrganizerItemEngineId may have to include
  a field which identifies which datastore the item was from, so that persisted item ids
  don't "magically" change which item they reference based on nothing more than the
  parameters the client uses to construct the manager.

  Alternatively, engine implementors can return a qHash() their managerUri; but be aware that
  any engine construction parameters which are used will cause type disparity (even if the
  engine only provides access to a single datastore, regardless of construction parameters).
  This may be a problem if (for example) clients store an item id (for example, a "favorite"
  event that the user wishes to be reminded about regularly) but don't necessarily construct
  the manager (which stores that item) with the same parameters each time.
 */

/*!
  \fn QOrganizerItemEngineId::clone() const
  Returns a deep-copy clone of this id.
  The caller takes ownership of the returned engine item id.
 */

/*!
  \fn QOrganizerItemEngineId::debugStreamOut(QDebug dbg)
  Streams this id out to the debug stream \a dbg.
 */

/*!
  \fn QOrganizerItemEngineId::dataStreamOut(QDataStream& out)
  Streams this id out to the data stream \a out.
 */

/*!
  \fn QOrganizerItemEngineId::dataStreamIn(QDataStream& in)
  Streams this id in from the datastream \a in.
 */

/*!
  \fn QOrganizerItemEngineId::hash() const
  Returns the hash value of this id.
 */

