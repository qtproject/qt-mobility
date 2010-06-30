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


#include "qorganizeritemmanagerenginefactory.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItemManagerEngineFactory
  \brief The QOrganizerItemManagerEngineFactory class provides the interface for
 plugins that implement QOrganizerItemManagerEngine functionality.
  \ingroup organizeritems-backends

  This class provides a simple interface for the creation of
  manager engine instances.  Each factory has a specific id
  associated with it, which forms the \c managerName parameter
  when creating \l QOrganizerItemManager objects.

  More information on writing a organizeritems engine plugin is available in
  the \l{Qt OrganizerItems Manager Engines} documentation.

  \sa QOrganizerItemManager, QOrganizerItemManagerEngine
 */

/*!
  A default, empty destructor.
 */
QOrganizerItemManagerEngineFactory::~QOrganizerItemManagerEngineFactory()
{
}

/*!
  \fn QOrganizerItemManagerEngineFactory::engine(const QMap<QString, QString>& parameters, QOrganizerItemManager::Error* error)

  This function is called by the QOrganizerItemManager implementation to
  create an instance of the engine provided by this factory.

  The \a parameters supplied can be ignored or interpreted as desired.

  If a supplied parameter results in an unfulfillable request, or some other error
  occurs, this function may return a null pointer, and the client developer will get an
  invalid QOrganizerItemManager in return.  Any error should be stored in the supplied \a error
  reference.
 */

/*!
  \fn QOrganizerItemManagerEngineFactory::managerName() const

  This function should return a unique string that identifies
  the engines provided by this factory.

  Typically this would be of the form "com.nokia.qt.organizeritems.engines.memory", with
  the appropriate domain and engine name substituted.
 */

/*!
  \fn QOrganizerItemManagerEngineFactory::supportedImplementationVersions() const

  This function should return a list of versions of the engine which this factory can instantiate.
 */
QList<int> QOrganizerItemManagerEngineFactory::supportedImplementationVersions() const
{
    return QList<int>();
}

QTM_END_NAMESPACE
