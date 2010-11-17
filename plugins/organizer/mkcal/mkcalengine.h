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

#ifndef MKCALENGINE_H
#define MKCALENGINE_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QSharedData>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QQueue>
#include <QPair>
#include <QSet>
#include <QDateTime>
#include <QString>
#include <QObject>
#include <QMutex>

#include <extendedcalendar.h>
#include <extendedstorage.h>
#include <notebook.h>

#include "qorganizeritem.h"
#include "qorganizermanager.h"
#include "qorganizermanagerengine.h"
#include "qorganizermanagerenginefactory.h"
#include "qorganizeritemdetaildefinition.h"
#include "qorganizerabstractrequest.h"
#include "qorganizeritemchangeset.h"

QTM_BEGIN_NAMESPACE
class QOrganizerEvent;
class QOrganizerTodo;
class QOrganizerNote;
class QOrganizerJournal;
class QOrganizerItemRecurrence;
class QOrganizerRecurrenceRule;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class OrganizerAsynchManager; // helper class to process async requests

class MKCalEngineFactory : public QObject, public QOrganizerManagerEngineFactory
{
  Q_OBJECT
  Q_INTERFACES(QtMobility::QOrganizerManagerEngineFactory)
  public:
    QOrganizerManagerEngine* engine(const QMap<QString, QString>& parameters, QOrganizerManager::Error*);
    QString managerName() const;
    QOrganizerItemEngineId* createItemEngineId(const QMap<QString, QString>& parameters, const QString& engineIdString) const;
    QOrganizerCollectionEngineId* createCollectionEngineId(const QMap<QString, QString>& parameters, const QString& engineIdString) const;
};

class MKCalEngineData : public QSharedData
{
public:
    MKCalEngineData()
        : QSharedData(),
        m_calendarBackendPtr(new mKCal::ExtendedCalendar(KDateTime::Spec::LocalZone())),
        m_storagePtr(mKCal::ExtendedCalendar::defaultStorage(m_calendarBackendPtr)),
        m_operationMutex(QMutex::Recursive)
    {
        m_storagePtr->open();
        m_storagePtr->load();
        mKCal::Notebook::Ptr defaultNotebook = m_storagePtr->defaultNotebook();
        if (!defaultNotebook)
            m_storagePtr->createDefaultNotebook("defaultNotebook");
    }

    ~MKCalEngineData()
    {
    }

    // map of organizeritem type to map of definition name to definitions:
    mutable QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > m_definitions;

    mKCal::ExtendedCalendar::Ptr m_calendarBackendPtr;
    mKCal::ExtendedStorage::Ptr m_storagePtr;

    // asynchronous request handler instance
    OrganizerAsynchManager *m_asynchProcess;

    QString m_managerUri;

    mutable QMutex m_operationMutex;
};

class MKCalEngine : public QOrganizerManagerEngine, public mKCal::ExtendedStorageObserver
{
    Q_OBJECT

public:
    ~MKCalEngine();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    int managerVersion() const;

    QList<QOrganizerItem> itemOccurrences(const QOrganizerItem& parentItem, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;
    QList<QOrganizerItemId> itemIds(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerManager::Error* error) const;
    QList<QOrganizerItem> itemsForExport(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;
    QList<QOrganizerItem> items(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;
    QOrganizerItem item(const QOrganizerItemId& itemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;

    bool saveItems(QList<QOrganizerItem>* items, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error);
    bool saveItem(QOrganizerItem* item, QOrganizerManager::Error* error);
    bool removeItems(const QList<QOrganizerItemId>& itemIds, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(const QString& itemType, QOrganizerManager::Error* error) const;
    QOrganizerItemDetailDefinition detailDefinition(const QString& definitionId, const QString& itemType, QOrganizerManager::Error* error) const;
    bool saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& itemType, QOrganizerManager::Error* error);
    bool removeDetailDefinition(const QString& definitionId, const QString& itemType, QOrganizerManager::Error* error);

    /* Collections - every item belongs to exactly one collection */
    QOrganizerCollection defaultCollection(QOrganizerManager::Error* error) const;
    QOrganizerCollection collection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error) const;
    QList<QOrganizerCollection> collections(QOrganizerManager::Error* error) const;
    bool saveCollection(QOrganizerCollection* collection, QOrganizerManager::Error* error);
    bool removeCollection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error);

    /* Capabilities reporting */
    bool hasFeature(QOrganizerManager::ManagerFeature feature, const QString& itemType) const;
    bool isFilterSupported(const QOrganizerItemFilter& filter) const;
    QList<int> supportedDataTypes() const;
    QStringList supportedItemTypes() const;

    /* Asynchronous Request Support */
    void requestDestroyed(QOrganizerAbstractRequest* req);
    bool startRequest(QOrganizerAbstractRequest* req);
    bool cancelRequest(QOrganizerAbstractRequest* req);
    bool waitForRequestFinished(QOrganizerAbstractRequest* req, int msecs);

    /* mKCal Change Notification Support (ExtendedStorageObserver) */
    void storageModified(mKCal::ExtendedStorage* storage, const QString& info);
    void storageProgress(mKCal::ExtendedStorage* storage, const QString& info);
    void storageFinished(mKCal::ExtendedStorage* storage, bool error, const QString& info);

private:
    enum IncludedOccurrences {
        OnlyGeneratedOccurrences,
        GeneratedAndPersistentOccurrences
    };

    MKCalEngine(const QString& managerUri = QString());
    QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > schemaDefinitions() const;
    KCalCore::Incidence::Ptr incidence(const QOrganizerItemId& itemId) const;
    KCalCore::Incidence::Ptr detachedIncidenceFromItem(const QOrganizerItem& item) const;
    bool internalSaveItem(QOrganizerItemChangeSet* ics, QOrganizerItem* item, QOrganizerManager::Error* error);
    KCalCore::Incidence::Ptr softSaveItem(QOrganizerItemChangeSet* ics, QOrganizerItem* item, QOrganizerManager::Error* error);
    void convertQEventToKEvent(const QOrganizerItem& item, KCalCore::Incidence::Ptr incidence, bool recurs);
    void convertQTodoToKTodo(const QOrganizerItem& item, KCalCore::Incidence::Ptr incidence, bool recurs);
    void convertQJournalToKJournal(const QOrganizerItem& item, KCalCore::Incidence::Ptr incidence);
    void convertQNoteToKNote(const QOrganizerItem& item, KCalCore::Incidence::Ptr incidence);
    void convertCommonDetailsToIncidenceFields(const QOrganizerItem& item, KCalCore::Incidence::Ptr incidence);
    void convertQRecurrenceToKRecurrence(
            const QOrganizerItemRecurrence& qRecurrence,
            const QDate& startDate,
            KCalCore::Recurrence* kRecurrence);
    KCalCore::RecurrenceRule* createKRecurrenceRule(
            KCalCore::Recurrence* kRecurrence,
            const QDate& startDate,
            const QOrganizerRecurrenceRule& qRRule);

    bool convertIncidenceToItem(KCalCore::Incidence::Ptr i, QOrganizerItem* item) const;
    void convertKEventToQEvent(KCalCore::Event::Ptr e, QOrganizerItem* item) const;
    void convertKTodoToQTodo(KCalCore::Todo::Ptr t, QOrganizerItem* item) const;
    void convertKJournalToQJournal(KCalCore::Journal::Ptr j, QOrganizerItem* item) const;
    void convertCommonIncidenceFieldsToDetails(KCalCore::Incidence::Ptr i, QOrganizerItem* item) const;
    void convertKRecurrenceToQRecurrence(const KCalCore::Recurrence* kRecurrence, QOrganizerItem* item) const;
    QOrganizerRecurrenceRule createQRecurrenceRule(const KCalCore::RecurrenceRule* kRRule) const;

    QOrganizerCollection convertNotebookToCollection(mKCal::Notebook::Ptr notebook) const;
    void convertCollectionToNotebook(const QOrganizerCollection& collection, mKCal::Notebook::Ptr notebook) const;

    QList<QOrganizerItem> internalItemOccurrences(const QOrganizerItem& parentItem, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error, IncludedOccurrences includedOccurrences = OnlyGeneratedOccurrences) const;
    QList<QOrganizerItem> internalItems(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error, bool expand) const;
    bool itemHasRecurringChildInInterval(KCalCore::Incidence::Ptr incidence, QOrganizerItem generator, QDateTime startDate, QDateTime endDate, QOrganizerItemFilter filter) const;

    MKCalEngineData* d;

    friend class MKCalEngineFactory;
};

#endif

