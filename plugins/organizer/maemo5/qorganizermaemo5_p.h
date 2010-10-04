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
#include "qorganizeritemmanager.h"
#include "qorganizeritemmanagerengine.h"
#include "qorganizeritemmanagerenginefactory.h"
#include "qorganizeritemenginelocalid.h"
#include "qorganizercollectionenginelocalid.h"
#include "qorganizeritemdetaildefinition.h"
#include "qorganizeritemabstractrequest.h"
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


class QOrganizerItemMaemo5Factory : public QObject, public QOrganizerItemManagerEngineFactory
{
  Q_OBJECT
  Q_INTERFACES(QtMobility::QOrganizerItemManagerEngineFactory)
  public:
    QOrganizerItemManagerEngine* engine(const QMap<QString, QString>& parameters, QOrganizerItemManager::Error*);
    QOrganizerItemEngineLocalId* createItemEngineLocalId() const;
    QOrganizerCollectionEngineLocalId* createCollectionEngineLocalId() const;
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

class QOrganizerItemMaemo5Engine : public QOrganizerItemManagerEngine
{
    Q_OBJECT

public:
    ~QOrganizerItemMaemo5Engine();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    int managerVersion() const;

    QList<QOrganizerItem> itemInstances(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const;
    QList<QOrganizerItem> itemInstances(const QOrganizerItem &generator, const QDateTime &periodStart, const QDateTime &periodEnd, int maxCount, QOrganizerItemManager::Error *error) const;
    QList<QOrganizerItemLocalId> itemIds(const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, QOrganizerItemManager::Error *error) const;

    QList<QOrganizerItem> items(const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const;
    QOrganizerItem item(const QOrganizerItemLocalId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const;

    bool saveItems(QList<QOrganizerItem>* items, const QOrganizerCollectionLocalId& collectionId, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error);
    bool removeItems(const QList<QOrganizerItemLocalId> &itemIds, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error *error);

    /* Collections - every item belongs to exactly one collection */
    QOrganizerCollectionLocalId defaultCollectionId(QOrganizerItemManager::Error* error) const;
    QList<QOrganizerCollectionLocalId> collectionIds(QOrganizerItemManager::Error* error) const;
    QList<QOrganizerCollection> collections(const QList<QOrganizerCollectionLocalId>& collectionIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error) const;
    bool saveCollection(QOrganizerCollection* collection, QOrganizerItemManager::Error* error);
    bool removeCollection(const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error);

    /* Capabilities reporting */
    bool hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString &itemType) const;
    bool isFilterSupported(const QOrganizerItemFilter &filter) const;
    QList<int> supportedDataTypes() const;
    QStringList supportedItemTypes() const;

    /* Asynchronous Request Support */
    void requestDestroyed(QOrganizerItemAbstractRequest *req);
    bool startRequest(QOrganizerItemAbstractRequest *req);
    bool cancelRequest(QOrganizerItemAbstractRequest *req);
    bool waitForRequestFinished(QOrganizerItemAbstractRequest *req, int msecs);

public Q_SLOTS:
    void dataChanged();

private:
    QList<QOrganizerItem> internalItemInstances(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const;
    QList<QOrganizerItem> internalItemInstances(const QOrganizerItem &generator, const QDateTime &periodStart, const QDateTime &periodEnd, int maxCount, QOrganizerItemManager::Error *error) const;
    QList<QOrganizerItemLocalId> internalItemIds(const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, QOrganizerItemManager::Error *error) const;

    QList<QOrganizerItem> internalItems(const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const;
    QOrganizerItem internalItem(const QOrganizerItemLocalId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const;

    bool internalSaveItems(QList<QOrganizerItem>* items, const QOrganizerCollectionLocalId& collectionId, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error);
    bool internalRemoveItems(const QList<QOrganizerItemLocalId> &itemIds, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error *error);

    /* Collections - every item belongs to exactly one collection */
    QOrganizerCollectionLocalId internalDefaultCollectionId(QOrganizerItemManager::Error* error) const;
    QList<QOrganizerCollectionLocalId> internalCollectionIds(QOrganizerItemManager::Error* error) const;
    QList<QOrganizerCollection> internalCollections(const QList<QOrganizerCollectionLocalId>& collectionIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error) const;
    bool internalSaveCollection(QOrganizerCollection* collection, QOrganizerItemManager::Error* error);
    bool internalRemoveCollection(const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error);

    // single item saving implementation
    void checkItemIdValidity(CCalendar *cal, QOrganizerItem *checkItem, QOrganizerItemManager::Error *error);
    int doSaveItem(CCalendar *cal, QOrganizerItem *item, QOrganizerItemChangeSet &cs, QOrganizerItemManager::Error *error);

    // saving the occurrences
    int saveEventOccurrence(CCalendar *cal, QOrganizerEventOccurrence *occurrence, QOrganizerEvent *parent, QOrganizerItemChangeSet &cs, QOrganizerItemManager::Error *error);
    void insertOccurenceSortedByStartDate(QOrganizerItem *occurrence, QList<QOrganizerItem> &target) const;

    // getting the parent (base) item of an exception item (event occurrence or todo occurrence)
    QOrganizerItem parentOf(CCalendar *cal, QOrganizerItem *occurence, QOrganizerItemManager::Error *error) const;

    // internal fetch item
    QOrganizerItem internalFetchItem(const QOrganizerItemLocalId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error, bool fetchOccurrences) const;

    // identifying native item as an occurrence
    bool isOccurrence(CCalendar *cal, CComponent *ccomponent, QString typeStr, QOrganizerItemManager::Error *error) const;
    bool isParent(CCalendar *cal, CComponent *ccomponent, QString typeStr, QOrganizerItemManager::Error *error) const;
    bool containsRecurrenceInformation(CComponent *ccomponent) const;

    // returns a fetch hint for fetching minimal amount of information
    QOrganizerItemFetchHint fetchMinimalData() const;

    // calendar instance deletion helper
    void cleanupCal(CCalendar *cal) const;

    // get calendar
    CCalendar* getCalendar(QOrganizerCollectionLocalId collectionId, QOrganizerItemManager::Error *error) const;

    // extract possible collection ids from the filters
    QSet<QOrganizerCollectionId> extractCollectionIds(const QOrganizerItemFilter& filter) const;
    QSet<QOrganizerCollectionLocalId> extractCollectionLocalIds(const QOrganizerItemFilter& filter) const;

    // ctor
    QOrganizerItemMaemo5Engine();

private:
    QOrganizerItemMaemo5EngineData *d;
    QTimer m_waitTimer;
    mutable QMutex m_operationMutex;
    friend class QOrganizerItemMaemo5Factory;
};

#endif
