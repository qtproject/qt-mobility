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

#include "qcontactmanagerinfo.h"

#include "qcontactmanager_p.h"
#include "qcontactfilter.h"

/*!
 * \deprecated
 * \class QContactManagerInfo
 *
 * \brief The QContactManagerInfo class provides information about a particular QContactManager.
 *
 * This class allows managers to report information about their supported functionality to clients.
 * The supported functionality of a manager includes which data types the backend supports,
 * which definitions are natively filterable in the backend (and thus are high performance),
 * and what capabilities the manager supports.
 *
 * If the manager that provided an information object is subsequently destroyed, the
 * returned QContactManagerInfo object will be invalid.
 *
 * THIS CLASS IS DEPRECATED AND WILL BE REMOVED IN WEEK 47 - see commit SHA1: 948704fe9b83ef164f0c816897f67ef34654e510
 *
 * \sa QContactManager
 */

/*!
 * \enum QContactManagerInfo::ManagerFeature
 * This enum describes the possible features that a particular manager may support
 * \value ChangeLogs The manager supports changelog related operations
 * \value ActionPreferences The manager supports saving preferred details per action per contact
 * \value Groups The manager supports contacts of the \c QContactType::Group type
 * \value Relationships The manager supports at least some types of relationships between contacts
 * \value ArbitraryRelationshipTypes The manager supports relationships of arbitrary types between contacts
 * \value MutableDefinitions The manager supports saving, updating or removing detail definitions.  Some built-in definitions may still be immutable
 * \value SelfContact The manager supports the concept of saving a contact which represents the current user
 * \value Anonymous The manager is isolated from other managers
 */

/*! Frees the memory used by this instance */
QContactManagerInfo::~QContactManagerInfo()
{
}

/*! \internal */
QContactManagerInfo::QContactManagerInfo()
{
    qWarning("This class is deprecated!  Please use QContactManager for reporting instead!  See commit SHA1: 948704fe9b83ef164f0c816897f67ef34654e510");
}

/*!
 * Returns true if the given \a feature is supported by the manager from whom this information object was received.
 */
bool QContactManagerInfo::hasFeature(QContactManagerInfo::ManagerFeature feature, const QString& contactType) const
{
    qWarning("This class is deprecated!  Please use QContactManager for reporting instead!  See commit SHA1: 948704fe9b83ef164f0c816897f67ef34654e510");
    QContactManager::ManagerFeature managerFeature = (QContactManager::ManagerFeature)feature;
    return d->m_engine->hasFeature(managerFeature, contactType);
}

/*!
 * Returns the list of data types supported by the manager
 * associated with this object.
 */
QList<QVariant::Type> QContactManagerInfo::supportedDataTypes() const
{
    qWarning("This class is deprecated!  Please use QContactManager for reporting instead!  See commit SHA1: 948704fe9b83ef164f0c816897f67ef34654e510");
    return d->m_engine->supportedDataTypes();
}

/*!
 * Returns true if the given \a filter is supported natively by the
 * manager, and false if the filter behaviour would be emulated.
 *
 * Note: In some cases, the behaviour of an unsupported filter
 * cannot be emulated.  For example, a filter that requests contacts
 * that have changed since a given time depends on having that information
 * available.  In these cases, the filter will fail.
 */
bool QContactManagerInfo::filterSupported(const QContactFilter& filter) const
{
    qWarning("This class is deprecated!  Please use QContactManager for reporting instead!  See commit SHA1: 948704fe9b83ef164f0c816897f67ef34654e510");
    return d->m_engine->filterSupported(filter);
}

/*!
 * Returns the list of relationship types which are supported by this backend.
 * If the backend does not support the \c QContactManagerInfo::Relationships feature, this list should
 * be empty.  If the backend supports the \c QContactManagerInfo::Relationships feature and also
 * supports the \c QContactManagerInfo::ArbitraryRelationshipTypes feature, the list will
 * contain the natively supported (well-known) relationship types contained in the list, but clients
 * are able to add relationships of any custom type also.
 */
QStringList QContactManagerInfo::supportedRelationshipTypes(const QString& contactType) const
{
    qWarning("This class is deprecated!  Please use QContactManager for reporting instead!  See commit SHA1: 948704fe9b83ef164f0c816897f67ef34654e510");
    return d->m_engine->supportedRelationshipTypes(contactType);
}
