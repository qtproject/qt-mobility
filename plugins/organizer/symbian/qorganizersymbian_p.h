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
#include "qorganizeritemmanager.h"
#include "qorganizeritemmanagerengine.h"
#include "qorganizeritemmanagerenginefactory.h"
#include "qorganizeritemchangeset.h"

#include "organizeritemtransform.h"

#include <calprogresscallback.h> // MCalProgressCallBack
#include <calchangecallback.h>
#ifdef SYMBIAN_CALENDAR_V2
#include <calfilechangenotification.h>
#endif

QTM_USE_NAMESPACE

class QOrganizerItemSymbianFactory : public QObject, public QOrganizerItemManagerEngineFactory
{
  Q_OBJECT
  Q_INTERFACES(QtMobility::QOrganizerItemManagerEngineFactory)
  public:
    QOrganizerItemManagerEngine* engine(const QMap<QString, QString>& parameters, QOrganizerItemManager::Error*);
    QString managerName() const;
};

class QOrganizerItemSymbianEngineData : public QSharedData
{
public:
    QOrganizerItemSymbianEngineData()
        : QSharedData()
    {
    }

    QOrganizerItemSymbianEngineData(const QOrganizerItemSymbianEngineData& other)
        : QSharedData(other)
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

class QOrganizerItemSymbianEngine : public QOrganizerItemManagerEngine, 
                                    public MCalProgressCallBack,
                                    public MCalChangeCallBack2
#ifdef SYMBIAN_CALENDAR_V2
                                    ,public MCalFileChangeObserver
#endif
{
    Q_OBJECT

public:
    static QOrganizerItemSymbianEngine *createSkeletonEngine(const QMap<QString, QString>& parameters);

    QOrganizerItemSymbianEngine();
    ~QOrganizerItemSymbianEngine();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    int managerVersion() const;

    QList<QOrganizerItem> itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, QOrganizerItemManager::Error* error) const;
    QList<QOrganizerItem> itemInstances(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint,QOrganizerItemManager::Error* error) const;

    QList<QOrganizerItemLocalId> itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const;
    QList<QOrganizerItem> items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const;
    QOrganizerItem item(const QOrganizerItemLocalId& itemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const;

    bool saveItem(QOrganizerItem* item, const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error);
    bool saveItems(QList<QOrganizerItem> *items, const QOrganizerCollectionLocalId& collectionId, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error *error);

    bool removeItem(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemManager::Error* error);
    bool removeItems(const QList<QOrganizerItemLocalId> &itemIds, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error *error);
    
    /* Collections - every item belongs to exactly one collection */
#ifdef SYMBIAN_CALENDAR_V2
    QOrganizerCollectionLocalId defaultCollectionId(QOrganizerItemManager::Error* error) const;
    QList<QOrganizerCollectionLocalId> collectionIds(QOrganizerItemManager::Error* error) const;
    QList<QOrganizerCollection> collections(const QList<QOrganizerCollectionLocalId>& collectionIds, QOrganizerItemManager::Error* error) const;
    bool saveCollection(QOrganizerCollection* collection, QOrganizerItemManager::Error* error);
    bool removeCollection(const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error);
#endif

    /* Definitions - Accessors and Mutators */
    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(const QString& itemType, QOrganizerItemManager::Error* error) const;

    /* Capabilities reporting */
    bool hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& itemType) const;
    bool isFilterSupported(const QOrganizerItemFilter& filter) const;
    QList<QVariant::Type> supportedDataTypes() const;
    QStringList supportedItemTypes() const;

    /* Asynchronous Request Support */
    void requestDestroyed(QOrganizerItemAbstractRequest* req);
    bool startRequest(QOrganizerItemAbstractRequest* req);
    bool cancelRequest(QOrganizerItemAbstractRequest* req);
    bool waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs);

public: // MCalProgressCallBack
    void Progress(TInt aPercentageCompleted);
    void Completed(TInt aError);
    TBool NotifyProgress();

public: // MCalChangeCallBack2
    void CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems);

#ifdef SYMBIAN_CALENDAR_V2
public: // MCalFileChangeObserver
    void CalendarInfoChangeNotificationL(RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries);
#endif
    
public: 
    /* Util functions */
    static bool transformError(TInt symbianError, QOrganizerItemManager::Error* qtError);
    void deleteItemL(const QOrganizerItemLocalId& organizeritemId);
    void saveItemL(QOrganizerItem *item, QOrganizerItemChangeSet *changeSet = 0);
    void itemL(const QOrganizerItemLocalId& itemId, QOrganizerItem *item, 
            const QOrganizerItemFetchHint& fetchHint) const;
    QList<QOrganizerItem> slowFilter(const QList<QOrganizerItem> &items, 
        const QOrganizerItemFilter& filter, 
        const QList<QOrganizerItemSortOrder>& sortOrders) const;
#ifdef SYMBIAN_CALENDAR_V2
    QList<QOrganizerCollectionLocalId> collectionIdsL() const;
    QList<QOrganizerCollection> collectionsL(const QList<QOrganizerCollectionLocalId>& collectionIds) const;
    void saveCollectionL(QOrganizerCollection* collection);
    void removeCollectionL(const QOrganizerCollectionLocalId& collectionId);
#endif
    
private:
    CCalEntry* entryForItemOccurrenceL(QOrganizerItem *item, bool &isNewEntry) const;
    CCalEntry* entryForItemL(QOrganizerItem *item, bool &isNewEntry) const;
    CCalEntry* findEntryL(QOrganizerItemLocalId localId, QString manageruri) const;
    CCalEntry* findEntryL(const TDesC8& globalUid) const;
    CCalEntry* findParentEntryLC(QOrganizerItem *item, const TDesC8& globalUid) const;
    void removeItemL(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemChangeSet *changeSet);
	
private:
    QOrganizerItemSymbianEngineData *d;
    CCalSession *m_defaultCalSession;
#ifdef SYMBIAN_CALENDAR_V2    
    RPointerArray<CCalSession> m_calSessions;
#endif
    CCalEntryView *m_entryView;
    CCalInstanceView *m_instanceView;
    CActiveSchedulerWait *m_activeSchedulerWait;
    QOrganizerItemRequestQueue* m_requestServiceProviderQueue;

    // TODO: replace this with an algorithm that generates the calendar entry UID
    int m_entrycount;
    OrganizerItemTransform m_itemTransform;
    mutable QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > m_definition;

    friend class QOrganizerItemSymbianFactory;
};


#endif

