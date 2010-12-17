/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include <qorganizeritemdetails.h>

#include "qdeclarativeorganizermodel_p.h"
#include "qorganizermanager.h"
#include "qversitreader.h"
#include "qversitwriter.h"
#include "qversitorganizerimporter.h"
#include "qversitorganizerexporter.h"
#include <QFile>

#include "qorganizeritemrequests.h"

static QString urlToLocalFileName(const QUrl& url)
{
   if (!url.isValid()) {
      return url.toString();
   } else if (url.scheme() == "qrc") {
      return url.toString().remove(0, 5).prepend(':');
   } else {
      return url.toLocalFile();
   }

}


class QDeclarativeOrganizerModelPrivate
{
public:
    QDeclarativeOrganizerModelPrivate()
        :m_manager(0),
        m_fetchHint(0),
        m_filter(0),
        m_fetchRequest(0),
        m_occurrenceFetchRequest(0),
        m_startPeriod(QDateTime::currentDateTime()),
        m_endPeriod(QDateTime::currentDateTime()),
        m_autoUpdate(true),
        m_updatePending(false),
        m_componentCompleted(false)
    {
    }
    ~QDeclarativeOrganizerModelPrivate()
    {
        if (m_manager)
            delete m_manager;
    }

    QList<QDeclarativeOrganizerItem*> m_items;
    QMap<QString, QDeclarativeOrganizerItem*> m_itemMap;
    QOrganizerManager* m_manager;
    QDeclarativeOrganizerItemFetchHint* m_fetchHint;
    QList<QOrganizerItemSortOrder> m_sortOrders;
    QList<QDeclarativeOrganizerItemSortOrder*> m_declarativeSortOrders;
    QDeclarativeOrganizerItemFilter* m_filter;
    QOrganizerItemFetchRequest* m_fetchRequest;
    QOrganizerItemOccurrenceFetchRequest* m_occurrenceFetchRequest;
    QList<QString> m_updatedItemIds;
    QStringList m_importProfiles;
    QVersitReader m_reader;
    QVersitWriter m_writer;
    QDateTime m_startPeriod;
    QDateTime m_endPeriod;

    bool m_autoUpdate;
    bool m_updatePending;
    bool m_componentCompleted;
};

/*!
    \qmlclass OrganizerModel QDeclarativeOrganizerModel
    \brief The OrganizerModel element provides access to organizer items from the organizer store.
    \ingroup qml-organizer

    This element is part of the \bold{QtMobility.organizer 1.1} module.

    OrganizerModel provides a model of organizer items from the organizer store.
    The contents of the model can be specified with \l filter, \l sortOrders and \l fetchHint properties.
    Whether the model is automatically updated when the store or \l organizer item changes, can be
    controlled with \l OrganizerModel::autoUpdate property.

    There are two ways of accessing the organizer item data: via model by using views and delegates,
    or alternatively via \l items list property. Of the two, the model access is preferred.
    Direct list access (i.e. non-model) is not guaranteed to be in order set by \l sortOrder.

    At the moment the model roles provided by OrganizerModel are display and \c item.
    Through the \c item role can access any data provided by the OrganizerItem element.

    \sa OrganizerItem, {QOrganizerManager}
*/

QDeclarativeOrganizerModel::QDeclarativeOrganizerModel(QObject *parent) :
    QAbstractListModel(parent),
    d(new QDeclarativeOrganizerModelPrivate)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(OrganizerItemRole, "item");
    setRoleNames(roleNames);

    connect(this, SIGNAL(managerChanged()), SLOT(update()));
    connect(this, SIGNAL(filterChanged()), SLOT(update()));
    connect(this, SIGNAL(fetchHintChanged()), SLOT(update()));
    connect(this, SIGNAL(sortOrdersChanged()), SLOT(update()));
    connect(this, SIGNAL(startPeriodChanged()), SLOT(update()));
    connect(this, SIGNAL(endPeriodChanged()), SLOT(update()));

    //import vcard
    connect(&d->m_reader, SIGNAL(stateChanged(QVersitReader::State)), this, SLOT(startImport(QVersitReader::State)));
}

/*!
  \qmlproperty string OrganizerModel::manager

  This property holds the manager name or manager uri of the organizer backend engine.
  The manager uri format: qtorganizer:<managerid>:<key>=<value>&<key>=<value>.

  \sa QOrganizerManager::fromUri()
  */
QString QDeclarativeOrganizerModel::manager() const
{
    if (d->m_manager)
        return d->m_manager->managerUri();
    return QString();
}

/*!
  \qmlproperty string OrganizerModel::managerName

  This property holds the manager name of the organizer backend engine.
  This property is read only.
  \sa QOrganizerManager::fromUri()
  */
QString QDeclarativeOrganizerModel::managerName() const
{
    if (d->m_manager)
        return d->m_manager->managerName();
    return QString();
}

/*!
  \qmlproperty list<string> OrganizerModel::availableManagers

  This property holds the list of available manager names.
  This property is read only.
  */
QStringList QDeclarativeOrganizerModel::availableManagers() const
{
    return QOrganizerManager::availableManagers();
}

/*!
  \qmlproperty bool OrganizerModel::autoUpdate

  This property indicates whether or not the organizer model should be updated automatically, default value is true.

  \sa OrganizerModel::update()
  */
void QDeclarativeOrganizerModel::setAutoUpdate(bool autoUpdate)
{
    if (autoUpdate == d->m_autoUpdate)
        return;
    d->m_autoUpdate = autoUpdate;
    emit autoUpdateChanged();
}

bool QDeclarativeOrganizerModel::autoUpdate() const
{
    return d->m_autoUpdate;
}

/*!
  \qmlmethod OrganizerModel::update()

  Manually update the organizer model content.

  \sa OrganizerModel::autoUpdate
  */
void QDeclarativeOrganizerModel::update()
{
    if (!d->m_componentCompleted || d->m_updatePending)
        return;

    d->m_updatePending = true; // Disallow possible duplicate request triggering
    QMetaObject::invokeMethod(this, "fetchAgain", Qt::QueuedConnection);
}

/*!
  \qmlmethod OrganizerModel::cancelUpdate()

  Cancel the running organizer model content update request.

  \sa OrganizerModel::autoUpdate  OrganizerModel::update
  */
void QDeclarativeOrganizerModel::cancelUpdate()
{
    if (d->m_fetchRequest) {
        d->m_fetchRequest->cancel();
        d->m_fetchRequest->deleteLater();
        d->m_fetchRequest = 0;
        d->m_updatePending = false;
    }
}
/*!
  \qmlproperty date OrganizerModel::startPeriod

  This property holds the start date and time period used by the organizer model to fetch organizer items.
  */
QDateTime QDeclarativeOrganizerModel::startPeriod() const
{
    return d->m_startPeriod;
}
void QDeclarativeOrganizerModel::setStartPeriod(const QDateTime& start)
{
    if (start != d->m_startPeriod) {
        d->m_startPeriod = start;
        emit startPeriodChanged();
    }
}

/*!
  \qmlproperty date OrganizerModel::endPeriod

  This property holds the end date and time period used by the organizer model to fetch organizer items.
  */
QDateTime QDeclarativeOrganizerModel::endPeriod() const
{
    return d->m_endPeriod;
}
void QDeclarativeOrganizerModel::setEndPeriod(const QDateTime& end)
{
    if (end != d->m_endPeriod) {
        d->m_endPeriod = end;
        emit endPeriodChanged();
    }
}

/*!
  \qmlmethod OrganizerModel::importItems(url url, list<string> profiles)

  Import organizer items from a vcalendar by the given \a url and optional \a profiles.
  */
void QDeclarativeOrganizerModel::importItems(const QUrl& url, const QStringList& profiles)
{
   d->m_importProfiles = profiles;
   //TODO: need to allow download vcard from network
   QFile*  file = new QFile(urlToLocalFileName(url));
   bool ok = file->open(QIODevice::ReadOnly);
   if (ok) {
      d->m_reader.setDevice(file);
      d->m_reader.startReading();
   }
}

/*!
  \qmlmethod OrganizerModel::exportItems(url url, list<string> profiles)
  Export organizer items into a vcalendar file to the given \a url by optional \a profiles.
  At the moment only the local file url is supported in export method.
  */
void QDeclarativeOrganizerModel::exportItems(const QUrl& url, const QStringList& profiles)
{
    QString profile = profiles.isEmpty()? QString() : profiles.at(0);

    QVersitOrganizerExporter exporter(profile);
    QList<QOrganizerItem> items;
    foreach (QDeclarativeOrganizerItem* di, d->m_items) {
        items.append(di->item());
    }

    exporter.exportItems(items, QVersitDocument::VCard30Type);
    QVersitDocument document = exporter.document();
    QFile* file = new QFile(urlToLocalFileName(url));
    bool ok = file->open(QIODevice::ReadWrite);
    if (ok) {
       d->m_writer.setDevice(file);
       d->m_writer.startWriting(document);
    }
}

void QDeclarativeOrganizerModel::itemsExported(QVersitWriter::State state)
{
    if (state == QVersitWriter::FinishedState || state == QVersitWriter::CanceledState) {
         delete d->m_writer.device();
         d->m_writer.setDevice(0);
    }
}

int QDeclarativeOrganizerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->m_items.count();
}

void QDeclarativeOrganizerModel::setManager(const QString& managerName)
{
    if (d->m_manager) {
        delete d->m_manager;
    }

    if (managerName.startsWith("qtorganizer:")) {
        d->m_manager = QOrganizerManager::fromUri(managerName, this);
    } else {
        d->m_manager = new QOrganizerManager(managerName, QMap<QString, QString>(), this);
    }

    connect(d->m_manager, SIGNAL(dataChanged()), this, SLOT(update()));
    connect(d->m_manager, SIGNAL(itemsAdded(QList<QOrganizerItemId>)), this, SLOT(update()));
    connect(d->m_manager, SIGNAL(itemsRemoved(QList<QOrganizerItemId>)), this, SLOT(itemsRemoved(QList<QOrganizerItemId>)));
    connect(d->m_manager, SIGNAL(itemsChanged(QList<QOrganizerItemId>)), this, SLOT(itemsChanged(QList<QOrganizerItemId>)));
    emit managerChanged();
}

void QDeclarativeOrganizerModel::componentComplete()
{
    d->m_componentCompleted = true;
    if (!d->m_manager)
        setManager(QString());

    if (d->m_autoUpdate)
        update();
}
/*!
  \qmlproperty Filter OrganizerModel::filter

  This property holds the filter instance used by the organizer model.

  \sa Filter
  */
QDeclarativeOrganizerItemFilter* QDeclarativeOrganizerModel::filter() const
{
    return d->m_filter;
}

void QDeclarativeOrganizerModel::setFilter(QDeclarativeOrganizerItemFilter* filter)
{
    if (filter && filter != d->m_filter) {
        if (d->m_filter)
            delete d->m_filter;
        d->m_filter = filter;
        connect(d->m_filter, SIGNAL(filterChanged()), this, SIGNAL(filterChanged()));
        emit filterChanged();
    }
}

/*!
  \qmlproperty FetchHint OrganizerModel::fetchHint

  This property holds the fetch hint instance used by the organizer model.

  \sa FetchHint
  */
QDeclarativeOrganizerItemFetchHint* QDeclarativeOrganizerModel::fetchHint() const
{
    return d->m_fetchHint;
}

void QDeclarativeOrganizerModel::setFetchHint(QDeclarativeOrganizerItemFetchHint* fetchHint)
{
    if (fetchHint && fetchHint != d->m_fetchHint) {
        if (d->m_fetchHint)
            delete d->m_fetchHint;
        d->m_fetchHint = fetchHint;
        connect(d->m_fetchHint, SIGNAL(fetchHintChanged()), this, SIGNAL(fetchHintChanged()));
        emit fetchHintChanged();
    }
}
/*!
  \qmlproperty int OrganizerModel::itemCount

  This property holds the size of organizer items the OrganizerModel currently holds.

  This property is read only.
  */
int QDeclarativeOrganizerModel::itemCount() const
{
    return d->m_items.size();
}
/*!
  \qmlproperty string OrganizerModel::error

  This property holds the latest error code returned by the organizer manager.

  This property is read only.
  */
QString QDeclarativeOrganizerModel::error() const
{
    if (d->m_manager) {
        switch (d->m_manager->error()) {
        case QOrganizerManager::DoesNotExistError:
            return QLatin1String("DoesNotExist");
        case QOrganizerManager::AlreadyExistsError:
            return QLatin1String("AlreadyExists");
        case QOrganizerManager::InvalidDetailError:
            return QLatin1String("InvalidDetail");
        case QOrganizerManager::InvalidCollectionError:
            return QLatin1String("InvalidCollection");
        case QOrganizerManager::LockedError:
            return QLatin1String("LockedError");
        case QOrganizerManager::DetailAccessError:
            return QLatin1String("DetailAccessError");
        case QOrganizerManager::PermissionsError:
            return QLatin1String("PermissionsError");
        case QOrganizerManager::OutOfMemoryError:
            return QLatin1String("OutOfMemory");
        case QOrganizerManager::NotSupportedError:
            return QLatin1String("NotSupported");
        case QOrganizerManager::BadArgumentError:
            return QLatin1String("BadArgument");
        case QOrganizerManager::UnspecifiedError:
            return QLatin1String("UnspecifiedError");
        case QOrganizerManager::VersionMismatchError:
            return QLatin1String("VersionMismatch");
        case QOrganizerManager::LimitReachedError:
            return QLatin1String("LimitReached");
        case QOrganizerManager::InvalidItemTypeError:
            return QLatin1String("InvalidItemType");
            case QOrganizerManager::InvalidOccurrenceError:
                return QLatin1String("InvalidOccurrence");
        default:
            break;
        }
    }
    return QLatin1String("NoError");
}

/*!
  \qmlproperty QDeclarativeListProperty OrganizerModel::sortOrders

  This property holds a list of sort orders used by the organizer model.

  \sa SortOrder
  */
QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder> QDeclarativeOrganizerModel::sortOrders()
{
    return QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder>(this,
                                                                        0,
                                                                        sortOrder_append,
                                                                        sortOrder_count,
                                                                        sortOrder_at,
                                                                        sortOrder_clear);
}

void QDeclarativeOrganizerModel::startImport(QVersitReader::State state)
{
    if (state == QVersitReader::FinishedState || state == QVersitReader::CanceledState) {
        if (!d->m_reader.results().isEmpty()) {
            QVersitOrganizerImporter importer;

            importer.importDocument(d->m_reader.results().at(0));
            QList<QOrganizerItem> items = importer.items();
            delete d->m_reader.device();
            d->m_reader.setDevice(0);


            if (d->m_manager) {
                if (d->m_manager->saveItems(&items)) {
                    update();
                }
            }
        }
    }
}

bool QDeclarativeOrganizerModel::itemHasReccurence(const QOrganizerItem& oi) const
{
    if (oi.type() == QOrganizerItemType::TypeEvent || oi.type() == QOrganizerItemType::TypeTodo) {
        QOrganizerItemRecurrence recur = oi.detail(QOrganizerItemRecurrence::DefinitionName);
        return !recur.recurrenceDates().isEmpty() || !recur.recurrenceRules().isEmpty();
    }

    return false;
}
void QDeclarativeOrganizerModel::fetchOccurrences(const QOrganizerItem& item)
{
    QOrganizerItemOccurrenceFetchRequest* req =  new QOrganizerItemOccurrenceFetchRequest(this);
    req->setManager(d->m_manager);
    req->setStartDate(d->m_startPeriod);
    req->setEndDate(d->m_endPeriod);
    req->setFetchHint(d->m_fetchHint ? d->m_fetchHint->fetchHint() : QOrganizerItemFetchHint());
    req->setParentItem(item);

    connect(req, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)), this, SLOT(requestUpdated()));
    req->start();

}

void QDeclarativeOrganizerModel::addSorted(QDeclarativeOrganizerItem* item)
{
    removeItemsFromModel(QList<QString>() << item->itemId());
    int idx = itemIndex(item);
    beginInsertRows(QModelIndex(), idx, idx + 1);
    d->m_items.insert(idx, item);
    endInsertRows();
    d->m_itemMap.insert(item->itemId(), item);

    if (itemHasReccurence(item->item())) {
        foreach (QDeclarativeOrganizerItem* di, d->m_items) {
            if (di->isOccurrence()) {
                 QOrganizerItemParent oip = di->item().detail<QOrganizerItemParent>();
                 if (oip.parentId() == item->item().id()) {
                     //delete this occurrence item, we will refetch it
                     d->m_itemMap.remove(di->itemId());
                     d->m_items.removeOne(di);
                     di->deleteLater();
                 }
            }
        }

        fetchOccurrences(item->item());
    }
}

int QDeclarativeOrganizerModel::itemIndex(const QDeclarativeOrganizerItem* item)
{
    if (d->m_sortOrders.count() > 0) {
        for (int i = 0; i < d->m_items.size(); i++) {
            // check to see if the new item should be inserted here
            int comparison = QOrganizerManagerEngine::compareItem(d->m_items.at(i)->item(),
                                                                  item->item(),
                                                                  d->m_sortOrders);
            if (comparison > 0) {
                return i;
            }
        }
    }
    return d->m_items.size();
}

void QDeclarativeOrganizerModel::clearItems()
{
    qDeleteAll(d->m_items);
    d->m_items.clear();
    d->m_itemMap.clear();
}

QDeclarativeOrganizerItem* QDeclarativeOrganizerModel::createItem(const QOrganizerItem& item)
{
    QDeclarativeOrganizerItem* di;
    if (item.type() == QOrganizerItemType::TypeEvent)
        di = new QDeclarativeOrganizerEvent(this);
    else if (item.type() == QOrganizerItemType::TypeEventOccurrence)
        di = new QDeclarativeOrganizerEventOccurrence(this);
    else if (item.type() == QOrganizerItemType::TypeTodo)
        di = new QDeclarativeOrganizerTodo(this);
    else if (item.type() == QOrganizerItemType::TypeTodoOccurrence)
        di = new QDeclarativeOrganizerTodoOccurrence(this);
    else if (item.type() == QOrganizerItemType::TypeJournal)
        di = new QDeclarativeOrganizerJournal(this);
    else if (item.type() == QOrganizerItemType::TypeNote)
        di = new QDeclarativeOrganizerNote(this);
    else
        di = new QDeclarativeOrganizerItem(this);
    di->setItem(item);
    di->setDetailDefinitions(d->m_manager->detailDefinitions(item.type()));
    return di;
}

/*!
  \qmlmethod OrganizerModel::fetchItems(list<QString> itemIds)
  Fetch a list of organizer items from the organizer store by given \a itemIds.
  */
void QDeclarativeOrganizerModel::fetchItems(const QList<QString>& itemIds)
{
    d->m_updatedItemIds = itemIds;
    d->m_updatePending = true;
    QMetaObject::invokeMethod(this, "fetchAgain", Qt::QueuedConnection);
}

/*!
  \qmlmethod bool OrganizerModel::containsItems(date start, date end)
  Returns true if there is at least one OrganizerItem between the given date range,
  otherwise returns false.  The second parameter \a end is optional.

  \since organizer 1.1.1
  */
bool QDeclarativeOrganizerModel::containsItems(QDateTime start, QDateTime end)
{
    //TODO: quick search this
    end = end.isNull()? start.addDays(1):end;
    foreach (const QDeclarativeOrganizerItem* item, d->m_items) {

        if  ( (item->itemStartTime() >= start && item->itemStartTime() <= end)
                ||
              (item->itemEndTime() >= start && item->itemEndTime() <= end)
                ||
              (item->itemEndTime() > end && item->itemStartTime() < start))
            return true;
    }
    return false;
}

/*!
  \qmlmethod OrganizerItem OrganizerModel::item(string itemId)
  Returns the OrganizerItem object which item id is the given \a itemId.

  \since organizer 1.1.1
  */
QDeclarativeOrganizerItem* QDeclarativeOrganizerModel::item(const QString& id)
{

    if (d->m_itemMap.contains(id))
        return d->m_itemMap.value(id);
    return 0;
}

/*!
  \qmlmethod list<string> OrganizerModel::itemIds(date start, date end)
  Returns the list of organizer item ids between the given date range \a start and \a end,
  the \a end parameter is optional.

  \since organizer 1.1.1
  */
QStringList QDeclarativeOrganizerModel::itemIds(QDateTime start, QDateTime end)
{
    //TODO: quick search this
    QStringList ids;
    end = end.isNull()? start.addDays(1):end;
    foreach (QDeclarativeOrganizerItem* item, d->m_items) {
        if ( (item->itemStartTime() >= start && item->itemEndTime() <= end)
             ||
             (item->itemEndTime() >= start && item->itemEndTime() <= end)
             ||
             (item->itemEndTime() > end && item->itemStartTime() < start))
            ids << item->itemId();
    }

    return ids;
}

void QDeclarativeOrganizerModel::fetchAgain()
{
    cancelUpdate();
    if (d->m_updatedItemIds.isEmpty()) //fetch all items
        clearItems();

    d->m_fetchRequest  = new QOrganizerItemFetchRequest(this);
    d->m_fetchRequest->setManager(d->m_manager);
    d->m_fetchRequest->setSorting(d->m_sortOrders);
    d->m_fetchRequest->setStartDate(d->m_startPeriod);
    d->m_fetchRequest->setEndDate(d->m_endPeriod);

    if (!d->m_updatedItemIds.isEmpty()) {
        QOrganizerItemIdFilter f;
        QList<QOrganizerItemId> ids;
        foreach (const QString& id, d->m_updatedItemIds) {
            ids << QOrganizerItemId::fromString(id);
        }

        f.setIds(ids);
        d->m_fetchRequest->setFilter(f);
        d->m_updatedItemIds.clear();
    } else if (d->m_filter){
        d->m_fetchRequest->setFilter(d->m_filter->filter());
    } else {
        d->m_fetchRequest->setFilter(QOrganizerItemFilter());
    }

    d->m_fetchRequest->setFetchHint(d->m_fetchHint ? d->m_fetchHint->fetchHint() : QOrganizerItemFetchHint());

    connect(d->m_fetchRequest, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)), this, SLOT(requestUpdated()));
    d->m_fetchRequest->start();
}

/*
  This slot function is connected with item fetch requests and item occurrence fetch requests,
  so the QObject::sender() must be checked for the right sender type.
  During update() function, the fetchAgain() will be invoked, inside fetchAgain(), a QOrganizerItemFetchRequest object
  is created and started, when this fetch request finished, this requestUpdate() slot will be invoked for the first time.
  Then check each of the organizer items returned by the item fetch request, if the item is a recurrence item,
  a QOrganizerItemOccurrenceFetchRequest object will be created and started. When each of these occurrence fetch requests
  finishes, this requestUpdated() slot will be invoked again and insert the returned occurrence items into the d->m_items
  list.
  */
void QDeclarativeOrganizerModel::requestUpdated()
{
    QList<QOrganizerItem> items;
    QOrganizerItemFetchRequest* ifr = qobject_cast<QOrganizerItemFetchRequest*>(QObject::sender());
    if (ifr && ifr->isFinished()) {
        items = ifr->items();
        ifr->deleteLater();
        d->m_fetchRequest = 0;
        d->m_updatePending = false;
    } else {
        QOrganizerItemOccurrenceFetchRequest* iofr = qobject_cast<QOrganizerItemOccurrenceFetchRequest*>(QObject::sender());
        if (iofr && iofr->isFinished()) {
            items = iofr->itemOccurrences();
            iofr->deleteLater();
        }
    }

    if (!items.isEmpty()) {
        if (d->m_items.isEmpty()) {
            QDeclarativeOrganizerItem* di;
            foreach (const QOrganizerItem& item, items) {
                di = createItem(item);
                addSorted(di);
            }
        } else {
            //Partial updating, insert the fetched items into the the exist item list.
            foreach (const QOrganizerItem& item, items) {
                QDeclarativeOrganizerItem* di;
                if (d->m_itemMap.contains(item.id().toString())) {
                    di = d->m_itemMap.value(item.id().toString());
                    di->setItem(item);
                } else {
                    di = createItem(item);
                }
                addSorted(di);
            }
        }
        emit itemsChanged();
        emit errorChanged();
    }
}

/*!
  \qmlmethod OrganizerModel::saveItem(OrganizerItem item)
  Saves the given \a item into the organizer backend.

  \since organizer 1.1.1
  */
void QDeclarativeOrganizerModel::saveItem(QDeclarativeOrganizerItem* di)
{
    if (di) {
        QOrganizerItem item = di->item();
        QOrganizerItemSaveRequest* req = new QOrganizerItemSaveRequest(this);
        req->setManager(d->m_manager);
        req->setItem(item);

        connect(req,SIGNAL(stateChanged(QOrganizerAbstractRequest::State)), this, SLOT(itemsSaved()));

        req->start();
    }
}

void QDeclarativeOrganizerModel::itemsSaved()
{
    QOrganizerItemSaveRequest* req = qobject_cast<QOrganizerItemSaveRequest*>(QObject::sender());
    if (req->isFinished()) {
        if (req->error() == QOrganizerManager::NoError) {
            QList<QOrganizerItem> items = req->items();
            QDeclarativeOrganizerItem* di;
            foreach (const QOrganizerItem& item, items) {
                QString itemId = item.id().toString();
                if (d->m_itemMap.contains(itemId)) {
                    di = d->m_itemMap.value(itemId);
                    di->setItem(item);
                } else {
                    //new saved item
                    di = createItem(item);
                    d->m_itemMap.insert(itemId, di);
                    beginInsertRows(QModelIndex(), d->m_items.count(), d->m_items.count() + 1);
                    d->m_items.append(di);
                    endInsertRows();
                }
                addSorted(di);
            }
        }
        req->deleteLater();
        emit errorChanged();
    }
}


/*!
  \qmlmethod OrganizerModel::removeItem(string itemId)
  Removes the organizer item with the given \a itemId from the backend.

  \since organizer 1.1.1
  */
void QDeclarativeOrganizerModel::removeItem(const QString& id)
{
    QList<QString> ids;
    ids << id;
    removeItems(ids);
}

/*!
  \qmlmethod OrganizerModel::removeItem(list<string> itemId)
  Removes the organizer items with the given \a ids from the backend.

  \since organizer 1.1.1
  */
void QDeclarativeOrganizerModel::removeItems(const QList<QString>& ids)
{
    QOrganizerItemRemoveRequest* req = new QOrganizerItemRemoveRequest(this);
    req->setManager(d->m_manager);
    QList<QOrganizerItemId> oids;

    foreach (const QString& id, ids) {
        if (id.startsWith(QString("qtorganizer:occurrence"))) {
            qWarning() << "Can't remove an occurrence item, please modify the parent item's recurrence rule instead!";
            continue;
        }
        QOrganizerItemId itemId = QOrganizerItemId::fromString(id);
        if (!itemId.isNull()) {
             oids.append(itemId);
        }
    }

    req->setItemIds(oids);

    connect(req,SIGNAL(stateChanged(QOrganizerAbstractRequest::State)), this, SLOT(itemsRemoved()));

    req->start();
}

void QDeclarativeOrganizerModel::itemsChanged(const QList<QOrganizerItemId>& ids)
{
    if (d->m_autoUpdate) {
        QList<QString> updatedIds;
        foreach (const QOrganizerItemId& id, ids) {
            if (d->m_itemMap.contains(id.toString())) {
                updatedIds << id.toString();
            }
        }

        if (updatedIds.count() > 0)
            fetchItems(updatedIds);
    }
}

void QDeclarativeOrganizerModel::itemsRemoved()
{
    if (d->m_autoUpdate) {
        QOrganizerItemRemoveRequest* req = qobject_cast<QOrganizerItemRemoveRequest*>(QObject::sender());


        if (req->isFinished()) {
            QList<QOrganizerItemId> ids = req->itemIds();
            QList<int> errorIds = req->errorMap().keys();
            QList<QOrganizerItemId> removedIds;
            for (int i = 0; i < ids.count(); i++) {
                if (!errorIds.contains(i))
                    removedIds << ids.at(i);
            }
            if (!removedIds.isEmpty())
                itemsRemoved(removedIds);
            req->deleteLater();
        }
    }
}

void QDeclarativeOrganizerModel::removeItemsFromModel(const QList<QString>& ids)
{
    bool emitSignal = false;
    foreach (const QString& itemId, ids) {
        if (d->m_itemMap.contains(itemId)) {
            int row = 0;
            //TODO:need a fast lookup
            for (; row < d->m_items.count(); row++) {
                if (d->m_items.at(row)->itemId() == itemId)
                    break;
            }

            if (row < d->m_items.count()) {
                beginRemoveRows(QModelIndex(), row, row);
                d->m_items.removeAt(row);
                d->m_itemMap.remove(itemId);
                endRemoveRows();
                emitSignal = true;
            }
        }
    }
    emit errorChanged();
    if (emitSignal)
        emit itemsChanged();
}

void QDeclarativeOrganizerModel::itemsRemoved(const QList<QOrganizerItemId>& ids)
{
    if (!ids.isEmpty()) {
        QList<QString> idStrings;
        foreach (const QOrganizerItemId& id, ids) {
            idStrings << id.toString();
        }
        removeItemsFromModel(idStrings);
    }
}



QVariant QDeclarativeOrganizerModel::data(const QModelIndex &index, int role) const
{
    QDeclarativeOrganizerItem* di = d->m_items.value(index.row());
    QOrganizerItem item = di->item();
    switch(role) {
        case Qt::DisplayRole:
            return item.displayLabel();
        case Qt::DecorationRole:
            //return pixmap for this item type
        case OrganizerItemRole:
            return QVariant::fromValue(di);
    }
    return QVariant();
}

/*!
  \qmlproperty QDeclarativeListProperty OrganizerModel::items

  This property holds a list of organizer items in the organizer model.

  \sa OrganizerItem
  */
QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::items()
{
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, 0, item_append, item_count, item_at, item_clear);
}

/*!
  \qmlproperty QDeclarativeListProperty OrganizerModel::occurrences

  This property holds a list of event or todo occurrence items in the organizer model.

  \sa Event, Todo, EventOccurrence, TodoOccurrence
  */
QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::occurrences()
{
    //TODO:XXX
    qWarning() << "OrganizerModel: occurrences is not currently supported.";
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>();
}

/*!
  \qmlproperty QDeclarativeListProperty OrganizerModel::events

  This property holds a list of events in the organizer model.

  \sa Event
  */
QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::events()
{
    void* d = const_cast<char*>(QOrganizerItemType::TypeEvent.latin1());
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d, item_append, item_count, item_at, item_clear);
}

/*!
  \qmlproperty QDeclarativeListProperty OrganizerModel::eventOccurrences

  This property holds a list of event occurrences in the organizer model.

  \sa EventOccurrence
  */
QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::eventOccurrences()
{
    void* d = const_cast<char*>(QOrganizerItemType::TypeEventOccurrence.latin1());
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d, item_append, item_count, item_at, item_clear);
}

/*!
  \qmlproperty QDeclarativeListProperty OrganizerModel::todos

  This property holds a list of todos in the organizer model.

  \sa Todo
  */
QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::todos()
{
    void* d = const_cast<char*>(QOrganizerItemType::TypeTodo.latin1());
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d, item_append, item_count, item_at, item_clear);
}

/*!
  \qmlproperty QDeclarativeListProperty OrganizerModel::todoOccurrences

  This property holds a list of todo occurrences in the organizer model.

  \sa TodoOccurrence
  */
QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::todoOccurrences()
{
    void* d = const_cast<char*>(QOrganizerItemType::TypeTodoOccurrence.latin1());
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d, item_append, item_count, item_at, item_clear);
}

/*!
  \qmlproperty QDeclarativeListProperty OrganizerModel::journals

  This property holds a list of journal items in the organizer model.

  \sa Journal
  */
QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::journals()
{
    void* d = const_cast<char*>(QOrganizerItemType::TypeJournal.latin1());
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d, item_append, item_count, item_at, item_clear);
}


/*!
  \qmlproperty QDeclarativeListProperty OrganizerModel::notes

  This property holds a list of note items in the organizer model.

  \sa Note
  */
QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::notes()
{
    void* d = const_cast<char*>(QOrganizerItemType::TypeNote.latin1());
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d, item_append, item_count, item_at, item_clear);
}


void QDeclarativeOrganizerModel::item_append(QDeclarativeListProperty<QDeclarativeOrganizerItem> *p, QDeclarativeOrganizerItem *item)
{
    Q_UNUSED(p);
    Q_UNUSED(item);
    qWarning() << "OrganizerModel: appending items is not currently supported";
}

int  QDeclarativeOrganizerModel::item_count(QDeclarativeListProperty<QDeclarativeOrganizerItem> *p)
{
    QString type((const char*)(p->data));
    QDeclarativeOrganizerModel* model = qobject_cast<QDeclarativeOrganizerModel*>(p->object);
    int count = 0;

    if (model) {
        if (!type.isEmpty()) {
            foreach (const QDeclarativeOrganizerItem* item, model->d->m_items) {
                if (item->item().type() == type)
                    count++;
            }
        } else {
            return model->d->m_items.count();
        }
    }
    return count;
}

QDeclarativeOrganizerItem * QDeclarativeOrganizerModel::item_at(QDeclarativeListProperty<QDeclarativeOrganizerItem> *p, int idx)
{
    QString type((const char*)(p->data));
    QDeclarativeOrganizerModel* model = qobject_cast<QDeclarativeOrganizerModel*>(p->object);

    QDeclarativeOrganizerItem* item = 0;
    if (model && idx < model->d->m_items.size()) {
        int i = 0;
        if (!type.isEmpty()) {
            foreach (QDeclarativeOrganizerItem* di, model->d->m_items) {
                if (di->item().type() == type) {
                    if (i == idx) {
                        item = di;
                        break;
                    } else {
                        i++;
                    }
                }
            }
        } else {
            item = model->d->m_items.at(idx);
        }
    }
    return item;
}

void  QDeclarativeOrganizerModel::item_clear(QDeclarativeListProperty<QDeclarativeOrganizerItem> *p)
{
    QString type((const char*)(p->data));
    QDeclarativeOrganizerModel* model = qobject_cast<QDeclarativeOrganizerModel*>(p->object);

    if (model) {
        if (!type.isEmpty()) {
            foreach (QDeclarativeOrganizerItem* di, model->d->m_items) {
                if (di->item().type() == type) {
                    di->deleteLater();
                    model->d->m_items.removeAll(di);
                }
            }
        } else {
            model->d->m_items.clear();
        }
        emit model->itemsChanged();
    }
}

void QDeclarativeOrganizerModel::sortOrder_append(QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder> *p, QDeclarativeOrganizerItemSortOrder *sortOrder)
{
    QDeclarativeOrganizerModel* model = qobject_cast<QDeclarativeOrganizerModel*>(p->object);
    if (model && sortOrder) {
        QObject::connect(sortOrder, SIGNAL(sortOrderChanged()), model, SIGNAL(sortOrdersChanged()));
        model->d->m_declarativeSortOrders.append(sortOrder);
        model->d->m_sortOrders.append(sortOrder->sortOrder());
        emit model->sortOrdersChanged();
    }
}

int  QDeclarativeOrganizerModel::sortOrder_count(QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder> *p)
{
    QDeclarativeOrganizerModel* model = qobject_cast<QDeclarativeOrganizerModel*>(p->object);
    if (model)
        return model->d->m_declarativeSortOrders.size();
    return 0;
}
QDeclarativeOrganizerItemSortOrder * QDeclarativeOrganizerModel::sortOrder_at(QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder> *p, int idx)
{
    QDeclarativeOrganizerModel* model = qobject_cast<QDeclarativeOrganizerModel*>(p->object);

    QDeclarativeOrganizerItemSortOrder* sortOrder = 0;
    if (model) {
        int i = 0;
        foreach (QDeclarativeOrganizerItemSortOrder* s, model->d->m_declarativeSortOrders) {
            if (i == idx) {
                sortOrder = s;
                break;
            } else {
                i++;
            }
        }
    }
    return sortOrder;
}
void  QDeclarativeOrganizerModel::sortOrder_clear(QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder> *p)
{
    QDeclarativeOrganizerModel* model = qobject_cast<QDeclarativeOrganizerModel*>(p->object);

    if (model) {
        model->d->m_sortOrders.clear();
        model->d->m_declarativeSortOrders.clear();
        emit model->sortOrdersChanged();
    }
}

