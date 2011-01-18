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


#ifndef QORGANIZERMANAGERENGINE_H
#define QORGANIZERMANAGERENGINE_H

#include <QSharedData>
#include <QMap>
#include <QSet>
#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QObject>

#include "qorganizercollection.h"
#include "qorganizercollectionid.h"
#include "qorganizeritem.h"
#include "qorganizeritemdetaildefinition.h"
#include "qorganizermanager.h"
#include "qorganizerabstractrequest.h"
#include "qorganizeritemfetchhint.h"
#include "qorganizeritemrequests.h"

QTM_BEGIN_NAMESPACE

class QOrganizerItemFilter;
class QOrganizerItemSortOrder;
class QOrganizerItemEngineId;

/* Forward decls, since these don't actually exist yet */
class QOrganizerItemOccurrenceFetchRequest;
class QOrganizerItemIdFetchRequest;
class QOrganizerItemFetchRequest;
class QOrganizerItemRemoveRequest;
class QOrganizerItemSaveRequest;
class QOrganizerItemDetailDefinitionSaveRequest;
class QOrganizerItemDetailDefinitionRemoveRequest;
class QOrganizerItemDetailDefinitionFetchRequest;
class QOrganizerItemFetchForExportRequest;

class Q_ORGANIZER_EXPORT QOrganizerManagerEngine : public QObject
{
    Q_OBJECT

public:
    QOrganizerManagerEngine() {}

    /* URI reporting */
    virtual QString managerName() const = 0;                       // e.g. "Symbian"
    virtual QMap<QString, QString> managerParameters() const;  // e.g. "filename=private.db"
    virtual int managerVersion() const = 0;

    /* Default and only implementation of this */
    QString managerUri() const;

    /* Filtering */
    virtual QList<QOrganizerItem> itemOccurrences(const QOrganizerItem& parentItem, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;
    virtual QList<QOrganizerItemId> itemIds(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerManager::Error* error) const;
    virtual QList<QOrganizerItem> items(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;
    virtual QList<QOrganizerItem> itemsForExport(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;
    virtual QOrganizerItem item(const QOrganizerItemId& itemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;

    virtual bool saveItem(QOrganizerItem* item, QOrganizerManager::Error* error);
    virtual bool removeItem(const QOrganizerItemId& itemId, QOrganizerManager::Error* error);

    virtual bool saveItems(QList<QOrganizerItem>* items, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error);
    virtual bool removeItems(const QList<QOrganizerItemId>& itemIds, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error);

    /* Collections - every item belongs to exactly one collection */
    virtual QOrganizerCollection defaultCollection(QOrganizerManager::Error* error) const;
    virtual QOrganizerCollection collection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error) const;
    virtual QList<QOrganizerCollection> collections(QOrganizerManager::Error* error) const;
    virtual bool saveCollection(QOrganizerCollection* collection, QOrganizerManager::Error* error);
    virtual bool removeCollection(const QOrganizerCollectionId& collectionId, QOrganizerManager::Error* error);

    /* Return a pruned or modified item which is valid and can be saved in the backend */
    virtual QOrganizerItem compatibleItem(const QOrganizerItem& original, QOrganizerManager::Error* error) const;
    virtual QOrganizerCollection compatibleCollection(const QOrganizerCollection& original, QOrganizerManager::Error* error) const;

    /* Validation for saving */
    virtual bool validateItem(const QOrganizerItem& item, QOrganizerManager::Error* error) const;
    virtual bool validateCollection(const QOrganizerCollection& collection, QOrganizerManager::Error* error) const;
    virtual bool validateDefinition(const QOrganizerItemDetailDefinition& def, QOrganizerManager::Error* error) const;

    /* Definitions - Accessors and Mutators */
    virtual QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(const QString& itemType, QOrganizerManager::Error* error) const;
    virtual QOrganizerItemDetailDefinition detailDefinition(const QString& definitionId, const QString& itemType, QOrganizerManager::Error* error) const;
    virtual bool saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& itemType, QOrganizerManager::Error* error);
    virtual bool removeDetailDefinition(const QString& definitionId, const QString& itemType, QOrganizerManager::Error* error);

    /* Asynchronous Request Support */
    virtual void requestDestroyed(QOrganizerAbstractRequest* req);
    virtual bool startRequest(QOrganizerAbstractRequest* req);
    virtual bool cancelRequest(QOrganizerAbstractRequest* req);
    virtual bool waitForRequestFinished(QOrganizerAbstractRequest* req, int msecs);

    /* Capabilities reporting */
    virtual bool hasFeature(QOrganizerManager::ManagerFeature feature, const QString& itemType) const;
    virtual bool isFilterSupported(const QOrganizerItemFilter& filter) const;
    virtual QStringList supportedItemTypes() const;

    /* Reports the built-in definitions from the schema */
    static QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > schemaDefinitions(int version = 1);

Q_SIGNALS:
    void dataChanged();
    void itemsAdded(const QList<QOrganizerItemId>& itemIds);
    void itemsChanged(const QList<QOrganizerItemId>& itemIds);
    void itemsRemoved(const QList<QOrganizerItemId>& itemIds);
    void collectionsAdded(const QList<QOrganizerCollectionId>& collectionIds);
    void collectionsChanged(const QList<QOrganizerCollectionId>& collectionIds);
    void collectionsRemoved(const QList<QOrganizerCollectionId>& collectionIds);

public:
    // Async update functions
    static void updateRequestState(QOrganizerAbstractRequest* req, QOrganizerAbstractRequest::State state);

    static void updateItemOccurrenceFetchRequest(QOrganizerItemOccurrenceFetchRequest* req, const QList<QOrganizerItem>& result, QOrganizerManager::Error error, QOrganizerAbstractRequest::State newState);
    static void updateItemIdFetchRequest(QOrganizerItemIdFetchRequest* req, const QList<QOrganizerItemId>& result, QOrganizerManager::Error error, QOrganizerAbstractRequest::State newState);
    static void updateItemFetchRequest(QOrganizerItemFetchRequest* req, const QList<QOrganizerItem>& result, QOrganizerManager::Error error, QOrganizerAbstractRequest::State newState);
    static void updateItemFetchForExportRequest(QOrganizerItemFetchForExportRequest* req, const QList<QOrganizerItem>& result, QOrganizerManager::Error error, QOrganizerAbstractRequest::State newState);
    static void updateItemRemoveRequest(QOrganizerItemRemoveRequest* req, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState);
    static void updateItemSaveRequest(QOrganizerItemSaveRequest* req, const QList<QOrganizerItem>& result, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState);
    static void updateDefinitionSaveRequest(QOrganizerItemDetailDefinitionSaveRequest* req, const QList<QOrganizerItemDetailDefinition>& result, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState);
    static void updateDefinitionRemoveRequest(QOrganizerItemDetailDefinitionRemoveRequest* req, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState);
    static void updateDefinitionFetchRequest(QOrganizerItemDetailDefinitionFetchRequest* req, const QMap<QString, QOrganizerItemDetailDefinition>& result, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState);
    static void updateCollectionFetchRequest(QOrganizerCollectionFetchRequest* req, const QList<QOrganizerCollection>& result, QOrganizerManager::Error error, QOrganizerAbstractRequest::State newState);
    static void updateCollectionRemoveRequest(QOrganizerCollectionRemoveRequest* req, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState);
    static void updateCollectionSaveRequest(QOrganizerCollectionSaveRequest* req, const QList<QOrganizerCollection>& result, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State newState);

    // Other protected area update functions
    static void setDetailAccessConstraints(QOrganizerItemDetail* detail, QOrganizerItemDetail::AccessConstraints constraints);
    static const QOrganizerItemEngineId* engineItemId(const QOrganizerItemId& id);
    static const QOrganizerCollectionEngineId* engineCollectionId(const QOrganizerCollectionId& id);

    /* Helper functions */
    static bool isItemBetweenDates(const QOrganizerItem& item, const QDateTime& startPeriod, const QDateTime& endPeriod);
    static bool itemLessThan(const QOrganizerItem& a, const QOrganizerItem& b);
    static int compareItem(const QOrganizerItem& a, const QOrganizerItem& b, const QList<QOrganizerItemSortOrder>& sortOrders);
    static void addSorted(QList<QOrganizerItem>* sorted, const QOrganizerItem& toAdd, const QList<QOrganizerItemSortOrder>& sortOrders);
    static int compareVariant(const QVariant& first, const QVariant& second, Qt::CaseSensitivity sensitivity);
    static bool testFilter(const QOrganizerItemFilter& filter, const QOrganizerItem& item);

    static QOrganizerItemFilter canonicalizedFilter(const QOrganizerItemFilter& filter);

private:
    /* QOrganizerItemChangeSet is a utility class used to emit the appropriate signals */
    friend class QOrganizerItemChangeSet;
    friend class QOrganizerCollectionChangeSet;
};

class Q_ORGANIZER_EXPORT QOrganizerManagerEngineV2 : public QOrganizerManagerEngine
{
    Q_OBJECT
public:
    QOrganizerManagerEngineV2() : QOrganizerManagerEngine() {}

    // This is the V1 function - c++ overloading rules require this here, or to use "using"
    virtual QList<QOrganizerItem> items(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;
    virtual QList<QOrganizerItem> items(const QDateTime& startDate, const QDateTime& endDate, int maxCount, const QOrganizerItemFilter& filter, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;

    // Again, this is the v1 function
    QList<QOrganizerItem> itemsForExport(const QDateTime& startDate, const QDateTime& endDate, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerManager::Error* error) const;
    virtual QList<QOrganizerItem> itemsForExport(const QList<QOrganizerItemId>& itemIds, const QOrganizerItemFetchHint& fetchHint, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error) const;

    // Again, this is the v1 function
    bool saveItems(QList<QOrganizerItem>* items, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error);
    virtual bool saveItems(QList<QOrganizerItem>* items, const QStringList& definitionMask, QMap<int, QOrganizerManager::Error>* errorMap, QOrganizerManager::Error* error);

    static void updateItemFetchByIdRequest(QOrganizerItemFetchByIdRequest* req, const QList<QOrganizerItem>& result, QOrganizerManager::Error error, const QMap<int, QOrganizerManager::Error>& errorMap, QOrganizerAbstractRequest::State);


    // TODO async function
};

QTM_END_NAMESPACE

#endif
