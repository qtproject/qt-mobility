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

#include "qorganizerskeleton_p.h"
#include "qtorganizer.h"

//QTM_USE_NAMESPACE

QOrganizerItemManagerEngine* QOrganizerItemSkeletonFactory::engine(const QMap<QString, QString>& parameters, QOrganizerItemManager::Error* error)
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);

    /* TODO - if you understand any specific parameters. save them in the engine so that engine::managerParameters can return them */

    QOrganizerItemSkeletonEngine* ret = new QOrganizerItemSkeletonEngine(); // manager takes ownership and will clean up.
    return ret;
}

QOrganizerItemEngineLocalId* QOrganizerItemSkeletonFactory::createItemEngineLocalId() const
{
    /* TODO - instantiate your engine-specific item id in this function */
    return new QOrganizerItemSkeletonEngineLocalId;
}

QOrganizerCollectionEngineLocalId* QOrganizerItemSkeletonFactory::createCollectionEngineLocalId() const
{
    /* TODO - instantiate your engine-specific collection id in this function */
    return new QOrganizerCollectionSkeletonEngineLocalId;
}

QString QOrganizerItemSkeletonFactory::managerName() const
{
    /* TODO - put your engine name here */
    return QLatin1String("skeleton");
}
Q_EXPORT_PLUGIN2(qtorganizer_skeleton, QOrganizerItemSkeletonFactory);

QOrganizerItemSkeletonEngineLocalId::QOrganizerItemSkeletonEngineLocalId()
    : QOrganizerItemEngineLocalId(), m_localItemId(0)
{
    /*
      TODO

      Initialize any data members of your engine-specific item id in the constructor.

      This default constructor should not be used when returning a null id,
      but is provided in order to allow use of the ids in a list, and as an
      enabler for the implementation of QOrganizerItemLocalId.

      When returning a null id, the backend should simply return a default
      constructed QOrganizerItemLocalId.

      In this example, we use just a single quint32 to identify the item,
      however your engine may require more information in order to uniquely
      identify an item within it (e.g., a collection identifier plus an item
      identifier, and perhaps a datastore identifier which identifies the
      datastore in which the collection can be found).
     */
}

QOrganizerItemSkeletonEngineLocalId::QOrganizerItemSkeletonEngineLocalId(quint32 itemId)
    : QOrganizerItemEngineLocalId(), m_localItemId(itemId)
{
    /*
      TODO

      Whatever data members your particular class has, should be passed as arguments
      to a ctor of this type.  This is the constructor which will be used by your
      engine code.

      In particular, you will most likely be returning to clients an id by calling:
      QOrganizerItemLocalId id(new QOrganizerItemSkeletonEngineLocalId(3));
      or something similar.  Note that the QOrganizerItemLocalId constructor which
      takes a QOrganizerItemEngineLocalId pointer as a parameter takes ownership
      of that pointer (and so controls its lifetime).
     */
}

QOrganizerItemSkeletonEngineLocalId::QOrganizerItemSkeletonEngineLocalId(const QOrganizerItemSkeletonEngineLocalId& other)
    : QOrganizerItemEngineLocalId(), m_localItemId(other.m_localItemId)
{
    /* TODO - implement a copy constructor for your engine-specific local id class */
}

QOrganizerItemSkeletonEngineLocalId::~QOrganizerItemSkeletonEngineLocalId()
{
    /* TODO - Clean up any memory in use by your engine-specific local id. */
}

bool QOrganizerItemSkeletonEngineLocalId::isEqualTo(const QOrganizerItemEngineLocalId* other) const
{
    /*
      TODO

      The isEqualTo(other) function is called by the QOrganizerItemLocalId::operator==(other) function.
      You must implement this in terms of the data members which your class contains.

      An example implementation is provided below, for the case where only a single quint32
      is required to uniquely identify an item in a manager.
     */

    quint32 otherlocalItemId = static_cast<const QOrganizerItemSkeletonEngineLocalId*>(other)->m_localItemId;
    if (m_localItemId != otherlocalItemId)
        return false;
    return true;
}

bool QOrganizerItemSkeletonEngineLocalId::isLessThan(const QOrganizerItemEngineLocalId* other) const
{
    /*
      TODO

      The isLessThan(other) function is called by the QOrganizerItemLocalId::operator<(other) function.
      You must implement this in terms of the data members which your class contains.

      An example implementation is provided below, for the case where only a single quint32
      is required to uniquely identify an item in a manager.
     */

    quint32 otherlocalItemId = static_cast<const QOrganizerItemSkeletonEngineLocalId*>(other)->m_localItemId;
    return (m_localItemId < otherlocalItemId);
}

uint QOrganizerItemSkeletonEngineLocalId::engineLocalIdType() const
{
    /*
      TODO

      The engine-local id type value is used by QOrganizerItemLocalId to ensure that
      comparisons are occurring between ids of the same engine-specific type.
      Such comparisons would return false if the types are not equal.

      As such, it is important that the type identifier of your engine does not
      clash with that of any other engine, in order to avoid possibly undefined
      behaviour during comparison.

      We recommend returning the pre-calculated hash value of your manager name.
      Engines should embed the hash value into their application as a constant,
      read-only data value (uint), instead of calculating the value in this
      function.  For example purposes, however, we calculate it every time, below.
     */

    return qHash(QString(QLatin1String("skeleton")));
}

QOrganizerItemEngineLocalId* QOrganizerItemSkeletonEngineLocalId::clone() const
{
    /*
      TODO

      When a QOrganizerItemLocalId is copied or assigned, it performs a clone of
      the engine-specific local id.  This function is called in that case.

      Implement this function so that the data members of your engine-specific id
      are deep-copied.

      An example implementation for the case where an item can be uniquely identified
      with just a single quint32 is given below.
     */

    QOrganizerItemSkeletonEngineLocalId *myClone = new QOrganizerItemSkeletonEngineLocalId;
    myClone->m_localItemId = m_localItemId;
    return myClone;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug QOrganizerItemSkeletonEngineLocalId::debugStreamOut(QDebug dbg)
{
    /*
      TODO

      In order to allow clients to debug applications, you must implement this
      function.  We recommend streaming the name of your class followed by the
      values of the data members in your engine-specific local id class in
      parentheses.

      An example implementation for the case where an item can be uniquely identified
      with just a single quint32 is given below.

      Note that you must include the #ifndef QT_NO_DEBUG_STREAM preprocessor
      directive block in order to ensure compilation in environments where that
      directive is defined.
     */

    dbg.nospace() << "QOrganizerItemSkeletonEngineLocalId(" << m_localItemId << ")";
    return dbg.maybeSpace();
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream& QOrganizerItemSkeletonEngineLocalId::dataStreamOut(QDataStream& out)
{
    /*
      TODO

      In order to allow clients to serialize QOrganizerItemId's, you must implement
      this function.

      An example implementation for the case where an item can be uniquely identified
      with just a single quint32 is given below.

      Note that you must include the #ifndef QT_NO_DATASTREAM preprocessor
      directive block in order to ensure compilation in environments where that
      directive is defined.
     */

    out << m_localItemId;
    return out;
}

QDataStream& QOrganizerItemSkeletonEngineLocalId::dataStreamIn(QDataStream& in)
{
    /*
      TODO

      In order to allow clients to deserialize QOrganizerItemId's, you must implement
      this function.  If the dataStreamOut() function is implemented as:
          out << dataMemberOne << dataMemberTwo;
      then this function should be implemented as:
          in >> dataMemberOne >> dataMemberTwo;

      An example implementation for the case where an item can be uniquely identified
      with just a single quint32 is given below.

      Note that you must include the #ifndef QT_NO_DATASTREAM preprocessor
      directive block in order to ensure compilation in environments where that
      directive is defined.
     */

    in >> m_localItemId;
    return in;
}
#endif

uint QOrganizerItemSkeletonEngineLocalId::hash() const
{
    /*
      TODO

      Provide a hash function for your engine-specific id.
      Note that the hash doesn't strictly need to be unique, since isEqualTo()
      ensures that individual id's in a single hash-bucket can be uniquely
      determined; however a better hash function will result in better performance
      because the ids will be distributed more randomly in a hash table.

      In the example implementation below, we could simply return the id, since the
      id is a quint32.  In more complex id classes, however, you may need to
      qHash() individual data members and combine the results somehow.
     */

    return QT_PREPEND_NAMESPACE(qHash)(m_localItemId);
}


QOrganizerCollectionSkeletonEngineLocalId::QOrganizerCollectionSkeletonEngineLocalId()
    : QOrganizerCollectionEngineLocalId(), m_localCollectionId(0)
{
    /*
      TODO

      Initialize any data members of your engine-specific collection id in the constructor.

      This default constructor should not be used when returning a null id,
      but is provided in order to allow use of the ids in a list, and as an
      enabler for the implementation of QOrganizerCollectionLocalId.

      When returning a null id, the backend should simply return a default
      constructed QOrganizerCollectionLocalId.

      In this example, we use just a single quint32 to identify the collection,
      however your engine may require more information in order to uniquely
      identify a collection within it (e.g., a collection identifier plus a datastore
      identifier which identifies the datastore in which the collection can be found).
     */
}

QOrganizerCollectionSkeletonEngineLocalId::QOrganizerCollectionSkeletonEngineLocalId(quint32 collectionId)
    : QOrganizerCollectionEngineLocalId(), m_localCollectionId(collectionId)
{
    /*
      TODO

      Whatever data members your particular class has, should be passed as arguments
      to a ctor of this type.  This is the constructor which will be used by your
      engine code.

      In particular, you will most likely be returning to clients an id by calling:
      QOrganizerCollectionLocalId id(new QOrganizerCollectionSkeletonEngineLocalId(3));
      or something similar.  Note that the QOrganizerCollectionLocalId constructor which
      takes a QOrganizerCollectionEngineLocalId pointer as a parameter takes ownership
      of that pointer (and so controls its lifetime).
     */
}

QOrganizerCollectionSkeletonEngineLocalId::QOrganizerCollectionSkeletonEngineLocalId(const QOrganizerCollectionSkeletonEngineLocalId& other)
    : QOrganizerCollectionEngineLocalId(), m_localCollectionId(other.m_localCollectionId)
{
    /* TODO - implement a copy constructor for your engine-specific local id class */
}

QOrganizerCollectionSkeletonEngineLocalId::~QOrganizerCollectionSkeletonEngineLocalId()
{
    /* TODO - Clean up any memory in use by your engine-specific local id. */
}

bool QOrganizerCollectionSkeletonEngineLocalId::isEqualTo(const QOrganizerCollectionEngineLocalId* other) const
{
    /*
      TODO

      The isEqualTo(other) function is called by the QOrganizerCollectionLocalId::operator==(other) function.
      You must implement this in terms of the data members which your class contains.

      An example implementation is provided below, for the case where only a single quint32
      is required to uniquely identify a collection in a manager.
     */

    quint32 otherlocalCollectionId = static_cast<const QOrganizerCollectionSkeletonEngineLocalId*>(other)->m_localCollectionId;
    if (m_localCollectionId != otherlocalCollectionId)
        return false;
    return true;
}

bool QOrganizerCollectionSkeletonEngineLocalId::isLessThan(const QOrganizerCollectionEngineLocalId* other) const
{
    /*
      TODO

      The isLessThan(other) function is called by the QOrganizerCollectionLocalId::operator<(other) function.
      You must implement this in terms of the data members which your class contains.

      An example implementation is provided below, for the case where only a single quint32
      is required to uniquely identify a collection in a manager.
     */

    quint32 otherlocalCollectionId = static_cast<const QOrganizerCollectionSkeletonEngineLocalId*>(other)->m_localCollectionId;
    if (m_localCollectionId < otherlocalCollectionId)
        return true;
    return false;
}

uint QOrganizerCollectionSkeletonEngineLocalId::engineLocalIdType() const
{
    /*
      TODO

      The engine-local id type value is used by QOrganizerCollectionLocalId to ensure that
      comparisons are occurring between ids of the same engine-specific type.
      Such comparisons would return false if the types are not equal.

      As such, it is important that the type identifier of your engine does not
      clash with that of any other engine, in order to avoid possibly undefined
      behaviour during comparison.

      We recommend returning the pre-calculated hash value of your manager name.
      Engines should embed the hash value into their application as a constant,
      read-only data value (uint), instead of calculating the value in this
      function.  For example purposes, however, we calculate it every time, below.
     */

    return qHash(QString(QLatin1String("skeleton")));
}

QOrganizerCollectionEngineLocalId* QOrganizerCollectionSkeletonEngineLocalId::clone() const
{
    /*
      TODO

      When a QOrganizerCollectionLocalId is copied or assigned, it performs a clone of
      the engine-specific local id.  This function is called in that case.

      Implement this function so that the data members of your engine-specific id
      are deep-copied.

      An example implementation for the case where a collection can be uniquely identified
      with just a single quint32 is given below.
     */

    QOrganizerCollectionSkeletonEngineLocalId *myClone = new QOrganizerCollectionSkeletonEngineLocalId;
    myClone->m_localCollectionId = m_localCollectionId;
    return myClone;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug QOrganizerCollectionSkeletonEngineLocalId::debugStreamOut(QDebug dbg)
{
    /*
      TODO

      In order to allow clients to debug applications, you must implement this
      function.  We recommend streaming the name of your class followed by the
      values of the data members in your engine-specific local id class in
      parentheses.

      An example implementation for the case where a collection can be uniquely identified
      with just a single quint32 is given below.

      Note that you must include the #ifndef QT_NO_DEBUG_STREAM preprocessor
      directive block in order to ensure compilation in environments where that
      directive is defined.
     */

    dbg.nospace() << "QOrganizerCollectionSkeletonEngineLocalId(" << m_localCollectionId << ")";
    return dbg.maybeSpace();
}
#endif

#ifndef QT_NO_DATASTREAM
QDataStream& QOrganizerCollectionSkeletonEngineLocalId::dataStreamOut(QDataStream& out)
{
    /*
      TODO

      In order to allow clients to serialize QOrganizerCollectionId's, you must implement
      this function.

      An example implementation for the case where a collection can be uniquely identified
      with just a single quint32 is given below.

      Note that you must include the #ifndef QT_NO_DATASTREAM preprocessor
      directive block in order to ensure compilation in environments where that
      directive is defined.
     */

    out << m_localCollectionId;
    return out;
}

QDataStream& QOrganizerCollectionSkeletonEngineLocalId::dataStreamIn(QDataStream& in)
{
    /*
      TODO

      In order to allow clients to deserialize QOrganizerCollectionId's, you must implement
      this function.  If the dataStreamOut() function is implemented as:
          out << dataMemberOne << dataMemberTwo;
      then this function should be implemented as:
          in >> dataMemberOne >> dataMemberTwo;

      An example implementation for the case where a collection can be uniquely identified
      with just a single quint32 is given below.

      Note that you must include the #ifndef QT_NO_DATASTREAM preprocessor
      directive block in order to ensure compilation in environments where that
      directive is defined.
     */

    in >> m_localCollectionId;
    return in;
}
#endif

uint QOrganizerCollectionSkeletonEngineLocalId::hash() const
{
    /*
      TODO

      Provide a hash function for your engine-specific id.
      Note that the hash doesn't strictly need to be unique, since isEqualTo()
      ensures that individual id's in a single hash-bucket can be uniquely
      determined; however a better hash function will result in better performance
      because the ids will be distributed more randomly in a hash table.

      In the example implementation below, we could simply return the id, since the
      id is a quint32.  In more complex id classes, however, you may need to
      qHash() individual data members and combine the results somehow.
     */

    return QT_PREPEND_NAMESPACE(qHash)(m_localCollectionId);
}



QOrganizerItemSkeletonEngine::~QOrganizerItemSkeletonEngine()
{
    /* TODO clean up your stuff.  Perhaps a QScopedPointer or QSharedDataPointer would be in order */
}

QString QOrganizerItemSkeletonEngine::managerName() const
{
    /* TODO - put your engine name here */
    return QLatin1String("skeleton");
}

QMap<QString, QString> QOrganizerItemSkeletonEngine::managerParameters() const
{
    /* TODO - in case you have any actual parameters that are relevant that you saved in the factory method, return them here */
    return QMap<QString, QString>();
}

int QOrganizerItemSkeletonEngine::managerVersion() const
{
    /* TODO - implement this appropriately.  This is strictly defined by the engine, so return whatever you like */
    return 1;
}

QList<QOrganizerItem> QOrganizerItemSkeletonEngine::itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, QOrganizerItemManager::Error* error) const
{
    /*
        TODO

        This function should create a list of instances that occur in the time period from the supplied item.
        The periodStart should always be valid, and either the periodEnd or the maxCount will be valid (if periodEnd is
        valid, use that.  Otherwise use the count).  It's permissible to limit the number of items returned...

        Basically, if the generator item is an Event, a list of EventOccurrences should be returned.  Similarly for
        Todo/TodoOccurrence.

        If there are no instances, return an empty list.

        The returned items should have a QOrganizerItemInstanceOrigin detail that points to the generator and the
        original instance that the event would have occurred on (e.g. with an exception).

        They should not have recurrence information details in them.

        We might change the signature to split up the periodStart + periodEnd / periodStart + maxCount cases.
    */

    return QOrganizerItemManagerEngine::itemInstances(generator, periodStart, periodEnd, maxCount, error);
}

QList<QOrganizerItemLocalId> QOrganizerItemSkeletonEngine::itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const
{
    /*
        TODO

        Given the supplied filter and sort order, fetch the list of items [not instances] that correspond, and return their ids.

        If you don't support the filter or sort orders, you can fetch a partially (or un-) filtered list and ask the helper
        functions to filter and sort it for you.

        If you do have to fetch, consider setting a fetch hint that restricts the information to that needed for filtering/sorting.
    */

    *error = QOrganizerItemManager::NotSupportedError; // TODO <- remove this

    QList<QOrganizerItem> partiallyFilteredItems; // = ..., your code here.. [TODO]
    QList<QOrganizerItem> ret;

    foreach(const QOrganizerItem& item, partiallyFilteredItems) {
        if (QOrganizerItemManagerEngine::testFilter(filter, item)) {
            ret.append(item);
        }
    }

    return QOrganizerItemManagerEngine::sortItems(ret, sortOrders);
}

QList<QOrganizerItem> QOrganizerItemSkeletonEngine::items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    /*
        TODO

        Given the supplied filter and sort order, fetch the list of items [not instances] that correspond, and return them.

        If you don't support the filter or sort orders, you can fetch a partially (or un-) filtered list and ask the helper
        functions to filter and sort it for you.

        The fetch hint suggests how much of the item to fetch.  You can ignore the fetch hint and fetch everything (but you must
        fetch at least what is mentioned in the fetch hint).
    */

    Q_UNUSED(fetchHint);
    *error = QOrganizerItemManager::NotSupportedError; // TODO <- remove this

    QList<QOrganizerItem> partiallyFilteredItems; // = ..., your code here.. [TODO]
    QList<QOrganizerItem> ret;

    foreach(const QOrganizerItem& item, partiallyFilteredItems) {
        if (QOrganizerItemManagerEngine::testFilter(filter, item)) {
            QOrganizerItemManagerEngine::addSorted(&ret, item, sortOrders);
        }
    }

    /* An alternative formulation, depending on how your engine is implemented is just:

        foreach(const QOrganizerItemLocalId& id, itemIds(filter, sortOrders, error)) {
            ret.append(item(id, fetchHint, error);
        }
     */

    return ret;
}

QOrganizerItem QOrganizerItemSkeletonEngine::item(const QOrganizerItemLocalId& itemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    /*
        TODO

        Fetch a single item by id.

        The fetch hint suggests how much of the item to fetch.  You can ignore the fetch hint and fetch everything (but you must
        fetch at least what is mentioned in the fetch hint).

    */
    return QOrganizerItemManagerEngine::item(itemId, fetchHint, error);
}

bool QOrganizerItemSkeletonEngine::saveItems(QList<QOrganizerItem>* items, const QOrganizerCollectionLocalId& collectionId, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    /*
        TODO

        Save a list of items into the collection specified (or their current collection
        if no collection is specified and they already exist, or the default collection
        if no collection is specified and they do not exist).

        For each item, convert it to your local type, assign an item id, and update the
        QOrganizerItem's ID (in the list above - e.g. *items[idx] = updated item).

        If you encounter an error (e.g. converting to local type, or saving), insert an entry into
        the map above at the corresponding index (e.g. errorMap->insert(idx, QOIM::InvalidDetailError).
        You should set the "error" variable as well (first, last, most serious error etc).

        The item passed in should be validated according to the schema.
    */
    return QOrganizerItemManagerEngine::saveItems(items, collectionId, errorMap, error);

}

bool QOrganizerItemSkeletonEngine::removeItems(const QList<QOrganizerItemLocalId>& itemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    /*
        TODO

        Remove a list of items, given by their id.

        If you encounter an error, insert an error into the appropriate place in the error map,
        and update the error variable as well.

        DoesNotExistError should be used if the id refers to a non existent item.
    */
    return QOrganizerItemManagerEngine::removeItems(itemIds, errorMap, error);
}

QMap<QString, QOrganizerItemDetailDefinition> QOrganizerItemSkeletonEngine::detailDefinitions(const QString& itemType, QOrganizerItemManager::Error* error) const
{
    /* TODO - once you know what your engine will support, implement this properly.  One way is to call the base version, and add/remove things as needed */
    return QOrganizerItemManagerEngine::detailDefinitions(itemType, error);
}

QOrganizerItemDetailDefinition QOrganizerItemSkeletonEngine::detailDefinition(const QString& definitionId, const QString& itemType, QOrganizerItemManager::Error* error) const
{
    /* TODO - the default implementation just calls the base detailDefinitions function.  If that's inefficent, implement this */
    return QOrganizerItemManagerEngine::detailDefinition(definitionId, itemType, error);
}

bool QOrganizerItemSkeletonEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& itemType, QOrganizerItemManager::Error* error)
{
    /* TODO - if you support adding custom fields, do that here.  Otherwise call the base functionality. */
    return QOrganizerItemManagerEngine::saveDetailDefinition(def, itemType, error);
}

bool QOrganizerItemSkeletonEngine::removeDetailDefinition(const QString& definitionId, const QString& itemType, QOrganizerItemManager::Error* error)
{
    /* TODO - if you support removing custom fields, do that here.  Otherwise call the base functionality. */
    return QOrganizerItemManagerEngine::removeDetailDefinition(definitionId, itemType, error);
}


QOrganizerCollectionLocalId QOrganizerItemSkeletonEngine::defaultCollectionId(QOrganizerItemManager::Error* error) const
{
    /*
        TODO

        This allows clients to determine which collection an item will be saved,
        if the item is saved via saveItems() without specifying a collection id
        of a collection in which to save the item.

        If the backend does not support multiple collections (calendars) it may
        return the default constructed collection id.

        There is always at least one collection in a manager, and all items are
        saved in exactly one collection.
     */
    return QOrganizerItemManagerEngine::defaultCollectionId(error);
}

QList<QOrganizerCollectionLocalId> QOrganizerItemSkeletonEngine::collectionIds(QOrganizerItemManager::Error* error) const
{
    /*
        TODO

        This allows clients to retrieve the ids of all collections currently
        in this manager.  Some backends will have a prepopulated list of valid
        collections, others will not.
     */
    return QOrganizerItemManagerEngine::collectionIds(error);
}

QList<QOrganizerCollection> QOrganizerItemSkeletonEngine::collections(const QList<QOrganizerCollectionLocalId>& collectionIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error) const
{
    /*
        TODO

        This allows clients to retrieve the collections which correspond
        to the given collection ids.  A collection can have properties
        like colour, description, perhaps a priority, etc etc.
     */
    return QOrganizerItemManagerEngine::collections(collectionIds, errorMap, error);
}

bool QOrganizerItemSkeletonEngine::saveCollection(QOrganizerCollection* collection, QOrganizerItemManager::Error* error)
{
    /*
        TODO

        This allows clients to create or update collections if the backend supports
        mutable collections.  If the backend does support mutable collections, it
        should report that it supports the MutableCollections manager feature.
     */
    return QOrganizerItemManagerEngine::saveCollection(collection, error);
}

bool QOrganizerItemSkeletonEngine::removeCollection(const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error)
{
    /*
        TODO

        This allows clients to remove collections if the backend supports mutable
        collections.  If the backend does support mutable collections, it should
        report that it supports the MutableCollections manager feature.

        When a collection is removed, all items in the collection are removed.
        That is, they are _not_ transferred to another collection.

        If the user attempts to remove the collection which is the default collection,
        the backend may decide whether to fail (with a permissions error) or to
        succeed and arbitrarily choose another collection to be the default collection.
     */
    return QOrganizerItemManagerEngine::removeCollection(collectionId, error);
}

bool QOrganizerItemSkeletonEngine::startRequest(QOrganizerItemAbstractRequest* req)
{
    /*
        TODO

        This is the entry point to the async API.  The request object describes the
        type of request (switch on req->type()).  Req will not be null when called
        by the framework.

        Generally, you can queue the request and process them at some later time
        (probably in another thread).

        Once you start a request, call the updateRequestState and/or the
        specific updateXXXXXRequest functions to mark it in the active state.

        If your engine is particularly fast, or the operation involves only in
        memory data, you can process and complete the request here.  That is
        probably not the case, though.

        Note that when the client is threaded, and the request might live on a
        different thread, you might need to be careful with locking.  In particular,
        the request might be deleted while you are still working on it.  In this case,
        your requestDestroyed function will be called while the request is still valid,
        and you should block in that function until your worker thread (etc) has been
        notified not to touch that request any more.

        We plan to provide some boiler plate code that will allow you to:

        1) implement the sync functions, and have the async versions call the sync
           in another thread

        2) or implement the async versions of the function, and have the sync versions
           call the async versions.

        It's not ready yet, though.

        Return true if the request can be started, false otherwise.  You can set an error
        in the request if you like.
    */
    return QOrganizerItemManagerEngine::startRequest(req);
}

bool QOrganizerItemSkeletonEngine::cancelRequest(QOrganizerItemAbstractRequest* req)
{
    /*
        TODO

        Cancel an in progress async request.  If not possible, return false from here.
    */
    return QOrganizerItemManagerEngine::cancelRequest(req);
}

bool QOrganizerItemSkeletonEngine::waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs)
{
    /*
        TODO

        Wait for a request to complete (up to a max of msecs milliseconds).

        Return true if the request is finished (including if it was already).  False otherwise.

        You should really implement this function, if nothing else than as a delay, since clients
        may call this in a loop.

        It's best to avoid processing events, if you can, or at least only process non-UI events.
    */
    return QOrganizerItemManagerEngine::waitForRequestFinished(req, msecs);
}

void QOrganizerItemSkeletonEngine::requestDestroyed(QOrganizerItemAbstractRequest* req)
{
    /*
        TODO

        This is called when a request is being deleted.  It lets you know:

        1) the client doesn't care about the request any more.  You can still complete it if
           you feel like it.
        2) you can't reliably access any properties of the request pointer any more.  The pointer will
           be invalid once this function returns.

        This means that if you have a worker thread, you need to let that thread know that the
        request object is not valid and block until that thread acknowledges it.  One way to do this
        is to have a QSet<QOIAR*> (or QMap<QOIAR, MyCustomRequestState>) that tracks active requests, and
        insert into that set in startRequest, and remove in requestDestroyed (or when it finishes or is
        cancelled).  Protect that set/map with a mutex, and make sure you take the mutex in the worker
        thread before calling any of the QOIAR::updateXXXXXXRequest functions.  And be careful of lock
        ordering problems :D

    */
    return QOrganizerItemManagerEngine::requestDestroyed(req);
}

bool QOrganizerItemSkeletonEngine::hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& itemType) const
{
    // TODO - the answer to the question may depend on the type
    Q_UNUSED(itemType);
    switch(feature) {
        case QOrganizerItemManager::MutableDefinitions:
            // TODO If you support save/remove detail definition, return true
            return false;

        case QOrganizerItemManager::Anonymous:
            // TODO if this engine is anonymous (e.g. no other engine can share the data) return true
            // (mostly for an in memory engine)
            return false;
        case QOrganizerItemManager::ChangeLogs:
            // TODO if this engine supports filtering by last modified/created/removed timestamps, return true
            return false;
    }
    return false;
}

bool QOrganizerItemSkeletonEngine::isFilterSupported(const QOrganizerItemFilter& filter) const
{
    // TODO if you engine can natively support the filter, return true.  Otherwise you should emulate support in the item{Ids} functions.
    Q_UNUSED(filter);
    return false;
}

QList<int> QOrganizerItemSkeletonEngine::supportedDataTypes() const
{
    QList<int> ret;
    // TODO - tweak which data types this engine understands
    ret << QVariant::String;
    ret << QVariant::Date;
    ret << QVariant::DateTime;
    ret << QVariant::Time;

    return ret;
}

QStringList QOrganizerItemSkeletonEngine::supportedItemTypes() const
{
    // TODO - return which [predefined] types this engine supports
    QStringList ret;

    ret << QOrganizerItemType::TypeEvent;
    ret << QOrganizerItemType::TypeEventOccurrence;
    ret << QOrganizerItemType::TypeJournal;
    ret << QOrganizerItemType::TypeNote;
    ret << QOrganizerItemType::TypeTodo;
    ret << QOrganizerItemType::TypeTodoOccurrence;

    return ret;
}
