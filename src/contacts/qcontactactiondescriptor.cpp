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
#include "qcontactactionfactory.h"
#include "qcontact.h"
#include "qcontactinvalidfilter.h"

#include <QHash>

QTM_BEGIN_NAMESPACE

/*!
  \class QContactActionDescriptor
  \brief The QContactActionDescriptor class provides information that
  uniquely identifies a specific implementation of an action
  \ingroup contacts-actions
*/

/*!
 * Constructs a new, invalid action descriptor
 */
QContactActionDescriptor::QContactActionDescriptor()
        : d(new QContactActionDescriptorPrivate(QString(), QString(), -1, 0))
{
}

/*!
 * Constructs a new action descriptor for the implementation of the action identified by the given \a actionName
 * of the given implementation \a implementationVersion, as implemented by the service identified by the given \a serviceName
 */
QContactActionDescriptor::QContactActionDescriptor(const QString& actionName, const QString& serviceName, int implementationVersion, QContactActionFactory* factory)
        : d(new QContactActionDescriptorPrivate(actionName, serviceName, implementationVersion, factory))
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
 * Returns the name of the action which is identified by the action descriptor
 */
QString QContactActionDescriptor::actionName() const
{
    return d->m_actionName;
}

/*!
 * Returns the name of the service of the action implementation which is identified by the action descriptor
 */
QString QContactActionDescriptor::serviceName() const
{
    return d->m_serviceName;
}

/*!
 * Returns the service-specified version of the action implementation which is identified by the action descriptor
 */
int QContactActionDescriptor::implementationVersion() const
{
    return d->m_implementationVersion;
}

QSet<QContactActionTarget> QContactActionDescriptor::supportedTargets(const QContact& contact) const
{
    if (d->m_factory) {
        return d->m_factory->supportedTargets(contact);
    }

    return QSet<QContactActionTarget>();
}
QContactFilter QContactActionDescriptor::contactFilter() const
{
    if (d->m_factory) {
        return d->m_factory->contactFilter();
    }

    return QContactInvalidFilter();
}
QVariant QContactActionDescriptor::metaData(const QString& key, const QList<QContactActionTarget>& targets, const QVariantMap& parameters) const
{
    if (d->m_factory) {
        return d->m_factory->metaData(key, targets, parameters);
    }

    return QVariant();
}
bool QContactActionDescriptor::supportsContact(const QContact& contact) const
{
    if (d->m_factory) {
        return d->m_factory->supportsContact(contact);
    }

    return false;
}

/*!
 * Returns false if either the name, service and version of the descriptor are missing from the descriptor,
 * or if the descriptor is not associated with a valid action factory which can create instances of an action.
 * An empty descriptor cannot uniquely identify an action.
 */
bool QContactActionDescriptor::isValid() const
{
    if (d->m_actionName.isEmpty())
        return false;
    if (d->m_serviceName.isEmpty())
        return false;
    if (d->m_implementationVersion <= 0)
        return false;
    if (d->m_factory == 0)
        return false;
    return true;
}

/*!
 * Returns true if the action name, service name and service-specified implementation version
 * specified by this action descriptor are equal to those specified by \a other
 */
bool QContactActionDescriptor::operator==(const QContactActionDescriptor& other) const
{
    return d->m_actionName == other.d->m_actionName
            && d->m_serviceName == other.d->m_serviceName
            && d->m_implementationVersion == other.d->m_implementationVersion;
}

/*!
 * Returns true if the action name, service name or service-specified implementation version
 * specified by this action descriptor are different to that specified by \a other
 */
bool QContactActionDescriptor::operator!=(const QContactActionDescriptor& other) const
{
    return !(*this == other);
}

/*!
 * Returns true if the action descriptor is less than the \a other action descriptor.  The
 * comparison is performed first on the service name, then the action name, then the implementation
 * version.
 */
bool QContactActionDescriptor::operator<(const QContactActionDescriptor& other) const
{
    int comp = d->m_serviceName.compare(other.d->m_serviceName);
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
    return QT_PREPEND_NAMESPACE(qHash)(key.serviceName())
            + QT_PREPEND_NAMESPACE(qHash)(key.actionName())
            + QT_PREPEND_NAMESPACE(qHash)(key.implementationVersion());
}

QTM_END_NAMESPACE
