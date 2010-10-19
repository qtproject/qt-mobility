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
  \fn QOrganizerItemEngineId::managerUri() const
  Returns the manager URI of the constructed manager which created
  the id.  If the item which the id identifies has not been deleted,
  the id should still be valid in the manager identified by the
  manager URI returned by this function.
 */

/*!
  \fn QOrganizerItemEngineId::toString() const
  Serializes the id to a string.  It contains all of the information
  required to identify a particular item in the manager which created
  the id, formatted according to the serialization format of the
  manager.
 */

/*!
  \fn QOrganizerItemEngineId::clone() const
  Returns a deep-copy clone of this id.
  The caller takes ownership of the returned engine item id.
 */

/*!
  \fn QOrganizerItemEngineId::debugStreamOut(QDebug& dbg) const
  Streams this id out to the debug stream \a dbg.
 */

/*!
  \fn QOrganizerItemEngineId::hash() const
  Returns the hash value of this id.
 */

