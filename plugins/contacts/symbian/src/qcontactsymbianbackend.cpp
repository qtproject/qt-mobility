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
#include "qcontactchangeset.h"

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

/*!
 * Returns a list of the ids of contacts that match the supplied \a filter, sorted according to the given \a sortOrders.
 * Any error that occurs will be stored in \a error. Uses either the Symbian backend native filtering or in case of an
 * unsupported filter, the generic (slow) filtering of QContactManagerEngine.
 */
QList<QUniqueId> QContactSymbianEngine::contacts(
        const QContactFilter& filter,
        const QList<QContactSortOrder>& sortOrders,
        QContactManager::Error& error) const
{
    QList<QUniqueId> result;

    // Check if the filter is supported by the underlying filter implementation
    QAbstractContactFilter::FilterSupport filterSupport = d->filterSupported(filter);

    if (filterSupport == QAbstractContactFilter::Supported) {
        // Filter supported, use as the result directly
        result = d->contacts(filter, sortOrders, error);
    } else if (filterSupport == QAbstractContactFilter::SupportedPreFilterOnly) {
        // Filter only does pre-filtering and may include false positives
        QList<QUniqueId> contacts = d->contacts(filter, sortOrders, error);
        if(error == QContactManager::NoError)
            slowFilter(filter, contacts, result, error);
    } else {
        // Filter not supported; fetch all contacts and remove false positives
        // one-by-one
        QList<QUniqueId> contacts = d->contacts(error);
        if(error == QContactManager::NoError)
            slowFilter(filter, contacts, result, error);
    }
    return result;
}

QList<QUniqueId> QContactSymbianEngine::contacts(const QList<QContactSortOrder>& /*sortOrders*/, QContactManager::Error& error) const
{
	return d->contacts(error);
}

QContact QContactSymbianEngine::contact(const QUniqueId& contactId, QContactManager::Error& error) const
{
    QContact contact = d->contact(contactId, error);

    // Synthesize display label (the label it is not saved to the contact
    // database and thus not modifiable by a client).
    if(error == QContactManager::NoError)
        updateDisplayLabel(contact);

    return contact;
}

bool QContactSymbianEngine::saveContact(QContact* contact, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    TBool ret = doSaveContact(contact, changeSet, error);
    changeSet.emitSignals(this);
    return ret;
}

QList<QContactManager::Error> QContactSymbianEngine::saveContacts(QList<QContact>* contacts, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    QList<QContactManager::Error> ret;
    if (!contacts) {
        error = QContactManager::BadArgumentError;
    } else {
        QContactManager::Error functionError = QContactManager::NoError;
        for (int i = 0; i < contacts->count(); i++) {
            QContact current = contacts->at(i);
            if (!doSaveContact(&current, changeSet, error)) {
                functionError = error;
                ret.append(functionError);
            } else {
                (*contacts)[i] = current;
                ret.append(QContactManager::NoError);
            }
        }
        error = functionError;
    }
    changeSet.emitSignals(this);
    return ret;
}

void QContactSymbianEngine::slowFilter(
        const QContactFilter& filter,
        const QList<QUniqueId>& contacts,
        QList<QUniqueId>& result,
        QContactManager::Error& error
        ) const
{
    for (int i(0); i < contacts.count(); i++) {
        QUniqueId contactid = contacts.at(i);
        // Check if this is a false positive. If not, add to the result set.
        if(QContactManagerEngine::testFilter(filter, d->contact(contactid, error)))
            result << contactid;
    }
}
bool QContactSymbianEngine::doSaveContact(QContact* contact, QContactChangeSet& changeSet, QContactManager::Error& error)
{
    bool ret = false;
    if (contact->id()) { //save contact
        ret = d->updateContact(*contact, changeSet, error);
        if (ret)
            updateDisplayLabel(*contact);
    }
    else { //create new contact
        ret = d->addContact(*contact, changeSet, error);
        if (ret) {
            ASSERT(contact->id());
            updateDisplayLabel(*contact);
        }
    }
    return ret;
}

void QContactSymbianEngine::updateDisplayLabel(QContact& contact) const
{
    QContactManager::Error error(QContactManager::NoError);
    QContactDisplayLabel label = contact.displayLabel();
    QString labelString = synthesiseDisplayLabel(contact, error);
    if(error == QContactManager::NoError) {
        label.setLabel(labelString);
        label.setSynthesised(true);
        contact.setDisplayLabel(label);
    }
}

bool QContactSymbianEngine::removeContact(const QUniqueId& contactId, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    TBool ret = d->removeContact(contactId, changeSet, error);
    changeSet.emitSignals(this);
    return ret;
}

QList<QContactManager::Error> QContactSymbianEngine::removeContacts(QList<QUniqueId>* contactIds, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    QList<QContactManager::Error> ret;
    if (!contactIds) {
        error = QContactManager::BadArgumentError;
        return ret;
    } else {
        QList<QUniqueId> removedList;
        QContactManager::Error functionError = QContactManager::NoError;
        for (int i = 0; i < contactIds->count(); i++) {
            QUniqueId current = contactIds->at(i);
            if (!d->removeContact(current, changeSet, error)) {
                functionError = error;
                ret.append(functionError);
            } else {
                (*contactIds)[i] = 0;
                ret.append(QContactManager::NoError);
            }
        }

        error = functionError;
    }
    changeSet.emitSignals(this);
    return ret;
}

QList<QUniqueId> QContactSymbianEngine::groups(QContactManager::Error& error) const
{
	return d->groups(error);
}

QContactGroup QContactSymbianEngine::group(const QUniqueId& groupId, QContactManager::Error& error) const
{
	return d->group(groupId, error);
}

bool QContactSymbianEngine::saveGroup(QContactGroup* group, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    bool ret = false;

    if(group)
    {
        if (group->id() > 0) {
            ret = d->updateGroup(*group, changeSet, error);
        }
        else {
            ret = d->addGroup(*group, changeSet, error);
        }
    }
    else
    {
        error = QContactManager::BadArgumentError;
    }
    changeSet.emitSignals(this);
    return ret;
}

bool QContactSymbianEngine::removeGroup(const QUniqueId& groupId, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    bool ret = d->removeGroup(groupId, changeSet, error);
    changeSet.emitSignals(this);
    return ret;
}

QMap<QString, QContactDetailDefinition> QContactSymbianEngine::detailDefinitions(QContactManager::Error& error) const
{
    error = QContactManager::NoError;

    // get default constraints schema
    QMap<QString, QContactDetailDefinition> defMap = QContactManagerEngine::schemaDefinitions();

    // update default constraints
    defMap[QContactName::DefinitionName].setUnique(true);
    defMap[QContactNickname::DefinitionName].setUnique(true);
    return defMap;
}

bool QContactSymbianEngine::hasFeature(QContactManagerInfo::ManagerFeature feature) const
{
    switch (feature) {
        /* TODO: case QContactManagerInfo::Groups to be implemented.
           How about the others? like:
           QContactManagerInfo::ActionPreferences,
           QContactManagerInfo::MutableDefinitions,
           QContactManagerInfo::Anonymous? */
    case QContactManagerInfo::Groups:
        return true;
    default:
        return false;
    }
}

bool QContactSymbianEngine::filterSupported(const QContactFilter& filter) const
{
    TBool result;

    // Map filter support into a boolean value
    QAbstractContactFilter::FilterSupport filterSupport = d->filterSupported(filter);
    if (filterSupport == QAbstractContactFilter::Supported
        || filterSupport == QAbstractContactFilter::SupportedPreFilterOnly) {
        result = true;
    } else {
        result = false;
    }

    return result;
}

/* Synthesise the display label of a contact */
QString QContactSymbianEngine::synthesiseDisplayLabel(const QContact& contact, QContactManager::Error& /*error*/) const
{
    QContactName name = contact.detail<QContactName>();
    QContactOrganization org = contact.detail<QContactOrganization>();

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
    } else if (!org.name().isEmpty()) {
        return org.name();
    } else {
        // XXX grargh.
        return QLatin1String("Unnamed");
    }
}

bool QContactSymbianEngine::setSelfContactId(const QUniqueId& contactId, QContactManager::Error& error)
{
    return d->setSelfContactId(contactId, error);
}

QUniqueId QContactSymbianEngine::selfContactId(QContactManager::Error& error) const
{
    return d->selfContactId(error);
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
