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

#include "qorganizermaemo6.h"
#include "qtorganizer.h"

//QTM_USE_NAMESPACE

QOrganizerItemManagerEngine* QOrganizerItemMaemo6Factory::engine(const QMap<QString, QString>& parameters, QOrganizerItemManager::Error* error)
{
    Q_UNUSED(parameters);

    Q_UNUSED(error);

    /* TODO - if you understand any specific parameters. save them in the engine so that engine::managerParameters can return them */

    QOrganizerItemMaemo6Engine* ret = new QOrganizerItemMaemo6Engine(); // manager takes ownership and will clean up.
    return ret;
}

QString QOrganizerItemMaemo6Factory::managerName() const
{
    /* TODO - put your engine name here */
    return QLatin1String("maemo6");
}
Q_EXPORT_PLUGIN2(qtorganizer_maemo6, QOrganizerItemMaemo6Factory);


QOrganizerItemMaemo6Engine::QOrganizerItemMaemo6Engine()
    : d(new QOrganizerItemMaemo6EngineData)
{
}

QOrganizerItemMaemo6Engine::~QOrganizerItemMaemo6Engine()
{
    /* TODO clean up your stuff.  Perhaps a QScopedPointer or QSharedDataPointer would be in order */
}

QString QOrganizerItemMaemo6Engine::managerName() const
{
    /* TODO - put your engine name here */
    return QLatin1String("maemo6");
}

QMap<QString, QString> QOrganizerItemMaemo6Engine::managerParameters() const
{
    /* TODO - in case you have any actual parameters that are relevant that you saved in the factory method, return them here */
    return QMap<QString, QString>();
}

int QOrganizerItemMaemo6Engine::managerVersion() const
{
    /* TODO - implement this appropriately.  This is strictly defined by the engine, so return whatever you like */
    return 1;
}

QList<QOrganizerItem> QOrganizerItemMaemo6Engine::itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, QOrganizerItemManager::Error* error) const
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

    QOrganizerItemLocalId generatorId = generator.localId();
    if (!d->m_QIdToKId.contains(generatorId)) {
        *error = QOrganizerItemManager::DoesNotExistError;
        return QList<QOrganizerItem>();
    }

    QString kId = d->m_QIdToKId.value(generatorId);
    Incidence* generatorIncidence = d->m_calendarBackend.incidence(kId);
    Incidence::List generatorList;
    generatorList.append(generatorIncidence);
    ExtendedCalendar::ExpandedIncidenceList incidenceList = d->m_calendarBackend.expandRecurrences(
            &generatorList,
            KDateTime(periodStart),
            KDateTime(periodEnd));
    QList<QOrganizerItem> instances;
    foreach (const ExtendedCalendar::ExpandedIncidence& expandedIncidence, incidenceList) {
        QDateTime incidenceDateTime = expandedIncidence.first;
        Incidence* incidence = expandedIncidence.second;
        IncidenceToItemConverter converter(managerUri());
        QOrganizerItem instance;
        if (converter.convertIncidenceToItem(incidence, &instance)) {
            if (instance.type() == QOrganizerItemType::TypeEvent) {
                QOrganizerEventOccurrence* event = static_cast<QOrganizerEventOccurrence*>(&instance);
                event->setType(QOrganizerItemType::TypeEventOccurrence);
                event->setStartDateTime(incidenceDateTime);
            } else if (instance.type() == QOrganizerItemType::TypeTodo) {
                QOrganizerTodoOccurrence* todo = static_cast<QOrganizerTodoOccurrence*>(&instance);
                todo->setType(QOrganizerItemType::TypeTodo);
                todo->setStartDateTime(incidenceDateTime);
            }
            instances << instance;
        }
    }
    return instances;
}

QList<QOrganizerItemLocalId> QOrganizerItemMaemo6Engine::itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const
{
    QList<QOrganizerItem> ret = items(filter, sortOrders, QOrganizerItemFetchHint(), error);

    // TODO: we don't have to sort again since items() has done it for us
    return QOrganizerItemManagerEngine::sortItems(ret, sortOrders);
}

QList<QOrganizerItem> QOrganizerItemMaemo6Engine::items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(fetchHint);
    // TODO: optimise by using our own filters

    // Just naively get all the incidences
    d->m_calendarBackend.setFilter(0);
    Incidence::List incidences = d->m_calendarBackend.incidences();
    QList<QOrganizerItem> partiallyFilteredItems;

    // Convert them all to QOrganizerItems
    IncidenceToItemConverter converter(managerUri());
    foreach (Incidence* incidence, incidences) {
        QOrganizerItem item;
        if (converter.convertIncidenceToItem(incidence, &item))
            partiallyFilteredItems << item;
    }
    QList<QOrganizerItem> ret;

    // Now filter them
    foreach(const QOrganizerItem& item, partiallyFilteredItems) {
        if (QOrganizerItemManagerEngine::testFilter(filter, item)) {
            QOrganizerItemManagerEngine::addSorted(&ret, item, sortOrders);
        }
    }

    return ret;
}

QOrganizerItem QOrganizerItemMaemo6Engine::item(const QOrganizerItemLocalId& itemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(fetchHint);
    Incidence* theIncidence = incidence(itemId);
    if (!theIncidence) {
        *error = QOrganizerItemManager::DoesNotExistError;
        return QOrganizerItem();
    }
    IncidenceToItemConverter converter(managerUri());
    QOrganizerItem item;
    if (converter.convertIncidenceToItem(theIncidence, &item)) {
        return item;
    } else {
        *error = QOrganizerItemManager::DoesNotExistError;
        return QOrganizerItem();
    }
}

bool QOrganizerItemMaemo6Engine::saveItems(QList<QOrganizerItem>* items, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    /*
        TODO
        The item passed in should be validated according to the schema.
    */
    *error = QOrganizerItemManager::NoError;
    for (int i = 0; i < items->size(); i++) {
        QOrganizerItemManager::Error thisError;
        QOrganizerItem item = items->at(i);
        if (saveItem(&item, &thisError)) {
            items->replace(i, item);
        } else {
            *error = thisError;
            errorMap->insert(i, thisError);
        }
    }
    return *error == QOrganizerItemManager::NoError;
}

bool QOrganizerItemMaemo6Engine::saveItem(QOrganizerItem* item,  QOrganizerItemManager::Error* error)
{
    // ensure that the organizeritem's details conform to their definitions
    if (!validateItem(*item, error)) {
        return false;
    }

    Incidence* theIncidence = softSaveItem(item, error);
    if (theIncidence) {
        d->m_calendarBackend.save();
        QString kId = theIncidence->uid();
        QOrganizerItemLocalId qLocalId = qHash(kId);
        QOrganizerItemId qId;
        qId.setManagerUri(managerUri());
        qId.setLocalId(qLocalId);
        item->setId(qId);
        d->m_QIdToKId.insert(qLocalId, kId);
        return true;
    }
    return false;
}

bool QOrganizerItemMaemo6Engine::removeItems(const QList<QOrganizerItemLocalId>& itemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    *error = QOrganizerItemManager::NoError;
    for (int i = 0; i < itemIds.size(); i++) {
        QOrganizerItemLocalId id = itemIds[i];
        Incidence* theIncidence = incidence(id);
        if (!theIncidence) {
            *error = QOrganizerItemManager::DoesNotExistError;
            errorMap->insert(i, QOrganizerItemManager::DoesNotExistError);
            continue;
        }
        if (d->m_calendarBackend.deleteIncidence(theIncidence)) {
            d->m_QIdToKId.remove(id);
        } else {
            *error = QOrganizerItemManager::UnspecifiedError;
            errorMap->insert(i, QOrganizerItemManager::UnspecifiedError);
        }
    }
    return *error == QOrganizerItemManager::NoError;
}

QMap<QString, QOrganizerItemDetailDefinition> QOrganizerItemMaemo6Engine::detailDefinitions(const QString& itemType, QOrganizerItemManager::Error* error) const
{
    *error = QOrganizerItemManager::NoError;
    return schemaDefinitions().value(itemType);
}

QOrganizerItemDetailDefinition QOrganizerItemMaemo6Engine::detailDefinition(const QString& definitionId, const QString& itemType, QOrganizerItemManager::Error* error) const
{
    /* TODO - the default implementation just calls the base detailDefinitions function.  If that's inefficent, implement this */
    return QOrganizerItemManagerEngine::detailDefinition(definitionId, itemType, error);
}

bool QOrganizerItemMaemo6Engine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& itemType, QOrganizerItemManager::Error* error)
{
    /* TODO - if you support adding custom fields, do that here.  Otherwise call the base functionality. */
    return QOrganizerItemManagerEngine::saveDetailDefinition(def, itemType, error);
}

bool QOrganizerItemMaemo6Engine::removeDetailDefinition(const QString& definitionId, const QString& itemType, QOrganizerItemManager::Error* error)
{
    /* TODO - if you support removing custom fields, do that here.  Otherwise call the base functionality. */
    return QOrganizerItemManagerEngine::removeDetailDefinition(definitionId, itemType, error);
}

bool QOrganizerItemMaemo6Engine::startRequest(QOrganizerItemAbstractRequest* req)
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

bool QOrganizerItemMaemo6Engine::cancelRequest(QOrganizerItemAbstractRequest* req)
{
    /*
        TODO

        Cancel an in progress async request.  If not possible, return false from here.
    */
    return QOrganizerItemManagerEngine::cancelRequest(req);
}

bool QOrganizerItemMaemo6Engine::waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs)
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

void QOrganizerItemMaemo6Engine::requestDestroyed(QOrganizerItemAbstractRequest* req)
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

bool QOrganizerItemMaemo6Engine::hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& itemType) const
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

bool QOrganizerItemMaemo6Engine::isFilterSupported(const QOrganizerItemFilter& filter) const
{
    // TODO if you engine can natively support the filter, return true.  Otherwise you should emulate support in the item{Ids} functions.
    Q_UNUSED(filter);
    return false;
}

QList<QVariant::Type> QOrganizerItemMaemo6Engine::supportedDataTypes() const
{
    QList<QVariant::Type> ret;
    // TODO - tweak which data types this engine understands
    ret << QVariant::String;
    ret << QVariant::Date;
    ret << QVariant::DateTime;
    ret << QVariant::Time;

    return ret;
}

QStringList QOrganizerItemMaemo6Engine::supportedItemTypes() const
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

QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > QOrganizerItemMaemo6Engine::schemaDefinitions() const {
    // lazy initialisation of schema definitions.
    if (d->m_definitions.isEmpty()) {
        // Loop through default schema definitions
        QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > schema
                = QOrganizerItemManagerEngine::schemaDefinitions();
        foreach (const QString& itemType, schema.keys()) {
            // Only add the item types that we support
            if (itemType == QOrganizerItemType::TypeEvent ||
                itemType == QOrganizerItemType::TypeEventOccurrence ||
                itemType == QOrganizerItemType::TypeTodo ||
                itemType == QOrganizerItemType::TypeTodoOccurrence ||
                itemType == QOrganizerItemType::TypeJournal ||
                itemType == QOrganizerItemType::TypeNote) {
                QMap<QString, QOrganizerItemDetailDefinition> definitions
                        = schema.value(itemType);
                
                QMap<QString, QOrganizerItemDetailDefinition> supportedDefinitions;

                QMapIterator<QString, QOrganizerItemDetailDefinition> it(definitions);
                while (it.hasNext()) {
                    it.next();
                    // Only add the definitions that we support
                    if (it.key() == QOrganizerItemType::DefinitionName ||
                        it.key() == QOrganizerItemDescription::DefinitionName ||
                        it.key() == QOrganizerItemDisplayLabel::DefinitionName ||
                        it.key() == QOrganizerItemRecurrence::DefinitionName ||
                        it.key() == QOrganizerEventTimeRange::DefinitionName) {
                        supportedDefinitions.insert(it.key(), it.value());
                    }
                }
                d->m_definitions.insert(itemType, supportedDefinitions);
            }
        }
    }
    return d->m_definitions;
}

Incidence* QOrganizerItemMaemo6Engine::incidence(const QOrganizerItemLocalId& itemId) const
{
    // TODO: check managerUri as well
    if (!d->m_QIdToKId.contains(itemId))
        return 0;
    else
        return d->m_calendarBackend.incidence(d->m_QIdToKId.value(itemId));
}

/*!
 * Saves \a item to the manager, but doesn't persist the change to disk.
 * Sets \a error appropriately if if couldn't be saved.
 */
Incidence* QOrganizerItemMaemo6Engine::softSaveItem(QOrganizerItem* item, QOrganizerItemManager::Error* error)
{
    bool itemIsNew = (managerUri() != item->id().managerUri()
            || item->localId() == 0);
    Incidence* incidence = 0;
    if (item->type() == QOrganizerItemType::TypeEvent) {
        QOrganizerEvent* event = static_cast<QOrganizerEvent*>(item);
        incidence = createKEvent(*event);
    } else if (item->type() == QOrganizerItemType::TypeTodo) {
        QOrganizerTodo* todo = static_cast<QOrganizerTodo*>(item);
        incidence = createKTodo(*todo);
    } else if (item->type() == QOrganizerItemType::TypeNote) {
        QOrganizerNote* note = static_cast<QOrganizerNote*>(item);
        incidence = createKNote(*note);
    } else if (item->type() == QOrganizerItemType::TypeJournal) {
        QOrganizerJournal* journal = static_cast<QOrganizerJournal*>(item);
        incidence = createKJournal(*journal);
    } else {
        *error = QOrganizerItemManager::InvalidItemTypeError;
        return 0;
    }
    if (!itemIsNew) {
        if (!d->m_QIdToKId.contains(item->localId())) {
            *error = QOrganizerItemManager::DoesNotExistError;
            return 0;
        }
        QString uid = d->m_QIdToKId.value(item->localId());
        Incidence* oldIncidence = d->m_calendarBackend.incidence(uid);
        Q_ASSERT(oldIncidence);
        d->m_calendarBackend.deleteIncidence(oldIncidence);
        incidence->setUid(uid);
    }
    d->m_calendarBackend.addIncidence(incidence);
    *error = QOrganizerItemManager::NoError;
    return incidence;
}

/*!
 * Converts \a qEvent into an Incidence which is of subclass Event.  The caller is responsible
 * for deleting the object.
 */
Event* QOrganizerItemMaemo6Engine::createKEvent(const QOrganizerEvent& qEvent)
{
    Event* kEvent = new Event;
    convertCommonDetailsToIncidenceFields(qEvent, kEvent);
    kEvent->setDtStart(KDateTime(qEvent.startDateTime()));
    kEvent->setDtEnd(KDateTime(qEvent.endDateTime()));
    convertQRecurrenceToKRecurrence(qEvent.detail<QOrganizerItemRecurrence>(), kEvent->recurrence());
    return kEvent;
}

/*!
 * Converts \a qTodo into an Incidence which is of subclass Todo.  The caller is responsible
 * for deleting the object.
 */
Todo* QOrganizerItemMaemo6Engine::createKTodo(const QOrganizerTodo& qTodo)
{
    Todo* kTodo = new Todo;
    convertCommonDetailsToIncidenceFields(qTodo, kTodo);
    kTodo->setDtStart(KDateTime(qTodo.startDateTime()));
    kTodo->setDtDue(KDateTime(qTodo.dueDateTime()));
    convertQRecurrenceToKRecurrence(qTodo.detail<QOrganizerItemRecurrence>(), kTodo->recurrence());
    return kTodo;
}

/*!
 * Converts \a qJournal into an Incidence which is of subclass Journal.  The caller is responsible
 * for deleting the object.
 */
Journal* QOrganizerItemMaemo6Engine::createKJournal(const QOrganizerJournal& qJournal)
{
    Journal* kJournal = new Journal;
    convertCommonDetailsToIncidenceFields(qJournal, kJournal);
    return kJournal;
}

/*!
 * Converts \a qNote into an Incidence which is of subclass Journal.  The caller is responsible
 * for deleting the object.
 */
Journal* QOrganizerItemMaemo6Engine::createKNote(const QOrganizerNote& qNote)
{
    Journal* kJournal = new Journal;
    convertCommonDetailsToIncidenceFields(qNote, kJournal);
    return kJournal;
}

/*!
 * Converts the item-common details of \a item to fields to set in \a incidence.
 */
void QOrganizerItemMaemo6Engine::convertCommonDetailsToIncidenceFields(
        const QOrganizerItem& item, Incidence* incidence)
{
    incidence->setDescription(item.description());
    incidence->setSummary(item.displayLabel());
}

/*! Converts \a qRecurrence into the libkcal equivalent, stored in \a kRecurrence.  kRecurrence must
 * point to an initialized Recurrence.
 */
void QOrganizerItemMaemo6Engine::convertQRecurrenceToKRecurrence(
        const QOrganizerItemRecurrence& qRecurrence,
        Recurrence* kRecurrence)
{
    // Remove all recurrence rules in kRecurrence
    foreach (RecurrenceRule* rrule, kRecurrence->rRules()) {
        kRecurrence->deleteRRule(rrule);
    }

    foreach (const QOrganizerItemRecurrenceRule& rrule, qRecurrence.recurrenceRules()) {
        RecurrenceRule* krrule = createKRecurrenceRule(kRecurrence, rrule);
        kRecurrence->addRRule(krrule);
    }
}

RecurrenceRule* QOrganizerItemMaemo6Engine::createKRecurrenceRule(
        Recurrence* kRecurrence,
        const QOrganizerItemRecurrenceRule& qRRule)
{
    RecurrenceRule* kRRule = kRecurrence->defaultRRule(true);
    switch (qRRule.frequency()) {
        case QOrganizerItemRecurrenceRule::Daily:
            kRRule->setRecurrenceType(RecurrenceRule::rDaily);
            break;
        case QOrganizerItemRecurrenceRule::Weekly:
            kRRule->setRecurrenceType(RecurrenceRule::rWeekly);
            break;
        case QOrganizerItemRecurrenceRule::Monthly:
            kRRule->setRecurrenceType(RecurrenceRule::rMonthly);
            break;
        case QOrganizerItemRecurrenceRule::Yearly:
            kRRule->setRecurrenceType(RecurrenceRule::rYearly);
            break;
    }
    kRRule->setFrequency(qRRule.interval());
    kRRule->setDuration(qRRule.count() > 1 ? qRRule.count() : -1);
    QList<RecurrenceRule::WDayPos> days;
    foreach (Qt::DayOfWeek day, qRRule.daysOfWeek()) {
        days.append(RecurrenceRule::WDayPos(0, (short)day)); // first argument is setByPos
                                                             // - unimplemented for now
    }
    kRRule->setByDays(days);
    QDate endDate = qRRule.endDate();
    if (endDate.isValid()) {
        // The endDate is non-inclusive, as per iCalendar
        kRRule->setEndDt(KDateTime(endDate.addDays(-1)));
    }
    return kRRule;
}


/*! \class IncidenceToItemConverter:
 * 
 * This class converts a single kcal Incidence to a QOrganizerItem
 */

/*!
 * Converts a kcal \a incidence into a QOrganizer \a item.
 * \a incidence and \a item must both not be null.
 * Whatever was in \a item before will be ignored and lost.
 */
bool QOrganizerItemMaemo6Engine::IncidenceToItemConverter::convertIncidenceToItem(
        Incidence* incidence, QOrganizerItem* item)
{
    if (incidence->accept(m_visitor)) {
        *item = m_converted;
        return true;
    } else {
        return false;
    }
}

/*!
 * Don't call this directly unless you are an Incidence::AddVisitor.
 * Converts a kcal Event.
 */
bool QOrganizerItemMaemo6Engine::IncidenceToItemConverter::addEvent(Event* e)
{
    m_converted = QOrganizerEvent();
    QOrganizerEvent* event = static_cast<QOrganizerEvent*>(&m_converted);
    convertCommonDetails(e, event);
    event->setStartDateTime(e->dtStart().dateTime());
    event->setEndDateTime(e->dtEnd().dateTime());
    return true;
}

/*!
 * Don't call this directly unless you are an Incidence::AddVisitor.
 * Converts a kcal Todo.
 */
bool QOrganizerItemMaemo6Engine::IncidenceToItemConverter::addTodo(Todo* t)
{
    m_converted = QOrganizerTodo();
    convertCommonDetails(t, &m_converted);
    return true;
}

/*!
 * Don't call this directly unless you are an Incidence::AddVisitor.
 * Converts a kcal Journal.
 */
bool QOrganizerItemMaemo6Engine::IncidenceToItemConverter::addJournal(Journal* j)
{
    if (j->dtStart().isValid())
        m_converted = QOrganizerJournal();
    else
        m_converted = QOrganizerNote();
    convertCommonDetails(j, &m_converted);
    return true;
}

/*!
 * Adds details to \a item based on fields found in \a incidence.
 */
void QOrganizerItemMaemo6Engine::IncidenceToItemConverter::convertCommonDetails(
        Incidence* incidence, QOrganizerItem* item)
{
    QOrganizerItemId id;
    id.setManagerUri(m_managerUri);
    id.setLocalId(qHash(incidence->uid()));
    item->setId(id);
    item->setDisplayLabel(incidence->summary());
    item->setDescription(incidence->description());
}

