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

#include "qcontactactiondescriptor.h"
#include "qcontactactiondescriptor_p.h"
#include <QHash>

QTM_BEGIN_NAMESPACE

/*!
  \class QContactActionDescriptor
  \brief The QContactActionDescriptor class provides information that
  uniquely identifies a specific implementation of an action
  \ingroup contacts-actions
  \inmodule QtContacts
*/

/*!
 * Constructs a new action descriptor for the implementation of the action identified by the given \a actionName
 * of the given implementation \a vendorVersion, as implemented by the vendor identified by the given \a vendorName
 */
QContactActionDescriptor::QContactActionDescriptor(const QString& actionName, const QString& vendorName, int vendorVersion)
        : d(new QContactActionDescriptorPrivate(actionName, vendorName, vendorVersion))
{
}

/*!
 * Constructs a copy of the \a other action descriptor
 */
QContactActionDescriptor::QContactActionDescriptor(const QContactActionDescriptor& other)
        : d(other.d)
{
}

/*!
 * Assigns this action descriptor to be equal to \a other
 */
QContactActionDescriptor& QContactActionDescriptor::operator=(const QContactActionDescriptor& other)
{
    d = other.d;
    return *this;
}

/*!
 * Cleans up any memory in use by the action descriptor
 */
QContactActionDescriptor::~QContactActionDescriptor()
{
}

/*!
 * Sets the name of the action identified by this action descriptor to \a actionName
 */
void QContactActionDescriptor::setActionName(const QString& actionName)
{
    d->m_actionName = actionName;
}

/*!
 * Sets the name of the vendor of the action implementation identified by this action descriptor to \a vendorName
 */
void QContactActionDescriptor::setVendorName(const QString& vendorName)
{
    d->m_vendorName = vendorName;
}

/*!
 * Sets the vendor-specified implementation version of the action implementation identified by this action descriptor to \a implementationVersion
 */
void QContactActionDescriptor::setImplementationVersion(int implementationVersion)
{
    d->m_implementationVersion = implementationVersion;
}

/*!
 * Returns the name of the action which is identified by the action descriptor
 */
QString QContactActionDescriptor::actionName() const
{
    return d->m_actionName;
}

/*!
 * Returns the name of the vendor of the action implementation which is identified by the action descriptor
 */
QString QContactActionDescriptor::vendorName() const
{
    return d->m_vendorName;
}

/*!
 * Returns the vendor-specified version of the action implementation which is identified by the action descriptor
 */
int QContactActionDescriptor::implementationVersion() const
{
    return d->m_implementationVersion;
}

/*!
 * Returns true if either the name, vendor and version of the descriptor are missing from the descriptor.
 * An empty descriptor cannot uniquely identify an action.
 */
bool QContactActionDescriptor::isEmpty() const
{
    if (d->m_actionName.isEmpty())
        return true;
    if (d->m_vendorName.isEmpty())
        return true;
    if (d->m_implementationVersion <= 0)
        return true;
    return false;
}

/*!
 * Returns true if the action name, vendor name and vendor-specified implementation version
 * specified by this action descriptor are equal to those specified by \a other
 */
bool QContactActionDescriptor::operator==(const QContactActionDescriptor& other) const
{
    return d->m_actionName == other.d->m_actionName
            && d->m_vendorName == other.d->m_vendorName
            && d->m_implementationVersion == other.d->m_implementationVersion;
}

/*!
 * Returns true if the action name, vendor name or vendor-specified implementation version
 * specified by this action descriptor are different to that specified by \a other
 */
bool QContactActionDescriptor::operator!=(const QContactActionDescriptor& other) const
{
    return !(*this == other);
}

/*!
 * Returns true if the action descriptor is less than the \a other action descriptor.  The
 * comparison is performed first on the vendor name, then the action name, then the implementation
 * version.
 */
bool QContactActionDescriptor::operator<(const QContactActionDescriptor& other) const
{
    int comp = d->m_vendorName.compare(other.d->m_vendorName);
    if (comp != 0)
        return comp < 0;
    comp = d->m_actionName.compare(other.d->m_actionName);
    if (comp != 0)
        return comp < 0;
    return d->m_implementationVersion < other.d->m_implementationVersion;
}

/*! Returns the hash value for \a key. */
uint qHash(const QContactActionDescriptor& key)
{
    return QT_PREPEND_NAMESPACE(qHash)(key.vendorName())
            + QT_PREPEND_NAMESPACE(qHash)(key.actionName())
            + QT_PREPEND_NAMESPACE(qHash)(key.implementationVersion());
}

QTM_END_NAMESPACE
