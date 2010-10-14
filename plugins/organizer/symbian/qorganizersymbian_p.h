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
#include "qorganizeritemenginelocalid.h"
#include "qorganizercollectionenginelocalid.h"
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

// This is the result of qHash(QString(QLatin1String("symbian")))
// Precalculated value is used for performance reasons.
const uint KSymbianEngineLocalIdType = 0xb038f9e;

class OrganizerItemTransform; // forward declare transform class.
class QOrganizerItemSymbianEngine; // forward declare symbian engine.
class QOrganizerCollectionSymbianEngineLocalId : public QOrganizerCollectionEngineLocalId
{
public:
    QOrganizerCollectionSymbianEngineLocalId();
    QOrganizerCollectionSymbianEngineLocalId(quint64 collectionId);
    ~QOrganizerCollectionSymbianEngineLocalId();
    QOrganizerCollectionSymbianEngineLocalId(const QOrganizerCollectionSymbianEngineLocalId& other);

    bool isEqualTo(const QOrganizerCollectionEngineLocalId* other) const;
    bool isLessThan(const QOrganizerCollectionEngineLocalId* other) const;

    uint engineLocalIdType() const;
    QOrganizerCollectionEngineLocalId* clone() const;

#ifndef QT_NO_DEBUG_STREAM
    QDebug debugStreamOut(QDebug dbg);
#endif
#ifndef QT_NO_DATASTREAM
    QDataStream& dataStreamOut(QDataStream& out);
    QDataStream& dataStreamIn(QDataStream& in);
#endif
    uint hash() const;

private:
    quint64 m_localCollectionId;
    friend class QOrganizerItemSymbianEngine;
};

class QOrganizerItemSymbianEngineLocalId : public QOrganizerItemEngineLocalId
{
public:
    QOrganizerItemSymbianEngineLocalId();
    QOrganizerItemSymbianEngineLocalId(quint64 collectionId, quint32 itemId);
    ~QOrganizerItemSymbianEngineLocalId();
    QOrganizerItemSymbianEngineLocalId(const QOrganizerItemSymbianEngineLocalId& other);

    bool isEqualTo(const QOrganizerItemEngineLocalId* other) const;
    bool isLessThan(const QOrganizerItemEngineLocalId* other) const;

    uint engineLocalIdType() const;
    QOrganizerItemEngineLocalId* clone() const;

#ifndef QT_NO_DEBUG_STREAM
    QDebug debugStreamOut(QDebug dbg);
#endif
#ifndef QT_NO_DATASTREAM
    QDataStream& dataStreamOut(QDataStream& out);
    QDataStream& dataStreamIn(QDataStream& in);
#endif
    uint hash() const;
    
public:
    quint32 calLocalUid() { return m_localItemId; }
    quint64 calCollectionId() { return m_localCollectionId; }
    
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
    QOrganizerItemEngineLocalId* createItemEngineLocalId() const;
    QOrganizerCollectionEngineLocalId* createCollectionEngineLocalId() const;
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

    QList<QOrganizerItemLocalId> itemIds(const QDateTime& periodStart,
        const QDateTime& periodEnd,
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders, 
        QOrganizerManager::Error* error) const;
    QList<QOrganizerItemLocalId> getIdsModifiedSinceDateL(
        const QDateTime& periodStart,
        const QDateTime& periodEnd,
        const QOrganizerItemFilter& filter) const;
    void itemIdsL(
        QList<QOrganizerItemLocalId>& ids, 
        const QDateTime& periodStart,
        const QDateTime& periodEnd,
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders) const;
    QList<QOrganizerItem> itemsForExport(const QDateTime& periodStart,
        const QDateTime& periodEnd,
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders, 
        const QOrganizerItemFetchHint& fetchHint, 
        QOrganizerManager::Error* error) const;
    void itemsForExportL(QList<QOrganizerItem>& itemsList, 
        const QDateTime& periodStart,
        const QDateTime& periodEnd,
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders, 
        const QOrganizerItemFetchHint& fetchHint) const;
    QOrganizerItem item(const QOrganizerItemLocalId& itemId, 
        const QOrganizerItemFetchHint& fetchHint, 
        QOrganizerManager::Error* error) const;

    bool saveItem(QOrganizerItem* item,
        QOrganizerManager::Error* error);
    bool saveItems(QList<QOrganizerItem> *items, 
        QMap<int, QOrganizerManager::Error> *errorMap, 
        QOrganizerManager::Error *error);

    bool removeItem(const QOrganizerItemLocalId& organizeritemId, 
        QOrganizerManager::Error* error);
    bool removeItems(const QList<QOrganizerItemLocalId> &itemIds, 
        QMap<int, QOrganizerManager::Error> *errorMap, 
        QOrganizerManager::Error *error);
    
    /* Collections - every item belongs to exactly one collection */
    QOrganizerCollection defaultCollection(
        QOrganizerManager::Error* error) const;
    QList<QOrganizerCollection> collections(
        QOrganizerManager::Error* error) const;
    QOrganizerCollection collection(
        const QOrganizerCollectionLocalId& collectionId,
        QOrganizerManager::Error* error);
    bool saveCollection(QOrganizerCollection* collection, 
        QOrganizerManager::Error* error);
    bool removeCollection(const QOrganizerCollectionLocalId& collectionId, 
        QOrganizerManager::Error* error);

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
    void itemOccurrencesL(
        QList<QOrganizerItem> &itemOccurrences,
        const QOrganizerItem &parentItem,
        const QDateTime &periodStart,
        const QDateTime &periodEnd,
        int maxCount,
        const QOrganizerItemFetchHint &fetchHint) const;
    QList<QOrganizerItem> itemsL(
        QList<QOrganizerItem> &itemOccurrences,
        const QDateTime& periodStart,
        const QDateTime& periodEnd,
        const QOrganizerItemFilter &filter,
        const QList<QOrganizerItemSortOrder> &sortOrders,
        const QOrganizerItemFetchHint &fetchHint) const;
    void toItemOccurrencesL(
        const RPointerArray<CCalInstance> &calInstanceList,
        QOrganizerItem parentItem,
        const int maxCount,
        QOrganizerCollectionLocalId collectionLocalId,
        QList<QOrganizerItem> &itemOccurrences) const;
    QOrganizerItem itemL(const QOrganizerItemLocalId& itemId,
            const QOrganizerItemFetchHint& fetchHint) const;
    void saveItemL(QOrganizerItem *item,
        QOrganizerItemChangeSet *changeSet);
    void removeItemL(const QOrganizerItemLocalId& organizeritemId);
    QList<QOrganizerItem> slowFilter(const QList<QOrganizerItem> &items, 
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders) const;

private:
    QList<QOrganizerCollection> collectionsL() const;
    QOrganizerCollection collectionL(
        const QOrganizerCollectionLocalId& collectionId) const;
    void saveCollectionL(QOrganizerCollection* collection);
    void removeCollectionL(const QOrganizerCollectionLocalId& collectionId);
    CCalEntryView* entryViewL(const QOrganizerCollectionLocalId& collectionId) const;
    CCalInstanceView* instanceViewL(const QOrganizerCollectionLocalId& collectionId) const;
    QOrganizerCollectionLocalId collectionLocalIdL(const QOrganizerItem &item, 
        const QOrganizerCollectionLocalId &collectionId = QOrganizerCollectionLocalId()) const;
    CCalEntry* entryForItemOccurrenceL(
        const QOrganizerCollectionLocalId &collectionId, const QOrganizerItem &item, 
        bool &isNewEntry) const;
    CCalEntry* entryForItemL(const QOrganizerCollectionLocalId &collectionId, 
        const QOrganizerItem &item, bool &isNewEntry) const;
    CCalEntry* findEntryL(const QOrganizerCollectionLocalId &collectionId, 
        const QOrganizerItemLocalId &localId, QString manageruri) const;
    CCalEntry* findEntryL(const QOrganizerCollectionLocalId &collectionId, 
        const TDesC8& globalUid) const;
    CCalEntry* findParentEntryLC(const QOrganizerCollectionLocalId &collectionId,
        const QOrganizerItem &item, const TDesC8& globalUid) const;
	
private:
    QOrganizerItemSymbianEngineData *d;
    
    OrganizerSymbianCollection m_defaultCollection;
    QMap<QOrganizerCollectionLocalId, OrganizerSymbianCollection> m_collections;
    QOrganizerItemRequestQueue* m_requestServiceProviderQueue;
    OrganizerItemTransform m_itemTransform;
    mutable QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > m_definition;

    friend class QOrganizerItemSymbianFactory;
};


#endif

