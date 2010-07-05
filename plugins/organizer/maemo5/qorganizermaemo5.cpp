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

#include "qorganizermaemo5_p.h"
#include "qtorganizer.h"

#include <CalendarErrors.h>
#include <CEvent.h>
#include <CJournal.h>
#include <CTodo.h>
#include <CRecurrence.h>

QTM_USE_NAMESPACE

QOrganizerItemManagerEngine* QOrganizerItemMaemo5Factory::engine(const QMap<QString, QString> &parameters, QOrganizerItemManager::Error *error)
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);

    QOrganizerItemMaemo5Engine* retn = new QOrganizerItemMaemo5Engine(); // manager takes ownership and will clean up.
    return retn;
}

QString QOrganizerItemMaemo5Factory::managerName() const
{
    return QString("maemo5");
}
Q_EXPORT_PLUGIN2(qtorganizer_maemo5, QOrganizerItemMaemo5Factory);

QOrganizerItemMaemo5Engine::QOrganizerItemMaemo5Engine()
    : d(new QOrganizerItemMaemo5EngineData)
{
    d->m_itemTransformer.setManagerUri(managerUri());
}

QOrganizerItemMaemo5Engine::~QOrganizerItemMaemo5Engine()
{
}

QString QOrganizerItemMaemo5Engine::managerName() const
{
    return QLatin1String("maemo5");
}

QMap<QString, QString> QOrganizerItemMaemo5Engine::managerParameters() const
{
    return QMap<QString, QString>();
}

int QOrganizerItemMaemo5Engine::managerVersion() const
{
    return 1;
}

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::itemInstances(const QOrganizerItem &generator, const QDateTime &periodStart, const QDateTime &periodEnd, int maxCount, QOrganizerItemManager::Error *error) const
{
    *error = QOrganizerItemManager::NoError;
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    QList<QOrganizerItem> retn;

    if (periodStart > periodEnd)
    {
        *error = QOrganizerItemManager::BadArgumentError;
        return retn;
    }

    CCalendar *cal = d->m_mcInstance->getDefaultCalendar();
    std::string nativeId = QString::number(generator.localId()).toStdString();

    if (generator.type() == QOrganizerItemType::TypeEvent)
    {
        CEvent *cevent = cal->getEvent(nativeId, calError);
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (cevent && *error == QOrganizerItemManager::NoError)
        {
            // Get event instance times
            std::vector< std::time_t > eventInstanceDates;
            cevent->generateInstanceTimes(periodStart.toTime_t(), periodEnd.toTime_t(), eventInstanceDates);

            // Calculate the generator event duration (the occurrences will have the same duration)
            time_t generatorDuration = cevent->getDateEnd() - cevent->getDateStart();

            // Generate the event occurrences
            std::vector< std::time_t >::const_iterator i;
            for (i = eventInstanceDates.begin(); i != eventInstanceDates.end(); ++i)
            {
                QDateTime instanceStartDate = QDateTime::fromTime_t(*i);
                QDateTime instanceEndDate = QDateTime::fromTime_t(*i + generatorDuration);

                // Ensure that the instance is within the period.
                // CEvent::generateInstanceTimes seems to sometime return erroneous dates.
                if (instanceStartDate >= periodStart && instanceEndDate <= periodEnd) {
                    QOrganizerEventOccurrence eventOcc =
                            d->m_itemTransformer.convertCEventToQEventOccurrence(cevent, instanceStartDate, instanceEndDate);
                    d->m_itemTransformer.fillInCommonCComponentDetails(&eventOcc, cevent, false); // false = do not set ids
                    retn << eventOcc;
                }
            }

            // Now we have got the simple occurrences that are generated with the recurrence rules
            // But events can have also occurrence that are not generated with rules, but saved as
            // events (because they have become modified). Those occurrences are saved with GUID set
            // equal to the generator event's GUID.
            QString eventType = QOrganizerItemType::TypeEvent;
            QList<QString> occurrenceCandidateIds =
                    d->m_guidMapper.itemIds(QString::fromStdString(cevent->getGUid()), eventType);

            foreach(QString occurrenceCandidateId, occurrenceCandidateIds) {
                if (occurrenceCandidateId != QString::fromStdString(cevent->getId())) {
                    // for all other events than the generator itself
                    CEvent *coccurenceCandidate = cal->getEvent(occurrenceCandidateId.toStdString(), calError);
                    if (coccurenceCandidate && *error == QOrganizerItemManager::NoError) {
                        QDateTime instanceStartDate = QDateTime::fromTime_t(coccurenceCandidate->getDateStart());
                        QDateTime instanceEndDate = QDateTime::fromTime_t(coccurenceCandidate->getDateEnd());

                        // instance must be within the period
                        if (instanceStartDate >= periodStart && instanceEndDate <= periodEnd) {
                            QOrganizerEventOccurrence eventOcc =
                                    d->m_itemTransformer.convertCEventToQEventOccurrence(coccurenceCandidate, instanceStartDate, instanceEndDate);
                            d->m_itemTransformer.fillInCommonCComponentDetails(&eventOcc, coccurenceCandidate, false); // false = do not set ids

                            // change the parent local id, now it must be the generator event, not the occurrence
                            QString idString = QString::fromStdString(cevent->getId());
                            QOrganizerItemLocalId localId = idString.toUInt();
                            eventOcc.setParentLocalId(localId);

                            // insert occurrence to the result list in right position
                            insertOccurenceSortedByStartDate(&eventOcc, retn);
                        }
                    }
                }
            }

            // finally the result list might need to be resized
            if (maxCount > 0) {
                while (retn.size() > maxCount)
                    retn.removeLast();
            }
        }
    }
    else if (generator.type() == QOrganizerItemType::TypeTodo)
    {
        CTodo* ctodo = cal->getTodo(nativeId, calError);
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (ctodo && *error == QOrganizerItemManager::NoError)
        {
            // TODO
            // Note that in Maemo5 todos can't have occurrences
        }
    }
    else
    {
        // Other item types can't have occurrences
        *error = QOrganizerItemManager::BadArgumentError;
    }

    cleanupCal( cal );

    return retn;
}

QList<QOrganizerItemLocalId> QOrganizerItemMaemo5Engine::itemIds(const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, QOrganizerItemManager::Error *error) const
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
    QList<QOrganizerItem> retn;

    foreach(const QOrganizerItem& item, partiallyFilteredItems) {
        if (QOrganizerItemManagerEngine::testFilter(filter, item)) {
            retn.append(item);
        }
    }

    return QOrganizerItemManagerEngine::sortItems(retn, sortOrders);
}

void QOrganizerItemMaemo5Engine::removeAllForDebug() const
{
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();

    std::vector<CEvent*> events = cal->getEvents( calError );
    for ( int i = 0; i < events.size(); ++i )
    {
        CEvent* cevent = events[i];
        std::string itemId = cevent->getId();
        int calError = CALENDAR_OPERATION_SUCCESSFUL;
        cal->deleteComponent( itemId, calError );
        delete cevent;
        qDebug() << "Deleted event";
    }
    std::vector<CTodo*> todos = cal->getTodos( calError );
    for ( int i = 0; i < todos.size(); ++i )
    {
        CTodo* ctodo = todos[i];
        std::string itemId = ctodo->getId();
        int calError = CALENDAR_OPERATION_SUCCESSFUL;
        cal->deleteComponent( itemId, calError );
        delete ctodo;
        qDebug() << "Deleted todo";
    }
    std::vector<CJournal*> journals = cal->getJournals( calError );
    for ( int i = 0; i < journals.size(); ++i )
    {
        CJournal* cjournal = journals[i];
        std::string itemId = cjournal->getId();
        int calError = CALENDAR_OPERATION_SUCCESSFUL;
        cal->deleteComponent( itemId, calError );
        delete cjournal;
        qDebug() << "Deleted journal";
    }
}

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::items(const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const
{
    // TODO: This quick implementation is provided only to make the demo application to work
    // It doesn't support any filters, sort orders or fetch hints

    //removeAllForDebug();

    *error = QOrganizerItemManager::NoError;
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    QList<QOrganizerItem> retn;
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();

    std::vector<CEvent*> events = cal->getEvents(calError);
    for (int i = 0; i < events.size(); ++i)
    {
        CEvent* cevent = events[i];
        QOrganizerEvent event = d->m_itemTransformer.convertCEventToQEvent(cevent);
        d->m_itemTransformer.fillInCommonCComponentDetails(&event, cevent);
        delete cevent;
        retn.append(event);
    }

    std::vector<CTodo*> todos = cal->getTodos(calError);
    for (int i = 0; i < todos.size(); ++i)
    {
        CTodo* ctodo = todos[i];
        QOrganizerTodo todo = d->m_itemTransformer.convertCTodoToQTodo(ctodo);
        d->m_itemTransformer.fillInCommonCComponentDetails(&todo, ctodo);
        delete ctodo;
        retn.append(todo);
    }

    std::vector<CJournal*> journals = cal->getJournals(calError);
    for (int i = 0; i < journals.size(); ++i)
    {
        CJournal* cjournal = journals[i];
        QOrganizerJournal journal = d->m_itemTransformer.convertCJournalToQJournal(cjournal);
        d->m_itemTransformer.fillInCommonCComponentDetails(&journal, cjournal);
        delete cjournal;
        retn.append(journal);
    }

    cleanupCal(cal);
    return retn;

    /*
        TODO

        Given the supplied filter and sort order, fetch the list of items [not instances] that correspond, and return them.

        If you don't support the filter or sort orders, you can fetch a partially (or un-) filtered list and ask the helper
        functions to filter and sort it for you.

        The fetch hint suggests how much of the item to fetch.  You can ignore the fetch hint and fetch everything (but you must
        fetch at least what is mentioned in the fetch hint).
    */

    /*

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

    //return ret;
}

QOrganizerItem QOrganizerItemMaemo5Engine::item(const QOrganizerItemLocalId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const
{
    Q_UNUSED(fetchHint);
    // TODO: Make this method to use the fetch hint

    CCalendar *cal = d->m_mcInstance->getDefaultCalendar();
    std::string nativeId = QString::number(itemId).toStdString();
    int calError = CALENDAR_OPERATION_SUCCESSFUL;

/*
    // Temporary test code, does not belong here, remove it!!
    CEvent *cevent = new CEvent();
    cevent->setCalendarId(cal->getCalendarId());

    QDateTime t1(QDate(2010,6,30),QTime(9,0,0));
    QDateTime t2(QDate(2010,6,30),QTime(10,0,0));

    cevent->setDateStart(t1.toTime_t());
    cevent->setDateEnd(t2.toTime_t());
    cevent->setDescription(QString("Testi").toStdString());
    cevent->setGUid(QString("TESTIEVENTTI").toStdString());
    cevent->setRelated(QString("RELTYPE=CHILD;TESTIKAKSI").toStdString());

    cal->addEvent(cevent, calError);

    CEvent *cevent2 = new CEvent();
    cevent2->setCalendarId(cal->getCalendarId());

    QDateTime t3(QDate(2010,6,20),QTime(9,0,0));
    QDateTime t4(QDate(2010,6,20),QTime(10,0,0));

    cevent2->setDateStart(t3.toTime_t());
    cevent2->setDateEnd(t4.toTime_t());
    cevent2->setDescription(QString("Testi2").toStdString());
    cevent2->setGUid(QString("TESTIKAKSI").toStdString());
    cevent2->setRelated(QString("RELTYPE=PARENT;TESTIEVENTTI").toStdString());

    cal->addEvent(cevent2, calError);

    cal->deleteComponent(cevent->getId(), calError); //30
    qDebug() << QString::fromStdString(static_cast<CComponentDetails*>(cevent2)->toString());


    return QOrganizerItem();
*/

    CEvent *event = cal->getEvent(nativeId, calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (event) {
        QOrganizerEvent retn;
        QString type = QOrganizerItemType::TypeEvent;
        if (!isOccurrence(cal, event, type, error)) {
            retn = d->m_itemTransformer.convertCEventToQEvent(event);
            d->m_itemTransformer.fillInCommonCComponentDetails(&retn, event);
        }
        else {
            // item is actually an occurrence of an another event
            *error = QOrganizerItemManager::DoesNotExistError;
        }
        delete event;
        cleanupCal(cal);
        return retn;
    }
    CTodo *todo = cal->getTodo(nativeId, calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (todo) {
        QOrganizerTodo retn = d->m_itemTransformer.convertCTodoToQTodo(todo);
        d->m_itemTransformer.fillInCommonCComponentDetails(&retn, todo);
        delete todo;
        cleanupCal(cal);
        return retn;
    }
    CJournal *journal = cal->getJournal(nativeId, calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (journal) {
        QOrganizerJournal retn = d->m_itemTransformer.convertCJournalToQJournal(journal);
        d->m_itemTransformer.fillInCommonCComponentDetails(&retn, journal);
        delete journal;
        cleanupCal(cal);
        return retn;
    }

    // In an error situation return an invalid item
    cleanupCal(cal);
    *error = QOrganizerItemManager::DoesNotExistError;
    return QOrganizerItem();
}

bool QOrganizerItemMaemo5Engine::saveItems(QList<QOrganizerItem> *items, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error* error)
{
    // TODO: Add changeset manipulation and signal emissions

    if (!items) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    *error = QOrganizerItemManager::NoError;
    CCalendar *cal = d->m_mcInstance->getDefaultCalendar();
    bool success = true;
    QOrganizerItemChangeSet cs;

    for (int i = 0; i < items->size(); ++i) {
        QOrganizerItem curr = items->at(i);
        int calError = doSaveItem(cal, &curr, cs, error);

        if (calError != CALENDAR_OPERATION_SUCCESSFUL || *error != QOrganizerItemManager::NoError) {
            success = false;
            curr.setId(QOrganizerItemId()); // clear ID
            if (errorMap) {
                if (*error != QOrganizerItemManager::NoError)
                    errorMap->insert(i, *error);
                else
                    errorMap->insert(i, QOrganizerItemManager::UnspecifiedError);
            }
        }

        items->replace(i, curr);
    }

    d->m_mcInstance->commitAllChanges(); // ensure that changes are committed before emitting signals
    cs.emitSignals(this);
    cleanupCal(cal);
    return success;
}

bool QOrganizerItemMaemo5Engine::removeItems(const QList<QOrganizerItemLocalId> &itemIds, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error *error)
{
    // TODO: Check all the dependencies of the removed item

    *error = QOrganizerItemManager::NoError;
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();
    bool success = true;
    QOrganizerItemChangeSet cs;

    for (int i = 0; i < itemIds.size(); ++i) {
        QOrganizerItemLocalId currId = itemIds.at(i);

        QOrganizerItem currItem = item(currId, fetchMinimalData(), error);
        if (*error == QOrganizerItemManager::NoError) {
            // Item exists
            QString itemId = QString::number(currItem.localId());
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            cal->deleteComponent(itemId.toStdString(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);

            if (calError != CALENDAR_OPERATION_SUCCESSFUL)
            {
                success = false;
                if (errorMap)
                    errorMap->insert(i, *error);
            }
            else {
                // Success, update the changeset
                cs.insertRemovedItem(currItem.localId());
            }
        }
        else {
            success = false;
            if (errorMap)
                errorMap->insert(i, *error);
        }
    }

    d->m_mcInstance->commitAllChanges(); // ensure that changes are committed before emitting signals
    cs.emitSignals(this);
    cleanupCal(cal);
    return success;
}

bool QOrganizerItemMaemo5Engine::startRequest(QOrganizerItemAbstractRequest* req)
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

bool QOrganizerItemMaemo5Engine::cancelRequest(QOrganizerItemAbstractRequest* req)
{
    /*
        TODO

        Cancel an in progress async request.  If not possible, return false from here.
    */
    return QOrganizerItemManagerEngine::cancelRequest(req);
}

bool QOrganizerItemMaemo5Engine::waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs)
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

void QOrganizerItemMaemo5Engine::requestDestroyed(QOrganizerItemAbstractRequest* req)
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

bool QOrganizerItemMaemo5Engine::hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString &itemType) const
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

bool QOrganizerItemMaemo5Engine::isFilterSupported(const QOrganizerItemFilter &filter) const
{
    // TODO if you engine can natively support the filter, return true.  Otherwise you should emulate support in the item{Ids} functions.
    Q_UNUSED(filter);
    return false;
}

QList<QVariant::Type> QOrganizerItemMaemo5Engine::supportedDataTypes() const
{
    QList<QVariant::Type> retn;
    // TODO - tweak which data types this engine understands
    retn << QVariant::String;
    retn << QVariant::Date;
    retn << QVariant::DateTime;
    retn << QVariant::Time;

    return retn;
}

QStringList QOrganizerItemMaemo5Engine::supportedItemTypes() const
{
    // TODO - return which [predefined] types this engine supports
    QStringList retn;

    retn << QOrganizerItemType::TypeEvent;
    retn << QOrganizerItemType::TypeEventOccurrence;
    retn << QOrganizerItemType::TypeJournal;
    retn << QOrganizerItemType::TypeNote;
    retn << QOrganizerItemType::TypeTodo;
    retn << QOrganizerItemType::TypeTodoOccurrence;

    return retn;
}

void QOrganizerItemMaemo5Engine::checkItemIdValidity(QOrganizerItem *checkItem, QOrganizerItemManager::Error *error)
{
    *error = QOrganizerItemManager::NoError;

    // Check local id
    if (checkItem->localId()) {
        // Don't allow saving with local id defined unless the item is from this manager.
        if (checkItem->id().managerUri() != managerUri()) {
            *error = QOrganizerItemManager::BadArgumentError;
            return;
        }
    }

    QOrganizerEventOccurrence *eventOccurrence(0);
    if (checkItem->type() == QOrganizerItemType::TypeEventOccurrence)
        eventOccurrence = static_cast<QOrganizerEventOccurrence *>(checkItem);
    QOrganizerTodoOccurrence *todoOccurrence(0);
    if (checkItem->type() == QOrganizerItemType::TypeTodoOccurrence)
        todoOccurrence = static_cast<QOrganizerTodoOccurrence *>(checkItem);

    // If the item is not an occurrence, no more checks are needed
    if (!eventOccurrence && !todoOccurrence)
        return;

    // Event occurrence validity checks:
    if (eventOccurrence) {
        // Either parent id or GUID (or both) must be set
        if (!eventOccurrence->parentLocalId() && eventOccurrence->guid().isEmpty()) {
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return;
        }

        // If the parent ID is set, it must point to an event
        QOrganizerItem parent;
        if (eventOccurrence->parentLocalId()) {
            parent = item(eventOccurrence->parentLocalId(), fetchMinimalData(), error);
            if (*error != QOrganizerItemManager::NoError)
                return;
            if (parent.type() != QOrganizerItemType::TypeEvent) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }
        else if(d->m_guidMapper.itemIds(eventOccurrence->guid(), QOrganizerItemType::TypeEvent).isEmpty()) {
            // The parent ID was not set, so the GUID is always set here.
            // Must find at least one event with the given GUID.
            // If not, then the occurrence is not valid.
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return;
        }

        // If both parent ID and GUID are both set, they must be consistent
        if (eventOccurrence->parentLocalId() && !eventOccurrence->guid().isEmpty()) {
            // Assumes that the parent item is fetched earlier
            if (eventOccurrence->guid() != parent.guid()) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }
    }

    // Todo occurrence validity checks:
    if (todoOccurrence) {
        // Either parent id or GUID (or both) must be set
        if (!todoOccurrence->parentLocalId() && todoOccurrence->guid().isEmpty()) {
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return;
        }

        // Parent ID must point to a todo
        QOrganizerItem parent;
        if (todoOccurrence->parentLocalId()) {
            parent = item(todoOccurrence->parentLocalId(), fetchMinimalData(), error);
            if (*error != QOrganizerItemManager::NoError)
                return;
            if (parent.type() != QOrganizerItemType::TypeTodo) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }
        else if(d->m_guidMapper.itemIds(todoOccurrence->guid(), QOrganizerItemType::TypeTodo).isEmpty()) {
            // The parent ID was not set, so the GUID is always set here.
            // Must find at least one todo with the given GUID.
            // If not, then the occurrence is not valid.
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return;
        }

        // If both parent ID and GUID are set, they must be consistent
        if (todoOccurrence->parentLocalId() && !todoOccurrence->guid().isEmpty()) {
            // Assumes that the parent item is fetched earlier
            if (todoOccurrence->guid() != parent.guid()) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }
    }
}

int QOrganizerItemMaemo5Engine::doSaveItem(CCalendar *cal, QOrganizerItem *item, QOrganizerItemChangeSet &cs, QOrganizerItemManager::Error *error)
{
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    *error = QOrganizerItemManager::NoError;

    // Check item validity
    checkItemIdValidity(item, error);
    if (*error != QOrganizerItemManager::NoError)
        return calError;

    // Returns InvalidItemTypeError if the type won't be recognized later
    *error = QOrganizerItemManager::InvalidItemTypeError;

    CComponent *component = d->m_itemTransformer.createCComponent(cal, item);
    if (!component)
        return calError;

    // TODO: The custom detail fields should be iterated and the corresponding
    // fields should be set to CComponent for saving.

    if (item->type() == QOrganizerItemType::TypeEvent) {
        CEvent* cevent = static_cast<CEvent *>(component);
        QString ceventId = QString::fromStdString(cevent->getId());
        if (!ceventId.isEmpty()) {
            // CEvent ID is not empty, the event already exists in calendar
            cal->modifyEvent(cevent, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError)
                cs.insertChangedItem(item->localId());
        }
        else {
            // CEvent ID is empty, the event is new
            cal->addEvent(cevent, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (calError == CALENDAR_OPERATION_SUCCESSFUL || calError == CALENDAR_ENTRY_DUPLICATED)
                // The Maemo5 calendar does not accept two items if all the
                // time details are equal. That's so even if the item IDs differ.
                // If the "new" item is actually a duplicate, then let
                // the calendar to modify the existing item.
                // TODO: Is that what we want?
            {
                // Set id for the event
                QString newIdString = QString::fromStdString(cevent->getId());
                QOrganizerItemLocalId newId = newIdString.toUInt();
                QOrganizerItemId id;
                id.setLocalId(newId);
                id.setManagerUri(managerUri());
                item->setId(id);

                // Update changeset
                if (calError == CALENDAR_ENTRY_DUPLICATED)
                    cs.insertChangedItem(item->localId());
                else
                    cs.insertAddedItem(item->localId());

                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error
            }
        }

        // Update GUID mapping
        d->m_guidMapper.setCalendar(cal);
        d->m_guidMapper.addMapping(item->guid(),QString::fromStdString(cevent->getId()), QOrganizerItemType::TypeEvent);

        delete cevent;
        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeEventOccurrence)
    {
        QOrganizerEventOccurrence *eventOccurrence = static_cast<QOrganizerEventOccurrence *>(item);

        qDebug() << "Occurrence start date: " << eventOccurrence->startDateTime();
        qDebug() << "Occurrence end date: " << eventOccurrence->endDateTime();
        qDebug() << "Occurrence original date: " << eventOccurrence->originalDate();
        qDebug() << "Occurrence disp label: " << eventOccurrence->displayLabel();

        QOrganizerItem parentItem = parentOf(eventOccurrence, error);
        if (*error == QOrganizerItemManager::NoError) {

            qDebug() << "Parent is " << parentItem.localId();

            if (parentItem.localId()) {
                // parent item found, cast it to event
                QOrganizerEvent *parentEvent = static_cast<QOrganizerEvent *>(&parentItem);
                qDebug() << "Parent start date: " << parentEvent->startDateTime();
                qDebug() << "Parent end date: " << parentEvent->endDateTime();
                qDebug() << "Parent disp label: " << parentEvent->displayLabel();

                // save the event occurrence
                //calError = saveEventOccurrence(eventOccurrence, parentEvent, error);
            }
            else {
                // Event occurrence without a parent item fails,
                // but this should not happen after passing the validity check...
                *error = QOrganizerItemManager::InvalidOccurrenceError;
            }
        }
    }
    else if (item->type() == QOrganizerItemType::TypeTodo) {
        CTodo* ctodo = static_cast<CTodo *>(component);
        QString ctodoId = QString::fromStdString(ctodo->getId());
        if (!ctodoId.isEmpty()) {
            // CTodo ID is not empty, the todo already exists in calendar
            cal->modifyTodo(ctodo, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError)
                cs.insertChangedItem(item->localId());
        }
        else {
            // CTodo ID is empty, the todo is new
            cal->addTodo(ctodo, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (calError == CALENDAR_OPERATION_SUCCESSFUL || calError == CALENDAR_ENTRY_DUPLICATED)
                // The Maemo5 calendar does not accept two items if all the
                // time details are equal. That's so even if the item IDs differ.
                // If the "new" item is actually a duplicate, then let
                // the calendar to modify the existing item.
                // TODO: Is that what we want?
            {
                // Set id for the todo
                QString newIdString = QString::fromStdString(ctodo->getId());
                QOrganizerItemLocalId newId = newIdString.toUInt();
                QOrganizerItemId id;
                id.setLocalId(newId);
                id.setManagerUri(managerUri());
                item->setId(id);

                // Update changeset
                if (calError == CALENDAR_ENTRY_DUPLICATED)
                    cs.insertChangedItem(item->localId());
                else
                    cs.insertAddedItem(item->localId());

                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error
            }
        }

        // Update GUID mapping
        d->m_guidMapper.setCalendar(cal);
        d->m_guidMapper.addMapping(item->guid(),QString::fromStdString(ctodo->getId()), QOrganizerItemType::TypeTodo);

        delete ctodo;
        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeTodoOccurrence)
    {
        // TODO
    }
    else if (item->type() == QOrganizerItemType::TypeJournal) {
        CJournal* cjournal = static_cast<CJournal *>(component);
        QString cjournalId = QString::fromStdString(cjournal->getId());
        if (!cjournalId.isEmpty()) {
            // CJournal ID is not empty, the journal already exists in calendar
            cal->modifyJournal(cjournal, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError)
                cs.insertChangedItem(item->localId());
        }
        else {
            // CJournal ID is empty, the journal is new
            cal->addJournal(cjournal, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (calError == CALENDAR_OPERATION_SUCCESSFUL || calError == CALENDAR_ENTRY_DUPLICATED)
                // The Maemo5 calendar does not accept two items if all the
                // time details are equal. That's so even if the item IDs differ.
                // If the "new" item is actually a duplicate, then let
                // the calendar to modify the existing item.
                // TODO: Is that what we want?
            {
                // Set id for the journal
                QString newIdString = QString::fromStdString(cjournal->getId());
                QOrganizerItemLocalId newId = newIdString.toUInt();
                QOrganizerItemId id;
                id.setLocalId(newId);
                id.setManagerUri(managerUri());
                item->setId(id);

                // Update changeset
                if (calError == CALENDAR_ENTRY_DUPLICATED)
                    cs.insertChangedItem(item->localId());
                else
                    cs.insertAddedItem(item->localId());

                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error
            }
        }

        // Update GUID mapping
        d->m_guidMapper.setCalendar(cal);
        d->m_guidMapper.addMapping(item->guid(),QString::fromStdString(cjournal->getId()), QOrganizerItemType::TypeJournal);

        delete cjournal;
        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeNote) {
        // TODO
    }

    return calError;
}

int QOrganizerItemMaemo5Engine::saveEventOccurrence(QOrganizerEventOccurrence *occurrence, QOrganizerEvent *parent, QOrganizerItemManager::Error *error)
{
    // set occurrence GUID equal to the parent GUID
    occurrence->setGuid(parent->guid());

    if (!parent->recurrenceRules().isEmpty() || !parent->recurrenceDates().isEmpty()) {
        // add dates that the occurrence covers as exception dates to the parent item
        QDate occurrenceStart = occurrence->startDateTime().date();
        QDate occurrenceEnd = occurrence->endDateTime().date();

        QDate exceptionDate = occurrenceStart;
        while (exceptionDate <= occurrenceEnd) {
            if (parent->exceptionDates().indexOf(exceptionDate) == -1)
                parent->exceptionDates() << exceptionDate;
            exceptionDate = exceptionDate.addDays(1);
        }

        // then set a new exception rule for parent

        // TODO...
    }

}

void QOrganizerItemMaemo5Engine::insertOccurenceSortedByStartDate(QOrganizerItem *occurrence, QList<QOrganizerItem> &target) const
{
    if (occurrence->type() == QOrganizerItemType::TypeEventOccurrence)
    {
        QOrganizerEventOccurrence *occ = static_cast<QOrganizerEventOccurrence *>(occurrence);
        int index = 0;
        while (index < target.count()) {
            QOrganizerEventOccurrence curr = static_cast<QOrganizerEventOccurrence>(target[index]);
            if (curr.startDateTime() > occ->startDateTime())
                break;
            ++index;
        }
        target.insert(index, *occ);
    }
    else if (occurrence->type() == QOrganizerItemType::TypeTodoOccurrence)
    {
        QOrganizerTodoOccurrence *occ = static_cast<QOrganizerTodoOccurrence *>(occurrence);
        int index = 0;
        while (index < target.count()) {
            QOrganizerTodoOccurrence curr = static_cast<QOrganizerTodoOccurrence>(target[index]);
            if (curr.startDateTime() > occ->startDateTime())
                break;
            ++index;
        }
        target.insert(index, *occ);
    }
}

QOrganizerItem QOrganizerItemMaemo5Engine::parentOf(QOrganizerItem *occurrence, QOrganizerItemManager::Error *error)
{
    // the occurrence is supposed be valid when this method becomes called
    QOrganizerItemLocalId parentId;
    if (occurrence->type() == QOrganizerItemType::TypeEventOccurrence) {
        QOrganizerEventOccurrence *eventOccurrence = static_cast<QOrganizerEventOccurrence *>(occurrence);
        if (eventOccurrence->parentLocalId()) {
            // the simple case: parent local id was set
            parentId = eventOccurrence->parentLocalId();
        }
        else {
            // fetch with [GUID,originalDate]
            QList<QString> ids = d->m_guidMapper.itemIds(eventOccurrence->guid(), QOrganizerItemType::TypeEvent);
            foreach (QString id, ids) {
                QOrganizerItemLocalId parentLocalId = id.toUInt();
                QOrganizerItem parentCandidate = item(parentLocalId, fetchMinimalData(), error);
                if (*error != QOrganizerItemManager::NoError)
                    return QOrganizerItem(); // error occured

                // parent candidate must be an event here as events were requested from the GUID mapper
                QOrganizerEvent *parentCandidateEvent = static_cast<QOrganizerEvent *>(&parentCandidate);

                // Event must have some recurrence information, otherwise it can't be a parent
                if (!parentCandidateEvent->recurrenceDates().isEmpty()
                    || !parentCandidateEvent->recurrenceRules().isEmpty()) {
                    // Now get the instances (occurrences) of the possible parent event. If there's a match
                    // to our occurrence's originalDate, then we have found the parent.
                    QDateTime periodStart = QDateTime(eventOccurrence->originalDate(), QTime(0,0,0));
                    QDateTime periodEnd = QDateTime(eventOccurrence->originalDate(), QTime(23,59,59,999));

                    QList<QOrganizerItem> parentCandidateOccurrences =
                            itemInstances(*parentCandidateEvent, periodStart, periodEnd, 0, error);
                    if (*error != QOrganizerItemManager::NoError)
                        return QOrganizerItem(); // error occured

                    // Because the period was set, it's enough to just check if we got any
                    if (!parentCandidateOccurrences.isEmpty()) {
                        parentId = parentLocalId;
                        break; // parent event found
                    }
                }
            }
        }
    }
    else if (occurrence->type() == QOrganizerItemType::TypeTodoOccurrence) {

        // TODO: This is not completed yet, make it much like the event case...

        QOrganizerTodoOccurrence *todoOccurrence = static_cast<QOrganizerTodoOccurrence *>(occurrence);
        if (todoOccurrence->parentLocalId()) {
            // the simple case: parent local id was set
            parentId = todoOccurrence->parentLocalId();
        }
        else {
            // fetch with [GUID,originalDate]
            QList<QString> ids = d->m_guidMapper.itemIds(todoOccurrence->guid(), QOrganizerItemType::TypeTodo);
            foreach (QString id, ids) {
                QOrganizerItemLocalId parentLocalId = id.toUInt();
                QOrganizerItem parentCandidate = item(parentLocalId, fetchMinimalData(), error);
                if (*error != QOrganizerItemManager::NoError)
                    return QOrganizerItem(); // error occured
                // parent must be a todo here as todos were requested from the GUID mapper
                QOrganizerTodo *parentCandidateTodo = static_cast<QOrganizerTodo *>(&parentCandidate);

                if (parentCandidateTodo->startDateTime().date() == todoOccurrence->originalDate()) {
                    parentId = parentLocalId;
                    break; // parent todo found
                }
            }
        }
    }
    else {
        return QOrganizerItem(); // other types can't have a parent
    }

    // Parent id got, now fetch & return the parent
    return item(parentId, fetchMinimalData(), error);
}

bool QOrganizerItemMaemo5Engine::isOccurrence(CCalendar *cal, CComponent *ccomponent, QString typeStr, QOrganizerItemManager::Error *error) const
{
    // checks if a native event or todo is actually an occurrence

    *error = QOrganizerItemManager::NoError;
    QString guid = QString::fromStdString(ccomponent->getGUid());
    if (guid.isEmpty())
        return false; // items with no GUID are never considered as occurrences

    if (typeStr == QOrganizerItemType::TypeEvent || typeStr == QOrganizerItemType::TypeTodo) {
        // if item contains recurrence information, it can't be an occurrence
        if (containsRecurrenceInformation(ccomponent))
            return false;

        // if no duplicate items with GUID are found, item can't be an occurrence
        d->m_guidMapper.setCalendar(cal);
        QList<QString> itemsWithGuid = d->m_guidMapper.itemIds(guid, typeStr);
        if (itemsWithGuid.count() < 2)
            return false;

        // Items with duplicate GUID are found and this item does not contain recurrence
        // information. Now check if any of the items with the GUID contains recurrence
        // information. If there's at least one such, then this item clearly is an occurrence.
        int calError = CALENDAR_OPERATION_SUCCESSFUL;
        QString ccomponentId = QString::fromStdString(ccomponent->getId());
        foreach (QString itemId, itemsWithGuid) {
            if (itemId != ccomponentId) {
                CComponent *ccalItem = 0;
                if (typeStr == QOrganizerItemType::TypeEvent)
                    ccalItem = cal->getEvent(itemId.toStdString(), calError);
                else
                    ccalItem = cal->getTodo(itemId.toStdString(), calError);

                if (calError == CALENDAR_OPERATION_SUCCESSFUL)
                {
                    if (containsRecurrenceInformation(ccalItem)) {
                        delete ccalItem;
                        ccalItem = 0;
                        return true;
                    }
                    else {
                        delete ccalItem;
                        ccalItem = 0;
                    }
                }
                else {
                    *error = d->m_itemTransformer.calErrorToManagerError(calError);
                }
           }
        }

        // None of items contains recurrence information. There are items
        // with multiple GUID in calendar database for some reason.
        return false;
    }
    else if (typeStr == QOrganizerItemType::TypeJournal || typeStr == QOrganizerItemType::TypeNote) {
        return false; // these types have no occurrences
    }
    else {
        return true; // an occurrence type was given
    }
}

bool QOrganizerItemMaemo5Engine::containsRecurrenceInformation(CComponent *ccomponent) const
{
    CRecurrence *crecurrence = ccomponent->getRecurrence();
    if (crecurrence) {
        return (crecurrence->getRecurrenceRule().size() > 0
                || crecurrence->getRDays().size() > 0
                || crecurrence->getEDays().size() > 0);
    }
    else {
        return false;
    }
}

QOrganizerItemFetchHint QOrganizerItemMaemo5Engine::fetchMinimalData() const
{
    QOrganizerItemFetchHint::OptimizationHints optimizationHints(QOrganizerItemFetchHint::NoBinaryBlobs);
    optimizationHints |= QOrganizerItemFetchHint::NoActionPreferences;
    QOrganizerItemFetchHint fetchHints;
    fetchHints.setOptimizationHints(optimizationHints);
    return fetchHints;
}

void QOrganizerItemMaemo5Engine::cleanupCal(CCalendar *cal) const
{
    if (cal != d->m_mcInstance->getDefaultCalendar())
        delete cal; // The default calendar should not be deleted
}
