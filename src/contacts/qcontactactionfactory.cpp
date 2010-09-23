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

#include "qcontactactionfactory.h"
#include "qcontactactiondescriptor_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactActionFactory
  \brief The QContactActionFactory class provides an interface for clients
  to retrieve instances of action implementations
  \inmodule QtContacts
  \ingroup contacts-actions
 */

QContactActionFactory::~QContactActionFactory()
{
}

/*!
  \fn QContactActionFactory::~QContactActionFactory()
  Clears any memory in use by this factory
 */

/*!
  \fn QContactActionFactory::supportedTargets(const QContact& contact, const QContactActionDescriptor& which) const
  Returns the targets which are supported by the action which may be instantiated by this factory
  for the given \a contact.  If there are no supported targets for the \a contact, then that
  contact is not supported by the action.
  \sa supportsContact()
 */

/*!
  \fn QContactActionFactory::supportsContact(const QContact& contact, const QContactActionDescriptor& which) const
  Returns true if the list of supported targets for the given \a contact is not empty.
 */

/*!
  \fn QContactActionFactory::metaData(const QString& key, const QList<QContactActionTarget>& targets, const QVariantMap& parameters = QVariantMap(), const QContactActionDescriptor& which) const
  Returns the meta-data associated with the action which this factory generates, for the given \a key (such as icon, label or sound cues).
  The meta-data may vary depending on the \a targets of the action and any \a parameters to invocation which the client may specify.
 */

/*!
  \variable QContactActionFactory::InterfaceName
  The name of the interface that action plugins should implement.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactActionFactory::InterfaceName, "com.nokia.qt.mobility.contacts.action");

bool QContactActionFactory::supportsContact(const QContact& contact, const QContactActionDescriptor& which) const
{
    // default implementation is naive.
    return !supportedTargets(contact, which).isEmpty();
}

QContactActionDescriptor QContactActionFactory::createDescriptor(const QString& actionName, const QString& serviceName, const QString& actionIdentifier, int implementationVersion) const
{
    QContactActionDescriptor retn;
    retn.d->m_actionName = actionName;
    retn.d->m_serviceName = serviceName;
    retn.d->m_identifier = actionIdentifier;
    retn.d->m_implementationVersion = implementationVersion;
    retn.d->m_factory = this;
    return retn;
}

#include "moc_qcontactactionfactory.cpp"

QTM_END_NAMESPACE
