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

    //TODO!

    *error = QOrganizerItemManager::NoError;

    //QOrganizerItem item = item( generator.localId(), , error );


    //CCalendar* cal = d->m_mcInstance->getDefaultCalendar();

    if( generator.type() == QOrganizerItemType::TypeTodo )
    {
        // Todo types do not have recurrent occurrences in Maemo5



        //return;
    }


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

QList<QOrganizerItemLocalId> QOrganizerItemMaemo5Engine::itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const
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

void QOrganizerItemMaemo5Engine::removeAllForDebug() const
{
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();

    std::vector<CEvent*> events = cal->getEvents( calError );
    for( int i = 0; i < events.size(); ++i )
    {
        CEvent* cevent = events[i];
        std::string itemId = cevent->getId();
        int calError = CALENDAR_OPERATION_SUCCESSFUL;
        cal->deleteComponent( itemId, calError );
        delete cevent;
        qDebug() << "Deleted event";
    }
    std::vector<CTodo*> todos = cal->getTodos( calError );
    for( int i = 0; i < todos.size(); ++i )
    {
        CTodo* ctodo = todos[i];
        std::string itemId = ctodo->getId();
        int calError = CALENDAR_OPERATION_SUCCESSFUL;
        cal->deleteComponent( itemId, calError );
        delete ctodo;
        qDebug() << "Deleted todo";
    }
    std::vector<CJournal*> journals = cal->getJournals( calError );
    for( int i = 0; i < journals.size(); ++i )
    {
        CJournal* cjournal = journals[i];
        std::string itemId = cjournal->getId();
        int calError = CALENDAR_OPERATION_SUCCESSFUL;
        cal->deleteComponent( itemId, calError );
        delete cjournal;
        qDebug() << "Deleted journal";
    }
}

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    /*
    // A TEMPORARY TEST CODE FOR CREATING RECURRING EVENTS
    // Create event
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();
    CEvent* cevent = new CEvent();
    cevent->setCalendarId( cal->getCalendarId() );
    cevent->setDateStart(QDateTime(QDate(2010,8,20),QTime(11,0,0)).toTime_t());
    cevent->setDateEnd(QDateTime(QDate(2010,8,20),QTime(12,0,0)).toTime_t());
    cevent->setSummary(QString("Recurrence test event").toStdString());
    cevent->setDescription(QString("Should repeat 10 times yearly").toStdString());

    // Create recurrence rule
    CRecurrenceRule* crecrule = new CRecurrenceRule();
    crecrule->setFrequency( YEARLY_RECURRENCE );
    crecrule->setCount( 10 );
    crecrule->setRrule( "FREQ=YEARLY;COUNT=10" );

    qDebug() << "RecurrenceRule: " << QString::fromStdString( crecrule->toString() );
    std::vector< CRecurrenceRule* > rrules;
    rrules.push_back( crecrule );

    // Create recurrence
    CRecurrence* crecurrence = new CRecurrence();
    crecurrence->setRtype( 4 );
    crecurrence->setRecurrenceRule( rrules );
    qDebug() << "Recurrence: " << QString::fromStdString( crecurrence->toString() );

    // Add recurrence to event
    bool ok = cevent->setRecurrence( crecurrence );
    qDebug() << "Status: " << ok;

    // Add event to calendar
    int calError;
    cal->addEvent(cevent, calError);
    qDebug() << "Cal error: " << calError;
    */




    // TODO: This quick implementation is provided only to make the demo application to work
    // It doesn't support any filters, sort orders or fetch hints

    *error = QOrganizerItemManager::NoError;
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    QList<QOrganizerItem> retn;
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();

    std::vector<CEvent*> events = cal->getEvents( calError );
    for( int i = 0; i < events.size(); ++i )
    {
        CEvent* cevent = events[i];

/*
        // Debug code:
        qDebug() << "**- Event id " << QString::fromStdString(cevent->getId());
        CRecurrence* crec = cevent->getRecurrence();
        qDebug() << "**- Recurrence: " << QString::fromStdString( crec->toString() );

        std::vector< CRecurrenceRule* > rrules;
        rrules = crec->getRecurrenceRule();
        qDebug() << rrules.size();
        for( int i = 0; i < rrules.size(); ++i )
        {
            CRecurrenceRule* rule = rrules[i];
            qDebug() << "**- " << QString::fromStdString( rule->toString() );
        }
*/
        QOrganizerEvent event = convertCEventToQEvent( cevent );
        fillInCommonCComponentDetails( &event, cevent );
        delete cevent;
        retn.append( event );
    }

    std::vector<CTodo*> todos = cal->getTodos( calError );
    for( int i = 0; i < todos.size(); ++i )
    {
        CTodo* ctodo = todos[i];
        QOrganizerTodo todo = convertCTodoToQTodo( ctodo );
        fillInCommonCComponentDetails( &todo, ctodo );
        delete ctodo;
        retn.append( todo );
    }

    std::vector<CJournal*> journals = cal->getJournals( calError );
    for( int i = 0; i < journals.size(); ++i )
    {
        CJournal* cjournal = journals[i];
        QOrganizerJournal journal = convertCJournalToQJournal( cjournal );
        fillInCommonCComponentDetails( &journal, cjournal );
        delete cjournal;
        retn.append( journal );
    }

    cleanupCal( cal );
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

QOrganizerItem QOrganizerItemMaemo5Engine::item(const QOrganizerItemLocalId& itemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(fetchHint);
    // TODO: Make this method to use the fetch hint

    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();
    std::string nativeId = QString::number(itemId).toStdString();
    int calError = CALENDAR_OPERATION_SUCCESSFUL;

    CEvent* event = cal->getEvent( nativeId, calError );
    *error = calErrorToManagerError( calError );
    if ( event ) {
        QOrganizerEvent retn = convertCEventToQEvent( event );
        fillInCommonCComponentDetails( &retn, event );
        delete event;
        cleanupCal( cal );
        return retn;
    }
    CTodo* todo = cal->getTodo( nativeId, calError );
    *error = calErrorToManagerError( calError );
    if ( todo ) {
        QOrganizerTodo retn = convertCTodoToQTodo( todo );
        fillInCommonCComponentDetails( &retn, todo );
        delete todo;
        cleanupCal( cal );
        return retn;
    }
    CJournal* journal = cal->getJournal( nativeId, calError );
    *error = calErrorToManagerError( calError );
    if ( journal ) {
        QOrganizerJournal retn = convertCJournalToQJournal( journal ); // TODO: Probably this call is not needed
        fillInCommonCComponentDetails( &retn, journal );
        delete journal;
        cleanupCal( cal );
        return retn;
    }

    // In an error situation return an invalid item
    cleanupCal( cal );
    *error = QOrganizerItemManager::DoesNotExistError;
    return QOrganizerItem();
}

bool QOrganizerItemMaemo5Engine::saveItems(QList<QOrganizerItem>* items, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    // TODO: Add changeset manipulation and signal emissions

    if ( !items ) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    *error = QOrganizerItemManager::NoError;
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();
    bool success = true;

    for( int i = 0; i < items->size(); ++i) {
        QOrganizerItem curr = items->at(i);
        int calError = doSaveItem( cal, &curr, error );

        if ( calError != CALENDAR_OPERATION_SUCCESSFUL || *error != QOrganizerItemManager::NoError ) {
            success = false;
            curr.setId(QOrganizerItemId()); // clear ID
            if ( errorMap ) {
                if ( *error != QOrganizerItemManager::NoError )
                    errorMap->insert(i, *error );
                else
                    errorMap->insert(i, QOrganizerItemManager::UnspecifiedError );
            }
        }

        items->replace(i, curr);
    }

    cleanupCal( cal );
    return success;
}

bool QOrganizerItemMaemo5Engine::removeItems(const QList<QOrganizerItemLocalId>& itemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    // TODO: Add changeset manipulation and signal emissions
    // TODO: Check all the dependencies of the removed item

    *error = QOrganizerItemManager::NoError;
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();
    bool success = true;

    for( int i = 0; i < itemIds.size(); ++i ) {
        QOrganizerItemLocalId currId = itemIds.at(i);

        QOrganizerItemFetchHint::OptimizationHints optimizationHints( QOrganizerItemFetchHint::NoBinaryBlobs );
        optimizationHints |= QOrganizerItemFetchHint::NoActionPreferences;
        QOrganizerItemFetchHint fetchHints;
        fetchHints.setOptimizationHints( optimizationHints );
        QOrganizerItem currItem = item( currId, fetchHints, error );
        if ( *error == QOrganizerItemManager::NoError ) {
            // Item exists
            QString itemId = QString::number( currItem.localId() );
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            cal->deleteComponent( itemId.toStdString(), calError );
            *error = calErrorToManagerError( calError );

            if ( calError != CALENDAR_OPERATION_SUCCESSFUL )
            {
                success = false;
                if ( errorMap )
                    errorMap->insert(i, *error);
            }
        } else {
            success = false;
            if ( errorMap )
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

int QOrganizerItemMaemo5Engine::doSaveItem(CCalendar* cal, QOrganizerItem* item, QOrganizerItemManager::Error* error)
{
    // TODO: Add signal emissions
    // TODO: Or might be better not to implement emissions here?

    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    *error = QOrganizerItemManager::InvalidItemTypeError;

    CComponent* component = createCComponent( cal, item );
    if( !component )
        return calError;

    // TODO: The custom detail fields should be iterated and the corresponding
    // fields should be set to CComponent for saving.

    if (item->type() == QOrganizerItemType::TypeEvent) {
        CEvent* cevent = static_cast<CEvent*>( component );
        QString ceventId = QString::fromStdString( cevent->getId() );
        if ( !ceventId.isEmpty() ) {
            // CEvent ID is not empty, the event already exists in calendar
            cal->modifyEvent(cevent, calError);
            *error = calErrorToManagerError( calError );
        } else {
            // CEvent ID is empty, the event is new
            cal->addEvent(cevent, calError);
            *error = calErrorToManagerError( calError );
            if( calError == CALENDAR_OPERATION_SUCCESSFUL ||
                calError == CALENDAR_ENTRY_DUPLICATED )
                // The Maemo5 calendar does not accept two items if all the
                // time details are equal. That's so even if the item IDs differ.
                // If the "new" item is actually a duplicate, then let
                // the calendar to modify the existing item.
                // TODO: Is that what we want?
            {
                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error

                QString newIdString = QString::fromStdString( cevent->getId() );
                QOrganizerItemLocalId newId = newIdString.toUInt();
                QOrganizerItemId id;
                id.setLocalId( newId );
                id.setManagerUri(managerUri());
                item->setId( id );
            }

        }
        delete cevent;
        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeTodo) {
        CTodo* ctodo = static_cast<CTodo*>( component );
        QString ctodoId = QString::fromStdString( ctodo->getId() );
        if ( !ctodoId.isEmpty() ) {
            // CTodo ID is not empty, the todo already exists in calendar
            cal->modifyTodo(ctodo, calError);
            *error = calErrorToManagerError( calError );
        } else {
            // CTodo ID is empty, the todo is new
            cal->addTodo(ctodo, calError);
            *error = calErrorToManagerError( calError );
            if( calError == CALENDAR_OPERATION_SUCCESSFUL ||
                calError == CALENDAR_ENTRY_DUPLICATED )
                // The Maemo5 calendar does not accept two items if all the
                // time details are equal. That's so even if the item IDs differ.
                // If the "new" item is actually a duplicate, then let
                // the calendar to modify the existing item.
                // TODO: Is that what we want?
            {
                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error

                QString newIdString = QString::fromStdString( ctodo->getId() );
                QOrganizerItemLocalId newId = newIdString.toUInt();
                QOrganizerItemId id;
                id.setLocalId( newId );
                id.setManagerUri(managerUri());
                item->setId( id );
            }

        }
        delete ctodo;
        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeJournal) {
        CJournal* cjournal = static_cast<CJournal*>( component );
        QString cjournalId = QString::fromStdString( cjournal->getId() );
        if ( !cjournalId.isEmpty() ) {
            // CJournal ID is not empty, the journal already exists in calendar
            cal->modifyJournal(cjournal, calError);
            *error = calErrorToManagerError( calError );
        } else {
            // CJournal ID is empty, the journal is new
            cal->addJournal(cjournal, calError);
            *error = calErrorToManagerError( calError );
            if( calError == CALENDAR_OPERATION_SUCCESSFUL ||
                calError == CALENDAR_ENTRY_DUPLICATED )
                // The Maemo5 calendar does not accept two items if all the
                // time details are equal. That's so even if the item IDs differ.
                // If the "new" item is actually a duplicate, then let
                // the calendar to modify the existing item.
                // TODO: Is that what we want?
            {
                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error

                QString newIdString = QString::fromStdString( cjournal->getId() );
                QOrganizerItemLocalId newId = newIdString.toUInt();
                QOrganizerItemId id;
                id.setLocalId( newId );
                id.setManagerUri(managerUri());
                item->setId( id );
            }

        }
        delete cjournal;
        return calError;
    }

    return calError;
}

QOrganizerEvent QOrganizerItemMaemo5Engine::convertCEventToQEvent(CEvent* cevent) const
{
    QOrganizerEvent ret;
    int tempint = cevent->getPriority();
    if (tempint != -1)
        ret.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint)); // assume that the saved priority is vCal compliant.
    QDateTime tempdt = QDateTime::fromTime_t(cevent->getDateStart());
    if (!tempdt.isNull())
        ret.setStartDateTime(tempdt);
    tempdt = QDateTime::fromTime_t(cevent->getDateEnd());
    if (!tempdt.isNull())
        ret.setEndDateTime(tempdt);

    // TODO: the recurrence information?

    return ret;
}

QOrganizerEventOccurrence QOrganizerItemMaemo5Engine::convertCEventToQEventOccurrence(CEvent* cevent, const QDateTime& instanceDate, const QString& calendarName) const
{
    // TODO: The code in this method is from the Proof of Concept
    // TODO: Replace the Proof of Concept codes at some point

    QOrganizerEventOccurrence ret;
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

    // now set the parent information (parent id and original datetime)
    QOrganizerItemId pId;
    pId.setManagerUri(managerUri());
    QString hashKey = calendarName + ":" + QString::fromStdString(cevent->getId());
    pId.setLocalId(QOrganizerItemLocalId(qHash(hashKey)));

    // TODO: These were removed from API, check if these are still needed
    //ret.setParentItemId(pId);
    //ret.setOriginalDateTime(instanceDate);
    
    // generate a new (empty) id for the occurrence.
    QOrganizerItemId rId;
    rId.setManagerUri(managerUri());
    rId.setLocalId(QOrganizerItemLocalId()); // generated instances have no local id.
    ret.setId(rId);

    return ret;
}

QOrganizerTodo QOrganizerItemMaemo5Engine::convertCTodoToQTodo(CTodo* ctodo ) const
{
    QOrganizerTodo ret;
    int tempint = ctodo->getPriority();
    if (tempint != -1)
        ret.setPriority(static_cast<QOrganizerItemPriority::Priority>(tempint));
    QDateTime tempdt = QDateTime::fromTime_t(ctodo->getDateStart());
    if (!tempdt.isNull())
        ret.setNotBeforeDateTime(tempdt);
    tempdt = QDateTime::fromTime_t(ctodo->getDue());
    if (!tempdt.isNull())
        ret.setDueDateTime(tempdt);

    return ret;
}

QOrganizerTodoOccurrence QOrganizerItemMaemo5Engine::convertCTodoToQTodoOccurrence(CTodo* ctodo, const QString& calendarName) const
{
    // TODO: The code in this method is from the Proof of Concept
    // TODO: Replace the Proof of Concept codes at some point

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
    // TODO: Commented out, check this!
    //ret.setStatus(static_cast<QOrganizerItemTodoProgress::Status>(ctodo->getStatus()));
    
    QOrganizerItemId rId;
    rId.setManagerUri(managerUri());
    QString hashKey = calendarName + ":" + QString::fromStdString(ctodo->getId());
    rId.setLocalId(QOrganizerItemLocalId(qHash(hashKey)));
    ret.setId(rId);

    // now, in maemo, the parent id is the same as this id (todo's only have one occurrence).

    // TODO: These were removed from API, check if these are still needed?
    //ret.setParentItemId(rId);
    //ret.setOriginalDateTime(QDateTime::fromTime_t(ctodo->getDue())); // XXX TODO: verify this is the correct field to use as the instance date...

    return ret;
}

QOrganizerJournal QOrganizerItemMaemo5Engine::convertCJournalToQJournal(CJournal* cjournal) const
{
    // TODO: Probably this method is not needed at all. Check if this can be removed.

    QOrganizerJournal ret;
    QString tempstr = QString::fromStdString(cjournal->getDescription());
    if (!tempstr.isEmpty())
        ret.setDescription(tempstr);
    tempstr = QString::fromStdString(cjournal->getSummary());
    if (!tempstr.isEmpty())
        ret.setDisplayLabel(tempstr);

    /*
    QString idString = QString::fromStdString( cjournal->getId() );
    QOrganizerItemLocalId localId = idString.toUInt();
    QOrganizerItemId id;
    id.setLocalId( localId );
    id.setManagerUri(managerUri());
    ret.setId( id );
    */

    return ret;
}

void QOrganizerItemMaemo5Engine::fillInCommonCComponentDetails( QOrganizerItem* item, CComponent* component ) const
{
    // TODO: Possible fields to add (defined in the CComponent superclass):
    // TODO: flags, status, (datestart), (dateend), lastmodified, (comments)
    // TODO: recurrence, until, rtype, alarm, allday, tzoffset... still others??

    // TODO: What to do with attendee, instanceorigin, participation, priority, provenance
    // TODO: rsvpinfo, timestamp... others??

    if( item ) {
        QString tempstr = QString::fromStdString(component->getSummary());
        if (!tempstr.isEmpty())
            item->setDisplayLabel(tempstr);
        tempstr = QString::fromStdString(component->getDescription());
        if (!tempstr.isEmpty())
            item->setDescription(tempstr);

        tempstr = QString::fromStdString(component->getLocation());
        if(!tempstr.isEmpty()) {
            QOrganizerItemLocation il = item->detail<QOrganizerItemLocation>();
            il.setGeoLocation( tempstr );
            item->saveDetail(&il);
        }
        tempstr = QString::fromStdString(component->getGUid());
        if(!tempstr.isEmpty()) {
            QOrganizerItemGuid ig = item->detail<QOrganizerItemGuid>();
            ig.setGuid( tempstr );
            item->saveDetail(&ig);
        }

        // Component ID
        QString idString = QString::fromStdString( component->getId() );
        QOrganizerItemLocalId localId = idString.toUInt();
        QOrganizerItemId id;
        id.setLocalId(localId);
        id.setManagerUri(managerUri());
        item->setId( id );
    }
}

CComponent* QOrganizerItemMaemo5Engine::createCComponent( CCalendar* cal, const QOrganizerItem* item ) const
{
    QOrganizerItemLocalId itemId = item->localId();
    QString itemIdStr = QString::number( itemId );
    int calId = cal->getCalendarId();
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    CComponent* retn = 0; // Return null on errors

    if (item->type() == QOrganizerItemType::TypeEvent) {
        CEvent* cevent = cal->getEvent( itemIdStr.toStdString(), calError );
        if ( !cevent ) {
            // Event did not existed in calendar, create a new CEvent with an empty ID
            cevent = new CEvent();
        }
        else {
            // Event existed in calendar, use the existing event's ID
            cevent->setId( itemIdStr.toStdString() );
        }
        cevent->setCalendarId( calId );

        // Set the event specific details
        const QOrganizerEvent* event = static_cast<const QOrganizerEvent*>( item );
        if (!event->locationGeoCoordinates().isEmpty())
            cevent->setGeo(event->locationGeoCoordinates().toStdString());
        if (!event->detail("QOrganizerItemPriority::DefinitionName").isEmpty())
            cevent->setPriority(static_cast<int>(event->priority()));

        // Build and set the recurrence information for the event
        CRecurrence* recurrence = createCRecurrence( item );
        cevent->setRecurrence( recurrence );
        delete recurrence; // setting makes a copy
        recurrence = 0;

        // TODO: Maybe the following should be removed and should be set on the upper level?
        if (!event->startDateTime().isNull())
            cevent->setDateStart(event->startDateTime().toTime_t());
        if (!event->endDateTime().isNull())
            cevent->setDateEnd(event->endDateTime().toTime_t());

        retn = cevent;
    }
    else if (item->type() == QOrganizerItemType::TypeTodo) {
        CTodo* ctodo = cal->getTodo( itemIdStr.toStdString(), calError );
        if ( !ctodo ) {
            // Event did not existed in calendar, create a new CEvent with an empty ID
            ctodo = new CTodo();
        }
        else {
            // Event existed in calendar, use the existing event's ID
            ctodo->setId( itemIdStr.toStdString() );
        }
        ctodo->setCalendarId( calId );

        // Set the todo specific details
        const QOrganizerTodo* todo = static_cast<const QOrganizerTodo*>( item );
        if (!todo->detail("QOrganizerItemLocation::DefinitionName").isEmpty())
            ctodo->setGeo(todo->detail<QOrganizerItemLocation>().geoLocation().toStdString());
        if (!todo->detail("QOrganizerItemPriority::DefinitionName").isEmpty())
            ctodo->setPriority(static_cast<int>(todo->priority()));
        if (!todo->dueDateTime().isNull())
            ctodo->setDue(todo->dueDateTime().toTime_t());
        // TODO: CTodo::setCompleted, CTodo::setPercentComplete ??

        // TODO: Recurrence information

        // TODO: Maybe the following should be removed and should be set on the upper level?
        if (!todo->notBeforeDateTime().isNull())
            ctodo->setDateStart(todo->notBeforeDateTime().toTime_t());

        retn = ctodo;
    }
    else if (item->type() == QOrganizerItemType::TypeJournal) {
        CJournal* cjournal = cal->getJournal( itemIdStr.toStdString(), calError );
        if ( !cjournal ) {
            // Event did not existed in calendar, create a new CEvent with an empty ID
            cjournal = new CJournal();
        }
        else {
            // Event existed in calendar, use the existing event's ID
            cjournal->setId( itemIdStr.toStdString() );
        }
        cjournal->setCalendarId( calId );

        // Set journal specific details
        // TODO: There are no journal specific details in Maemo.
        // TODO: Maybe should be removed and should be set on the upper level?
        const QOrganizerJournal* journal = static_cast<const QOrganizerJournal*>( item );
        if (!journal->dateTime().isNull())
            cjournal->setDateStart(journal->dateTime().toTime_t());

        retn = cjournal;
    }

    if( retn ) {
        // Set the common details for all CComponents:
        if (!item->displayLabel().isEmpty())
            retn->setSummary(item->displayLabel().toStdString());
        if (!item->description().isEmpty())
            retn->setDescription(item->description().toStdString());
        if (!item->guid().isEmpty())
            retn->setGUid(item->guid().toStdString());

        QStringList comments = item->comments();
        if ( !comments.isEmpty() ) {
            // TODO: Maemo5 has only a single string for comments,
            // map the comment list to a string some way.
        }
    }

    return retn;
}

CRecurrence* QOrganizerItemMaemo5Engine::createCRecurrence( const QOrganizerItem* item ) const
{
    // Only the event and todo types contain recurrence information
    if (item->type() == QOrganizerItemType::TypeEvent) {
        d->m_recTransformer.beginTransformToCrecurrence();
        const QOrganizerEvent* event = static_cast<const QOrganizerEvent*>( item );

        // Add recurrence rules
        QList<QOrganizerItemRecurrenceRule> recurrenceRules = event->recurrenceRules();
        foreach( QOrganizerItemRecurrenceRule rule, recurrenceRules )
            d->m_recTransformer.addQOrganizerItemRecurrenceRule( rule );

        // Add exception rules
        QList<QOrganizerItemRecurrenceRule> exceptionRules = event->exceptionRules();
        foreach( QOrganizerItemRecurrenceRule rule, exceptionRules )
            d->m_recTransformer.addQOrganizerItemExceptionRule( rule );

        // TODO: Add recurrenceDates & exceptionDates

        return d->m_recTransformer.crecurrence(); // TODO: This may need error handling?
    }
    else if (item->type() == QOrganizerItemType::TypeTodo) {
        const QOrganizerTodo* todo = static_cast<const QOrganizerTodo*>( item );
        QList<QDate> recurrenceDates = todo->recurrenceDates();
        QList<QOrganizerItemRecurrenceRule> recurrenceRules = todo->recurrenceRules();

        // TODO
    }

    return 0; // no recurrence information for this item type
}

QOrganizerItemManager::Error QOrganizerItemMaemo5Engine::calErrorToManagerError( int calError ) const
{
    switch( calError ) {
    case CALENDAR_OPERATION_SUCCESSFUL:
        {
            return QOrganizerItemManager::NoError;
        }
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
        {
            return QOrganizerItemManager::UnspecifiedError;
        }
    case CALENDAR_MEMORY_ERROR:
        {
            return QOrganizerItemManager::OutOfMemoryError;
        }
    case CALENDAR_FILE_ERROR:
    case CALENDAR_DOESNOT_EXISTS:
    case CALENDAR_NONE_INDB:
    case NO_DUPLICATE_ITEM:
    case CALENDAR_FETCH_NOITEMS:
        {
            return QOrganizerItemManager::DoesNotExistError;
        }
    case CALENDAR_DISK_FULL:
    case CALENDAR_DB_FULL:
        {
            return QOrganizerItemManager::LimitReachedError;
        }
    case CALENDAR_INVALID_ARG_ERROR:
        {
            return QOrganizerItemManager::BadArgumentError;
        }
    case CALENDAR_ALREADY_EXISTS:
    case CALENDAR_ENTRY_DUPLICATED:
        {
            return QOrganizerItemManager::AlreadyExistsError;
        }
    case CALENDAR_CANNOT_BE_DELETED:
    case EXT_ITEM_RETAINED:
    case LOCAL_ITEM_RETAINED:
        {
            return QOrganizerItemManager::PermissionsError;
        }
    case CALENDAR_DB_LOCKED:
        {
            return QOrganizerItemManager::LockedError;
        }
    case CALENDAR_ICS_COMPONENT_INVALID:
    case CALENDAR_BATCH_ADD_INVALID:
        {
            return QOrganizerItemManager::InvalidDetailError;
        }
    default:
        {
            return QOrganizerItemManager::UnspecifiedError;
        }
    }
}

void QOrganizerItemMaemo5Engine::cleanupCal( CCalendar* cal ) const
{
    if ( cal != d->m_mcInstance->getDefaultCalendar() )
        delete cal; // The default calendar should not be deleted
}
