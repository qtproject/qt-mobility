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


#ifndef QORGANIZERITEMMANAGER_H
#define QORGANIZERITEMMANAGER_H

#include <QObject>

#include <QMap>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDateTime>

#include "qtorganizerglobal.h"
#include "qorganizeritem.h"
#include "qorganizeritemid.h"
#include "qorganizeritemsortorder.h"
#include "qorganizeritemfetchhint.h"
#include "qorganizeritemfilter.h"

#include "qorganizeritemcollection.h"

QTM_BEGIN_NAMESPACE

class QOrganizerItemAction;

class QOrganizerItemManagerData;
class Q_ORGANIZER_EXPORT QOrganizerItemManager : public QObject
{
    Q_OBJECT

public:
#if Q_QDOC // qdoc's parser fails to recognise the default map argument
    explicit QOrganizerItemManager(const QString& managerName = QString(), const QMap<QString, QString>& parameters = 0, QObject* parent = 0);
    QOrganizerItemManager(const QString& managerName, int implementationVersion, const QMap<QString, QString>& parameters = 0, QObject* parent = 0);
#else
    explicit QOrganizerItemManager(const QString& managerName = QString(), const QMap<QString, QString>& parameters = (QMap<QString, QString>()), QObject* parent = 0);
    QOrganizerItemManager(const QString& managerName, int implementationVersion, const QMap<QString, QString>& parameters = (QMap<QString, QString>()), QObject* parent = 0);
#endif
    explicit QOrganizerItemManager(QObject* parent);

    static QOrganizerItemManager* fromUri(const QString& uri, QObject* parent = 0);
    ~QOrganizerItemManager();

    QString managerName() const;                       // e.g. "Symbian"
    QMap<QString, QString> managerParameters() const;  // e.g. "filename=private.db"
    QString managerUri() const;                        // managerName + managerParameters
    int managerVersion() const;

    static bool parseUri(const QString& uri, QString* managerName, QMap<QString, QString>* params); // replaces the above.
    static QString buildUri(const QString& managerName, const QMap<QString, QString>& params, int implementationVersion = -1);

    /* The values of the Error enum are still to be decided! */
    enum Error {
        NoError = 0,
        DoesNotExistError,
        AlreadyExistsError,
        InvalidDetailError,
        LockedError,
        DetailAccessError,
        PermissionsError,
        OutOfMemoryError,
        NotSupportedError,
        BadArgumentError,
        UnspecifiedError,
        VersionMismatchError,
        LimitReachedError,
        InvalidItemTypeError
    };

    /* Error reporting */
    QOrganizerItemManager::Error error() const;

    /* Item Instances - both items and item occurrences (occurrences either saved or generated) */
    QList<QOrganizerItem> itemInstances(const QOrganizerItemFilter& filter = QOrganizerItemFilter(), const QList<QOrganizerItemSortOrder>& sortOrders = QList<QOrganizerItemSortOrder>(), const QOrganizerItemFetchHint& fetchHint = QOrganizerItemFetchHint()) const;
    QList<QOrganizerItem> itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart = QDateTime(), const QDateTime& periodEnd = QDateTime(), int maxCount = -1) const;
    // following two are unnecessary conveniences which might be expensive anyway.
    //QList<QOrganizerItem> nextItemInstance(const QDateTime& fromDate, const QOrganizerItemFilter& filter, const QOrganizerItemFetchHint& fetchHint) const;
    //QList<QOrganizerItem> nextItemInstance(const QDateTime& fromDate, const QOrganizerItem& recurrentItem, const QOrganizerItemFetchHint& fetchHint) const;

    /* Items - Accessors and Mutators */
    QList<QOrganizerItemLocalId> itemIds(const QList<QOrganizerItemSortOrder>& sortOrders = QList<QOrganizerItemSortOrder>()) const;
    QList<QOrganizerItemLocalId> itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders = QList<QOrganizerItemSortOrder>()) const;

    QList<QOrganizerItem> items(const QList<QOrganizerItemSortOrder>& sortOrders = QList<QOrganizerItemSortOrder>(), const QOrganizerItemFetchHint& fetchHint = QOrganizerItemFetchHint()) const;
    QList<QOrganizerItem> items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders = QList<QOrganizerItemSortOrder>(), const QOrganizerItemFetchHint& fetchHint = QOrganizerItemFetchHint()) const;
    QOrganizerItem item(const QOrganizerItemLocalId& itemId, const QOrganizerItemFetchHint& fetchHint = QOrganizerItemFetchHint()) const;  // retrieve an item

    bool saveItem(QOrganizerItem* item);
    bool saveItems(QList<QOrganizerItem>* items, QMap<int, QOrganizerItemManager::Error>* errorMap);
    bool removeItem(const QOrganizerItemLocalId& itemId);
    bool removeItems(const QList<QOrganizerItemLocalId>& itemIds, QMap<int, QOrganizerItemManager::Error>* errorMap);

    /* Collections - every item belongs to one or more collections */
    QOrganizerItemCollection defaultCollection() const;
    QList<QOrganizerItemCollection> collections() const;
    bool saveCollection(QOrganizerItemCollection* collection);
    bool removeCollection(const QOrganizerItemCollection& collection);

    /* Return a pruned or modified item which is valid and can be saved in the manager */
    QOrganizerItem compatibleItem(const QOrganizerItem& original); // Preliminary function!

    /* Definitions - Accessors and Mutators */
    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(const QString& itemType = QOrganizerItemType::TypeEvent) const;
    QOrganizerItemDetailDefinition detailDefinition(const QString& definitionName, const QString& itemType = QOrganizerItemType::TypeEvent) const;
    bool saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& itemType = QOrganizerItemType::TypeEvent);
    bool removeDetailDefinition(const QString& definitionName, const QString& itemType = QOrganizerItemType::TypeEvent);

    /* Functionality reporting */
    enum ManagerFeature {
        MutableDefinitions,
        Anonymous,
        ChangeLogs
    };
    bool hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& itemType = QOrganizerItemType::TypeEvent) const;
    QList<QVariant::Type> supportedDataTypes() const;
    bool isFilterSupported(const QOrganizerItemFilter& filter) const;
    QStringList supportedItemTypes() const;

    /* return a list of available backends for which a QOrganizerItemManager can be constructed. */
    static QStringList availableManagers();

Q_SIGNALS:
    void dataChanged();
    void itemsAdded(const QList<QOrganizerItemLocalId>& itemIds);
    void itemsChanged(const QList<QOrganizerItemLocalId>& itemIds);
    void itemsRemoved(const QList<QOrganizerItemLocalId>& itemIds);

private:
    friend class QOrganizerItemManagerData;
    void createEngine(const QString& managerName, const QMap<QString, QString>& parameters); 
    Q_DISABLE_COPY(QOrganizerItemManager)
    // private data pointer
    QOrganizerItemManagerData* d;
};

QTM_END_NAMESPACE

#endif
