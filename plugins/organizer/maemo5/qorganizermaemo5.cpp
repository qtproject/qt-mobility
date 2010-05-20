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

QTM_USE_NAMESPACE

QOrganizerItemManagerEngine* QOrganizerItemMaemo5Factory::engine(const QMap<QString, QString>& parameters, QOrganizerItemManager::Error* error)
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);


    QOrganizerItemMaemo5Engine* ret = new QOrganizerItemMaemo5Engine(); // manager takes ownership and will clean up.
    return ret;
}

QString QOrganizerItemMaemo5Factory::managerName() const
{
    return QString("maemo5");
}
Q_EXPORT_PLUGIN2(qtorganizer_maemo5, QOrganizerItemMaemo5Factory);

QOrganizerItemMaemo5Engine::QOrganizerItemMaemo5Engine()
    : d(new QOrganizerItemMaemo5EngineData)
{
    // XXX TODO: on construction, we need to enumerate all items in
    // the database and fill out our maps of ids and calendar names.
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

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, QOrganizerItemManager::Error* error) const
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

/*!
  Loads all items from all calendars, updates the private data maps of cId to qId and cId to cName, and updates the changeset with any changes which were noticed during enumeration.  Does not emit signals.
  */
QList<QOrganizerItem> QOrganizerItemMaemo5Engine::enumerateAllItems(QOrganizerItemChangeSet *cs, QOrganizerItemManager::Error* error) const
{
    *error = QOrganizerItemManager::NoError;
    int calError = CALENDAR_OPERATION_SUCCESSFUL; // default is no error

    QList<QOrganizerItem> allItems;

    // update our map arguments and changeset where needed.
    QMap<QString, QOrganizerItemLocalId> newIdMap;
    QMap<QString, QString> newCNameMap;
    QString entireId;
    
    std::vector<CCalendar*> allCalendars = d->m_mcInstance->getListCalFromMc();
    for (unsigned int i = 0; i < allCalendars.size(); i++) {
        CCalendar *currCal = allCalendars[i];
        QString calName = QString::fromStdString(currCal->getCalendarName());

	// get the events
        std::vector<CEvent*> events = currCal->getEvents(calError);
        if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
            *error = QOrganizerItemManager::UnspecifiedError;
            calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
        }
        for (unsigned int j = 0; j < events.size(); ++j) {
            entireId = calName + ":" + QString::fromStdString(events[j]->getId());
	    newIdMap.insert(entireId, QOrganizerItemLocalId(qHash(entireId)));
	    newCNameMap.insert(entireId, calName);
            allItems.append(convertCEventToQEvent(events[j], calName));
            delete events[j];
        }

	// get the todos
        std::vector<CTodo*> todos = currCal->getTodos(calError);
        if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
            *error = QOrganizerItemManager::UnspecifiedError;
            calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
        }
        for (unsigned int j = 0; j < todos.size(); ++j) {
            entireId = calName + ":" + QString::fromStdString(todos[j]->getId());
	    newIdMap.insert(entireId, QOrganizerItemLocalId(qHash(entireId)));
	    newCNameMap.insert(entireId, calName);
            allItems.append(convertCTodoToQTodo(todos[j], calName));
            delete todos[j];
        }

	// get the journals
        std::vector<CJournal*> journals = currCal->getJournals(calError);
        if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
            *error = QOrganizerItemManager::UnspecifiedError;
            calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
        }
        for (unsigned int j = 0; j < journals.size(); ++j) {
            entireId = calName + ":" + QString::fromStdString(journals[j]->getId());
	    newIdMap.insert(entireId, QOrganizerItemLocalId(qHash(entireId)));
	    newCNameMap.insert(entireId, calName);
            allItems.append(convertCJournalToQJournal(journals[j], calName));
	    delete journals[j];
        }
    }
    d->m_mcInstance->releaseListCalendars(allCalendars);
    
    // before returning, build the changeset and emit appropriate signals
    // then replace our maps with our newly updated ones.
    QStringList origKeys = d->m_cIdToQId.keys();
    QStringList newKeys = newIdMap.keys();
    foreach (const QString& key, origKeys) {
        if (!newKeys.contains(key)) {
            // this id is only in the old map (ie, has been removed)
            cs->insertRemovedItem(d->m_cIdToQId.value(key));
        } else {
            // this id is in both (ie, hasn been added or removed)
            newKeys.removeOne(key);
        }
    }

    foreach (const QString& key, newKeys) {
        // this id is only in the new map (ie, has been added)
        cs->insertAddedItem(newIdMap.value(key));
    }

    // update our maps.
    d->m_cIdToQId = newIdMap;
    d->m_cIdToCName = newCNameMap;

    // caller should emit signals from cs if they want to.
    return allItems;
}

QList<QOrganizerItemLocalId> QOrganizerItemMaemo5Engine::itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const
{
    // build the list of items which we'll return
    QOrganizerItemChangeSet cs;
    QList<QOrganizerItem> allItems = enumerateAllItems(&cs, error);
    QList<QOrganizerItem> ret;
    foreach(const QOrganizerItem& item, allItems) {
        if (QOrganizerItemManagerEngine::testFilter(filter, item)) {
            ret.append(item);
        }
    }

    //cs.emitSignals(this); // XXX TODO: does this break synchronous behaviour?

    return QOrganizerItemManagerEngine::sortItems(ret, sortOrders);
}

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(fetchHint);

    QOrganizerItemChangeSet cs;
    QList<QOrganizerItem> allItems = enumerateAllItems(&cs, error);
    QList<QOrganizerItem> ret;
    foreach(const QOrganizerItem& item, allItems) {
        if (QOrganizerItemManagerEngine::testFilter(filter, item)) {
            QOrganizerItemManagerEngine::addSorted(&ret, item, sortOrders);
        }
    }

    //cs.emitSignals(this); // XXX TODO: does this break synchronous behaviour?

    return ret;
}

QOrganizerItem QOrganizerItemMaemo5Engine::item(const QOrganizerItemLocalId& itemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(fetchHint);
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    *error = QOrganizerItemManager::NoError;

    QString entireItemId = d->m_cIdToQId.key(itemId);
    if (entireItemId.isEmpty()) {
        // user has specified a nonexistent id.
        *error = QOrganizerItemManager::DoesNotExistError;
        return QOrganizerItem();
    }
    QString calendarName = d->m_cIdToCName.value(entireItemId);
    QString calItemId = entireItemId.mid((calendarName.size() + 1), -1); // entireItemId = calendarName:cId
    CCalendar* calendar = d->m_mcInstance->getCalendarByName(calendarName.toStdString(), calError);

    // now attempt to get the item.
    CEvent* event = calendar->getEvent(calItemId.toStdString(), calError);
    if (event) {
        QOrganizerEvent retn = convertCEventToQEvent(event, calendarName);
        delete event;
        return retn;
    }
    
    CTodo* todo = calendar->getTodo(calItemId.toStdString(), calError);
    if (todo) {
        QOrganizerTodo retn = convertCTodoToQTodo(todo, calendarName);
        delete todo;
        return retn;
    }

    CJournal* journal = calendar->getJournal(calItemId.toStdString(), calError);
    if (journal) {
        QOrganizerJournal retn = convertCJournalToQJournal(journal, calendarName);
        delete journal;
        return retn;
    }

    // the id exists in our maps but not in the database.  we need to update our maps.
    QOrganizerItemChangeSet cs;
    cs.insertRemovedItem(d->m_cIdToQId.value(entireItemId));
    d->m_cIdToQId.remove(entireItemId);
    d->m_cIdToCName.remove(entireItemId);
    //cs.emitSignals(this); // XXX TODO: does this break synchronous behaviour?
    
    // none of the above exist in the specified calendar, so return error.
    *error = QOrganizerItemManager::DoesNotExistError;
    return QOrganizerItem();
}

bool QOrganizerItemMaemo5Engine::saveItems(QList<QOrganizerItem>* items, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    *error = QOrganizerItemManager::NoError;
    QOrganizerItemChangeSet cs;

    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    for (int i = 0; i < items->size(); i++) {
        QOrganizerItem curr = items->at(i);
        int calendarId = -1;
	if (curr.type() == QOrganizerItemType::TypeEvent) {
            QOrganizerEvent event = curr;
            CEvent* cevent = convertQEventToCEvent(event);
            calendarId = cevent->getCalendarId();
            CCalendar *currCal = d->m_mcInstance->getCalendarById(calendarId, calError); // note: during the conversion phase we set the calendar id... likely to be very slow.
	    QString calName = QString::fromStdString(currCal->getCalendarName());

            // the convert function will set the id to empty string if has been deleted in meantime.
	    if (!QString::fromStdString(cevent->getId()).isEmpty()) {
                d->m_mcInstance->modifyEvent(cevent, calendarId, calError);
                if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
                    if (errorMap) {
                        errorMap->insert(i, QOrganizerItemManager::UnspecifiedError);
                    }
                    *error = QOrganizerItemManager::UnspecifiedError;
                    calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
                } else {
                    // successfully update event.
                    items->replace(i, convertCEventToQEvent(cevent, calName));
                    cs.insertChangedItem(curr.localId());
                }
            } else {
                // this is a new (or previously deleted) event.  save it new.
                d->m_mcInstance->addEvent(cevent, calendarId, calError);
		if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
                    if (errorMap) {
                        errorMap->insert(i, QOrganizerItemManager::UnspecifiedError);
                    }
                    *error = QOrganizerItemManager::UnspecifiedError;
                    calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
                } else {
                    // success.  add the details to our internal maps.
                    QString hashKey = calName + ":" + QString::fromStdString(cevent->getId());
                    d->m_cIdToQId.insert(hashKey, QOrganizerItemLocalId(qHash(hashKey)));
                    d->m_cIdToCName.insert(hashKey, calName);
                    items->replace(i, convertCEventToQEvent(cevent, calName));
                    cs.insertAddedItem(d->m_cIdToQId.value(hashKey));
                }
            }

            // clean up.
	    delete cevent;
            delete currCal;
        } else if (curr.type() == QOrganizerItemType::TypeEventOccurrence) {
            // check if there are any differences from the "generated" occurrence for this date
            // if not, then save can fail (already exists error)
            // if so, we save it as an exception.
	} else if (curr.type() == QOrganizerItemType::TypeTodo) {
            // some tricks here; in maemo5 there are no todoOccurrences.
            // this means, they can modify the todo, and the (single) occurrence for it will change...
            // other than that concern, the code for a todo is similar to that of event.
            QOrganizerTodo todo = curr;
            CTodo* ctodo = convertQTodoToCTodo(todo);
            calendarId = ctodo->getCalendarId();
            CCalendar *currCal = d->m_mcInstance->getCalendarById(calendarId, calError); // note: during the conversion phase we set the calendar id... likely to be very slow.
	    QString calName = QString::fromStdString(currCal->getCalendarName());

            // the convert function will set the id to empty string if has been deleted in meantime.
	    if (!QString::fromStdString(ctodo->getId()).isEmpty()) {
                d->m_mcInstance->modifyTodo(ctodo, calendarId, calError);
                if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
                    if (errorMap) {
                        errorMap->insert(i, QOrganizerItemManager::UnspecifiedError);
                    }
                    *error = QOrganizerItemManager::UnspecifiedError;
                    calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
                } else {
                    // successfully updated todo item.
                    items->replace(i, convertCTodoToQTodo(ctodo, calName));
                    cs.insertChangedItem(curr.localId());
                }
            } else {
                // this is a new (or previously deleted) event.  save it new.
                d->m_mcInstance->addTodo(ctodo, calendarId, calError);
		if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
                    if (errorMap) {
                        errorMap->insert(i, QOrganizerItemManager::UnspecifiedError);
                    }
                    *error = QOrganizerItemManager::UnspecifiedError;
                    calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
                } else {
                    // success.  add the details to our internal maps.
                    QString hashKey = calName + ":" + QString::fromStdString(ctodo->getId());
                    d->m_cIdToQId.insert(hashKey, QOrganizerItemLocalId(qHash(hashKey)));
                    d->m_cIdToCName.insert(hashKey, calName);
                    items->replace(i, convertCTodoToQTodo(ctodo, calName));
                    cs.insertAddedItem(d->m_cIdToQId.value(hashKey));
                }
            }

            // clean up.
	    delete ctodo;
            delete currCal;
        } else if (curr.type() == QOrganizerItemType::TypeTodoOccurrence) {
            // modifies the actual todo in the datastore...
        } else if (curr.type() == QOrganizerItemType::TypeJournal) {
            // basically the same as code for the event.
            QOrganizerJournal journal = curr;
            CJournal* cjournal = convertQJournalToCJournal(journal);
            calendarId = cjournal->getCalendarId();
            CCalendar *currCal = d->m_mcInstance->getCalendarById(calendarId, calError); // note: during the conversion phase we set the calendar id... likely to be very slow.
	    QString calName = QString::fromStdString(currCal->getCalendarName());

            // the convert function will set the id to empty string if has been deleted in meantime.
	    if (!QString::fromStdString(cjournal->getId()).isEmpty()) {
                d->m_mcInstance->modifyJournal(cjournal, calendarId, calError);
                if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
                    if (errorMap) {
                        errorMap->insert(i, QOrganizerItemManager::UnspecifiedError);
                    }
                    *error = QOrganizerItemManager::UnspecifiedError;
                    calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
                } else {
                    // successfully updated journal.
                    items->replace(i, convertCJournalToQJournal(cjournal, calName));
                    cs.insertChangedItem(curr.localId());
                }
            } else {
                // this is a new (or previously deleted) event.  save it new.
                d->m_mcInstance->addJournal(cjournal, calendarId, calError);
		if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
                    if (errorMap) {
                        errorMap->insert(i, QOrganizerItemManager::UnspecifiedError);
                    }
                    *error = QOrganizerItemManager::UnspecifiedError;
                    calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
                } else {
                    // success.  add the details to our internal maps.
                    QString hashKey = calName + ":" + QString::fromStdString(cjournal->getId());
                    d->m_cIdToQId.insert(hashKey, QOrganizerItemLocalId(qHash(hashKey)));
                    d->m_cIdToCName.insert(hashKey, calName);
                    items->replace(i, convertCJournalToQJournal(cjournal, calName));
                    cs.insertAddedItem(d->m_cIdToQId.value(hashKey));
                }
            }

            // clean up.
	    delete cjournal;
            delete currCal;
        } else { // QOrganizerItemType::TypeNote
            // not sure what to do here - should be similar to journal without datestart??
        }
    }

    // ensure that the changes are committed to the dbase.
    d->m_mcInstance->commitAllChanges();

    // emit signals...
    cs.emitSignals(this);
    
    return (*error == QOrganizerItemManager::NoError);
}

bool QOrganizerItemMaemo5Engine::removeItems(const QList<QOrganizerItemLocalId>& itemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    // this is much simpler, since the maemo5 calendar-backend API
    // supports removal by component id.  Note that while the API
    // supports batch remove (and rolls back if fails),
    // we want to provide fine-grained error reporting, so we do
    // the operation one at a time...
    // XXX TODO: check performance, change to batch if too slow.

    *error = QOrganizerItemManager::NoError;
    int calError = 1; // no error.
    QOrganizerItemChangeSet cs;
   
    for (int i = 0; i < itemIds.size(); ++i) {
        QOrganizerItemLocalId currId = itemIds.at(i);
        QString entireId = d->m_cIdToQId.key(currId);
        if (entireId.isEmpty()) {
            calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
            *error = QOrganizerItemManager::DoesNotExistError;
            if (errorMap) {
                errorMap->insert(i, QOrganizerItemManager::DoesNotExistError);
            }

            // user has specified a nonexistent id to remove.
            continue;
        }
        QString calendarName = d->m_cIdToCName.value(entireId);
        QString cId = entireId.mid((calendarName.size() + 1), -1); // entireId = calendarName:cId
        CCalendar* calendar = d->m_mcInstance->getCalendarByName(calendarName.toStdString(), calError);
        if (calendar) {
            calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
            std::vector<std::string> removeItemIds;
            removeItemIds.push_back(cId.toStdString());
            d->m_mcInstance->deleteComponents(removeItemIds, calendar->getCalendarId(), calError);
	    
            if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
                *error = QOrganizerItemManager::DoesNotExistError;
                if (errorMap) {
                    errorMap->insert(i, QOrganizerItemManager::DoesNotExistError);
                }
            } else {
                // remove the item from our maps.
                d->m_cIdToQId.remove(entireId);
                d->m_cIdToCName.remove(entireId);
                cs.insertRemovedItem(currId);
            }
        } else {
            // XXX TODO: make a "InvalidCalendar" error
            calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
            *error = QOrganizerItemManager::DoesNotExistError;
            if (errorMap) {
                errorMap->insert(i, QOrganizerItemManager::UnspecifiedError);
            }
        }
	delete calendar;
    }

    // ensure that the changes are committed to the dbase.
    d->m_mcInstance->commitAllChanges();

    // emit signals
    cs.emitSignals(this);
    return (*error == QOrganizerItemManager::NoError);
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

bool QOrganizerItemMaemo5Engine::hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& itemType) const
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

bool QOrganizerItemMaemo5Engine::isFilterSupported(const QOrganizerItemFilter& filter) const
{
    // TODO if you engine can natively support the filter, return true.  Otherwise you should emulate support in the item{Ids} functions.
    Q_UNUSED(filter);
    return false;
}

QList<QVariant::Type> QOrganizerItemMaemo5Engine::supportedDataTypes() const
{
    QList<QVariant::Type> ret;
    // TODO - tweak which data types this engine understands
    ret << QVariant::String;
    ret << QVariant::Date;
    ret << QVariant::DateTime;
    ret << QVariant::Time;

    return ret;
}

QStringList QOrganizerItemMaemo5Engine::supportedItemTypes() const
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


QOrganizerEvent QOrganizerItemMaemo5Engine::convertCEventToQEvent(CEvent* cevent, const QString& calendarName) const
{
    QOrganizerEvent ret;
    QString tempstr = QString::fromStdString(cevent->getGeo());
    if (!tempstr.isEmpty())
        ret.setLocationGeoCoordinates(tempstr);
    int tempint = cevent->getPriority();
    if (tempint != -1)
        ret.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint)); // assume that the saved priority is vCal compliant.
    tempstr = QString::fromStdString(cevent->getSummary());
    if (!tempstr.isEmpty())
        ret.setDisplayLabel(tempstr);
    tempstr = QString::fromStdString(cevent->getDescription());
    if (!tempstr.isEmpty())
        ret.setDescription(tempstr);
    tempstr = QString::fromStdString(cevent->getLocation());
    if (!tempstr.isEmpty())
        ret.setLocationName(tempstr);
    QDateTime tempdt = QDateTime::fromTime_t(cevent->getDateStart());
    if (!tempdt.isNull())
        ret.setStartDateTime(tempdt);
    tempdt = QDateTime::fromTime_t(cevent->getDateEnd());
    if (!tempdt.isNull())
        ret.setEndDateTime(tempdt);

    QOrganizerItemId rId;
    rId.setManagerUri(managerUri());
    QString hashKey = calendarName + ":" + QString::fromStdString(cevent->getId());
    rId.setLocalId(QOrganizerItemLocalId(qHash(hashKey)));
    ret.setId(rId);
    // and the recurrence information...

    return ret;
}

QOrganizerTodo QOrganizerItemMaemo5Engine::convertCTodoToQTodo(CTodo* ctodo, const QString& calendarName) const
{
    QOrganizerTodo ret;
    int tempint = ctodo->getPriority();
    if (tempint != -1)
        ret.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint));
    QDateTime tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        ret.setNotBeforeDateTime(tempdt);
    QString tempstr = QString::fromStdString(ctodo->getSummary());
    if (!tempstr.isEmpty())
        ret.setDisplayLabel(tempstr);
    tempdt = QDateTime::fromTime_t(ctodo->getDue());
    if (!tempdt.isNull())
        ret.setDueDateTime(tempdt);

    QOrganizerItemId rId;
    rId.setManagerUri(managerUri());
    QString hashKey = calendarName + ":" + QString::fromStdString(ctodo->getId());
    rId.setLocalId(QOrganizerItemLocalId(qHash(hashKey)));
    ret.setId(rId);
    // and the recurrence information

    return ret;
}

QOrganizerTodoOccurrence QOrganizerItemMaemo5Engine::convertCTodoToQTodoOccurrence(CTodo* ctodo, const QString& calendarName) const
{
    QOrganizerTodoOccurrence ret;
    int tempint = ctodo->getPriority();
    if (tempint != -1)
        ret.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint));
    QDateTime tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        ret.setNotBeforeDateTime(tempdt);
    QString tempstr = QString::fromStdString(ctodo->getSummary());
    if (!tempstr.isEmpty())
        ret.setDisplayLabel(tempstr);
    tempdt = QDateTime::fromTime_t(ctodo->getDue());
    if (!tempdt.isNull())
        ret.setDueDateTime(tempdt);
    tempint = ctodo->getPercentComplete();
    if (tempint != -1)
        ret.setProgressPercentage(tempint);
    tempdt = QDateTime::fromTime_t(ctodo->getCompleted());
    if (!tempdt.isNull())
        ret.setFinishedDateTime(tempdt);
    tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        ret.setStartedDateTime(tempdt);
    
    // status is always available..
    ret.setStatus(static_cast<QOrganizerItemTodoProgress::Status>(ctodo->getStatus()));
    
    // XXX TODO: set the parent information stuff.
    QOrganizerItemId rId;
    rId.setManagerUri(managerUri());
    QString hashKey = calendarName + ":" + QString::fromStdString(ctodo->getId());
    rId.setLocalId(QOrganizerItemLocalId(qHash(hashKey)));
    ret.setId(rId);

    return ret;
}

QOrganizerJournal QOrganizerItemMaemo5Engine::convertCJournalToQJournal(CJournal* cjournal, const QString& calendarName) const
{
    QOrganizerJournal ret;

    QString tempstr = QString::fromStdString(cjournal->getDescription());
    if (!tempstr.isEmpty())
        ret.setDescription(tempstr);
    tempstr = QString::fromStdString(cjournal->getSummary());
    if (!tempstr.isEmpty())
        ret.setDisplayLabel(tempstr);

    QOrganizerItemId rId;
    rId.setManagerUri(managerUri());
    QString hashKey = calendarName + ":" + QString::fromStdString(cjournal->getId());
    rId.setLocalId(QOrganizerItemLocalId(qHash(hashKey)));
    ret.setId(rId);
    
    return ret;
}

CEvent* QOrganizerItemMaemo5Engine::convertQEventToCEvent(const QOrganizerEvent& event) const
{
    // first grab the QOILId, dehash it, find the calendar name, and then
    // set the id and calendar id in the event item.
    // then set all of the details and return.

    QOrganizerItemLocalId eventId = event.localId();
    QString entireId = d->m_cIdToQId.key(eventId);
    QString calName = d->m_cIdToCName.value(entireId);
    QString eventStrId = entireId.mid((calName.size() + 1), -1); // entireId = calendarName:cId

    int calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
    bool deleteCalendar = true;
    CCalendar* destinationCal = d->m_mcInstance->getCalendarByName(calName.toStdString(), calError);
    if (destinationCal == 0) {
        // the calendar does not exist.  either the item is a "new" item,
        // or the calendar or item has been removed by another user.
        // use the default calendar instead.
        // XXX TODO: parametrise which calendar is default in ctor of Maemo5Engine.
        destinationCal = d->m_mcInstance->getDefaultCalendar();
        deleteCalendar = false; // according to docs, shouldn't delete this calendar.
    }

    int calId = destinationCal->getCalendarId(); // id of calendar in which to save the event.
    
    // now, check to see whether the item still exists in the calendar.
    // if not, set the id to empty string.
    // we'd use CCalendar::checkEntryExist() but no idea what the tableName should be...
    CEvent *ret = destinationCal->getEvent(eventStrId.toStdString(), calError);
    if (!ret) {
        // if doesn't exist, clear the id, and remove the entries from the map (cache).
        eventStrId.clear();
        ret = new CEvent();
        d->m_cIdToQId.remove(entireId);
        d->m_cIdToCName.remove(entireId);
    }
    ret->setId(eventStrId.toStdString());
    ret->setCalendarId(calId);
   
    if (!event.locationGeoCoordinates().isEmpty()) 
        ret->setGeo(event.locationGeoCoordinates().toStdString());
    if (!event.detail("QOrganizerItemPriority::DefinitionName").isEmpty())
        ret->setPriority(static_cast<int>(event.priority()));
    if (!event.displayLabel().isEmpty())
        ret->setSummary(event.displayLabel().toStdString());
    if (!event.description().isEmpty())
        ret->setDescription(event.description().toStdString());
    if (!event.startDateTime().isNull())
        ret->setDateStart(event.startDateTime().toTime_t());
    if (!event.endDateTime().isNull())
        ret->setDateEnd(event.endDateTime().toTime_t());

    // XXX TODO: recurrence information.
 

    if (destinationCal && deleteCalendar)
        delete destinationCal;

    return ret;
}


CTodo* QOrganizerItemMaemo5Engine::convertQTodoToCTodo(const QOrganizerTodo& todo) const
{
    // first grab the QOILId, dehash it, find the calendar name, and then
    // set the id and calendar id in the todo item.
    // then set all of the details and return.

    // suggest we also need to (find or generate) the occurrence and fill out the
    // appropriate CTodo fields from the occurrence...

    QOrganizerItemLocalId todoId = todo.localId();
    QString entireId = d->m_cIdToQId.key(todoId);
    QString calName = d->m_cIdToCName.value(entireId);
    QString todoStrId = entireId.mid((calName.size() + 1), -1); // entireId = calendarName:cId

    int calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
    bool deleteCalendar = true;
    CCalendar* destinationCal = d->m_mcInstance->getCalendarByName(calName.toStdString(), calError);
    if (destinationCal == 0) {
        // the calendar does not exist.  either the item is a "new" item,
        // or the calendar or item has been removed by another user.
        // use the default calendar instead.
        // XXX TODO: parametrise which calendar is default in ctor of Maemo5Engine.
        destinationCal = d->m_mcInstance->getDefaultCalendar();
        deleteCalendar = false; // according to docs, shouldn't delete this calendar.
    }

    int calId = destinationCal->getCalendarId(); // id of calendar in which to save the event.
    
    // now, check to see whether the item still exists in the calendar.
    // if not, set the id to empty string.
    // we'd use CCalendar::checkEntryExist() but no idea what the tableName should be...
    CTodo *ret = destinationCal->getTodo(todoStrId.toStdString(), calError);
    if (!ret) {
        // if doesn't exist, clear the id, and remove the entries from the map (cache).
        todoStrId.clear();
        ret = new CTodo();
        d->m_cIdToQId.remove(entireId);
        d->m_cIdToCName.remove(entireId);
    }
    ret->setId(todoStrId.toStdString());
    ret->setCalendarId(calId);
   
    if (!todo.detail("QOrganizerItemPriority::DefinitionName").isEmpty())
        ret->setPriority(static_cast<int>(todo.priority()));
    if (!todo.displayLabel().isEmpty())
        ret->setSummary(todo.displayLabel().toStdString());
    if (!todo.description().isEmpty())
        ret->setDescription(todo.description().toStdString());
    if (!todo.notBeforeDateTime().isNull())
        ret->setDateStart(todo.notBeforeDateTime().toTime_t());
    if (!todo.dueDateTime().isNull())
        ret->setDue(todo.dueDateTime().toTime_t());

    // XXX TODO: generate or load the occurrence, and fill out required fields.
 

    if (destinationCal && deleteCalendar)
        delete destinationCal;

    return ret;
}


CJournal* QOrganizerItemMaemo5Engine::convertQJournalToCJournal(const QOrganizerJournal& journal) const
{
    // first grab the QOILId, dehash it, find the calendar name, and then
    // set the id and calendar id in the journal item.
    // then set all of the details and return.

    QOrganizerItemLocalId journalId = journal.localId();
    QString entireId = d->m_cIdToQId.key(journalId);
    QString calName = d->m_cIdToCName.value(entireId);
    QString journalStrId = entireId.mid((calName.size() + 1), -1); // entireId = calendarName:cId

    int calError = CALENDAR_OPERATION_SUCCESSFUL; // reset error variable
    bool deleteCalendar = true;
    CCalendar* destinationCal = d->m_mcInstance->getCalendarByName(calName.toStdString(), calError);
    if (destinationCal == 0) {
        // the calendar does not exist.  either the item is a "new" item,
        // or the calendar or item has been removed by another user.
        // use the default calendar instead.
        // XXX TODO: parametrise which calendar is default in ctor of Maemo5Engine.
        destinationCal = d->m_mcInstance->getDefaultCalendar();
        deleteCalendar = false; // according to docs, shouldn't delete this calendar.
    }

    int calId = destinationCal->getCalendarId(); // id of calendar in which to save the event.
    
    // now, check to see whether the item still exists in the calendar.
    // if not, set the id to empty string.
    // we'd use CCalendar::checkEntryExist() but no idea what the tableName should be...
    CJournal *ret = destinationCal->getJournal(journalStrId.toStdString(), calError);
    if (!ret) {
        // if doesn't exist, clear the id, and remove the entries from the map (cache).
        journalStrId.clear();
        ret = new CJournal();
        d->m_cIdToQId.remove(entireId);
        d->m_cIdToCName.remove(entireId);
    }
    ret->setId(journalStrId.toStdString());
    ret->setCalendarId(calId);

    if (!journal.displayLabel().isEmpty())
        ret->setSummary(journal.displayLabel().toStdString());
    if (!journal.description().isEmpty())
        ret->setDescription(journal.description().toStdString());
    if (!journal.dateTime().isNull())
        ret->setDateStart(journal.dateTime().toTime_t());

    // XXX TODO: other fields?  notes etc?
 

    if (destinationCal && deleteCalendar)
        delete destinationCal;

    return ret;
}
