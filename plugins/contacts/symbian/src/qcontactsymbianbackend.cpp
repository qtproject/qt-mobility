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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QFileSystemWatcher>
#include <QFile>
#include <QSettings>
#include <QUuid>

#include <QDebug>

#include <qtcontacts.h>

#include "qcontact_p.h"
#include "qcontactgroup_p.h"
#include "qcontactmanager_p.h"

#include "qcontactsymbianbackend.h"
#include "qcontactsymbianengine_p.h" 

QContactSymbianEngine::QContactSymbianEngine(const QMap<QString, QString>& /*parameters*/, QContactManager::Error& error)
{
  error = QContactManager::NoError;
  
  d = new QContactSymbianEngineData();
	
	// Connect database observer events appropriately.
	connect(d, SIGNAL(contactAdded(QUniqueId)), 
			this, SLOT(eventContactAdded(QUniqueId)));
	
	connect(d, SIGNAL(contactRemoved(QUniqueId)), 
			this, SLOT(eventContactRemoved(QUniqueId)));
	
	connect(d, SIGNAL(contactChanged(QUniqueId)), 
			this, SLOT(eventContactChanged(QUniqueId)));
	
	connect(d, SIGNAL(groupAdded(QUniqueId)), 
			this, SLOT(eventGroupAdded(QUniqueId)));
	
	connect(d, SIGNAL(groupRemoved(QUniqueId)), 
			this, SLOT(eventGroupRemoved(QUniqueId)));
	
	connect(d, SIGNAL(groupChanged(QUniqueId)), 
			this, SLOT(eventGroupChanged(QUniqueId)));
}

QContactSymbianEngine::QContactSymbianEngine(const QContactSymbianEngine& other)
    : QContactManagerEngine(), d(other.d)
{

}

QContactSymbianEngine& QContactSymbianEngine::operator=(const QContactSymbianEngine& other)
{
    // assign
    d = other.d;

    return *this;
}

QContactSymbianEngine::~QContactSymbianEngine()
{
}

void QContactSymbianEngine::deref()
{
	//This class is not using a private shared class so should this be always deleted?
	//d->deref();
	
	/*if (!d->m_refCount.deref())
 	delete this;*/ 
}

QList<QUniqueId> QContactSymbianEngine::contacts(const QList<QContactSortOrder>& /*sortOrders*/, QContactManager::Error& /*error*/) const
{
	return d->contacts();
}

QContact QContactSymbianEngine::contact(const QUniqueId& contactId, QContactManager::Error& error) const
{
	return d->contact(contactId, error);
}

bool QContactSymbianEngine::saveContact(QContact* contact, QSet<QUniqueId>& /*contactsAdded*/, QSet<QUniqueId>& /*contactsChanged*/, QSet<QUniqueId>& /*groupsChanged*/, QContactManager::Error& /*error*/)
{
	//TODO: fix this function to use the new parameters and error code
	int error_id = 0;
	
	if (contact->id()) { //save contact
		d->updateContact(*contact);
	}
	else { //create new contact
		d->addContact(*contact, error_id);
	}
	
	return true;
}


bool QContactSymbianEngine::removeContact(const QUniqueId& contactId, QSet<QUniqueId>& /*contactsChanged*/, QSet<QUniqueId>& /*groupsChanged*/, QContactManager::Error& /*error*/)
{
	return d->removeContact(contactId);
}

QList<QUniqueId> QContactSymbianEngine::groups(QContactManager::Error& /*error*/) const
{
	return d->groups();
}

QContactGroup QContactSymbianEngine::group(const QUniqueId& groupId, QContactManager::Error& /*error*/) const
{
	return d->group(groupId);
}

bool QContactSymbianEngine::saveGroup(QContactGroup* group, QSet<QUniqueId>& /*groupsAdded*/, QSet<QUniqueId>& /*groupsChanged*/, QSet<QUniqueId>& /*contactsChanged*/, QContactManager::Error& error)
{
	return d->saveGroup(*group, error);
}

bool QContactSymbianEngine::removeGroup(const QUniqueId& groupId, QSet<QUniqueId>& /*groupsRemoved*/, QSet<QUniqueId>& /*contactsChanged*/, QContactManager::Error& /*error*/)
{
	return d->removeGroup(groupId);
}



QMap<QString, QContactDetailDefinition> QContactSymbianEngine::detailDefinitions(QContactManager::Error& error) const
{
    error = QContactManager::NoError;
    return QContactManagerEngine::schemaDefinitions();
}

bool QContactSymbianEngine::hasFeature(QContactManagerInfo::ManagerFeature feature) const
{
    switch (feature) {
        case QContactManagerInfo::ReadOnlyDetails:
        case QContactManagerInfo::Synchronous:
            return true;
        default:
            return false;
    }
}

/* Synthesise the display label of a contact */
QString QContactSymbianEngine::synthesiseDisplayLabel(const QContact& contact, QContactManager::Error& /*error*/) const
{
    QContactName name = contact.detail<QContactName>();
    QContactOrganisation org = contact.detail<QContactOrganisation>();

    QString firstName = name.first();
    QString lastName = name.last();
    
    if (!name.last().isEmpty()) {
        if (!name.first().isEmpty()) {
            return QString(QLatin1String("%1, %2")).arg(name.last()).arg(name.first());
        } else {
            // Just last
            return name.last();
        }
    } else if (!name.first().isEmpty()) {
        return name.first();
    } else if (!org.displayLabel().isEmpty()) {
        return org.displayLabel();
    } else {
        // XXX grargh.
        return QLatin1String("Unnamed");
    }
}


/*!
 * Returns the list of data types supported by the Symbian S60 engine
 */
QList<QVariant::Type> QContactSymbianEngine::supportedDataTypes() const
{
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    
    return st;
}

/*!
 * Private slot to receive events about added entries.
 * 
 * \param contactId The new contact's ID.
 */
void QContactSymbianEngine::eventContactAdded(const QUniqueId &contactId)
{
	QList<QUniqueId> contactList;
	contactList.append(contactId);
	
	emit contactsAdded(contactList);
}

/*!
 * Private slot to receive events about deleted entries.
 * 
 * \param contactId ID for the deleted contact item.
 */
void QContactSymbianEngine::eventContactRemoved(const QUniqueId &contactId)
{
	QList<QUniqueId> contactList;
	contactList.append(contactId);
		
	emit contactsRemoved(contactList);
}

/*!
 * Private slot to receive events about modified contact items.
 * 
 * \param ID for the contact entry with modified data.
 */
void QContactSymbianEngine::eventContactChanged(const QUniqueId &contactId)
{
	QList<QUniqueId> contactList;
	contactList.append(contactId);
		
	emit contactsChanged(contactList);
}

/*!
 * Private slot to receive events about added groups.
 * 
 * \param groupId The new groups's ID.
 */
void QContactSymbianEngine::eventGroupAdded(const QUniqueId &groupId)
{
	QList<QUniqueId> groupList;
	groupList.append(groupId);
		
	emit groupsAdded(groupList);
}

/*!
 * Private slot to receive events about deleted groups.
 * 
 * \param groupId ID for the deleted contact group.
 */
void QContactSymbianEngine::eventGroupRemoved(const QUniqueId &groupId)
{
	QList<QUniqueId> groupList;
	groupList.append(groupId);
		
	emit groupsRemoved(groupList);
}

/*!
 * Private slot to receive events about modified groups.
 * 
 * \param ID for the group with modified data.
 */
void QContactSymbianEngine::eventGroupChanged(const QUniqueId &groupId)
{
	QList<QUniqueId> groupList;
	groupList.append(groupId);
		
	emit groupsChanged(groupList);
}


/* Factory lives here in the basement */
QContactManagerEngine* QContactSymbianFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    return new QContactSymbianEngine(parameters, error);
}

QString QContactSymbianFactory::managerName() const
{
    return QString("symbian");
}




Q_EXPORT_PLUGIN2(mobapicontactspluginsymbian, QContactSymbianFactory);
