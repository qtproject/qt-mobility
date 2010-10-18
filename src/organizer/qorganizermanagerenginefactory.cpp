/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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


#include "qorganizermanagerenginefactory.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerManagerEngineFactory
  \brief The QOrganizerManagerEngineFactory class provides the interface for
 plugins that implement QOrganizerManagerEngine functionality.

  \inmodule QtOrganizer
  \ingroup organizer-backends

  This class provides a simple interface for the creation of
  manager engine instances.  Each factory has a specific id
  associated with it, which forms the \c managerName parameter
  when creating \l QOrganizerManager objects.

  More information on writing a organizeritems engine plugin is available in
  the \l{Qt Organizer Manager Engines} documentation.

  \sa QOrganizerManager, QOrganizerManagerEngine
 */

/*!
  A default, empty destructor.
 */
QOrganizerManagerEngineFactory::~QOrganizerManagerEngineFactory()
{
}

/*!
  \fn QOrganizerManagerEngineFactory::engine(const QMap<QString, QString>& parameters, QOrganizerManager::Error* error)

  This function is called by the QOrganizerManager implementation to
  create an instance of the engine provided by this factory.

  The \a parameters supplied can be ignored or interpreted as desired.

  If a supplied parameter results in an unfulfillable request, or some other error
  occurs, this function may return a null pointer, and the client developer will get an
  invalid QOrganizerManager in return.  Any error should be stored in the supplied \a error
  reference.
 */

/*!
  \fn QOrganizerManagerEngineFactory::createCollectionEngineId() const

  This function is used internally when deserializing an id.  It allows the
  deserialization functions to correctly allocate an empty engine-specific
  id for an item, which it then fills with the serialized data.

  Engine implementers must implement this function, but should not need
  to call it.
 */

/*!
  \fn QOrganizerManagerEngineFactory::createItemEngineId() const

  This function is used internally when deserializing an id.  It allows the
  deserialization functions to correctly allocate an empty engine-specific
  id for a collection, which it then fills with the serialized data.

  Engine implementers must implement this function, but should not need
  to call it.
 */

/*!
  \fn QOrganizerManagerEngineFactory::managerName() const

  This function should return a unique string that identifies
  the engines provided by this factory.

  Typically this would be of the form "com.nokia.qt.organizeritems.engines.memory", with
  the appropriate domain and engine name substituted.
 */

/*!
  \fn QOrganizerManagerEngineFactory::supportedImplementationVersions() const

  This function should return a list of versions of the engine which this factory can instantiate.
 */
QList<int> QOrganizerManagerEngineFactory::supportedImplementationVersions() const
{
    return QList<int>();
}

QTM_END_NAMESPACE
