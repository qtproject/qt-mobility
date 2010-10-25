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

#include "qorganizermanagerengine.h"
#include "qorganizeritemengineid.h"

#include "qorganizeritemdetaildefinition.h"
#include "qorganizeritemdetailfielddefinition.h"
#include "qorganizeritemdetails.h"
#include "qorganizeritemsortorder.h"
#include "qorganizeritemfilters.h"
#include "qorganizerabstractrequest.h"
#include "qorganizerabstractrequest_p.h"
#include "qorganizeritemrequests.h"
#include "qorganizeritemrequests_p.h"
#include "qorganizeritem.h"
#include "qorganizeritemfetchhint.h"

#include "qorganizercollection_p.h"
#include "qorganizeritem_p.h"
#include "qorganizeritemdetail_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerManagerEngine
  \brief The QOrganizerManagerEngine class provides the interface for all
  implementations of the organizer item manager backend functionality.

  \inmodule QtOrganizer
  \ingroup organizer-backends

  Instances of this class are usually provided by a
  \l QOrganizerManagerEngineFactory, which is loaded from a plugin.

  The default implementation of this interface provides a basic
  level of functionality for some functions so that specific engines
  can simply implement the functionality that is supported by
  the specific organizer items engine that is being adapted.

  More information on writing a organizer items engine plugin is available in
  the \l{Qt Organizer Manager Engines} documentation.

  \sa QOrganizerManager, QOrganizerManagerEngineFactory
 */

/*!
  \fn QOrganizerManagerEngine::QOrganizerManagerEngine()

  A default, empty constructor.
 */

/*!
  \fn QOrganizerManagerEngine::dataChanged()

  This signal is emitted some time after changes occur to the data managed by this
  engine, and the engine is unable to determine which changes occurred, or if the
  engine considers the changes to be radical enough to require clients to reload all data.

  If this signal is emitted, no other signals may be emitted for the associated changes.

  As it is possible that other processes (or other devices) may have caused the
  changes, the timing can not be determined.

  \sa itemsAdded(), itemsChanged(), itemsRemoved()
 */

/*!
  \fn QOrganizerManagerEngine::itemsAdded(const QList<QOrganizerItemId>& organizeritemIds);

  This signal is emitted some time after a set of organizer items has been added to
  this engine where the \l dataChanged() signal was not emitted for those changes.
  As it is possible that other processes (or other devices) may
  have added the organizer items, the timing cannot be determined.

  The list of ids of organizer items added is given by \a organizeritemIds.  There may be one or more
  ids in the list.

  \sa dataChanged()
 */

/*!
  \fn QOrganizerManagerEngine::itemsChanged(const QList<QOrganizerItemId>& organizeritemIds);

  This signal is emitted some time after a set of organizer items has been modified in
  this engine where the \l dataChanged() signal was not emitted for those changes.
  As it is possible that other processes (or other devices) may
  have modified the organizer items, the timing cannot be determined.

  The list of ids of changed organizer items is given by \a organizeritemIds.  There may be one or more
  ids in the list.

  \sa dataChanged()
 */

/*!
  \fn QOrganizerManagerEngine::itemsRemoved(const QList<QOrganizerItemId>& organizeritemIds);

  This signal is emitted some time after a set of organizer items has been removed from
  this engine where the \l dataChanged() signal was not emitted for those changes.
  As it is possible that other processes (or other devices) may
  have removed the organizer items, the timing cannot be determined.

  The list of ids of removed organizer items is given by \a organizeritemIds.  There may be one or more
  ids in the list.

  \sa dataChanged()
 */

/*!
  \fn QOrganizerManagerEngine::collectionsAdded(const QList<QOrganizerCollectionId>& collectionIds)
  This signal should be emitted at some point once the collections identified by \a collectionIds have been added to a datastore managed by this engine.
  This signal must not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
  \fn QOrganizerManagerEngine::collectionsChanged(const QList<QOrganizerCollectionId>& collectionIds)
  This signal should be emitted at some point once the metadata for the collections identified by \a collectionIds have been modified in a datastore managed by this engine.
  This signal is not emitted if one of the items in this collection has changed - itemsChanged() will be emitted instead.
  This signal must not be emitted if the dataChanged() signal was previously emitted for these changes.
 */

/*!
  \fn QOrganizerManagerEngine::collectionsRemoved(const QList<QOrganizerCollectionId>& collectionIds)
  This signal should be emitted at some point once the collections identified by \a collectionIds have been removed from a datastore managed by this engine.
  This signal must not be emitted if the dataChanged() signal was previously emitted for these changes.
 */



/*! Returns the manager name for this QOrganizerManagerEngine */
QString QOrganizerManagerEngine::managerName() const
{
    return QString(QLatin1String("base"));
}

/*!
  Returns the parameters with which this engine was constructed.  Note that
  the engine may have discarded unused or invalid parameters at the time of
  construction, and these will not be returned.
 */
QMap<QString, QString> QOrganizerManagerEngine::managerParameters() const
{
    return QMap<QString, QString>(); // default implementation requires no parameters.
}

/*!
  Returns the unique URI of this manager, which is built from the manager name and the parameters
  used to construct it.
 */
QString QOrganizerManagerEngine::managerUri() const
{
    return QOrganizerManager::buildUri(managerName(), managerParameters());
}

/*!
  Return the list of a maximum of \a maxCount organizer item instances which are occurrences of the
  given \a parentItem recurring item, which occur between the given \a periodStart date and the given
  \a periodEnd date.

  If \a periodStart is after \a periodEnd, the operation will fail, and \a error will be set to \c
  QOrganizerManager::BadArgumentError.
  If \a maxCount is negative, it is backend specific as to how many occurrences will be returned.
  Some backends may return no instances, others may return some limited number of occurrences.

  If the \a parentItem is an item of type QOrganizerItemType::TypeEvent, a list of items of type
  QOrganizerItemType::TypeEventOccurrence will be returned, representing the expansion of the
  parent item according to its QOrganizerItemRecurrence detail.  Similarly, a \a parentItem of type
  QOrganizerItemType::TypeTodo will result in a list of QOrganizerItemType::TypeTodoOccurrence
  items.  If the \a parentItem is of any other type, it is returned by itself from the backend.

  The occurrence-typed items returned should have a QOrganizerItemParent detail that refers
  to the parent item and the original instance that the event would have occurred on (if it is an
  exception).  No returned item should contain a QOrganizerItemRecurrence detail.

  If the \a parentItem does not exist in the backend, or if there are no instances matching the
  criteria, an empty list should be returned.

  The \a fetchHint parameter is a hint to the manager about which details the client is interested
  in.  It allows the manager to optimize retrieval of occurrences.  The manager may ignore the
  \a fetchHint, but if it does so each item occurrence it returns must include all of the details
  associated with it in the database.
  */
QList<QOrganizerItem> QOrganizerManagerEngine::itemOccurrences(const QOrganizerItem& parentItem, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const
{
    Q_UNUSED(parentItem);
    Q_UNUSED(periodStart);
    Q_UNUSED(periodEnd);
    Q_UNUSED(maxCount);
    Q_UNUSED(fetchHint);

    *error = QOrganizerManager::NotSupportedError;
    return QList<QOrganizerItem>();
}

/*!
  Returns a list of organizer item ids that match the given \a filter, sorted according to the given
  list of \a sortOrders, for any item which occurs (or has an occurrence which occurs) in the range
  specified by the given \a startDate and \a endDate.  A default-constructed (invalid) \a startDate
  specifies an open start date (matches anything which occurs up until the \a endDate), and a
  default-constructed (invalid) \a endDate specifies an open end date (matches anything which occurs
  after the \a startDate).  If both the \a startDate and \a endDate are invalid, this function will
  return the ids of all items which match the \a filter criteria.

  Depending on the backend, this filtering operation may involve retrieving
  all the organizer items.  Any error which occurs will be saved in \a error.
 */
QList<QOrganizerItemId> QOrganizerManagerEngine::itemIds(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerManager::Error* error) const
{
    Q_UNUSED(startDate);
    Q_UNUSED(endDate);
    Q_UNUSED(filter);
    Q_UNUSED(sortOrders);

    *error = QOrganizerManager::NotSupportedError;
    return QList<QOrganizerItemId>();
}

/*!
  Returns the list of organizer items which match the given \a filter stored in the manager sorted according to the given list of \a sortOrders,
  for any item or item occurrence which occurs in the range specified by the given \a startDate and \a endDate.
  A default-constructed (invalid) \a startDate specifies an open start date (matches anything which occurs up until the \a endDate),
  and a default-constructed (invalid) \a endDate specifies an open end date (matches anything which occurs after the \a startDate).
  If both the \a startDate and \a endDate are invalid, this function will return all items which match the \a filter criteria.

  Any operation error which occurs will be saved in \a error.

  The \a fetchHint parameter describes the optimization hints that a manager may take.
  If the \a fetchHint is the default constructed hint, all existing details and relationships
  in the matching organizer items will be returned.  A client should not make changes to an item which has
  been retrieved using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the item back to the manager (as the "new" restricted item will
  replace the previously saved item in the backend).

  \sa QOrganizerItemFetchHint
 */
QList<QOrganizerItem> QOrganizerManagerEngine::items(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const
{
    Q_UNUSED(startDate);
    Q_UNUSED(endDate);
    Q_UNUSED(filter);
    Q_UNUSED(sortOrders);
    Q_UNUSED(fetchHint);
    *error = QOrganizerManager::NotSupportedError;
    return QList<QOrganizerItem>();
}

/*!
  Returns the list of organizer items which match the given \a filter stored in the manager sorted according to the given list of \a sortOrders,
  for any persisted item which occurs (or has an occurrence which occurs) in the range specified by the given \a startDate and \a endDate.
  A default-constructed (invalid) \a startDate specifies an open start date (matches anything which occurs up until the \a endDate),
  and a default-constructed (invalid) \a endDate specifies an open end date (matches anything which occurs after the \a startDate).
  If both the \a startDate and \a endDate are invalid, this function will return all items which match the \a filter criteria.

  Any operation error which occurs will be saved in \a error.

  The \a fetchHint parameter describes the optimization hints that a manager may take.
  If the \a fetchHint is the default constructed hint, all existing details and relationships
  in the matching organizer items will be returned.  A client should not make changes to an item which has
  been retrieved using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the item back to the manager (as the "new" restricted item will
  replace the previously saved item in the backend).

  Items of type EventOccurrence and TodoOccurrence should only be returned when they represent an
  exceptional occurrence; ie. if the client has specifically saved the item occurrence in the
  manager.  Occurrence-typed items that are generated purely from a recurrence specification of
  another detail should not be returned in this list.

  All items returned should have a non-zero ID.

  \sa QOrganizerItemFetchHint
 */
QList<QOrganizerItem> QOrganizerManagerEngine::itemsForExport(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const
{
    Q_UNUSED(startDate);
    Q_UNUSED(endDate);
    Q_UNUSED(filter);
    Q_UNUSED(sortOrders);
    Q_UNUSED(fetchHint);
    *error = QOrganizerManager::NotSupportedError;
    return QList<QOrganizerItem>();
}

/*!
  Returns the organizer item in the database identified by \a organizeritemId.

  If the item does not exist, an empty, default constructed QOrganizerItem will be returned,
  and the \a error will be set to  \c QOrganizerManager::DoesNotExistError.

  Any operation error which occurs will be saved in \a error.

  The \a fetchHint parameter describes the optimization hints that a manager may take.
  If the \a fetchHint is the default constructed hint, all existing details and relationships
  in the matching item will be returned.  A client should not make changes to an item which has
  been retrieved using a fetch hint other than the default fetch hint.  Doing so will result in information
  loss when saving the item back to the manager (as the "new" restricted item will
  replace the previously saved item in the backend).

  \sa QOrganizerItemFetchHint
 */
QOrganizerItem QOrganizerManagerEngine::item(const QOrganizerItemId& organizeritemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const
{
    Q_UNUSED(organizeritemId);
    Q_UNUSED(fetchHint);
    *error = QOrganizerManager::NotSupportedError;
    return QOrganizerItem();
}

/*!
  Returns true if the given \a feature is supported by this engine for organizer items of the given \a organizeritemType
 */
bool QOrganizerManagerEngine::hasFeature(QOrganizerManager::ManagerFeature feature, const QString& organizeritemType) const
{
    Q_UNUSED(feature);
    Q_UNUSED(organizeritemType);

    return false;
}

/*!
  Given an input \a filter, returns the canonical version of the filter.

  Some of the following transformations may be applied:
  \list
   \o Any QOrganizerItemInvalidFilters contained in a union filter will be removed
   \o Any default QOrganizerItemFilters contained in an intersection filter will be removed
   \o Any QOrganizerItemIntersectionFilters with a QOrganizerItemInvalidFilter contained will be
     replaced with a QOrganizerItemInvalidFilter
   \o Any QOrganizerItemUnionFilters with a default QOrganizerItemFilter contained will be replaced
     with a default QOrganizerItemFilter
   \o An empty QOrganizerItemIntersectionFilter will be replaced with a QOrganizerItemDefaultFilter
   \o An empty QOrganizerItemUnionFilter will be replaced with a QOrganizerItemInvalidFilter
   \o An empty QOrganizerItemIdFilter will be replaced with a QOrganizerItemInvalidFilter
   \o An intersection or union filter with a single entry will be replaced by that entry
   \o A QOrganizerItemDetailFilter or QOrganizerItemDetailRangeFilter with no definition name will be replaced with a QOrganizerItemInvalidFilter
   \o A QOrganizerItemDetailRangeFilter with no range specified will be converted to a QOrganizerItemDetailFilter
  \endlist
*/
QOrganizerItemFilter QOrganizerManagerEngine::canonicalizedFilter(const QOrganizerItemFilter &filter)
{
    switch(filter.type()) {
        case QOrganizerItemFilter::IntersectionFilter:
        {
            QOrganizerItemIntersectionFilter f(filter);
            QList<QOrganizerItemFilter> filters = f.filters();
            QList<QOrganizerItemFilter>::iterator it = filters.begin();

            // XXX in theory we can remove duplicates in a set filter
            while (it != filters.end()) {
                QOrganizerItemFilter canon = canonicalizedFilter(*it);
                if (canon.type() == QOrganizerItemFilter::DefaultFilter) {
                    it = filters.erase(it);
                } else if (canon.type() == QOrganizerItemFilter::InvalidFilter) {
                    return QOrganizerItemInvalidFilter();
                } else {
                    *it = canon;
                    ++it;
                }
            }

            if (filters.count() == 0)
                return QOrganizerItemFilter();
            if (filters.count() == 1)
                return filters.first();

            f.setFilters(filters);
            return f;
        }
        // unreachable

        case QOrganizerItemFilter::UnionFilter:
        {
            QOrganizerItemUnionFilter f(filter);
            QList<QOrganizerItemFilter> filters = f.filters();
            QList<QOrganizerItemFilter>::iterator it = filters.begin();

            // XXX in theory we can remove duplicates in a set filter
            while (it != filters.end()) {
                QOrganizerItemFilter canon = canonicalizedFilter(*it);
                if (canon.type() == QOrganizerItemFilter::InvalidFilter) {
                    it = filters.erase(it);
                } else if (canon.type() == QOrganizerItemFilter::DefaultFilter) {
                    return QOrganizerItemFilter();
                } else {
                    *it = canon;
                    ++it;
                }
            }

            if (filters.count() == 0)
                return QOrganizerItemInvalidFilter();
            if (filters.count() == 1)
                return filters.first();

            f.setFilters(filters);
            return f;
        }
        // unreachable

        case QOrganizerItemFilter::IdFilter:
        {
            QOrganizerItemIdFilter f(filter);
            if (f.ids().count() == 0)
                return QOrganizerItemInvalidFilter();
        }
        break; // fall through to return at end

        case QOrganizerItemFilter::OrganizerItemDetailRangeFilter:
        {
            QOrganizerItemDetailRangeFilter f(filter);
            if (f.detailDefinitionName().isEmpty())
                return QOrganizerItemInvalidFilter();
            if (f.minValue() == f.maxValue()
                && f.rangeFlags() == (QOrganizerItemDetailRangeFilter::ExcludeLower | QOrganizerItemDetailRangeFilter::ExcludeUpper))
                return QOrganizerItemInvalidFilter();
            if ((f.minValue().isNull() && f.maxValue().isNull()) || (f.minValue() == f.maxValue())) {
                QOrganizerItemDetailFilter df;
                df.setDetailDefinitionName(f.detailDefinitionName(), f.detailFieldName());
                df.setMatchFlags(f.matchFlags());
                df.setValue(f.minValue());
                return df;
            }
        }
        break; // fall through to return at end

        case QOrganizerItemFilter::OrganizerItemDetailFilter:
        {
            QOrganizerItemDetailFilter f(filter);
            if (f.detailDefinitionName().isEmpty())
                return QOrganizerItemInvalidFilter();
        }
        break; // fall through to return at end

        default:
            break; // fall through to return at end
    }
    return filter;
}


/*!
  Returns a whether the supplied \a filter can be implemented
  natively by this engine.  If not, the base class implementation
  will emulate the functionality.
 */
bool QOrganizerManagerEngine::isFilterSupported(const QOrganizerItemFilter& filter) const
{
    Q_UNUSED(filter);

    return false;
}

/*!
  Returns the list of item types which are supported by this engine.
  This is a convenience function, equivalent to retrieving the allowable values
  for the \c QOrganizerItemType::FieldType field of the QOrganizerItemType definition
  which is valid in this engine.
 */
QStringList QOrganizerManagerEngine::supportedItemTypes() const
{
    QOrganizerManager::Error error;
    // XXX TODO: ensure that the TYPE field value for EVERY SINGLE TYPE contains all possible types...
    // XXX TODO: don't use TypeNote because some collections won't support Notes, only Journals / Events...
    QList<QVariant> allowableVals = detailDefinition(QOrganizerItemType::DefinitionName, QOrganizerItemType::TypeNote, &error).fields().value(QOrganizerItemType::FieldType).allowableValues();
    QStringList retn;
    for (int i = 0; i < allowableVals.size(); i++)
        retn += allowableVals.at(i).toString();
    return retn;
}

/*!
  \fn int QOrganizerManagerEngine::managerVersion() const

  Returns the engine backend implementation version number
 */

/*!
   Returns the default schema definitions for the given \a version of the schema.
   Version 1 of the schema corresponds to version 1.1 of the Qt Mobility APIs.
 */
QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > QOrganizerManagerEngine::schemaDefinitions(int version)
{
    // This implementation provides the base schema.
    // The schema documentation (organizeritemsschema.qdoc)
    // MUST BE KEPT UP TO DATE as definitions are added here.

    // the map we will eventually return
    QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > retnSchema;
    QMap<QString, QOrganizerItemDetailDefinition> retn; // each type has a different schema.

    // local variables for reuse
    QMap<QString, QOrganizerItemDetailFieldDefinition> fields;
    QOrganizerItemDetailFieldDefinition f;
    QOrganizerItemDetailDefinition d;

    // build the schema for the NOTEs  =============================
    retn.clear();

    // type
    d.setName(QOrganizerItemType::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList()
                         << QString(QLatin1String(QOrganizerItemType::TypeEvent))
                         << QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence))
                         << QString(QLatin1String(QOrganizerItemType::TypeJournal))
                         << QString(QLatin1String(QOrganizerItemType::TypeNote))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodo))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodoOccurrence)));
    fields.insert(QOrganizerItemType::FieldType, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // guid
    d.setName(QOrganizerItemGuid::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemGuid::FieldGuid, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // timestamp
    d.setName(QOrganizerItemTimestamp::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemTimestamp::FieldModificationTimestamp, f);
    fields.insert(QOrganizerItemTimestamp::FieldCreationTimestamp, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // display label
    d.setName(QOrganizerItemDisplayLabel::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDisplayLabel::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // description
    d.setName(QOrganizerItemDescription::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDescription::FieldDescription, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // comment
    d.setName(QOrganizerItemComment::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemComment::FieldComment, f);
    d.setFields(fields);
    d.setUnique(false);
    retn.insert(d.name(), d);

    retnSchema.insert(QOrganizerItemType::TypeNote, retn);

    // and then again for EVENTs =============================
    retn.clear();

    // type
    d.setName(QOrganizerItemType::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList()
                         << QString(QLatin1String(QOrganizerItemType::TypeEvent))
                         << QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence))
                         << QString(QLatin1String(QOrganizerItemType::TypeJournal))
                         << QString(QLatin1String(QOrganizerItemType::TypeNote))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodo))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodoOccurrence)));
    fields.insert(QOrganizerItemType::FieldType, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // guid
    d.setName(QOrganizerItemGuid::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemGuid::FieldGuid, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // timestamp
    d.setName(QOrganizerItemTimestamp::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemTimestamp::FieldModificationTimestamp, f);
    fields.insert(QOrganizerItemTimestamp::FieldCreationTimestamp, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // display label
    d.setName(QOrganizerItemDisplayLabel::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDisplayLabel::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // description
    d.setName(QOrganizerItemDescription::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDescription::FieldDescription, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // recurrence information
    d.setName(QOrganizerItemRecurrence::DefinitionName);
    fields.clear();
    f.setDataType(qMetaTypeId< QSet<QDate> >());
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemRecurrence::FieldExceptionDates, f);
    fields.insert(QOrganizerItemRecurrence::FieldRecurrenceDates, f);
    f.setDataType(qMetaTypeId< QSet<QOrganizerRecurrenceRule> >());
    fields.insert(QOrganizerItemRecurrence::FieldExceptionRules, f);
    fields.insert(QOrganizerItemRecurrence::FieldRecurrenceRules, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // reminder
    d.setName(QOrganizerItemReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event
    retn.insert(d.name(), d);

    // audible reminder
    d.setName(QOrganizerItemAudibleReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemAudibleReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event
    retn.insert(d.name(), d);

    // email reminder
    d.setName(QOrganizerItemEmailReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemEmailReminder::FieldBody, f);
    fields.insert(QOrganizerItemEmailReminder::FieldSubject, f);
    f.setDataType(QVariant::StringList);
    fields.insert(QOrganizerItemEmailReminder::FieldRecipients, f);
    f.setDataType(QVariant::List);
    fields.insert(QOrganizerItemEmailReminder::FieldAttachments, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event
    retn.insert(d.name(), d);

    // visual reminder
    d.setName(QOrganizerItemVisualReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemVisualReminder::FieldMessage, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemVisualReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event
    retn.insert(d.name(), d);

    // event time range
    d.setName(QOrganizerEventTime::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerEventTime::FieldStartDateTime, f);
    fields.insert(QOrganizerEventTime::FieldEndDateTime, f);
    f.setDataType(QVariant::Bool);
    fields.insert(QOrganizerEventTime::FieldAllDay, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // priority
    d.setName(QOrganizerItemPriority::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList()
                         << static_cast<int>(QOrganizerItemPriority::UnknownPriority)
                         << static_cast<int>(QOrganizerItemPriority::HighestPriority)
                         << static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryHighPriority)
                         << static_cast<int>(QOrganizerItemPriority::HighPriority)
                         << static_cast<int>(QOrganizerItemPriority::MediumPriority)
                         << static_cast<int>(QOrganizerItemPriority::LowPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryLowPriority)
                         << static_cast<int>(QOrganizerItemPriority::ExtremelyLowPriority)
                         << static_cast<int>(QOrganizerItemPriority::LowestPriority));
    fields.insert(QOrganizerItemPriority::FieldPriority, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // location
    d.setName(QOrganizerItemLocation::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Double);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemLocation::FieldLatitude, f);
    fields.insert(QOrganizerItemLocation::FieldLongitude, f);
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemLocation::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // comment
    d.setName(QOrganizerItemComment::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemComment::FieldComment, f);
    d.setFields(fields);
    d.setUnique(false);
    retn.insert(d.name(), d);

    retnSchema.insert(QOrganizerItemType::TypeEvent, retn);

    // and then again for EVENTOCCURRENCEs =============================
    retn.clear();

    // type
    d.setName(QOrganizerItemType::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList()
                         << QString(QLatin1String(QOrganizerItemType::TypeEvent))
                         << QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence))
                         << QString(QLatin1String(QOrganizerItemType::TypeJournal))
                         << QString(QLatin1String(QOrganizerItemType::TypeNote))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodo))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodoOccurrence)));
    fields.insert(QOrganizerItemType::FieldType, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // guid
    d.setName(QOrganizerItemGuid::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemGuid::FieldGuid, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // timestamp
    d.setName(QOrganizerItemTimestamp::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemTimestamp::FieldModificationTimestamp, f);
    fields.insert(QOrganizerItemTimestamp::FieldCreationTimestamp, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // display label
    d.setName(QOrganizerItemDisplayLabel::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDisplayLabel::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // description
    d.setName(QOrganizerItemDescription::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDescription::FieldDescription, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // instance origin
    d.setName(QOrganizerItemParent::DefinitionName);
    fields.clear();
    f.setDataType(qMetaTypeId<QOrganizerItemId>());
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemParent::FieldParentId, f);
    f.setDataType(QVariant::Date);
    fields.insert(QOrganizerItemParent::FieldOriginalDate, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // event time range
    d.setName(QOrganizerEventTime::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerEventTime::FieldStartDateTime, f);
    fields.insert(QOrganizerEventTime::FieldEndDateTime, f);
    f.setDataType(QVariant::Bool);
    fields.insert(QOrganizerEventTime::FieldAllDay, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // priority
    d.setName(QOrganizerItemPriority::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList()
                         << static_cast<int>(QOrganizerItemPriority::UnknownPriority)
                         << static_cast<int>(QOrganizerItemPriority::HighestPriority)
                         << static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryHighPriority)
                         << static_cast<int>(QOrganizerItemPriority::HighPriority)
                         << static_cast<int>(QOrganizerItemPriority::MediumPriority)
                         << static_cast<int>(QOrganizerItemPriority::LowPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryLowPriority)
                         << static_cast<int>(QOrganizerItemPriority::ExtremelyLowPriority)
                         << static_cast<int>(QOrganizerItemPriority::LowestPriority));
    fields.insert(QOrganizerItemPriority::FieldPriority, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // location
    d.setName(QOrganizerItemLocation::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Double);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemLocation::FieldLatitude, f);
    fields.insert(QOrganizerItemLocation::FieldLongitude, f);
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemLocation::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // reminder
    d.setName(QOrganizerItemReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event occurrence
    retn.insert(d.name(), d);

    // audible reminder
    d.setName(QOrganizerItemAudibleReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemAudibleReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event occurrence
    retn.insert(d.name(), d);

    // email reminder
    d.setName(QOrganizerItemEmailReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemEmailReminder::FieldBody, f);
    fields.insert(QOrganizerItemEmailReminder::FieldSubject, f);
    f.setDataType(QVariant::StringList);
    fields.insert(QOrganizerItemEmailReminder::FieldRecipients, f);
    f.setDataType(QVariant::List);
    fields.insert(QOrganizerItemEmailReminder::FieldAttachments, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event occurrence
    retn.insert(d.name(), d);

    // visual reminder
    d.setName(QOrganizerItemVisualReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemVisualReminder::FieldMessage, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemVisualReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same event occurrence
    retn.insert(d.name(), d);

    // comment
    d.setName(QOrganizerItemComment::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemComment::FieldComment, f);
    d.setFields(fields);
    d.setUnique(false);
    retn.insert(d.name(), d);

    retnSchema.insert(QOrganizerItemType::TypeEventOccurrence, retn);

    // and then again for TODOs =============================
    retn.clear();

    // type
    d.setName(QOrganizerItemType::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList()
                         << QString(QLatin1String(QOrganizerItemType::TypeEvent))
                         << QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence))
                         << QString(QLatin1String(QOrganizerItemType::TypeJournal))
                         << QString(QLatin1String(QOrganizerItemType::TypeNote))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodo))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodoOccurrence)));
    fields.insert(QOrganizerItemType::FieldType, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // guid
    d.setName(QOrganizerItemGuid::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemGuid::FieldGuid, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // timestamp
    d.setName(QOrganizerItemTimestamp::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemTimestamp::FieldModificationTimestamp, f);
    fields.insert(QOrganizerItemTimestamp::FieldCreationTimestamp, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // display label
    d.setName(QOrganizerItemDisplayLabel::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDisplayLabel::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // description
    d.setName(QOrganizerItemDescription::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDescription::FieldDescription, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // recurrence information
    d.setName(QOrganizerItemRecurrence::DefinitionName);
    fields.clear();
    f.setDataType(qMetaTypeId< QSet<QDate> >());
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemRecurrence::FieldExceptionDates, f);
    fields.insert(QOrganizerItemRecurrence::FieldRecurrenceDates, f);
    f.setDataType(qMetaTypeId< QSet<QOrganizerRecurrenceRule> >());
    fields.insert(QOrganizerItemRecurrence::FieldExceptionRules, f);
    fields.insert(QOrganizerItemRecurrence::FieldRecurrenceRules, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // reminder
    d.setName(QOrganizerItemReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo
    retn.insert(d.name(), d);

    // audible reminder
    d.setName(QOrganizerItemAudibleReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemAudibleReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo
    retn.insert(d.name(), d);

    // email reminder
    d.setName(QOrganizerItemEmailReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemEmailReminder::FieldBody, f);
    fields.insert(QOrganizerItemEmailReminder::FieldSubject, f);
    f.setDataType(QVariant::StringList);
    fields.insert(QOrganizerItemEmailReminder::FieldRecipients, f);
    f.setDataType(QVariant::List);
    fields.insert(QOrganizerItemEmailReminder::FieldAttachments, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo
    retn.insert(d.name(), d);

    // visual reminder
    d.setName(QOrganizerItemVisualReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemVisualReminder::FieldMessage, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemVisualReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo
    retn.insert(d.name(), d);

    // todo progress
    d.setName(QOrganizerTodoProgress::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerTodoProgress::FieldFinishedDateTime, f);
    f.setDataType(QVariant::Int);
    fields.insert(QOrganizerTodoProgress::FieldPercentageComplete, f);
    fields.insert(QOrganizerTodoProgress::FieldStatus, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // todo time range
    d.setName(QOrganizerTodoTime::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerTodoTime::FieldStartDateTime, f);
    fields.insert(QOrganizerTodoTime::FieldDueDateTime, f);
    f.setDataType(QVariant::Bool);
    fields.insert(QOrganizerTodoTime::FieldAllDay, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // priority
    d.setName(QOrganizerItemPriority::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList()
                         << static_cast<int>(QOrganizerItemPriority::UnknownPriority)
                         << static_cast<int>(QOrganizerItemPriority::HighestPriority)
                         << static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryHighPriority)
                         << static_cast<int>(QOrganizerItemPriority::HighPriority)
                         << static_cast<int>(QOrganizerItemPriority::MediumPriority)
                         << static_cast<int>(QOrganizerItemPriority::LowPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryLowPriority)
                         << static_cast<int>(QOrganizerItemPriority::ExtremelyLowPriority)
                         << static_cast<int>(QOrganizerItemPriority::LowestPriority));
    fields.insert(QOrganizerItemPriority::FieldPriority, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // comment
    d.setName(QOrganizerItemComment::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemComment::FieldComment, f);
    d.setFields(fields);
    d.setUnique(false);
    retn.insert(d.name(), d);

    retnSchema.insert(QOrganizerItemType::TypeTodo, retn);

    // and then again for TODOOCCURRENCEs =============================
    retn.clear();

    // type
    d.setName(QOrganizerItemType::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList()
                         << QString(QLatin1String(QOrganizerItemType::TypeEvent))
                         << QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence))
                         << QString(QLatin1String(QOrganizerItemType::TypeJournal))
                         << QString(QLatin1String(QOrganizerItemType::TypeNote))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodo))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodoOccurrence)));
    fields.insert(QOrganizerItemType::FieldType, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // guid
    d.setName(QOrganizerItemGuid::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemGuid::FieldGuid, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // timestamp
    d.setName(QOrganizerItemTimestamp::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemTimestamp::FieldModificationTimestamp, f);
    fields.insert(QOrganizerItemTimestamp::FieldCreationTimestamp, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // display label
    d.setName(QOrganizerItemDisplayLabel::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDisplayLabel::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // description
    d.setName(QOrganizerItemDescription::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDescription::FieldDescription, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // instance origin
    d.setName(QOrganizerItemParent::DefinitionName);
    fields.clear();
    f.setDataType(qMetaTypeId<QOrganizerItemId>());
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemParent::FieldParentId, f);
    f.setDataType(QVariant::Date);
    fields.insert(QOrganizerItemParent::FieldOriginalDate, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // todo progress
    d.setName(QOrganizerTodoProgress::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerTodoProgress::FieldFinishedDateTime, f);
    f.setDataType(QVariant::Int);
    fields.insert(QOrganizerTodoProgress::FieldPercentageComplete, f);
    fields.insert(QOrganizerTodoProgress::FieldStatus, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // reminder
    d.setName(QOrganizerItemReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo occurrence
    retn.insert(d.name(), d);

    // audible reminder
    d.setName(QOrganizerItemAudibleReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemAudibleReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo occurrence
    retn.insert(d.name(), d);

    // email reminder
    d.setName(QOrganizerItemEmailReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemEmailReminder::FieldBody, f);
    fields.insert(QOrganizerItemEmailReminder::FieldSubject, f);
    f.setDataType(QVariant::StringList);
    fields.insert(QOrganizerItemEmailReminder::FieldRecipients, f);
    f.setDataType(QVariant::List);
    fields.insert(QOrganizerItemEmailReminder::FieldAttachments, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo occurrence
    retn.insert(d.name(), d);

    // visual reminder
    d.setName(QOrganizerItemVisualReminder::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemReminder::FieldSecondsBeforeStart, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionCount, f);
    fields.insert(QOrganizerItemReminder::FieldRepetitionDelay, f);
    f.setDataType(QVariant::String);
    fields.insert(QOrganizerItemVisualReminder::FieldMessage, f);
    f.setDataType(QVariant::Url);
    fields.insert(QOrganizerItemVisualReminder::FieldDataUrl, f);
    d.setFields(fields);
    d.setUnique(false); // can have multiple alarms at different times for the same todo occurrence
    retn.insert(d.name(), d);

    // todo time range
    d.setName(QOrganizerTodoTime::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerTodoTime::FieldStartDateTime, f);
    fields.insert(QOrganizerTodoTime::FieldDueDateTime, f);
    f.setDataType(QVariant::Bool);
    fields.insert(QOrganizerTodoTime::FieldAllDay, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // priority
    d.setName(QOrganizerItemPriority::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::Int);
    f.setAllowableValues(QVariantList()
                         << static_cast<int>(QOrganizerItemPriority::UnknownPriority)
                         << static_cast<int>(QOrganizerItemPriority::HighestPriority)
                         << static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryHighPriority)
                         << static_cast<int>(QOrganizerItemPriority::HighPriority)
                         << static_cast<int>(QOrganizerItemPriority::MediumPriority)
                         << static_cast<int>(QOrganizerItemPriority::LowPriority)
                         << static_cast<int>(QOrganizerItemPriority::VeryLowPriority)
                         << static_cast<int>(QOrganizerItemPriority::ExtremelyLowPriority)
                         << static_cast<int>(QOrganizerItemPriority::LowestPriority));
    fields.insert(QOrganizerItemPriority::FieldPriority, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // comment
    d.setName(QOrganizerItemComment::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemComment::FieldComment, f);
    d.setFields(fields);
    d.setUnique(false);
    retn.insert(d.name(), d);

    retnSchema.insert(QOrganizerItemType::TypeTodoOccurrence, retn);

    // and then again for JOURNALs =============================
    retn.clear();

    // type
    d.setName(QOrganizerItemType::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList()
                         << QString(QLatin1String(QOrganizerItemType::TypeEvent))
                         << QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence))
                         << QString(QLatin1String(QOrganizerItemType::TypeJournal))
                         << QString(QLatin1String(QOrganizerItemType::TypeNote))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodo))
                         << QString(QLatin1String(QOrganizerItemType::TypeTodoOccurrence)));
    fields.insert(QOrganizerItemType::FieldType, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // guid
    d.setName(QOrganizerItemGuid::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemGuid::FieldGuid, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // timestamp
    d.setName(QOrganizerItemTimestamp::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemTimestamp::FieldModificationTimestamp, f);
    fields.insert(QOrganizerItemTimestamp::FieldCreationTimestamp, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // display label
    d.setName(QOrganizerItemDisplayLabel::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDisplayLabel::FieldLabel, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // description
    d.setName(QOrganizerItemDescription::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemDescription::FieldDescription, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // journal time range
    d.setName(QOrganizerJournalTime::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::DateTime);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerJournalTime::FieldEntryDateTime, f);
    d.setFields(fields);
    d.setUnique(true);
    retn.insert(d.name(), d);

    // comment
    d.setName(QOrganizerItemComment::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QOrganizerItemComment::FieldComment, f);
    d.setFields(fields);
    d.setUnique(false);
    retn.insert(d.name(), d);

    retnSchema.insert(QOrganizerItemType::TypeJournal, retn);

    if (version == 1) {
        return retnSchema;
    }

    // the most recent version of the schema is version 1.
    QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > empty;
    return empty;
}

/*!
  Checks that the given item \a organizeritem does not have details which
  don't conform to a valid definition, violate uniqueness constraints,
  or contain values for nonexistent fields, and that the values contained are
  of the correct type for each field, and are allowable values for that field.

  Note that this function is unable to ensure that the access constraints
  (such as CreateOnly and ReadOnly) are observed; backend specific code
  must be written if you wish to enforce these constraints.

  Returns true if the \a organizeritem is valid according to the definitions for
  its details, otherwise returns false.

  Any errors encountered during this operation should be stored to
  \a error.
 */
bool QOrganizerManagerEngine::validateItem(const QOrganizerItem& organizeritem, QOrganizerManager::Error* error) const
{
    QList<QString> uniqueDefinitionIds;

    // check that each detail conforms to its definition as supported by this manager.
    foreach (const QOrganizerItemDetail& detail, organizeritem.details()) {
        QVariantMap values = detail.variantValues();
        QOrganizerItemDetailDefinition def = detailDefinition(detail.definitionName(), organizeritem.type(), error);
        // check that the definition is supported
        if (*error != QOrganizerManager::NoError) {
            *error = QOrganizerManager::InvalidDetailError;
            return false; // this definition is not supported.
        }

        // check uniqueness
        if (def.isUnique()) {
            if (uniqueDefinitionIds.contains(def.name())) {
                *error = QOrganizerManager::AlreadyExistsError;
                return false; // can't have two of a unique detail.
            }
            uniqueDefinitionIds.append(def.name());
        }

        QMapIterator<QString,QVariant> fieldIt(values);
        while (fieldIt.hasNext()) {
            fieldIt.next();
            const QString& key = fieldIt.key();
            const QVariant& variant = fieldIt.value();
            // check that no values exist for nonexistent fields.
            if (!def.fields().contains(key)) {
                *error = QOrganizerManager::InvalidDetailError;
                return false; // value for nonexistent field.
            }

            QOrganizerItemDetailFieldDefinition field = def.fields().value(key);
            // check that the type of each value corresponds to the allowable field type
            if (static_cast<int>(field.dataType()) != variant.userType()) {
                *error = QOrganizerManager::InvalidDetailError;
                return false; // type doesn't match.
            }

            // check that the value is allowable
            // if the allowable values is an empty list, any are allowed.
            if (!field.allowableValues().isEmpty()) {
                // if the field datatype is a list, check that it contains only allowable values
                if (field.dataType() == QVariant::List || field.dataType() == QVariant::StringList) {
                    QList<QVariant> innerValues = variant.toList();
                    QListIterator<QVariant> it(innerValues);
                    while (it.hasNext()) {
                        if (!field.allowableValues().contains(it.next())) {
                            *error = QOrganizerManager::InvalidDetailError;
                            return false; // value not allowed.
                        }
                    }
                } else if (!field.allowableValues().contains(variant)) {
                    // the datatype is not a list; the value wasn't allowed.
                    *error = QOrganizerManager::InvalidDetailError;
                    return false; // value not allowed.
                }
            }
        }
    }

    return true;
}

/*!
  Returns true if the \a collection is valid and can be saved in the engine.
  By default, modifiable collections are not supported, so this function returns false,
  and \a error is set to QOrganizerManager::NotSupportedError.
  Engines which do implement mutable collections should reimplement this function.
 */
bool QOrganizerManagerEngine::validateCollection(const QOrganizerCollection& collection, QOrganizerManager::Error* error) const
{
    Q_UNUSED(collection);
    *error = QOrganizerManager::NotSupportedError;
    return false;
}

/*!
  Checks that the given detail definition \a definition seems valid,
  with a correct id, defined fields, and any specified value types
  are supported by this engine.  This function is called before
  trying to save a definition.

  Returns true if the \a definition seems valid, otherwise returns
  false.

  Any errors encountered during this operation should be stored to
  \a error.
 */
bool QOrganizerManagerEngine::validateDefinition(const QOrganizerItemDetailDefinition& definition, QOrganizerManager::Error* error) const
{
    if (definition.name().isEmpty()) {
        *error = QOrganizerManager::BadArgumentError;
        return false;
    }

    if (definition.fields().count() == 0) {
        *error = QOrganizerManager::BadArgumentError;
        return false;
    }

    // Check each field now
    QMapIterator<QString, QOrganizerItemDetailFieldDefinition> it(definition.fields());
    while(it.hasNext()) {
        it.next();
        if (it.key().isEmpty()) {
            *error = QOrganizerManager::BadArgumentError;
            return false;
        }

        // Check that each allowed value is the same type
        for (int i=0; i < it.value().allowableValues().count(); i++) {
            if (it.value().allowableValues().at(i).userType() != it.value().dataType()) {
                *error = QOrganizerManager::BadArgumentError;
                return false;
            }
        }
    }
    *error = QOrganizerManager::NoError;
    return true;
}

/*!
  Returns the registered detail definitions which are valid for organizer items whose type is of the given \a organizeritemType in this engine.

  Any errors encountered during this operation should be stored to
  \a error.
 */
QMap<QString, QOrganizerItemDetailDefinition> QOrganizerManagerEngine::detailDefinitions(const QString& organizeritemType, QOrganizerManager::Error* error) const
{
    Q_UNUSED(organizeritemType);
    *error = QOrganizerManager::NotSupportedError;
    return QMap<QString, QOrganizerItemDetailDefinition>();
}

/*!
  Returns the definition identified by the given \a definitionName that
  is valid for organizer items whose type is of the given \a organizeritemType in this store, or a default-constructed QOrganizerItemDetailDefinition
  if no such definition exists

  Any errors encountered during this operation should be stored to
  \a error.
 */
QOrganizerItemDetailDefinition QOrganizerManagerEngine::detailDefinition(const QString& definitionName, const QString& organizeritemType, QOrganizerManager::Error* error) const
{
    QMap<QString, QOrganizerItemDetailDefinition> definitions = detailDefinitions(organizeritemType, error);
    if (definitions.contains(definitionName))  {
        *error = QOrganizerManager::NoError;
        return definitions.value(definitionName);
    } else {
        *error = QOrganizerManager::DoesNotExistError;
        return QOrganizerItemDetailDefinition();
    }
}

/*!
  Persists the given definition \a def in the database, which is valid for organizer items whose type is the given \a organizeritemType.

  Returns true if the definition was saved successfully, and otherwise returns false.

  The backend must emit the appropriate signals to inform clients of changes
  to the database resulting from this operation.

  Any errors encountered during this operation should be stored to
  \a error.
 */
bool QOrganizerManagerEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerManager::Error* error)
{
    Q_UNUSED(def);
    Q_UNUSED(organizeritemType);

    *error = QOrganizerManager::NotSupportedError;
    return false;
}

/*!
  Removes the definition identified by the given \a definitionName from the database, where it was valid for organizer items whose type was the given \a organizeritemType.

  Returns true if the definition was removed successfully, otherwise returns false.

  The backend must emit the appropriate signals to inform clients of changes
  to the database resulting from this operation.

  Any errors encountered during this operation should be stored to
  \a error.
 */
bool QOrganizerManagerEngine::removeDetailDefinition(const QString& definitionName, const QString& organizeritemType, QOrganizerManager::Error* error)
{
    Q_UNUSED(definitionName);
    Q_UNUSED(organizeritemType);

    *error = QOrganizerManager::NotSupportedError;
    return false;
}

/*!
  Sets the access constraints of \a detail to the supplied \a constraints.

  This function is provided to allow engine implementations to report the
  access constraints of retrieved details, without generally allowing the
  access constraints to be modified after retrieval.

  Application code should not call this function, since validation of the
  detail will happen in the engine in any case.
 */
void QOrganizerManagerEngine::setDetailAccessConstraints(QOrganizerItemDetail *detail, QOrganizerItemDetail::AccessConstraints constraints)
{
    if (detail) {
        QOrganizerItemDetailPrivate::setAccessConstraints(detail, constraints);
    }
}

/*!
  Adds the given \a organizeritem to the database if \a organizeritem has a
  default-constructed id, or an id with the manager URI set to the URI of
  this manager and a id of zero, otherwise updates the organizer item in
  the database which has the same id to be the given \a organizeritem.
  If the id is non-zero but does not identify any item stored in the
  manager, the function will return false and \a error will be set to
  \c QOrganizerManager::DoesNotExistError.

  The \a organizeritem will be added to the collection identified by the
  collectionId specified in the item (accessible via item->organizerId())
  if it exists, and the item conforms to the schema supported
  for that collection.  If the collection exists but the item does not conform
  to the schema supported for that collection, the function will return false,
  and the \a error will be set to QOrganizerManager::InvalidDetailError.

  If the collectionId is not the default (zero) id, but does not identify
  a valid collection, the function will return false, and \a error will be set
  to QOrganizerManager::InvalidCollectionError.  If the collectionId
  is the default (zero) id, the item should be saved in the collection in which
  it is already saved (if it is already saved in this manager), or in the default
  collection (if it is a new item in this manager).

  Returns true if the save operation completed successfully, otherwise
  returns false.  Any error which occurs will be saved in \a error.

  The default implementation will convert this into a call to saveItems.

  \sa managerUri()
 */
bool QOrganizerManagerEngine::saveItem(QOrganizerItem* organizeritem, QOrganizerManager::Error* error)
{
    // Convert to a list op
    if (organizeritem) {
        QList<QOrganizerItem> list;
        list.append(*organizeritem);

        QMap<int, QOrganizerManager::Error> errors;
        bool ret = saveItems(&list, &errors, error);

        if (errors.count() > 0)
            *error = errors.begin().value();

        *organizeritem = list.value(0);
        return ret;
    } else {
        *error = QOrganizerManager::BadArgumentError;
        return false;
    }
}

/*!
  Remove the item identified by \a organizeritemId from the database,
  and also removes any relationships in which the item was involved.
  Returns true if the item was removed successfully, otherwise
  returns false.

  Any error which occurs will be saved in \a error.

  The default implementation will convert this into a call to removeItems.
 */
bool QOrganizerManagerEngine::removeItem(const QOrganizerItemId& organizeritemId, QOrganizerManager::Error* error)
{
    // Convert to a list op
    QList<QOrganizerItemId> list;
    list.append(organizeritemId);

    QMap<int, QOrganizerManager::Error> errors;
    bool ret = removeItems(list, &errors, error);

    if (errors.count() > 0)
        *error = errors.begin().value();

    return ret;
}

/*!
  Adds the list of organizer items given by \a organizeritems list to the database.
  Returns true if the organizer items were saved successfully, otherwise false.

  The engine might populate \a errorMap (the map of indices of the \a organizeritems list to
  the error which occurred when saving the item at that index) for
  every index for which the item could not be saved, if it is able.
  The \l QOrganizerManager::error() function will only return \c QOrganizerManager::NoError
  if all organizer items were saved successfully.

  For each newly saved item that was successful, the id of the item
  in the \a organizeritems list will be updated with the new value.  If a failure occurs
  when saving a new item, the id will be cleared.

  Each item in the given list \a organizeritems will be added to the collection
  identified in the item (accessible via item->collectionId()) if it exists, and if
  the item conform to the schema supported for that collection.  If the collection
  exists but the item does not conform to the schema supported for that collection,
  the function will return false, and the error in the \a errorMap for the item at
  that index will be set to QOrganizerManager::InvalidDetailError.

  If the collectionId is not the default (zero) id, but does not identify
  a valid collection, the function will return false, and \a error will be set
  to QOrganizerManager::InvalidCollectionError.  If the collectionId
  is the default (zero) id, the item should be saved in the collection in which
  it is already saved (if they are already saved in this manager), or in the default
  collection (if they are new items in this manager).

  Any errors encountered during this operation should be stored to
  \a error.

  \sa QOrganizerManager::saveItem()
 */
bool QOrganizerManagerEngine::saveItems(QList<QOrganizerItem>* organizeritems, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error)
{
    Q_UNUSED(organizeritems);
    Q_UNUSED(errorMap);

    *error = QOrganizerManager::NotSupportedError;
    return false;
}

/*!
  Remove every item whose id is contained in the list of organizer items ids
  \a organizeritemIds.  Returns true if all organizer items were removed successfully,
  otherwise false.

  Any item that was removed successfully will have the relationships
  in which it was involved removed also.

  The manager might populate \a errorMap (the map of indices of the \a organizeritemIds list to
  the error which occurred when saving the item at that index) for every
  index for which the item could not be removed, if it is able.
  The \l QOrganizerManager::error() function will
  only return \c QOrganizerManager::NoError if all organizer items were removed
  successfully.

  If the list contains ids which do not identify a valid item in the manager, the function will
  remove any organizer items which are identified by ids in the \a organizeritemIds list, insert
  \c QOrganizerManager::DoesNotExist entries into the \a errorMap for the indices of invalid ids
  in the \a organizeritemIds list, return false, and set the overall operation error to
  \c QOrganizerManager::DoesNotExistError.

  Any errors encountered during this operation should be stored to
  \a error.

  \sa QOrganizerManager::removeItem()
 */
bool QOrganizerManagerEngine::removeItems(const QList<QOrganizerItemId>& organizeritemIds, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error)
{
    Q_UNUSED(organizeritemIds);
    Q_UNUSED(errorMap);
    *error = QOrganizerManager::NotSupportedError;
    return false;
}

/*!
    Returns the default collection of the manager.
    Any errors encountered during this operation should be stored to
   \a error.
*/
QOrganizerCollection QOrganizerManagerEngine::defaultCollection(QOrganizerManager::Error* error) const
{
    *error = QOrganizerManager::NotSupportedError;
    return QOrganizerCollection();
}

/*!
    Returns the collection identified by the given \a collectionId in the manager.
    Any errors encountered during this operation should be stored to \a error.
    If the given \a collectionId does not specify a valid collection, \a error will
    be set to \c QOrganizerManager::DoesNotExistError.
*/
QOrganizerCollection QOrganizerManagerEngine::collection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error) const
{
    Q_UNUSED(collectionId);
    *error = QOrganizerManager::NotSupportedError;
    return QOrganizerCollection();
}

/*!
    Returns the list of all of the collections managed by this manager.
    Any errors encountered during this operation should be stored to
    \a error.
 */
QList<QOrganizerCollection> QOrganizerManagerEngine::collections(QOrganizerManager::Error* error) const
{
    *error = QOrganizerManager::NotSupportedError;
    return QList<QOrganizerCollection>();
}

/*!
    Returns true if the saving of the \a collection was successfull otherwise false.
    Any errors encountered during this operation should be stored to
    \a error.
*/
bool QOrganizerManagerEngine::saveCollection(QOrganizerCollection* collection, QOrganizerManager::Error* error)
{
    Q_UNUSED(collection);

    *error = QOrganizerManager::NotSupportedError;
    return false;
}

/*!
    Returns true if the removing of the \a collectionId was successfull otherwise false.
    Any errors encountered during this operation should be stored to
    \a error.
*/
bool QOrganizerManagerEngine::removeCollection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error)
{
    Q_UNUSED(collectionId);

    *error = QOrganizerManager::NotSupportedError;
    return false;
}

/*!
  Returns a pruned or modified version of the \a original item which is valid and can be saved in the manager.
  The returned item might have details removed or arbitrarily changed.  The cache of relationships
  in the item are ignored entirely when considering compatibility with the backend, as they are
  saved and validated separately.  Any error which occurs will be saved to \a error.
 */
QOrganizerItem QOrganizerManagerEngine::compatibleItem(const QOrganizerItem& original, QOrganizerManager::Error* error) const
{
    QOrganizerItem conforming;
    conforming.setType(original.type());
    conforming.setId(original.id());
    QOrganizerManager::Error tempError;
    QList<QString> uniqueDefinitionIds;
    foreach (QOrganizerItemDetail detail, original.details()) {
        // check that the detail conforms to the definition in this manager.
        // if so, then add it to the conforming item to be returned.  if not, prune it.

        QVariantMap values = detail.variantValues();
        QOrganizerItemDetailDefinition def = detailDefinition(detail.definitionName(), original.type(), &tempError);
        // check that the definition is supported
        if (tempError != QOrganizerManager::NoError) {
            continue; // this definition is not supported.
        }

        // check uniqueness
        if (def.isUnique()) {
            if (uniqueDefinitionIds.contains(def.name())) {
                continue; // can't have two of a unique detail.
            }
            uniqueDefinitionIds.append(def.name());
        }

        QMapIterator<QString,QVariant> fieldIt(values);
        while (fieldIt.hasNext()) {
            fieldIt.next();
            const QString& key = fieldIt.key();
            const QVariant& variant = fieldIt.value();
            // prune values for nonexistent fields.
            if (!def.fields().contains(key)) {
                detail.removeValue(key);
            }

            QOrganizerItemDetailFieldDefinition field = def.fields().value(key);
            // prune values that do not correspond to the allowable field type
            if (static_cast<int>(field.dataType()) != variant.userType()) {
                detail.removeValue(key);
            }

            // check that the value is allowable
            // if the allowable values is an empty list, any are allowed.
            if (!field.allowableValues().isEmpty()) {
                // if the field datatype is a list, remove non-allowable values
                if (field.dataType() == QVariant::List || field.dataType() == QVariant::StringList) {
                    QList<QVariant> innerValues = variant.toList();
                    QMutableListIterator<QVariant> it(innerValues);
                    while (it.hasNext()) {
                        if (!field.allowableValues().contains(it.next())) {
                            it.remove();
                        }
                    }
                    if (innerValues.isEmpty()) {
                        detail.removeValue(key);
                    } else {
                        detail.setValue(key, innerValues);
                    }
                } else if (!field.allowableValues().contains(variant)) {
                    detail.removeValue(key);
                }
            }
        }

        // if it hasn't been pruned away to nothing, save it in the conforming item
        if (!detail.isEmpty()) {
            conforming.saveDetail(&detail);
        }
    }

    *error = QOrganizerManager::NoError;
    if (conforming.isEmpty())
        *error = QOrganizerManager::DoesNotExistError;
    return conforming;
}

/*!
  Returns a pruned or modified version of the \a original collection which is valid and can be saved in the manager.
  The returned item might have meta data removed or arbitrarily changed.  Any error which occurs will be saved to \a error.
  By default, modifiable collections are not supported, and so this function always returns false.
  Any engine which supports mutable collections should reimplement this function.
 */
QOrganizerCollection QOrganizerManagerEngine::compatibleCollection(const QOrganizerCollection& original, QOrganizerManager::Error* error) const
{
    Q_UNUSED(original);
    *error = QOrganizerManager::NotSupportedError;
    return QOrganizerCollection();
}

/*!
  Compares \a first against \a second.  If the types are
  strings (QVariant::String), the \a sensitivity argument controls
  case sensitivity when comparing.

  Returns:
  <0 if \a first is less than \a second
   0 if \a first is equal to \a second
  >0 if \a first is greater than \a second.

  The results are undefined if the variants are different types, or
  cannot be compared.
 */
int QOrganizerManagerEngine::compareVariant(const QVariant& first, const QVariant& second, Qt::CaseSensitivity sensitivity)
{
    switch(first.type()) {
        case QVariant::Int:
            return first.toInt() - second.toInt();

        case QVariant::LongLong:
            return first.toLongLong() - second.toLongLong();

        case QVariant::Bool:
        case QVariant::Char:
        case QVariant::UInt:
            return first.toUInt() - second.toUInt();

        case QVariant::ULongLong:
            return first.toULongLong() - second.toULongLong();

       case QVariant::String:
            return first.toString().compare(second.toString(), sensitivity);

        case QVariant::Double:
            {
                const double a = first.toDouble();
                const double b = second.toDouble();
                return (a < b) ? -1 : ((a == b) ? 0 : 1);
            }

        case QVariant::DateTime:
            {
                const QDateTime a = first.toDateTime();
                const QDateTime b = second.toDateTime();
                return (a < b) ? -1 : ((a == b) ? 0 : 1);
            }

        case QVariant::Date:
            return first.toDate().toJulianDay() - second.toDate().toJulianDay();

        case QVariant::Time:
            {
                const QTime a = first.toTime();
                const QTime b = second.toTime();
                return (a < b) ? -1 : ((a == b) ? 0 : 1);
            }

        default:
            return 0;
    }
}

/*!
  Returns true if the supplied item \a organizeritem matches the supplied filter \a filter.

  This function will test each condition in the filter, possibly recursing.
 */
bool QOrganizerManagerEngine::testFilter(const QOrganizerItemFilter &filter, const QOrganizerItem &organizeritem)
{
    switch(filter.type()) {
        case QOrganizerItemFilter::InvalidFilter:
        case QOrganizerItemFilter::ActionFilter:
            return false;

        case QOrganizerItemFilter::DefaultFilter:
            return true;

        case QOrganizerItemFilter::IdFilter:
            {
                const QOrganizerItemIdFilter idf(filter);
                if (idf.ids().contains(organizeritem.id()))
                    return true;
            }
            // Fall through to end
            break;

        case QOrganizerItemFilter::OrganizerItemDetailFilter:
            {
                const QOrganizerItemDetailFilter cdf(filter);
                if (cdf.detailDefinitionName().isEmpty())
                    return false;

                /* See if this organizer item has one of these details in it */
                const QList<QOrganizerItemDetail>& details = organizeritem.details(cdf.detailDefinitionName());

                if (details.count() == 0)
                    return false; /* can't match */

                /* See if we need to check the values */
                if (cdf.detailFieldName().isEmpty())
                    return true;  /* just testing for the presence of a detail of the specified definition */

                /* Now figure out what tests we are doing */
                const bool valueTest = cdf.value().isValid();
                const bool presenceTest = !valueTest;

                /* See if we need to test any values at all */
                if (presenceTest) {
                    for(int j=0; j < details.count(); j++) {
                        const QOrganizerItemDetail& detail = details.at(j);

                        /* Check that the field is present and has a non-empty value */
                        if (detail.variantValues().contains(cdf.detailFieldName()) && !detail.value(cdf.detailFieldName()).isEmpty())
                            return true;
                    }
                    return false;
                }

                /* Case sensitivity, for those parts that use it */
                Qt::CaseSensitivity cs = (cdf.matchFlags() & QOrganizerItemFilter::MatchCaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;

                /* See what flags are requested, since we're looking at a value */
                if (cdf.matchFlags() & (QOrganizerItemFilter::MatchEndsWith | QOrganizerItemFilter::MatchStartsWith | QOrganizerItemFilter::MatchContains | QOrganizerItemFilter::MatchFixedString)) {
                    /* We're strictly doing string comparisons here */
                    bool matchStarts = (cdf.matchFlags() & 7) == QOrganizerItemFilter::MatchStartsWith;
                    bool matchEnds = (cdf.matchFlags() & 7) == QOrganizerItemFilter::MatchEndsWith;
                    bool matchContains = (cdf.matchFlags() & 7) == QOrganizerItemFilter::MatchContains;

                    /* Value equality test */
                    for(int j=0; j < details.count(); j++) {
                        const QOrganizerItemDetail& detail = details.at(j);
                        const QString& var = detail.value(cdf.detailFieldName());
                        const QString& needle = cdf.value().toString();
                        if (matchStarts && var.startsWith(needle, cs))
                            return true;
                        if (matchEnds && var.endsWith(needle, cs))
                            return true;
                        if (matchContains && var.contains(needle, cs))
                            return true;
                        if (QString::compare(var, needle, cs) == 0)
                            return true;
                    }
                    return false;
                } else {
                    /* Nope, testing the values as a variant */
                    /* Value equality test */
                    for(int j = 0; j < details.count(); j++) {
                        const QOrganizerItemDetail& detail = details.at(j);
                        const QVariant& var = detail.variantValue(cdf.detailFieldName());
                        if (!var.isNull() && compareVariant(var, cdf.value(), cs) == 0)
                            return true;
                    }
                }
            }
            break;

        case QOrganizerItemFilter::OrganizerItemDetailRangeFilter:
            {
                const QOrganizerItemDetailRangeFilter cdf(filter);
                if (cdf.detailDefinitionName().isEmpty())
                    return false; /* we do not know which field to check */

                /* See if this organizer item has one of these details in it */
                const QList<QOrganizerItemDetail>& details = organizeritem.details(cdf.detailDefinitionName());

                if (details.count() == 0)
                    return false; /* can't match */

                /* Check for a detail presence test */
                if (cdf.detailFieldName().isEmpty())
                    return true;

                /* See if this is a field presence test */
                if (!cdf.minValue().isValid() && !cdf.maxValue().isValid()) {
                    for(int j=0; j < details.count(); j++) {
                        const QOrganizerItemDetail& detail = details.at(j);
                        if (detail.variantValues().contains(cdf.detailFieldName()))
                            return true;
                    }
                    return false;
                }

                /* open or closed interval testing support */
                const int minComp = cdf.rangeFlags() & QOrganizerItemDetailRangeFilter::ExcludeLower ? 1 : 0;
                const int maxComp = cdf.rangeFlags() & QOrganizerItemDetailRangeFilter::IncludeUpper ? 1 : 0;

                const bool testMin = cdf.minValue().isValid();
                const bool testMax = cdf.maxValue().isValid();

                /* At this point we know that at least of testMin & testMax is true */

                /* Case sensitivity, for those parts that use it */
                Qt::CaseSensitivity cs = (cdf.matchFlags() & QOrganizerItemFilter::MatchCaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;

                /* See what flags are requested, since we're looking at a value */
                if (cdf.matchFlags() & (QOrganizerItemFilter::MatchEndsWith | QOrganizerItemFilter::MatchStartsWith | QOrganizerItemFilter::MatchContains | QOrganizerItemFilter::MatchFixedString)) {
                    /* We're strictly doing string comparisons here */
                    //bool matchStarts = (cdf.matchFlags() & 7) == QOrganizerItemFilter::MatchStartsWith;
                    bool matchEnds = (cdf.matchFlags() & 7) == QOrganizerItemFilter::MatchEndsWith;
                    bool matchContains = (cdf.matchFlags() & 7) == QOrganizerItemFilter::MatchContains;

                    /* Min/Max and contains do not make sense */
                    if (matchContains)
                        return false;

                    QString minVal = cdf.minValue().toString();
                    QString maxVal = cdf.maxValue().toString();

                    /* Starts with is the normal compare case, endsWith is a bit trickier */
                    for(int j=0; j < details.count(); j++) {
                        const QOrganizerItemDetail& detail = details.at(j);
                        const QString& var = detail.value(cdf.detailFieldName());
                        if (!matchEnds) {
                            // MatchStarts or MatchFixedString
                            if (testMin && QString::compare(var, minVal, cs) < minComp)
                                continue;
                            if (testMax && QString::compare(var, maxVal, cs) >= maxComp)
                                continue;
                            return true;
                        } else {
                            /* Have to test the length of min & max */
                            // using refs means the parameter order is backwards, so negate the result of compare
                            if (testMin && -QString::compare(minVal, var.rightRef(minVal.length()), cs) < minComp)
                                continue;
                            if (testMax && -QString::compare(maxVal, var.rightRef(maxVal.length()), cs) >= maxComp)
                                continue;
                            return true;
                        }
                    }
                    // Fall through to end
                } else {
                    /* Nope, testing the values as a variant */
                    for(int j=0; j < details.count(); j++) {
                        const QOrganizerItemDetail& detail = details.at(j);
                        const QVariant& var = detail.variantValue(cdf.detailFieldName());

                        if (testMin && compareVariant(var, cdf.minValue(), cs) < minComp)
                            continue;
                        if (testMax && compareVariant(var, cdf.maxValue(), cs) >= maxComp)
                            continue;
                        return true;
                    }
                    // Fall through to end
                }
            }
            break;

        case QOrganizerItemFilter::ChangeLogFilter:
            {
                QOrganizerItemChangeLogFilter ccf(filter);

                // See what we can do...
                QOrganizerItemTimestamp ts = organizeritem.detail(QOrganizerItemTimestamp::DefinitionName);

                // See if timestamps are even supported
                if (ts.isEmpty())
                    break;

                if (ccf.eventType() == QOrganizerItemChangeLogFilter::EventAdded)
                    return ccf.since() <= ts.created();
                if (ccf.eventType() == QOrganizerItemChangeLogFilter::EventChanged)
                    return ccf.since() <= ts.lastModified();

                // You can't emulate a removed..
                // Fall through to end
            }
            break;

        case QOrganizerItemFilter::IntersectionFilter:
            {
                /* XXX In theory we could reorder the terms to put the native tests first */
                const QOrganizerItemIntersectionFilter bf(filter);
                const QList<QOrganizerItemFilter>& terms = bf.filters();
                if (terms.count() > 0) {
                    for(int j = 0; j < terms.count(); j++) {
                        if (!testFilter(terms.at(j), organizeritem)) {
                            return false;
                        }
                    }
                    return true;
                }
                // Fall through to end
            }
            break;

        case QOrganizerItemFilter::UnionFilter:
            {
                /* XXX In theory we could reorder the terms to put the native tests first */
                const QOrganizerItemUnionFilter bf(filter);
                const QList<QOrganizerItemFilter>& terms = bf.filters();
                if (terms.count() > 0) {
                    for(int j = 0; j < terms.count(); j++) {
                        if (testFilter(terms.at(j), organizeritem)) {
                            return true;
                        }
                    }
                    return false;
                }
                // Fall through to end
            }
            break;

    case QOrganizerItemFilter::CollectionFilter:
            {
                const QOrganizerItemCollectionFilter cf(filter);
                const QSet<QOrganizerCollectionId>& ids = cf.collectionIds();
                if (ids.contains(organizeritem.collectionId()))
                    return true;
                return false;
            }
            break;
    }
    return false;
}

/*!
  Returns true if the given \a item (or an occurrence of the item) occurs within the range
  specified by the \a startPeriod and the \a endPeriod.
  A default-constructed \a startPeriod signifies that the lower bound of the range is
  infinitely small (i.e., will match anything up to the \a endPeriod) and a default-constructed
  \a endPeriod signifies that the upper bound of the range is infinitely large
  (i.e., will match anything which occurs after the \a startPeriod).
 */
bool QOrganizerManagerEngine::isItemBetweenDates(const QOrganizerItem& item, const QDateTime& startPeriod, const QDateTime& endPeriod)
{
    if (startPeriod.isNull() && endPeriod.isNull())
        return true;

    QDateTime itemDateStart;
    QDateTime itemDateEnd;

    if (item.type() == QOrganizerItemType::TypeEvent || item.type() == QOrganizerItemType::TypeEventOccurrence) {
        QOrganizerEventTime etr = item.detail<QOrganizerEventTime>();
        itemDateStart = etr.startDateTime();
        itemDateEnd = etr.endDateTime();
    } else if (item.type() == QOrganizerItemType::TypeTodo || item.type() == QOrganizerItemType::TypeTodoOccurrence) {
        QOrganizerTodoTime ttr = item.detail<QOrganizerTodoTime>();
        itemDateStart = ttr.startDateTime();
        itemDateEnd = ttr.dueDateTime();
    } else if (item.type() == QOrganizerItemType::TypeJournal) {
        QOrganizerJournal journal = item;
        itemDateStart = itemDateEnd = journal.dateTime();
    } else if (item.type() == QOrganizerItemType::TypeNote) {
        //for note, there is no such start/end datetime so we always return false
        return false;
    }

    // if period start date is not given, check that item is starting or ending before period end
    if (startPeriod.isNull()) // endPeriod must be non-null because of initial test
        return (!itemDateStart.isNull() && itemDateStart <= endPeriod) ||
               (!itemDateEnd.isNull() && itemDateEnd <= endPeriod);

    // if period end date is not given, check that item is starting or ending after the period start
    if (endPeriod.isNull())   // startPeriod must be non-null because of initial test
        return (!itemDateEnd.isNull() && itemDateEnd >= startPeriod) ||
               (!itemDateStart.isNull() && itemDateStart >= startPeriod);

    // Both startPeriod and endPeriod are not null
    // check if item start date is between the period start and end date
    if (!itemDateStart.isNull() && itemDateStart >= startPeriod && itemDateStart <= endPeriod)
        return true;

    // check if item end date is between the period start and end date
    if (!itemDateEnd.isNull() && itemDateEnd >= startPeriod && itemDateEnd <= endPeriod)
        return true;

    // check if item interval is including the period interval
    if (!itemDateStart.isNull() && !itemDateEnd.isNull() && itemDateStart <= startPeriod && itemDateEnd >= endPeriod)
        return true;

    return false;
}

/*!
  Compares two organizer items (\a a and \a b) using the given list of \a sortOrders.  Returns a negative number if \a a should appear
  before \a b according to the sort order, a positive number if \a a should appear after \a b according to the sort order,
  and zero if the two are unable to be sorted.
 */
int QOrganizerManagerEngine::compareItem(const QOrganizerItem& a, const QOrganizerItem& b, const QList<QOrganizerItemSortOrder>& sortOrders)
{
    QList<QOrganizerItemSortOrder> copy = sortOrders;
    while (copy.size()) {
        // retrieve the next sort order in the list
        QOrganizerItemSortOrder sortOrder = copy.takeFirst();
        if (!sortOrder.isValid())
            break;

        // obtain the values which this sort order concerns
        const QVariant& aVal = a.detail(sortOrder.detailDefinitionName()).variantValue(sortOrder.detailFieldName());
        const QVariant& bVal = b.detail(sortOrder.detailDefinitionName()).variantValue(sortOrder.detailFieldName());

        bool aIsNull = false;
        bool bIsNull = false;

        // treat empty strings as null qvariants.
        if ((aVal.type() == QVariant::String && aVal.toString().isEmpty()) || aVal.isNull()) {
            aIsNull = true;
        }
        if ((bVal.type() == QVariant::String && bVal.toString().isEmpty()) || bVal.isNull()) {
            bIsNull = true;
        }

        // early exit error checking
        if (aIsNull && bIsNull)
            continue; // use next sort criteria.
        if (aIsNull)
            return (sortOrder.blankPolicy() == QOrganizerItemSortOrder::BlanksFirst ? -1 : 1);
        if (bIsNull)
            return (sortOrder.blankPolicy() == QOrganizerItemSortOrder::BlanksFirst ? 1 : -1);

        // real comparison
        int comparison = compareVariant(aVal, bVal, sortOrder.caseSensitivity()) * (sortOrder.direction() == Qt::AscendingOrder ? 1 : -1);
        if (comparison == 0)
            continue;
        return comparison;
    }

    return 0; // or according to id? return (a.id() < b.id() ? -1 : 1);
}


/*!
  Performs insertion sort of the item \a toAdd into the \a sorted list, according to the provided \a sortOrders list.
  The first QOrganizerItemSortOrder in the list has the highest priority; if the item \a toAdd is deemed equal to another
  in the \a sorted list, the second QOrganizerItemSortOrder in the list is used (and so on until either the item is inserted
  or there are no more sort order objects in the list).
 */
void QOrganizerManagerEngine::addSorted(QList<QOrganizerItem>* sorted, const QOrganizerItem& toAdd, const QList<QOrganizerItemSortOrder>& sortOrders)
{
    if (sortOrders.count() > 0) {
        for (int i = 0; i < sorted->size(); i++) {
            // check to see if the new item should be inserted here
            int comparison = compareItem(sorted->at(i), toAdd, sortOrders);
            if (comparison > 0) {
                sorted->insert(i, toAdd);
                return;
            }
        }
    }

    // hasn't been inserted yet?  append to the list.
    sorted->append(toAdd);
}

/*!
  Returns the engine id from the given \a id.
  The caller does not take ownership of the pointer, and should not delete returned id or undefined behavior may occur.
 */
const QOrganizerItemEngineId* QOrganizerManagerEngine::engineItemId(const QOrganizerItemId& id)
{
    return id.d.data();
}

/*!
  Returns the engine id from the given \a id.
  The caller does not take ownership of the pointer, and should not delete returned id or undefined behavior may occur.
 */
const QOrganizerCollectionEngineId* QOrganizerManagerEngine::engineCollectionId(const QOrganizerCollectionId& id)
{
    return id.d.data();
}

/*!
  Notifies the manager engine that the given request \a req has been destroyed.

  This notifies the engine that:
  \list
  \o the client doesn't care about the request any more.  The engine can still complete it,
     but completion is not required.
  \o it can't reliably access any properties of the request pointer any more.  The pointer will
     be invalid once this function returns.
  \endlist
  
  This means that if there is a worker thread, the engine needs to let that thread know that the
  request object is not valid and block until that thread acknowledges it.  One way to do this is to
  have a QSet<QOrganizerAbstractRequest*> (or QMap<QOrganizerAbstractRequest,
  MyCustomRequestState>) that tracks active requests, and insert into that set in startRequest, and
  remove in requestDestroyed (or when it finishes or is cancelled).  Protect that set/map with a
  mutex, and make sure you take the mutex in the worker thread before calling any of the
  QOrganizerAbstractRequest::updateXXXXXXRequest functions.  And be careful of lock ordering
  problems :D
 */
void QOrganizerManagerEngine::requestDestroyed(QOrganizerAbstractRequest* req)
{
    Q_UNUSED(req);
}

/*!
  Asks the manager engine to begin the given request \a req which is currently in a (re)startable
  state.  Returns true if the request was started successfully, else returns false.

  Generally, the engine queues the request and processes it at some later time (probably in another
  thread).
  
  Once a request is started, the engine should call the updateRequestState and/or the specific
  updateXXXXXRequest functions to mark it in the active state.
  
  If the engine is particularly fast, or the operation involves only in memory data, the request can
  be processed and completed without queueing it.
  
  Note that when the client is threaded, and the request might live on a different thread, the
  engine needs to be careful with locking.  In particular, the request might be deleted while the
  engine is still working on it.  In this case, the requestDestroyed function will be called while
  the request is still valid, and that function should block until the worker thread (etc.) has been
  notified not to touch that request any more.

  \sa QOrganizerAbstractRequest::start()
 */
bool QOrganizerManagerEngine::startRequest(QOrganizerAbstractRequest* req)
{
    Q_UNUSED(req);
    return false;
}

/*!
  Asks the manager engine to cancel the given request \a req which was
  previously started and is currently in a cancellable state.
  Returns true if cancellation of the request was started successfully,
  otherwise returns false.

  \sa startRequest(), QOrganizerAbstractRequest::cancel()
 */
bool QOrganizerManagerEngine::cancelRequest(QOrganizerAbstractRequest* req)
{
    Q_UNUSED(req);
    return false;
}

/*!
  Blocks until the manager engine has completed the given request \a req
  which was previously started, or until \a msecs milliseconds have passed.
  Returns true if the request was completed, and false if the request was not in the
  \c QOrganizerAbstractRequest::Active state or no progress could be reported.

  It is important that this function is implemented by the engine, at least merely as a delay, since
  clients may call it in a loop.

  \sa startRequest()
 */
bool QOrganizerManagerEngine::waitForRequestFinished(QOrganizerAbstractRequest* req, int msecs)
{
    Q_UNUSED(req);
    Q_UNUSED(msecs);
    return false;
}

/*!
  Updates the given asynchronous request \a req by setting the new \a state
  of the request.  If the new state is different, the stateChanged() signal
  will be emitted by the request.
 */
void QOrganizerManagerEngine::updateRequestState(QOrganizerAbstractRequest* req, QOrganizerAbstractRequest::State state)
{
    if (req) {
        QMutexLocker ml(&req->d_ptr->m_mutex);
        if (req->d_ptr->m_state != state) {
            req->d_ptr->m_state = state;
            ml.unlock();
            emit req->stateChanged(state);
        }
    }
}

/*!
  Updates the given QOrganizerItemOccurrenceFetchRequest \a req with the latest results \a result, and operation error \a error.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerManagerEngine::updateItemOccurrenceFetchRequest(QOrganizerItemOccurrenceFetchRequest* req, const QList<QOrganizerItem>& result, QOrganizerManager::Error error, QOrganizerAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QOrganizerItemOccurrenceFetchRequest> ireq(req); // Take this in case the first emit deletes us
        QOrganizerItemOccurrenceFetchRequestPrivate* rd = static_cast<QOrganizerItemOccurrenceFetchRequestPrivate*>(req->d_ptr);
        QMutexLocker ml(&rd->m_mutex);
        bool emitState = rd->m_state != newState;
        rd->m_organizeritems = result;
        rd->m_error = error;
        rd->m_state = newState;
        ml.unlock();
        emit ireq.data()->resultsAvailable();
        if (emitState && ireq)
            emit ireq.data()->stateChanged(newState);
    }
}

/*!
  Updates the given QOrganizerItemIdFetchRequest \a req with the latest results \a result, and operation error \a error.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerManagerEngine::updateItemIdFetchRequest(QOrganizerItemIdFetchRequest* req, const QList<QOrganizerItemId>& result, QOrganizerManager::Error error, QOrganizerAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QOrganizerItemIdFetchRequest> ireq(req); // Take this in case the first emit deletes us
        QOrganizerItemIdFetchRequestPrivate* rd = static_cast<QOrganizerItemIdFetchRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->m_mutex);
        bool emitState = rd->m_state != newState;
        rd->m_ids = result;
        rd->m_error = error;
        rd->m_state = newState;
        ml.unlock();
        emit ireq.data()->resultsAvailable();
        if (emitState && ireq)
            emit ireq.data()->stateChanged(newState);
    }
}

/*!
  Updates the given QOrganizerItemFetchRequest \a req with the latest results \a result, and operation error \a error.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerManagerEngine::updateItemFetchRequest(QOrganizerItemFetchRequest* req, const QList<QOrganizerItem>& result, QOrganizerManager::Error error, QOrganizerAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QOrganizerItemFetchRequest> ireq(req); // Take this in case the first emit deletes us
        QOrganizerItemFetchRequestPrivate* rd = static_cast<QOrganizerItemFetchRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->m_mutex);
        bool emitState = rd->m_state != newState;
        rd->m_organizeritems = result;
        rd->m_error = error;
        rd->m_state = newState;
        ml.unlock();
        emit ireq.data()->resultsAvailable();
        if (emitState && ireq)
            emit ireq.data()->stateChanged(newState);
    }
}

/*!
  Updates the given QOrganizerItemFetchForExportRequest \a req with the latest results \a result, and operation error \a error.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerManagerEngine::updateItemFetchForExportRequest(QOrganizerItemFetchForExportRequest* req, const QList<QOrganizerItem>& result, QOrganizerManager::Error error, QOrganizerAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QOrganizerItemFetchForExportRequest> ireq(req); // Take this in case the first emit deletes us
        QOrganizerItemFetchForExportRequestPrivate* rd = static_cast<QOrganizerItemFetchForExportRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->m_mutex);
        bool emitState = rd->m_state != newState;
        rd->m_organizeritems = result;
        rd->m_error = error;
        rd->m_state = newState;
        ml.unlock();
        emit ireq.data()->resultsAvailable();
        if (emitState && ireq)
            emit ireq.data()->stateChanged(newState);
    }
}

/*!
  Updates the given QOrganizerItemRemoveRequest \a req with the operation error \a error, and map of input index to individual error \a errorMap.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerManagerEngine::updateItemRemoveRequest(QOrganizerItemRemoveRequest* req, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QOrganizerItemRemoveRequest> ireq(req); // Take this in case the first emit deletes us
        QOrganizerItemRemoveRequestPrivate* rd = static_cast<QOrganizerItemRemoveRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->m_mutex);
        bool emitState = rd->m_state != newState;
        rd->m_errors = errorMap;
        rd->m_error = error;
        rd->m_state = newState;
        ml.unlock();
        emit ireq.data()->resultsAvailable();
        if (emitState && ireq)
            emit ireq.data()->stateChanged(newState);
    }
}

/*!
  Updates the given QOrganizerItemSaveRequest \a req with the latest results \a result, operation error \a error, and map of input index to individual error \a errorMap.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerManagerEngine::updateItemSaveRequest(QOrganizerItemSaveRequest* req, const QList<QOrganizerItem>& result, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QOrganizerItemSaveRequest> ireq(req); // Take this in case the first emit deletes us
        QOrganizerItemSaveRequestPrivate* rd = static_cast<QOrganizerItemSaveRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->m_mutex);
        bool emitState = rd->m_state != newState;
        rd->m_organizeritems = result;
        rd->m_errors = errorMap;
        rd->m_error = error;
        rd->m_state = newState;
        ml.unlock();
        emit ireq.data()->resultsAvailable();
        if (emitState && ireq)
            emit ireq.data()->stateChanged(newState);
    }
}

/*!
  Updates the given QOrganizerItemDetailDefinitionSaveRequest \a req with the latest results \a result, operation error \a error, and map of input index to individual error \a errorMap.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerManagerEngine::updateDefinitionSaveRequest(QOrganizerItemDetailDefinitionSaveRequest* req, const QList<QOrganizerItemDetailDefinition>& result, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QOrganizerItemDetailDefinitionSaveRequest> ireq(req); // Take this in case the first emit deletes us
        QOrganizerItemDetailDefinitionSaveRequestPrivate* rd = static_cast<QOrganizerItemDetailDefinitionSaveRequestPrivate*>(req->d_ptr);
        QMutexLocker ml(&rd->m_mutex);
        bool emitState = rd->m_state != newState;
        rd->m_definitions = result;
        rd->m_errors = errorMap;
        rd->m_error = error;
        rd->m_state = newState;
        ml.unlock();
        emit ireq.data()->resultsAvailable();
        if (emitState && ireq)
            emit ireq.data()->stateChanged(newState);
    }
}

/*!
  Updates the given QOrganizerItemDetailDefinitionRemoveRequest \a req with the operation error \a error, and map of input index to individual error \a errorMap.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerManagerEngine::updateDefinitionRemoveRequest(QOrganizerItemDetailDefinitionRemoveRequest* req, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QOrganizerItemDetailDefinitionRemoveRequest> ireq(req); // Take this in case the first emit deletes us
        QOrganizerItemDetailDefinitionRemoveRequestPrivate* rd = static_cast<QOrganizerItemDetailDefinitionRemoveRequestPrivate*>(req->d_ptr);
        QMutexLocker ml(&rd->m_mutex);
        bool emitState = rd->m_state != newState;
        rd->m_errors = errorMap;
        rd->m_error = error;
        rd->m_state = newState;
        ml.unlock();
        emit ireq.data()->resultsAvailable();
        if (emitState && ireq)
            emit ireq.data()->stateChanged(newState);
    }
}

/*!
  Updates the given QOrganizerItemDetailDefinitionFetchRequest \a req with the latest results \a result, operation error \a error, and map of input index to individual error \a errorMap.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.

  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerManagerEngine::updateDefinitionFetchRequest(QOrganizerItemDetailDefinitionFetchRequest* req, const QMap<QString, QOrganizerItemDetailDefinition>& result, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QOrganizerItemDetailDefinitionFetchRequest> ireq(req); // Take this in case the first emit deletes us
        QOrganizerItemDetailDefinitionFetchRequestPrivate* rd = static_cast<QOrganizerItemDetailDefinitionFetchRequestPrivate*>(req->d_ptr);
        QMutexLocker ml(&rd->m_mutex);
        bool emitState = rd->m_state != newState;
        rd->m_definitions = result;
        rd->m_errors = errorMap;
        rd->m_error = error;
        rd->m_state = newState;
        ml.unlock();
        emit ireq.data()->resultsAvailable();
        if (emitState && ireq)
            emit ireq.data()->stateChanged(newState);
    }
}

/*!
  Updates the given QOrganizerCollectionFetchRequest \a req with the latest results \a result, and operation error \a error.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.
  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerManagerEngine::updateCollectionFetchRequest(QOrganizerCollectionFetchRequest* req, const QList<QOrganizerCollection>& result, QOrganizerManager::Error error, QOrganizerAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QOrganizerCollectionFetchRequest> ireq(req); // Take this in case the first emit deletes us
        QOrganizerCollectionFetchRequestPrivate* rd = static_cast<QOrganizerCollectionFetchRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->m_mutex);
        bool emitState = rd->m_state != newState;
        rd->m_collections = result;
        rd->m_error = error;
        rd->m_state = newState;
        ml.unlock();
        emit ireq.data()->resultsAvailable();
        if (emitState && ireq)
            emit ireq.data()->stateChanged(newState);
    }
}

/*!
  Updates the given QOrganizerCollectionRemoveRequest \a req with the operation error \a error, and map of input index to individual error \a errorMap.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.
  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerManagerEngine::updateCollectionRemoveRequest(QOrganizerCollectionRemoveRequest* req, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QOrganizerCollectionRemoveRequest> ireq(req); // Take this in case the first emit deletes us
        QOrganizerCollectionRemoveRequestPrivate* rd = static_cast<QOrganizerCollectionRemoveRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->m_mutex);
        bool emitState = rd->m_state != newState;
        rd->m_errors = errorMap;
        rd->m_error = error;
        rd->m_state = newState;
        ml.unlock();
        emit ireq.data()->resultsAvailable();
        if (emitState && ireq)
            emit ireq.data()->stateChanged(newState);
    }
}

/*!
  Updates the given QOrganizerCollectionSaveRequest \a req with the latest results \a result, operation error \a error, and map of input index to individual error \a errorMap.
  In addition, the state of the request will be changed to \a newState.

  It then causes the request to emit its resultsAvailable() signal to notify clients of the request progress.
  If the new request state is different from the previous state, the stateChanged() signal will also be emitted from the request.
 */
void QOrganizerManagerEngine::updateCollectionSaveRequest(QOrganizerCollectionSaveRequest* req, const QList<QOrganizerCollection>& result, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState)
{
    if (req) {
        QWeakPointer<QOrganizerCollectionSaveRequest> ireq(req); // Take this in case the first emit deletes us
        QOrganizerCollectionSaveRequestPrivate* rd = static_cast<QOrganizerCollectionSaveRequestPrivate*>(ireq.data()->d_ptr);
        QMutexLocker ml(&rd->m_mutex);
        bool emitState = rd->m_state != newState;
        rd->m_collections = result;
        rd->m_errors = errorMap;
        rd->m_error = error;
        rd->m_state = newState;
        ml.unlock();
        emit ireq.data()->resultsAvailable();
        if (emitState && ireq)
            emit ireq.data()->stateChanged(newState);
    }
}


#include "moc_qorganizermanagerengine.cpp"

QTM_END_NAMESPACE
