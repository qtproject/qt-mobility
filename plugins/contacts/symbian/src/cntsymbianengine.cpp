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
#include <QFileSystemWatcher>
#include <QFile>
#include <QSettings>
#include <QUuid>

#include <QDebug>

#include <qtcontacts.h>

#include "qcontact_p.h"
#include "qcontactmanager_p.h"

#include "cntsymbianengine.h"
#include "cntsymbianengine_p.h"
#include "qcontactchangeset.h"

#include <flogger.h>
namespace {
    void PbkPrintToLog( TRefByValue<const TDesC> aFormat, ... )
    {
        _LIT( KLogDir, "sp" );
        _LIT( KLogName, "sp.log" );

        VA_LIST args;
        VA_START( args, aFormat );
        RFileLogger::WriteFormat(KLogDir, KLogName, EFileLoggingModeAppend, aFormat, args);
        VA_END( args );
    }
}  // namespace

CntSymbianEngine::CntSymbianEngine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    PbkPrintToLog(_L("CntSymbianEngine::CntSymbianEngine"));

    error = QContactManager::NoError;

    d = new CntSymbianEnginePrivate(parameters, error);

    // Connect database observer events appropriately.
    connect(d, SIGNAL(contactAdded(QContactLocalId)),
        this, SLOT(eventContactAdded(QContactLocalId)));

    connect(d, SIGNAL(contactRemoved(QContactLocalId)),
        this, SLOT(eventContactRemoved(QContactLocalId)));

    connect(d, SIGNAL(contactChanged(QContactLocalId)),
        this, SLOT(eventContactChanged(QContactLocalId)));

    connect(d, SIGNAL(relationshipAdded(QContactLocalId)),
        this, SLOT(eventRelationshipRemoved(QContactLocalId)));

    connect(d, SIGNAL(relationshipRemoved(QContactLocalId)),
        this, SLOT(eventRelationshipRemoved(QContactLocalId)));
}

CntSymbianEngine::CntSymbianEngine(const CntSymbianEngine& other)
    : QContactManagerEngine(), d(other.d)
{
    PbkPrintToLog(_L("CntSymbianEngine::CntSymbianEngine"));
}

CntSymbianEngine& CntSymbianEngine::operator=(const CntSymbianEngine& other)
{
    // assign
    d = other.d;

    return *this;
}

CntSymbianEngine::~CntSymbianEngine()
{
    PbkPrintToLog(_L("~CntSymbianEngine::CntSymbianEngine"));
}

void CntSymbianEngine::deref()
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
QList<QContactLocalId> CntSymbianEngine::contacts(
        const QContactFilter& filter,
        const QList<QContactSortOrder>& sortOrders,
        QContactManager::Error& error) const
{
    QList<QContactLocalId> result;

    // Check if the filter is supported by the underlying filter implementation
    CntAbstractContactFilter::FilterSupport filterSupport = d->filterSupported(filter);

    if (filterSupport == CntAbstractContactFilter::Supported) {
        // Filter supported, use as the result directly
        result = d->contacts(filter, sortOrders, error);
        // If sorting is not supported, we need to fallback to slow sorting
        if(!d->sortOrderSupported(sortOrders))
            result = slowSort(result, sortOrders, error);
    } else if (filterSupport == CntAbstractContactFilter::SupportedPreFilterOnly) {
        // Filter only does pre-filtering and may include false positives
        QList<QContactLocalId> contacts = d->contacts(filter, sortOrders, error);
        if(error == QContactManager::NoError)
            result = slowFilter(filter, contacts, error);
        // If sorting is not supported, we need to fallback to slow sorting
        if(!d->sortOrderSupported(sortOrders))
            result = slowSort(result, sortOrders, error);
    } else {
        // Filter not supported; fetch all contacts and remove false positives
        // one-by-one. Note: this is reeeeaally slow. Both sorting and
        // filtering are done the slow way.
        QList<QContactLocalId> sortedIds = contacts(sortOrders,error);
        if(error == QContactManager::NoError)
            result = slowFilter(filter, sortedIds, error);
    }
    return result;
}

QList<QContactLocalId> CntSymbianEngine::contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    // Check if sorting is supported by backend
    if(d->sortOrderSupported(sortOrders))
        return d->contacts(sortOrders,error);

    // Backend does not support this sorting.
    // Fall back to slow QContact-level sorting method.

    // Get unsorted contact ids
    QList<QContactSortOrder> noSortOrders;
    QList<QContactLocalId> unsortedIds = d->contacts(noSortOrders, error);
    if (error != QContactManager::NoError)
        return QList<QContactLocalId>();

    // Sort contacts
    return slowSort(unsortedIds, sortOrders, error);
}

QList<QContactLocalId> CntSymbianEngine::contacts(const QString& contactType, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    QList<QContactLocalId> contactIds;

    //retrieve contacts
    if(contactType == QContactType::TypeContact) {
        return contacts(sortOrders, error);
    } else if(contactType == QContactType::TypeGroup) {
        //retrieve groups
        contactIds = d->groups(error);

        // Check if sorting is supported by backend
        if(d->sortOrderSupported(sortOrders)) {
            return d->sort(contactIds, sortOrders, error);
            //return contactIds;
        } else {
            // Backend does not support this sorting.
            // Fall back to slow QContact-level sorting method.
            return slowSort(contactIds, sortOrders, error);
        }
    }

    // Should never happen
    error = QContactManager::BadArgumentError;
    return contactIds; // empty
}

QContact CntSymbianEngine::contact(const QContactLocalId& contactId, QContactManager::Error& error) const
{
    QContact contact = d->contact(contactId, error);

    // synthesize display label (the label it is not saved to the contact
    // database, thus not modifiable by a client).
    if(error == QContactManager::NoError) {
        updateDisplayLabel(contact);
    }

    return contact;
}

bool CntSymbianEngine::saveContact(QContact* contact, QContactManager::Error& error)
{
    PbkPrintToLog(_L("CntSymbianEngine::saveContact - IN"));
    QContactChangeSet changeSet;
    TBool ret = doSaveContact(contact, changeSet, error);
    changeSet.emitSignals(this);
    PbkPrintToLog(_L("CntSymbianEngine::saveContact - OUT, ret = %d"), ret);
    return ret;
}

QList<QContactManager::Error> CntSymbianEngine::saveContacts(QList<QContact>* contacts, QContactManager::Error& error)
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

QList<QContactLocalId> CntSymbianEngine::slowFilter(
        const QContactFilter& filter,
        const QList<QContactLocalId>& contacts,
        QContactManager::Error& error
        ) const
{
    QList<QContactLocalId> result;
    for (int i(0); i < contacts.count(); i++) {
        QContactLocalId id = contacts.at(i);

        // Check if this is a false positive. If not, add to the result set.
        if(QContactManagerEngine::testFilter(filter, d->contact(id, error)))
            result << id;
    }
    return result;
}

QList<QContactLocalId> CntSymbianEngine::slowSort(
        const QList<QContactLocalId>& contactIds,
        const QList<QContactSortOrder>& sortOrders,
        QContactManager::Error& error) const
{
    // Get unsorted contacts
    QList<QContact> unsortedContacts;
    foreach (QContactLocalId id, contactIds) {
        QContact c = contact(id, error);
        if (error != QContactManager::NoError)
            return QList<QContactLocalId>();
        unsortedContacts << c;
    }
    return QContactManagerEngine::sortContacts(unsortedContacts, sortOrders);
}

bool CntSymbianEngine::doSaveContact(QContact* contact, QContactChangeSet& changeSet, QContactManager::Error& error)
{
    bool ret = false;
    
    // If contact has GUid and no local Id, try to find it in database
    if (contact && !contact->localId() && 
        contact->details(QContactGuid::DefinitionName).count() > 0) {
        QContactDetailFilter guidFilter;
        guidFilter.setDetailDefinitionName(QContactGuid::DefinitionName, QContactGuid::FieldGuid); 
        QContactGuid guidDetail = static_cast<QContactGuid>(contact->details(QContactGuid::DefinitionName).at(0));
        guidFilter.setValue(guidDetail.guid());
        
        QContactManager::Error err;
        QList<QContactLocalId> localIdList = contacts(guidFilter,
                QList<QContactSortOrder>(), err);
        if (err == QContactManager::NoError && localIdList.count() > 0) {
            QScopedPointer<QContactId> contactId(new QContactId());
            contactId->setLocalId(localIdList.at(0));
            contactId->setManagerUri(managerUri());
            contact->setId(*contactId);
        }
    }
    
    // Check parameters
    if(!contact) {
        error = QContactManager::BadArgumentError;
        ret = false;
    // Update an existing contact
    } else if(contact->localId()) {
        if(contact->id().managerUri() == managerUri()) {
            ret = d->updateContact(*contact, changeSet, error);
            if (ret)
                updateDisplayLabel(*contact);
        } else {
            error = QContactManager::BadArgumentError;
            ret = false;
        }
    // Create new contact
    } else {
        ret = d->addContact(*contact, changeSet, error);
        if (ret)
            updateDisplayLabel(*contact);
    }
    return ret;
}

void CntSymbianEngine::updateDisplayLabel(QContact& contact) const
{
    QContactManager::Error error(QContactManager::NoError);
    QContactDisplayLabel label = contact.displayLabel();
    QString labelString = synthesizeDisplayLabel(contact, error);
    if(error == QContactManager::NoError) {
        label.setLabel(labelString);
        label.setSynthesized(true);
        contact.setDisplayLabel(label);
    }
}

bool CntSymbianEngine::removeContact(const QContactLocalId& contactId, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    TBool ret = d->removeContact(contactId, changeSet, error);
    changeSet.emitSignals(this);
    return ret;
}

QList<QContactManager::Error> CntSymbianEngine::removeContacts(QList<QContactLocalId>* contactIds, QContactManager::Error& error)
{
    QContactChangeSet changeSet;
    QList<QContactManager::Error> ret;
    if (!contactIds) {
        error = QContactManager::BadArgumentError;
        return ret;
    } else {
        QList<QContactLocalId> removedList;
        QContactManager::Error functionError = QContactManager::NoError;
        for (int i = 0; i < contactIds->count(); i++) {
            QContactLocalId current = contactIds->at(i);
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

/* relationships */

QList<QContactRelationship> CntSymbianEngine::relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationshipFilter::Role role, QContactManager::Error& error) const
{
    //retrieve the relationships
    return d->relationships(relationshipType, participantId, role, error);
}

bool CntSymbianEngine::saveRelationship(QContactRelationship* relationship, QContactManager::Error& error)
{
    //affected contacts
    QContactChangeSet changeSet;

    //save the relationship
    bool returnValue = d->saveRelationship(&changeSet, relationship, error);

    //emit signals
    changeSet.emitSignals(this);

    return returnValue;
}

QList<QContactManager::Error> CntSymbianEngine::saveRelationships(QList<QContactRelationship>* relationships, QContactManager::Error& error)
{
    //affected contacts
    QContactChangeSet changeSet;

    //save the relationships
    QList<QContactManager::Error> returnValue = d->saveRelationships(&changeSet, relationships, error);

    //emit signals
    changeSet.emitSignals(this);

    return returnValue;
}

bool CntSymbianEngine::removeRelationship(const QContactRelationship& relationship, QContactManager::Error& error)
{
    //affected contacts
    QContactChangeSet changeSet;

    //remove the relationship
    bool returnValue = d->removeRelationship(&changeSet, relationship, error);

    //emit signals
    changeSet.emitSignals(this);

    return returnValue;
}

QList<QContactManager::Error> CntSymbianEngine::removeRelationships(const QList<QContactRelationship>& relationships, QContactManager::Error& error)
{
    //affected contacts
    QContactChangeSet changeSet;

    //remove the relationships
    QList<QContactManager::Error> returnValue = d->removeRelationships(&changeSet, relationships, error);

    //emit signals
    changeSet.emitSignals(this);

    return returnValue;
}

QMap<QString, QContactDetailDefinition> CntSymbianEngine::detailDefinitions(const QString& contactType, QContactManager::Error& error) const
{
    // TODO: update for SIM contacts later
    if (contactType != QContactType::TypeContact && contactType != QContactType::TypeGroup) {
        error = QContactManager::InvalidContactTypeError;
        return QMap<QString, QContactDetailDefinition>();
    }

    error = QContactManager::NoError;

    // Get the supported detail definitions from the contact transformer
    CntTransformContact *transformContact = new CntTransformContact;
    QMap<QString, QContactDetailDefinition> defMap = transformContact->detailDefinitions(error);
    delete transformContact;

    return defMap;
}

bool CntSymbianEngine::hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const
{
    bool returnValue(false);

    // TODO: update for SIM contacts later
    if (contactType != QContactType::TypeContact && contactType != QContactType::TypeGroup)
        return false;

    switch (feature) {
        /* TODO: case QContactManager::Groups to be implemented.
           How about the others? like:
           QContactManager::ActionPreferences,
           QContactManager::MutableDefinitions,
           QContactManager::Anonymous? */
    case QContactManager::Groups:
    case QContactManager::Relationships:
    case QContactManager::SelfContact: {
        returnValue = true;
        break;
    }

    default:
        returnValue = false;
    }

    return returnValue;
}

bool CntSymbianEngine::filterSupported(const QContactFilter& filter) const
{
    TBool result;

    // Map filter support into a boolean value
    CntAbstractContactFilter::FilterSupport filterSupport = d->filterSupported(filter);
    if (filterSupport == CntAbstractContactFilter::Supported
        || filterSupport == CntAbstractContactFilter::SupportedPreFilterOnly) {
        result = true;
    } else {
        result = false;
    }

    return result;
}

/* Synthesise the display label of a contact */
QString CntSymbianEngine::synthesizeDisplayLabel(const QContact& contact, QContactManager::Error& /*error*/) const
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

bool CntSymbianEngine::setSelfContactId(const QContactLocalId& contactId, QContactManager::Error& error)
{
    return d->setSelfContactId(contactId, error);
}

QContactLocalId CntSymbianEngine::selfContactId(QContactManager::Error& error) const
{
    return d->selfContactId(error);
}

/*!
 * Returns the list of data types supported by the Symbian S60 engine
 */
QList<QVariant::Type> CntSymbianEngine::supportedDataTypes() const
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
void CntSymbianEngine::eventContactAdded(const QContactLocalId &contactId)
{
    QList<QContactLocalId> contactList;
	contactList.append(contactId);

	emit contactsAdded(contactList);
}

/*!
 * Private slot to receive events about deleted entries.
 *
 * \param contactId ID for the deleted contact item.
 */
void CntSymbianEngine::eventContactRemoved(const QContactLocalId &contactId)
{
    QList<QContactLocalId> contactList;
	contactList.append(contactId);

	emit contactsRemoved(contactList);
}

/*!
 * Private slot to receive events about modified contact items.
 *
 * \param ID for the contact entry with modified data.
 */
void CntSymbianEngine::eventContactChanged(const QContactLocalId &contactId)
{
    QList<QContactLocalId> contactList;
	contactList.append(contactId);

	emit contactsChanged(contactList);
}

/*!
 * Private slot to receive events about added relationships.
 *
 * \param contactId the added contact
 */
void CntSymbianEngine::eventRelationshipAdded(const QContactLocalId &contactId)
{
    QList<QContactLocalId> contactList;
    contactList.append(contactId);

    emit relationshipsAdded(contactList);
}

/*!
 * Private slot to receive events about removed relationships.
 *
 * \param contactId the modified contact from relationship
 */
void CntSymbianEngine::eventRelationshipRemoved(const QContactLocalId &contactId)
{
    QList<QContactLocalId> contactList;
    contactList.append(contactId);

    emit relationshipsRemoved(contactList);
}

QString CntSymbianEngine::managerName() const
{
    return QString(CNT_SYMBIAN_MANAGER_NAME);
}

/* Factory lives here in the basement */
QContactManagerEngine* CntSymbianFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    return new CntSymbianEngine(parameters, error);
}

QString CntSymbianFactory::managerName() const
{
    return QString(CNT_SYMBIAN_MANAGER_NAME);
}

Q_EXPORT_PLUGIN2(mobapicontactspluginsymbian, CntSymbianFactory);
