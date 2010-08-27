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

#include <QDir>
#include <QFileSystemWatcher>

#include <CalendarErrors.h>
#include <CEvent.h>
#include <CJournal.h>
#include <CTodo.h>
#include <CRecurrence.h>

/**
 * entryType enum
 * This enum is from the Maemo5 calendar backend file Common.h
 * The enumeration is pasted here to avoid including Common.h file,
 * which contains dependencies to D-Bus handling and other components
 * that are not actually needed here.
*/
enum entryType {
    E_CALENDAR = 0,
    E_EVENT,
    E_TODO,
    E_JOURNAL,
    E_BDAY,
    E_SPARE,
    E_VTIMEZONE
};

QTM_USE_NAMESPACE

static const char* CALENDAR =  "/.calendar";
static const char* CALENDARDB = "/calendardb";

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
    QString dbPath = QDir::homePath().append(CALENDAR).append(CALENDARDB);
    QFileSystemWatcher *databaseMonitor = new QFileSystemWatcher(this);
    databaseMonitor->addPath(dbPath);
    connect(databaseMonitor, SIGNAL(fileChanged(const QString &)), this, SLOT(dataChanged()));

    d->m_itemTransformer.setManagerUri(managerUri());
    d->m_asynchProcess = new OrganizerAsynchProcess(this);

    bool dbOk = d->m_databaseAccess.open(QDir::homePath().append(CALENDAR).append(CALENDARDB));
    if (!dbOk) {
        qDebug() << "Database didn't open!";
        // TODO: Then what???
    }
}

void QOrganizerItemMaemo5Engine::dataChanged()
{
    // Timer prevents from sending multiple signals
    // when database is changed during short period of time
    if (!m_waitTimer.isActive()) {
        m_waitTimer.setSingleShot(true);
        m_waitTimer.setInterval(50);
        m_waitTimer.start();
        emit dataChanged();
    }
}

QOrganizerItemMaemo5Engine::~QOrganizerItemMaemo5Engine()
{
    delete d->m_asynchProcess;
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

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::itemInstances(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    QList<QOrganizerItem> retn;

    QList<QOrganizerItemSortOrder> noSorting;
    QList<QOrganizerItem> itemList = items(QOrganizerItemFilter(), noSorting, fetchMinimalData(), error);

    if (*error != QOrganizerItemManager::NoError)
        return QList<QOrganizerItem>();

    if (itemList.isEmpty())
        return retn;

    QDateTime periodStart;
    QDateTime periodEnd;
    if (filter.type() == QOrganizerItemFilter::OrganizerItemDateTimePeriodFilter) {
        // if only a single period filter is used, the period can be obtained directly from the filter
        QOrganizerItemDateTimePeriodFilter dateTimePeriodFilter = static_cast<QOrganizerItemDateTimePeriodFilter>(filter);
        periodStart = dateTimePeriodFilter.startPeriod();
        periodEnd = dateTimePeriodFilter.endPeriod();
    }
    else {
        // otherwise just set a large enough period
        // TODO: If absolutely needed for performace reasons the union and intersection filters could be iterated
        // TODO: too to resolve the needed period. However it is quite a laborous task and not done for now.
        periodStart = QDateTime(QDate(1970, 1, 1), QTime(0, 0, 0));
        periodEnd = QDateTime(QDate(2037, 12, 31), QTime(23, 59, 59)); // the max year for maemo5 calendar is 2037
    }

    foreach (QOrganizerItem generatorItem, itemList) {
        if (generatorItem.type() == QOrganizerItemType::TypeEvent || generatorItem.type() == QOrganizerItemType::TypeTodo)
        {
            QList<QOrganizerItem> generatorInstances = itemInstances(generatorItem, periodStart, periodEnd, 0, error);
            if (*error != QOrganizerItemManager::NoError)
                break;

            foreach (QOrganizerItem instance, generatorInstances)
                if (QOrganizerItemManagerEngine::testFilter(filter, instance))
                    QOrganizerItemManagerEngine::addSorted(&retn, instance, sortOrders);
        }
    }

    return retn;
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

                // NOTE: If this implementation have to be changed so that only items
                // which completely fit inside the period will be returned, it affects to
                // other parts of code as well. Especially event occurrence saving and
                // parent resolving must be verified to work.
                if (instanceStartDate >= periodStart && instanceStartDate <= periodEnd) {
                    QOrganizerEventOccurrence eventOcc =
                            d->m_itemTransformer.convertCEventToQEventOccurrence(cevent, instanceStartDate, instanceEndDate);
                    d->m_itemTransformer.fillInCommonCComponentDetails(&eventOcc, cevent, false); // false = do not set local ids
                    retn << eventOcc;
                }
            }

            // Now we have got the simple occurrences that are generated with the recurrence rules.
            // Events can have also occurrence that are not generated with rules, but saved as
            // events (because they have become modified). Those occurrences are saved with GUID set
            // equal to the generator event's GUID.
            QString eventType = QOrganizerItemType::TypeEvent;
            ////std::vector<CEvent*> occurrenceCandidates = cal->getEvents(cevent->getGUid(), calError);
            std::vector<CEvent*> occurrenceCandidates = d->m_databaseAccess.getEvents(cal->getCalendarId(), cevent->getGUid(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError) {
                std::vector<CEvent*>::iterator occCand;
                for (occCand = occurrenceCandidates.begin(); occCand != occurrenceCandidates.end(); ++occCand) {
                    CEvent* coccurrenceCandidate = *occCand;
                    if (coccurrenceCandidate && coccurrenceCandidate->getId() != cevent->getId()) {
                        // for all other events than the generator itself
                        QDateTime instanceStartDate = QDateTime::fromTime_t(coccurrenceCandidate->getDateStart());
                        QDateTime instanceEndDate = QDateTime::fromTime_t(coccurrenceCandidate->getDateEnd());
                        QString idString = QString::fromStdString(cevent->getId());
                        QOrganizerItemLocalId parentLocalId = idString.toUInt();

                        // instance must be within the period
                        // NOTE: If this implementation have to be changed so that only items
                        // which completely fit inside the period will be returned, it affects to
                        // other parts of code as well. Especially event occurrence saving and
                        // parent resolving must be verified to work.
                        if (instanceStartDate >= periodStart && instanceStartDate <= periodEnd) {
                            if (isOccurrence(cal, coccurrenceCandidate, eventType, error)) {
                                if (*error == QOrganizerItemManager::NoError) {
                                    QOrganizerEventOccurrence eventOcc =
                                            d->m_itemTransformer.convertCEventToQEventOccurrence(coccurrenceCandidate, instanceStartDate, instanceEndDate, parentLocalId);
                                    d->m_itemTransformer.fillInCommonCComponentDetails(&eventOcc, coccurrenceCandidate);

                                    // insert occurrence to the result list in right position
                                    insertOccurenceSortedByStartDate(&eventOcc, retn);
                                }
                            }
                        }
                    }
                    delete coccurrenceCandidate;
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
            // In Maemo5, only one occurrence per todo. It has the same id as the parent
            QDateTime instanceStartDate = QDateTime::fromTime_t(ctodo->getDateStart());
            QDateTime instanceDueDate = QDateTime::fromTime_t(ctodo->getDue());
            if ((instanceStartDate >= periodStart && instanceStartDate <= periodEnd)
                || (instanceDueDate >= periodStart && instanceDueDate <= periodEnd)) {
                // Instance start time or instance due time is in period
                QOrganizerTodoOccurrence todoOcc = d->m_itemTransformer.convertCTodoToQTodoOccurrence(ctodo);
                d->m_itemTransformer.fillInCommonCComponentDetails(&todoOcc, ctodo);
                retn << todoOcc;
            }
        }
        delete ctodo;
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
    *error = QOrganizerItemManager::NoError;
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    QList<QOrganizerItemLocalId> retn;
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();

    std::vector<std::string>::const_iterator id;

    // Append event ids
    std::vector<std::string> eventIds = cal->getIdList(E_EVENT, calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (*error != QOrganizerItemManager::NoError)
        return retn;
    for (id = eventIds.begin(); id != eventIds.end(); ++id)
        retn << QString::fromStdString(*id).toUInt();

    // Append todo ids
    std::vector<std::string> todoIds = cal->getIdList(E_TODO, calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (*error != QOrganizerItemManager::NoError)
        return retn;
    for (id = todoIds.begin(); id != todoIds.end(); ++id)
        retn << QString::fromStdString(*id).toUInt();

    // Append journal ids
    std::vector<std::string> journalIds = cal->getIdList(E_JOURNAL, calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (*error != QOrganizerItemManager::NoError)
        return retn;
    for (id = journalIds.begin(); id != journalIds.end(); ++id)
        retn << QString::fromStdString(*id).toUInt();

    // If no filtering and sorting needed, return the result (this a fast case)
    if (filter == QOrganizerItemInvalidFilter() && sortOrders.count() == 0)
        return retn;

    // Use the general implementation to filter and sort items
    QList<QOrganizerItem> filteredAndSorted;
    foreach(const QOrganizerItemLocalId& id, retn) {
        QOrganizerItem item = internalFetchItem(id, fetchMinimalData(), error, true);
        if (*error == QOrganizerItemManager::NoError) {
            if (QOrganizerItemManagerEngine::testFilter(filter, item))
                QOrganizerItemManagerEngine::addSorted(&filteredAndSorted, item, sortOrders);
        }
    }

    retn.clear();
    foreach(const QOrganizerItem& item, filteredAndSorted)
        retn << item.localId();

    return retn;
}

QList<QOrganizerItem> QOrganizerItemMaemo5Engine::items(const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const
{
    *error = QOrganizerItemManager::NoError;
    QList<QOrganizerItem> retn;

    // Get item ids
    QList<QOrganizerItemLocalId> ids = itemIds(filter, sortOrders, error);
    if (*error != QOrganizerItemManager::NoError)
        return QList<QOrganizerItem>();

    // Get items
    QList<QOrganizerItemLocalId>::const_iterator id;
    for (id = ids.constBegin(); id != ids.constEnd(); ++id)
        retn << internalFetchItem(*id, fetchHint, error, true);

    return retn;
}

QOrganizerItem QOrganizerItemMaemo5Engine::item(const QOrganizerItemLocalId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const
{
    return internalFetchItem(itemId, fetchHint, error, true);

    /*
    // TODO: Do multicalendar (collection) support properly
    QOrganizerCollection defaultCollection;
    defaultCollection.setId(QOrganizerCollectionId());

        QOrganizerItemManagerEngine::setItemCollectionId(&retn, defaultCollection.id()); // TODO: FIXME.
        QOrganizerItemManagerEngine::setItemCollectionId(&retn, defaultCollection.id()); // TODO: FIXME.
        QOrganizerItemManagerEngine::setItemCollectionId(&retn, defaultCollection.id()); // TODO: FIXME.
        */
}

bool QOrganizerItemMaemo5Engine::saveItems(QList<QOrganizerItem>* items, const QOrganizerCollectionLocalId& collectionId, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    if (!items || items->isEmpty()) {
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
    *error = QOrganizerItemManager::NoError;
    CCalendar* cal = d->m_mcInstance->getDefaultCalendar();
    bool success = true;
    QOrganizerItemChangeSet cs;

    QList<QOrganizerItemLocalId> idsToDelete = itemIds;
    int i = 0;
    while (i < idsToDelete.size()) {
        QOrganizerItemLocalId currId = idsToDelete.at(i);

        QOrganizerItem currItem = internalFetchItem(currId, fetchMinimalData(), error, false);
        if (*error == QOrganizerItemManager::NoError) {
            // Item exists
            QString itemId = QString::number(currItem.localId());
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            if (currItem.type() == QOrganizerItemType::TypeEvent) {
                // Delete also child events if this event is a parent
                CEvent* cevent = cal->getEvent(itemId.toStdString(), calError);
                if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
                    bool parentItem = isParent(cal, cevent, QOrganizerItemType::TypeEvent, error);
                    if (parentItem && *error == QOrganizerItemManager::NoError) {
                        ////std::vector<CEvent*> eventsWithGuid = cal->getEvents(cevent->getGUid(), calError);
                        std::vector<CEvent*> eventsWithGuid = d->m_databaseAccess.getEvents(cal->getCalendarId(), cevent->getGUid(), calError);
                        *error = d->m_itemTransformer.calErrorToManagerError(calError);
                        if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
                            std::vector<CEvent*>::const_iterator childEvent;
                            for (childEvent = eventsWithGuid.begin(); childEvent != eventsWithGuid.end(); ++childEvent) {
                                QOrganizerItemLocalId childId = QString::fromStdString((*childEvent)->getId()).toUInt();
                                if (!idsToDelete.contains(childId))
                                    idsToDelete << childId;
                                delete *childEvent;
                            }
                        }
                    }
                    delete cevent;
                }
            }
            *error = d->m_itemTransformer.calErrorToManagerError(calError);

            if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
                // Delete a component:
                cal->deleteComponent(itemId.toStdString(), calError);
                *error = d->m_itemTransformer.calErrorToManagerError(calError);

                if (calError == CALENDAR_OPERATION_SUCCESSFUL) {
                    // Success, update the changeset
                    cs.insertRemovedItem(currItem.localId());
                }
                else {
                    success = false;
                    if (errorMap)
                        errorMap->insert(i, *error);
                }
            }
            else {
                success = false;
                if (errorMap)
                    errorMap->insert(i, *error);
            }
        }
        else {
            success = false;
            if (errorMap)
                errorMap->insert(i, *error);
        }
        ++i;
    }

    d->m_mcInstance->commitAllChanges(); // ensure that changes are committed before emitting signals
    cs.emitSignals(this);
    cleanupCal(cal);
    return success;
}

QOrganizerCollectionLocalId QOrganizerItemMaemo5Engine::defaultCollectionId(QOrganizerItemManager::Error* error) const
{
    *error = QOrganizerItemManager::NoError;
    CCalendar *cal = d->m_mcInstance->getDefaultCalendar();
    return static_cast<QOrganizerCollectionLocalId>(cal->getCalendarId());
}

QList<QOrganizerCollectionLocalId> QOrganizerItemMaemo5Engine::collectionIds(QOrganizerItemManager::Error* error) const
{
    *error = QOrganizerItemManager::NoError;
    QList<QOrganizerCollectionLocalId> retn;

    // Append the default calendar id
    retn << defaultCollectionId(error);

    // Append other calendar ids
    std::vector<CCalendar*> calendars = d->m_mcInstance->getListCalFromMc();
    std::vector<CCalendar*>::iterator calendar;
    for (calendar = calendars.begin(); calendar != calendars.end(); ++calendar) {
        QOrganizerCollectionLocalId current = static_cast<QOrganizerCollectionLocalId>((*calendar)->getCalendarId());
        if (current != retn[0])
            retn << current;
    }

    // Cleanup
    d->m_mcInstance->releaseListCalendars(calendars);

    return retn;
}

QList<QOrganizerCollection> QOrganizerItemMaemo5Engine::collections(const QList<QOrganizerCollectionLocalId>& collectionIds, QOrganizerItemManager::Error* error) const
{
    *error = QOrganizerItemManager::NoError;
    QList<QOrganizerCollection> retn;

    // Fetch the available collection ids
    QList<QOrganizerCollectionLocalId> fetchCollIds = this->collectionIds(error);

    // Check that all the requested collections exist
    foreach (QOrganizerCollectionLocalId collectionId, collectionIds)
        if (!fetchCollIds.contains(collectionId))
            *error = QOrganizerItemManager::DoesNotExistError;

    if (*error != QOrganizerItemManager::NoError)
        return retn;

    foreach (QOrganizerCollectionLocalId collectionId, fetchCollIds) {
        if (collectionIds.isEmpty() || collectionIds.contains(collectionId)) {
            // Fetch calendar
            int calendarId = static_cast<int>(collectionId);
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            CCalendar *cal = d->m_mcInstance->getCalendarById(calendarId, calError);
            if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
                *error = d->m_itemTransformer.calErrorToManagerError(calError);
                break;
            }

            // Build collection id
            QOrganizerCollectionId currCollectionId;
            currCollectionId.setManagerUri(managerUri());
            currCollectionId.setLocalId(collectionId);

            // Set collection id
            QOrganizerCollection currCollection;
            currCollection.setId(currCollectionId);

            // Set collection metadata:
            // Available calendar colors
            QList<QString> availableColors = d->m_itemTransformer.calendarColourMap().values();
            currCollection.setMetaData("Available colors", QVariant(availableColors));

            // Calendar color
            currCollection.setMetaData("Color", QVariant(
                    d->m_itemTransformer.fromCalendarColour(cal->getCalendarColor())));

            // Calendar name
            currCollection.setMetaData("Name", QVariant(QString::fromStdString(cal->getCalendarName())));

            // Calendar version
            currCollection.setMetaData("Version", QVariant(QString::fromStdString(cal->getCalendarVersion())));

            // Available calendar types
            QList<QString> availableTypes = d->m_itemTransformer.calendarTypeMap().values();
            currCollection.setMetaData("Available types", QVariant(availableTypes));

            // Calendar type
            currCollection.setMetaData("Type", QVariant(
                    d->m_itemTransformer.fromCalendarType(cal->getCalendarType())));

            // Calendar tune
            currCollection.setMetaData("Tune", QVariant(QString::fromStdString(cal->getCalendarTune())));

            // Readonly
            currCollection.setMetaData("Readonly", QVariant(cal->IsReadOnly()));

            // Visible
            currCollection.setMetaData("Visible", QVariant(cal->IsShown()));

            // Free calendar
            cleanupCal(cal);
            cal = 0;

            // Append to result
            retn << currCollection;
        }
    }

    return retn;
}

bool QOrganizerItemMaemo5Engine::saveCollection(QOrganizerCollection* collection, QOrganizerItemManager::Error* error)
{
    if (!collection) {
        *error = QOrganizerItemManager::BadArgumentError;
        return false;
    }

    if (collection->id().managerUri() != managerUri())
    {
        if (collection->id().localId() != 0) {
            // Do not allow to save other manager's collection with local id set
            *error = QOrganizerItemManager::BadArgumentError;
            return false;
        }
    }

    QList<QOrganizerCollectionLocalId> collIds = collectionIds(error);
    if (*error != QOrganizerItemManager::NoError)
        return false;

    QString calName = ""; // default value
    QVariant tmpData = collection->metaData("Name");
    if (tmpData.isValid())
        calName = tmpData.toString();

    CalendarColour calColor = COLOUR_NEXT_FREE; // default value
    tmpData = collection->metaData("Color");
    if (tmpData.isValid())
        calColor = d->m_itemTransformer.toCalendarColour(tmpData.toString());

    bool calReadOnly = false; // default value
    tmpData = collection->metaData("Readonly");
    if (tmpData.isValid())
        calReadOnly = tmpData.toBool();

    bool calVisible = true; // default value
    tmpData = collection->metaData("Visible");
    if (tmpData.isValid())
        calVisible = tmpData.toBool();

    CalendarType calType = LOCAL_CALENDAR; // default value
    tmpData = collection->metaData("Type");
    if (tmpData.isValid())
        calType = d->m_itemTransformer.toCalendarType(tmpData.toString());

    QString calTune = ""; // default value
    tmpData = collection->metaData("Tune");
    if (tmpData.isValid())
        calTune = tmpData.toString();

    QString calVersion = "1.0"; // default value
    tmpData = collection->metaData("Version");
    if (tmpData.isValid())
        calVersion = tmpData.toString();

    int calError = CALENDAR_OPERATION_SUCCESSFUL;

    if (!collIds.contains(collection->id().localId())) {
        // New calendar

        // Can't save two calendars with same name
        int calExistStatus = CALENDAR_OPERATION_SUCCESSFUL;
        CCalendar *oldCalendar = d->m_mcInstance->getCalendarByName(
                calName.toStdString(), calExistStatus);
        if (oldCalendar) {
            delete oldCalendar;
            oldCalendar = 0;
        }
        if (calExistStatus != CALENDAR_DOESNOT_EXISTS) {
            *error = QOrganizerItemManager::AlreadyExistsError;
            return false;
        }

        // Ok, create a new calendar
        CCalendar *newCalendar = d->m_mcInstance->addCalendar(
                calName.toStdString(),
                calColor,
                calReadOnly ? 1:0,
                calVisible ? 1:0,
                calType,
                calTune.toStdString(),
                calVersion.toStdString(),
                calError);

        if (!newCalendar) {
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError)
                *error = QOrganizerItemManager::UnspecifiedError;
            return false;
        }
        if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
            delete newCalendar;
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            return false;
        }

        // Build collection id
        QOrganizerCollectionId collId;
        collId.setManagerUri(managerUri());
        collId.setLocalId(static_cast<QOrganizerCollectionLocalId>(newCalendar->getCalendarId()));

        // Set collection id
        collection->setId(collId);

        // Free calendar
        delete newCalendar;
    }
    else {
        // Modify calendar

        bool ok = d->m_mcInstance->modifyCalendar(
                static_cast<int>(collection->id().localId()),
                calName.toStdString(),
                calColor,
                calReadOnly ? 1 : 0,
                calVisible ? 1 : 0,
                calType,
                calTune.toStdString(),
                calVersion.toStdString(),
                calError);

        if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            return false;
        }
        else if (!ok) {
            *error = QOrganizerItemManager::UnspecifiedError;
            return false;
        }
    }

    return true;
}

bool QOrganizerItemMaemo5Engine::removeCollection(const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error)
{
    int calError = CALENDAR_OPERATION_SUCCESSFUL;
    bool ok = d->m_mcInstance->deleteCalendar(static_cast<int>(collectionId), calError);
    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    return ok;
}

bool QOrganizerItemMaemo5Engine::startRequest(QOrganizerItemAbstractRequest* req)
{
    d->m_asynchProcess->addRequest(req);
    QMetaObject::invokeMethod(d->m_asynchProcess, "processRequest", Qt::QueuedConnection);
    return true; // TODO: Is this ok?
}

bool QOrganizerItemMaemo5Engine::cancelRequest(QOrganizerItemAbstractRequest* req)
{
    return d->m_asynchProcess->cancelRequest(req);
}

bool QOrganizerItemMaemo5Engine::waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs)
{
    return d->m_asynchProcess->waitForRequestFinished(req, msecs);
}

void QOrganizerItemMaemo5Engine::requestDestroyed(QOrganizerItemAbstractRequest* req)
{
    return d->m_asynchProcess->requestDestroyed(req);
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
    QStringList retn;

    retn << QOrganizerItemType::TypeEvent;
    retn << QOrganizerItemType::TypeEventOccurrence;
    retn << QOrganizerItemType::TypeJournal;
    retn << QOrganizerItemType::TypeTodo;
    retn << QOrganizerItemType::TypeTodoOccurrence;

    return retn;
}

void QOrganizerItemMaemo5Engine::checkItemIdValidity(CCalendar *cal, QOrganizerItem *checkItem, QOrganizerItemManager::Error *error)
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

    // TODO: Maybe here should be added a GUID check: if item is NOT an occurrence, setting duplicate GUIDs should be disallowed

    // If the item is not an occurrence, no more checks are needed
    if (!eventOccurrence && !todoOccurrence)
        return;

    // Event occurrence validity checks:
    if (eventOccurrence) {
        // Original date must be set
        if (eventOccurrence->originalDate() == QDate()) {
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return;
        }

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
        else {
            // The parent ID was not set, so the GUID is always set here.
            // Must find at least one event with the given GUID.
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            ////std::vector<CEvent*> eventsWithGuid = cal->getEvents(eventOccurrence->guid().toStdString(), calError);
            std::vector<CEvent*> eventsWithGuid = d->m_databaseAccess.getEvents(cal->getCalendarId(), eventOccurrence->guid().toStdString(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            int numberOfEvents = eventsWithGuid.size();
            for (int i = 0; i < numberOfEvents; ++i)
                delete eventsWithGuid[i];

            if (*error != QOrganizerItemManager::NoError)
                return;

            if (!numberOfEvents) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }

        // If both parent ID and GUID are set, they must be consistent
        if (eventOccurrence->parentLocalId() && !eventOccurrence->guid().isEmpty()) {
            if (eventOccurrence->guid() != parent.guid()) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }
    }

    // Todo occurrence validity checks:
    if (todoOccurrence) {
        // Original date must be set
        if (todoOccurrence->originalDate() == QDate()) {
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return;
        }

        // Either parent id or GUID (or both) must be set
        if (!todoOccurrence->parentLocalId() && todoOccurrence->guid().isEmpty()) {
            *error = QOrganizerItemManager::InvalidOccurrenceError;
            return;
        }

        if (todoOccurrence->parentLocalId()) {
            // Parent ID must point to this occurrence due to restrictions in Maemo5 todos
            if (todoOccurrence->parentLocalId() != todoOccurrence->localId()) {
                *error = QOrganizerItemManager::InvalidOccurrenceError;
                return;
            }
        }
        else {
            // Must find at least one todo with the GUID from the DB
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            ////std::vector<CTodo*> todosWithGuid = cal->getTodos(todoOccurrence->guid().toStdString(), calError);
            std::vector<CTodo*> todosWithGuid = d->m_databaseAccess.getTodos(cal->getCalendarId(), todoOccurrence->guid().toStdString(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            int numberOfTodos = todosWithGuid.size();
            for (int i = 0; i < numberOfTodos; ++i)
                delete todosWithGuid[i];

            if (*error != QOrganizerItemManager::NoError)
                return;

            if (!numberOfTodos) {
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
    checkItemIdValidity(cal, item, error);
    if (*error != QOrganizerItemManager::NoError)
        return calError;

    // Returns InvalidItemTypeError if the type won't be recognized later
    *error = QOrganizerItemManager::InvalidItemTypeError;

    CComponent *component = d->m_itemTransformer.createCComponent(cal, item, error);
    if (!component || *error != QOrganizerItemManager::NoError) {
        delete component;
        return calError;
    }

    // Read the given GUID
    item->setGuid(QString::fromStdString(component->getGUid()));

    // TODO: The custom detail fields should be iterated and the corresponding
    // fields should be set to CComponent for saving.

    if (item->type() == QOrganizerItemType::TypeEvent) {
        CEvent* cevent = static_cast<CEvent *>(component);
        QString ceventId = QString::fromStdString(cevent->getId());
        if (!ceventId.isEmpty()) {
            // CEvent ID is not empty, the event already exists in calendar

            // increase the sequence number
            cevent->setSequence(cevent->getSequence() + 1);

            cal->modifyEvent(cevent, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError) {
                // Set alarm (must always be set only after the component is saved)
                d->m_itemTransformer.setAlarm(cal, item, component);

                cs.insertChangedItem(item->localId());
            }
        }
        else {
            // CEvent ID is empty, the event is new

            // set the sequence number zero
            cevent->setSequence(0);

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

                // Set alarm (must always be set only after the component is saved)
                d->m_itemTransformer.setAlarm(cal, item, component);

                // Update changeset
                if (calError == CALENDAR_ENTRY_DUPLICATED)
                    cs.insertChangedItem(item->localId());
                else
                    cs.insertAddedItem(item->localId());

                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error
            }
        }

        delete cevent;
        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeEventOccurrence)
    {
        QOrganizerEventOccurrence *eventOccurrence = static_cast<QOrganizerEventOccurrence *>(item);

        QOrganizerItem parentItem = parentOf(cal, eventOccurrence, error);
        if (*error == QOrganizerItemManager::NoError) {
            if (parentItem.localId()) {
                // parent item found, cast it to event
                QOrganizerEvent *parentEvent = static_cast<QOrganizerEvent *>(&parentItem);

                // save the event occurrence
                calError = saveEventOccurrence(cal, eventOccurrence, parentEvent, cs, error);
                *error = d->m_itemTransformer.calErrorToManagerError(calError);
            }
            else {
                // Event occurrence without a parent item fails,
                // but this should not happen after passing the validity check...
                *error = QOrganizerItemManager::InvalidOccurrenceError;
            }
        }

        return calError;
    }
    else if (item->type() == QOrganizerItemType::TypeTodo || item->type() == QOrganizerItemType::TypeTodoOccurrence) {
        CTodo* ctodo = static_cast<CTodo *>(component);
        QString ctodoId = QString::fromStdString(ctodo->getId());

        // A todo occurrence can never be a new item. If local id is empty, set it according
        // to the GUID.
        if (item->type() == QOrganizerItemType::TypeTodoOccurrence && ctodoId.isEmpty()) {
            calError = CALENDAR_OPERATION_SUCCESSFUL;
            ////std::vector<CTodo*> todosWithGuid = cal->getTodos(item->guid().toStdString(), calError);
            std::vector<CTodo*> todosWithGuid = d->m_databaseAccess.getTodos(cal->getCalendarId(), item->guid().toStdString(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
                delete ctodo;
                return calError;
            }
            ctodo->setId(todosWithGuid[0]->getId()); // this is safe as validity check has passed
            ctodoId = QString::fromStdString(ctodo->getId());
            int count = todosWithGuid.size();
            for (int i = 0; i < count; ++i)
                delete todosWithGuid[i];
        }

        if (!ctodoId.isEmpty()) {
            // CTodo ID is not empty, the todo already exists in calendar

            // increase the sequence number
            ctodo->setSequence(ctodo->getSequence() + 1);

            cal->modifyTodo(ctodo, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError) {
                // Set alarm (must always be set only after the component is saved)
                d->m_itemTransformer.setAlarm(cal, item, component);

                cs.insertChangedItem(item->localId());
            }
        }
        else {
            // CTodo ID is empty, the todo is new

            // set the sequence number zero
            ctodo->setSequence(0);

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

                // Set alarm (must always be set only after the component is saved)
                d->m_itemTransformer.setAlarm(cal, item, component);

                // Update changeset
                if (calError == CALENDAR_ENTRY_DUPLICATED)
                    cs.insertChangedItem(item->localId());
                else
                    cs.insertAddedItem(item->localId());

                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error
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

            // increase the sequence number
            cjournal->setSequence(cjournal->getSequence() + 1);

            cal->modifyJournal(cjournal, calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            if (*error == QOrganizerItemManager::NoError) {
                // Set alarm (must always be set only after the component is saved)
                d->m_itemTransformer.setAlarm(cal, item, component);

                cs.insertChangedItem(item->localId());
            }
        }
        else {
            // CJournal ID is empty, the journal is new

            // set the sequence number zero
            cjournal->setSequence(0);

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

                // Set alarm (must always be set only after the component is saved)
                d->m_itemTransformer.setAlarm(cal, item, component);

                // Update changeset
                if (calError == CALENDAR_ENTRY_DUPLICATED)
                    cs.insertChangedItem(item->localId());
                else
                    cs.insertAddedItem(item->localId());

                calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
                *error = QOrganizerItemManager::NoError; // reset the error
            }
        }

        delete cjournal;
        return calError;
    }

    return calError;
}

int QOrganizerItemMaemo5Engine::saveEventOccurrence(CCalendar *cal, QOrganizerEventOccurrence *occurrence, QOrganizerEvent *parent, QOrganizerItemChangeSet &cs, QOrganizerItemManager::Error *error)
{
    int calError = CALENDAR_OPERATION_SUCCESSFUL;

    // set occurrence GUID equal to the parent GUID
    occurrence->setGuid(parent->guid());

    // set occurrence's parent id
    occurrence->setParentLocalId(parent->localId());

    // backup the parent item
    QOrganizerEvent parentBackup(*parent);

    // did parent become modified
    bool parentModified = false;

    if (!occurrence->localId()) {
        // Does not contain a local id => this is a generated occurrence or a new exception occurrence
        // After modifications this won't be saved anymore as a generated occurrence
        // but as an event to the calendar DB. Add an exception rule to the parent item
        // to leave out this one when generating occurrences.

        if (occurrence->originalDate().isValid() && occurrence->originalDate() != parent->startDateTime().date()) {
            // The original date is valid => this is a generated occurrence
            // First solve the original occurrence, using this occurrence's original date.
            // We need that to know the original length of the generated occurrence
            // (as it may have changed now).
            QDateTime originalPeriodStart = QDateTime(occurrence->originalDate(), QTime(0,0,0));
            QDateTime originalPeriodEnd = QDateTime(occurrence->originalDate(), QTime(23,59,59,999));
            QList<QOrganizerItem> parentsOccurrences = itemInstances(*parent, originalPeriodStart, originalPeriodEnd, 0, error);

            if (!parentsOccurrences.isEmpty() && occurrence->originalDate() != parent->startDateTime().date()) {
                QOrganizerEventOccurrence originalOccurrence = static_cast<QOrganizerEventOccurrence>(parentsOccurrences[0]);

                // Add exception dates to the parent
                QDate periodStart = originalOccurrence.startDateTime().date();
                QDate periodEnd = originalOccurrence.endDateTime().date();

                QList<QDate> newExceptionDates = parent->exceptionDates();
                QDate exceptionDate = periodStart;
                while (exceptionDate <= periodEnd) {
                    if (newExceptionDates.indexOf(exceptionDate) == -1)
                        newExceptionDates << exceptionDate;
                    exceptionDate = exceptionDate.addDays(1);
                }
                parent->setExceptionDates(newExceptionDates);
                parentModified = true;
            }
        }
        else {
            // The original date is not valid => this is a new exception occurrence, not generated
            // Add exception days covering the occurrence's period
            QDate periodStart = occurrence->startDateTime().date();
            QDate periodEnd = occurrence->endDateTime().date();

            QList<QDate> newExceptionDates = parent->exceptionDates();
            QDate exceptionDate = periodStart;
            while (exceptionDate <= periodEnd) {
                if (newExceptionDates.indexOf(exceptionDate) == -1)
                    newExceptionDates << exceptionDate;
                exceptionDate = exceptionDate.addDays(1);
            }
            parent->setExceptionDates(newExceptionDates);
            parentModified = true;
        }
    }

    // Create a CComponent from occurrence
    // This is done before saving the parent, so we don't have to rollback
    // the parent changes if the component creation fails.
    calError = CALENDAR_OPERATION_SUCCESSFUL;
    CComponent *component = d->m_itemTransformer.createCComponent(cal, occurrence, error);
    if (!component) {
        *error = QOrganizerItemManager::UnspecifiedError;
        return calError;
    }
    if (*error != QOrganizerItemManager::NoError) {
        delete component;
        return calError;
    }
    // TODO: The custom detail fields should be iterated and the corresponding
    // fields should be set to CComponent for saving.

    // save the parent back to the DB (if needed)
    if (parentModified) {
        calError = doSaveItem(cal, parent, cs, error);

        if (calError != CALENDAR_OPERATION_SUCCESSFUL) {
            // saving the parent modifications failed
            // we must not save the occurrence either
            delete component;
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            return calError;
        }
        if (*error != QOrganizerItemManager::NoError) {
            // saving the parent modifications failed
            // we must not save the occurrence either
            delete component;
            return calError;
        }
    }

    // Save occurrence
    CEvent* cevent = static_cast<CEvent *>(component);
    QString ceventId = QString::fromStdString(cevent->getId());
    if (!ceventId.isEmpty()) {
        // CEvent ID is not empty, the event already exists in calendar

        // increase the sequence number
        cevent->setSequence(cevent->getSequence() + 1);

        cal->modifyEvent(cevent, calError);
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (calError != CALENDAR_OPERATION_SUCCESSFUL && parentModified) {
            // occurrence saving failed, we must undo the parent changes too
            QOrganizerItemManager::Error ignoreError;
            doSaveItem(cal, &parentBackup, cs, &ignoreError);
        }
    }
    else {
        // CEvent ID is empty, the event is new

        // set the sequence number zero
        cevent->setSequence(0);

        cal->addEvent(cevent, calError);
        *error = d->m_itemTransformer.calErrorToManagerError(calError);

        if (calError == CALENDAR_OPERATION_SUCCESSFUL || calError == CALENDAR_ENTRY_DUPLICATED)
        {
            // The Maemo5 calendar does not accept two items if all the
            // time details are equal. That's so even if the item IDs differ.
            // If the "new" item is actually a duplicate, then let
            // the calendar to modify the existing item.
            // TODO: Is that what we want?

            // Set id for the occurrence
            QString newIdString = QString::fromStdString(cevent->getId());
            QOrganizerItemLocalId newId = newIdString.toUInt();
            QOrganizerItemId id;
            id.setLocalId(newId);
            id.setManagerUri(managerUri());
            occurrence->setId(id);

            // Update changeset
            if (calError == CALENDAR_ENTRY_DUPLICATED)
                cs.insertChangedItem(occurrence->localId());
            else
                cs.insertAddedItem(occurrence->localId());

            calError = CALENDAR_OPERATION_SUCCESSFUL; // reset the error
            *error = QOrganizerItemManager::NoError; // reset the error
        }
        else if (parentModified) {
            // occurrence saving failed, we must undo the parent changes too
            QOrganizerItemManager::Error ignoreError;
            doSaveItem(cal, &parentBackup, cs, &ignoreError);
        }
    }

    delete cevent;
    cevent = 0;

    return calError;
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
}

QOrganizerItem QOrganizerItemMaemo5Engine::parentOf(CCalendar *cal, QOrganizerItem *occurrence, QOrganizerItemManager::Error *error) const
{
    // the occurrence is supposed be valid when this method becomes called
    QOrganizerItemLocalId parentLocalId = 0;
    if (occurrence->type() == QOrganizerItemType::TypeEventOccurrence) {
        QOrganizerEventOccurrence *eventOccurrence = static_cast<QOrganizerEventOccurrence *>(occurrence);
        if (eventOccurrence->parentLocalId()) {
            // the simple case: parent local id was set
            parentLocalId = eventOccurrence->parentLocalId();
        }
        else {
            // parent local id was not set, fetch with [GUID,originalDate]
            int calError = CALENDAR_OPERATION_SUCCESSFUL;
            ////std::vector<CEvent*> parentCands = cal->getEvents(eventOccurrence->guid().toStdString(), calError);
            std::vector<CEvent*> parentCands = d->m_databaseAccess.getEvents(cal->getCalendarId(), eventOccurrence->guid().toStdString(), calError);
            *error = d->m_itemTransformer.calErrorToManagerError(calError);
            std::vector<CEvent*>::iterator pCand;

            // Check if some event contains recurrence information and then that the given
            // occurrence is one of the occurrences obtained from the recurrence information.
            for (pCand = parentCands.begin(); pCand != parentCands.end(); ++pCand) {
                QOrganizerItemLocalId candidateLocalId = QString::fromStdString((*pCand)->getId()).toUInt();

                QOrganizerItem parentCandidate = internalFetchItem(candidateLocalId, fetchMinimalData(), error, false);
                if (*error == QOrganizerItemManager::NoError) {
                    // parent candidate must be an event here as events were requested from the GUID mapper
                    QOrganizerEvent *parentCandidateEvent = static_cast<QOrganizerEvent *>(&parentCandidate);

                    if (!parentCandidateEvent->recurrenceDates().isEmpty()
                        || !parentCandidateEvent->recurrenceRules().isEmpty()) {

                        if (eventOccurrence->originalDate() == QDate()) {
                            // The given occurrence is a new one, not previously saved in the DB.
                            // If there is an event with recurrence information, it must be the parent.
                            parentLocalId = candidateLocalId;
                            break; // parent event found
                        }
                        else {
                            // The given occurrence has existed before.
                            // Get the occurrences of the parent candidate event. If there's a match
                            // to our occurrence's originalDate, then we have found the parent.
                            QDateTime periodStart = QDateTime(eventOccurrence->originalDate(), QTime(0,0,0));
                            QDateTime periodEnd = QDateTime(eventOccurrence->originalDate(), QTime(23,59,59,999));

                            QList<QOrganizerItem> parentCandidateOccurrences =
                                    itemInstances(*parentCandidateEvent, periodStart, periodEnd, 0, error);
                            if (*error != QOrganizerItemManager::NoError)
                                return QOrganizerItem(); // error occured

                            // Because the period was set, it's enough to just check if we got any
                            if (!parentCandidateOccurrences.isEmpty()) {
                                parentLocalId = candidateLocalId;
                                break; // parent event found
                            }
                        }
                    }
                }
                else if (*error == QOrganizerItemManager::DoesNotExistError) {
                    // Tried to get an item that does not exist.
                    // It is not a serious error here, so just reset the error.
                    *error = QOrganizerItemManager::NoError;
                }
            }

            if (parentLocalId)
            {
                // Parent local id got, just free the resources
                for (pCand = parentCands.begin(); pCand != parentCands.end(); ++pCand)
                    delete *pCand;
            }
            else {
                // None of the parent candidates contained recurrence information
                // => return the earliest created event as parent

                CEvent *firstCreated = 0;
                if (parentCands.size() > 0) {
                    firstCreated = parentCands[0];
                    for (pCand = parentCands.begin(); pCand != parentCands.end(); ++pCand) {
                        if ((*pCand)->getCreatedTime() < firstCreated->getCreatedTime()) {
                            firstCreated = *pCand;
                        }
                        else if((*pCand)->getCreatedTime() == firstCreated->getCreatedTime()
                            && (*pCand)->getId() < firstCreated->getId()) {
                            firstCreated = *pCand; // if two entries are created exactly at the same time, consider one with the smaller id as earlier
                        }
                    }
                }

                if (firstCreated) {
                    // the firstly created event resolved, return it as parent
                    parentLocalId = QString::fromStdString(firstCreated->getId()).toUInt();
                }

                // Free the resources
                for (pCand = parentCands.begin(); pCand != parentCands.end(); ++pCand)
                    delete *pCand;
            }
        }
    }
    else if (occurrence->type() == QOrganizerItemType::TypeTodoOccurrence) {
        // In Maemo5 parent of a todo occurrence is the occurrence itself
        parentLocalId = occurrence->localId();
    }
    else {
        return QOrganizerItem(); // other types can't have a parent
    }

    // Parent local id got, now fetch & return the parent
    return internalFetchItem(parentLocalId, fetchMinimalData(), error, false);
}

QOrganizerItem QOrganizerItemMaemo5Engine::internalFetchItem(const QOrganizerItemLocalId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error, bool fetchOccurrences) const
{
    Q_UNUSED(fetchHint);
    // TODO: Make this method to use the fetch hint

    const int EVENT_TYPE = 1;
    const int TODO_TYPE = 2;
    const int JOURNAL_TYPE = 3;

    CCalendar *cal = d->m_mcInstance->getDefaultCalendar();
    std::string nativeId = QString::number(itemId).toStdString();
    int calError = CALENDAR_OPERATION_SUCCESSFUL;

    CEvent *cevent = 0;
    if (d->m_databaseAccess.typeOf(itemId) == EVENT_TYPE)
        cevent = cal->getEvent(nativeId, calError);
    else
        calError = CALENDAR_DOESNOT_EXISTS;

    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (cevent) {
        // first resolve is this event or an event occurrence (exception event)
        bool isOcc = false;
        if (fetchOccurrences) {
            QString type = QOrganizerItemType::TypeEvent;
            isOcc = isOccurrence(cal, cevent, type, error);
            if (*error != QOrganizerItemManager::NoError) {
                delete cevent;
                cleanupCal(cal);
                return QOrganizerItem();
            }
        }

        if (!isOcc) {
            QOrganizerEvent retn = d->m_itemTransformer.convertCEventToQEvent(cevent);
            d->m_itemTransformer.fillInCommonCComponentDetails(&retn, cevent);
            delete cevent;
            cleanupCal(cal);
            return retn;
        }
        else {
            QOrganizerEventOccurrence retn = d->m_itemTransformer.convertCEventToQEventOccurrence(cevent);
            d->m_itemTransformer.fillInCommonCComponentDetails(&retn, cevent);

            // resolve and set the parent event
            QOrganizerItem parent = parentOf(cal, &retn, error);
            if (*error != QOrganizerItemManager::NoError) {
                delete cevent;
                cleanupCal(cal);
                return QOrganizerItem();
            }
            retn.setParentLocalId(parent.localId());

            delete cevent;
            cleanupCal(cal);
            return retn;
        }
    }

    CTodo *todo = 0;
    if (d->m_databaseAccess.typeOf(itemId) == TODO_TYPE)
        todo = cal->getTodo(nativeId, calError);
    else
        calError = CALENDAR_DOESNOT_EXISTS;

    *error = d->m_itemTransformer.calErrorToManagerError(calError);
    if (todo) {
        QOrganizerTodo retn = d->m_itemTransformer.convertCTodoToQTodo(todo);
        d->m_itemTransformer.fillInCommonCComponentDetails(&retn, todo);
        delete todo;
        cleanupCal(cal);
        return retn;
    }

    CJournal *journal = 0;
    if (d->m_databaseAccess.typeOf(itemId) == JOURNAL_TYPE)
        journal = cal->getJournal(nativeId, calError);
    else
        calError = CALENDAR_DOESNOT_EXISTS;

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

bool QOrganizerItemMaemo5Engine::isOccurrence(CCalendar *cal, CComponent *ccomponent, QString typeStr, QOrganizerItemManager::Error *error) const
{
    // checks if a native event or todo is actually an occurrence

    *error = QOrganizerItemManager::NoError;
    QString guid = QString::fromStdString(ccomponent->getGUid());
    if (guid.isEmpty())
        return false; // possible items with no GUID are never considered as occurrences

    if (typeStr == QOrganizerItemType::TypeEvent) {
        // if item contains recurrence information, it can't be an occurrence
        if (containsRecurrenceInformation(ccomponent))
            return false;

        // if no duplicate GUIDs are found, event can't be an occurrence
        int calError = CALENDAR_OPERATION_SUCCESSFUL;
        ////std::vector<CEvent*> eventsWithGuid = cal->getEvents(guid.toStdString(), calError);
        std::vector<CEvent*> eventsWithGuid = d->m_databaseAccess.getEvents(cal->getCalendarId(), guid.toStdString(), calError);
        int eventsWithGuidSize = eventsWithGuid.size();
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        if (eventsWithGuidSize < 2) {
            for(int i = 0; i < eventsWithGuidSize; ++i)
                delete eventsWithGuid[i];
            return false;
        }

        // Now check if any of the events with the equal GUID contains recurrence
        // information. If there's at least one such, then this item is an occurrence.
        QString ccomponentId = QString::fromStdString(ccomponent->getId());
        for (int i = 0; i < eventsWithGuidSize; ++i) {
            CEvent *currEvent = eventsWithGuid[i];
            if (currEvent && QString::fromStdString(currEvent->getId()) != ccomponentId) { // exclude itself
                if (containsRecurrenceInformation(currEvent)) {
                    for(int j = 0; j < eventsWithGuidSize; ++j)
                        delete eventsWithGuid[j];
                    return true;
                }
            }
        }

        // There are multiple events with equal GUID and no event contains recurrence information.
        // If this event is created first, it is consider as a parent, otherwise an occurrence.
        CEvent* firstCreated = eventsWithGuid[0]; // always contains >= 2 events here
        for (int i = 1; i < eventsWithGuidSize; ++i) {
            CEvent *currEvent = eventsWithGuid[i];
            if (currEvent->getCreatedTime() < firstCreated->getCreatedTime()) {
                firstCreated = currEvent;
            }
            else if (currEvent->getCreatedTime() == firstCreated->getCreatedTime()
                && currEvent->getId() < firstCreated->getId()) {
                firstCreated = currEvent; // if two entries are created exactly at the same time, consider one with the smaller id as earlier
            }
        }

        bool returnValue = (ccomponent->getId() != firstCreated->getId());
        for(int i = 0; i < eventsWithGuidSize; ++i)
            delete eventsWithGuid[i];

        return returnValue;
    }
    else if (typeStr == QOrganizerItemType::TypeTodo || typeStr == QOrganizerItemType::TypeJournal
             || typeStr == QOrganizerItemType::TypeNote) {
        return false; // these types never have occurrences saved in the DB
    }
    else {
        return true; // an occurrence type was given
    }
}

bool QOrganizerItemMaemo5Engine::isParent(CCalendar *cal, CComponent *ccomponent, QString typeStr, QOrganizerItemManager::Error *error) const
{
    *error = QOrganizerItemManager::NoError;
    QString guid = QString::fromStdString(ccomponent->getGUid());
    if (guid.isEmpty())
        return false; // possible items with no GUID are never parents

    if (typeStr == QOrganizerItemType::TypeEvent) {
        bool retValue = false;
        int calError = CALENDAR_OPERATION_SUCCESSFUL;
        ////std::vector<CEvent*> eventsWithGuid = cal->getEvents(guid.toStdString(), calError);
        std::vector<CEvent*> eventsWithGuid = d->m_databaseAccess.getEvents(cal->getCalendarId(), guid.toStdString(), calError);
        *error = d->m_itemTransformer.calErrorToManagerError(calError);
        int eventsWithGuidSize = eventsWithGuid.size();
        if (eventsWithGuidSize > 1) {
            // Multiple events with GUID found => if this one is not an occurrence
            // then it has to be a parent
            bool isOcc = isOccurrence(cal, ccomponent, QOrganizerItemType::TypeEvent, error);
            if (!isOcc && *error == QOrganizerItemManager::NoError)
                retValue = true;
        }

        // Free the resources
        for (int i = 0; i < eventsWithGuidSize; ++i)
            delete eventsWithGuid[i];

        return retValue;
    }
    else {
        return false;  // these types never have occurrences saved in the DB
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
