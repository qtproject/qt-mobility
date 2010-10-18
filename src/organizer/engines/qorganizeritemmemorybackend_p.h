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
#include "qorganizeritemmanager.h"
#include "qorganizeritemmanagerengine.h"
#include "qorganizeritemdetaildefinition.h"
#include "qorganizeritemabstractrequest.h"
#include "qorganizeritemchangeset.h"
#include "qorganizeritemrecurrencerule.h"
#include "qorganizeritemenginelocalid.h"
#include "qorganizercollectionenginelocalid.h"

QTM_BEGIN_NAMESPACE

class QOrganizerItemMemoryEngineLocalId : public QOrganizerItemEngineLocalId
{
public:
    QOrganizerItemMemoryEngineLocalId();
    QOrganizerItemMemoryEngineLocalId(quint32 collectionId, quint32 itemId);
    ~QOrganizerItemMemoryEngineLocalId();
    QOrganizerItemMemoryEngineLocalId(const QOrganizerItemMemoryEngineLocalId& other);

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

private:
    quint32 m_localCollectionId;
    quint32 m_localItemId;
    friend class QOrganizerItemMemoryEngine;
};

class QOrganizerCollectionMemoryEngineLocalId : public QOrganizerCollectionEngineLocalId
{
public:
    QOrganizerCollectionMemoryEngineLocalId();
    QOrganizerCollectionMemoryEngineLocalId(quint32 collectionId);
    ~QOrganizerCollectionMemoryEngineLocalId();
    QOrganizerCollectionMemoryEngineLocalId(const QOrganizerCollectionMemoryEngineLocalId& other);

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
    quint32 m_localCollectionId;
    friend class QOrganizerItemMemoryEngine;
};

class QOrganizerItemAbstractRequest;
class QOrganizerItemManagerEngine;
class QOrganizerItemMemoryEngineData : public QSharedData
{
public:
    QOrganizerItemMemoryEngineData();
    ~QOrganizerItemMemoryEngineData()
    {
    }

    QAtomicInt m_refCount;
    QString m_id;                                  // the id parameter value

    QList<QOrganizerItem> m_organizeritems;                      // list of organizer items
    QList<QOrganizerItemLocalId> m_organizeritemIds;             // list of organizer item Id's
    QList<QOrganizerCollection> m_organizerCollections;          // list of collections
    QList<QOrganizerCollectionLocalId> m_organizerCollectionIds; // list of collection ids
    QMultiMap<QOrganizerCollectionLocalId, QOrganizerItemLocalId> m_itemsInCollections; // map of collection ids to the ids of items the collection contains.
    QList<QString> m_definitionIds;                // list of definition types (id's)
    mutable QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > m_definitions; // map of organizer item type to map of definition name to definitions.
    quint32 m_nextOrganizerItemId; // the m_localItemId portion of a QOrganizerItemMemoryEngineLocalId.
    quint32 m_nextOrganizerCollectionId; // the m_localCollectionId portion of a QOrganizerCollectionMemoryEngineLocalId.
    bool m_anonymous;                              // Is this backend ever shared?

    void emitSharedSignals(QOrganizerItemChangeSet* cs)
    {
        foreach(QOrganizerItemManagerEngine* engine, m_sharedEngines)
            cs->emitSignals(engine);
    }

    QList<QOrganizerItemManagerEngine*> m_sharedEngines;   // The list of engines that share this data
};

class QOrganizerItemMemoryEngine : public QOrganizerItemManagerEngine
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

    virtual QList<QOrganizerItem> itemInstances(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const;
    virtual QList<QOrganizerItem> itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, QOrganizerItemManager::Error* error) const;

    virtual QList<QOrganizerItemLocalId> itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const;
    virtual QList<QOrganizerItem> items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const;
    virtual QOrganizerItem item(const QOrganizerItemLocalId& organizeritemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const;

    virtual bool saveItems(QList<QOrganizerItem>* organizeritems, const QOrganizerCollectionLocalId& collectionId, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error);
    virtual bool removeItems(const QList<QOrganizerItemLocalId>& organizeritemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error);

    virtual QOrganizerCollectionLocalId defaultCollectionId(QOrganizerItemManager::Error* error) const;
    virtual QList<QOrganizerCollectionLocalId> collectionIds(QOrganizerItemManager::Error* error) const;
    virtual QList<QOrganizerCollection> collections(const QList<QOrganizerCollectionLocalId>& collectionIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error) const;
    virtual bool saveCollection(QOrganizerCollection* collection, QOrganizerItemManager::Error* error);
    virtual bool removeCollection(const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error);

    /*! \reimp */
    virtual QOrganizerItem compatibleItem(const QOrganizerItem& original, QOrganizerItemManager::Error* error) const
    {
        return QOrganizerItemManagerEngine::compatibleItem(original, error);
    }
    /*! \reimp */
    virtual QOrganizerCollection compatibleCollection(const QOrganizerCollection& original, QOrganizerItemManager::Error* error) const;

    /*! \reimp */
    virtual bool validateItem(const QOrganizerItem& organizeritem, QOrganizerItemManager::Error* error) const
    {
        return QOrganizerItemManagerEngine::validateItem(organizeritem, error);
    }
    /*! \reimp */
    virtual bool validateCollection(const QOrganizerCollection& collection, QOrganizerItemManager::Error* error) const
    {
        Q_UNUSED(collection)
        *error = QOrganizerItemManager::NoError;
        return true; // all collections are valid in the memory engine.
    }
    /*! \reimp */
    virtual bool validateDefinition(const QOrganizerItemDetailDefinition& def, QOrganizerItemManager::Error* error) const
    {
        return QOrganizerItemManagerEngine::validateDefinition(def, error);
    }

    /* Definitions - Accessors and Mutators */
    virtual QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(const QString& organizeritemType, QOrganizerItemManager::Error* error) const;
    /*! \reimp */
    virtual QOrganizerItemDetailDefinition detailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemManager::Error* error) const
    {
        return QOrganizerItemManagerEngine::detailDefinition(definitionId, organizeritemType, error);
    }
    virtual bool saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemManager::Error* error);
    virtual bool removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemManager::Error* error);

    /* Asynchronous Request Support */
    virtual void requestDestroyed(QOrganizerItemAbstractRequest* req);
    virtual bool startRequest(QOrganizerItemAbstractRequest* req);
    virtual bool cancelRequest(QOrganizerItemAbstractRequest* req);
    virtual bool waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs);

    /* Capabilities reporting */
    virtual bool hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& organizeritemType) const;
    virtual bool isFilterSupported(const QOrganizerItemFilter& filter) const;
    virtual QList<int> supportedDataTypes() const;
    /*! \reimp */
    virtual QStringList supportedItemTypes() const
    {
        return QOrganizerItemManagerEngine::supportedItemTypes();
    }

protected:
    QOrganizerItemMemoryEngine(QOrganizerItemMemoryEngineData* data);

private:
    /* Implement "signal coalescing" for batch functions via change set */
    bool saveItem(QOrganizerItem* theOrganizerItem, const QOrganizerCollectionLocalId& collectionId, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error);
    bool fixOccurrenceReferences(QOrganizerItem* item, QOrganizerItemManager::Error* error);
    bool typesAreRelated(const QString& occurrenceType, const QString& parentType);
    bool removeItem(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error);
    bool saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error);
    bool removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error);

    void performAsynchronousOperation(QOrganizerItemAbstractRequest* request);

    static QOrganizerItem generateInstance(const QOrganizerItem& generator, const QDateTime& rdate);

    QList<QDateTime> generateDateTimes(const QDateTime& initialDateTime, QOrganizerItemRecurrenceRule rrule, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount) const;
    void inferMissingCriteria(QOrganizerItemRecurrenceRule* rrule, const QDate& initialDate) const;
    bool inIntervaledPeriod(const QDate& date, const QDate& initialDate, QOrganizerItemRecurrenceRule::Frequency frequency, int interval, Qt::DayOfWeek firstDayOfWeek) const;
    QDate firstDateInPeriod(const QDate& date, QOrganizerItemRecurrenceRule::Frequency frequency, Qt::DayOfWeek firstDayOfWeek) const;
    QDate firstDateInNextPeriod(const QDate& date, QOrganizerItemRecurrenceRule::Frequency frequency, Qt::DayOfWeek firstDayOfWeek) const;
    QList<QDate> matchingDates(const QDate& periodStart, const QDate& periodEnd, const QOrganizerItemRecurrenceRule& rrule) const;
    QList<QDate> filterByPosition(const QList<QDate>& dates, const QList<int> positions) const;

    QOrganizerItemMemoryEngineData* d;
    static QMap<QString, QOrganizerItemMemoryEngineData*> engineDatas;
};

QTM_END_NAMESPACE

#endif

