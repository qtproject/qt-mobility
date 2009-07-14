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
#include "qcontactfilter.h"
#include "qcontactgroup_p.h"
#include "qcontactdetaildefinition.h"
#include "qcontactmanager_p.h"

#include "qcontactabstractaction.h"
#include "qcontactmanagerinfo.h"

#include <QSharedData>
#include <QPair>

/*!
 * \class QContactManager
 *
 * This class provides adding, updating and removal of contacts and groups.
 * It also provides definitions for fields that can be found in contacts.
 */

/*!
 * \fn QContactManager::contactsAdded(const QList<QUniqueId>& contactIds)
 * This signal is emitted at some point once the contacts identified by \a contactIds have been added to a datastore managed by this manager
 */

/*!
 * \fn QContactManager::contactsChanged(const QList<QUniqueId>& contactIds)
 * This signal is emitted at some point once the contacts identified by \a contactIds have been modified in a datastore managed by this manager
 */

/*!
 * \fn QContactManager::contactsRemoved(const QList<QUniqueId>& contactIds)
 * This signal is emitted at some point once the contacts identified by \a contactIds have been removed from a datastore managed by this manager
 */

/*!
 * \fn QContactManager::groupsAdded(const QList<QUniqueId>& groupIds)
 * This signal is emitted at some point once the groups identified by \a groupIds have been added to a datastore managed by this manager
 */

/*!
 * \fn QContactManager::groupsChanged(const QList<QUniqueId>& groupIds)
 * This signal is emitted at some point once the groups identified by \a groupIds have been modified in a datastore managed by this manager
 */

/*!
 * \fn QContactManager::groupsRemoved(const QList<QUniqueId>& groupIds)
 * This signal is emitted at some point once the groups identified by \a groupIds have been removed from a datastore managed by this manager
 */

/*!
    Returns a list of available manager ids that can be used when constructing
    a QContactManager.  If an empty id is specified to the constructor, the
    first value in this list will be used instead.
  */
QStringList QContactManager::availableManagers()
{
    QStringList ret;
    ret << QLatin1String("memory") << QLatin1String("invalid");
    QContactManagerData::loadFactories();
    ret.append(QContactManagerData::m_engines.keys());
    return ret;
}

/*!
 * Returns a list of identifiers of the available actions which are provided by the given \a vendor and of the given \a implementationVersion.
 * If \a vendor is empty, actions from all vendors and of any implementation version are returned; if \a implementationVersion is empty,
 * any actions from the given \a vendor (regardless of implementation version) are returned.
 */
QStringList QContactManager::availableActions(const QString& vendor, int implementationVersion)
{
    // SLOW naive implementation...
    QStringList ret;
    QList<QContactAbstractAction*> actionImpls = QContactManagerData::actions(QString(), vendor, implementationVersion);
    for (int i = 0; i < actionImpls.size(); i++) {
        QContactAbstractAction* actionImpl = actionImpls.at(i);
        QString currAction = actionImpl->actionName();
        if (!ret.contains(currAction)) {
            ret.append(currAction);
        }

        // we took ownership; clean up.
        // TODO: fix this.  currently, we don't take ownership, so don't delete.
        //delete actionImpl;
    }

    return ret;
}

/*!
 * Returns a list of QContactAbstractAction instances which implement the given \a actionName and is provided by the
 * given \a vendor and is of the given \a implementationVersion.  If \a actionName is empty,
 * implementations of all actions are returned; if \a vendor is empty, implementations provided by any vendor and
 * of any implementation version are returned; if \a implementationVersion is empty, any implementations provided by the
 * given \a vendor of the given \a actionName are returned.
 *
 * The caller DOES NOT take ownership of each instance returned in the list, and MUST NOT delete them when finished using them
 * to avoid leaking memory as this will result in undefined behaviour.
 *
 * TODO: fix this.
 */
QList<QContactAbstractAction*> QContactManager::actions(const QString& actionName, const QString& vendor, int implementationVersion)
{
    // the caller takes ownership
    return QContactManagerData::actions(actionName, vendor, implementationVersion);
}


/*! Splits the given \a uri into the manager, store, and parameters that it describes, and places the information into the memory addressed by \a pManagerId and \a pParams respectively.  Returns true if \a uri could be split successfully, otherwise returns false */
bool QContactManager::splitUri(const QString& uri, QString* pManagerId, QMap<QString, QString>* pParams)
{
    // Format: qtcontacts:<managerid>:<key>=<value>&<key>=<value>
    // 1) parameters are currently a qstringlist.. should they be a map?
    // 2) is the uri going to be escaped?  my guess would be "probably not"
    // 3) hence, do we assume that the prefix, managerid and storeid cannot contain `:'
    // 4) similarly, that neither keys nor values can contain `=' or `&'

    QStringList colonSplit = uri.split(QLatin1Char(':'));
    QString prefix = colonSplit.value(0);

    if (prefix != QLatin1String("qtcontacts"))
        return false;

    QString managerName = colonSplit.value(1);

    if (managerName.trimmed().isEmpty())
        return false;

    QString firstParts = prefix + QLatin1Char(':') + managerName + QLatin1Char(':');
    QString paramString = uri.mid(firstParts.length());

    QMap<QString, QString> outParams;

    // Now we have to decode each parameter
    if (!paramString.isEmpty()) {
        QStringList params = paramString.split(QRegExp(QLatin1String("&(?!(amp;|equ;))")), QString::KeepEmptyParts);
        // If we have an empty string for paramstring, we get one entry in params,
        // so skip that case.
        for(int i = 0; i < params.count(); i++) {
            /* This should be something like "foo&amp;bar&equ;=grob&amp;" */
            QStringList paramChunk = params.value(i).split(QLatin1String("="), QString::KeepEmptyParts);

            if (paramChunk.count() != 2)
                return false;

            QString arg = paramChunk.value(0);
            QString param = paramChunk.value(1);
            arg.replace(QLatin1String("&equ;"), QLatin1String("="));
            arg.replace(QLatin1String("&amp;"), QLatin1String("&"));
            param.replace(QLatin1String("&equ;"), QLatin1String("="));
            param.replace(QLatin1String("&amp;"), QLatin1String("&"));
            if (arg.isEmpty())
                return false;
            outParams.insert(arg, param);
        }
    }

    if (pParams)
        *pParams = outParams;
    if (pManagerId)
        *pManagerId = managerName;
    return true;
}

/*! Returns a URI that completely describes a manager implementation, datastore, and the parameters with which to instantiate the manager, from the given \a managerName and \a params */
QString QContactManager::buildUri(const QString& managerName, const QMap<QString, QString>& params)
{
    QString ret(QLatin1String("qtcontacts:%1:%2"));
    // we have to escape each param
    QStringList escapedParams;
    QStringList keys = params.keys();
    for (int i=0; i < keys.size(); i++) {
        QString key = keys.at(i);
        QString arg = params.value(key);
        arg = arg.replace(QLatin1Char('&'), QLatin1String("&amp;"));
        arg = arg.replace(QLatin1Char('='), QLatin1String("&equ;"));
        key = key.replace(QLatin1Char('&'), QLatin1String("&amp;"));
        key = key.replace(QLatin1Char('='), QLatin1String("&equ;"));
        key = key + QLatin1Char('=') + arg;
        escapedParams.append(key);
    }
    return ret.arg(managerName, escapedParams.join(QLatin1String("&")));
}

/*!
 * Constructs a QContactManager whose implementation, store and parameters are specified in the given \a storeUri,
 * and whose parent object is \a parent.
 */
QContactManager* QContactManager::fromUri(const QString& storeUri, QObject* parent)
{
    if (storeUri.isEmpty()) {
        return new QContactManager(QString(), QMap<QString, QString>(), parent);
    } else {
        QString id;
        QMap<QString, QString> parameters;
        if (splitUri(storeUri, &id, &parameters)) {
            return new QContactManager(id, parameters, parent);
        } else {
            // invalid
            return new QContactManager(QLatin1String("invalid"), QMap<QString, QString>(), parent);
        }
    }
}

/*!
 * Constructs a QContactManager whose implementation is identified by \a managerName with the given \a parameters.
 *
 * The \a parent QObject will be used as the parent of this QContactManager.
 *
 * If an empty \a managerName is specified, the default implementation for the platform will
 * be used.
 */
QContactManager::QContactManager(const QString& managerName, const QMap<QString, QString>& parameters, QObject* parent)
    : QObject(parent),
    d(new QContactManagerData)
{
    d->createEngine(managerName, parameters);
    connect(d->m_engine, SIGNAL(contactsAdded(QList<QUniqueId>)), this, SIGNAL(contactsAdded(QList<QUniqueId>)));
    connect(d->m_engine, SIGNAL(contactsChanged(QList<QUniqueId>)), this, SIGNAL(contactsChanged(QList<QUniqueId>)));
    connect(d->m_engine, SIGNAL(contactsRemoved(QList<QUniqueId>)), this, SIGNAL(contactsRemoved(QList<QUniqueId>)));
    connect(d->m_engine, SIGNAL(groupsAdded(QList<QUniqueId>)), this, SIGNAL(groupsAdded(QList<QUniqueId>)));
    connect(d->m_engine, SIGNAL(groupsChanged(QList<QUniqueId>)), this, SIGNAL(groupsChanged(QList<QUniqueId>)));
    connect(d->m_engine, SIGNAL(groupsRemoved(QList<QUniqueId>)), this, SIGNAL(groupsRemoved(QList<QUniqueId>)));
}

/*! Frees the memory used by the QContactManager */
QContactManager::~QContactManager()
{
    delete d->m_info;
    delete d;
}

/*!
 * \enum QContactManager::Error
 *
 * This enum specifies an error that occurred during the most recent operation:
 *
 * \value NoError The most recent operation was successful
 * \value DoesNotExistError The most recent operation failed because the requested contact, group or definition does not exist
 * \value AlreadyExistsError The most recent operation failed because the specified contact, group or definition already exists
 * \value InvalidDetailError The most recent operation failed because the specified contact contains details which do not conform to their definition
 * \value LockedError The most recent operation failed because the datastore specified is currently locked
 * \value DetailAccessError The most recent operation failed because a detail was modified or removed and its access method does not allow that
 * \value PermissionsError The most recent operation failed because the caller does not have permission to perform the operation
 * \value OutOfMemoryError The most recent operation failed due to running out of memory
 * \value NotSupportedError The most recent operation failed because the requested operation is not supported in the specified store
 * \value BadArgumentError The most recent operation failed because one or more of the parameters to the operation were invalid
 * \value UnspecifiedError The most recent operation failed for an undocumented reason
 */

/*! Return the error code of the most recent operation */
QContactManager::Error QContactManager::error() const
{
    return d->m_error;
}

/*! Return the list of added contact ids, sorted according to the given \a sortOrder */
QList<QUniqueId> QContactManager::contacts(const QContactSortOrder& sortOrder) const
{
    return d->m_engine->contacts(sortOrder, d->m_error);
}

/*!
 * Returns a list of contact ids that match the given \a filter, sorted according to the given \a sortOrder.
 *
 * Depending on the backend, this filtering operation may involve retrieving all the
 * contacts.
 */
QList<QUniqueId> QContactManager::contacts(const QContactFilter &filter, const QContactSortOrder& sortOrder) const
{
    return d->m_engine->contacts(filter, sortOrder, d->m_error);
}

/*! Returns the contact in the database identified by \a contactId */
QContact QContactManager::contact(const QUniqueId& contactId) const
{
    return d->m_engine->contact(contactId, d->m_error);
}

/*!
 * Add the given \a contact to the database if it is a new contact,
 * or updates the existing contact. Returns false on failure, or true on
 * success.  If successful and the contact was a new contact, its UID will
 * be set to a new, valid id.
 *
 * If the \a contact contains one or more details whose definitions have
 * not yet been saved with the manager, the operation will fail and the
 * manager will return \c QContactManager::UnsupportedError.
 */
bool QContactManager::saveContact(QContact* contact)
{
    return d->m_engine->saveContact(contact, false, d->m_error);
}

/*!
 * Remove the contact identified by \a contactId from the database.
 * Returns true if the contact was removed successfully, otherwise
 * returns false.
 */
bool QContactManager::removeContact(const QUniqueId& contactId)
{
    return d->m_engine->removeContact(contactId, false, d->m_error);
}

/*!
 * Adds the list of contacts given by \a contactList to the database.
 * Returns a list of the error codes corresponding to the contacts in
 * the \a contactList.  The \l QContactManager::error() function will
 * only return \c QContactManager::NoError if all contacts were saved
 * successfully.
 *
 * For each newly saved contact that was successful, the uid of the contact
 * in the list will be updated with the new value.  If a failure occurs
 * when saving a new contact, the id will be cleared.  If a failure occurs
 * when updating a contact that already exists, then TODO.
 *
 * \sa QContactManager::saveContact()
 */
QList<QContactManager::Error> QContactManager::saveContacts(QList<QContact>* contactList)
{
    return d->m_engine->saveContacts(contactList, d->m_error);
}

/*!
 * Remove the list of contacts identified in \a idList.
 * Returns a list of the error codes corresponding to the contact ids in
 * the \a idList.  The \l QContactManager::error() function will
 * only return \c QContactManager::NoError if all contacts were removed
 * successfully.
 *
 * For each contact that was removed succesfully, the corresponding
 * id in the list will be retained but set to zero.  The id of contacts
 * that were not successfully removed will be left alone.
 *
 * \sa QContactManager::removeContact()
 */
QList<QContactManager::Error> QContactManager::removeContacts(QList<QUniqueId>* idList)
{
    return d->m_engine->removeContacts(idList, d->m_error);
}

/*! Returns a display label for a \a contact which is synthesised from its details in a platform-specific manner */
QString QContactManager::synthesiseDisplayLabel(const QContact& contact) const
{
    return d->m_engine->synthesiseDisplayLabel(contact, d->m_error);
}

/*! Return the list of added group ids */
QList<QUniqueId> QContactManager::groups() const
{
    return d->m_engine->groups(d->m_error);
}

/*! Returns the group which is identified by the given \a groupId, or a default-constructed group if no such group exists */
QContactGroup QContactManager::group(const QUniqueId& groupId) const
{
    return d->m_engine->group(groupId, d->m_error);
}

/*!
 * Saves the group \a group in the database.  The id of the group is
 * used to determine the group to update.  If the group has no name set,
 * this function will fail.  If the group does not already
 * exist, it is added to the database.
 *
 * Returns true on success, or false on failure.
 */
bool QContactManager::saveGroup(QContactGroup* group)
{
    return d->m_engine->saveGroup(group, d->m_error);
}

/*! Remove the group with the given id \a groupId from the database.  Returns false if no group with that id exists, or the operation otherwise failed.  Returns true if the group was successfully deleted. */
bool QContactManager::removeGroup(const QUniqueId& groupId)
{
    return d->m_engine->removeGroup(groupId, d->m_error);
}

/*!
 * Returns a map of identifier to detail definition for the registered detail definitions
 * which are valid for the contacts in this store
 */
QMap<QString, QContactDetailDefinition> QContactManager::detailDefinitions() const
{
    return d->m_engine->detailDefinitions(d->m_error);
}

/*! Returns the definition identified by the given \a definitionId that is valid for the contacts in this store, or a default-constructed QContactDetailDefinition if no such definition exists */
QContactDetailDefinition QContactManager::detailDefinition(const QString& definitionId) const
{
    return d->m_engine->detailDefinition(definitionId, d->m_error);
}

/*! Persists the given definition \a def in the database.  Returns true if the definition was saved successfully, otherwise returns false */
bool QContactManager::saveDetailDefinition(const QContactDetailDefinition& def)
{
    return d->m_engine->saveDetailDefinition(def, d->m_error);
}

/*! Removes the detail definition identified by \a definitionId from the database.  Returns true if the definition was removed successfully, otherwise returns false */
bool QContactManager::removeDetailDefinition(const QString& definitionId)
{
    return d->m_engine->removeDetailDefinition(definitionId, d->m_error);
}

/*!
    Returns an object describing the supported functionality of this QContactManager.

    \sa QContactManagerInfo
 */
QContactManagerInfo* QContactManager::information() const
{
    if (!d->m_info) {
        d->m_info = new QContactManagerInfo;
        d->m_info->d = d;
    }
    return d->m_info;
}

/*! Returns the manager id for this QContactManager */
QString QContactManager::managerName() const
{
    return d->m_managerName;
}

/*! Return the parameters supplied to this QContactManager */
QMap<QString, QString>QContactManager::managerParameters() const
{
    return d->m_params;
}

/*!
 * Return the uri describing this QContactManager, including
 * managerName, managerStoreId and any parameters.
 */
QString QContactManager::storeUri() const
{
    return d->m_uri;
}

/* Changelog Functions */
/*!
 * Returns a list of ids of contacts that have been added to this manager
 * since the given \a timestamp.
 */
QList<QUniqueId> QContactManager::contactsAddedSince(const QDateTime& timestamp) const
{
    return d->m_engine->contactsAddedSince(timestamp ,d->m_error);
}

/*!
 * Returns a list of contact ids that have been modified in this manager
 * since the given \a timestamp.
 */
QList<QUniqueId> QContactManager::contactsModifiedSince(const QDateTime& timestamp) const
{
    return d->m_engine->contactsModifiedSince(timestamp, d->m_error);
}

/*!
 * Returns a list of contact ids that have been removed in this manager
 * since the given \a timestamp.
 */
QList<QUniqueId> QContactManager::contactsRemovedSince(const QDateTime& timestamp) const
{
    return d->m_engine->contactsRemovedSince(timestamp, d->m_error);
}

/*!
 * Returns a list of group ids that have been modified in this manager
 * since the given \a timestamp.
 */
QList<QUniqueId> QContactManager::groupsAddedSince(const QDateTime& timestamp) const
{
    return d->m_engine->groupsAddedSince(timestamp, d->m_error);
}

/*!
 * Returns a list of group ids that have been modified in this manager
 * since the given \a timestamp.
 */
QList<QUniqueId> QContactManager::groupsModifiedSince(const QDateTime& timestamp) const
{
    return d->m_engine->groupsModifiedSince(timestamp, d->m_error);
}

/*!
 * Returns a list of group ids that have been modified in this manager
 * since the given \a timestamp.
 */
QList<QUniqueId> QContactManager::groupsRemovedSince(const QDateTime& timestamp) const
{
    return d->m_engine->groupsRemovedSince(timestamp, d->m_error);
}
