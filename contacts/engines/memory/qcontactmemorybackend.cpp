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

#include "qcontactmanager.h"

#include "qcontact_p.h"
#include "qcontactgroup_p.h"
#include "qcontactdetaildefinition.h"
#include "qcontactmanager_p.h"
#include "qcontactmanagerengine.h"

#include "qcontactmemorybackend_p.h"

#include "qcontactabstractrequest.h"
#include "qcontactabstractrequestresult.h"
#include "qcontactrequests.h"

#include <QTimer>
#include <QUuid>
#include <QSharedData>

/*!
 * \class QContactMemoryEngine
 * \brief This class provides an in-memory implementation of a contacts backend.
 *
 * It may be used as a reference implementation, or when persistent storage is not required.
 *
 * During construction, it will load the in-memory data associated with the memory store
 * identified by the "id" parameter from the given parameters if it exists, or a new,
 * anonymous store if it does not.
 *
 * Data stored in this engine is only available in the current process.
 *
 * This engine supports sharing, so an internal reference count is increased
 * whenever a manager uses this backend, and is decreased when the manager
 * no longer requires this engine.
 */

/* static data for manager class */
QMap<QString, QContactMemoryEngine*> QContactMemoryEngine::engines;

/*!
 * Factory function for creating a new in-memory backend, based
 * on the given \a parameters.
 *
 * The same engine will be returned for multiple calls with the
 * same value for the "id" parameter, while one of them is in scope.
 */
QContactMemoryEngine* QContactMemoryEngine::createMemoryEngine(const QMap<QString, QString>& parameters)
{
    QString idValue = parameters.value(QLatin1String("id"));
    if (idValue.isNull() || idValue.isEmpty()) {
        // no store given?  new, anonymous store.
        idValue = QUuid::createUuid().toString();
    }

    if (engines.contains(idValue)) {
        QContactMemoryEngine *engine = engines.value(idValue);
        engine->d->m_refCount.ref();
        return engine;
    } else {
        QContactMemoryEngine *engine = new QContactMemoryEngine(parameters);
        engine->d->m_id = idValue;
        engines.insert(idValue, engine);
        return engine;
    }
}

/*!
 * Constructs a new in-memory backend.
 *
 * Loads the in-memory data associated with the memory store identified by the "id" parameter
 * from the given \a parameters if it exists, or a new, anonymous store if it does not.
 */
QContactMemoryEngine::QContactMemoryEngine(const QMap<QString, QString>& parameters)
    : d(new QContactMemoryEngineData)
{
    Q_UNUSED(parameters);
}

/*! \reimp */
void QContactMemoryEngine::deref()
{
    if (!d->m_refCount.deref()) {
        engines.remove(d->m_id);
        delete d;
        delete this;
    }
}

/*! \reimp */
QList<QUniqueId> QContactMemoryEngine::contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    // TODO: this needs to be done properly...
    error = QContactManager::NoError;
    QList<QUniqueId> sortedIds;
    QList<QContact> sortedContacts;
    for (int i = 0; i < d->m_contacts.size(); i++)
        QContactManagerEngine::addSorted(&sortedContacts, d->m_contacts.at(i), sortOrders);
    for (int i = 0; i < sortedContacts.size(); i++)
        sortedIds.append(sortedContacts.at(i).id());
    return sortedIds;
}

/*! \reimp */
QContact QContactMemoryEngine::contact(const QUniqueId& contactId, QContactManager::Error& error) const
{
    int index = d->m_contactIds.indexOf(contactId);
    if (index != -1) {
        error = QContactManager::NoError;
        QContact retn = d->m_contacts.at(index);
        QContactDisplayLabel dl = retn.detail(QContactDisplayLabel::DefinitionName);
        if (dl.label().isEmpty()) {
            QContactManager::Error synthError;
            dl.setLabel(synthesiseDisplayLabel(retn, synthError));
            dl.setSynthesised(true);
            retn.saveDetail(&dl);
        }

        return retn;
    }

    error = QContactManager::DoesNotExistError;
    return QContact();
}

/*! \reimp */
bool QContactMemoryEngine::saveContact(QContact* contact, QSet<QUniqueId>& contactsAdded, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
    // ensure that the contact's details conform to their definitions
    if (!validateContact(*contact, error)) {
        return false;
    }

    // check to see if this contact already exists
    int index = d->m_contactIds.indexOf(contact->id());
    if (index != -1) {
        /* We also need to check that there are no modified create only details */
        QContact oldContact = d->m_contacts.at(index);

        QSetIterator<QString> it(d->m_createOnlyIds);
        while (it.hasNext()) {
            const QString& id = it.next();
            QList<QContactDetail> details = oldContact.details(id);
            QList<QContactDetail> newDetails = contact->details(id);

            /* Any entries in old should still be in new */
            if (newDetails.count() < details.count()) {
                error = QContactManager::DetailAccessError;
                return false;
            }

            /* Now do a more detailed check */
            for (int i=0; i < details.count(); i++) {
                if (!newDetails.contains(details.at(i))) {
                    error = QContactManager::DetailAccessError;
                    return false;
                }
            }
        }

        // See what groups are no longer present...
        QSet<QUniqueId> oldGroups = d->m_contacts.at(index).groups().toSet();
        oldGroups.subtract(contact->groups().toSet());
        groupsChanged.unite(oldGroups);

        // Remove this contact from those groups
        QSetIterator<QUniqueId> git(oldGroups);

        while(git.hasNext()) {
            d->m_groups[git.next()].removeMember(contact->id());
        };

        QContactTimestamp ts = contact->detail(QContactTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        contact->saveDetail(&ts);

        // Looks ok, so continue
        d->m_contacts.replace(index, *contact);

        contactsChanged << contact->id();
    } else {
        /* New contact */
        QContactTimestamp ts = contact->detail(QContactTimestamp::DefinitionName);
        ts.setLastModified(QDateTime::currentDateTime());
        ts.setCreated(ts.lastModified());
        contact->saveDetail(&ts);

        // update the contact item - set its ID
        contact->setId(++d->m_nextContactId);

        // finally, add the contact to our internal lists and return
        d->m_contacts.append(*contact);             // add contact to list
        d->m_contactIds.append(contact->id());      // track the contact id.

        contactsAdded << contact->id();
    }

    error = QContactManager::NoError;     // successful.

    // now fix up groups that this contact has
    for (int i=0; i < contact->groups().size(); i++) {
        QUniqueId groupId = contact->groups().at(i);
        if (d->m_groups[groupId].addMember(contact->id())) {
            groupsChanged << groupId;
        }
    }

    return true;
}

/*! \reimp */
bool QContactMemoryEngine::removeContact(const QUniqueId& contactId, QSet<QUniqueId>& removedContacts, QSet<QUniqueId>& changedGroups, QContactManager::Error& error)
{
    int index = d->m_contactIds.indexOf(contactId);

    if (index == -1) {
        error = QContactManager::DoesNotExistError;
        return false;
    }

    // remove the contact from the lists.
    d->m_contacts.removeAt(index);
    d->m_contactIds.removeAt(index);
    error = QContactManager::NoError;

    // also remove it from any groups that we have
    QMutableMapIterator<QUniqueId, QContactGroup> it(d->m_groups);
    while (it.hasNext()) {
        it.next();
        if(it.value().removeMember(contactId))
            changedGroups << it.value().id();
    }

    removedContacts.insert(contactId);

    return true;
}

/*! \reimp */
QList<QUniqueId> QContactMemoryEngine::groups(QContactManager::Error& error) const
{
    error = QContactManager::NoError;
    return d->m_groups.keys();
}

/*! \reimp */
QContactGroup QContactMemoryEngine::group(const QUniqueId& groupId, QContactManager::Error& error) const
{
    error = QContactManager::NoError;
    if (!d->m_groups.contains(groupId))
        error = QContactManager::DoesNotExistError;

    return d->m_groups.value(groupId);
}

/*! \reimp */
bool QContactMemoryEngine::saveGroup(QContactGroup* group, QSet<QUniqueId>& groupsAdded, QSet<QUniqueId>& groupsChanged, QSet<QUniqueId>& contactsChanged, QContactManager::Error& error)
{
    if (!group) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    if (!validateGroup(*group, error)) {
        return false;
    }

    // if the group does not exist, generate a new group id for it.
    if (!d->m_groups.contains(group->id())) {
        group->setId(++d->m_nextGroupId);
        groupsAdded << group->id();
    } else {
        groupsChanged << group->id();
    }

    // save it in the database
    d->m_groups.insert(group->id(), *group);

    // and update any members with the new information
    for (int i=0; i < group->members().count(); i++) {
        int idx = d->m_contactIds.indexOf(group->members().value(i));
        QList<QUniqueId> groups = d->m_contacts[idx].groups();
        if (!groups.contains(group->id())) {
            groups.append(group->id());
            d->m_contacts[idx].setGroups(groups);
            contactsChanged << d->m_contacts[idx].id();
        }
    }

    error = QContactManager::NoError;
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::removeGroup(const QUniqueId& groupId, QSet<QUniqueId>& groupsRemoved, QSet<QUniqueId>& contactsChanged, QContactManager::Error& error)
{
    if (!d->m_groups.contains(groupId)) {
        error = QContactManager::DoesNotExistError;
        return false;
    }

    /* Update any contacts that this group has */
    QContactGroup g = d->m_groups.value(groupId);
    for (int i=0; i < g.members().count(); i++) {
        int idx = d->m_contactIds.indexOf(g.members().value(i));
        QList<QUniqueId> groups = d->m_contacts[idx].groups();
        groups.removeAll(groupId);
        d->m_contacts[idx].setGroups(groups);
        contactsChanged << d->m_contacts[idx].id();
    }

    d->m_groups.remove(groupId);
    groupsRemoved << groupId;
    error = QContactManager::NoError;
    return true;
}

/*! \reimp */
QMap<QString, QContactDetailDefinition> QContactMemoryEngine::detailDefinitions(QContactManager::Error& error) const
{
    // lazy initialisation of schema definitions.
    if (d->m_definitions.isEmpty()) {
        d->m_definitions = QContactManagerEngine::schemaDefinitions();

        // Extract create only definitions
        QMapIterator<QString, QContactDetailDefinition> it(d->m_definitions);
        while (it.hasNext()) {
            it.next();
            if (it.value().accessConstraint() == QContactDetailDefinition::CreateOnly)
                d->m_createOnlyIds.insert(it.key());
        }
    }

    error = QContactManager::NoError;
    return d->m_definitions;
}

/*! \reimp */
bool QContactMemoryEngine::saveDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error)
{
    if (!validateDefinition(def, error)) {
        return false;
    }
    detailDefinitions(error); // just to populate the definitions if we haven't already.
    d->m_definitions.insert(def.id(), def);
    if (def.accessConstraint() == QContactDetailDefinition::CreateOnly)
        d->m_createOnlyIds.insert(def.id());
    error = QContactManager::NoError;
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::removeDetailDefinition(const QString& definitionId, QContactManager::Error& error)
{
    if (definitionId.isEmpty()) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    detailDefinitions(error); // just to populate the definitions if we haven't already.
    bool success = d->m_definitions.remove(definitionId);
    d->m_createOnlyIds.remove(definitionId);
    if (success)
        error = QContactManager::NoError;
    else
        error = QContactManager::DoesNotExistError;
    return success;
}

/*!
 * \reimp
 */
void QContactMemoryEngine::destroyAsynchronousRequest(QContactAbstractRequest* req)
{
    if (req->status() != QContactAbstractRequest::Finished && req->status() != QContactAbstractRequest::Cancelled)
        return; // cannot destroy request if not cancelled or finished

    if (d->m_asynchronousRequests.contains(req)) {
        delete (d->m_asynchronousRequests.value(req));
        d->m_asynchronousRequests.remove(req);
    }
}

/*!
 * \reimp
 */
bool QContactMemoryEngine::asynchronousRequestWaitForFinished(QContactAbstractRequest* req, int msecs)
{
    Q_UNUSED(req);
    Q_UNUSED(msecs);
}

/*!
 * \reimp
 */
bool QContactMemoryEngine::asynchronousRequestWaitForProgress(QContactAbstractRequest* req, int msecs)
{
    Q_UNUSED(req);
    Q_UNUSED(msecs);
}

/*!
 * \reimp
 */
void QContactMemoryEngine::cancelAsynchronousRequest(QContactAbstractRequest* req)
{
    Q_UNUSED(req);
}

/*!
 * \reimp
 */
void QContactMemoryEngine::startAsynchronousRequest(QContactAbstractRequest* req, QContactAbstractRequest::Operation operation)
{
    // check to see that the request isn't already started
    if (req->status() == QContactAbstractRequest::Pending || req->status() == QContactAbstractRequest::Cancelling)
        return;

    // we can start the request.
    QContactAbstractRequestResult *requestResult;
    switch (req->type()) {
        case QContactAbstractRequest::Contact:
        {
            requestResult = new QContactRequestResult;
            QContactRequest *creq = static_cast<QContactRequest*>(req);
            static_cast<QContactRequestResult*>(requestResult)->updateRequest(creq, QContactAbstractRequest::Pending);
        }
        break;

        case QContactAbstractRequest::DetailDefinition:
        {
            requestResult = new QContactDetailDefinitionRequestResult;
            QContactDetailDefinitionRequest *dreq = static_cast<QContactDetailDefinitionRequest*>(req);
            static_cast<QContactDetailDefinitionRequestResult*>(requestResult)->updateRequest(dreq, QContactAbstractRequest::Pending);
        }
        break;

        case QContactAbstractRequest::Group:
        {
            requestResult = new QContactGroupRequestResult;
            QContactGroupRequest *greq = static_cast<QContactGroupRequest*>(req);
            static_cast<QContactGroupRequestResult*>(requestResult)->updateRequest(greq, QContactAbstractRequest::Pending);
        }
        break;

        default: // unknown request type
        return;
    }

    // clean up memory in use from previous operation
    if (d->m_asynchronousRequests.contains(req)) {
        delete d->m_asynchronousRequests.value(req);
    }

    // and start the new operation
    d->m_asynchronousRequests.insert(req, requestResult);
    d->m_asynchronousOperations.enqueue(QPair<QContactAbstractRequest*, QContactAbstractRequest::Operation>(req, operation));
    QTimer::singleShot(2000, this, SLOT(performAsynchronousOperation()));
}

/*!
 * This slot is called some time after an asynchronous request is started.
 * It performs the required operation, sets the result and returns.
 */
void QContactMemoryEngine::performAsynchronousOperation()
{
    QContactAbstractRequest *currentRequest;
    QContactAbstractRequest::Operation operation;

    // take the first pending, non-destroyed request and finish it
    while (true) {
        currentRequest = d->m_asynchronousOperations.head().first;
        operation = d->m_asynchronousOperations.dequeue().second;
        if (d->m_asynchronousRequests.contains(currentRequest))
            break;
        if (d->m_asynchronousOperations.isEmpty())
            return;
    }

    QSet<QUniqueId> contactsChanged;
    QSet<QUniqueId> contactsAdded;
    QSet<QUniqueId> groupsChanged;

    if (currentRequest->status() == QContactAbstractRequest::Pending) {
        switch (currentRequest->type()) {
            case QContactAbstractRequest::Contact:
            {
                QContactRequest *cr = static_cast<QContactRequest*>(currentRequest);
                QContactRequestResult *crr = static_cast<QContactRequestResult*>(d->m_asynchronousRequests.value(currentRequest));
                QContactManager::Error asynchronousError = QContactManager::UnspecifiedError;
                if (operation == QContactAbstractRequest::SaveOperation) {
                    // save
                    QList<QContact> selection = cr->contactSelection();
                    crr->setErrors(saveContacts(&selection, contactsAdded, contactsChanged, groupsChanged, asynchronousError));
                } else {
                    // retrieve or remove
                    QList<QContact> result;
                    QList<QUniqueId> translatedRequest;
                    asynchronousError = QContactManager::DoesNotExistError;
                    if (cr->selectionType() == QContactRequest::SelectByIds) {
                        translatedRequest = cr->idSelection();
                    } else if (cr->selectionType() == QContactRequest::SelectByFilter) {
                        QContactFilter fil = cr->filterSelection();
                        QList<QContactSortOrder> defaultSortOrder;
                        translatedRequest = contacts(fil, defaultSortOrder, asynchronousError);
                    } else if (cr->selectionType() == QContactRequest::SelectAll) {
                        translatedRequest = contacts(QList<QContactSortOrder>(), asynchronousError);
                    } else {
                        // invalid selection type...
                        asynchronousError = QContactManager::BadArgumentError;
                    }

                    if (operation == QContactAbstractRequest::RetrieveOperation) {
                        // retrieve the specified contacts
                        for (int i = 0; i < d->m_contacts.size(); i++) {
                            if (translatedRequest.contains(d->m_contacts.at(i).id())) {
                                result.append(d->m_contacts.at(i));
                                asynchronousError = QContactManager::NoError;
                            }
                        }
                        crr->setContacts(result);
                    } else {
                        // remove the specified contacts
                        crr->setErrors(removeContacts(&translatedRequest, contactsChanged, groupsChanged, asynchronousError));
                    }
                }

                crr->setError(asynchronousError);
                crr->updateRequest(cr, QContactAbstractRequest::Finished);
            }
            break;

            case QContactAbstractRequest::DetailDefinition:
            {
                QContactDetailDefinitionRequestResult *drr = static_cast<QContactDetailDefinitionRequestResult*>(d->m_asynchronousRequests.value(currentRequest));
                if (operation == QContactAbstractRequest::SaveOperation) {
                } else if (operation == QContactAbstractRequest::RetrieveOperation) {
                } else {
                }
            }
            break;

            case QContactAbstractRequest::Group:
            {
                QContactGroupRequestResult *grr = static_cast<QContactGroupRequestResult*>(d->m_asynchronousRequests.value(currentRequest));
                if (operation == QContactAbstractRequest::SaveOperation) {
                } else if (operation == QContactAbstractRequest::RetrieveOperation) {
                } else {
                }
            }
            break;

            default: // unknown request type...
            return;
        }
    }
}

/*!
 * \reimp
 */
bool QContactMemoryEngine::hasFeature(QContactManagerInfo::ManagerFeature feature) const
{
    switch (feature) {
        case QContactManagerInfo::Groups:
        case QContactManagerInfo::Batch:
        case QContactManagerInfo::ActionPreferences:
        case QContactManagerInfo::ReadOnlyDetails:
        case QContactManagerInfo::CreateOnlyDetails:
        case QContactManagerInfo::MutableDefinitions:
        case QContactManagerInfo::Synchronous:
            return true;
        default:
            return false;
    }
}

/*!
 * \reimp
 */
QList<QVariant::Type> QContactMemoryEngine::supportedDataTypes() const
{
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);
    st.append(QVariant::Time);
    st.append(QVariant::Bool);
    st.append(QVariant::Char);
    st.append(QVariant::Int);
    st.append(QVariant::UInt);
    st.append(QVariant::LongLong);
    st.append(QVariant::ULongLong);
    st.append(QVariant::Double);

    return st;
}

/*!
 * \reimp
 */
bool QContactMemoryEngine::filterSupported(const QContactFilter& filter) const
{
    Q_UNUSED(filter);
    // Until we add hashes for common stuff, fall back to slow code
    return false;
}

