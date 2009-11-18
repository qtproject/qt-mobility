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

#include "qtcontacts.h"

#include "trackerchangelistener.h"
#include "qtrackercontactsaverequest.h"

QContactManagerEngine* ContactTrackerFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    Q_UNUSED(error);
    return new QContactTrackerEngine(managerName(), 1, parameters);
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
    TrackerChangeListener *listener = new TrackerChangeListener(this);
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

QList<QContactLocalId> QContactTrackerEngine::contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{

    // TODO Implement sorting
    QList<QContactLocalId> ids;
    RDFVariable rdfContact = RDFVariable::fromType<nco::PersonContact>();
    if (filter.type() == QContactFilter::ChangeLogFilter) {
        const QContactChangeLogFilter& clFilter = static_cast<const QContactChangeLogFilter&>(filter);
        // Removed since
        if (clFilter.eventType() == QContactChangeLogFilter::EventRemoved) {
            error = QContactManager::NotSupportedError;
            return ids;
        }
        // Added since
        if (clFilter.eventType() == QContactChangeLogFilter::EventAdded) {
            rdfContact.property<nao::hasTag>().property<nao::prefLabel>() = LiteralValue("addressbook");
            rdfContact.property<nie::contentCreated>() >= LiteralValue(clFilter.since().toString(Qt::ISODate));
        }
        // Changed since
        else if (clFilter.eventType() == QContactChangeLogFilter::EventChanged) {
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

QList<QContactLocalId> QContactTrackerEngine::contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    Q_UNUSED(sortOrders)

    QList<QContactLocalId> ids;
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

QContact QContactTrackerEngine::contact(const QContactLocalId& contactId, QContactManager::Error& error ) const
{
    qWarning() << "QContactManager::contact()" << "api is not supported for tracker plugin. Please use asynchronous API QContactFetchRequest.";
    return contact_impl(contactId, error);
}
// used in tests, removed warning while decided if to provide sync api. Until then customers are advised to use async
QContact QContactTrackerEngine::contact_impl(const QContactLocalId& contactId, QContactManager::Error& error ) const
{
    // the rest of the code is for internal usage, unit tests etc.
    QContactLocalIdFilter idlist;
    QList<QContactLocalId> ids; ids << contactId;
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
            << QContactOrganization::DefinitionName
            << QContactPhoneNumber::DefinitionName
            << QContactOnlineAccount::DefinitionName
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
    while(t.elapsed() < msecs || msecs == 0) // 0 for infinite
    {
        usleep(10000);
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        if(req->isFinished())
            return true;
    }
    qDebug() << Q_FUNC_INFO << "not finished";
    return req->isFinished();

}

bool QContactTrackerEngine::saveContact( QContact* contact, QContactManager::Error& error)
{
    // Signal emitted from TrackerChangeListener
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
        return true;
    else
        return false;
}

bool QContactTrackerEngine::removeContact(const QContactLocalId& contactId, QContactManager::Error& error)
{
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

    return true;
}

QList<QContactManager::Error> QContactTrackerEngine::saveContacts(QList<QContact>* contacts,
                                                                  QContactManager::Error& error)
{
    // Signals emitted from TrackerChangeListener
    QList<QContactManager::Error> errorList;
    QContactManager::Error functionError = QContactManager::NoError;

    if(contacts == 0) {
        error = QContactManager::BadArgumentError;
        return errorList;
    }

    for(int i=0; i<contacts->count(); i++) {
        QContact contact = contacts->at(i);
        if(!saveContact(&contact, error)) {
            functionError = error;
            errorList.append(functionError);
        } else {
            // No error while saving.
            errorList.append(QContactManager::NoError);
            (*contacts)[i] = contact;
        }
    }

    error = functionError;      // Last operation error is the batch error.
    return errorList;
}

QList<QContactManager::Error> QContactTrackerEngine::removeContacts(QList<QContactLocalId>* contactIds, QContactManager::Error& error)
{
    QList<QContactManager::Error> errors;
    error = QContactManager::NoError;

    if (!contactIds) {
        error = QContactManager::BadArgumentError;
        return errors;
    }

    for (int i = 0; i < contactIds->count(); i++) {
        QContactManager::Error lastError;
        removeContact(contactIds->at(i), lastError);
        errors.append(lastError);
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

QMap<QString, QContactDetailDefinition> QContactTrackerEngine::detailDefinitions(const QString& contactType,
                                                                                 QContactManager::Error& error) const
{
    // lazy initialisation of schema definitions.
    if (d->m_definitions.isEmpty()) {
        // none in the list?  get the schema definitions, and modify them to match our capabilities.
        d->m_definitions = QContactManagerEngine::schemaDefinitions().value(QContactType::TypeContact);
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
        QMap<QString, QContactDetailDefinitionField> &fields(urlDef.fields());
        QContactDetailDefinitionField f;

        f.setDataType( QVariant::String );
        QVariantList subTypes;
        // removing social networking url
        subTypes << QString(QLatin1String(QContactUrl::SubTypeFavourite));
        subTypes << QString(QLatin1String(QContactUrl::SubTypeHomePage));
        f.setAllowableValues( subTypes );
        fields.insert(QContactUrl::FieldSubType, f);
        urlDef.setFields(fields);
        urlDef.setUnique(true);
        d->m_definitions.insert(QContactUrl::DefinitionName, urlDef);
    }

    error = QContactManager::NoError;
    return d->m_definitions;
}

/*!
 * \reimp
 */
bool QContactTrackerEngine::hasFeature(QContactManagerInfo::ManagerFeature feature) const
{
    switch (feature) {
        case QContactManagerInfo::Groups:
        case QContactManagerInfo::ActionPreferences:
        case QContactManagerInfo::Relationships:
            return true;
        case QContactManagerInfo::ArbitraryRelationshipTypes:
            return true;
        case QContactManagerInfo::MutableDefinitions:
            return true;
        case QContactManagerInfo::ChangeLogs:
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
bool QContactTrackerEngine::filterSupported(const QContactFilter& filter) const
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
 * Returns the implementation version of this engine
 */
int QContactTrackerEngine::implementationVersion() const
{
    return d->m_engineVersion;
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
        case QContactAbstractRequest::ContactLocalIdFetchRequest:
            request = new QTrackerContactIdFetchRequest(req, this);
            break;
        case QContactAbstractRequest::ContactFetchRequest:
            request = new QTrackerContactFetchRequest(req, this);
            break;
        case QContactAbstractRequest::ContactSaveRequest:
            request = new QTrackerContactSaveRequest(req, this);
            break;
        default:
            return false;
    }
    d->m_requests[req] = request;
    return true;
}

QString QContactTrackerEngine::synthesizeDisplayLabel(const QContact& contact, QContactManager::Error& error) const
{
    QString label = QContactManagerEngine::synthesizeDisplayLabel(contact, error);
    if( label.isEmpty() )
        return contact.detail<QContactNickname>().nickname();
    return label;
}
