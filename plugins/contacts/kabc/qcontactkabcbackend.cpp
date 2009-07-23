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
#include "qcontactkabcbackend_p.h"

#include <QFileSystemWatcher>
#include <QFile>
#include <QSettings>
#include <QUuid>

#include <QDebug>

#include "qcontact_p.h"
#include "qcontactgroup_p.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"

#include <kabc/resource.h>
#include <kabc/addressee.h>
#include <kabc/addresseelist.h>
#include <kabc/addressbook.h>
#include <kabc/stdaddressbook.h>

QContactManagerEngine* ContactKabcFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    return new QContactKabcEngine(parameters, error);
}

QString ContactKabcFactory::managerName() const
{
    return QString("kabc");
}
Q_EXPORT_PLUGIN2(contacts_kabc, ContactKabcFactory);

QContactKabcEngine::QContactKabcEngine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
    : d(new QContactKabcEngineData)
{
    // we load the addressbook from the given resource config
    d->m_resourceFile = parameters.value("resource");
    if (d->m_resourceFile.isNull() || d->m_resourceFile.isEmpty()) {
        d->ab = KABC::StdAddressBook::self();
        d->m_standardAddressbook = true;
    } else {
        d->ab = new KABC::AddressBook(d->m_resourceFile);
        d->m_standardAddressbook = false;
    }

    // load our backend state settings file
    // which contains our detail definitions, last used id, etc.
    d->m_settingsFile = parameters.value("settings");
    if (d->m_settingsFile.isNull() || d->m_settingsFile.isEmpty())
        d->m_settingsFile = "kabcbackend.ini";

    // and trigger a settings update
    settingsFileChanged();
    error = QContactManager::NoError;
}

QContactKabcEngine::QContactKabcEngine(const QContactKabcEngine& other)
    : QContactManagerEngine(), d(other.d)
{
    // but, since we can't share a settings watcher, we must set our ptr to 0
    d->m_settingsWatcher = 0;

    // then, we load the addressbook from the given resource config
    if (d->m_resourceFile.isNull() || d->m_resourceFile.isEmpty()) {
        d->ab = KABC::StdAddressBook::self();
        d->m_standardAddressbook = true;
    } else {
        d->ab = new KABC::AddressBook(d->m_resourceFile);
        d->m_standardAddressbook = false;
    }

    // and trigger a settings update
    settingsFileChanged();
}

QContactKabcEngine& QContactKabcEngine::operator=(const QContactKabcEngine& other)
{
    // cleanup
    if (d->ab && !d->m_standardAddressbook)
        delete d->ab;
    if (d->m_settingsWatcher)
        delete d->m_settingsWatcher;

    // assign
    d = other.d;

    // but, since we can't share a settings watcher, we must set our ptr to 0
    d->m_settingsWatcher = 0;

    // then, we load the addressbook from the given resource config
    if (d->m_resourceFile.isNull() || d->m_resourceFile.isEmpty()) {
        d->ab = KABC::StdAddressBook::self();
        d->m_standardAddressbook = true;
    } else {
        d->ab = new KABC::AddressBook(d->m_resourceFile);
        d->m_standardAddressbook = false;
    }

    // and trigger a settings update
    settingsFileChanged();

    return *this;
}

QContactKabcEngine::~QContactKabcEngine()
{
}

void QContactKabcEngine::deref()
{
    if (!d->m_refCount.deref())
        delete this;
}

void QContactKabcEngine::settingsFileChanged()
{
    // we save our definitions and last used id to this file.
    // TODO: use SQLite to solve concurrency/locking problems inherent.

    // discover whether or not the settings file already exists, or
    // whether we are going to create it.
    bool settingsExists = QFile::exists(d->m_settingsFile);

    // if the settings file has not yet been created, then we have no definitions
    // to load; we need to retrieve the schema definitions, modify them, and save them.
    if (!settingsExists) {
        // set our last used id if we need to.
        if (d->m_lastUsedId == 0)
            d->m_lastUsedId = 1;

        // lazy initialisation of schema definitions; modify them to match our capabilities.
        d->m_definitions = QContactManagerEngine::schemaDefinitions();

        // modification: avatar is unique.
        QContactDetailDefinition avatarDef = d->m_definitions.value(QContactAvatar::DefinitionName);
        avatarDef.setUnique(true);
        d->m_definitions.insert(QContactAvatar::DefinitionName, avatarDef);

        // modification: url is unique.
        QContactDetailDefinition urlDef = d->m_definitions.value(QContactUrl::DefinitionName);
        urlDef.setUnique(true);
        d->m_definitions.insert(QContactUrl::DefinitionName, urlDef);

        // now save each of our definitions
        QContactManager::Error error; // dummy only; if it fails we ignore.
        foreach (const QContactDetailDefinition& def, d->m_definitions.values()) {
            saveDetailDefinition(def, error);
        }
    } else {
        // load the next available contact id from the file.
        bool ok;
        QSettings definitions(d->m_settingsFile, QSettings::IniFormat);
        d->m_lastUsedId = definitions.value("nextAvailableContactId", "1").toUInt(&ok) - 1;

        QMap<QString, QContactDetailDefinition> tempDefinitions;
        
        // load the schema definitions from the settings file.
        QStringList defIds = definitions.childGroups();
        foreach (const QString& defId, defIds) {
            definitions.beginGroup(defId);
            QContactDetailDefinition dd;
            dd.setId(defId);
            dd.setUnique(definitions.value("unique").toBool());
            dd.setAccessConstraint(((QContactDetailDefinition::AccessConstraint)definitions.value("access").toUInt()));

            // now add the fields
            QMap<QString, QContactDetailDefinition::Field> allFields;
            definitions.beginGroup("fields");
            QStringList fieldKeys = definitions.childGroups();
            foreach (const QString& field, fieldKeys) {
                definitions.beginGroup(field);
                QContactDetailDefinition::Field f;
                f.dataType = (QVariant::Type)(definitions.value("type").toInt());
                if (definitions.value("allowable").isValid())
                    f.allowableValues = definitions.value("allowable").value<QList<QVariant> >();

                allFields.insert(field, f);
                definitions.endGroup();
            }

            dd.setFields(allFields);
            tempDefinitions.insert(defId, dd);
            definitions.endGroup();
            definitions.endGroup();
        }

        // having loaded all of the definitions, set our m_defs
        d->m_definitions = tempDefinitions;
    }


    if (!d->m_settingsWatcher) {
        // create new watcher for the settings file.
        d->m_settingsWatcher = new QFileSystemWatcher(QStringList(d->m_settingsFile));
        connect(d->m_settingsWatcher, SIGNAL(fileChanged(QString)), this, SLOT(settingsFileChanged()));
    }
}

QList<QUniqueId> QContactKabcEngine::contacts(QContactManager::Error& error) const
{
    QList<QUniqueId> allCIds;
    error = QContactManager::NoError;
    KABC::Addressee::List allAddressees = d->ab->allAddressees();
    foreach (const KABC::Addressee& curr, allAddressees) {
        allCIds.append(getIdOfAddressee(curr, error));
    }

    if (allCIds.count() > 0 && error == QContactManager::NoError)
        error = QContactManager::DoesNotExistError;
    return allCIds;
}

/*
QList<QUniqueId> QContactKabcEngine::contactsWithDetail(const QString& definitionName, const QVariant& value, QContactManager::Error& error) const
{
    QList<QUniqueId> retn;
    error = QContactManager::NoError;

    // firstly, use KABC's native API
    if (definitionName == "EmailAddress") {
        QList<KABC::Addressee> matches = d->ab->findByEmail(value.toString());
        foreach (const KABC::Addressee& a, matches)
            retn.append(getIdOfAddressee(a, error));
        if (retn.isEmpty() && error == QContactManager::NoError)
            error = QContactManager::DoesNotExistError;
        return retn;
    } else if (definitionName == "Name") {
        QList<KABC::Addressee> matches = d->ab->findByName(value.toString());
        foreach (const KABC::Addressee& a, matches)
            retn.append(getIdOfAddressee(a, error));
        if (retn.isEmpty() && error == QContactManager::NoError)
            error = QContactManager::DoesNotExistError;
        return retn;
    }

    // if that doesn't work, use our default (slow) implementation
    return QContactManagerEngine::contactsWithDetail(definitionName, value, error);
}
*/

QContact QContactKabcEngine::contact(const QUniqueId& contactId, QContactManager::Error& error) const
{
    error = QContactManager::NoError;
    if (d->m_QUniqueIdToKabcUid.contains(contactId)) {
        KABC::Addressee corresponding = d->ab->findByUid(d->m_QUniqueIdToKabcUid.value(contactId));
        QContact ret = convertAddressee(corresponding);
        ret.setId(contactId);
        return ret;
    }

    error = QContactManager::DoesNotExistError;
    return QContact();
}

bool QContactKabcEngine::saveContact(QContact* contact, bool batch, QContactManager::Error& error)
{
    if (contact == 0) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    // ensure that the contact's details conform to their definitions
    if (!validateContact(*contact, error)) {
qDebug() << "KABC::saveContact(): invalid detail error (didn't validate)";
        error = QContactManager::InvalidDetailError;
        return false;
    }

    KABC::Ticket *ticket = d->ab->requestSaveTicket();
    KABC::Addressee converted = convertContact(*contact);
    bool newContact = false;
    if (contact->id() == 0) {
        // new contact
        d->m_lastUsedId += 1;
        contact->setId(d->m_lastUsedId);
        converted.insertCustom("com.nokia.mobility.contacts.KAbcBackend", "id", QString::number(d->m_lastUsedId));
        QString newUuid = QUuid::createUuid().toString();
        d->m_QUniqueIdToKabcUid.insert(d->m_lastUsedId, newUuid);
        d->m_kabcUidToQUniqueId.insert(newUuid, contact->id());
        QSettings definitions(d->m_settingsFile, QSettings::IniFormat);
        definitions.setValue("nextAvailableContactId", QString::number(d->m_lastUsedId + 1));
        newContact = true;
    }
    
    converted.setUid(d->m_QUniqueIdToKabcUid.value(contact->id()));
    d->ab->insertAddressee(converted);
    d->ab->save(ticket);

    // success!
    error = QContactManager::NoError;

    // if we need to emit signals (ie, this isn't part of a batch operation)
    // then emit the correct one.
    if (!batch) {
        QList<QUniqueId> emitList;
        emitList.append(contact->id());
        if (newContact) {
            emit contactsAdded(emitList);
        } else {
            emit contactsChanged(emitList);
        }
    }

    return true;
}

bool QContactKabcEngine::removeContact(const QUniqueId& contactId, bool batch, QContactManager::Error& error)
{
    if (!d->m_QUniqueIdToKabcUid.contains(contactId)) {
        error = QContactManager::DoesNotExistError;
        return false;
    }

    KABC::Ticket *ticket = d->ab->requestSaveTicket();
    d->ab->removeAddressee(d->ab->findByUid(d->m_QUniqueIdToKabcUid.value(contactId)));
    d->m_kabcUidToQUniqueId.remove(d->m_QUniqueIdToKabcUid.value(contactId));
    d->m_QUniqueIdToKabcUid.remove(contactId);
    d->ab->save(ticket);
    error = QContactManager::NoError;

    // if we need to emit signals (ie, this isn't part of a batch operation)
    // then emit the correct one.
    if (!batch) {
        QList<QUniqueId> emitList;
        emitList.append(contactId);
        emit contactsRemoved(emitList);
    }

    return true;
}

QList<QContactManager::Error> QContactKabcEngine::saveContacts(QList<QContact>* contacts, QContactManager::Error& error)
{
    QList<QContactManager::Error> ret;
    QContactManager::Error functionError = QContactManager::NoError;
    QList<QUniqueId> addedList;
    QList<QUniqueId> changedList;

    for (int i = 0; i < contacts->count(); i++) {
        QContact current = contacts->at(i);
        int oldId = current.id();

        // perform a save operation as part of the batch operation.
        if (!saveContact(&current, true, error)) {
            functionError = error;
            ret.append(functionError);
        } else {
            // depending on whether or not it was a new contact,
            // add the id to the correct signal emission list.
            if (oldId == 0) {
                addedList.append(current.id());
            } else {
                changedList.append(current.id());
            }

            // this contact was saved successfully.
            ret.append(QContactManager::NoError);
        }

        contacts->insert(i, current);
    }

    error = functionError; // set the last real error for the batch operation.

    // emit the required signals.
    if (!addedList.isEmpty())
        emit contactsAdded(addedList);
    if (!changedList.isEmpty())
        emit contactsChanged(changedList);

    // return the list of errors.
    return ret;
}

QList<QContactManager::Error> QContactKabcEngine::removeContacts(QList<QUniqueId>* contactIds, QContactManager::Error& error)
{
    QList<QContactManager::Error> ret;
    QContactManager::Error functionError = QContactManager::NoError;
    QList<QUniqueId> removedList;

    for (int i = 0; i < contactIds->count(); i++) {
        QUniqueId current = contactIds->at(i);
        // remove the contact as part of a batch operation
        if (!removeContact(current, true, error)) {
            functionError = error;
            ret.append(functionError);
        } else {
            removedList.append(current);
            ret.append(QContactManager::NoError);
        }

        contactIds->insert(i, current);
    }

    error = functionError;
    if (!removedList.isEmpty())
        emit contactsRemoved(removedList);
    return ret;
}

/*
QList<QUniqueId> QContactKabcEngine::groups(QContactManager::Error& error) const
{
    QList<QUniqueId> retn;
    QList<KABC::DistributionList*> allKabcGroups = d->ab->allDistributionLists();
    for (int i = 0; i < allKabcGroups.count(); i++) {
        bool ok = true;
        QUniqueId currId = allKabcGroups.at(i)->identifier().toUInt(&ok);
        if (ok) {
            retn.append(currId);
        } else {
            // nothing.  we simply don't include it in the list.
        }
    }

    error = QContactManager::NoError;
    return retn;
}

QContactGroup QContactKabcEngine::group(const QUniqueId& groupId, QContactManager::Error& error) const
{
    KABC::DistributionList* requested = 0; // = d->ab->findDistributionListByIdentifier(QString::number(groupId)); // doesn't work...
    QList<KABC::DistributionList*> allDL = d->ab->allDistributionLists();
    foreach(KABC::DistributionList* dlp, allDL) {
        if (dlp->identifier() == QString::number(groupId)) {
            requested = dlp;
        }
    }

    if (requested == 0) {
        error = QContactManager::DoesNotExistError;
        return QContactGroup();
    }

    // set the id of the group
    bool ok = true;
    QContactGroup ret;
    ret.setId(requested->identifier().toUInt(&ok));
    if (!ok) {
        error = QContactManager::UnspecifiedError;
        return QContactGroup();
    }

    // set the name of the group
    ret.setName(requested->name());

    // get the contactid of each addressee in the group, and add to group.
    error = QContactManager::NoError;
    foreach (const KABC::DistributionList::Entry& entry, requested->entries())
        ret.addMember(getIdOfAddressee(entry.addressee(), error));

    return ret;
}

bool QContactKabcEngine::saveGroup(QContactGroup* group, QContactManager::Error& error)
{
    if (group == 0 || group->name().isNull() || group->name().isEmpty()) {
        error = QContactManager::BadArgumentError;        
        return false;
    }

    // create the group if required.
    KABC::Ticket* ticket = d->ab->requestSaveTicket();
    KABC::DistributionList* kabcGroup = 0;
    if (group->id() == 0) {
        d->m_lastUsedId += 1;
        group->setId(d->m_lastUsedId);
        kabcGroup = d->ab->createDistributionList(group->name());
        kabcGroup->setIdentifier(QString::number(group->id()));
    } else {
        QList<KABC::DistributionList*> allDL = d->ab->allDistributionLists();
        foreach(KABC::DistributionList* dlp, allDL) {
            if (dlp->identifier() == QString::number(group->id())) {
                kabcGroup = dlp;
            }
        }
    }

    if (kabcGroup == 0) {
        error = QContactManager::UnspecifiedError;
        return false;
    }

    // having found or created the group, we now remove all entries
    QList<KABC::DistributionList::Entry> groupEntries = kabcGroup->entries();
    foreach (const KABC::DistributionList::Entry& entry, groupEntries) {
        kabcGroup->removeEntry(entry.addressee());
    }

    // and set the new details.
    kabcGroup->setName(group->name());
    QList<QUniqueId> members = group->members();
    foreach(QUniqueId id, members) {
        kabcGroup->insertEntry(convertContact(contact(id, error)));
    }

    d->ab->save(ticket);

    error = QContactManager::NoError;
    return true;
}

bool QContactKabcEngine::removeGroup(const QUniqueId& groupId, QContactManager::Error& error)
{
    KABC::DistributionList* requested = 0; //d->ab->findDistributionListByIdentifier(QString::number(groupId));
    QList<KABC::DistributionList*> allDL = d->ab->allDistributionLists();
    foreach(KABC::DistributionList* dlp, allDL) {
        if (dlp->identifier() == QString::number(groupId)) {
            requested = dlp;
            break;
        }
    }

    if (requested == 0) {
        error = QContactManager::DoesNotExistError;
        return false;
    }

    KABC::Ticket* ticket = d->ab->requestSaveTicket(requested->resource());
    d->ab->removeDistributionList(requested);
    d->ab->save(ticket);

    error = QContactManager::NoError;
    return true;
}
*/

QMap<QString, QContactDetailDefinition> QContactKabcEngine::detailDefinitions(QContactManager::Error& error) const
{
    error = QContactManager::NoError;
    return d->m_definitions;
}

QContactDetailDefinition QContactKabcEngine::detailDefinition(const QString& definitionName, QContactManager::Error& error) const
{
    error = QContactManager::DoesNotExistError;
    if (d->m_definitions.contains(definitionName))
        error = QContactManager::NoError;
    return d->m_definitions.value(definitionName);
}

bool QContactKabcEngine::saveDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error)
{
    if (!d->m_definitions.key(def, "").isEmpty())
        removeDetailDefinition(def, error);
    d->m_definitions.insert(def.id(), def);

    // we need to persist definitions.
    if (d->m_settingsWatcher)
        d->m_settingsWatcher->blockSignals(true);
    QSettings definitions(d->m_settingsFile, QSettings::IniFormat);
    if (definitions.childGroups().contains(def.id()))
        definitions.remove(def.id());

    definitions.beginGroup(def.id());
    definitions.setValue("unique", def.isUnique());
    definitions.setValue("access", def.accessConstraint());
    definitions.beginGroup("fields");
    foreach (const QString& field, def.fields().keys()) {
        definitions.beginGroup(field);
        QContactDetailDefinition::Field f = def.fields().value(field);
        definitions.setValue("type", (int)(f.dataType));
        if (!f.allowableValues.isEmpty())
            definitions.setValue("allowable", QVariant(f.allowableValues));
        definitions.endGroup();
    }
    definitions.endGroup();
    definitions.endGroup();

    if (d->m_settingsWatcher)
        d->m_settingsWatcher->blockSignals(false);

    error = QContactManager::NoError;
    return true;
}

bool QContactKabcEngine::removeDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error)
{
    error = QContactManager::DoesNotExistError;
    bool success = (d->m_definitions.remove(def.id()));

    if (success) {
        // we need to persist the removal.
        QSettings definitions(d->m_settingsFile, QSettings::IniFormat);
        definitions.remove(def.id());
        error = QContactManager::NoError;
    }

    return success;
}

/*!
 * Returns a list of definition identifiers which are natively (fast) filterable
 * on the default backend store managed by the manager from which the capabilities object was accessed
 */
QStringList QContactKabcEngine::fastFilterableDefinitions() const
{
    QStringList fastlist;
    fastlist << "Name::First" << "Name::Last" << "PhoneNumber::PhoneNumber" << "EmailAddress::EmailAddress";
    return fastlist;
}

/*!
 * Returns the list of data types supported by the vCard engine
 */
QList<QVariant::Type> QContactKabcEngine::supportedDataTypes() const
{
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);

    return st;
}

QUniqueId QContactKabcEngine::getIdOfAddressee(const KABC::Addressee& addressee, QContactManager::Error& error) const
{
    if (d->m_kabcUidToQUniqueId.contains(addressee.uid())) {
        return d->m_kabcUidToQUniqueId.value(addressee.uid());
    } else {
        bool ok = true;
        QUniqueId currId = addressee.custom("com.nokia.mobility.contacts.KAbcBackend", "id").toUInt(&ok);
        if (!ok) {
            // new contact that has been added through the native KABC API
            d->m_lastUsedId += 1;
            currId = d->m_lastUsedId;
            KABC::Addressee modified = d->ab->findByUid(addressee.uid());
            KABC::Ticket* ticket = d->ab->requestSaveTicket();
            modified.insertCustom("com.nokia.mobility.contacts.KAbcBackend", "id", QString::number(d->m_lastUsedId));
            d->ab->insertAddressee(modified);
            d->ab->save(ticket);
            QSettings definitions(d->m_settingsFile, QSettings::IniFormat);
            definitions.setValue("nextAvailableContactId", QString::number(d->m_lastUsedId + 1));
        }

        d->m_kabcUidToQUniqueId.insert(addressee.uid(), currId);
        d->m_QUniqueIdToKabcUid.insert(currId, addressee.uid());
        return currId;
    }

    error = QContactManager::UnspecifiedError;
    return QUniqueId();
}

QString QContactKabcEngine::escaped(const QString& input) const
{
    QString retn = "";
    for (int i = 0; i < input.length(); i++) {
        QChar currChar = input.at(i);
        if (currChar == '\\' ||
                currChar == '=' ||
                currChar == ',' ||
                currChar == ';') {
            // we need to escape this character.
            retn += '\\'; // escape with a single backslash.
        }
        retn += currChar;
    }

    return retn;
}

QString QContactKabcEngine::convertDetail(const QContactDetail& detail, const QContact& contact) const
{
    // the format of the converted detail will be:
    // X-com-nokia-mobility-contacts-kabcbackend-detail-UUID:definitionName;key=value,key=value;attr=bute,attr=bute;preferredFor=actionId,actionId
    // where each of the elements are escaped strings.
    QString retn = "X-com-nokia-mobility-contacts-kabcbackend-detail-";
    retn += escaped(QUuid::createUuid().toString());
    retn += ":";
    retn += escaped(detail.definitionName());
    retn += ";";

    QVariantMap vals = detail.values();
    foreach (const QString& key, vals.keys()) {
        retn += escaped(key);
        retn += "=";
        retn += escaped(vals.value(key).toString());
        retn += ",";
    }
    retn.chop(1);
    retn += ";";

    QMap<QString, QString> attrs = detail.attributes();
    foreach (const QString& key, attrs.keys()) {
        retn += escaped(key);
        retn += "=";
        retn += escaped(attrs.value(key));
        retn += ",";
    }
    retn.chop(1);

    retn += "preferredFor=";
    foreach(const QString& actionId, contact.availableActions()) {
        if (contact.isPreferredDetail(actionId, detail)) {
            retn += escaped(actionId) + ",";
        }
    }

    if (retn.endsWith(","))
        retn.chop(1);

    return retn;
}

QContactDetail QContactKabcEngine::convertCustomString(const QString& customString) const
{
    // check to see if the custom detail was put there by us.
    if (customString.indexOf("X-com-nokia-mobility-contacts-kabcbackend-detail-") != 0) {
        return QContactDetail();
    }

    // now skip past the custom detail marker
    int nextSection = customString.indexOf(":") + 1;
    int oldNextSection = nextSection; 

    // first, we parse the definitionName from the string.
    QString definitionName = "";
    for (int i = nextSection; i < customString.length(); i++) {
        QChar currChar = customString.at(i);
        if (currChar == ';') {
            nextSection = i + 1;
            break;
        } else if (currChar == '\\') {
            // the next character is escaped.
            i += 1;
        }
        definitionName += customString.at(i);
    }

    // check to see whether this custom string was built by us
    if (nextSection == oldNextSection) {
        // nope; just return an empty detail.
        return QContactDetail();
    }

    // then, we parse the values.
    QVariantMap values;
    QString currKey = "";
    QString currVal = "";
    for (int i = nextSection; i < customString.length(); i++) {
        QChar currChar = customString.at(i);
        if (currChar == ';') {
            // finished with the key/value pairs.
            nextSection = i+1;
            break;
        } else if (currChar == '=') {
            // we have found the key.  now find the value
            for (int j = i + 1; j < customString.length(); j++) {
                currChar = customString.at(j);
                if (currChar == ',') {
                    // finished with this key/value pair
                    i = j;
                    break;
                } else if (currChar == ';') {
                    // finished with all key/value pairs.
                    i = j - 1; // we want to see this char next time around.
                    break;
                } else if (currChar == '\\') {
                    j += 1;
                    currVal += customString.at(j);
                } else {
                    currVal += currChar;
                }
            }

            // and insert it into the map
            values.insert(currKey, currVal);
            currKey = "";
            currVal = "";
        } else if (currChar == ',') {
            // finished with this key/value pair.
            // this should have been handled in the above case...
            currKey = "";
            currVal = "";
        } else if (currChar == '\\') {
            // an escaped character in the key
            i += 1;
            currKey += customString.at(i);
        } else {
            // a normal character in the key.
            currKey += currChar;
        }
    }

    // check to see whether this custom string was built by us
    if (nextSection == oldNextSection) {
        // nope; just return an empty detail.
        return QContactDetail();
    }

    // finally, we parse the attributes.
    QMap<QString, QString> attrs;
    currKey = "";
    QString currAttr = "";
    for (int i = nextSection; i < customString.length(); i++) {
        QChar currChar = customString.at(i);
        if (currChar == ';') {
            // finished with the key/value pairs.
            nextSection = i+1;
            break;
        } else if (currChar == '=') {
            // we have found the key.  now find the value
            for (int j = i + 1; j < customString.length(); j++) {
                currChar = customString.at(j);
                if (currChar == ',') {
                    // finished with this key/value pair
                    i = j;
                    break;
                } else if (currChar == ';') {
                    // finished with all key/value pairs.
                    i = j - 1; // we want to see this char next time around.
                    break;
                } else if (currChar == '\\') {
                    j += 1;
                    currAttr += customString.at(j);
                } else {
                    currAttr += currChar;
                }
            }

            // and insert it into the map
            values.insert(currKey, currAttr);
            currKey = "";
            currAttr = "";
        } else if (currChar == ',') {
            // finished with this key/value pair.
            // this should have been handled in the above case...
            currKey = "";
            currVal = "";
        } else if (currChar == '\\') {
            // an escaped character in the key
            i += 1;
            currKey += customString.at(i);
        } else {
            // a normal character in the key.
            currKey += currChar;
        }
    }

    // check to see whether this custom string was built by us
    if (nextSection == oldNextSection) {
        // nope; just return an empty detail.
        return QContactDetail();
    }

    // having parsed the definitionName, values and attributes, we build the detail.
    QContactDetail retn(definitionName);
    retn.setValues(values);
    retn.setAttributes(attrs);
    return retn;
}

bool QContactKabcEngine::compareDetails(const QContactDetail& fromNative, const QContactDetail& fromCustom) const
{
    // Compares a detail built from a native field, with a detail built from a custom field
    // and determines whether they are the same detail (sans extra metadata stored in the custom).
    // Requires both details to have their definitions set properly.
    // Performs value matching (ie, considers only the fields stored in the definition)

    QString definitionName = fromNative.definitionName();
    if (definitionName != fromCustom.definitionName())
        return false;

    QContactManager::Error error = QContactManager::NoError;
    QContactDetailDefinition dd = detailDefinition(definitionName, error);
    if (error != QContactManager::NoError)
        return false;
    QMap<QString, QContactDetailDefinition::Field> fields = dd.fields();
    foreach (const QString& key, fields.keys()) {
        if (fromNative.value(key) != fromCustom.value(key)) {
            return false;
        }
    }

    return true;
}

QString QContactKabcEngine::findDuplicate(const QContactDetail& built, const QMap<QString, QContactDetail>& customDetails) const
{
    foreach (const QString& key, customDetails.keys()) {
        QContactDetail currentDetail = customDetails.value(key);
        if (compareDetails(built, currentDetail)) {
            // we have found a matching custom detail.
            return key;
        }
    }

    return QString();
}

QStringList QContactKabcEngine::findActionIdsFromCustomString(const QString& customString) const
{
    QString pref = "preferredFor=";
    int startPos = customString.indexOf(pref) + pref.length();
    if (startPos <= pref.length() || startPos >= customString.length())
        return QStringList();

    QStringList retn;
    QString currentActionId = "";
    for (int i = startPos; i < customString.length(); i++) {
        QChar currChar = customString.at(i);
        if (currChar == '\\') {
            i += 1;
            currentActionId += customString.at(i);
        } else if (currChar == ',') {
            if (currentActionId.length() > 0) {
                retn << currentActionId;
            }

            currentActionId = "";
        }
    }

    if (currentActionId.length() > 0)
        retn << currentActionId;

    return retn;
}

QContact QContactKabcEngine::convertAddressee(const KABC::Addressee& a) const
{
    // 1) Read back custom fields
    // 2) check for "this is one of my custom fields" marker
    // 3) a) (ours) build the detail, add to map of "saved custom details"
    //    b) (not ours) throw away
    // 4) Read back built-in fields
    // 5) Attempt to match every built in field with a saved custom field (by value?)
    // 6) If matches a custom, ignore it but add the custom to the contact
    // 7) Otherwise, build a new detail from it and add it.
    // 8) for any custom field that doesn't have a corresponding kabc field, add it
    // 9) for any other custom field, discard (since user deleted it using native api)
    // 10) set the id from the uid field.
    QContact retn;

    // perform steps 1, 2, 3.
    QMap<QString, QContactDetail> customDetails;
    QMap<QString, QStringList> customDetailPreferences;
    foreach (const QString& customField, a.customs()) {
        QContactDetail converted = convertCustomString(customField);
        if (!converted.isEmpty()) {
            // save the parsed detail in our map.
            QString detailUuid = QUuid::createUuid().toString();
            customDetails.insert(detailUuid, converted);

            // also, save the preferences associated with it.
            customDetailPreferences.insert(detailUuid, findActionIdsFromCustomString(customField));
        }
    }

    // perform steps 4, 5, 6, 7.
    // built-in: name
    QContactName name;
    if (!a.prefix().isEmpty())
        name.setPrefix(a.prefix());
    if (!a.givenName().isEmpty())
        name.setFirst(a.givenName());
    if (!a.additionalName().isEmpty())
        name.setMiddle(a.additionalName());
    if (!a.familyName().isEmpty())
        name.setLast(a.familyName());
    if (!a.suffix().isEmpty())
        name.setSuffix(a.suffix());

    QContact nameSynthesis;
    nameSynthesis.saveDetail(&name); // temporary
    QContactManager::Error error;

    if (!a.formattedName().isEmpty() && synthesiseDisplayLabel(nameSynthesis, error) != a.formattedName())
        retn.setDisplayLabel(a.formattedName());

    QString dupKey = findDuplicate(name, customDetails);
    if (!dupKey.isEmpty()) {
        // we have found a matching custom detail.  add it, then remove from map.
        QContactDetail currentDetail = customDetails.value(dupKey);
        retn.saveDetail(&currentDetail);
        customDetails.remove(dupKey);

        // also set the preferences; and remove from pref map.
        QStringList actionIds = customDetailPreferences.take(dupKey);
        foreach (const QString& actionId, actionIds) {
            if (!actionId.isEmpty()) {
                retn.setPreferredDetail(actionId, currentDetail);
            }
        }
    } else {
        retn.saveDetail(&name);
    }

    // built-in: phone number
    foreach (const KABC::PhoneNumber& kph, a.phoneNumbers()) {
        QContactPhoneNumber qph;
        qph.setValue(QContactPhoneNumber::FieldNumber, kph.number());
        dupKey = findDuplicate(qph, customDetails);
        if (!dupKey.isEmpty()) {
            // we have found a matching custom detail.  add it, then remove from map.
            QContactDetail currentDetail = customDetails.value(dupKey);
            retn.saveDetail(&currentDetail);
            customDetails.remove(dupKey);

            // also set the preferences; and remove from pref map.
            QStringList actionIds = customDetailPreferences.take(dupKey);
            foreach (const QString& actionId, actionIds) {
                if (!actionId.isEmpty()) {
                    retn.setPreferredDetail(actionId, currentDetail);
                }
            }
        } else {
            // nope, no matching detail.  Scrape as much information as we can
            // from the KABC detail, and build and add our own.
            QString context = "";
            QString subtype = "";
            if (kph.type() & KABC::PhoneNumber::Home)
                context += QContactDetail::AttributeContextHome + ",";
            if (kph.type() & KABC::PhoneNumber::Work)
                context += QContactDetail::AttributeContextWork + ",";
            if (kph.type() & KABC::PhoneNumber::Cell)
                subtype += QContactPhoneNumber::AttributeSubTypeMobile + ",";
            if (kph.type() & KABC::PhoneNumber::Video)
                subtype += QContactPhoneNumber::AttributeSubTypeVideo + ",";
            if (kph.type() & KABC::PhoneNumber::Car)
                subtype += QContactPhoneNumber::AttributeSubTypeCar + ",";
            if (kph.type() & KABC::PhoneNumber::Modem)
                subtype += QContactPhoneNumber::AttributeSubTypeModem + ",";
            if (kph.type() & KABC::PhoneNumber::Pager)
                subtype += QContactPhoneNumber::AttributeSubTypePager + ",";

            if (context.isEmpty())
                context = QContactDetail::AttributeContextOther + ",";
            if (subtype.isEmpty())
                subtype = QContactPhoneNumber::AttributeSubTypeLandline + ",";
            context.chop(1);
            subtype.chop(1);            

            qph.setAttribute(QContactDetail::AttributeSubType, subtype);
            qph.setAttribute(QContactDetail::AttributeContext, context);
            retn.saveDetail(&qph);
        }
    }

    // built-in: street address
    foreach (const KABC::Address& kadr, a.addresses()) {
        QContactAddress qadr;
        qadr.setStreet(kadr.street());
        qadr.setLocality(kadr.locality());
        qadr.setRegion(kadr.region());
        qadr.setPostcode(kadr.postalCode());
        qadr.setCountry(kadr.country());
        qadr.setDisplayLabel(kadr.label());
        dupKey = findDuplicate(qadr, customDetails);
        if (!dupKey.isEmpty()) {
            // we have found a matching custom detail.  add it, then remove from map.
            QContactDetail currentDetail = customDetails.value(dupKey);
            retn.saveDetail(&currentDetail);
            customDetails.remove(dupKey);

            // also set the preferences; and remove from pref map.
            QStringList actionIds = customDetailPreferences.take(dupKey);
            foreach (const QString& actionId, actionIds) {
                if (!actionId.isEmpty()) {
                    retn.setPreferredDetail(actionId, currentDetail);
                }
            }
        } else {
            // nope, no matching detail.  Scrape as much information as we can
            // from the KABC detail, and build and add our own.
            QString context = "";
            QString subtype = "";
            if (kadr.type() & KABC::Address::Home)
                context += QContactDetail::AttributeContextHome + ",";
            if (kadr.type() & KABC::Address::Work)
                context += QContactDetail::AttributeContextWork + ",";
            if (kadr.type() & KABC::Address::Postal)
                subtype += QContactAddress::AttributeSubTypePostal + ",";
            if (kadr.type() & KABC::Address::Parcel)
                subtype += QContactAddress::AttributeSubTypeParcel + ",";
            if (kadr.type() & KABC::Address::Dom)
                subtype += QContactAddress::AttributeSubTypeDomestic + ",";
            if (kadr.type() & KABC::Address::Intl)
                subtype += QContactAddress::AttributeSubTypeInternational + ",";

            if (context.isEmpty())
                context = QContactDetail::AttributeContextOther + ",";
            context.chop(1);
            if (!subtype.isEmpty())
                subtype.chop(1);

            qadr.setAttribute(QContactDetail::AttributeSubType, subtype);
            qadr.setAttribute(QContactDetail::AttributeContext, context);

            retn.saveDetail(&qadr);
        }
    }

    // built-in: email address
    foreach (const QString& kem, a.emails()) {
        QContactEmailAddress qem;
        qem.setEmailAddress(kem);
        dupKey = findDuplicate(qem, customDetails);
        if (!dupKey.isEmpty()) {
            // we have found a matching custom detail.  add it, then remove from map.
            QContactDetail currentDetail = customDetails.value(dupKey);
            retn.saveDetail(&currentDetail);
            customDetails.remove(dupKey);

            // also set the preferences; and remove from pref map.
            QStringList actionIds = customDetailPreferences.take(dupKey);
            foreach (const QString& actionId, actionIds) {
                if (!actionId.isEmpty()) {
                    retn.setPreferredDetail(actionId, currentDetail);
                }
            }
        } else {
            // nope, no matching detail.  Scrape as much information as we can
            // from the KABC detail, and build and add our own.
            retn.saveDetail(&qem);
        }
    }

    // built-in: avatar
    QString tempstr = a.photo().url();
    if (!tempstr.isEmpty()) {
        QContactAvatar qav;
        qav.setAvatar(a.photo().url());
        dupKey = findDuplicate(qav, customDetails);
        if (!dupKey.isEmpty()) {
            // we have found a matching custom detail.  add it, then remove from map.
            QContactDetail currentDetail = customDetails.value(dupKey);
            retn.saveDetail(&currentDetail);
            customDetails.remove(dupKey);

            // also set the preferences; and remove from pref map.
            QStringList actionIds = customDetailPreferences.take(dupKey);
            foreach (const QString& actionId, actionIds) {
                if (!actionId.isEmpty()) {
                    retn.setPreferredDetail(actionId, currentDetail);
                }
            }
        } else {
            // nope, no matching detail.  Scrape as much information as we can
            // from the KABC detail, and build and add our own.
            retn.saveDetail(&qav);
        }
    }

    // built-in: birthday
    tempstr = a.birthday().date().toString();
    if (!tempstr.isEmpty()) {
        QContactBirthday qbd;
        qbd.setDate(a.birthday().date());
        dupKey = findDuplicate(qbd, customDetails);
        if (!dupKey.isEmpty()) {
            // we have found a matching custom detail.  add it, then remove from map.
            QContactDetail currentDetail = customDetails.value(dupKey);
            retn.saveDetail(&currentDetail);
            customDetails.remove(dupKey);

            // also set the preferences; and remove from pref map.
            QStringList actionIds = customDetailPreferences.take(dupKey);
            foreach (const QString& actionId, actionIds) {
                if (!actionId.isEmpty()) {
                    retn.setPreferredDetail(actionId, currentDetail);
                }
            }
        } else {
            // nope, no matching detail.  Scrape as much information as we can
            // from the KABC detail, and build and add our own.
            retn.saveDetail(&qbd);
        }
    }

    // built-in: url
    tempstr = a.url().url();
    if (!tempstr.isEmpty()) {
        QContactUrl qurl;
        qurl.setUrl(a.url().url());
        dupKey = findDuplicate(qurl, customDetails);
        if (!dupKey.isEmpty()) {
            // we have found a matching custom detail.  add it, then remove from map.
            QContactDetail currentDetail = customDetails.value(dupKey);
            retn.saveDetail(&currentDetail);
            customDetails.remove(dupKey);

            // also set the preferences; and remove from pref map.
            QStringList actionIds = customDetailPreferences.take(dupKey);
            foreach (const QString& actionId, actionIds) {
                if (!actionId.isEmpty()) {
                    retn.setPreferredDetail(actionId, currentDetail);
                }
            }
        } else {
            // nope, no matching detail.  Scrape as much information as we can
            // from the KABC detail, and build and add our own.
            retn.saveDetail(&qurl);
        }
    }

    // perform steps 8, 9.
    foreach (const QString& key, customDetails.keys()) {
        // grab the detail associated with the current key
        QContactDetail det = customDetails.value(key);

        // we ignore any details whose definitions we have already dealt with
        QString defId = det.definitionName();
        if (defId == QContactName::DefinitionName
                || defId == QContactAddress::DefinitionName
                || defId == QContactPhoneNumber::DefinitionName
                || defId == QContactEmailAddress::DefinitionName
                || defId == QContactAvatar::DefinitionName
                || defId == QContactBirthday::DefinitionName
                || defId == QContactUrl::DefinitionName
                || defId == QContactBirthday::DefinitionName) {
            // discard this custom detail.  we should already have dealt with it;
            // the fact that we haven't means that the user has used the native
            // KABC api to delete the associated data.
        } else {
            // we haven't dealt with details of this definition; save it.
            QContactDetail toAdd = det;
            retn.saveDetail(&toAdd);

            // and set any associated preferences
            QStringList actionIds = customDetailPreferences.value(key);
            foreach (const QString& actionId, actionIds) {
                if (!actionId.isEmpty()) {
                    retn.setPreferredDetail(actionId, det);
                }
            }
        }
    }

    // perform step 10.
    retn.setId(getIdOfAddressee(a, error));

    return retn;
}

KABC::Addressee QContactKabcEngine::convertContact(const QContact& contact) const
{
    KABC::Addressee retn;
    retn.setUid(d->m_QUniqueIdToKabcUid.value(contact.id())); 

    foreach (const QContactDetail& det, contact.details()) {
        QString definitionName = det.definitionName();
        if (definitionName == QContactName::DefinitionName) {
            retn.setPrefix(det.value(QContactName::FieldPrefix));
            retn.setGivenName(det.value(QContactName::FieldFirst));
            retn.setAdditionalName(det.value(QContactName::FieldMiddle));
            retn.setFamilyName(det.value(QContactName::FieldLast));
            retn.setSuffix(det.value(QContactName::FieldSuffix));
        } else if (definitionName == QContactDisplayLabel::DefinitionName) {
            if (!det.variantValue(QContactDisplayLabel::FieldSynthesised).toBool()) {
                retn.setFormattedName(det.value(QContactDisplayLabel::FieldLabel));
            }
        } else if (definitionName == QContactAddress::DefinitionName) {
            KABC::Address kadr;
            KABC::Address::Type typeFlags = 0x00;

            kadr.setStreet(det.value(QContactAddress::FieldStreet));
            kadr.setLocality(det.value(QContactAddress::FieldLocality));
            kadr.setRegion(det.value(QContactAddress::FieldRegion));
            kadr.setPostalCode(det.value(QContactAddress::FieldPostcode));
            kadr.setCountry(det.value(QContactAddress::FieldCountry));
            kadr.setLabel(det.value(QContactAddress::FieldDisplayLabel));

            if (det.attributes().value(QContactDetail::AttributeSubType).contains(QContactAddress::AttributeSubTypeDomestic))
                typeFlags |= KABC::Address::Dom;
            if (det.attributes().value(QContactDetail::AttributeSubType).contains(QContactAddress::AttributeSubTypeInternational))
                typeFlags |= KABC::Address::Intl;
            if (det.attributes().value(QContactDetail::AttributeSubType).contains(QContactAddress::AttributeSubTypePostal))
                typeFlags |= KABC::Address::Postal;
            if (det.attributes().value(QContactDetail::AttributeSubType).contains(QContactAddress::AttributeSubTypeParcel))
                typeFlags |= KABC::Address::Parcel;
            if (det.attributes().value(QContactDetail::AttributeContext).contains(QContactDetail::AttributeContextHome))
                typeFlags |= KABC::Address::Home;
            if (det.attributes().value(QContactDetail::AttributeContext).contains(QContactDetail::AttributeContextWork))
                typeFlags |= KABC::Address::Work;

            kadr.setType(typeFlags);
            retn.insertAddress(kadr);
        } else if (definitionName == QContactEmailAddress::DefinitionName) {
            retn.insertEmail(det.value(QContactEmailAddress::FieldEmailAddress));
        } else if (definitionName == QContactPhoneNumber::DefinitionName) {
            KABC::PhoneNumber phn;
            KABC::PhoneNumber::Type typeFlags = 0x00;

            phn.setNumber(det.value(QContactPhoneNumber::FieldNumber));
            if (det.attributes().value(QContactDetail::AttributeSubType).contains(QContactPhoneNumber::AttributeSubTypeMobile))
                typeFlags |= KABC::PhoneNumber::Cell;
            if (det.attributes().value(QContactDetail::AttributeSubType).contains(QContactPhoneNumber::AttributeSubTypeVideo))
                typeFlags |= KABC::PhoneNumber::Video;
            if (det.attributes().value(QContactDetail::AttributeContext).contains(QContactDetail::AttributeContextHome))
                typeFlags |= KABC::PhoneNumber::Home;
            if (det.attributes().value(QContactDetail::AttributeContext).contains(QContactDetail::AttributeContextWork))
                typeFlags |= KABC::PhoneNumber::Work;

            phn.setType(typeFlags);
            retn.insertPhoneNumber(phn);
        } else if (definitionName == QContactAvatar::DefinitionName) {
            retn.setPhoto(det.value(QContactAvatar::FieldAvatar));
        } else if (definitionName == QContactBirthday::DefinitionName) {
            retn.setBirthday(QDateTime(det.value<QDate>(QContactBirthday::FieldBirthday)));
        } else if (definitionName == QContactGuid::DefinitionName) {
            // TODO: resolve c.id() vs c.guid() problem...
        } else if (definitionName == QContactUrl::DefinitionName) {
            retn.setUrl(det.value(QContactUrl::FieldUrl));
        } else if (definitionName == QContactBirthday::DefinitionName) {
            retn.setNote(det.value(QContactBirthday::FieldBirthday));
        } else {
            // there is no kabc field mapping for this detail;
            // so, simply save it as a custom field.
        }

        // even if there was a mapping, we save it as a custom field (to save all of the associated metadata etc)
        retn.insertCustom("com.nokia.mobility.contacts.kabcbackend", QUuid::createUuid().toString(), convertDetail(det, contact));
    }

    return retn;
}
 
