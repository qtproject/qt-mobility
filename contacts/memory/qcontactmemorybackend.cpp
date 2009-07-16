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
QList<QUniqueId> QContactMemoryEngine::contacts(const QContactSortOrder& sortOrder, QContactManager::Error& error) const
{
    error = QContactManager::NoError;
    if (!sortOrder.isValid())
        return d->m_contactIds;

    // TODO: this needs to be done properly...
    QList<QUniqueId> sortedIds;
    QList<QContact> sortedContacts;
    for (int i = 0; i < d->m_contacts.size(); i++)
        QContactManagerEngine::addSorted(&sortedContacts, d->m_contacts.at(i), sortOrder);
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
bool QContactMemoryEngine::saveContact(QContact* contact, bool batch, QContactManager::Error& error)
{
    // ensure that the contact's details conform to their definitions
    if (!validateContact(*contact, error)) {
        error = QContactManager::InvalidDetailError;
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

        // Looks ok, so continue
        d->m_contacts.replace(index, *contact);
        error = QContactManager::NoError;
        if (!batch) {
            QList<QUniqueId> emitList;
            emitList.append(contact->id());
            emit contactsChanged(emitList);
        }
        return true;
    }

    /* We ignore read only details here - we may have provided some */

    // update the contact item - set its ID
    contact->setId(++d->m_nextContactId);

    // finally, add the contact to our internal lists and return
    d->m_contacts.append(*contact);             // add contact to list
    d->m_contactIds.append(contact->id());      // track the contact id.

    error = QContactManager::NoError;     // successful.

    // if we need to emit signals (ie, this isn't part of a batch operation)
    // then emit the correct one.
    if (!batch) {
        QList<QUniqueId> emitList;
        emitList.append(contact->id());
        emit contactsAdded(emitList);
    }

    return true;
}

/*! \reimp */
bool QContactMemoryEngine::removeContact(const QUniqueId& contactId, bool batch, QContactManager::Error& error)
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
        it.value().removeMember(contactId);
    }

    // if we need to emit signals (ie, this isn't part of a batch operation)
    // then emit the correct one.
    if (!batch) {
        QList<QUniqueId> emitList;
        emitList.append(contactId);
        emit contactsRemoved(emitList);
    }

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
bool QContactMemoryEngine::saveGroup(QContactGroup* group, QContactManager::Error& error)
{
    if (!group) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    if (group->name().isEmpty()) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    // if the group does not exist, generate a new group id for it.
    if (!d->m_groups.contains(group->id())) {
        group->setId(++d->m_nextGroupId);
    }

    // save it in the database
    d->m_groups.insert(group->id(), *group);
    error = QContactManager::NoError;
    return true;
}

/*! \reimp */
bool QContactMemoryEngine::removeGroup(const QUniqueId& groupId, QContactManager::Error& error)
{
    if (!d->m_groups.contains(groupId)) {
        error = QContactManager::DoesNotExistError;
        return false;
    }

    d->m_groups.remove(groupId);
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

