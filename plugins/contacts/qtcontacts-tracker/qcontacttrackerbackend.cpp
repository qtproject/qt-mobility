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

#include "qcontacttrackerbackend_p.h"

#include <QtTracker/Tracker>
#include <QtTracker/ontologies/nco.h>
#include <QtTracker/ontologies/nie.h>
#include <QtTracker/ontologies/nao.h>
#include <QRegExp>
#include <QDir>
#include <QFile>
#include <QSet>
#include <QList>

#include "qtcontacts.h"

#include "trackerchangelistener.h"
#include "qtrackercontactsaverequest.h"
#include <qtrackerrelationshipfetchrequest.h>
#include <qtrackerrelationshipsaverequest.h>
#include <qtrackercontactidfetchrequest.h>


QContactManagerEngine* ContactTrackerFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error* error)
{
    Q_UNUSED(error);
    QString version = QLatin1String(VERSION_INFO);
    return new QContactTrackerEngine(managerName(), version.toInt(), parameters);
}

QString ContactTrackerFactory::managerName() const
{
    return QString("tracker");
}
Q_EXPORT_PLUGIN2(qtcontacts_tracker, ContactTrackerFactory);

QContactTrackerEngine::QContactTrackerEngine(const QString& engineName, int engineVersion, const QMap<QString, QString>& parameters)
    : d(new QContactTrackerEngineData),
    contactArchiveFile("removed"),
    contactArchiveDir(QDir::homePath()+"/.contacts")
{
    Q_UNUSED(parameters);
    d->m_engineName = engineName;
    d->m_engineVersion = engineVersion;
    connectToSignals();
}

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
    TrackerChangeListener *listener = new TrackerChangeListener(this, this);
    connect(listener, SIGNAL(contactsAdded(const QList<QContactLocalId>&)), SIGNAL(contactsAdded(const QList<QContactLocalId>&)));
    connect(listener, SIGNAL(contactsChanged(const QList<QContactLocalId>&)), SIGNAL(contactsChanged(const QList<QContactLocalId>&)));
    connect(listener, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)), SIGNAL(contactsRemoved(const QList<QContactLocalId>&)));
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

QList<QContactLocalId> QContactTrackerEngine::contactIds(const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const
{
    return contactIds(QContactFilter(), sortOrders, error);
}

QList<QContactLocalId> QContactTrackerEngine::contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const
{
    QContactLocalIdFetchRequest request;
    request.setFilter(filter);
    request.setSorting(sortOrders);

    QContactTrackerEngine engine(*this);
    engine.startRequest(&request);
    // 10 seconds should be enough
    engine.waitForRequestFinished(&request, 10000);
    if(!request.isFinished()) {
        *error = QContactManager::UnspecifiedError;
    }
    else {
        // leave the code for now while not all other code is fixed
        *error = request.error();
    }
    return request.ids();
}

QList<QContact> QContactTrackerEngine::contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QContactFetchHint& fetchHint, QContactManager::Error* error) const
{
    // the rest of the code is for internal usage, unit tests etc.
    QContactFetchRequest request;
    request.setFetchHint(fetchHint);
    request.setFilter(filter);
    request.setSorting(sortOrders);

    QContactTrackerEngine engine(*this);
    engine.startRequest(&request);
    // 10 seconds should be enough
    engine.waitForRequestFinished(&request, 10000);

    if( !request.isFinished()) {
        *error = QContactManager::UnspecifiedError;
    }
    else {
        // leave the code for now while not all other code is fixed
        *error = request.error();
    }
    return request.contacts();
}

QContact QContactTrackerEngine::contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const
{
    // plan to keep this warning for a while - as message to customers using the API
    qWarning() << "QContactManager::contact()" << "api is blocking on dbus roundtrip while accessing tracker. Please, consider using asynchronous API QContactFetchRequest and not fetching contacts by id \n"
            "- reading 100 ids and 100 contact by ids is ~100 times slower then reading 100 contacts at once with QContactFetchRequest.";
    return contact_impl(contactId, fetchHint, error);
}

QContactLocalId QContactTrackerEngine::selfContactId(QContactManager::Error* error) const
{
    *error = QContactManager::NoError;
    return QContactLocalId(0xFFFFFFFF);
}

// used in tests, removed warning while decided if to provide sync api. Until then customers are advised to use async
QContact QContactTrackerEngine::contact_impl(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error ) const
{
    QContactLocalIdFilter idlist;
    QList<QContactLocalId> ids; ids << contactId;
    idlist.setIds(ids);
    QContactFetchRequest request;
    QStringList definitionNames = fetchHint.detailDefinitionsHint();
    if (fetchHint.detailDefinitionsHint().isEmpty())
    {
        definitionNames << QContactAvatar::DefinitionName
                << QContactBirthday::DefinitionName
                << QContactAddress::DefinitionName
                << QContactEmailAddress::DefinitionName
                << QContactDisplayLabel::DefinitionName
                << QContactGender::DefinitionName
                << QContactAnniversary::DefinitionName
                << QContactName::DefinitionName
                << QContactOnlineAccount::DefinitionName
                << QContactOrganization::DefinitionName
                << QContactPhoneNumber::DefinitionName
                << QContactOnlineAccount::DefinitionName
                << QContactUrl::DefinitionName;
    }

    QContactFetchHint modifiedHint;
    modifiedHint.setDetailDefinitionsHint(definitionNames);
    request.setFetchHint(modifiedHint);
    request.setFilter(idlist);

    QContactTrackerEngine engine(*this);
    engine.startRequest(&request);
    // 10 seconds should be enough
    engine.waitForRequestFinished(&request, 10000);

    if( !request.isFinished()) {
        *error = QContactManager::UnspecifiedError;
        return QContact();
    }
    else if(request.contacts().size() == 0)
    {
        *error = QContactManager::DoesNotExistError;
        return QContact();
    }
    else {
        // leave the code for now while not all other code is fixed
        *error = request.error();
        return request.contacts()[0];
    }

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
    while(t.elapsed() < msecs || msecs == 0) // 0 for infinite
    {
        usleep(10000);
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        if(req->isFinished())
            return true;
    }
    qDebug() << Q_FUNC_INFO <<"Status Finished" << req->isFinished();
    return req->isFinished();

}

bool QContactTrackerEngine::saveContact( QContact* contact, QContactManager::Error* error)
{
    // Signal emitted from TrackerChangeListener
    QContactSaveRequest request;
    QList<QContact> contacts(QList<QContact>()<<*contact);
    request.setContacts(contacts);
    QContactTrackerEngine engine(*this);
    engine.startRequest(&request);
    // 10 seconds should be enough
    engine.waitForRequestFinished(&request, 10000);
    *error = request.error();
    Q_ASSERT(request.contacts().size() == 1);
    *contact = request.contacts()[0];

    if( request.isFinished() && *error == QContactManager::NoError)
        return true;
    else
        return false;
}

bool QContactTrackerEngine::removeContact(const QContactLocalId& contactId, QContactManager::Error* error)
{
    *error = QContactManager::NoError;

    // TODO: Do with LiveNodes when they support strict querying.
    RDFVariable RDFContact = RDFVariable::fromType<nco::PersonContact>();
    RDFContact.property<nco::contactUID>() = LiteralValue(QString::number(contactId));
    RDFSelect query;

    query.addColumn("contact_uri", RDFContact);
    LiveNodes ncoContacts = ::tracker()->modelQuery(query);
    if(ncoContacts->rowCount() == 0) {
        *error = QContactManager::DoesNotExistError;
        return false;
    }


    Live< nco::PersonContact> ncoContact = ncoContacts->liveNode(0);
    LiveNodes contactMediums = ncoContact->getHasContactMediums();
    foreach(Live<nco::ContactMedium> media, contactMediums) {
        media->remove();
    }
    ncoContact->remove();

    //Temporary workaround to get removed- signal
    QList<QContactLocalId> removed;
    removed << contactId;
    emit contactsRemoved(removed);

    return true;
}

bool QContactTrackerEngine::saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    // @todo: Handle errors per saved contact.
    Q_UNUSED(errorMap)

    *error = QContactManager::NoError;

    if(contacts == 0) {
        *error = QContactManager::BadArgumentError;
        return false;
    }

    // Signal emitted from TrackerChangeListener
    QContactSaveRequest request;
    QList<QContact> contactList;
    for (int i = 0; i < contacts->size(); ++i) {
        contactList.append(contacts->at(i));
    }
    request.setContacts(contactList);
    QContactTrackerEngine engine(*this);
    engine.startRequest(&request);
    /// @todo what should be the delay
    engine.waitForRequestFinished(&request, 1000*contacts->size());
    /// @todo what should we do in case request.isFinished() == false
    if (request.isFinished() == false) {
        qWarning() << "QContactTrackerEngine::saveContacts:" << "request not finished";
    }
    *error = request.error();
    for (int i = 0; i < contacts->size(); ++i) {
        (*contacts)[i] = request.contacts().at(i);
    }

    // Returns false if we have any errors - true if everything went ok.
    return (request.errorMap().isEmpty() && *error == QContactManager::NoError);
}

bool QContactTrackerEngine::removeContacts(const QList<QContactLocalId>& contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    // Cannot report errors - giving up.
    if(!errorMap) {
        *error = QContactManager::BadArgumentError;
        return false;
    }

    // let's clear the error hash so there is nothing old haunting us.
    errorMap->clear();

    for (int i = 0; i < contactIds.count(); i++) {
        QContactManager::Error lastError;
        removeContact(contactIds.at(i), &lastError);
        if (lastError != QContactManager::NoError) {
            errorMap->insert(i, lastError);
        }
    }

    // Returns true if no errors were encountered - false if there was errors.
    // emit signals removed as they are fired from QContactManager
    return (errorMap->isEmpty());
}

QMap<QString, QContactDetailDefinition> QContactTrackerEngine::detailDefinitions(const QString& contactType,
                                                                                 QContactManager::Error* error) const
{
    if (contactType != QContactType::TypeContact) {
        *error = QContactManager::InvalidContactTypeError;
        return QMap<QString, QContactDetailDefinition>();
    }

    // lazy initialisation of schema definitions.
    if (d->m_definitions.isEmpty()) {
        // none in the list?  get the schema definitions, and modify them to match our capabilities.
        d->m_definitions = QContactManagerEngine::schemaDefinitions().value(QContactType::TypeContact);

        // modification: name is unique
        QContactDetailDefinition nameDef = d->m_definitions.value(QContactName::DefinitionName);
        nameDef.setUnique(true);
        d->m_definitions.insert(QContactName::DefinitionName, nameDef);

        // modification: avatar is unique.
        QContactDetailDefinition avatarDef = d->m_definitions.value(QContactAvatar::DefinitionName);
        avatarDef.setUnique(true);
        d->m_definitions.insert(QContactAvatar::DefinitionName, avatarDef);

        // modification: url is unique.
        {
            const QContactDetailDefinition urlDef = d->m_definitions.value(
                    QContactUrl::DefinitionName);
            QContactDetailDefinition newUrlDef;

            QMap<QString, QContactDetailFieldDefinition> urlFieldNames = urlDef.fields();
            QMap<QString, QContactDetailFieldDefinition> &fields(urlFieldNames);
            QContactDetailFieldDefinition f;

            f.setDataType(QVariant::String);
            QVariantList subTypes;
            // removing social networking url
            subTypes << QString(QLatin1String(QContactUrl::SubTypeFavorite));
            subTypes << QString(QLatin1String(QContactUrl::SubTypeHomePage));
            f.setAllowableValues(subTypes);
            fields.insert(QContactUrl::FieldSubType, f);
            newUrlDef.setFields(fields);
            newUrlDef.setUnique(true);
            newUrlDef.setName(QContactUrl::DefinitionName);
            d->m_definitions.insert(QContactUrl::DefinitionName, newUrlDef);
        }

        // QContactOnlineAccount custom fields
        {
            const QContactDetailDefinition accDef = d->m_definitions.value(QContactOnlineAccount::DefinitionName);
            QContactDetailDefinition newAccountDefinition;

            QMap<QString, QContactDetailFieldDefinition> accountFieldName = accDef.fields();
            QMap<QString, QContactDetailFieldDefinition> &fields(accountFieldName);
            QContactDetailFieldDefinition f;

            f.setDataType(QVariant::String);
            fields.insert("Account", f);
            fields.insert("AccountPath", f);
            newAccountDefinition.setFields(fields);
            newAccountDefinition.setName(QContactOnlineAccount::DefinitionName);
            d->m_definitions.insert(QContactOnlineAccount::DefinitionName, newAccountDefinition);
        }
    }

    *error = QContactManager::NoError;
    return d->m_definitions;
}

/*!
 * \reimp
 */
bool QContactTrackerEngine::hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const
{
    if (!supportedContactTypes().contains(contactType)) {
        return false;
    }

    switch (feature) {
        case QContactManager::Groups:
        case QContactManager::ActionPreferences:
        case QContactManager::Relationships:
        case QContactManager::SelfContact:
            return true;
        case QContactManager::ArbitraryRelationshipTypes:
            return true;
        case QContactManager::MutableDefinitions:
            return true;
        case QContactManager::ChangeLogs:
            return true;
        default:
            return false;
    }
}


/*!
 * \reimp
 */
/*!
 * Definition identifiers which are natively (fast) filterable
 * on the default backend store managed by the manager from which the capabilities object was accessed
 */
bool QContactTrackerEngine::isFilterSupported(const QContactFilter& filter) const
{
    switch (filter.type()) {
        case QContactFilter::InvalidFilter:
        case QContactFilter::DefaultFilter:
        case QContactFilter::LocalIdFilter:
        case QContactFilter::ContactDetailFilter:
        case QContactFilter::ContactDetailRangeFilter:
        case QContactFilter::ActionFilter:
        case QContactFilter::ChangeLogFilter:
        case QContactFilter::RelationshipFilter:

// not yet done
//        case QContactFilter::IntersectionFilter:
//        case QContactFilter::UnionFilter:
            return true;
        default:
            return false;
    }
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

/*!
 * Returns the name of the Tracker engine
 */
QString QContactTrackerEngine::managerName() const
{
    return d->m_engineName;
}

/*!
 * Returns the manager version of this engine
 */
int QContactTrackerEngine::managerVersion() const
{
    return d->m_engineVersion;
}

RDFVariable QContactTrackerEngine::contactDetail2Rdf(const RDFVariable& rdfContact, const QString& definitionName,
                                                      const QString& fieldName) const
{
    if (definitionName == QContactName::DefinitionName) {
        if (fieldName == QContactName::FieldFirstName) {
            return rdfContact.property<nco::nameGiven>();
        }
        else if (fieldName == QContactName::FieldLastName) {
            return rdfContact.property<nco::nameFamily>();
        }
        else if (fieldName == QContactName::FieldMiddleName) {
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
        case QContactAbstractRequest::ContactLocalIdFetchRequest:
            request = new QTrackerContactIdFetchRequest(req, this);
            break;
        case QContactAbstractRequest::ContactFetchRequest:
            request = new QTrackerContactFetchRequest(req, this);
            break;
        case QContactAbstractRequest::ContactSaveRequest:
            request = new QTrackerContactSaveRequest(req, this);
            break;
        case QContactAbstractRequest::RelationshipFetchRequest:
            request = new QTrackerRelationshipFetchRequest(req, this);
            break;
        case QContactAbstractRequest::RelationshipSaveRequest:
            request = new QTrackerRelationshipSaveRequest(req, this);
            break;
        default:
            return false;
    }
    d->m_requests[req] = request;
    return true;
}

/*! \reimp */
QString QContactTrackerEngine::synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const
{
    QString label = QContactManagerEngine::synthesizedDisplayLabel(contact, error);
    if (label.isEmpty())
        label = contact.detail<QContactNickname>().nickname();
    //XXX TODO: FIXME - take the nickname from the presence field associated with the online account
    //if(label.isEmpty())
    //    label = contact.detail<QContactOnlineAccount>().nickname();

    qDebug() << Q_FUNC_INFO << label;
    return label;
}
