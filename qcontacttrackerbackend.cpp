/* * This file is part of qtcontacts-tracker
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia.
 */

#include "qcontacttrackerbackend_p.h"

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nco.h>
#include <QtTracker/ontologies/nie.h>
#include <QtTracker/ontologies/nao.h>
#include <QRegExp>
#include <QDir>
#include <QFile>
#include <QSet>

#include "qcontact_p.h"
#include "qcontactgroup_p.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"


#include "trackerchangelistener.h"
#include "qtrackercontactsaverequest.h"

QContactManagerEngine* ContactTrackerFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    Q_UNUSED(error);
    return new QContactTrackerEngine(parameters);
}

QString ContactTrackerFactory::managerName() const
{
    return QString("tracker");
}
Q_EXPORT_PLUGIN2(qtcontacts_tracker, ContactTrackerFactory);

QContactTrackerEngine::QContactTrackerEngine(const QMap<QString, QString>& parameters)
    : d(new QContactTrackerEngineData),
    contactArchiveFile("removed"),
    contactArchiveDir(QDir::homePath()+"/.contacts")
{
    Q_UNUSED(parameters);
    connectToSignals();
}

QContactTrackerEngine::QContactTrackerEngine(const QContactTrackerEngine& other)
    : QContactManagerEngine(), d(other.d)
{
    Q_UNUSED(other);
    connectToSignals();
}

void QContactTrackerEngine::connectToSignals()
{
    TrackerChangeListener *listener = new TrackerChangeListener(this);
    connect(listener, SIGNAL(contactsAdded(const QList<QUniqueId>&)), SIGNAL(contactsAdded(const QList<QUniqueId>&)));
    connect(listener, SIGNAL(contactsChanged(const QList<QUniqueId>&)), SIGNAL(contactsChanged(const QList<QUniqueId>&)));
    connect(listener, SIGNAL(contactsRemoved(const QList<QUniqueId>&)), SIGNAL(contactsRemoved(const QList<QUniqueId>&)));
}

QContactTrackerEngine& QContactTrackerEngine::operator=(const QContactTrackerEngine& other)
{
    d = other.d;
    return *this;
}

QContactTrackerEngine::~QContactTrackerEngine()
{
}

QContactManagerEngine* QContactTrackerEngine::clone()
{
    // this engine allows sharing - so we increase the reference count.
    d->m_refCount.ref();
    return this;
}

void QContactTrackerEngine::deref()
{
    if (!d->m_refCount.deref())
        delete this;
}

QList<QUniqueId> QContactTrackerEngine::contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{

    // TODO Implement sorting
    QList<QUniqueId> ids;
    RDFVariable rdfContact = RDFVariable::fromType<nco::PersonContact>();
    if (filter.type() == QContactFilter::ChangeLog) {
        const QContactChangeLogFilter& clFilter = static_cast<const QContactChangeLogFilter&>(filter);
        // Removed since
        if (clFilter.changeType() == QContactChangeLogFilter::Removed) {
            error = QContactManager::NotSupportedError;
            return ids;
        }
        // Added since
        if (clFilter.changeType() == QContactChangeLogFilter::Added) {
            rdfContact.property<nao::hasTag>().property<nao::prefLabel>() = LiteralValue("addressbook");
            rdfContact.property<nie::contentCreated>() >= LiteralValue(clFilter.since().toString(Qt::ISODate));
        }
        // Changed since
        else if (clFilter.changeType() == QContactChangeLogFilter::Changed) {
            rdfContact.property<nao::hasTag>().property<nao::prefLabel>() = LiteralValue("addressbook");
            rdfContact.property<nie::contentLastModified>() >= LiteralValue(clFilter.since().toString(Qt::ISODate));
        }
    }
    RDFSelect query;
    query.addColumn("contact_uri", rdfContact);
    query.addColumn("contactId", rdfContact.property<nco::contactUID>());
    foreach (QContactSortOrder sort, sortOrders) {
        query.orderBy(contactDetail2Rdf(rdfContact, sort.detailDefinitionName(), sort.detailFieldName()),
                      sort.direction() == Qt::AscendingOrder);
    }
    LiveNodes ncoContacts = ::tracker()->modelQuery(query);
    for (int i = 0; i < ncoContacts->rowCount(); i++) {
        ids.append(ncoContacts->index(i, 1).data().toUInt());
    }

    error = QContactManager::NoError;
    return ids;
}

QList<QUniqueId> QContactTrackerEngine::contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    Q_UNUSED(sortOrders)

    QList<QUniqueId> ids;
    RDFVariable RDFContact = RDFVariable::fromType<nco::PersonContact>();
    RDFSelect query;

    query.addColumn("contact_uri", RDFContact);
    query.addColumn("contactId", RDFContact.property<nco::contactUID>());
    LiveNodes ncoContacts = ::tracker()->modelQuery(query);
    for(int i=0; i<ncoContacts->rowCount(); i++) {
        ids.append(ncoContacts->index(i, 1).data().toUInt());
    }

    error = QContactManager::NoError;
    return ids;
}

QContact QContactTrackerEngine::contact(const QUniqueId& contactId, QContactManager::Error& error ) const
{
    qWarning()<<"QContactManager::contact()"<<"api is not supported for tracker plugin. Please use asynchronous API QContactFetchRequest.";
    return contact_impl(contactId, error);
}
// used in tests, removed warning while decided if to provide sync api. Until then customers are advised to use async
QContact QContactTrackerEngine::contact_impl(const QUniqueId& contactId, QContactManager::Error& error ) const
{
    // the rest of the code is for internal usage, unit tests etc.
    QContactIdListFilter idlist;
    QList<QUniqueId> ids; ids << contactId;
    idlist.setIds(ids);
    QContactFetchRequest request;
    QStringList fields;

    fields << QContactAvatar::DefinitionName
            << QContactBirthday::DefinitionName
            << QContactAddress::DefinitionName
            << QContactEmailAddress::DefinitionName
            << QContactDisplayLabel::DefinitionName
            << QContactGender::DefinitionName
            << QContactAnniversary::DefinitionName
            << QContactName::DefinitionName
            << QContactOnlineAccount::DefinitionName
            << QContactOrganisation::DefinitionName
            << QContactPhoneNumber::DefinitionName
            << QContactPresence::DefinitionName
            << QContactUrl::DefinitionName;
    request.setDefinitionRestrictions(fields);
    request.setFilter(idlist);

    QContactTrackerEngine engine(*this);
    engine.startRequest(&request);
    // 10 seconds should be enough
    engine.waitForRequestFinished(&request, 10000);

    if( !request.isFinished() || request.contacts().size() == 0) {
        error = QContactManager::UnspecifiedError;
    }
    else {
        // leave the code for now while not all other code is fixed
        error = QContactManager::NoError;
        return request.contacts()[0];
    }

    return QContact();
}

bool QContactTrackerEngine::waitForRequestFinished(QContactAbstractRequest* req, int msecs)
{
    Q_ASSERT(req);
    if(!req->isActive())
    {
        return req->isFinished(); // might be already finished
    }
    QTime t;
    t.start();
    while(t.elapsed() < msecs || msecs != 0) // 0 for infinite
    {
        usleep(10000);
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        if(req->isFinished())
            return true;
    }
    qDebug()<<Q_FUNC_INFO<<"not finished";
    return req->isFinished();

}

bool QContactTrackerEngine::saveContact( QContact* contact,
                                         QSet<QUniqueId>& contactsAdded,
                                         QSet<QUniqueId>& contactsChanged,
                                         QSet<QUniqueId>& groupsChanged,
                                         QContactManager::Error& error)
{
    Q_UNUSED(groupsChanged)
    bool newOne = contact->id() == 0;
    QContactSaveRequest request;
    QList<QContact> contacts(QList<QContact>()<<*contact);
    request.setContacts(contacts);
    QContactTrackerEngine engine(*this);
    engine.startRequest(&request);
    // 10 seconds should be enough
    engine.waitForRequestFinished(&request, 10000);
    error = request.error();
    Q_ASSERT(request.contacts().size() == 1);
    *contact = request.contacts()[0];
    if( request.isFinished() && error == QContactManager::NoError)
    {
        if(newOne)
            contactsAdded << contact->id();
        else
            contactsChanged << contact->id();
        return true;
    }
    else
        return false;
}

bool QContactTrackerEngine::removeContact(const QUniqueId& contactId, QSet<QUniqueId>& contactsRemoved, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
    Q_UNUSED(groupsChanged)
    contactsRemoved.clear();
    error = QContactManager::NoError;

    // TODO: Do with LiveNodes when they support strict querying.
    RDFVariable RDFContact = RDFVariable::fromType<nco::PersonContact>();
    RDFContact.property<nco::contactUID>() = LiteralValue(QString::number(contactId));
    RDFSelect query;

    query.addColumn("contact_uri", RDFContact);
    LiveNodes ncoContacts = ::tracker()->modelQuery(query);
    if(ncoContacts->rowCount() == 0) {
        error = QContactManager::DoesNotExistError;
        return false;
    }


    Live< nco::PersonContact> ncoContact = ncoContacts->liveNode(0);
    LiveNodes contactMediums = ncoContact->getHasContactMediums();
    foreach(Live<nco::ContactMedium> media, contactMediums) {
        media->remove();
    }
    ncoContact->remove();

    contactsRemoved << contactId;
    return true;
}

QList<QContactManager::Error> QContactTrackerEngine::saveContacts(QList<QContact>* contacts, QSet<QUniqueId>& contactsAdded, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
    QList<QContactManager::Error> errorList;
    QContactManager::Error functionError = QContactManager::NoError;
    contactsAdded.clear();
    contactsChanged.clear();

    if(contacts == 0) {
        error = QContactManager::BadArgumentError;
        return QList<QContactManager::Error>();
    }

    for(int i=0; i<contacts->count(); i++) {
        QContact contact = contacts->at(i);

        QSet<QUniqueId> added4One;
        QSet<QUniqueId> changed4One;
        QSet<QUniqueId> changedGroup4One;

        if(!saveContact(&contact, added4One, changed4One, changedGroup4One, error)) {
            functionError = error;
            errorList.append(functionError);
        } else {
            // No error while saving.
            errorList.append(QContactManager::NoError);
            (*contacts)[i].setId(contact.id());
        }
        groupsChanged += changedGroup4One;
        contactsAdded += added4One;
        contactsChanged += changed4One;
    }

    error = functionError;      // Last operation error is the batch error.
/*
    // commented as it is fired from manager
    if(!addedList.isEmpty()) {
        emit contactsAdded(addedList);
    }

    if(!changedList.isEmpty()) {
        emit contactsChanged(changedList);
    }
*/
    return errorList;

}

QList<QContactManager::Error> QContactTrackerEngine::removeContacts(QList<QUniqueId>* contactIds, QSet<QUniqueId>& contactsRemoved, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
    groupsChanged.clear();
    contactsRemoved.clear();
    QList<QContactManager::Error> errors;
    error = QContactManager::NoError;

    if (!contactIds) {
        error = QContactManager::BadArgumentError;
        return errors;
    }

    for (int i = 0; i < contactIds->count(); i++) {
        QContactManager::Error lastError;
        QSet<QUniqueId> removedCs;
        QSet<QUniqueId> changedGs;
        removeContact(contactIds->at(i),removedCs, changedGs, lastError);
        errors.append(lastError);
        contactsRemoved += removedCs;
        groupsChanged += changedGs;
        if (lastError == QContactManager::NoError) {
            (*contactIds)[i] = 0;
        }
        else {
            error = lastError;
        }
    }

    // emit signals removed as they are fired from QContactManager
    return errors;
}

QList<QUniqueId> QContactTrackerEngine::groups() const
{
    return QList<QUniqueId>();
}

QContactGroup QContactTrackerEngine::group(const QUniqueId& groupId) const
{
    Q_UNUSED(groupId)
    return QContactGroup();
}

bool QContactTrackerEngine::saveGroup(QContactGroup* group)
{
    Q_UNUSED(group)
    return false;
}

bool QContactTrackerEngine::removeGroup(const QUniqueId& groupId)
{
    Q_UNUSED(groupId)

    return false;
}

QMap<QString, QContactDetailDefinition> QContactTrackerEngine::detailDefinitions(QContactManager::Error& error) const
{
    // lazy initialisation of schema definitions.
    if (d->m_definitions.isEmpty()) {
        // none in the list?  get the schema definitions, and modify them to match our capabilities.
        d->m_definitions = QContactManagerEngine::schemaDefinitions();
        {
            qDebug() << "the definitions";
            QList<QString> defs = d->m_definitions.keys();
            foreach(QString def,  defs)
            {
                qDebug() << def;
            }
        }
        // modification: name is unique
        QContactDetailDefinition nameDef = d->m_definitions.value(QContactName::DefinitionName);
        nameDef.setUnique(true);
        d->m_definitions.insert(QContactName::DefinitionName, nameDef);

        // modification: avatar is unique.
        QContactDetailDefinition avatarDef = d->m_definitions.value(QContactAvatar::DefinitionName);
        avatarDef.setUnique(true);
        d->m_definitions.insert(QContactAvatar::DefinitionName, avatarDef);

        // modification: url is unique.
        QContactDetailDefinition urlDef = d->m_definitions.value(QContactUrl::DefinitionName);
        QMap<QString, QContactDetailDefinition::Field> &fields(urlDef.fields());
        QContactDetailDefinition::Field f;

        f.dataType = QVariant::String;
        QVariantList subTypes;
        // removing social networking url
        subTypes << QString(QLatin1String(QContactUrl::SubTypeFavourite));
        subTypes << QString(QLatin1String(QContactUrl::SubTypeHomePage));
        f.allowableValues = subTypes;
        fields.insert(QContactUrl::FieldSubType, f);
        urlDef.setFields(fields);
        urlDef.setUnique(true);
        d->m_definitions.insert(QContactUrl::DefinitionName, urlDef);
    }

    error = QContactManager::NoError;
    return d->m_definitions;
}

QContactDetailDefinition QContactTrackerEngine::detailDefinition(const QString& definitionId, QContactManager::Error& error) const
{
    detailDefinitions(error); // just to populate the definitions if we haven't already.
    error = QContactManager::DoesNotExistError;
    if (d->m_definitions.contains(definitionId))
        error = QContactManager::NoError;
    return d->m_definitions.value(definitionId);
}

bool QContactTrackerEngine::saveDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error)
{
    Q_UNUSED(def)
    error = QContactManager::UnspecifiedError; // Not implemented yet;

    return false;
}

bool QContactTrackerEngine::removeDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error)
{
    Q_UNUSED(def)
    error = QContactManager::UnspecifiedError; // Not implemented yet;

    return false;
}


/*! Returns the capabilities of the in-memory engine. */
QStringList QContactTrackerEngine::capabilities() const
{
    // TODO: Check capabilities for Tracker backend.
    QStringList caplist;
    caplist << "Groups" << "Locking" << "Batch" << "ReadOnly" << "Filtering" << "Sorting" << "Preferences";
    // ie, doesn't support: Changelog, Volatile, Asynchronous.
    return caplist;
}

/*!
 * Returns a list of definition identifiers which are natively (fast) filterable
 * on the default backend store managed by the manager from which the capabilities object was accessed
 */
QStringList QContactTrackerEngine::fastFilterableDefinitions() const
{
    // TODO: Check definitions for Tracker backend.
    QStringList fastlist;
    fastlist << "Name::First" << "Name::Last" << "PhoneNumber::PhoneNumber" << "EmailAddress::EmailAddress";
    return fastlist;
}

/*!
 * Returns the list of data types supported by the Tracker engine
 */
QList<QVariant::Type> QContactTrackerEngine::supportedDataTypes() const
{
    // TODO: Check supported datatypes for Tracker backend.
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);

    return st;
}

RDFVariable QContactTrackerEngine::contactDetail2Rdf(const RDFVariable& rdfContact, const QString& definitionName,
                                                      const QString& fieldName) const
{
    if (definitionName == QContactName::DefinitionName) {
        if (fieldName == QContactName::FieldFirst) {
            return rdfContact.property<nco::nameGiven>();
        }
        else if (fieldName == QContactName::FieldLast) {
            return rdfContact.property<nco::nameFamily>();
        }
        else if (fieldName == QContactName::FieldMiddle) {
            return rdfContact.property<nco::nameAdditional>();
        }
        else if (fieldName == QContactName::FieldPrefix) {
            return rdfContact.property<nco::nameHonorificPrefix>();
        }
        else if (fieldName == QContactName::FieldSuffix) {
            return rdfContact.property<nco::nameHonorificSuffix>();
        }
        else if (fieldName == QContactNickname::FieldNickname) {
            return rdfContact.property<nco::nickname>();
        }
    }
    return RDFVariable();
}

#if 0
// Might not be needed but leaving here just in case.
QString QContactTrackerEngine::escaped(const QString& input) const
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
#endif



/*! \reimp */
void QContactTrackerEngine::requestDestroyed(QContactAbstractRequest* req)
{
    if( d->m_requests.contains(req) )
    {
        QTrackerContactAsyncRequest *request = d->m_requests.take(req);
        delete request;
    }
}

/*! \reimp */
bool QContactTrackerEngine::startRequest(QContactAbstractRequest* req)
{
    QTrackerContactAsyncRequest *request = 0;
    switch (req->type())
    {
        case QContactAbstractRequest::ContactIdFetch:
        case QContactAbstractRequest::ContactFetch:
            request = new QTrackerContactFetchRequest(req, this);
            break;
        case QContactAbstractRequest::ContactSave:
            request = new QTrackerContactSaveRequest(req, this);
            break;
        default:
            return false;
    }
    d->m_requests[req] = request;
    return true;
}
