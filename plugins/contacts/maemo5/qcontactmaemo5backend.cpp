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

#include "qcontactmaemo5backend_p.h"

#include <QSharedData>

#include "qcontactmaemo5debug_p.h"

DEFINE_GLOBAL_DEBUG_VAR

QContactManagerEngine* ContactMaemo5Factory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);

    initDebugLogger();
    return new QContactMaemo5Engine(); //FIXME Wonderfull memory leak :D
}

QString ContactMaemo5Factory::managerName() const
{
    return QString("maemo5");
}

Q_EXPORT_PLUGIN2(qtcontacts_maemo5, ContactMaemo5Factory);

/*!
  \class QContactMaemo5Engine
  \brief The QContactMaemo5Engine class provides an implementation of
  QContactManagerEngine whose functions always return an error.
  
  The Maemo5 engine.
 */

/*! Constructs a new invalid contacts backend. */
QContactMaemo5Engine::QContactMaemo5Engine() : d(new QContactMaemo5EngineData)
{
}

/*! \reimp */
QContactMaemo5Engine& QContactMaemo5Engine::operator=(const QContactMaemo5Engine& other)
{
    d = other.d;
    return *this;
}

/*! \reimp */
void QContactMaemo5Engine::deref()
{
    QCM5_DEBUG << __PRETTY_FUNCTION__;
    if (!d->m_refCount.deref())
        delete this;
}

/*! \reimp */
QString QContactMaemo5Engine::managerName() const
{
    return QString(QLatin1String("maemo5"));
}

/* Synthesise the display label of a contact */
QString QContactMaemo5Engine::synthesizedDisplayLabel(const QContact& contact, QContactManager::Error& error) const
{
  Q_UNUSED(error)
  QString label = QContactManagerEngine::synthesizedDisplayLabel(contact, error);
  
  if (label.isEmpty()) {
    QContactNickname n = contact.detail(QContactNickname::DefinitionName);
    label = n.nickname();
  }
  
  if (label.isEmpty())
    label = "No name";
  
  return label;
}

QList<QContactLocalId> QContactMaemo5Engine::contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
  Q_CHECK_PTR(d->m_abook);
  
  //return QContactManagerEngine::contactIds(filter, sortOrders, error);
  return d->m_abook->contactIds(filter, sortOrders, error);
}

QContact QContactMaemo5Engine::contact(const QContactLocalId& contactId, const QStringList& definitionRestrictions, QContactManager::Error& error) const
{
  Q_UNUSED(definitionRestrictions); //TODO
  Q_CHECK_PTR(d->m_abook);
  
  QContact *contact = d->m_abook->getQContact(contactId, error);
  
  QString displayLabel = synthesizedDisplayLabel(*contact, error);
  
  QContact rtn = setContactDisplayLabel(displayLabel, *contact);
  delete contact;
  
  return rtn;
}

bool QContactMaemo5Engine::saveContact(QContact* contact, QContactManager::Error& error)
{
  Q_CHECK_PTR(d->m_abook);
  
  if (!contact) {
    error = QContactManager::BadArgumentError;
    return false;
  }
  
  // ensure that the contact's details conform to their definitions
  if (!validateContact(*contact, error)) {
    QCM5_DEBUG << "Validate Contact failed";
    error = QContactManager::InvalidDetailError;
    return false;
  }
  return d->m_abook->saveContact(contact, error);
}

#if 0
QList<QContactManager::Error> QContactMaemo5Engine::removeContacts(QList<QContactLocalId>* contactIds, QContactManager::Error& error)
{
  bool ok = true;
  
  if (contactIds->isEmpty())
    return false;
  
  QContactLocalId id;
  foreach(id, contactIds){
      if (!removeContact(id, error))
	ok = false;
  }
  
  return ok;
}
#endif

bool QContactMaemo5Engine::removeContact(const QContactLocalId& contactId, QContactManager::Error& error)
{
  Q_CHECK_PTR(d->m_abook);
  return d->m_abook->removeContact(contactId, error);
}

QMap<QString, QContactDetailDefinition> QContactMaemo5Engine::detailDefinitions(const QString& contactType, QContactManager::Error& error) const
{
    QMap<QString, QMap<QString, QContactDetailDefinition> > defns;
    QMap<QString, QContactDetailFieldDefinition> fields;
    
    
    // Remove unsupported definitions
    defns = QContactManagerEngine::schemaDefinitions();
    defns[contactType].remove(QContactAnniversary::DefinitionName);
    defns[contactType].remove(QContactGeoLocation::DefinitionName);
    defns[contactType].remove(QContactSyncTarget::DefinitionName);
    // QContactTimestamp is Read ONLY
    
    //TODO Remove unsupported fields
    
    fields = defns[contactType][QContactAddress::DefinitionName].fields();
    //fields.remove(QContactAddress::FieldSubTypes);
    QContactDetailFieldDefinition dfd;
    dfd.setDataType(QVariant::String);
    fields.insert("Estension", dfd);
    defns[contactType][QContactAddress::DefinitionName].setFields(fields);
    
    //"Estension");
    
    //fields = defns[contactType][QContactAnniversary::DefinitionName].fields()
    //fields.remove(QContactAnniversary::FieldCalendarId);

    fields = defns[contactType][QContactOnlineAccount::DefinitionName].fields();
    fields.remove(QContactOnlineAccount::FieldAccountUri);
    fields.remove(QContactOnlineAccount::FieldSubTypes);
    defns[contactType][QContactOnlineAccount::DefinitionName].setFields(fields);
      
    fields = defns[contactType][QContactOrganization::DefinitionName].fields();
    fields.remove(QContactOrganization::FieldAssistantName);
    fields.remove(QContactOrganization::FieldDepartment);
    fields.remove(QContactOrganization::FieldLocation);
    fields.remove(QContactOrganization::FieldLogo);
    fields.remove(QContactOrganization::FieldName);
    fields.remove(QContactOrganization::FieldRole);
    defns[contactType][QContactOrganization::DefinitionName].setFields(fields);
    
    fields = defns[contactType][QContactUrl::DefinitionName].fields();
    fields.remove(QContactUrl::FieldSubType);
    defns[contactType][QContactUrl::DefinitionName].setFields(fields);
    
    error = QContactManager::NoError;
    return defns[contactType];
}
