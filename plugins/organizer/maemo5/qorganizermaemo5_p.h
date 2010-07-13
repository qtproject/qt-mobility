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
#include "qorganizeritemdetaildefinition.h"
#include "qorganizeritemabstractrequest.h"
#include "qorganizeritemchangeset.h"

#include "qorganizeritemtransform.h"
#include "qorganizerguidmapper.h"

#include "qorganizerjournal.h"
#include "qorganizertodo.h"
#include "qorganizerevent.h"

#include <CMulticalendar.h>
#include <CEvent.h>
#include <CTodo.h>
#include <CJournal.h>
#include <CRecurrence.h>

QTM_USE_NAMESPACE

class QOrganizerItemMaemo5Factory : public QObject, public QOrganizerItemManagerEngineFactory
{
  Q_OBJECT
  Q_INTERFACES(QtMobility::QOrganizerItemManagerEngineFactory)
  public:
    QOrganizerItemManagerEngine* engine(const QMap<QString, QString>& parameters, QOrganizerItemManager::Error*);
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

    // key = QString(QLatin1String(CEvent.id().data()));
    // value = QOrganizerItemLocalId(qHash(key));
    QMap<QString, QOrganizerItemLocalId> m_cIdToQId; // TODO: This is not used anymore, remove

    // the cId consists of a calendar name and an item id
    // we need to be able to separate both of these parts.
    QMap<QString, QString> m_cIdToCName; // TODO: This is not used anymore, remove

    // the multicalendar instance
    CMulticalendar *m_mcInstance;

    // item converter instance
    OrganizerItemTransform m_itemTransformer;

    // guid mapper instance
    OrganizerGuidMapper m_guidMapper;
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

    QList<QOrganizerItem> itemInstances(const QOrganizerItem &generator, const QDateTime &periodStart, const QDateTime &periodEnd, int maxCount, QOrganizerItemManager::Error *error) const;
    QList<QOrganizerItemLocalId> itemIds(const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, QOrganizerItemManager::Error *error) const;

    QList<QOrganizerItem> items(const QOrganizerItemFilter &filter, const QList<QOrganizerItemSortOrder> &sortOrders, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const;
    QOrganizerItem item(const QOrganizerItemLocalId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error) const;

    bool saveItems(QList<QOrganizerItem> *items, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error *error);
    bool removeItems(const QList<QOrganizerItemLocalId> &itemIds, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error *error);

    /* Capabilities reporting */
    bool hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString &itemType) const;
    bool isFilterSupported(const QOrganizerItemFilter &filter) const;
    QList<QVariant::Type> supportedDataTypes() const;
    QStringList supportedItemTypes() const;

    /* Asynchronous Request Support */
    void requestDestroyed(QOrganizerItemAbstractRequest *req);
    bool startRequest(QOrganizerItemAbstractRequest *req);
    bool cancelRequest(QOrganizerItemAbstractRequest *req);
    bool waitForRequestFinished(QOrganizerItemAbstractRequest *req, int msecs);

public Q_SLOTS:
    void dataChanged();

private:
    // single item saving implementation
    void checkItemIdValidity(QOrganizerItem *checkItem, QOrganizerItemManager::Error *error);
    int doSaveItem(CCalendar *cal, QOrganizerItem *item, QOrganizerItemChangeSet &cs, QOrganizerItemManager::Error *error);

    // saving the occurrences
    int saveEventOccurrence(CCalendar *cal, QOrganizerEventOccurrence *occurrence, QOrganizerEvent *parent, QOrganizerItemChangeSet &cs, QOrganizerItemManager::Error *error);
    //int saveTodoOccurrence(QOrganizerTodoOccurrence *occurrence, QOrganizerEvent *parent, QOrganizerItemManager::Error *error);
    void insertOccurenceSortedByStartDate(QOrganizerItem *occurrence, QList<QOrganizerItem> &target) const;

    // getting the parent (base) item of an exception item (event occurrence or todo occurrence)
    QOrganizerItem parentOf(CCalendar *cal, QOrganizerItem *occurence, QOrganizerItemManager::Error *error) const;

    // internal fetch item
    QOrganizerItem internalFetchItem(const QOrganizerItemLocalId &itemId, const QOrganizerItemFetchHint &fetchHint, QOrganizerItemManager::Error *error, bool fetchOccurrences) const;

    // identifying native item as an occurrence
    bool isOccurrence(CCalendar *cal, CComponent *ccomponent, QString typeStr, QOrganizerItemManager::Error *error) const;
    bool containsRecurrenceInformation(CComponent *ccomponent) const;

    // returns a fetch hint for fetching minimal amount of information
    QOrganizerItemFetchHint fetchMinimalData() const;

    // calendar instance deletion helper
    void cleanupCal(CCalendar *cal) const;

    // ctor
    QOrganizerItemMaemo5Engine();

private:
    QOrganizerItemMaemo5EngineData *d;
    QTimer m_waitTimer;
    friend class QOrganizerItemMaemo5Factory;
};

#endif
