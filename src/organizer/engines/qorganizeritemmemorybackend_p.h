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


#ifndef QCONTACTMEMORYBACKEND_P_H
#define QCONTACTMEMORYBACKEND_P_H

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
#include "qorganizermanager.h"
#include "qorganizermanagerengine.h"
#include "qorganizeritemdetaildefinition.h"
#include "qorganizerabstractrequest.h"
#include "qorganizeritemchangeset.h"
#include "qorganizerrecurrencerule.h"
#include "qorganizeritemengineid.h"
#include "qorganizercollectionengineid.h"

#ifdef QT_SIMULATOR
#define Q_ORGANIZER_MEMORYENGINE_EXPORT Q_ORGANIZER_EXPORT
#else
#define Q_ORGANIZER_MEMORYENGINE_EXPORT
#endif

QTM_BEGIN_NAMESPACE

class Q_ORGANIZER_MEMORYENGINE_EXPORT QOrganizerItemMemoryEngineId : public QOrganizerItemEngineId
{
public:
    QOrganizerItemMemoryEngineId();
    QOrganizerItemMemoryEngineId(quint32 collectionId, quint32 itemId, const QString& managerUri);
    ~QOrganizerItemMemoryEngineId();
    QOrganizerItemMemoryEngineId(const QOrganizerItemMemoryEngineId& other);
    QOrganizerItemMemoryEngineId(const QString& idString);

    bool isEqualTo(const QOrganizerItemEngineId* other) const;
    bool isLessThan(const QOrganizerItemEngineId* other) const;

    QString managerUri() const;
    QOrganizerItemEngineId* clone() const;

    QString toString() const;

#ifndef QT_NO_DEBUG_STREAM
    QDebug& debugStreamOut(QDebug& dbg) const;
#endif
    uint hash() const;

private:
    quint32 m_collectionId;
    quint32 m_itemId;
    QString m_managerUri;
    friend class QOrganizerItemMemoryEngine;
};

class Q_ORGANIZER_MEMORYENGINE_EXPORT QOrganizerCollectionMemoryEngineId : public QOrganizerCollectionEngineId
{
public:
    QOrganizerCollectionMemoryEngineId();
    QOrganizerCollectionMemoryEngineId(quint32 collectionId, const QString& managerUri);
    ~QOrganizerCollectionMemoryEngineId();
    QOrganizerCollectionMemoryEngineId(const QOrganizerCollectionMemoryEngineId& other);
    QOrganizerCollectionMemoryEngineId(const QString& idString);

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
    quint32 m_collectionId;
    QString m_managerUri;
    friend class QOrganizerItemMemoryEngine;
};

class QOrganizerAbstractRequest;
class QOrganizerManagerEngine;
class Q_ORGANIZER_MEMORYENGINE_EXPORT QOrganizerItemMemoryEngineData : public QSharedData
{
public:
    QOrganizerItemMemoryEngineData();
    ~QOrganizerItemMemoryEngineData()
    {
    }

    QAtomicInt m_refCount;
    QString m_id;                                  // the id parameter value

    QList<QOrganizerItem> m_organizeritems;                      // list of organizer items
    QList<QOrganizerItemId> m_organizeritemIds;             // list of organizer item Id's
    QList<QOrganizerCollection> m_organizerCollections;          // list of collections
    QList<QOrganizerCollectionId> m_organizerCollectionIds; // list of collection ids
    QMultiMap<QOrganizerCollectionId, QOrganizerItemId> m_itemsInCollections; // map of collection ids to the ids of items the collection contains.
    QList<QString> m_definitionIds;                // list of definition types (id's)
    mutable QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > m_definitions; // map of organizer item type to map of definition name to definitions.
    quint32 m_nextOrganizerItemId; // the m_itemId portion of a QOrganizerItemMemoryEngineId.
    quint32 m_nextOrganizerCollectionId; // the m_collectionId portion of a QOrganizerCollectionMemoryEngineId.
    bool m_anonymous;                              // Is this backend ever shared?
    QString m_managerUri;                        // for faster lookup.

    void emitSharedSignals(QOrganizerItemChangeSet* cs)
    {
        foreach(QOrganizerManagerEngine* engine, m_sharedEngines)
            cs->emitSignals(engine);
    }

    QList<QOrganizerManagerEngine*> m_sharedEngines;   // The list of engines that share this data
};

class Q_ORGANIZER_MEMORYENGINE_EXPORT QOrganizerItemMemoryEngine : public QOrganizerManagerEngine
{
    Q_OBJECT

public:
    static QOrganizerItemMemoryEngine *createMemoryEngine(const QMap<QString, QString>& parameters);

    ~QOrganizerItemMemoryEngine();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    /*! \reimp */
    int managerVersion() const {return 1;}

    virtual QList<QOrganizerItem> itemOccurrences(const QOrganizerItem& parentItem, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;

    virtual QList<QOrganizerItemId> itemIds(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerManager::Error* error) const;
    virtual QList<QOrganizerItem> items(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;
    virtual QList<QOrganizerItem> itemsForExport(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;
    virtual QOrganizerItem item(const QOrganizerItemId& organizeritemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;

    virtual bool saveItems(QList<QOrganizerItem>* organizeritems, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error);
    virtual bool removeItems(const QList<QOrganizerItemId>& organizeritemIds, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error);

    virtual QOrganizerCollection defaultCollection(QOrganizerManager::Error* error) const;
    virtual QOrganizerCollection collection(const QOrganizerCollectionId &collectionId, QOrganizerManager::Error *error) const;
    virtual QList<QOrganizerCollection> collections(QOrganizerManager::Error* error) const;
    virtual bool saveCollection(QOrganizerCollection* collection, QOrganizerManager::Error* error);
    virtual bool removeCollection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error);

    /*! \reimp */
    virtual QOrganizerItem compatibleItem(const QOrganizerItem& original, QOrganizerManager::Error* error) const
    {
        return QOrganizerManagerEngine::compatibleItem(original, error);
    }
    /*! \reimp */
    virtual QOrganizerCollection compatibleCollection(const QOrganizerCollection& original, QOrganizerManager::Error* error) const;

    /*! \reimp */
    virtual bool validateItem(const QOrganizerItem& organizeritem, QOrganizerManager::Error* error) const
    {
        return QOrganizerManagerEngine::validateItem(organizeritem, error);
    }
    /*! \reimp */
    virtual bool validateCollection(const QOrganizerCollection& collection, QOrganizerManager::Error* error) const
    {
        Q_UNUSED(collection)
        *error = QOrganizerManager::NoError;
        return true; // all collections are valid in the memory engine.
    }
    /*! \reimp */
    virtual bool validateDefinition(const QOrganizerItemDetailDefinition& def, QOrganizerManager::Error* error) const
    {
        return QOrganizerManagerEngine::validateDefinition(def, error);
    }

    /* Definitions - Accessors and Mutators */
    virtual QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(const QString& organizeritemType, QOrganizerManager::Error* error) const;
    /*! \reimp */
    virtual QOrganizerItemDetailDefinition detailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerManager::Error* error) const
    {
        return QOrganizerManagerEngine::detailDefinition(definitionId, organizeritemType, error);
    }
    virtual bool saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerManager::Error* error);
    virtual bool removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerManager::Error* error);

    /* Asynchronous Request Support */
    virtual void requestDestroyed(QOrganizerAbstractRequest* req);
    virtual bool startRequest(QOrganizerAbstractRequest* req);
    virtual bool cancelRequest(QOrganizerAbstractRequest* req);
    virtual bool waitForRequestFinished(QOrganizerAbstractRequest* req, int msecs);

    /* Capabilities reporting */
    virtual bool hasFeature(QOrganizerManager::ManagerFeature feature, const QString& organizeritemType) const;
    virtual bool isFilterSupported(const QOrganizerItemFilter& filter) const;
    /*! \reimp */
    virtual QStringList supportedItemTypes() const
    {
        return QOrganizerManagerEngine::supportedItemTypes();
    }

protected:
    QOrganizerItemMemoryEngine(QOrganizerItemMemoryEngineData* data);

protected:
    /* Implement "signal coalescing" for batch functions via change set */
    virtual bool saveItem(QOrganizerItem* theOrganizerItem, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error);
    virtual bool removeItem(const QOrganizerItemId& organizeritemId, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error);
    virtual bool saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error);
    virtual bool removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerManager::Error* error);

private:
    QList<QOrganizerItem> internalItems(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error, bool forExport) const;
    QList<QOrganizerItem> internalItemOccurrences(const QOrganizerItem& parentItem, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, bool includeExceptions, QOrganizerManager::Error* error) const;
    void addItemRecurrences(QList<QOrganizerItem>& sorted, const QOrganizerItem& c, const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, bool forExport) const;

    bool fixOccurrenceReferences(QOrganizerItem* item, QOrganizerManager::Error* error);
    bool typesAreRelated(const QString& occurrenceType, const QString& parentType);

    void performAsynchronousOperation(QOrganizerAbstractRequest* request);

    static QOrganizerItem generateOccurrence(const QOrganizerItem& parentItem, const QDateTime& rdate);

    QList<QDateTime> generateDateTimes(const QDateTime& initialDateTime, QOrganizerRecurrenceRule rrule, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount) const;
    void inferMissingCriteria(QOrganizerRecurrenceRule* rrule, const QDate& initialDate) const;
    bool inMultipleOfInterval(const QDate& date, const QDate& initialDate, QOrganizerRecurrenceRule::Frequency frequency, int interval, Qt::DayOfWeek firstDayOfWeek, int maxCount) const;
    QDate firstDateInPeriod(const QDate& date, QOrganizerRecurrenceRule::Frequency frequency, Qt::DayOfWeek firstDayOfWeek) const;
    QDate firstDateInNextPeriod(const QDate& date, QOrganizerRecurrenceRule::Frequency frequency, Qt::DayOfWeek firstDayOfWeek) const;
    QList<QDate> matchingDates(const QDate& periodStart, const QDate& periodEnd, const QOrganizerRecurrenceRule& rrule) const;
    QList<QDate> filterByPosition(const QList<QDate>& dates, const QSet<int> positions) const;
    bool itemHasReccurence(const QOrganizerItem& oi) const;

    QOrganizerItemMemoryEngineData* d;
    static QMap<QString, QOrganizerItemMemoryEngineData*> engineDatas;
};

QTM_END_NAMESPACE

#endif

