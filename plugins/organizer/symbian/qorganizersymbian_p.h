/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QORGANIZERSYMBIAN_P_H
#define QORGANIZERSYMBIAN_P_H


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

#include "qorganizeritem.h"
#include "qorganizeritemengineid.h"
#include "qorganizercollectionengineid.h"
#include "qorganizermanager.h"
#include "qorganizermanagerengine.h"
#include "qorganizermanagerenginefactory.h"
#include "qorganizeritemchangeset.h"

#include "organizeritemtransform.h"
#include "organizersymbiancollection.h"

#include <calprogresscallback.h> // MCalProgressCallBack
#include <calchangecallback.h>
#ifdef SYMBIAN_CALENDAR_V2
#include <calfilechangenotification.h>
#endif


QTM_USE_NAMESPACE

class OrganizerItemTransform; // forward declare transform class.
class QOrganizerItemSymbianEngine; // forward declare symbian engine.
class QOrganizerCollectionSymbianEngineId : public QOrganizerCollectionEngineId
{
public:
    QOrganizerCollectionSymbianEngineId();
    QOrganizerCollectionSymbianEngineId(quint64 collectionId);
    ~QOrganizerCollectionSymbianEngineId();
    QOrganizerCollectionSymbianEngineId(const QOrganizerCollectionSymbianEngineId& other);

    bool isEqualTo(const QOrganizerCollectionEngineId* other) const;
    bool isLessThan(const QOrganizerCollectionEngineId* other) const;

    QString managerUri() const;
    QOrganizerCollectionEngineId* clone() const;

    QString toString() const;

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const;
#endif
    uint hash() const;

private:
    quint64 m_localCollectionId;
    friend class QOrganizerItemSymbianEngine;
};

class QOrganizerItemSymbianEngineId : public QOrganizerItemEngineId
{
public:
    QOrganizerItemSymbianEngineId();
    QOrganizerItemSymbianEngineId(quint64 collectionId, quint32 itemId);
    ~QOrganizerItemSymbianEngineId();
    QOrganizerItemSymbianEngineId(const QOrganizerItemSymbianEngineId& other);

    bool isEqualTo(const QOrganizerItemEngineId* other) const;
    bool isLessThan(const QOrganizerItemEngineId* other) const;

    QString managerUri() const;
    QOrganizerItemEngineId* clone() const;

    QString toString() const;

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const;
#endif
    uint hash() const;
    
public:
    quint32 calLocalUid() const { return m_localItemId; }
    quint64 calCollectionId() const { return m_localCollectionId; }
    
private:
    quint64 m_localCollectionId;
    quint32 m_localItemId; // the symbian backend requires quint32 for itemId + quint64 for collectionId
    friend class QOrganizerItemSymbianEngine;
    friend class OrganizerItemTransform;
};




class QOrganizerItemSymbianFactory : public QObject, 
public QOrganizerManagerEngineFactory
{
  Q_OBJECT
  Q_INTERFACES(QtMobility::QOrganizerManagerEngineFactory)
  public:
    QOrganizerManagerEngine* engine(
        const QMap<QString, QString>& parameters, 
        QOrganizerManager::Error*);
    QString managerName() const;
    QOrganizerItemEngineId* createItemEngineId(const QMap<QString, QString>& parameters,
            const QString& engineIdString) const;
    QOrganizerCollectionEngineId* createCollectionEngineId(const QMap<QString, QString>& parameters,
            const QString& engineIdString) const;
};

class QOrganizerItemSymbianEngineData : public QSharedData
{
public:
    QOrganizerItemSymbianEngineData()
        : QSharedData()
    {
    }

    QOrganizerItemSymbianEngineData(
        const QOrganizerItemSymbianEngineData& other) : QSharedData(other)
    {
    }

    ~QOrganizerItemSymbianEngineData()
    {
    }
};

class CCalSession;
class CCalEntryView;
class CCalInstanceView;
class CActiveSchedulerWait;
class QOrganizerItemRequestQueue;
#ifdef SYMBIAN_CALENDAR_V2
class CCalCalendarInfo;
#endif

class QOrganizerItemSymbianEngine : public QOrganizerManagerEngine

#ifdef SYMBIAN_CALENDAR_V2
                                    ,public MCalFileChangeObserver
#endif
{
    Q_OBJECT

public:
    static QOrganizerItemSymbianEngine *createSkeletonEngine(
        const QMap<QString, QString>& parameters);

    QOrganizerItemSymbianEngine();
    ~QOrganizerItemSymbianEngine();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    int managerVersion() const;

    QList<QOrganizerItem> itemOccurrences(const QOrganizerItem& parentItem,
        const QDateTime& periodStart, const QDateTime& periodEnd,
        int maxCount, const QOrganizerItemFetchHint& fetchHint,
        QOrganizerManager::Error* error) const;
    QList<QOrganizerItem> items(const QDateTime& periodStart,
        const QDateTime& periodEnd,
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders, 
        const QOrganizerItemFetchHint& fetchHint, 
        QOrganizerManager::Error* error) const;
    QList<QOrganizerItemId> itemIds(
        const QDateTime& periodStart,
        const QDateTime& periodEnd,
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders, 
        QOrganizerManager::Error* error) const;
    QList<QOrganizerItem> itemsForExport(const QDateTime& periodStart,
        const QDateTime& periodEnd,
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders, 
        const QOrganizerItemFetchHint& fetchHint, 
        QOrganizerManager::Error* error) const;
    QOrganizerItem item(const QOrganizerItemId& itemId, 
        const QOrganizerItemFetchHint& fetchHint, 
        QOrganizerManager::Error* error) const;

    bool saveItem(QOrganizerItem* item,
        QOrganizerManager::Error* error);
    bool saveItems(QList<QOrganizerItem> *items, 
        QMap<int, QOrganizerManager::Error> *errorMap, 
        QOrganizerManager::Error *error);

    bool removeItem(const QOrganizerItemId& organizeritemId, 
        QOrganizerManager::Error* error);
    bool removeItems(const QList<QOrganizerItemId> &itemIds, 
        QMap<int, QOrganizerManager::Error> *errorMap, 
        QOrganizerManager::Error *error);
    
    /* Collections - every item belongs to exactly one collection */
    QOrganizerCollection defaultCollection(
        QOrganizerManager::Error* error) const;
    QOrganizerCollection collection(
        const QOrganizerCollectionId& collectionId,
        QOrganizerManager::Error* error) const;
    QList<QOrganizerCollection> collections(
        QOrganizerManager::Error* error) const;
    bool saveCollection(QOrganizerCollection* collection, 
        QOrganizerManager::Error* error);
    bool removeCollection(const QOrganizerCollectionId& collectionId, 
        QOrganizerManager::Error* error);

    /* Return a pruned or modified item which is valid and can be saved in the backend */
    QOrganizerItem compatibleItem(const QOrganizerItem& original, QOrganizerManager::Error* error) const;
    QOrganizerCollection compatibleCollection(const QOrganizerCollection& original, QOrganizerManager::Error* error) const;

    /* Definitions - Accessors and Mutators */
    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(
        const QString& itemType, QOrganizerManager::Error* error) const;

    /* Capabilities reporting */
    bool hasFeature(QOrganizerManager::ManagerFeature feature, 
        const QString& itemType) const;
    bool isFilterSupported(const QOrganizerItemFilter& filter) const;
    QList<int> supportedDataTypes() const;
    QStringList supportedItemTypes() const;

    /* Asynchronous Request Support */
    void requestDestroyed(QOrganizerAbstractRequest* req);
    bool startRequest(QOrganizerAbstractRequest* req);
    bool cancelRequest(QOrganizerAbstractRequest* req);
    bool waitForRequestFinished(QOrganizerAbstractRequest* req, int msecs);

public: // MCalProgressCallBack
    void Progress(TInt aPercentageCompleted);
    void Completed(TInt aError);
    TBool NotifyProgress();

#ifdef SYMBIAN_CALENDAR_V2
public: // MCalFileChangeObserver
    void CalendarInfoChangeNotificationL(
        RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries);
#endif
    
public:
    void initializeL();
    /* Util functions */
    static bool transformError(TInt symbianError, 
        QOrganizerManager::Error* qtError);
    QList<QOrganizerItemId> itemIdsL(
        const QDateTime& periodStart,
        const QDateTime& periodEnd,
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders) const;
    QList<QOrganizerItemId> itemIdsL(
        const QList<OrganizerSymbianCollection> &collections,
        const QDateTime& periodStart,
        const QDateTime& periodEnd) const;
    QList<QOrganizerItem> itemsForExportL(
        const QDateTime& periodStart,
        const QDateTime& periodEnd,
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders) const;    
    QList<QOrganizerItem> itemOccurrencesL(
        const QOrganizerItemId &parentItemId,
        const QDateTime &periodStart,
        const QDateTime &periodEnd,
        int maxCount,
        const QOrganizerItemFetchHint &fetchHint) const;
    QList<QOrganizerItem> itemOccurrencesL(
        const QDateTime& periodStart,
        const QDateTime& periodEnd,
        const QOrganizerItemFilter &filter,
        const QList<QOrganizerItemSortOrder> &sortOrders,
        const QOrganizerItemFetchHint &fetchHint) const;
    void toItemOccurrencesL(
        const RPointerArray<CCalInstance> &calInstanceList,
        QOrganizerItem parentItem,
        const int maxCount,
        QOrganizerCollectionId collectionId,
        QList<QOrganizerItem> &itemOccurrences) const;
    QOrganizerItem itemL(const QOrganizerItemId& itemId,
        const QOrganizerItemFetchHint& fetchHint) const;
    void saveItemL(QOrganizerItem *item,
        QOrganizerItemChangeSet *changeSet);
    void removeItemL(const QOrganizerItemId& organizeritemId);
    QList<QOrganizerItem> slowFilterIdsToItemsL(
        const QList<QOrganizerItemId> &itemIds,
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders) const;
    QList<QOrganizerItemId> slowFilterIdsL(
        const QList<QOrganizerItemId> &itemIds,
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders) const;    
    QList<QOrganizerItem> slowFilterItems(
        const QList<QOrganizerItem> &items, 
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders) const;
    void addFilteredAndSorted(
        QList<QOrganizerItem> *items,
        const QOrganizerItem &item,
        const QOrganizerItemFilter& filter,
        const QList<QOrganizerItemSortOrder>& sortOrders) const;
    QList<OrganizerSymbianCollection> filteredSymbianCollectionsL(
        const QOrganizerItemFilter &filter) const;

private:
    QList<QOrganizerCollection> collectionsL() const;
    void saveCollectionL(QOrganizerCollection* collection);
    void removeCollectionL(const QOrganizerCollectionId& collectionId);
    bool isCollectionNameAvailable(QString name) const;
    CCalEntryView* entryViewL(const QOrganizerCollectionId& collectionId) const;
    CCalInstanceView* instanceViewL(const QOrganizerCollectionId& collectionId) const;
    QOrganizerCollectionId collectionIdL(const QOrganizerItem &item, 
        const QOrganizerCollectionId &collectionId = QOrganizerCollectionId()) const;
    CCalEntry* entryForItemOccurrenceL(
        const QOrganizerCollectionId &collectionId, const QOrganizerItem &item, 
        bool &isNewEntry) const;
    CCalEntry* entryForItemL(const QOrganizerCollectionId &collectionId, 
        const QOrganizerItem &item, bool &isNewEntry) const;
    CCalEntry* findEntryL(const QOrganizerCollectionId &collectionId, 
        const QOrganizerItemId &localId, QString manageruri) const;
    CCalEntry* findEntryL(const QOrganizerCollectionId &collectionId, 
        const TDesC8& globalUid) const;
    CCalEntry* findParentEntryLC(const QOrganizerCollectionId &collectionId,
        const QOrganizerItem &item, const TDesC8& globalUid) const;

private:
    QOrganizerItemSymbianEngineData *d;
    
    OrganizerSymbianCollection m_defaultCollection;
    QMap<QOrganizerCollectionId, OrganizerSymbianCollection> m_collections;
    QOrganizerItemRequestQueue* m_requestServiceProviderQueue;
    OrganizerItemTransform m_itemTransform;
    mutable QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > m_definition;

    friend class QOrganizerItemSymbianFactory;
};


#endif

