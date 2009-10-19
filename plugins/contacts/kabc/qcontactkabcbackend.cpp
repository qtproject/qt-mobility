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
#include "qcontactkabcbackend_p.h"

#include <QFileSystemWatcher>
#include <QFile>
#include <QSettings>
#include <QUuid>

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
            dd.setName(defId);
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

QMap<QString, QString> QContactKabcEngine::parameters() const
{
    QMap<QString, QString> params;
    params.insert("resource", d->m_resourceFile);
    params.insert("settings", d->m_settingsFile);
    return params;
}

QList<QContactLocalId> QContactKabcEngine::contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    QList<QContactLocalId> allCIds;
    error = QContactManager::NoError;
    KABC::Addressee::List allAddressees = d->ab->allAddressees();
    foreach (const KABC::Addressee& curr, allAddressees) {
        allCIds.append(getIdOfAddressee(curr, error));
    }

    // return the list sorted according to sortOrders
    QContactManager::Error sortError;
    QList<QContact> sorted;
    QList<QContactLocalId> sortedIds;
    for (int i = 0; i < allCIds.size(); i++)
        QContactManagerEngine::addSorted(&sorted, contact(allCIds.at(i), sortError), sortOrders);
    for (int i = 0; i < sorted.size(); i++)
        sortedIds.append(sorted.at(i).id());

    return sortedIds;
}

QContact QContactKabcEngine::contact(const QContactLocalId& contactId, QContactManager::Error& error) const
{
    error = QContactManager::NoError;
    if (d->m_QContactLocalIdToKabcUid.contains(contactId)) {
        KABC::Addressee corresponding = d->ab->findByUid(d->m_QContactLocalIdToKabcUid.value(contactId));
        QContact ret = convertAddressee(corresponding);
        ret.setId(contactId);
        return ret;
    }

    error = QContactManager::DoesNotExistError;
    return QContact();
}

bool QContactKabcEngine::saveContact(QContact* contact, QSet<QContactLocalId>& contactsAdded, QSet<QContactLocalId>& contactsChanged, QSet<QContactLocalId>& groupsChanged, QContactManager::Error& error)
{
    if (contact == 0) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    // ensure that the contact's details conform to their definitions
    if (!validateContact(*contact, error)) {
        error = QContactManager::InvalidDetailError;
        return false;
    }

    KABC::Ticket *ticket = d->ab->requestSaveTicket();
    KABC::Addressee converted = convertContact(*contact);
    if (!d->m_QContactLocalIdToKabcUid.contains(contact->id())) {
        // new contact (or previously saved and removed)
        d->m_lastUsedId += 1;
        contact->setId(d->m_lastUsedId);
        converted.insertCustom("com.nokia.mobility.contacts.KAbcBackend", "id", QString::number(d->m_lastUsedId));
        QString newUuid = QUuid::createUuid().toString();
        d->m_QContactLocalIdToKabcUid.insert(d->m_lastUsedId, newUuid);
        d->m_kabcUidToQContactLocalId.insert(newUuid, contact->id());
        QSettings definitions(d->m_settingsFile, QSettings::IniFormat);
        definitions.setValue("nextAvailableContactId", QString::number(d->m_lastUsedId + 1));
        contactsAdded.insert(contact->id());
    } else {
        contactsChanged.insert(contact->id());
    }

    // update groups if required.
    QContactManager::Error groupError;
    QList<QContactLocalId> allGroups = groups(groupError);
    QList<QContactLocalId> contactGroups = contact->groups();
    for (int i = 0; i < allGroups.size(); i++) {
        QSet<QContactLocalId> temp1, temp2, temp3;
        QContactGroup curr = group(allGroups.at(i), groupError);
        if (contactGroups.contains(allGroups.at(i))) {
            // the contact should be part of this group
            if (!curr.hasMember(contact->id())) {
                curr.addMember(contact->id());
                saveGroup(&curr, temp1, temp2, temp3, groupError);
                groupsChanged.insert(curr.id());
            }
        } else {
            // the contact should not be part of this group
            if (curr.hasMember(contact->id())) {
                curr.removeMember(contact->id());
                saveGroup(&curr, temp1, temp2, temp3, groupError);
                groupsChanged.insert(curr.id());
            }
        }
    }
    
    // save to KABC database
    converted.setUid(d->m_QContactLocalIdToKabcUid.value(contact->id()));
    d->ab->insertAddressee(converted);
    d->ab->save(ticket);

    // success!
    error = QContactManager::NoError;
    return true;
}

bool QContactKabcEngine::removeContact(const QContactLocalId& contactId, QSet<QContactLocalId>& contactsChanged, QSet<QContactLocalId>& groupsChanged, QContactManager::Error& error)
{
    if (!d->m_QContactLocalIdToKabcUid.contains(contactId)) {
        error = QContactManager::DoesNotExistError;
        return false;
    }

    KABC::Ticket *ticket = d->ab->requestSaveTicket();
    d->ab->removeAddressee(d->ab->findByUid(d->m_QContactLocalIdToKabcUid.value(contactId)));
    d->m_kabcUidToQContactLocalId.remove(d->m_QContactLocalIdToKabcUid.value(contactId));
    d->m_QContactLocalIdToKabcUid.remove(contactId);
    d->ab->save(ticket);
    error = QContactManager::NoError;

    // remove the contact from any groups it might have been in
    QContactManager::Error groupError;
    QList<QContactLocalId> allGroups = groups(groupError);
    for (int i = 0; i < allGroups.size(); i++) {
        QSet<QContactLocalId> temp1, temp2, temp3;
        QContactGroup curr = group(allGroups.at(i), groupError);
        if (curr.hasMember(contactId)) {
            curr.removeMember(contactId);
            saveGroup(&curr, temp1, temp2, temp3, groupError);
            groupsChanged.insert(curr.id());
        }
    }

    // success
    contactsChanged.insert(contactId);
    return true;
}

/*
QList<QContactLocalId> QContactKabcEngine::groups(QContactManager::Error& error) const
{
    QList<QContactLocalId> retn;
    QList<KABC::DistributionList*> allKabcGroups = d->ab->allDistributionLists();
    for (int i = 0; i < allKabcGroups.count(); i++) {
        bool ok = true;
        QContactLocalId currId = allKabcGroups.at(i)->identifier().toUInt(&ok);
        if (ok) {
            retn.append(currId);
        } else {
            // nothing.  we simply don't include it in the list.
        }
    }

    error = QContactManager::NoError;
    return retn;
}

QContactGroup QContactKabcEngine::group(const QContactLocalId& groupId, QContactManager::Error& error) const
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
    QList<QContactLocalId> members = group->members();
    foreach(QContactLocalId id, members) {
        kabcGroup->insertEntry(convertContact(contact(id, error)));
    }

    d->ab->save(ticket);

    error = QContactManager::NoError;
    return true;
}

bool QContactKabcEngine::removeGroup(const QContactLocalId& groupId, QContactManager::Error& error)
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
    if (!validateDefinition(def, error)) {
        return false;
    }
    if (!d->m_definitions.key(def, "").isEmpty())
        removeDetailDefinition(def.name(), error);
    d->m_definitions.insert(def.name(), def);

    // we need to persist definitions.
    if (d->m_settingsWatcher)
        d->m_settingsWatcher->blockSignals(true);
    QSettings definitions(d->m_settingsFile, QSettings::IniFormat);
    if (definitions.childGroups().contains(def.name()))
        definitions.remove(def.name());

    definitions.beginGroup(def.name());
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

bool QContactKabcEngine::removeDetailDefinition(const QString& definitionId, QContactManager::Error& error)
{
    if (definitionId.isEmpty()) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    error = QContactManager::DoesNotExistError;
    bool success = (d->m_definitions.remove(definitionId));

    if (success) {
        // we need to persist the removal.
        QSettings definitions(d->m_settingsFile, QSettings::IniFormat);
        definitions.remove(definitionId);
        error = QContactManager::NoError;
    }

    return success;
}

/*! \reimp */
bool QContactKabcEngine::hasFeature(QContactManager::ManagerFeature feature) const
{
    switch (feature) {
        case QContactManager::ActionPreferences:
        case QContactManager::MutableDefinitions:
            return true;
        default:
            return false;
    }
}

/*!
 * Returns the list of data types supported by the KABC engine
 */
QList<QVariant::Type> QContactKabcEngine::supportedDataTypes() const
{
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);

    return st;
}

QContactLocalId QContactKabcEngine::getIdOfAddressee(const KABC::Addressee& addressee, QContactManager::Error& error) const
{
    if (d->m_kabcUidToQContactLocalId.contains(addressee.uid())) {
        return d->m_kabcUidToQContactLocalId.value(addressee.uid());
    } else {
        bool ok = true;
        QContactLocalId currId = addressee.custom("com.nokia.mobility.contacts.KAbcBackend", "id").toUInt(&ok);
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

        d->m_kabcUidToQContactLocalId.insert(addressee.uid(), currId);
        d->m_QContactLocalIdToKabcUid.insert(currId, addressee.uid());
        return currId;
    }

    error = QContactManager::UnspecifiedError;
    return QContactLocalId();
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
    foreach (const QString& fieldName, values.keys())
        retn.setValue(fieldName, values.value(fieldName));
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

    QString synthName = synthesizeDisplayLabel(nameSynthesis, error);
    if (!a.formattedName().isEmpty() && synthName != a.formattedName())
        retn.setDisplayLabel(a.formattedName());
    else {
        QContactDisplayLabel dl = retn.detail(QContactDisplayLabel::DefinitionName);
        if (dl.label().isEmpty()) {
            dl.setLabel(synthName);
            dl.setSynthesized(true);
            retn.saveDetail(&dl);
        }
    }

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
            QStringList contexts;
            QStringList subtypes;
            if (kph.type() & KABC::PhoneNumber::Home)
                contexts << QContactDetail::ContextHome;
            if (kph.type() & KABC::PhoneNumber::Work)
                contexts << QContactDetail::ContextWork;
            if (kph.type() & KABC::PhoneNumber::Cell)
                subtypes << QContactPhoneNumber::SubTypeMobile;
            if (kph.type() & KABC::PhoneNumber::Video)
                subtypes << QContactPhoneNumber::SubTypeVideo;
            if (kph.type() & KABC::PhoneNumber::Car)
                subtypes << QContactPhoneNumber::SubTypeCar;
            if (kph.type() & KABC::PhoneNumber::Modem)
                subtypes << QContactPhoneNumber::SubTypeModem;
            if (kph.type() & KABC::PhoneNumber::Pager)
                subtypes << QContactPhoneNumber::SubTypePager;

            if (contexts.isEmpty())
                contexts << QContactDetail::ContextOther;
            if (subtypes.isEmpty())
                subtypes << QContactPhoneNumber::SubTypeLandline;

            qph.setSubTypes(subtypes);
            qph.setContexts(contexts);
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
            QStringList contexts;
            QStringList subtypes;
            if (kadr.type() & KABC::Address::Home)
                contexts << QContactDetail::ContextHome;
            if (kadr.type() & KABC::Address::Work)
                contexts << QContactDetail::ContextWork;
            if (kadr.type() & KABC::Address::Postal)
                subtypes << QContactAddress::SubTypePostal;
            if (kadr.type() & KABC::Address::Parcel)
                subtypes << QContactAddress::SubTypeParcel;
            if (kadr.type() & KABC::Address::Dom)
                subtypes << QContactAddress::SubTypeDomestic;
            if (kadr.type() & KABC::Address::Intl)
                subtypes << QContactAddress::SubTypeInternational;

            if (contexts.isEmpty())
                contexts << QContactDetail::ContextOther;

            qadr.setSubTypes(subtypes);
            qadr.setContexts(contexts);

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
    retn.setUid(d->m_QContactLocalIdToKabcUid.value(contact.id()));

    foreach (const QContactDetail& det, contact.details()) {
        QString definitionName = det.definitionName();
        if (definitionName == QContactName::DefinitionName) {
            retn.setPrefix(det.value(QContactName::FieldPrefix));
            retn.setGivenName(det.value(QContactName::FieldFirst));
            retn.setAdditionalName(det.value(QContactName::FieldMiddle));
            retn.setFamilyName(det.value(QContactName::FieldLast));
            retn.setSuffix(det.value(QContactName::FieldSuffix));
        } else if (definitionName == QContactDisplayLabel::DefinitionName) {
            if (!det.variantValue(QContactDisplayLabel::FieldSynthesized).toBool()) {
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

            if (det.variantValue(QContactAddress::FieldSubTypes).toStringList().contains(QContactAddress::SubTypeDomestic))
                typeFlags |= KABC::Address::Dom;
            if (det.variantValue(QContactAddress::FieldSubTypes).toStringList().contains(QContactAddress::SubTypeInternational))
                typeFlags |= KABC::Address::Intl;
            if (det.variantValue(QContactAddress::FieldSubTypes).toStringList().contains(QContactAddress::SubTypePostal))
                typeFlags |= KABC::Address::Postal;
            if (det.variantValue(QContactAddress::FieldSubTypes).toStringList().contains(QContactAddress::SubTypeParcel))
                typeFlags |= KABC::Address::Parcel;
            if (det.variantValue(QContactDetail::FieldContext).toStringList().contains(QContactDetail::ContextHome))
                typeFlags |= KABC::Address::Home;
            if (det.variantValue(QContactDetail::FieldContext).toStringList().contains(QContactDetail::ContextWork))
                typeFlags |= KABC::Address::Work;

            kadr.setType(typeFlags);
            retn.insertAddress(kadr);
        } else if (definitionName == QContactEmailAddress::DefinitionName) {
            retn.insertEmail(det.value(QContactEmailAddress::FieldEmailAddress));
        } else if (definitionName == QContactPhoneNumber::DefinitionName) {
            KABC::PhoneNumber phn;
            KABC::PhoneNumber::Type typeFlags = 0x00;

            phn.setNumber(det.value(QContactPhoneNumber::FieldNumber));
            if (det.variantValue(QContactPhoneNumber::FieldSubTypes).toStringList().contains(QContactPhoneNumber::SubTypeMobile))
                typeFlags |= KABC::PhoneNumber::Cell;
            if (det.variantValue(QContactPhoneNumber::FieldSubTypes).toStringList().contains(QContactPhoneNumber::SubTypeVideo))
                typeFlags |= KABC::PhoneNumber::Video;
            if (det.variantValue(QContactDetail::FieldContext).toStringList().contains(QContactDetail::ContextHome))
                typeFlags |= KABC::PhoneNumber::Home;
            if (det.variantValue(QContactDetail::FieldContext).toStringList().contains(QContactDetail::ContextWork))
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
 
