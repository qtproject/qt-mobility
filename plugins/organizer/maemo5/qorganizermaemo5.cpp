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
        *error = calErrorToManagerError(calError);
        if (cevent && *error == QOrganizerItemManager::NoError)
        {
            // Get event instance times
            std::vector< std::time_t > eventInstanceDates;
            cevent->generateInstanceTimes(periodStart.toTime_t(), periodEnd.toTime_t(), eventInstanceDates);

            // If maxCount is given, resize to maxCount
            if (maxCount > 0 && maxCount < eventInstanceDates.size())
                eventInstanceDates.resize(maxCount);

            // Generate the event occurrences
            std::vector< std::time_t >::const_iterator i;
            for (i = eventInstanceDates.begin(); i != eventInstanceDates.end(); ++i)
            {
                QDateTime instanceDate = QDateTime::fromTime_t(*i);
                QOrganizerEventOccurrence eventOcc = convertCEventToQEventOccurrence(cevent, instanceDate);
                fillInCommonCComponentDetails(&eventOcc, cevent, false); // false = do not set ids
                retn << eventOcc;
            }
        }
    }
    else if (generator.type() == QOrganizerItemType::TypeTodo)
    {
        CTodo* ctodo = cal->getTodo(nativeId, calError);
        *error = calErrorToManagerError(calError);
        if (ctodo && *error == QOrganizerItemManager::NoError)
        {
            // TODO
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
        QOrganizerEvent event = convertCEventToQEvent(cevent);
        fillInCommonCComponentDetails(&event, cevent);
        delete cevent;
        retn.append(event);
    }

    std::vector<CTodo*> todos = cal->getTodos(calError);
    for (int i = 0; i < todos.size(); ++i)
    {
        CTodo* ctodo = todos[i];
        QOrganizerTodo todo = convertCTodoToQTodo(ctodo);
        fillInCommonCComponentDetails(&todo, ctodo);
        delete ctodo;
        retn.append(todo);
    }

    std::vector<CJournal*> journals = cal->getJournals(calError);
    for (int i = 0; i < journals.size(); ++i)
    {
        CJournal* cjournal = journals[i];
        QOrganizerJournal journal = convertCJournalToQJournal(cjournal);
        fillInCommonCComponentDetails(&journal, cjournal);
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

    CEvent *event = cal->getEvent(nativeId, calError);
    *error = calErrorToManagerError(calError);
    if (event) {
        QOrganizerEvent retn = convertCEventToQEvent(event);
        fillInCommonCComponentDetails(&retn, event);
        delete event;
        cleanupCal(cal);
        return retn;
    }
    CTodo *todo = cal->getTodo(nativeId, calError);
    *error = calErrorToManagerError(calError);
    if (todo) {
        QOrganizerTodo retn = convertCTodoToQTodo(todo);
        fillInCommonCComponentDetails(&retn, todo);
        delete todo;
        cleanupCal(cal);
        return retn;
    }
    CJournal *journal = cal->getJournal(nativeId, calError);
    *error = calErrorToManagerError(calError);
    if (journal) {
        QOrganizerJournal retn = convertCJournalToQJournal(journal);
        fillInCommonCComponentDetails(&retn, journal);
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

    for (int i = 0; i < items->size(); ++i) {
        QOrganizerItem curr = items->at(i);
        int calError = doSaveItem(cal, &curr, error);

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

    cleanupCal(cal);
    return success;
}

bool QOrganizerItemMaemo5Engine::removeItems(const QList<QOrganizerItemLocalId> &itemIds, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error *error)
{
    // TODO: Add changeset manipulation and signal emissions
    // TODO: Check all the dependencies of the removed item

    *error = QOrganizerItemManager::NoError;
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();
    bool success = true;

    for (int i = 0; i < itemIds.size(); ++i) {
        QOrganizerItemLocalId currId = itemIds.at(i);

        QOrganizerItemFetchHint::OptimizationHints optimizationHints(QOrganizerItemFetchHint::NoBinaryBlobs);
        optimizationHints |= QOrganizerItemFetchHint::NoActionPreferences;
        QOrganizerItemFetchHint fetchHints;
        fetchHints.setOptimizationHints(optimizationHints);
        QOrganizerItem currItem = item(currId, fetchHints, error);
        if (*error == QOrganizerItemManager::NoError) {
            // Item exists
            QString itemId = QString::number(currItem.localId());
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            cal->deleteComponent(itemId.toStdString(), calError);
            *error = calErrorToManagerError(calError);

            if (calError != CALENDAR_OPERATION_SUCCESSFUL)
            {
                success = false;
                if (errorMap)
                    errorMap->insert(i, *error);
            }
        } else {
            success = false;
            if (errorMap)
                errorMap->insert(i, *error);
        }
    }

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

int QOrganizerItemMaemo5Engine::doSaveItem(CCalendar *cal, QOrganizerItem *item, QOrganizerItemManager::Error *error)
{
    // TODO: Add signal emissions
    // TODO: Or might be better not to implement emissions here?

    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    *error = QOrganizerItemManager::InvalidItemTypeError;

    CComponent *component = createCComponent(cal, item);
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
            *error = calErrorToManagerError(calError);
        }
        else {
            // CEvent ID is empty, the event is new
            cal->addEvent(cevent, calError);
            *error = calErrorToManagerError(calError);
            if (calError == CALENDAR_OPERATION_SUCCESSFUL || calError == CALENDAR_ENTRY_DUPLICATED)
                // The Maemo5 calendar does not accept two items if all the
                // time details are equal. That's so even if the item IDs differ.
                // If the "new" item is actually a duplicate, then let
                // the calendar to modify the existing item.
                // TODO: Is that what we want?
            {
                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error

                QString newIdString = QString::fromStdString(cevent->getId());
                QOrganizerItemLocalId newId = newIdString.toUInt();
                QOrganizerItemId id;
                id.setLocalId(newId);
                id.setManagerUri(managerUri());
                item->setId(id);
            }
        }
        delete cevent;
        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeTodo) {
        CTodo* ctodo = static_cast<CTodo *>(component);
        QString ctodoId = QString::fromStdString(ctodo->getId());
        if (!ctodoId.isEmpty()) {
            // CTodo ID is not empty, the todo already exists in calendar
            cal->modifyTodo(ctodo, calError);
            *error = calErrorToManagerError(calError);
        }
        else {
            // CTodo ID is empty, the todo is new
            cal->addTodo(ctodo, calError);
            *error = calErrorToManagerError(calError);
            if (calError == CALENDAR_OPERATION_SUCCESSFUL || calError == CALENDAR_ENTRY_DUPLICATED)
                // The Maemo5 calendar does not accept two items if all the
                // time details are equal. That's so even if the item IDs differ.
                // If the "new" item is actually a duplicate, then let
                // the calendar to modify the existing item.
                // TODO: Is that what we want?
            {
                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error

                QString newIdString = QString::fromStdString(ctodo->getId());
                QOrganizerItemLocalId newId = newIdString.toUInt();
                QOrganizerItemId id;
                id.setLocalId(newId);
                id.setManagerUri(managerUri());
                item->setId(id);
            }
        }
        delete ctodo;
        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeJournal) {
        CJournal* cjournal = static_cast<CJournal *>(component);
        QString cjournalId = QString::fromStdString(cjournal->getId());
        if (!cjournalId.isEmpty()) {
            // CJournal ID is not empty, the journal already exists in calendar
            cal->modifyJournal(cjournal, calError);
            *error = calErrorToManagerError(calError);
        }
        else {
            // CJournal ID is empty, the journal is new
            cal->addJournal(cjournal, calError);
            *error = calErrorToManagerError(calError);
            if (calError == CALENDAR_OPERATION_SUCCESSFUL || calError == CALENDAR_ENTRY_DUPLICATED)
                // The Maemo5 calendar does not accept two items if all the
                // time details are equal. That's so even if the item IDs differ.
                // If the "new" item is actually a duplicate, then let
                // the calendar to modify the existing item.
                // TODO: Is that what we want?
            {
                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error

                QString newIdString = QString::fromStdString(cjournal->getId());
                QOrganizerItemLocalId newId = newIdString.toUInt();
                QOrganizerItemId id;
                id.setLocalId(newId);
                id.setManagerUri(managerUri());
                item->setId(id);
            }

        }
        delete cjournal;
        return calError;
    }

    return calError;
}

QOrganizerEvent QOrganizerItemMaemo5Engine::convertCEventToQEvent(CEvent *cevent) const
{
    QOrganizerEvent retn;

    // Priority
    int tempint = cevent->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint)); // assume that the saved priority is vCal compliant.

    // Location geo coordinates
    QString tempstr = QString::fromStdString(cevent->getGeo());
    if (!tempstr.isNull())
        retn.setLocationGeoCoordinates(tempstr);

    // Start time
    QDateTime tempdt = QDateTime::fromTime_t(cevent->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);

    // End time
    tempdt = QDateTime::fromTime_t(cevent->getDateEnd());
    if (!tempdt.isNull())
        retn.setEndDateTime(tempdt);

    // Recurrence information
    d->m_recTransformer.transformToQrecurrence(cevent->getRecurrence());
    retn.setRecurrenceRules(d->m_recTransformer.recurrenceRules());
    retn.setExceptionRules(d->m_recTransformer.exceptionRules());

    return retn;
}

QOrganizerEventOccurrence QOrganizerItemMaemo5Engine::convertCEventToQEventOccurrence(CEvent* cevent, const QDateTime& instanceDate) const
{
    QOrganizerEventOccurrence retn;

    // Priority
    int tempint = cevent->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint)); // assume that the saved priority is vCal compliant.

    // Location geo coordinates
    QString tempstr = QString::fromStdString(cevent->getGeo());
    if (!tempstr.isNull())
        retn.setLocationGeoCoordinates(tempstr);

    // Start time
    QDateTime tempdt = QDateTime::fromTime_t(cevent->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);

    // End time
    tempdt = QDateTime::fromTime_t(cevent->getDateEnd());
    if (!tempdt.isNull())
        retn.setEndDateTime(tempdt);

    // Set parent id
    QString idString = QString::fromStdString( cevent->getId() );
    QOrganizerItemLocalId localId = idString.toUInt();
    retn.setParentLocalId(localId);

    // Set original event date
    retn.setOriginalDate( instanceDate.date() );

    return retn;
}

QOrganizerTodo QOrganizerItemMaemo5Engine::convertCTodoToQTodo(CTodo *ctodo) const
{
    QOrganizerTodo retn;

    // Priority
    int tempint = ctodo->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint));

    // Date start
    QDateTime tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);

    // Due
    tempdt = QDateTime::fromTime_t(ctodo->getDue());
    if (!tempdt.isNull())
        retn.setDueDateTime(tempdt);

    tempdt = QDateTime::fromTime_t(ctodo->getCompleted());
    if (!tempdt.isNull())
        retn.setFinishedDateTime(tempdt);
    tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);

    // TODO: How all the time fields of todos should be mapped????

    // Percent complete
    tempint = ctodo->getPercentComplete();
    if (tempint != -1)
        retn.setProgressPercentage(tempint);

    // Status
    retn.setStatus(static_cast<QOrganizerTodoProgress::Status>(ctodo->getStatus()));

    return retn;
}

QOrganizerTodoOccurrence QOrganizerItemMaemo5Engine::convertCTodoToQTodoOccurrence(CTodo *ctodo, const QString &calendarName) const
{
    // TODO: The code in this method is from the Proof of Concept
    // TODO: Replace the Proof of Concept codes at some point

    QOrganizerTodoOccurrence retn;
    int tempint = ctodo->getPriority();
    if (tempint != -1)
        retn.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint));
    QDateTime tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);
    QString tempstr = QString::fromStdString(ctodo->getSummary());
    if (!tempstr.isEmpty())
        retn.setDisplayLabel(tempstr);
    tempdt = QDateTime::fromTime_t(ctodo->getDue());
    if (!tempdt.isNull())
        retn.setDueDateTime(tempdt);
    tempint = ctodo->getPercentComplete();
    if (tempint != -1)
        retn.setProgressPercentage(tempint);
    tempdt = QDateTime::fromTime_t(ctodo->getCompleted());
    if (!tempdt.isNull())
        retn.setFinishedDateTime(tempdt);
    tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        retn.setStartDateTime(tempdt);
    
    // status is always available..
    // TODO: Commented out, check this!
    //ret.setStatus(static_cast<QOrganizerItemTodoProgress::Status>(ctodo->getStatus()));
    
    // now, in maemo, the parent id is the same as this id (todo's only have one occurrence).

    // TODO: These were removed from API, check if these are still needed?
    //ret.setParentItemId(rId);
    //ret.setOriginalDateTime(QDateTime::fromTime_t(ctodo->getDue())); // XXX TODO: verify this is the correct field to use as the instance date...

    return retn;
}

QOrganizerJournal QOrganizerItemMaemo5Engine::convertCJournalToQJournal(CJournal *cjournal) const
{
    QOrganizerJournal retn;

    // TODO: What else should be set for a journal?

    // Start time
    QDateTime tempdt = QDateTime::fromTime_t(cjournal->getDateStart());
    if (!tempdt.isNull())
        retn.setDateTime( tempdt );

    return retn;
}

void QOrganizerItemMaemo5Engine::fillInCommonCComponentDetails(QOrganizerItem *item, CComponent *component, bool setId) const
{
    // TODO: Possible fields to add (defined in the CComponent superclass):
    // TODO: flags, status, (datestart), (dateend), lastmodified, (comments)
    // TODO: recurrence, until, rtype, alarm, allday, tzoffset... still others??

    // TODO: What to do with attendee, instanceorigin, participation, priority, provenance
    // TODO: rsvpinfo, timestamp... others??

    if (item) {
        // Summary
        QString tempstr = QString::fromStdString(component->getSummary());
        if (!tempstr.isEmpty())
            item->setDisplayLabel(tempstr);

        // Description
        tempstr = QString::fromStdString(component->getDescription());
        if (!tempstr.isEmpty())
            item->setDescription(tempstr);

        // Location
        tempstr = QString::fromStdString(component->getLocation());
        if(!tempstr.isEmpty()) {
            QOrganizerItemLocation il = item->detail<QOrganizerItemLocation>();
            il.setLocationName( tempstr );
            item->saveDetail(&il);
        }

        // GUid
        tempstr = QString::fromStdString(component->getGUid());
        if(!tempstr.isEmpty()) {
            QOrganizerItemGuid ig = item->detail<QOrganizerItemGuid>();
            ig.setGuid( tempstr );
            item->saveDetail(&ig);
        }

        // Set component ID
        QOrganizerItemId id;
        id.setManagerUri(managerUri());
        if (setId) {
            QString idString = QString::fromStdString(component->getId());
            QOrganizerItemLocalId localId = idString.toUInt();
            id.setLocalId(localId);
        }
        else {
            id.setLocalId(QOrganizerItemLocalId()); // no local id
        }
        item->setId(id);
    }
}

CComponent* QOrganizerItemMaemo5Engine::createCComponent(CCalendar *cal, const QOrganizerItem *item) const
{
    QOrganizerItemLocalId itemId = item->localId();
    QString itemIdStr = QString::number(itemId);
    int calId = cal->getCalendarId();
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    CComponent *retn = 0; // Return null on errors

    if (item->type() == QOrganizerItemType::TypeEvent) {
        CEvent *cevent = cal->getEvent(itemIdStr.toStdString(), calError);
        if (!cevent) {
            // Event did not existed in calendar, create a new CEvent with an empty ID
            cevent = new CEvent();
        }
        else {
            // Event existed in calendar, use the existing event's ID
            cevent->setId(itemIdStr.toStdString());
        }
        cevent->setCalendarId(calId);

        // Set the event specific details:
        const QOrganizerEvent *event = static_cast<const QOrganizerEvent *>(item);

        // Location geo coordinates
        if (!event->locationGeoCoordinates().isEmpty())
            cevent->setGeo(event->locationGeoCoordinates().toStdString());

        // Priority
        if (!event->detail("QOrganizerItemPriority::DefinitionName").isEmpty())
            cevent->setPriority(static_cast<int>(event->priority()));

        // Start date
        if (!event->startDateTime().isNull())
            cevent->setDateStart(event->startDateTime().toTime_t());

        // End date
        if (!event->endDateTime().isNull())
            cevent->setDateEnd(event->endDateTime().toTime_t());

        // Build and set the recurrence information for the event
        CRecurrence *recurrence = createCRecurrence(item);
        cevent->setRecurrence(recurrence);
        delete recurrence; // setting makes a copy
        recurrence = 0;

        retn = cevent;
    }
    else if (item->type() == QOrganizerItemType::TypeTodo) {
        CTodo *ctodo = cal->getTodo(itemIdStr.toStdString(), calError);
        if (!ctodo) {
            // Event did not existed in calendar, create a new CEvent with an empty ID
            ctodo = new CTodo();
        }
        else {
            // Event existed in calendar, use the existing event's ID
            ctodo->setId(itemIdStr.toStdString());
        }
        ctodo->setCalendarId(calId);

        // Set the todo specific details:
        const QOrganizerTodo *todo = static_cast<const QOrganizerTodo *>(item);

        // Location geo coordinates
        if (!todo->detail("QOrganizerItemLocation::DefinitionName").isEmpty())
            ctodo->setGeo(todo->detail<QOrganizerItemLocation>().geoLocation().toStdString());

        // Priority
        if (!todo->detail("QOrganizerItemPriority::DefinitionName").isEmpty())
            ctodo->setPriority(static_cast<int>(todo->priority()));

        // Start date
        // TODO: ctodo->setDateStart and ctodo->setDue actually affect to the
        // same attribute of CTodo instance... How should these be set?

        // Due date
        if (!todo->dueDateTime().isNull())
            ctodo->setDue(todo->dueDateTime().toTime_t());
        // TODO: CTodo::setCompleted, CTodo::setPercentComplete ??

        // TODO: How shall all these time fields be set??????
        /*
        if (!todo->finishedDateTime().isNull())
            ctodo->setDateStart(todo->finishedDateTime().toTime_t());
            */

        // Progress percentage
        ctodo->setPercentComplete(todo->progressPercentage());

        // Build and set the recurrence information for the todo
        CRecurrence *recurrence = createCRecurrence(item);
        ctodo->setRecurrence(recurrence);
        delete recurrence; // setting makes a copy
        recurrence = 0;

        retn = ctodo;
    }
    else if (item->type() == QOrganizerItemType::TypeJournal) {
        CJournal *cjournal = cal->getJournal(itemIdStr.toStdString(), calError);
        if (!cjournal) {
            // Event did not existed in calendar, create a new CEvent with an empty ID
            cjournal = new CJournal();
        }
        else {
            // Event existed in calendar, use the existing event's ID
            cjournal->setId(itemIdStr.toStdString());
        }
        cjournal->setCalendarId(calId);

        // Set journal specific details
        // TODO: There are no journal specific details in Maemo.
        // TODO: Maybe should be removed and should be set on the upper level?
        const QOrganizerJournal *journal = static_cast<const QOrganizerJournal *>(item);
        if (!journal->dateTime().isNull())
            cjournal->setDateStart(journal->dateTime().toTime_t());

        retn = cjournal;
    }

    if (retn) {
        // Set the details common for all the CComponents:

        // Summary
        if (!item->displayLabel().isEmpty())
            retn->setSummary(item->displayLabel().toStdString());

        // Descriptiom
        if (!item->description().isEmpty())
            retn->setDescription(item->description().toStdString());

        // Location (Geo location is not set here as it's not a general CComponent detail)
        QOrganizerItemLocation location = item->detail("QOrganizerItemLocation::DefinitionName");
        if (!location.isEmpty())
            retn->setLocation(location.locationName().toStdString());

        // dateStart and dateEnd are common fields for all CComponents, but those are set
        // separately for each item type here, because there are independent time ranges
        // defined for each type in the Qt Mobility API.

        // (TODO: Status ?)

        // (TODO: Until ?)

        // GUid
        if (!item->guid().isEmpty())
            retn->setGUid(item->guid().toStdString());

        QStringList comments = item->comments();
        if (!comments.isEmpty()) {
            // TODO: Maemo5 has only a single string for comments,
            // map the comment list to a string some way.
        }
    }

    return retn;
}

CRecurrence* QOrganizerItemMaemo5Engine::createCRecurrence(const QOrganizerItem* item) const
{
    // Only the event and todo types contain recurrence information
    if (item->type() == QOrganizerItemType::TypeEvent) {
        d->m_recTransformer.beginTransformToCrecurrence();
        const QOrganizerEvent *event = static_cast<const QOrganizerEvent *>( item );

        // Add recurrence rules
        QList<QOrganizerItemRecurrenceRule> recurrenceRules = event->recurrenceRules();
        foreach (QOrganizerItemRecurrenceRule rule, recurrenceRules)
            d->m_recTransformer.addQOrganizerItemRecurrenceRule(rule);

        // Add exception rules
        QList<QOrganizerItemRecurrenceRule> exceptionRules = event->exceptionRules();
        foreach (QOrganizerItemRecurrenceRule rule, exceptionRules)
            d->m_recTransformer.addQOrganizerItemExceptionRule(rule);

        // TODO: Add recurrenceDates & exceptionDates

        return d->m_recTransformer.crecurrence(); // TODO: This may need error handling?
    }
    else if (item->type() == QOrganizerItemType::TypeTodo) {
        const QOrganizerTodo *todo = static_cast<const QOrganizerTodo *>(item);
        QList<QDate> recurrenceDates = todo->recurrenceDates();
        QList<QOrganizerItemRecurrenceRule> recurrenceRules = todo->recurrenceRules();

        // TODO
    }

    return 0; // no recurrence information for this item type
}

QOrganizerItemManager::Error QOrganizerItemMaemo5Engine::calErrorToManagerError(int calError) const
{
    switch(calError) {
    case CALENDAR_OPERATION_SUCCESSFUL:
        return QOrganizerItemManager::NoError;

    case CALENDAR_SYSTEM_ERROR:
    case CALENDAR_DATABASE_ERROR:
    // case CALENDAR_DBUS_ERROR: // CALENDAR_DBUS_ERROR and CALENDAR_FILE_ERROR has the same value 3
    case CALENDAR_LTIME_ERROR:
    case CALENDAR_LIBALARM_ERROR:
    case CALENDAR_ALARM_ERROR:
    case CALENDAR_APP_ERROR:
    case CALENDAR_FUNC_ERROR:
    case CALENDAR_ICAL_PARSE_ERROR:
    case CALENDAR_INVALID_FILE:
    case CALENDAR_INVALID_ICSFILE:
    case CALENDAR_SCHEMA_CHANGED:
    case CALENDAR_IMPORT_INCOMPLETE:
        return QOrganizerItemManager::UnspecifiedError;

    case CALENDAR_MEMORY_ERROR:
        return QOrganizerItemManager::OutOfMemoryError;

    case CALENDAR_FILE_ERROR:
    case CALENDAR_DOESNOT_EXISTS:
    case CALENDAR_NONE_INDB:
    case NO_DUPLICATE_ITEM:
    case CALENDAR_FETCH_NOITEMS:
        return QOrganizerItemManager::DoesNotExistError;

    case CALENDAR_DISK_FULL:
    case CALENDAR_DB_FULL:
        return QOrganizerItemManager::LimitReachedError;

    case CALENDAR_INVALID_ARG_ERROR:
        return QOrganizerItemManager::BadArgumentError;

    case CALENDAR_ALREADY_EXISTS:
    case CALENDAR_ENTRY_DUPLICATED:
        return QOrganizerItemManager::AlreadyExistsError;

    case CALENDAR_CANNOT_BE_DELETED:
    case EXT_ITEM_RETAINED:
    case LOCAL_ITEM_RETAINED:
        return QOrganizerItemManager::PermissionsError;

    case CALENDAR_DB_LOCKED:
        return QOrganizerItemManager::LockedError;

    case CALENDAR_ICS_COMPONENT_INVALID:
    case CALENDAR_BATCH_ADD_INVALID:
        return QOrganizerItemManager::InvalidDetailError;

    default:
        return QOrganizerItemManager::UnspecifiedError;
    }
}

void QOrganizerItemMaemo5Engine::cleanupCal(CCalendar *cal) const
{
    if (cal != d->m_mcInstance->getDefaultCalendar())
        delete cal; // The default calendar should not be deleted
}
