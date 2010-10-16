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

#ifndef QORGANIZERMAEMO5_P_H
#define QORGANIZERMAEMO5_P_H

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
#include <QTimer>

#include "qorganizeritem.h"
#include "qorganizermanager.h"
#include "qorganizermanagerengine.h"
#include "qorganizermanagerenginefactory.h"
#include "qorganizeritemengineid.h"
#include "qorganizercollectionengineid.h"
#include "qorganizeritemdetaildefinition.h"
#include "qorganizerabstractrequest.h"
#include "qorganizeritemchangeset.h"

#include "qorganizeritemtransform.h"
#include "qorganizerasynchprocess.h"
#include "qorganizercaldbaccess.h"

#include "qorganizerjournal.h"
#include "qorganizertodo.h"
#include "qorganizerevent.h"

#include <CMulticalendar.h>
#include <CEvent.h>
#include <CTodo.h>
#include <CJournal.h>
#include <CRecurrence.h>


class QOrganizerItemMaemo5Factory : public QObject, public QOrganizerManagerEngineFactory
{
  Q_OBJECT
  Q_INTERFACES(QtMobility::QOrganizerManagerEngineFactory)
  public:
    QOrganizerManagerEngine* engine(const QMap<QString, QString>& parameters, QOrganizerManager::Error*);
    QOrganizerItemEngineId* createItemEngineId(const QMap<QString, QString>& parameters, const QString& idString) const;
    QOrganizerCollectionEngineId* createCollectionEngineId(const QMap<QString, QString>& parameters, const QString& idString) const;
    QString managerName() const;
};

class QOrganizerItemMaemo5EngineData : public QSharedData
{
public:
    QOrganizerItemMaemo5EngineData()
        : QSharedData(),
        m_mcInstance(CMulticalendar::MCInstance()) // XXX TODO: sharing?
    {
    }

    QOrganizerItemMaemo5EngineData(const QOrganizerItemMaemo5EngineData& other)
        : QSharedData(other),
        m_mcInstance(other.m_mcInstance)
    {
    }

    ~QOrganizerItemMaemo5EngineData()
    {
        if (m_mcInstance)
            delete m_mcInstance;
    }

    // the multicalendar instance
    CMulticalendar *m_mcInstance;

    // item converter instance
    OrganizerItemTransform m_itemTransformer;

    // asynchronous request handler instance
    OrganizerAsynchProcess *m_asynchProcess;

    // calendar database accessor instance
    OrganizerCalendarDatabaseAccess* m_dbAccess;

    // calendar database cache instance
    OrganizerDbCache* m_dbCache;
};

class QOrganizerItemMaemo5Engine : public QOrganizerManagerEngine
{
    Q_OBJECT

public:
    ~QOrganizerItemMaemo5Engine();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    int managerVersion() const;

    QList<QOrganizerItem> itemOccurrences(const QOrganizerItem& parentItem, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;
    QList<QOrganizerItemId> itemIds(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, QOrganizerManager::Error *error) const;

    QList<QOrganizerItem> items(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, const QOrganizerItemFetchHint &fetchHint, QOrganizerManager::Error *error) const;
    QList<QOrganizerItem> itemsForExport(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, const QOrganizerItemFetchHint &fetchHint, QOrganizerManager::Error *error) const;
    QOrganizerItem item(const QOrganizerItemId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerManager::Error *error) const;

    bool saveItems(QList<QOrganizerItem>* items, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error);
    bool removeItems(const QList<QOrganizerItemId> &itemIds, QMap<int, QOrganizerManager::Error> *errorMap, QOrganizerManager::Error *error);

    /* Collections - every item belongs to exactly one collection */
    QOrganizerCollection defaultCollection(QOrganizerManager::Error* error) const;
    QOrganizerCollection collection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error) const;
    QList<QOrganizerCollection> collections(QOrganizerManager::Error* error) const;
    bool saveCollection(QOrganizerCollection* collection, QOrganizerManager::Error* error);
    bool removeCollection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(const QString& itemType, QOrganizerManager::Error* error) const;

    /* Capabilities reporting */
    bool hasFeature(QOrganizerManager::ManagerFeature feature, const QString &itemType) const;
    bool isFilterSupported(const QOrganizerItemFilter &filter) const;
    QList<int> supportedDataTypes() const;
    QStringList supportedItemTypes() const;

    /* Asynchronous Request Support */
    void requestDestroyed(QOrganizerAbstractRequest *req);
    bool startRequest(QOrganizerAbstractRequest *req);
    bool cancelRequest(QOrganizerAbstractRequest *req);
    bool waitForRequestFinished(QOrganizerAbstractRequest *req, int msecs);

public Q_SLOTS:
    void dataChanged();

private:
    QList<QOrganizerItem> internalItemOccurrences(const QOrganizerItem &parentItem, const QDateTime &periodStart, const QDateTime &periodEnd, int maxCount, QOrganizerManager::Error *error) const;
    QList<QOrganizerItemId> internalItemIds(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, QOrganizerManager::Error *error) const;

    QList<QOrganizerItem> internalItems(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, const QOrganizerItemFetchHint &fetchHint, bool forExport, QOrganizerManager::Error *error) const;
    void internalAddOccurances(QList<QOrganizerItem>* sorted, QOrganizerItem& item, const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, bool forExport, QOrganizerManager::Error *error) const;
    QOrganizerItem internalItem(const QOrganizerItemId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerManager::Error *error) const;

    bool internalSaveItems(QList<QOrganizerItem>* items, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error);
    bool internalRemoveItems(const QList<QOrganizerItemId> &itemIds, QMap<int, QOrganizerManager::Error> *errorMap, QOrganizerManager::Error *error);

    /* Collections - every item belongs to exactly one collection */
    QOrganizerCollection internalDefaultCollection(QOrganizerManager::Error* error) const;
    QList<QOrganizerCollection> internalCollections(QOrganizerManager::Error* error) const;
    QOrganizerCollection internalCollection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error) const;
    bool internalSaveCollection(QOrganizerCollection* collection, QOrganizerManager::Error* error);
    bool internalRemoveCollection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error);
    // helper function
    QList<QOrganizerCollectionId> internalCollectionIds(QOrganizerManager::Error* error) const;

    // single item saving implementation
    void checkItemIdValidity(QOrganizerItem *checkItem, QOrganizerManager::Error *error);
    int doSaveItem(CCalendar *cal, QOrganizerItem *item, QOrganizerItemChangeSet &cs, QOrganizerManager::Error *error);

    // saving the occurrences
    int saveEventOccurrence(CCalendar *cal, QOrganizerEventOccurrence *occurrence, QOrganizerEvent *parent, QOrganizerItemChangeSet &cs, QOrganizerManager::Error *error);
    void insertOccurenceSortedByStartDate(QOrganizerItem *occurrence, QList<QOrganizerItem> &target) const;

    // getting the parent (base) item of an exception item (event occurrence or todo occurrence)
    QOrganizerItem parentOf(CCalendar *cal, QOrganizerItem *occurence, QOrganizerManager::Error *error) const;

    // internal fetch item
    QOrganizerItem internalFetchItem(const QOrganizerItemId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerManager::Error *error, bool fetchOccurrences) const;

    // identifying native item as an occurrence
    bool isOccurrence(CCalendar *cal, CComponent *ccomponent, QString typeStr, QOrganizerManager::Error *error) const;
    bool isParent(CCalendar *cal, CComponent *ccomponent, QString typeStr, QOrganizerManager::Error *error) const;
    bool containsRecurrenceInformation(CComponent *ccomponent) const;

    // returns a fetch hint for fetching minimal amount of information
    QOrganizerItemFetchHint fetchMinimalData() const;

    // calendar instance deletion helper
    void cleanupCal(CCalendar *cal) const;

    // get calendar
    CCalendar* getCalendar(QOrganizerCollectionId collectionId, QOrganizerManager::Error *error) const;

    // extract possible collection ids from the filters
    QSet<QOrganizerCollectionId> extractCollectionIds(const QOrganizerItemFilter& filter) const;
    QSet<QOrganizerCollectionId> extractCollectionLocalIds(const QOrganizerItemFilter& filter) const;

    // ctor
    QOrganizerItemMaemo5Engine();

private:
    QOrganizerItemMaemo5EngineData *d;
    QTimer m_waitTimer;
    mutable QMutex m_operationMutex;
    mutable QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > m_definitions;
    friend class QOrganizerItemMaemo5Factory;
};

#endif
