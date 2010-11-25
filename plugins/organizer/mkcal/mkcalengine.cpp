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

#include <QStringBuilder>

#include "mkcalengine.h"
#include "qtorganizer.h"
#include "mkcalid.h"
#include "qorganizerasynchmanager.h"

Q_DEFINE_LATIN1_CONSTANT(MKCalEngineID, "mkcal");
Q_DEFINE_LATIN1_CONSTANT(NotebookSyncDate, "SyncDate");
Q_DEFINE_LATIN1_CONSTANT(NotebookModifiedDate, "ModifiedDate");
Q_DEFINE_LATIN1_CONSTANT(NotebookPluginName, "PluginName");
Q_DEFINE_LATIN1_CONSTANT(NotebookAccount, "Account");
Q_DEFINE_LATIN1_CONSTANT(NotebookSharedWith, "SharedWith");
Q_DEFINE_LATIN1_CONSTANT(NotebookSharedWithStr, "SharedWithStr");
Q_DEFINE_LATIN1_CONSTANT(NotebookSyncProfile, "SyncProfile");
Q_DEFINE_LATIN1_CONSTANT(NotebookAttachmentSize, "AttachmentSize");
Q_DEFINE_LATIN1_CONSTANT(NotebookIsDefault, "IsDefault");
Q_DEFINE_LATIN1_CONSTANT(NotebookIsShareable, "IsShareable");
Q_DEFINE_LATIN1_CONSTANT(NotebookIsShared, "IsShared");
Q_DEFINE_LATIN1_CONSTANT(NotebookIsMaster, "IsMaster");
Q_DEFINE_LATIN1_CONSTANT(NotebookIsSynchronized, "IsSynchronized");
Q_DEFINE_LATIN1_CONSTANT(NotebookIsReadOnly, "IsReadOnly");
Q_DEFINE_LATIN1_CONSTANT(NotebookIsVisible, "IsVisible");
Q_DEFINE_LATIN1_CONSTANT(NotebookIsRunTimeOnly, "IsRunTimeOnly");
Q_DEFINE_LATIN1_CONSTANT(NotebookEventsAllowed, "EventsAllowed");
Q_DEFINE_LATIN1_CONSTANT(NotebookJournalsAllowed, "JournalsAllowed");
Q_DEFINE_LATIN1_CONSTANT(NotebookTodosAllowed, "TodosAllowed");

//QTM_USE_NAMESPACE

QString MKCalItemId::toString() const
{
    QString d;
    if (!m_rid.isNull())
        d = QString("%1").arg(m_rid.toTime_t());
    return m_id % ':' % d % ':' % managerUri();
}

QString MKCalCollectionId::toString() const
{
    return m_uid % QLatin1String(":") % managerUri();
}

QOrganizerManagerEngine* MKCalEngineFactory::engine(const QMap<QString, QString>& parameters, QOrganizerManager::Error* error)
{
    Q_UNUSED(parameters);

    *error = QOrganizerManager::NoError;
    QString managerUri = QOrganizerManager::buildUri(MKCalEngineID, QMap<QString, QString>());

    return new MKCalEngine(managerUri); // manager takes ownership and will clean up.
}

QString MKCalEngineFactory::managerName() const
{
    return MKCalEngineID;
}

QOrganizerItemEngineId* MKCalEngineFactory::createItemEngineId(const QMap<QString, QString>& parameters, const QString& engineIdString) const
{
    Q_UNUSED(parameters);

    // format of engineIdString is "uid(string):rid(time_t):managerUri(string)"
    int col1 = engineIdString.indexOf(QChar(':'));
    int col2 = engineIdString.indexOf(QChar(':'), col1+1);
    if (col1 < 0 || col2 < 0)
        return NULL;
    KDateTime rid;
    QString tmp = engineIdString.mid(col1+1, col2-col1-1);
    if (!tmp.isEmpty())
        rid.setTime_t(tmp.toLongLong());

    //ignore the managerUri parameter
    return new MKCalItemId(engineIdString.mid(0, col1), rid);
}

QOrganizerCollectionEngineId* MKCalEngineFactory::createCollectionEngineId(const QMap<QString, QString>& parameters, const QString& engineIdString) const
{
    Q_UNUSED(parameters);

    // format of engineIdString is "uid(string):managerUri(string)"
    int col1 = engineIdString.indexOf(QChar(':'));
    return  col1 < 0 ? NULL : new MKCalCollectionId(engineIdString.mid(0, col1));
}

Q_EXPORT_PLUGIN2(qtorganizer_mkcal, MKCalEngineFactory);


MKCalEngine::MKCalEngine(const QString& managerUri)
    : d(new MKCalEngineData)
{
    // set our manager uri.
    d->m_managerUri = managerUri;

    // register ourselves as an observer of any changes to the db.
    d->m_storagePtr->registerObserver(this);

    // and start our asynchronous request helper
    d->m_asynchProcess = new OrganizerAsynchManager(this);
}

MKCalEngine::~MKCalEngine()
{
    delete d->m_asynchProcess;
    d->m_storagePtr->unregisterObserver(this);
}

QString MKCalEngine::managerName() const
{
    return MKCalEngineID;
}

QMap<QString, QString> MKCalEngine::managerParameters() const
{
    return QMap<QString, QString>();
}

int MKCalEngine::managerVersion() const
{
    return 1;
}

QList<QOrganizerItem> MKCalEngine::itemOccurrences(const QOrganizerItem& parentItem, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const
{
    QMutexLocker locker(&d->m_operationMutex);
    return internalItemOccurrences(parentItem, periodStart, periodEnd, maxCount, fetchHint, error, GeneratedAndPersistentOccurrences);
}

QList<QOrganizerItem> MKCalEngine::internalItemOccurrences(const QOrganizerItem& parentItem, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error, IncludedOccurrences includedOccurrences) const
{
    Q_UNUSED(fetchHint); // XXX TODO: why do we not use this?

    //for the moment we support generating of 1000 occurrences. 1000 is a random value
    if (maxCount < 0) {
        maxCount = 1000;
    } else if (!maxCount) {
        // if maxCount is 0 return empty list
        return QList<QOrganizerItem>();
    }

    //find the generator incidence
    QOrganizerItemId generatorId = parentItem.id();
    QString kId = MKCalItemId::id_cast(generatorId)->id();
    KCalCore::Incidence::Ptr generatorIncidence;
    if (kId.isEmpty() || !(generatorIncidence = d->m_calendarBackendPtr->incidence(kId))) {
        *error = QOrganizerManager::DoesNotExistError;
        return QList<QOrganizerItem>();
    }

    //TODO: Until QOrganizerItem will include a startDt, we will store a QPair of QDateTime and QOrganizerItem for faster mergeSort
    QList<QPair<QDateTime, QOrganizerItem> > persistentExceptions;
    QList<QPair<QDateTime, QOrganizerItem> > instances;
    QList<QOrganizerItem> res;

    //if persistent occurrences are needed then fetch them
    if ((includedOccurrences == GeneratedAndPersistentOccurrences) && generatorIncidence->recurs()) {
        if (generatorIncidence->type() == KCalCore::Incidence::TypeEvent) {
            //get all the instances of the event
            KCalCore::Event::List events(d->m_calendarBackendPtr->eventInstances(generatorIncidence, KCalCore::EventSortStartDate));
            foreach(const KCalCore::Event::Ptr& ev, events) {
                //early break if we reached the maximum count
                if (persistentExceptions.count() > maxCount)
                    break;
                QOrganizerItem instance;
                if (isIncidenceInInterval(ev, periodStart, periodEnd) && convertIncidenceToItem(ev, &instance)) {
                    persistentExceptions.append(QPair<QDateTime, QOrganizerItem>(ev->dtStart().dateTime(), instance));
                }
            }
        } else if (generatorIncidence->type() == KCalCore::Incidence::TypeTodo) {
            //get all the instances of the todo
            KCalCore::Todo::List todos(d->m_calendarBackendPtr->todoInstances(generatorIncidence));
            foreach(const KCalCore::Todo::Ptr& t, todos) {
                //early break if we reached the maximum count
                if (persistentExceptions.count() > maxCount)
                    break;
                QOrganizerItem instance;
                if (isIncidenceInInterval(t, periodStart, periodEnd) && convertIncidenceToItem(t, &instance)) {
                    persistentExceptions.append(QPair<QDateTime, QOrganizerItem>(t->dtStart().dateTime(), instance));
                }
            }
        }
    }

    //generate the incidences between the given period
    KCalCore::Incidence::List generatorList;
    generatorList.append(generatorIncidence);
    mKCal::ExtendedCalendar::ExpandedIncidenceList incidenceList = d->m_calendarBackendPtr->expandRecurrences(
            &generatorList,
            KDateTime(periodStart),
            KDateTime(periodEnd),
            maxCount);
    KCalCore::Recurrence *recurrence = generatorIncidence->recurrence();
    foreach (const mKCal::ExtendedCalendar::ExpandedIncidence& expandedIncidence, incidenceList) {
        QDateTime incidenceDateTime = expandedIncidence.first;
        KCalCore::Incidence::Ptr incidence = expandedIncidence.second;
        QOrganizerItem instance;
        //unfortunately sometimes expandRecurrences will include also those occurrences which are in exDates
        if (!recurrence->exDates().containsSorted(incidenceDateTime.date()) && convertIncidenceToItem(incidence, &instance)) {
            QDateTime startDT;
            if (instance.type() == QOrganizerItemType::TypeEvent) {
                //convert the type to event occurrence
                QOrganizerEventOccurrence* event = static_cast<QOrganizerEventOccurrence*>(&instance);
                event->setType(QOrganizerItemType::TypeEventOccurrence);

                //shift the startdate / enddate to the generated incidence start time which can be found in incidenceDateTime
                startDT = event->startDateTime();
                int duration = startDT.secsTo(event->endDateTime());
                startDT = QDateTime(incidenceDateTime.date(), startDT.time());
                event->setStartDateTime(startDT);
                event->setEndDateTime(startDT.addSecs(duration));

                //set parentid and originaldate
                event->setOriginalDate(incidenceDateTime.date());
                event->setParentId(generatorId);

                //remove recurrence rules
                foreach (QOrganizerItemDetail recurrence, event->details<QOrganizerItemRecurrence>()) {
                    event->removeDetail(&recurrence);
                }
            } else if (instance.type() == QOrganizerItemType::TypeTodo) {
                //convert the type to todo occurrence
                QOrganizerTodoOccurrence* todo = static_cast<QOrganizerTodoOccurrence*>(&instance);
                todo->setType(QOrganizerItemType::TypeTodoOccurrence);

                //shift the startdate / duedate to the generated incidence start time which can be found in incidenceDateTime
                startDT = todo->startDateTime();
                int duration = todo->dueDateTime().isNull() ? -1 : startDT.secsTo(todo->dueDateTime());
                startDT = QDateTime(incidenceDateTime.date(), startDT.time());
                todo->setStartDateTime(startDT);
                todo->setDueDateTime(duration >= 0 ? startDT.addSecs(duration) : QDateTime());

                //set parentid and originaldate
                todo->setOriginalDate(incidenceDateTime.date());
                todo->setParentId(generatorId);

                //remove recurrence rules
                foreach (QOrganizerItemDetail recurrence, todo->details<QOrganizerItemRecurrence>()) {
                    todo->removeDetail(&recurrence);
                }
            }

            // if the incidence is the generating event itself then clear its id
            if (incidence == generatorIncidence) {
                instance.setId(QOrganizerItemId());
            }

            //if the result needs to include the permanent exceptions then the instance need to be inserted
            //into a temporal list to be able to merge sort in the end
            if (persistentExceptions.isEmpty())
                res.append(instance);
            else
                instances.append(QPair<QDateTime, QOrganizerItem>(startDT, instance));
        }
    }

    //if persistant exceptions list is not null, mergesort the two list
    if (!persistentExceptions.isEmpty()) {
        res.reserve(qMin(maxCount, instances.count() + persistentExceptions.count()));

        //mergeSort the results based on the start time
        QList<QPair<QDateTime, QOrganizerItem> >::const_iterator itExceptions = persistentExceptions.constBegin();
        QList<QPair<QDateTime, QOrganizerItem> >::const_iterator itInstances = instances.constBegin();
        while (itExceptions != persistentExceptions.constEnd() && itInstances != instances.constEnd() && res.count() < maxCount) {
            if (itExceptions->first < itInstances->first) {
                res.append(itExceptions->second);
                ++itExceptions;
            } else {
                res.append(itInstances->second);
                ++itInstances;
            }
        }

        //add the remaining persistent exceptions
        for (; itExceptions != persistentExceptions.constEnd() && res.count() < maxCount; ++itExceptions) {
            res.append(itExceptions->second);
        }

        //add the remaining generated exceptions
        for (; itInstances != instances.constEnd() && res.count() < maxCount; ++itInstances) {
            res.append(itInstances->second);
        }
    }

    *error = QOrganizerManager::NoError;
    return res;
}

QList<QOrganizerItemId> MKCalEngine::itemIds(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerManager::Error* error) const
{
    QMutexLocker locker(&d->m_operationMutex);

    //small optimization for the default case when startDate, endDate, filter, sortOrders are the default values
    if (startDate.isNull() && endDate.isNull() && filter == QOrganizerItemFilter() && sortOrders.count() == 0) {
        QList<QOrganizerItemId> ids;
        KCalCore::Incidence::List rawIncidences = d->m_calendarBackendPtr->rawIncidences();
        foreach(const KCalCore::Incidence::Ptr& i, rawIncidences) {
            ids.append(QOrganizerItemId(new MKCalItemId(i->uid(),
                    i->hasRecurrenceId() ? i->recurrenceId() : KDateTime())));
        }

        return ids;
    }

    QList<QOrganizerItem> ret = itemsForExport(startDate, endDate, filter, sortOrders, QOrganizerItemFetchHint(), error);

    return QOrganizerManager::extractIds(ret);
}

QList<QOrganizerItem> MKCalEngine::itemsForExport(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const
{
    QMutexLocker locker(&d->m_operationMutex);

    return internalItems(startDate, endDate, filter, sortOrders, fetchHint, error, false);
}

QList<QOrganizerItem> MKCalEngine::items(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const
{
    QMutexLocker locker(&d->m_operationMutex);

    return internalItems(startDate, endDate, filter, sortOrders, fetchHint, error, true);
}

QList<QOrganizerItem> MKCalEngine::internalItems(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error, bool expand) const
{
    Q_UNUSED(fetchHint);
    Q_UNUSED(error);
    // TODO: optimize by using our own filters

    // Just naively get all the incidences between the given startDate and endDate
    d->m_calendarBackendPtr->setFilter(0);
    KCalCore::Incidence::List incidences = d->m_calendarBackendPtr->incidences(startDate.date(), endDate.date());
    QList<QOrganizerItem> partiallyFilteredItems;
    QList<QOrganizerItem> ret;

    // Convert them all to QOrganizerItems
    foreach (const KCalCore::Incidence::Ptr& incidence, incidences) {
        QOrganizerItem item;
        if (incidence->recurs()) {
            if (convertIncidenceToItem(incidence, &item)) {
                if (expand) {
                    partiallyFilteredItems << internalItemOccurrences(item, startDate, endDate, 100, fetchHint, error, OnlyGeneratedOccurrences);
                } else if (itemHasRecurringChildInInterval(incidence, item, startDate, endDate, filter)) {
                    QOrganizerManagerEngine::addSorted(&ret, item, sortOrders);
                }
            }
        } else {
            if (isIncidenceInInterval(incidence, startDate, endDate) && convertIncidenceToItem(incidence, &item)) {
                partiallyFilteredItems << item;
            }
        }
    }

    // Now filter them
    foreach(const QOrganizerItem& item, partiallyFilteredItems) {
        if (QOrganizerManagerEngine::testFilter(filter, item)) {
            QOrganizerManagerEngine::addSorted(&ret, item, sortOrders);
        }
    }

    return ret;
}


bool MKCalEngine::itemHasRecurringChildInInterval(KCalCore::Incidence::Ptr incidence, QOrganizerItem generator, QDateTime startDate, QDateTime endDate, QOrganizerItemFilter filter) const
{
    //this function will check if the incidence has recurring children in the specified interval with the given filter

    if (filter.type() != QOrganizerItemFilter::DefaultFilter) {

        //check the special case with parent filtering
        if (filter.type() == QOrganizerItemFilter::OrganizerItemDetailFilter) {
            const QOrganizerItemDetailFilter cdf(filter);

            if (cdf.detailDefinitionName() == QOrganizerItemParent::DefinitionName &&
                cdf.detailFieldName() == QOrganizerItemParent::FieldParentId) {
                //obtain the engine id from the filter
                QString id(MKCalItemId::id_cast(cdf.value().value<QOrganizerItemId>())->id());

                //if the filter id is not equal with the generator incidence id then return false
                if (incidence->uid() != id)
                    return false;
            } else if (!QOrganizerManagerEngine::testFilter(filter, generator)) {
                //test the filter
                return false;
            }
        } else if (!QOrganizerManagerEngine::testFilter(filter, generator)) {
            //test the filter
            return false;
        }
    }

    // if interval is null always return true
    if (startDate.isNull() && endDate.isNull())
        return true;

    // if start interval is null, check if the incidence start date is before the interval end date
    if (startDate.isNull())
        return incidence->dtStart().dateTime() <= endDate;

    QDateTime next(incidence->recurrence()->getNextDateTime(KDateTime(startDate)).dateTime());
    //fail if there are no recurrences after the start date
    if (next.isNull())
        return false;

    //if end date is empty, check if the calculated recurrence + duration of the event is after the start date
    if (endDate.isNull())
        return next.addSecs(incidenceDuration(incidence)) >= startDate;

    //in last case just check if a recurrence is before the interval end
    return next <= endDate;
}

QOrganizerItem MKCalEngine::item(const QOrganizerItemId& itemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const
{
    Q_UNUSED(fetchHint);

    QMutexLocker locker(&d->m_operationMutex);

    KCalCore::Incidence::Ptr theIncidence = incidence(itemId);
    if (!theIncidence) {
        *error = QOrganizerManager::DoesNotExistError;
        return QOrganizerItem();
    }
    QOrganizerItem item;
    if (convertIncidenceToItem(theIncidence, &item)) {
        return item;
    } else {
        *error = QOrganizerManager::UnspecifiedError;
        return QOrganizerItem();
    }
}

bool MKCalEngine::saveStorage(QOrganizerItemChangeSet* ics, QOrganizerManager::Error* error)
{
// TODO: because a bug in mkCal save() can fail sometimes (for the same testcase) by saying "constraint failed for incidence ..."
// even if in the memory everything is ok. We will not check the result of the save. Remove the comment when is fixed in mkCal.
/*    if (d->m_storagePtr->save()) { // commit all changes to the database.
        if (ics)
            ics->emitSignals(this);
    } else {
        *error = QOrganizerManager::UnspecifiedError;
    }*/
    d->m_storagePtr->save();

    if (ics)
        ics->emitSignals(this);

    return *error == QOrganizerManager::NoError;
}

bool MKCalEngine::saveItems(QList<QOrganizerItem>* items, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error)
{
    QMutexLocker locker(&d->m_operationMutex);

    if (!items) {
        *error = QOrganizerManager::BadArgumentError;
        return false;
    }

    QOrganizerItemChangeSet ics;
    QOrganizerManager::Error tempError = QOrganizerManager::NoError;
    *error = QOrganizerManager::NoError;

    for (int i = 0; i < items->size(); i++) {
        QOrganizerItem item = items->at(i);

        if (softSaveItem(&ics, &item, &tempError)) {
            items->replace(i, item);
        } else {
            *error = tempError;
            errorMap->insert(i, tempError);
        }
    }

    return saveStorage(&ics, error);
}

bool MKCalEngine::saveItem(QOrganizerItem* item,  QOrganizerManager::Error* error)
{
    QOrganizerItemChangeSet ics;

    return softSaveItem(&ics, item, error) && saveStorage(&ics, error);
}

bool MKCalEngine::removeItems(const QList<QOrganizerItemId>& itemIds, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error)
{
    QMutexLocker locker(&d->m_operationMutex);

    QOrganizerItemChangeSet ics;
    *error = QOrganizerManager::NoError;

    for (int i = 0; i < itemIds.size(); i++) {
        const QOrganizerItemId& id = itemIds[i];
        KCalCore::Incidence::Ptr theIncidence = incidence(id);

        if (!theIncidence) {
            *error = QOrganizerManager::DoesNotExistError;
            errorMap->insert(i, QOrganizerManager::DoesNotExistError);
            continue;
        }

        //if the item recurs remove its persistent exceptions
        if (theIncidence->recurs()) {
            // kcalcore calendar::deleteIncidenceInstances doesn't work
            if (theIncidence->type() == KCalCore::IncidenceBase::TypeEvent) {
                if (!d->m_calendarBackendPtr->deleteEventInstances(
                            theIncidence.staticCast<KCalCore::Event>())) {
                    *error = QOrganizerManager::UnspecifiedError;
                    errorMap->insert(i, QOrganizerManager::UnspecifiedError);
                }
            } else if (theIncidence->type() == KCalCore::IncidenceBase::TypeTodo) {
                if (!d->m_calendarBackendPtr->deleteTodoInstances(
                            theIncidence.staticCast<KCalCore::Todo>())) {
                    *error = QOrganizerManager::UnspecifiedError;
                    errorMap->insert(i, QOrganizerManager::UnspecifiedError);
                }
            }
        }

        //delete the item from the calendar
        if (!d->m_calendarBackendPtr->deleteIncidence(theIncidence)) {
            *error = QOrganizerManager::UnspecifiedError;
            errorMap->insert(i, QOrganizerManager::UnspecifiedError);
        } else {
            ics.insertRemovedItem(id);
        }
    }

    return saveStorage(&ics, error);
}

QMap<QString, QOrganizerItemDetailDefinition> MKCalEngine::detailDefinitions(const QString& itemType, QOrganizerManager::Error* error) const
{
    *error = QOrganizerManager::NoError;
    return schemaDefinitions().value(itemType);
}

QOrganizerItemDetailDefinition MKCalEngine::detailDefinition(const QString& definitionId, const QString& itemType, QOrganizerManager::Error* error) const
{
    return QOrganizerManagerEngine::detailDefinition(definitionId, itemType, error);
}

bool MKCalEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& itemType, QOrganizerManager::Error* error)
{
    return QOrganizerManagerEngine::saveDetailDefinition(def, itemType, error);
}

bool MKCalEngine::removeDetailDefinition(const QString& definitionId, const QString& itemType, QOrganizerManager::Error* error)
{
    return QOrganizerManagerEngine::removeDetailDefinition(definitionId, itemType, error);
}

QOrganizerCollection MKCalEngine::defaultCollection(QOrganizerManager::Error* error) const
{
    QMutexLocker locker(&d->m_operationMutex);

    *error = QOrganizerManager::NoError;
    mKCal::Notebook::Ptr defaultNotebook = d->m_storagePtr->defaultNotebook();
    if (defaultNotebook) {
        return convertNotebookToCollection(defaultNotebook);
    }

    // no default collection; create one.
    return convertNotebookToCollection(d->m_storagePtr->createDefaultNotebook("defaultNotebook"));
}

QOrganizerCollection MKCalEngine::collection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error) const
{
    QMutexLocker locker(&d->m_operationMutex);

    QString notebookUid = MKCalCollectionId::id_cast(collectionId)->uid();
    mKCal::Notebook::Ptr notebookPtr;
    if (notebookUid.isEmpty() || !(notebookPtr = d->m_storagePtr->notebook(notebookUid))) {
        *error = QOrganizerManager::DoesNotExistError;
        return QOrganizerCollection();
    }
    *error = QOrganizerManager::NoError;
    return convertNotebookToCollection(notebookPtr);
}

QList<QOrganizerCollection> MKCalEngine::collections(QOrganizerManager::Error* error) const
{
    QMutexLocker locker(&d->m_operationMutex);

    QList<QOrganizerCollection> retn;
    mKCal::Notebook::List allNotebooks(d->m_storagePtr->notebooks());
    foreach(const mKCal::Notebook::Ptr& currNotebook, allNotebooks) {
        retn.append(convertNotebookToCollection(currNotebook));
    }

    *error = QOrganizerManager::NoError;
    return retn;
}

bool MKCalEngine::saveCollection(QOrganizerCollection* collection, QOrganizerManager::Error* error)
{
    QMutexLocker locker(&d->m_operationMutex);

    QOrganizerCollectionId colId = collection->id();

    if (colId.isNull()) {
        // new collection.
        mKCal::Notebook::Ptr notebookPtr(new mKCal::Notebook);
        convertCollectionToNotebook(*collection, notebookPtr);

        if (!d->m_storagePtr->addNotebook(notebookPtr)) {
            *error = QOrganizerManager::InvalidCollectionError;
            return false;
        } else {
            // update the collection with its id.
            QOrganizerCollectionId newId(new MKCalCollectionId(notebookPtr->uid()));
            collection->setId(newId);
        }

        return saveStorage(NULL, error);
    }

    // retrieve the uid from the collection id
    // to try and get a pre-existing notebook.
    QString notebookUid = MKCalCollectionId::id_cast(colId)->uid();
    mKCal::Notebook::Ptr notebookPtr;
    if (notebookUid.isEmpty() || !(notebookPtr = d->m_storagePtr->notebook(notebookUid))) {
        // this notebook has been deleted (or never existed).
        *error = QOrganizerManager::DoesNotExistError;
        return false;
    }

    convertCollectionToNotebook(*collection, notebookPtr);

    if (!d->m_storagePtr->updateNotebook(notebookPtr)) {
        *error = QOrganizerManager::UnspecifiedError;
        return false;
    }

    return saveStorage(NULL, error);
}

bool MKCalEngine::removeCollection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error)
{
    QMutexLocker locker(&d->m_operationMutex);

    // first, check to see if it's the default collection.
    if (defaultCollection(error).id() == collectionId) {
        *error = QOrganizerManager::PermissionsError;
        return false;
    }

    // otherwise, it's a potentially removable collection.
    QString notebookUid = MKCalCollectionId::id_cast(collectionId)->uid();
    mKCal::Notebook::Ptr notebookPtr;
    if (!notebookUid.isEmpty() && (notebookPtr = d->m_storagePtr->notebook(notebookUid))) {
        if (d->m_storagePtr->deleteNotebook(notebookPtr)) {
            // success.
            return saveStorage(NULL, error);
        } else {
            *error = QOrganizerManager::UnspecifiedError;
        }
    } else {
        *error = QOrganizerManager::DoesNotExistError;
    }

    return false;
}

bool MKCalEngine::startRequest(QOrganizerAbstractRequest* req)
{
    d->m_asynchProcess->startRequest(req);
    return true;
}

bool MKCalEngine::cancelRequest(QOrganizerAbstractRequest* req)
{
    return d->m_asynchProcess->cancelRequest(req);
}

bool MKCalEngine::waitForRequestFinished(QOrganizerAbstractRequest* req, int msecs)
{
    return d->m_asynchProcess->waitForRequestFinished(req, msecs);
}

void MKCalEngine::requestDestroyed(QOrganizerAbstractRequest* req)
{
    return d->m_asynchProcess->requestDestroyed(req);
}

bool MKCalEngine::hasFeature(QOrganizerManager::ManagerFeature feature, const QString& itemType) const
{
    Q_UNUSED(itemType);
    switch(feature) {
        case QOrganizerManager::MutableDefinitions:
            return false;
        case QOrganizerManager::Anonymous:
            return false;
        case QOrganizerManager::ChangeLogs:
            return false;
    }
    return false;
}

bool MKCalEngine::isFilterSupported(const QOrganizerItemFilter& filter) const
{
    Q_UNUSED(filter);
    return false;
}

QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > MKCalEngine::schemaDefinitions() const {
    // lazy initialisation of schema definitions.
    static QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > defs;

    if (defs.isEmpty()) {
        //supported item types
        QSet<QString> itemTypes = QSet<QString>()
            << QOrganizerItemType::TypeEvent
            << QOrganizerItemType::TypeEventOccurrence
            << QOrganizerItemType::TypeTodo
            << QOrganizerItemType::TypeTodoOccurrence
            << QOrganizerItemType::TypeJournal
            << QOrganizerItemType::TypeNote;

        //supported definitions
        QSet<QString> definitions = QSet<QString>()
            << QOrganizerItemType::DefinitionName
            << QOrganizerItemDescription::DefinitionName
            << QOrganizerItemDisplayLabel::DefinitionName
            << QOrganizerItemRecurrence::DefinitionName
            << QOrganizerEventTime::DefinitionName
            << QOrganizerItemGuid::DefinitionName
            << QOrganizerItemParent::DefinitionName
            << QOrganizerTodoTime::DefinitionName
            << QOrganizerItemLocation::DefinitionName;

        // Loop through default schema definitions
        QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > schema
            = QOrganizerManagerEngine::schemaDefinitions();
        foreach (const QString& itemType, schema.keys()) {
            // Only add the item types that we support
            if (itemTypes.contains(itemType)) {
                QMap<QString, QOrganizerItemDetailDefinition> definitions
                    = schema.value(itemType);

                QMap<QString, QOrganizerItemDetailDefinition> supportedDefinitions;

                QMapIterator<QString, QOrganizerItemDetailDefinition> it(definitions);
                while (it.hasNext()) {
                    it.next();
                    // Only add the definitions that we support
                    if (definitions.contains(it.key())) {
                        supportedDefinitions.insert(it.key(), it.value());
                    }
                }

                defs.insert(itemType, supportedDefinitions);
            }
        }
    }

    return defs;
}

// observer for changes from mKCal.
void MKCalEngine::storageModified(mKCal::ExtendedStorage* storage, const QString& info)
{
    Q_UNUSED(info);
    if (storage != d->m_storagePtr) {
        qWarning("MKCalEngine::storageModified() received change notification from unknown storage!");
        return;
    }

    // XXX The mKCal docs says to not use this function because it's too slow!
    // Can we optimize by only loading items from the date range required, perhaps lazily?
    d->m_storagePtr->load();

    emit dataChanged();
}

// observer for changes from mKCal.
void MKCalEngine::storageProgress(mKCal::ExtendedStorage* storage, const QString& info)
{
    Q_UNUSED(storage);
    Q_UNUSED(info);
    // do nothing.
}

// observer for changes from mKCal.
void MKCalEngine::storageFinished(mKCal::ExtendedStorage* storage, bool error, const QString& info)
{
    Q_UNUSED(storage);
    Q_UNUSED(error);
    Q_UNUSED(info);
    // do nothing.
}

KCalCore::Incidence::Ptr MKCalEngine::incidence(const QOrganizerItemId& itemId) const
{
    const MKCalItemId *id = MKCalItemId::id_cast(itemId);
    return id->id().isEmpty() ? KCalCore::Incidence::Ptr() : d->m_calendarBackendPtr->incidence(id->id(), id->recurrenceID());
}

KCalCore::Incidence::Ptr MKCalEngine::createPersistentException(const QOrganizerItem& item) const
{
    // dissociate a single occurrence from the parent item to be able to save it later as a persistent exception
    QOrganizerItemParent parentDetail(item.detail<QOrganizerItemParent>());
    QOrganizerItemId parentId(parentDetail.parentId());
    QDate originalDate(parentDetail.originalDate());
    QString guid(item.guid());

    KCalCore::Incidence::Ptr parentIncidence;
    // check that parentId is not null
    if (!parentId.isNull()) {
        // get the engine id
        QString parentUid(MKCalItemId::id_cast(parentId)->id());
        // if is null then it is incompatible with this engine
        // also check that guid is equal with the engine id
        if (!guid.isEmpty() && guid != parentUid)
            return KCalCore::Incidence::Ptr();

        // retrieve the parent based on the engine id
        parentIncidence = incidence(parentId);
    } else if (!guid.isEmpty()) {
        // if parentId was null, but guid is not null, use that to find the parent
        parentIncidence = d->m_calendarBackendPtr->incidence(guid);
    }

    // if the parent coult not be retrieved or the original date is not valid return empty incidence
    if (parentIncidence.isNull() || !originalDate.isValid())
        return KCalCore::Incidence::Ptr();

    // call the backend to create the persistent exception
    return d->m_calendarBackendPtr->dissociateSingleOccurrence(
            parentIncidence, KDateTime(originalDate), KDateTime::LocalZone);
}

/*!
 * Saves \a item to the manager, but doesn't persist the change to disk.
 * Sets \a error appropriately if if couldn't be saved.
 */
bool MKCalEngine::softSaveItem(QOrganizerItemChangeSet* ics, QOrganizerItem* item, QOrganizerManager::Error* error)
{
    bool itemIsNew = item->id().isNull();

    // check manager uri if is the same with the engine uri
    if (!itemIsNew && (d->m_managerUri != item->id().managerUri())) {
        *error = QOrganizerManager::BadArgumentError;
        return false;
    }

    // ensure that the organizeritem's details conform to their definitions
    if (!validateItem(*item, error))
        return false;

    bool itemIsOccurrence = (item->type() == QOrganizerItemType::TypeEventOccurrence) ||
                            (item->type() == QOrganizerItemType::TypeTodoOccurrence);
    KCalCore::Incidence::Ptr newIncidence(0);

    // extract the collection id and ensure that we save it in the correct notebook
    QOrganizerCollectionId destinationCollectionId = item->collectionId();
    QString destinationNotebookUid;

    if (destinationCollectionId.isNull()) {
        // save to default collection.
        destinationNotebookUid = d->m_storagePtr->defaultNotebook()->uid();
        // note that we readjust the destinationNotebookUid if the item is an occurrence, so that it is always saved in the parent's notebook.
    } else {
        // save to the specified collection (if possible)
        destinationNotebookUid = MKCalCollectionId::id_cast(destinationCollectionId)->uid();
    }

    // if destination notebook uid is null it means that the id belonged to a different uri or the default notebook does not have a valid id
    if (destinationNotebookUid.isEmpty()) {
        *error = QOrganizerManager::InvalidCollectionError;
        return false;
    }

    // mkCal backend does not support setting of notebooks for item occurrences, because of this the item collection id either should be null
    // or equal with it's parent collection id
    if (itemIsOccurrence && !destinationCollectionId.isNull()) {
        // find parent id
        QOrganizerItemParent parentDetail(item->detail<QOrganizerItemParent>());
        QOrganizerItemId parentId(parentDetail.parentId());
        QString parentUid = parentId.isNull() ?  item->guid() : MKCalItemId::id_cast(parentId)->id();
        QString parentNotebookUid = parentUid.isEmpty() ? QLatin1String("") : d->m_calendarBackendPtr->notebook(parentUid);

        // if item collection id and parent id are not null, then item collection id must be equal with parent collection id
        if (!destinationNotebookUid.isEmpty() && parentNotebookUid != destinationNotebookUid) {
            *error = QOrganizerManager::InvalidCollectionError;
            return false;
        }

        // if the collection id was null update the destinationNotebookUid to the parent notebook id
        if (destinationNotebookUid.isEmpty())
            destinationNotebookUid = parentNotebookUid;
    } else if (!(d->m_calendarBackendPtr->hasValidNotebook(destinationNotebookUid))) {
        // fail if destination notebook does not exist in the storage
        *error = QOrganizerManager::InvalidCollectionError;
        return false;
    }

    // First, either create the incidence or get the correct existing one
    if (itemIsNew) {
        if (itemIsOccurrence) {
            // if it is an occurrence create a persistent exception
            newIncidence = createPersistentException(*item);
            if (newIncidence.isNull()) {
                *error = QOrganizerManager::InvalidOccurrenceError;
                return false;
            }
        } else {
            // create a new incifence based on item type
            if (item->type() == QOrganizerItemType::TypeEvent) {
                newIncidence = KCalCore::Event::Ptr(new KCalCore::Event);
            } else if (item->type() == QOrganizerItemType::TypeTodo) {
                newIncidence = KCalCore::Todo::Ptr(new KCalCore::Todo);
            } else if (item->type() == QOrganizerItemType::TypeNote
                    || item->type() == QOrganizerItemType::TypeJournal) {
                newIncidence = KCalCore::Journal::Ptr(new KCalCore::Journal);
            } else {
                *error = QOrganizerManager::InvalidItemTypeError;
                return false;
            }
        }
    } else {
        // if is an existing occurrence try to retrieve it based on its id
        newIncidence = incidence(item->id());
        if (!newIncidence) {
            *error = QOrganizerManager::DoesNotExistError;
            return false;
        }
    }

    Q_ASSERT(!newIncidence.isNull());

    // second, populate the incidence with the item's details
    updateIncidenceFromItem(*item, newIncidence);

    // third, add it if it is new
    if (itemIsNew) {
        d->m_calendarBackendPtr->addIncidence(newIncidence);
    } // if it is not new, nothing needs to be done (incidences are live objects)

    bool success = itemIsOccurrence || d->m_calendarBackendPtr->setNotebook(newIncidence, destinationNotebookUid);
    if (!success) {
        // unable to save to the correct notebook.
        *error = QOrganizerManager::InvalidCollectionError;
        // XXX TODO: roll back the save from memory.
    } else {
        *error = QOrganizerManager::NoError;
    }

    // set the id of the item.
    QString kId = newIncidence->uid();
    item->setId(QOrganizerItemId(new MKCalItemId(
                kId,
                newIncidence->hasRecurrenceId() ? newIncidence->recurrenceId() : KDateTime())));
    item->setGuid(kId);
    // if the initial collection id was null, set it to the destinationNotbookUid
    if (destinationCollectionId.isNull()) {
        item->setCollectionId(QOrganizerCollectionId(new MKCalCollectionId(destinationNotebookUid)));
    }

    // modify the changeset as required.
    if (itemIsNew) {
        ics->insertAddedItem(item->id());
    } else {
        ics->insertChangedItem(item->id());
    }

    return *error == QOrganizerManager::NoError;
}

void MKCalEngine::updateIncidenceFromItem(const QOrganizerItem& item, KCalCore::Incidence::Ptr incidence)
{

    // only set the id from guid if is not an occurrence because in the case of an occurrence
    // it will be handled in dissociateSingleOccurrence
    if (item.id().isNull() && !item.guid().isEmpty() &&
            (item.type() != QOrganizerItemType::TypeEventOccurrence) &&
            (item.type() != QOrganizerItemType::TypeTodoOccurrence)) {
        incidence->setUid(item.guid());
    }

    incidence->setDescription(item.description());
    incidence->setSummary(item.displayLabel());
    QOrganizerItemLocation loc =  item.detail<QOrganizerItemLocation>();
    incidence->setLocation(loc.label());

    if (item.type() == QOrganizerItemType::TypeEvent || item.type() == QOrganizerItemType::TypeEventOccurrence) {
        updateIncidenceFromEvent(item, incidence.staticCast<KCalCore::Event>());
    } else if (item.type() == QOrganizerItemType::TypeTodo || item.type() == QOrganizerItemType::TypeTodoOccurrence) {
        updateIncidenceFromTodo(item, incidence.staticCast<KCalCore::Todo>());
    } else if (item.type() == QOrganizerItemType::TypeJournal) {
        updateIncidenceFromJournal(item, incidence.staticCast<KCalCore::Journal>());
    }
}

void MKCalEngine::updateIncidenceFromEvent(const QOrganizerItem& item, KCalCore::Event::Ptr event)
{
    QOrganizerEventTime eventTime(item.detail<QOrganizerEventTime>());
    event->setDtStart(KDateTime(eventTime.startDateTime()));
    event->setDtEnd(KDateTime(eventTime.endDateTime()));
    if (item.type() == QOrganizerItemType::TypeEvent)
        convertQRecurrenceToKRecurrence(item.detail<QOrganizerItemRecurrence>(),
                                        eventTime.startDateTime().date(),
                                        event->recurrence());
}

void MKCalEngine::updateIncidenceFromTodo(const QOrganizerItem& item, KCalCore::Todo::Ptr todo)
{
    QOrganizerTodoTime todoTime(item.detail<QOrganizerTodoTime>());
    todo->setDtStart(KDateTime(todoTime.startDateTime()));
    todo->setDtDue(KDateTime(todoTime.dueDateTime()));
    if (item.type() == QOrganizerItemType::TypeTodo)
        convertQRecurrenceToKRecurrence(item.detail<QOrganizerItemRecurrence>(),
                                        todoTime.startDateTime().date(),
                                        todo->recurrence());
}

void MKCalEngine::updateIncidenceFromJournal(const QOrganizerItem& item, KCalCore::Journal::Ptr journal)
{
    QOrganizerJournalTime dt = item.detail<QOrganizerJournalTime>();
    journal->setDtStart(KDateTime(dt.entryDateTime()));
}

/*! Converts \a qRecurrence into the libkcal equivalent, stored in \a kRecurrence.  kRecurrence must
 * point to an initialized Recurrence.
 */
void MKCalEngine::convertQRecurrenceToKRecurrence(const QOrganizerItemRecurrence& qRecurrence, const QDate& startDate,
        KCalCore::Recurrence* kRecurrence)
{
    kRecurrence->clear();

    foreach (const QOrganizerRecurrenceRule& rrule, qRecurrence.recurrenceRules()) {
        if (rrule.frequency() != QOrganizerRecurrenceRule::Invalid) {
            kRecurrence->addRRule(createKRecurrenceRule(startDate, rrule));
        }
    }

    foreach (const QOrganizerRecurrenceRule& exrule, qRecurrence.exceptionRules()) {
        if (exrule.frequency() != QOrganizerRecurrenceRule::Invalid) {
            kRecurrence->addExRule(createKRecurrenceRule(startDate, exrule));
        }
    }

    foreach (const QDate& rdate, qRecurrence.recurrenceDates())
        kRecurrence->addRDate(rdate);

    foreach (const QDate& exdate, qRecurrence.exceptionDates())
        kRecurrence->addExDate(exdate);
}

KCalCore::RecurrenceRule* MKCalEngine::createKRecurrenceRule(const QDate& startDate, const QOrganizerRecurrenceRule& qRRule)
{
    KCalCore::RecurrenceRule* kRRule = new KCalCore::RecurrenceRule();
    switch (qRRule.frequency()) {
        case QOrganizerRecurrenceRule::Daily:
            kRRule->setRecurrenceType(KCalCore::RecurrenceRule::rDaily);
            break;
        case QOrganizerRecurrenceRule::Weekly:
            kRRule->setRecurrenceType(KCalCore::RecurrenceRule::rWeekly);
            break;
        case QOrganizerRecurrenceRule::Monthly:
            kRRule->setRecurrenceType(KCalCore::RecurrenceRule::rMonthly);
            break;
        case QOrganizerRecurrenceRule::Yearly:
            kRRule->setRecurrenceType(KCalCore::RecurrenceRule::rYearly);
            break;
        case QOrganizerRecurrenceRule::Invalid:
            //just to handle the compiler warning, it will never reach this point
            break;
    }
    kRRule->setFrequency(qRRule.interval());
    if (qRRule.limitCount() > 0) {
        kRRule->setDuration(qRRule.limitCount());
    }
    kRRule->setStartDt(KDateTime(startDate));
    QDate endDate = qRRule.limitDate();
    if (endDate.isValid()) {
        kRRule->setEndDt(KDateTime(endDate));
    }

    //TODO: QOrganizerRecurrenceRule does not support position associated with dayOfWeek so we will always
    //take the first position. When this will be implemented in the future please fix the code below.
    int pos = qRRule.positions().isEmpty() ? 0 : *qRRule.positions().begin();

    QList<KCalCore::RecurrenceRule::WDayPos> daysOfWeek;
    foreach (Qt::DayOfWeek dayOfWeek, qRRule.daysOfWeek()) {
        daysOfWeek.append(KCalCore::RecurrenceRule::WDayPos(pos, (short)dayOfWeek));
    }
    kRRule->setByDays(daysOfWeek);

    kRRule->setByMonthDays(qRRule.daysOfMonth().toList());

    kRRule->setByYearDays(qRRule.daysOfYear().toList());

    kRRule->setByWeekNumbers(qRRule.weeksOfYear().toList());

    QList<int> months;
    foreach (QOrganizerRecurrenceRule::Month month, qRRule.monthsOfYear()) {
        months.append((int)month);
    }
    kRRule->setByMonths(months);

    kRRule->setWeekStart((short)qRRule.firstDayOfWeek());

    return kRRule;
}

/*!
 * Converts a kcal \a incidence into a QOrganizer \a item.
 * \a incidence and \a item must both not be null.
 * \item must point to a default-constructed item.
 */
bool MKCalEngine::convertIncidenceToItem(
        KCalCore::Incidence::Ptr incidence, QOrganizerItem* item) const
{
    // convert common details first
    item->setId(QOrganizerItemId(new MKCalItemId(
                    incidence->uid(),
                    incidence->hasRecurrenceId() ? incidence->recurrenceId() : KDateTime())));
    item->setCollectionId(QOrganizerCollectionId(new MKCalCollectionId(
                    d->m_calendarBackendPtr->notebook(incidence))));

    if (!incidence->summary().isEmpty())
        item->setDisplayLabel(incidence->summary());
    if (!incidence->description().isEmpty())
        item->setDescription(incidence->description());
    if (!incidence->location().isEmpty()) {
        QOrganizerItemLocation location;
        location.setLabel(incidence->location());
        item->saveDetail(&location);
    }

    item->setGuid(incidence->uid());

    // convert individual details based on type
    if (incidence->type() == KCalCore::IncidenceBase::TypeEvent) {
        convertKEventToQEvent(incidence.staticCast<KCalCore::Event>(), item);
    } else if (incidence->type() == KCalCore::IncidenceBase::TypeTodo) {
        convertKTodoToQTodo(incidence.staticCast<KCalCore::Todo>(), item);
    } else if (incidence->type() == KCalCore::IncidenceBase::TypeJournal) {
        convertKJournalToQJournal(incidence.staticCast<KCalCore::Journal>(), item);
    } else {
        return false;
    }

    return true;
}

/*!
 * Converts a kcal Event.
 */
void MKCalEngine::convertKEventToQEvent(KCalCore::Event::Ptr e, QOrganizerItem* item) const
{
    QOrganizerEvent* event = static_cast<QOrganizerEvent*>(item);
    if (!e->dtStart().isNull())
        event->setStartDateTime(e->dtStart().dateTime());
    if (!e->dtEnd().isNull())
        event->setEndDateTime(e->dtEnd().dateTime());

    if (e->hasRecurrenceId()) {
        item->setType(QOrganizerItemType::TypeEventOccurrence);
        QOrganizerEventOccurrence* eventOccurrence = static_cast<QOrganizerEventOccurrence*>(item);
        eventOccurrence->setOriginalDate(e->recurrenceId().date());
        eventOccurrence->setParentId(QOrganizerItemId(new MKCalItemId(
                               e->uid(),
                               KDateTime())));
    } else {
        item->setType(QOrganizerItemType::TypeEvent);
        convertKRecurrenceToQRecurrence(e->recurrence(), item);
    }
}

/*!
 * Converts a kcal Todo.
 */
void MKCalEngine::convertKTodoToQTodo(KCalCore::Todo::Ptr t, QOrganizerItem* item) const
{
    QOrganizerTodo* todo = static_cast<QOrganizerTodo*>(item);
    if (!t->dtStart().isNull())
        todo->setStartDateTime(t->dtStart().dateTime());
    if (!t->dtDue().isNull())
        todo->setDueDateTime(t->dtDue().dateTime());

    if (t->hasRecurrenceId()) {
        item->setType(QOrganizerItemType::TypeTodoOccurrence);
        QOrganizerTodoOccurrence* todoOccurrence = static_cast<QOrganizerTodoOccurrence*>(item);
        todoOccurrence->setOriginalDate(t->recurrenceId().date());
        todoOccurrence->setParentId(QOrganizerItemId(new MKCalItemId(
                               t->uid(),
                               KDateTime())));
    } else {
        item->setType(QOrganizerItemType::TypeTodo);
        convertKRecurrenceToQRecurrence(t->recurrence(), item);
    }
}

/*!
 * Converts a kcal Journal.
 */
void MKCalEngine::convertKJournalToQJournal(KCalCore::Journal::Ptr j, QOrganizerItem* item) const
{
    if (j->dtStart().isValid()) {
        item->setType(QOrganizerItemType::TypeJournal);
        static_cast<QOrganizerJournal*>(item)->setDateTime(j->dtStart().dateTime());
    } else {
        item->setType(QOrganizerItemType::TypeNote);
    }
}

void MKCalEngine::convertKRecurrenceToQRecurrence(const KCalCore::Recurrence* kRecurrence, QOrganizerItem* item) const
{
    bool modified = false;
    QOrganizerItemRecurrence recurrence;
    foreach (const KCalCore::RecurrenceRule* kRRule, kRecurrence->rRules()) {
        QOrganizerRecurrenceRule rrule(createQRecurrenceRule(kRRule));
        if (rrule.frequency() != QOrganizerRecurrenceRule::Invalid) {
            recurrence.setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << rrule);
            modified = true;
        }
    }
    foreach (const KCalCore::RecurrenceRule* kExRule, kRecurrence->exRules()) {
        QOrganizerRecurrenceRule exrule(createQRecurrenceRule(kExRule));
        if (exrule.frequency() != QOrganizerRecurrenceRule::Invalid) {
            recurrence.setExceptionRules(QSet<QOrganizerRecurrenceRule>() << exrule);
            modified = true;
        }
    }
    if (!kRecurrence->rDates().isEmpty()) {
        recurrence.setRecurrenceDates(kRecurrence->rDates().toSet());
        modified = true;
    }
    if (!kRecurrence->exDates().isEmpty()) {
        recurrence.setExceptionDates(kRecurrence->exDates().toSet());
        modified = true;
    }
    if (modified)
        item->saveDetail(&recurrence);
}

QOrganizerRecurrenceRule MKCalEngine::createQRecurrenceRule(const KCalCore::RecurrenceRule* kRRule) const
{
    QOrganizerRecurrenceRule qRRule;
    switch (kRRule->recurrenceType()) {
        case KCalCore::RecurrenceRule::rDaily:
            qRRule.setFrequency(QOrganizerRecurrenceRule::Daily);
            break;
        case KCalCore::RecurrenceRule::rWeekly:
            qRRule.setFrequency(QOrganizerRecurrenceRule::Weekly);
            break;
        case KCalCore::RecurrenceRule::rMonthly:
            qRRule.setFrequency(QOrganizerRecurrenceRule::Monthly);
            break;
        case KCalCore::RecurrenceRule::rYearly:
            qRRule.setFrequency(QOrganizerRecurrenceRule::Yearly);
            break;
        default:
            return qRRule;
    }

    qRRule.setInterval(kRRule->frequency());

    if (kRRule->duration() > 0) {
        qRRule.setLimit(kRRule->duration());
    } else {
        QDate limitDate(kRRule->endDt().dateTime().date());
        if (limitDate.isValid())
            qRRule.setLimit(limitDate);
    }

    QSet<Qt::DayOfWeek> daysOfWeek;
    foreach (KCalCore::RecurrenceRule::WDayPos wday, kRRule->byDays())
        daysOfWeek.insert(static_cast<Qt::DayOfWeek>(wday.day()));
    qRRule.setDaysOfWeek(daysOfWeek);

    qRRule.setDaysOfMonth(kRRule->byMonthDays().toSet());

    qRRule.setDaysOfYear(kRRule->byYearDays().toSet());

    qRRule.setWeeksOfYear(kRRule->byWeekNumbers().toSet());

    QSet<QOrganizerRecurrenceRule::Month> months;
    foreach (int month, kRRule->byMonths()) {
        months.insert(static_cast<QOrganizerRecurrenceRule::Month>(month));
    }
    qRRule.setMonthsOfYear(months);

    qRRule.setFirstDayOfWeek(static_cast<Qt::DayOfWeek>(kRRule->weekStart()));

    return qRRule;
}

/*!
 * Convert mKCal notebook to QOrganizerCollection
 */
QOrganizerCollection MKCalEngine::convertNotebookToCollection(mKCal::Notebook::Ptr notebook) const
{
    QOrganizerCollection retn;

    QString string;
    QStringList stringList;
    QDateTime dateTime;

    if (!(string = notebook->name()).isEmpty())
        retn.setMetaData(QOrganizerCollection::KeyName, string);
    if (!(string = notebook->color()).isEmpty())
        retn.setMetaData(QOrganizerCollection::KeyColor, string);
    if (!(string = notebook->description()).isEmpty())
        retn.setMetaData(QOrganizerCollection::KeyDescription, string);
    // XXX We lose the timezone information here!:
    if ((dateTime = notebook->syncDate().dateTime()).isValid())
        retn.setMetaData(NotebookSyncDate, dateTime);
    if ((dateTime = notebook->modifiedDate().dateTime()).isValid())
        retn.setMetaData(NotebookModifiedDate, dateTime);
    if (!(string = notebook->pluginName()).isEmpty())
        retn.setMetaData(NotebookPluginName, string);
    if (!(string = notebook->account()).isEmpty())
        retn.setMetaData(NotebookAccount, string);
    if (!(stringList = notebook->sharedWith()).isEmpty())
        retn.setMetaData(NotebookSharedWith, stringList);
    if (!(string = notebook->sharedWithStr()).isEmpty())
        retn.setMetaData(NotebookSharedWithStr, string);
    if (!(string = notebook->syncProfile()).isEmpty())
        retn.setMetaData(NotebookSyncProfile, string);
    int attachmentSize = notebook->attachmentSize();
    if (attachmentSize >= 0)
        retn.setMetaData(NotebookAttachmentSize, attachmentSize);

    // Boolean flags that we always store
    retn.setMetaData(NotebookIsDefault, notebook->isDefault());
    retn.setMetaData(NotebookIsShareable, notebook->isShareable());
    retn.setMetaData(NotebookIsShared, notebook->isShared());
    retn.setMetaData(NotebookIsMaster, notebook->isMaster());
//    retn.setMetaData(NotebookIsSynchronized, notebook->isSynchronized());
    retn.setMetaData(NotebookIsReadOnly, notebook->isReadOnly());
    retn.setMetaData(NotebookIsVisible, notebook->isVisible());
    retn.setMetaData(NotebookIsRunTimeOnly, notebook->isRunTimeOnly());
    retn.setMetaData(NotebookEventsAllowed, notebook->eventsAllowed());
    retn.setMetaData(NotebookJournalsAllowed, notebook->journalsAllowed());
    retn.setMetaData(NotebookTodosAllowed, notebook->todosAllowed());

    // now set the id of the collection.
    QOrganizerCollectionId colId(new MKCalCollectionId(notebook->uid()));
    retn.setId(colId);

    // done
    return retn;
}

/*!
 * Convert QOrganizerCollection to mKCal notebook
 */
void MKCalEngine::convertCollectionToNotebook(const QOrganizerCollection& collection, mKCal::Notebook::Ptr notebook) const
{
    QVariant variant;
    if (!(variant = collection.metaData(QOrganizerCollection::KeyName)).isNull())
        notebook->setName(variant.toString());

    if (!(variant = collection.metaData(QOrganizerCollection::KeyColor)).isNull())
        notebook->setColor(variant.toString());
    if (!(variant = collection.metaData(QOrganizerCollection::KeyDescription)).isNull())
        notebook->setDescription(variant.toString());
    if (!(variant = collection.metaData(NotebookSyncDate)).isNull())
        notebook->setSyncDate(KDateTime(variant.toDateTime()));
    if (!(variant = collection.metaData(NotebookModifiedDate)).isNull())
        notebook->setModifiedDate(KDateTime(variant.toDateTime()));
    if (!(variant = collection.metaData(NotebookPluginName)).isNull())
        notebook->setPluginName(variant.toString());
    if (!(variant = collection.metaData(NotebookAccount)).isNull())
        notebook->setAccount(variant.toString());
    if (!(variant = collection.metaData(NotebookAttachmentSize)).isNull())
        notebook->setAttachmentSize(variant.toInt());
    if (!(variant = collection.metaData(NotebookSharedWith)).isNull())
        notebook->setSharedWith(variant.toStringList());
    if (!(variant = collection.metaData(NotebookSharedWithStr)).isNull())
        notebook->setSharedWithStr(variant.toString());
    if (!(variant = collection.metaData(NotebookSyncProfile)).isNull())
        notebook->setSyncProfile(variant.toString());

    // Boolean flags
    if (!(variant = collection.metaData(NotebookIsDefault)).isNull())
        notebook->setIsDefault(variant.toBool());
    if (!(variant = collection.metaData(NotebookIsShareable)).isNull())
        notebook->setIsShareable(variant.toBool());
    if (!(variant = collection.metaData(NotebookIsShared)).isNull())
        notebook->setIsShared(variant.toBool());
    if (!(variant = collection.metaData(NotebookIsMaster)).isNull())
        notebook->setIsMaster(variant.toBool());
/*    if (!(variant = collection.metaData(NotebookIsSynchronized)).isNull())
        notebook->setIsSynchronized(variant.toBool());*/
    if (!(variant = collection.metaData(NotebookIsReadOnly)).isNull())
        notebook->setIsReadOnly(variant.toBool());
    if (!(variant = collection.metaData(NotebookIsVisible)).isNull())
        notebook->setIsVisible(variant.toBool());
    if (!(variant = collection.metaData(NotebookIsRunTimeOnly)).isNull())
        notebook->setRunTimeOnly(variant.toBool());
    if (!(variant = collection.metaData(NotebookEventsAllowed)).isNull())
        notebook->setEventsAllowed(variant.toBool());
    if (!(variant = collection.metaData(NotebookJournalsAllowed)).isNull())
        notebook->setJournalsAllowed(variant.toBool());
    if (!(variant = collection.metaData(NotebookTodosAllowed)).isNull())
        notebook->setTodosAllowed(variant.toBool());
}

bool MKCalEngine::isIncidenceInInterval(KCalCore::Incidence::Ptr incidence, QDateTime startPeriod, QDateTime endPeriod) const
{
    if (startPeriod.isNull() && endPeriod.isNull())
        return true;

    QDateTime iStartDate = incidence->dtStart().dateTime();
    QDateTime iEndDate = iStartDate.addSecs(incidenceDuration(incidence));

    // if start period is not given, return true is the incidence start date is before the end period
    if (startPeriod.isNull())
        return iStartDate < endPeriod;

    // if end period is missing, return true is the incidence end date is after the start period
    if (endPeriod.isNull())
        return iEndDate > startPeriod;

    // if the incidence is starting before the period check that it's end date is after the period start
    if (startPeriod > iStartDate)
        return iEndDate >= startPeriod;

    // if the incidence is ending after the period check that it's start date is before the period end
    if (endPeriod < iEndDate)
        return iStartDate <= endPeriod;

    // incidence should be in the given interval, always true
    return true;
}

int MKCalEngine::incidenceDuration(KCalCore::Incidence::Ptr incidence) const
{
    // incidence->hasDuration() is not working so we do it manually
    int duration = 0;

    if (incidence->type() == KCalCore::Incidence::TypeEvent) {
        KCalCore::Event::Ptr ev = incidence.staticCast<KCalCore::Event>();
        if (!ev->dtEnd().isNull())
            duration = ev->dtStart().secsTo(ev->dtEnd());
    } else if (incidence->type() == KCalCore::Incidence::TypeTodo) {
        KCalCore::Todo::Ptr todo = incidence.staticCast<KCalCore::Todo>();
        if (!todo->dtDue().isNull())
            duration = todo->dtStart().secsTo(todo->dtDue());
    }

    return duration;
}
