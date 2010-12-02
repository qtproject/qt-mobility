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

#ifndef QORGANIZERMANAGERENGINEV2WRAPPER_P_H
#define QORGANIZERMANAGERENGINEV2WRAPPER_P_H

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

#include <QPointer>
#include <QScopedPointer>
#include "qorganizermanagerengine.h"

QTM_USE_NAMESPACE

QTM_BEGIN_NAMESPACE

class RequestController;

class QOrganizerManagerEngineV2Wrapper : public QOrganizerManagerEngineV2
{
    Q_OBJECT
public:
    QOrganizerManagerEngineV2Wrapper(QOrganizerManagerEngine* wrappee);
    ~QOrganizerManagerEngineV2Wrapper();

    /* Extra functions */
    static void setEngineOfRequest(QOrganizerAbstractRequest* request, QOrganizerManagerEngine* engine);

    /* Overridden function */
    QList<QOrganizerItem> items(
            const QDateTime& startDate,
            const QDateTime& endDate,
            const QOrganizerItemFilter& filter,
            const QList<QOrganizerItemSortOrder>& sortOrders,
            const QOrganizerItemFetchHint& fetchHint,
            QOrganizerManager::Error* error) const;

    QList<QOrganizerItem> items(
            const QDateTime& startDate,
            const QDateTime& endDate,
            int maxCount,
            const QOrganizerItemFilter& filter,
            const QOrganizerItemFetchHint& fetchHint,
            QOrganizerManager::Error* error) const;

    /* Asynchronous Request Support */
    void requestDestroyed(QOrganizerAbstractRequest* req);
    bool startRequest(QOrganizerAbstractRequest* req);
    bool cancelRequest(QOrganizerAbstractRequest* req);
    bool waitForRequestFinished(QOrganizerAbstractRequest* req, int msecs);

    QString managerName() const {
        return m_engine->managerName();
    }
    QMap<QString, QString> managerParameters() const {
        return m_engine->managerParameters();
    }
    int managerVersion() const {
        return m_engine->managerVersion();
    }
    QList<QOrganizerItem> itemOccurrences(
            const QOrganizerItem& parentItem,
            const QDateTime& periodStart,
            const QDateTime& periodEnd,
            int maxCount,
            const QOrganizerItemFetchHint& fetchHint,
            QOrganizerManager::Error* error) const {
        return m_engine->itemOccurrences(
                parentItem, periodStart, periodEnd, maxCount, fetchHint, error);
    }
    QList<QOrganizerItemId> itemIds(
            const QDateTime& startDate,
            const QDateTime& endDate,
            const QOrganizerItemFilter& filter,
            const QList<QOrganizerItemSortOrder>& sortOrders,
            QOrganizerManager::Error* error) const {
        return m_engine->itemIds(startDate, endDate, filter, sortOrders, error);
    }
    QList<QOrganizerItem> itemsForExport(
            const QDateTime& startDate,
            const QDateTime& endDate,
            const QOrganizerItemFilter& filter,
            const QList<QOrganizerItemSortOrder>& sortOrders,
            const QOrganizerItemFetchHint& fetchHint,
            QOrganizerManager::Error* error) const {
        return m_engine->itemsForExport(
                startDate, endDate, filter, sortOrders, fetchHint, error);
    }
    QOrganizerItem item(
            const QOrganizerItemId& itemId,
            const QOrganizerItemFetchHint& fetchHint,
            QOrganizerManager::Error* error) const {
        return m_engine->item(itemId, fetchHint, error);
    }
    bool saveItem(QOrganizerItem* item, QOrganizerManager::Error* error) {
        return m_engine->saveItem(item, error);
    }
    bool removeItem(const QOrganizerItemId& itemId, QOrganizerManager::Error* error) {
        return m_engine->removeItem(itemId, error);
    }
    bool saveItems(
            QList<QOrganizerItem>* items,
            QMap<int, QOrganizerManager::Error>* errorMap,
            QOrganizerManager::Error* error) {
        return m_engine->saveItems(items, errorMap, error);
    }
    bool removeItems(
            const QList<QOrganizerItemId>& itemIds,
            QMap<int, QOrganizerManager::Error>* errorMap,
            QOrganizerManager::Error* error) {
        return m_engine->removeItems(itemIds, errorMap, error);
    }
    QOrganizerCollection defaultCollection(QOrganizerManager::Error* error) const {
        return m_engine->defaultCollection(error);
    }
    QOrganizerCollection collection(
            const QOrganizerCollectionId& collectionId,
            QOrganizerManager::Error* error) const {
        return m_engine->collection(collectionId, error);
    }
    QList<QOrganizerCollection> collections(QOrganizerManager::Error* error) const {
        return m_engine->collections(error);
    }
    bool saveCollection(
            QOrganizerCollection* collection,
            QOrganizerManager::Error* error) {
        return m_engine->saveCollection(collection, error);
    }
    bool removeCollection(
            const QOrganizerCollectionId& collectionId,
            QOrganizerManager::Error* error) {
        return m_engine->removeCollection(collectionId, error);
    }
    QOrganizerItem compatibleItem(
            const QOrganizerItem& original,
            QOrganizerManager::Error* error) const {
        return m_engine->compatibleItem(original, error);
    }
    QOrganizerCollection compatibleCollection(
            const QOrganizerCollection& original,
            QOrganizerManager::Error* error) const {
        return m_engine->compatibleCollection(original, error);
    }
    bool validateItem(const QOrganizerItem& item, QOrganizerManager::Error* error) const {
        return m_engine->validateItem(item, error);
    }
    bool validateCollection(
            const QOrganizerCollection& collection,
            QOrganizerManager::Error* error) const {
        return m_engine->validateCollection(collection, error);
    }
    bool validateDefinition(
            const QOrganizerItemDetailDefinition& def,
            QOrganizerManager::Error* error) const {
        return m_engine->validateDefinition(def, error);
    }
    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(
            const QString& itemType,
            QOrganizerManager::Error* error) const {
        return m_engine->detailDefinitions(itemType, error);
    }
    QOrganizerItemDetailDefinition detailDefinition(
            const QString& definitionId,
            const QString& itemType,
            QOrganizerManager::Error* error) const {
        return m_engine->detailDefinition(definitionId, itemType, error);
    }
    bool saveDetailDefinition(
            const QOrganizerItemDetailDefinition& def,
            const QString& itemType,
            QOrganizerManager::Error* error) {
        return m_engine->saveDetailDefinition(def, itemType, error);
    }
    bool removeDetailDefinition(
            const QString& definitionId,
            const QString& itemType,
            QOrganizerManager::Error* error) {
        return m_engine->removeDetailDefinition(definitionId, itemType, error);
    }
    bool hasFeature(
            QOrganizerManager::ManagerFeature feature,
            const QString& itemType) const {
        return m_engine->hasFeature(feature, itemType);
    }
    bool isFilterSupported(const QOrganizerItemFilter& filter) const {
        return m_engine->isFilterSupported(filter);
    }
    QStringList supportedItemTypes() const {
        return m_engine->supportedItemTypes();
    }

private Q_SLOTS:
    void requestStateChanged(QOrganizerAbstractRequest::State state);

private:
    QHash<QOrganizerAbstractRequest*, RequestController*> m_controllerForRequest;
    QOrganizerManagerEngine* m_engine;
};

class RequestController : public QObject {
    Q_OBJECT
public Q_SLOTS:
    void handleUpdatedSubRequest(QOrganizerAbstractRequest::State state);

Q_SIGNALS:
    void stateChanged(QOrganizerAbstractRequest::State state);

public:
    RequestController()
        : QObject(0), m_request(0), m_currentSubRequest(0), m_finished(false) {}

    void setRequest(QOrganizerAbstractRequest* request) { m_request = request; }
    QOrganizerAbstractRequest* request() { return m_request; }

    virtual bool start() = 0;
    bool waitForFinished(int msecs);

protected:
    virtual void handleFinishedSubRequest(QOrganizerAbstractRequest* req) = 0;
    void finish() { m_finished = true; }
    bool isFinished() { return m_finished; }

private:
    void handleUpdatedSubRequest(
            QOrganizerAbstractRequest* subReq, QOrganizerAbstractRequest::State state);

protected:
    QPointer<QOrganizerAbstractRequest> m_request;
    QScopedPointer<QOrganizerAbstractRequest> m_currentSubRequest;

private:
    bool m_finished;
};

class ItemFetchRequestController : public RequestController
{
    Q_OBJECT
public:
    ItemFetchRequestController(QOrganizerManagerEngine* engine)
        : RequestController(), m_engine(engine) {}
    bool start();

protected:
    void handleFinishedSubRequest(QOrganizerAbstractRequest* req);

    QOrganizerManagerEngine* m_engine;
};

QTM_END_NAMESPACE

#endif
