/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactabstractactionfactory.h"

QContactAbstractActionFactory::~QContactAbstractActionFactory()
{
}

uint qHash(const QContactAbstractActionFactory::ActionDescriptor& ad)
{
    return qHash(ad.actionName) + qHash(ad.vendorName) + ad.vendorVersion;
}

/*!
 * \fn QContactAbstractActionFactory::~QContactAbstractActionFactory()
 * Clears any memory in use by this factory
 */

/*!
 * \fn QContactAbstractActionFactory::name()
 * Returns the name of this factory.  The name is used to identify the factory
 * when it is retrieved using the Qt Plugin framework.
 */

/*!
 * \fn QContactAbstractActionFactory::actionNames()
 * Returns a list of the names of the actions of which instances of their implementations are able to be retrieved
 * from this factory.
 */

/*!
 * \fn QContactAbstractActionFactory::instance(const QString& actionName = QString(), const QString& vendor = QString(), int implementationVersion = -1)
 * Returns a pointer to an instance of the implementation of the action whose name is \a actionName, which is provided by the given \a vendor and is of
 * the given \a implementationVersion.  If \a vendor is empty, \a implementationVersion is ignored, and an instance of an arbitrary
 * implementation is returned (if available).  If \a vendor and \a implementationVersion are both supplied, an instance of the implementation
 * matching the criteria is returned (if available).  If no implementation matching the given criteria is managed by this factory,
 * a null pointer is returned.
 */

/*!
 * \fn QContactAbstractActionFactory::instances(const QString& actionName = QString(), const QString& vendor = QString(), int implementationVersion = -1)
 * Returns a list of pointers to instances of the implementations of the actions whose name is \a actionName and is provided by the given \a vendor.
 * If \a actionName is empty but \a vendor is supplied, a list of pointers to instances of all actions implemented by the given \a vendor is returned.
 * If \a vendor is empty, a list of pointers to instances of the actions whose name is \a actionName is returned.
 * If \a actionName and \a vendor are both empty, a list of pointers instances of all implementations managed by this factory is returned.
 */
