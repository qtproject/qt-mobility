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

#include "qorganizermanager.h"

#include "qorganizeritem_p.h"
#include "qorganizeritemfilter.h"
#include "qorganizeritemdetaildefinition.h"
#include "qorganizermanager_p.h"
#include "qorganizeritemfetchhint.h"

#include <QSharedData>
#include <QPair>
#include <QSet>

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerManager
  \brief The QOrganizerManager class provides an interface which allows clients with access to organizer item information stored in a particular backend.

  \inmodule QtOrganizer
  \ingroup organizer-main

  This class provides an abstraction of a datastore or aggregation of datastores which contains organizer item information.
  It provides methods to retrieve and manipulate organizer item information, collection information and supported schema definitions.
  It also provides metadata and error information reporting.

  The functions provided by QOrganizerManager are purely synchronous; to access the same functionality in an
  asynchronous manner, clients should use the use-case-specific classes derived from QOrganizerAbstractRequest.

  Some functionality provided by QOrganizerManager directly is not accessible using the asynchronous API; see
  the \l{Organizer Synchronous API}{synchronous} and \l{Organizer Asynchronous API}{asynchronous} API
  information from the \l{Organizer}{organizer module} API documentation.
 */

/*!
  \fn QOrganizerManager::dataChanged()
  This signal is emitted by the manager if its internal state changes, and it is unable to determine the changes
  which occurred, or if the manager considers the changes to be radical enough to require clients to reload all data.
  If this signal is emitted, no other signals will be emitted for the associated changes.
 */

/*!
  \fn QOrganizerManager::itemsAdded(const QList<QOrganizerItemId>& itemIds)
  This signal is emitted at some point once the items identified by \a itemIds have been added to a datastore managed by this manager.
  This signal will not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
  \fn QOrganizerManager::itemsChanged(const QList<QOrganizerItemId>& itemIds)
  This signal is emitted at some point once the items identified by \a itemIds have been modified in a datastore managed by this manager.
  This signal will not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
  \fn QOrganizerManager::itemsRemoved(const QList<QOrganizerItemId>& itemIds)
  This signal is emitted at some point once the items identified by \a itemIds have been removed from a datastore managed by this manager.
  This signal will not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
  \fn QOrganizerManager::collectionsAdded(const QList<QOrganizerCollectionId>& collectionIds)
  This signal is emitted at some point once the collections identified by \a collectionIds have been added to a datastore managed by this manager.
  This signal will not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
  \fn QOrganizerManager::collectionsChanged(const QList<QOrganizerCollectionId>& collectionIds)
  This signal is emitted at some point once the metadata for the collections identified by \a collectionIds have been modified in a datastore managed by this manager.
  This signal is not emitted if one of the items in this collection has changed - itemsChanged() will be emitted instead.
  This signal will not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
  \fn QOrganizerManager::collectionsRemoved(const QList<QOrganizerCollectionId>& collectionIds)
  This signal is emitted at some point once the collections identified by \a collectionIds have been removed from a datastore managed by this manager.
  This signal will not be emitted if the dataChanged() signal was previously emitted for these changes.
 */


#define makestr(x) (#x)
#define makename(x) makestr(x)

/*!
    Returns a list of available manager ids that can be used when constructing
    a QOrganizerManager.  If an empty id is specified to the constructor, the
    first value in this list will be used instead.
  */
QStringList QOrganizerManager::availableManagers()
{
    QStringList ret;
    ret << QLatin1String("memory") << QLatin1String("invalid");
    QOrganizerManagerData::loadFactories();
    ret.append(QOrganizerManagerData::m_engines.keys());

    // now swizzle the default engine to pole position
#if defined(Q_ORGANIZER_DEFAULT_ENGINE)
    if (ret.removeAll(QLatin1String(makename(Q_ORGANIZER_DEFAULT_ENGINE)))) {
        ret.prepend(QLatin1String(makename(Q_ORGANIZER_DEFAULT_ENGINE)));
    }
#endif

    return ret;
}

/*!
  Splits the given \a uri into the manager, store, and parameters that it describes, and places the information into the memory addressed by \a pManagerId and \a pParams respectively.  Returns true if \a uri could be split successfully, otherwise returns false
 */
bool QOrganizerManager::parseUri(const QString& uri, QString* pManagerId, QMap<QString, QString>* pParams)
{
    // Format: qtorganizer:<managerid>:<key>=<value>&<key>=<value>
    // 1) parameters are currently a qstringlist.. should they be a map?
    // 2) is the uri going to be escaped?  my guess would be "probably not"
    // 3) hence, do we assume that the prefix, managerid and storeid cannot contain `:'
    // 4) similarly, that neither keys nor values can contain `=' or `&'

    QStringList colonSplit = uri.split(QLatin1Char(':'));
    QString prefix = colonSplit.value(0);

    if (prefix != QLatin1String("qtorganizer"))
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

/*! Returns a URI that completely describes a manager implementation, datastore, and the parameters with which to instantiate the manager, from the given \a managerName, \a params and an optional \a implementationVersion */
QString QOrganizerManager::buildUri(const QString& managerName, const QMap<QString, QString>& params, int implementationVersion)
{
    QString ret(QLatin1String("qtorganizer:%1:%2"));
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

    if (implementationVersion != -1) {
        QString versionString = QString(QLatin1String(QTORGANIZER_IMPLEMENTATION_VERSION_NAME));
        versionString += QString::fromAscii("=");
        versionString += QString::number(implementationVersion);
        escapedParams.append(versionString);
    }

    return ret.arg(managerName, escapedParams.join(QLatin1String("&")));
}

/*!
  Constructs a QOrganizerManager whose implementation, store and parameters are specified in the given \a storeUri,
  and whose parent object is \a parent.
 */
QOrganizerManager* QOrganizerManager::fromUri(const QString& storeUri, QObject* parent)
{
    if (storeUri.isEmpty()) {
        return new QOrganizerManager(QString(), QMap<QString, QString>(), parent);
    } else {
        QString id;
        QMap<QString, QString> parameters;
        if (parseUri(storeUri, &id, &parameters)) {
            return new QOrganizerManager(id, parameters, parent);
        } else {
            // invalid
            return new QOrganizerManager(QLatin1String("invalid"), QMap<QString, QString>(), parent);
        }
    }
}

/*!
  Constructs a QOrganizerManager whose parent QObject is \a parent.
  The default implementation for the platform will be created.
 */
QOrganizerManager::QOrganizerManager(QObject* parent)
    : QObject(parent),
    d(new QOrganizerManagerData)
{
    createEngine(QString(), QMap<QString, QString>());
}

/*!
  Constructs a QOrganizerManager whose implementation is identified by \a managerName with the given \a parameters.

  The \a parent QObject will be used as the parent of this QOrganizerManager.

  If an empty \a managerName is specified, the default implementation for the platform will
  be used.
 */
QOrganizerManager::QOrganizerManager(const QString& managerName, const QMap<QString, QString>& parameters, QObject* parent)
    : QObject(parent),
    d(new QOrganizerManagerData)
{
    createEngine(managerName, parameters);
}

void QOrganizerManager::createEngine(const QString& managerName, const QMap<QString, QString>& parameters)
{
    d->createEngine(managerName, parameters);
    connect(d->m_engine, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
    connect(d->m_engine, SIGNAL(itemsAdded(QList<QOrganizerItemId>)), this, SIGNAL(itemsAdded(QList<QOrganizerItemId>)));
    connect(d->m_engine, SIGNAL(itemsChanged(QList<QOrganizerItemId>)), this, SIGNAL(itemsChanged(QList<QOrganizerItemId>)));
    connect(d->m_engine, SIGNAL(itemsRemoved(QList<QOrganizerItemId>)), this, SIGNAL(itemsRemoved(QList<QOrganizerItemId>)));
    connect(d->m_engine, SIGNAL(collectionsAdded(QList<QOrganizerCollectionId>)), this, SIGNAL(collectionsAdded(QList<QOrganizerCollectionId>)));
    connect(d->m_engine, SIGNAL(collectionsChanged(QList<QOrganizerCollectionId>)), this, SIGNAL(collectionsChanged(QList<QOrganizerCollectionId>)));
    connect(d->m_engine, SIGNAL(collectionsRemoved(QList<QOrganizerCollectionId>)), this, SIGNAL(collectionsRemoved(QList<QOrganizerCollectionId>)));
}

/*!
  Constructs a QOrganizerManager whose backend has the name \a managerName and version \a implementationVersion, where the manager
  is constructed with the provided \a parameters.

  The \a parent QObject will be used as the parent of this QOrganizerManager.

  If an empty \a managerName is specified, the default implementation for the platform will be instantiated.
  If the specified implementation version is not available, the manager with the name \a managerName with the default implementation version is instantiated.
 */
QOrganizerManager::QOrganizerManager(const QString& managerName, int implementationVersion, const QMap<QString, QString>& parameters, QObject* parent)
    : QObject(parent),
    d(new QOrganizerManagerData)
{
    QMap<QString, QString> params = parameters;
    params[QString(QLatin1String(QTORGANIZER_IMPLEMENTATION_VERSION_NAME))] = QString::number(implementationVersion);
    createEngine(managerName, params);
}

/*! Frees the memory used by the QOrganizerManager */
QOrganizerManager::~QOrganizerManager()
{
    delete d;
}

/*!
  \enum QOrganizerManager::Error

  This enum specifies an error that occurred during the most recent operation:

  \value NoError The most recent operation was successful
  \value DoesNotExistError The most recent operation failed because the requested organizer item or detail definition does not exist
  \value AlreadyExistsError The most recent operation failed because the specified organizer item or detail definition already exists
  \value InvalidDetailError The most recent operation failed because the specified organizer detail definition already exists
  \value LockedError The most recent operation failed because the datastore specified is currently locked
  \value DetailAccessError The most recent operation failed because a detail was modified or removed and its access method does not allow that
  \value PermissionsError The most recent operation failed because the caller does not have permission to perform the operation
  \value OutOfMemoryError The most recent operation failed due to running out of memory
  \value NotSupportedError The most recent operation failed because the requested operation is not supported in the specified store
  \value BadArgumentError The most recent operation failed because one or more of the parameters to the operation were invalid
  \value UnspecifiedError The most recent operation failed for an undocumented reason
  \value VersionMismatchError The most recent operation failed because the backend of the manager is not of the required version
  \value LimitReachedError The most recent operation failed because the limit for that type of object has been reached
  \value InvalidItemTypeError The most recent operation failed because the item given was of an invalid type for the operation
  \value InvalidCollectionError The most recent operation failed because the collection is invalid
  \value InvalidOccurrenceError The most recent operation failed because it was an attempt to save an occurrence without a correct InstanceOrigin detail
 */

/*! Return the error code of the most recent operation */
QOrganizerManager::Error QOrganizerManager::error() const
{
    return d->m_error;
}

/*!
  Returns per-input error codes for the most recent operation.
  This function only returns meaningful information if the most
  recent operation was a batch operation.
  Each key in the map is the index of the element in the input list
  for which the error (whose error code is stored in the value for
  that key in the map) occurred during the batch operation.
  \sa error(), saveItems(), removeItems()
 */
QMap<int, QOrganizerManager::Error> QOrganizerManager::errorMap() const
{
    return d->m_errorMap;
}

/*!
  Return the list of a maximum of \a maxCount organizer item instances which are occurrences of the given \a parentItem recurring item, which
  occur between the given \a periodStart date and the given \a periodEnd date, inclusive.

  If \a periodStart is after \a periodEnd, the operation will fail.
  If \a maxCount is negative, it is backend specific as to how many occurrences will be returned.
  Some backends may return no instances, others may return some limited number of occurrences.

  The \a fetchHint allows clients to specify which pieces of information they are interested or not interested in, to allow
  backends to optimise data retrieval if possible.  Note that it is simply a hint; backends can ignore the \a fetchHint,
  but if they do so they must return the full item.
  */
QList<QOrganizerItem> QOrganizerManager::itemOccurrences(const QOrganizerItem& parentItem, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, const QOrganizerItemFetchHint& fetchHint) const
{
    d->m_error = QOrganizerManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->itemOccurrences(parentItem, periodStart, periodEnd, maxCount, fetchHint, &d->m_error);
}

/*!
  Returns a list of organizer item ids that match the given \a filter, sorted according to the given list of \a sortOrders.
  Depending on the backend, this filtering operation may involve retrieving all organizer items.
 */
QList<QOrganizerItemId> QOrganizerManager::itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders) const
{
    d->m_error = QOrganizerManager::NoError;
    return d->m_engine->itemIds(QDateTime(), QDateTime(), filter, sortOrders, &d->m_error);
}

/*!
  Returns a list of organizer item ids of persisted items that match the given \a filter, sorted according to the given list of \a sortOrders,
  for any item which occurs (or has an occurrence which occurs) in the range specified by the given \a startDate and \a endDate, inclusive.
  A default-constructed (invalid) \a startDate specifies an open start date (matches anything which occurs up until the \a endDate),
  and a default-constructed (invalid) \a endDate specifies an open end date (matches anything which occurs after the \a startDate).
  If both the \a startDate and \a endDate are invalid, this function will return the ids of all items which match the \a filter criteria.
  Depending on the backend, this filtering operation may involve retrieving all organizer items.
 */
QList<QOrganizerItemId> QOrganizerManager::itemIds(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders) const
{
    d->m_error = QOrganizerManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->itemIds(startDate, endDate, filter, sortOrders, &d->m_error);
}

/*!
  Returns a list of organizer items that match the given \a filter, sorted according to the given list of \a sortOrders.

  This function will return both persisted and generated occurrences of items which match the specified \a filter.

  Depending on the manager implementation, this filtering operation might be slow and involve retrieving all
  organizer items and testing them against the supplied filter - see the \l isFilterSupported() function.

  The \a fetchHint parameter describes the optimization hints that a manager may take.
  If the \a fetchHint is the default constructed hint, all existing details and relationships
  in the matching organizer items will be returned.  A client should not make changes to an organizer item which has
  been retrieved using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the organizer item back to the manager (as the "new" restricted organizer item will
  replace the previously saved organizer item in the backend).

  \sa QOrganizerItemFetchHint
 */
QList<QOrganizerItem> QOrganizerManager::items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint) const
{
    d->m_error = QOrganizerManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->items(QDateTime(), QDateTime(), filter, sortOrders, fetchHint, &d->m_error);
}

/*!
  Returns a list of organizer items that match the given \a filter, sorted according to the given list of \a sortOrders,
  for any item or occurrence of an item which occurs in the range specified by the given \a startDate and \a endDate, inclusive.
  A default-constructed (invalid) \a startDate specifies an open start date (matches anything which occurs up until the \a endDate),
  and a default-constructed (invalid) \a endDate specifies an open end date (matches anything which occurs after the \a startDate).
  If both the \a startDate and \a endDate are invalid, this function will return all items which match the \a filter criteria.

  This function will return both persisted and generated occurrences of items which match the specified criteria.

  Depending on the manager implementation, this filtering operation might be slow and involve retrieving all
  organizer items and testing them against the supplied filter - see the \l isFilterSupported() function.

  The \a fetchHint parameter describes the optimization hints that a manager may take.
  If the \a fetchHint is the default constructed hint, all existing details and relationships
  in the matching organizer items will be returned.  A client should not make changes to an organizer item which has
  been retrieved using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the organizer item back to the manager (as the "new" restricted organizer item will
  replace the previously saved organizer item in the backend).

  \sa QOrganizerItemFetchHint
 */
QList<QOrganizerItem> QOrganizerManager::items(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint) const
{
    d->m_error = QOrganizerManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->items(startDate, endDate, filter, sortOrders, fetchHint, &d->m_error);
}

/*!
  Returns a list of organizer items that match the given \a filter, sorted according to the given list of \a sortOrders,
  for any item which occurs (or has an occurrence which occurs) in the range specified by the given \a startDate and \a endDate, inclusive.
  A default-constructed (invalid) \a startDate specifies an open start date (matches anything which occurs up until the \a endDate),
  and a default-constructed (invalid) \a endDate specifies an open end date (matches anything which occurs after the \a startDate).
  If both the \a startDate and \a endDate are invalid, this function will return all items which match the \a filter criteria.

  This function will only return parent items and persisted exceptions which match the specified criteria; not generated occurrences.

  Depending on the manager implementation, this filtering operation might be slow and involve retrieving all
  organizer items and testing them against the supplied filter - see the \l isFilterSupported() function.

  The \a fetchHint parameter describes the optimization hints that a manager may take.
  If the \a fetchHint is the default constructed hint, all existing details and relationships
  in the matching organizer items will be returned.  A client should not make changes to an organizer item which has
  been retrieved using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the organizer item back to the manager (as the "new" restricted organizer item will
  replace the previously saved organizer item in the backend).

  \sa QOrganizerItemFetchHint
 */
QList<QOrganizerItem> QOrganizerManager::itemsForExport(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint) const
{
    d->m_error = QOrganizerManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->itemsForExport(startDate, endDate, filter, sortOrders, fetchHint, &d->m_error);
}

/*!
  Returns the organizer item in the database identified by \a organizeritemId.

  If the organizer item does not exist, an empty, default constructed QOrganizerItem will be returned,
  and the error returned by \l error() will be \c QOrganizerManager::DoesNotExistError.

  The \a fetchHint parameter describes the optimization hints that a manager may take.
  If the \a fetchHint is the default constructed hint, all existing details and relationships
  in the matching organizer item will be returned.  A client should not make changes to an organizer item which has
  been retrieved using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the organizer item back to the manager (as the "new" restricted organizer item will
  replace the previously saved organizer item in the backend).

  \sa QOrganizerItemFetchHint
 */
QOrganizerItem QOrganizerManager::item(const QOrganizerItemId& organizeritemId, const QOrganizerItemFetchHint& fetchHint) const
{
    d->m_error = QOrganizerManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->item(organizeritemId, fetchHint, &d->m_error);
}

/*!
  Adds the given \a organizeritem to the database if \a organizeritem has a
  default-constructed id, or an id with the manager URI set to the URI of
  this manager and a id of zero.  It will be saved in the collection whose
  id is reported by calling item->collectionId() if the specified collection exists,
  or if no collectionId is specified in the item, or the collectionId is the default
  collection id, it will be saved in the collection in which the item is currently
  saved (if it is not a new item) or in the default collection (if it is a new item).
  As such, an item may be moved between collections with this save operation.

  If the manager URI of the id of the \a organizeritem is neither empty nor equal to the URI of
  this manager, or id of the \a organizeritem is non-zero but does not exist in the
  manager, the operation will fail and calling error() will return
  \c QOrganizerManager::DoesNotExistError.

  Alternatively, the function will update the existing organizer item in the database if \a organizeritem
  has a non-zero id and currently exists in the database.

  If the \a organizeritem contains one or more details whose definitions have
  not yet been saved with the manager, the operation will fail and calling
  error() will return \c QOrganizerManager::UnsupportedError.

  Returns false on failure, or true on
  success.  On successful save of an organizer item with a null id, its
  id will be set to a new, valid id with the manager URI set to the URI of
  this manager, and the id set to a new, valid id.  On successful save of an
  organizer item with a null collection id, the collection id will be set to
  the id of the default collection of the manager.
  The manager is not required to fetch updated details of the organizer item on save,
  and as such, clients should fetch an organizer item if they want the most up-to-date information
  by calling \l QOrganizerManager::item().

  \sa managerUri()
 */
bool QOrganizerManager::saveItem(QOrganizerItem* organizeritem)
{
    d->m_errorMap.clear();
    if (organizeritem) {
        d->m_error = QOrganizerManager::NoError;
        return d->m_engine->saveItem(organizeritem, &d->m_error);
    } else {
        d->m_error = QOrganizerManager::BadArgumentError;
        return false;
    }
}

/*!
  Remove the organizer item identified by \a organizeritemId from the database.
  Returns true if the organizer item was removed successfully, otherwise
  returns false.
 */
bool QOrganizerManager::removeItem(const QOrganizerItemId& organizeritemId)
{
    d->m_error = QOrganizerManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->removeItem(organizeritemId, &d->m_error);
}

/*!
  Adds the list of organizer items given by \a organizeritems list to the database.
  Each item in the list will be saved in the collection whose
  id is reported by calling item->collectionId() if the specified collection exists,
  or if no collectionId is specified in the item, or the collectionId is the default
  collection id, it will be saved in the collection in which the item is currently
  saved (if it is not a new item) or in the default collection (if it is a new item).
  As such, an item may be moved between collections with this save operation.
  Returns true if the organizer items were saved successfully, otherwise false.

  Calling \l errorMap() will return the per-input errors for the latest batch function.
  The \l QOrganizerManager::error() function will only return \c QOrganizerManager::NoError
  if all organizer items were saved successfully.

  For each newly saved organizer item that was successful, the id of the organizer item
  in the \a organizeritems list will be updated with the new value.  If a failure occurs
  when saving a new item, the id will be cleared.

  \sa QOrganizerManager::saveItem()
 */
bool QOrganizerManager::saveItems(QList<QOrganizerItem>* organizeritems)
{
    d->m_errorMap.clear();
    if (!organizeritems) {
        d->m_error = QOrganizerManager::BadArgumentError;
        return false;
    }

    d->m_error = QOrganizerManager::NoError;
    return d->m_engine->saveItems(organizeritems, &d->m_errorMap, &d->m_error);
}

/*!
  Remove every organizer item whose id is contained in the list of organizer item ids
  \a organizeritemIds.  Returns true if all organizer items were removed successfully,
  otherwise false.

  Calling \l errorMap() will return the per-input errors for the latest batch function.
  The \l QOrganizerManager::error() function will
  only return \c QOrganizerManager::NoError if all organizer items were removed
  successfully.

  If the given list of organizer item ids \a organizeritemIds is empty, the function will return false
  and calling error() will return \c QOrganizerManager::BadArgumentError.  If the list is non-empty
  and contains ids which do not identify a valid organizer item in the manager, the function will
  remove any organizer items which are identified by ids in the \a organizeritemIds list, insert
  \c QOrganizerManager::DoesNotExist entries into the error map for the indices of invalid ids
  in the \a organizeritemIds list, return false, and set the overall operation error to
  \c QOrganizerManager::DoesNotExistError.

  \sa QOrganizerManager::removeItem()
 */
bool QOrganizerManager::removeItems(const QList<QOrganizerItemId>& organizeritemIds)
{
    d->m_errorMap.clear();
    if (organizeritemIds.isEmpty()) {
        d->m_error = QOrganizerManager::BadArgumentError;
        return false;
    }

    d->m_error = QOrganizerManager::NoError;
    return d->m_engine->removeItems(organizeritemIds, &d->m_errorMap, &d->m_error);
}

/*!
  Returns the id of the default collection managed by this manager
 */
QOrganizerCollection QOrganizerManager::defaultCollection() const
{
    d->m_error = QOrganizerManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->defaultCollection(&d->m_error);
}

/*!
  Returns the collection identified by the given \a collectionId which is managed by this manager.
 */
QOrganizerCollection QOrganizerManager::collection(const QOrganizerCollectionId& collectionId) const
{
    d->m_error = QOrganizerManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->collection(collectionId, &d->m_error);
}

/*!
  Returns a list of all of the collections managed by this manager.
 */
QList<QOrganizerCollection> QOrganizerManager::collections() const
{
    d->m_error = QOrganizerManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->collections(&d->m_error);
}

/*!
  Saves the given \a collection in the manager.
  Returns true on success, false on failure.

  Some managers do not allow modifications to collections,
  and thus attempting to save a collection will always fail
  when attempted in such a manager.

  Some managers do not allow adding new collections,
  and thus attempting to save a new collection will always fail
  when attempted in such a manager.

  Some managers provide front-ends to read-only datastores, and
  attempting to save a new collection in such a manager will
  always fail.

  Most managers will require a valid value for the \c QOrganizerCollection::KeyName
  meta data key to be set in a collection, before the collection can be saved
  correctly.  Clients can call compatibleCollection() to retrieve a pruned
  or updated version of the collection which is compatible with the manager.

  \sa compatibleCollection()
 */
bool QOrganizerManager::saveCollection(QOrganizerCollection* collection)
{
    d->m_errorMap.clear();
    if (collection) {
        d->m_error = QOrganizerManager::NoError;
        return d->m_engine->saveCollection(collection, &d->m_error);
    } else {
        d->m_error = QOrganizerManager::BadArgumentError;
        return false;
    }
}

/*!
  Removes the collection identified by the given \a collectionId (and all items in the collection)
  from the manager if the given \a collectionId exists.
  Returns true on success, false on failure.

  Attempting to remove the default collection will fail and calling \l error() will return
  QOrganizerManager::PermissionsError.
 */
bool QOrganizerManager::removeCollection(const QOrganizerCollectionId& collectionId)
{
    d->m_error = QOrganizerManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->removeCollection(collectionId, &d->m_error);
}

/*!
  Returns a pruned or modified version of the \a original organizer item which is valid and can be saved in the manager.
  The returned organizer item might have entire details removed or arbitrarily changed.
 */
QOrganizerItem QOrganizerManager::compatibleItem(const QOrganizerItem& original) const
{
    d->m_error = QOrganizerManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->compatibleItem(original, &d->m_error);
}

/*!
  Returns a pruned or modified version of the \a original organizer collection which is valid and can be saved in the manager.
  The returned organizer collection might have meta data removed or arbitrarily changed.
 */
QOrganizerCollection QOrganizerManager::compatibleCollection(const QOrganizerCollection& original) const
{
    d->m_error = QOrganizerManager::NoError;
    d->m_errorMap.clear();
    return d->m_engine->compatibleCollection(original, &d->m_error);
}

/*!
  Returns a map of identifier to detail definition for the registered detail definitions for items whose type is the given \a organizeritemType
  which are valid for the organizer items in this store
 */
QMap<QString, QOrganizerItemDetailDefinition> QOrganizerManager::detailDefinitions(const QString& organizeritemType) const
{
    d->m_errorMap.clear();
    if (!supportedItemTypes().contains(organizeritemType)) {
        d->m_error = QOrganizerManager::InvalidItemTypeError;
        return QMap<QString, QOrganizerItemDetailDefinition>();
    }

    d->m_error = QOrganizerManager::NoError;
    return d->m_engine->detailDefinitions(organizeritemType, &d->m_error);
}

/*! Returns the definition identified by the given \a definitionName that is valid for the organizer items whose type is the given \a organizeritemType in this store, or a default-constructed QOrganizerItemDetailDefinition if no such definition exists */
QOrganizerItemDetailDefinition QOrganizerManager::detailDefinition(const QString& definitionName, const QString& organizeritemType) const
{
    d->m_errorMap.clear();
    if (!supportedItemTypes().contains(organizeritemType)) {
        d->m_error = QOrganizerManager::InvalidItemTypeError;
        return QOrganizerItemDetailDefinition();
    }

    d->m_error = QOrganizerManager::NoError;
    return d->m_engine->detailDefinition(definitionName, organizeritemType, &d->m_error);
}

/*! Persists the given definition \a def in the database, which is valid for organizer items whose type is the given \a organizeritemType.  Returns true if the definition was saved successfully, otherwise returns false */
bool QOrganizerManager::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType)
{
    d->m_errorMap.clear();
    if (!supportedItemTypes().contains(organizeritemType)) {
        d->m_error = QOrganizerManager::InvalidItemTypeError;
        return false;
    }

    d->m_error = QOrganizerManager::NoError;
    return d->m_engine->saveDetailDefinition(def, organizeritemType, &d->m_error);
}

/*! Removes the detail definition identified by \a definitionName from the database, which is valid for organizer items whose type is the given \a organizeritemType.  Returns true if the definition was removed successfully, otherwise returns false */
bool QOrganizerManager::removeDetailDefinition(const QString& definitionName, const QString& organizeritemType)
{
    d->m_errorMap.clear();
    if (!supportedItemTypes().contains(organizeritemType)) {
        d->m_error = QOrganizerManager::InvalidItemTypeError;
        return false;
    }

    d->m_error = QOrganizerManager::NoError;
    return d->m_engine->removeDetailDefinition(definitionName, organizeritemType, &d->m_error);
}

/*!
  \enum QOrganizerManager::ManagerFeature
  This enum describes the possible features that a particular manager may support
  \value MutableDefinitions The manager supports saving, updating or removing detail definitions.  Some built-in definitions may still be immutable
  \value ChangeLogs The manager supports reporting of timestamps of changes, and filtering and sorting by those timestamps
  \value Anonymous The manager is isolated from other managers
 */

/*!
  Returns true if the given feature \a feature is supported by the manager, for the specified type of organizer item \a organizeritemType
 */
bool QOrganizerManager::hasFeature(QOrganizerManager::ManagerFeature feature, const QString& organizeritemType) const
{
    return d->m_engine->hasFeature(feature, organizeritemType);
}

/*!
  Returns true if the given \a filter is supported natively by the
  manager, and false if the filter behaviour would be emulated.

  Note: In some cases, the behaviour of an unsupported filter
  cannot be emulated.  For example, a filter that requests organizer items
  that have changed since a given time depends on having that information
  available.  In these cases, the filter will fail.
 */
bool QOrganizerManager::isFilterSupported(const QOrganizerItemFilter& filter) const
{
    return d->m_engine->isFilterSupported(filter);
}

/*!
  Returns the list of organizer item types which are supported by this manager.
  This is a convenience function, equivalent to retrieving the allowable values
  for the \c QOrganizerItemType::FieldType field of the QOrganizerItemType definition
  which is valid in this manager.
 */
QStringList QOrganizerManager::supportedItemTypes() const
{
    return d->m_engine->supportedItemTypes();
}

/*!
  Returns the engine backend implementation version number
 */
int QOrganizerManager::managerVersion() const
{
    return d->m_engine->managerVersion();
}

/*! Returns the manager name for this QOrganizerManager */
QString QOrganizerManager::managerName() const
{
    return d->m_engine->managerName();
}

/*! Return the parameters relevant to the creation of this QOrganizerManager */
QMap<QString, QString> QOrganizerManager::managerParameters() const
{
    QMap<QString, QString> params = d->m_engine->managerParameters();

    params.remove(QString::fromAscii(QTORGANIZER_VERSION_NAME));
    params.remove(QString::fromAscii(QTORGANIZER_IMPLEMENTATION_VERSION_NAME));
    return params;
}

/*!
  Return the uri describing this QOrganizerManager, consisting of the manager name and any parameters.
 */
QString QOrganizerManager::managerUri() const
{
    return d->m_engine->managerUri();
}

/*! Return a list of QOrganizerItemId extracted from the \a items */
QList<QOrganizerItemId> QOrganizerManager::extractIds(const QList<QOrganizerItem>& items)
{
    QList<QOrganizerItemId> ids;
#if QT_VERSION > 0x040700    
    ids.reserve(items.count());
#endif

    foreach(const QOrganizerItem& item, items)
        ids.append(item.id());
    return ids;
}

#include "moc_qorganizermanager.cpp"

QTM_END_NAMESPACE
