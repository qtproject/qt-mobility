/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** OrganizerItem: Nokia Corporation (qt-info@nokia.com)
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
** If you have questions regarding the use of this file, please organizerItem
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


#ifndef QORGANIZERITEMMANAGERENGINE_H
#define QORGANIZERITEMMANAGERENGINE_H

#include <QSharedData>
#include <QMap>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QObject>

#include "qorganizeritem.h"
#include "qorganizeritemdetaildefinition.h"
#include "qorganizeritemmanager.h"
#include "qorganizeritemabstractrequest.h"

QTM_BEGIN_NAMESPACE

class QOrganizerItemFilter;
class QOrganizerItemSortOrder;

class Q_CONTACTS_EXPORT QOrganizerItemManagerEngine : public QObject
{
    Q_OBJECT

public:
    QOrganizerItemManagerEngine() {}
    virtual void deref() = 0;

    /* URI reporting */
    virtual QString managerName() const;                       // e.g. "Symbian"
    virtual QMap<QString, QString> managerParameters() const;  // e.g. "filename=private.db"
    QString managerUri() const;

    /* Filtering */
    virtual QList<QOrganizerItemLocalId> organizerItems(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error& error) const;

    /* OrganizerItems - Accessors and Mutators */
    virtual QList<QOrganizerItemLocalId> organizerItems(const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error& error) const;
    virtual QOrganizerItem organizerItem(const QOrganizerItemLocalId& organizerItemId, QOrganizerItemManager::Error& error) const;

    virtual bool saveOrganizerItem(QOrganizerItem* organizerItem, QOrganizerItemManager::Error& error);
    virtual QList<QOrganizerItemManager::Error> saveOrganizerItems(QList<QOrganizerItem>* organizerItems, QOrganizerItemManager::Error& error);

    virtual bool removeOrganizerItem(const QOrganizerItemLocalId& organizerItemId, QOrganizerItemManager::Error& error);
    virtual QList<QOrganizerItemManager::Error> removeOrganizerItems(QList<QOrganizerItemLocalId>* organizerItemIds, QOrganizerItemManager::Error& error);

    /* Synthesize the display label of a organizerItem */
    virtual QString synthesizeDisplayLabel(const QOrganizerItem& organizerItem, QOrganizerItemManager::Error& error) const;
    QOrganizerItem setOrganizerItemDisplayLabel(const QString& displayLabel, const QOrganizerItem& organizerItem) const;

    /* Validation for saving */
    virtual bool validateOrganizerItem(const QOrganizerItem& organizerItem, QOrganizerItemManager::Error& error) const;
    virtual bool validateDefinition(const QOrganizerItemDetailDefinition& def, QOrganizerItemManager::Error& error) const;

    /* Definitions - Accessors and Mutators */
    virtual QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(const QString& organizerItemType, QOrganizerItemManager::Error& error) const;
    virtual QOrganizerItemDetailDefinition detailDefinition(const QString& definitionId, const QString& organizerItemType, QOrganizerItemManager::Error& error) const;
    virtual bool saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizerItemType, QOrganizerItemManager::Error& error);
    virtual bool removeDetailDefinition(const QString& definitionId, const QString& organizerItemType, QOrganizerItemManager::Error& error);

    /* Asynchronous Request Support */
    virtual void requestDestroyed(QOrganizerItemAbstractRequest* req);
    virtual bool startRequest(QOrganizerItemAbstractRequest* req);
    virtual bool cancelRequest(QOrganizerItemAbstractRequest* req);
    virtual bool waitForRequestProgress(QOrganizerItemAbstractRequest* req, int msecs);
    virtual bool waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs);
    static void updateRequestStatus(QOrganizerItemAbstractRequest* req, QOrganizerItemManager::Error error, QList<QOrganizerItemManager::Error>& errors, QOrganizerItemAbstractRequest::Status status, bool appendOnly = false);
    static void updateRequest(QOrganizerItemAbstractRequest* req, const QList<QOrganizerItemLocalId>& result, QOrganizerItemManager::Error error, const QList<QOrganizerItemManager::Error>& errors, QOrganizerItemAbstractRequest::Status status, bool appendOnly = false);
    static void updateRequest(QOrganizerItemAbstractRequest* req, const QList<QOrganizerItem>& result, QOrganizerItemManager::Error error, const QList<QOrganizerItemManager::Error>& errors, QOrganizerItemAbstractRequest::Status status, bool appendOnly = false);
    static void updateRequest(QOrganizerItemAbstractRequest* req, const QList<QOrganizerItemDetailDefinition>& result, QOrganizerItemManager::Error error, const QList<QOrganizerItemManager::Error>& errors, QOrganizerItemAbstractRequest::Status status);
    static void updateRequest(QOrganizerItemAbstractRequest* req, const QMap<QString, QOrganizerItemDetailDefinition>& result, QOrganizerItemManager::Error error, const QList<QOrganizerItemManager::Error>& errors, QOrganizerItemAbstractRequest::Status status, bool appendOnly = false);

    /* Capabilities reporting */
    virtual bool hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& organizerItemType) const;
    virtual bool filterSupported(const QOrganizerItemFilter& filter) const;
    virtual QList<QVariant::Type> supportedDataTypes() const;
    virtual QStringList supportedOrganizerItemTypes() const;
 
    /* Versions */ 
    static int version(); 
    virtual int implementationVersion() const; 

    /* Reports the built-in definitions from the schema */
    static QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > schemaDefinitions();

signals:
    void dataChanged();
    void organizerItemsAdded(const QList<QOrganizerItemLocalId>& organizerItemIds);
    void organizerItemsChanged(const QList<QOrganizerItemLocalId>& organizerItemIds);
    void organizerItemsRemoved(const QList<QOrganizerItemLocalId>& organizerItemIds);
    void selfOrganizerItemIdChanged(const QOrganizerItemLocalId& oldId, const QOrganizerItemLocalId& newId);

public:
    /* Helper functions */
    static int compareOrganizerItem(const QOrganizerItem& a, const QOrganizerItem& b, const QList<QOrganizerItemSortOrder>& sortOrders);
    static void addSorted(QList<QOrganizerItem>* sorted, const QOrganizerItem& toAdd, const QList<QOrganizerItemSortOrder>& sortOrders);
    static int compareVariant(const QVariant& first, const QVariant& second, Qt::CaseSensitivity sensitivity);
    static bool testFilter(const QOrganizerItemFilter& filter, const QOrganizerItem& organizerItem);
    static bool validateActionFilter(const QOrganizerItemFilter& filter);
    static QList<QOrganizerItemLocalId> sortOrganizerItems(const QList<QOrganizerItem>& organizerItems, const QList<QOrganizerItemSortOrder>& sortOrders);
    static void setOrganizerItemRelationships(QOrganizerItem* organizerItem, const QList<QOrganizerItemRelationship>& relationships);

private:
    /* QOrganizerItemChangeSet is a utility class used to emit the appropriate signals */
    friend class QOrganizerItemChangeSet;
};

QTM_END_NAMESPACE

#endif
