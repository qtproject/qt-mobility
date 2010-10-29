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


static QHash<uint, QOrganizerItemId> qt_organizerItemIdHash;
static QHash<uint, QOrganizerCollectionId> qt_organizerCollectionIdHash;


class QDeclarativeOrganizerModelPrivate
{
public:
    QDeclarativeOrganizerModelPrivate()
        :m_manager(0),
        m_fetchHint(0),
        m_filter(0),
        m_startPeriod(QDateTime::currentDateTime()),
        m_endPeriod(QDateTime::currentDateTime())
    {
    }
    ~QDeclarativeOrganizerModelPrivate()
    {
        if (m_manager)
            delete m_manager;
    }

    QList<QDeclarativeOrganizerItem*> m_items;
    QOrganizerManager* m_manager;
    QDeclarativeOrganizerItemFetchHint* m_fetchHint;
    QList<QDeclarativeOrganizerItemSortOrder*> m_sortOrders;
    QDeclarativeOrganizerItemFilter* m_filter;

    QVersitReader m_reader;
    QVersitWriter m_writer;
    QDateTime m_startPeriod;
    QDateTime m_endPeriod;
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

    connect(this, SIGNAL(managerChanged()), SLOT(fetchAgain()));
    connect(this, SIGNAL(filterChanged()), SLOT(fetchAgain()));
    connect(this, SIGNAL(fetchHintChanged()), SLOT(fetchAgain()));
    connect(this, SIGNAL(sortOrdersChanged()), SLOT(fetchAgain()));
    connect(this, SIGNAL(startPeriodChanged()), SLOT(fetchAgain()));
    connect(this, SIGNAL(endPeriodChanged()), SLOT(fetchAgain()));

    //import vcard
    connect(&d->m_reader, SIGNAL(stateChanged(QVersitReader::State)), this, SLOT(startImport(QVersitReader::State)));
}

/*!
  \qmlproperty string OrganizerModel::manager

  This property holds the manager uri of the organizer backend engine.
  */
QString QDeclarativeOrganizerModel::manager() const
{
    return d->m_manager->managerName();
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
    //TODO
    Q_UNUSED(autoUpdate);
}

bool QDeclarativeOrganizerModel::autoUpdate() const
{
    //TODO
    return true;
}

/*!
  \qmlmethod OrganizerModel::update()

  Manually update the organizer model content.

  \sa OrganizerModel::autoUpdate
  */
void QDeclarativeOrganizerModel::update()
{
    //TODO
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
    d->m_startPeriod = start;
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
    d->m_endPeriod = end;
}

/*!
  \qmlmethod OrganizerModel::importItems(url url, list<string> profiles)

  Import organizer items from a vcalendar by the given \a url and optional \a profiles.
  */
void QDeclarativeOrganizerModel::importItems(const QString& fileName)
{
   qWarning() << "importing items from:" << fileName;
   QFile*  file = new QFile(fileName);
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
void QDeclarativeOrganizerModel::exportItems(const QString& fileName)
{
   QVersitOrganizerExporter exporter;
   QList<QOrganizerItem> items;
   foreach (QDeclarativeOrganizerItem* di, d->m_items) {
       items.append(di->item());
   }

   exporter.exportItems(items);
   QVersitDocument document = exporter.document();
   QFile* file = new QFile(fileName);
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
    if (d->m_manager)
        delete d->m_manager;


    d->m_manager = new QOrganizerManager(managerName);

    connect(d->m_manager, SIGNAL(dataChanged()), this, SLOT(fetchAgain()));
    emit managerChanged();
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
        emit fetchHintChanged();
    }
}

/*!
  \qmlproperty string OrganizerModel::error

  This property holds the latest error code returned by the organizer manager.

  This property is read only.
  */
QString QDeclarativeOrganizerModel::error() const
{
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
    return QLatin1String("NoError");
}

/*!
  \qmlproperty QDeclarativeListProperty OrganizerModel::sortOrders

  This property holds a list of sort orders used by the organizer model.

  \sa SortOrder
  */
QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder> QDeclarativeOrganizerModel::sortOrders()
{
    return QDeclarativeListProperty<QDeclarativeOrganizerItemSortOrder>(this, d->m_sortOrders);
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
                if (d->m_manager->saveItems(&items))
                    qWarning() << "items imported.";
                    fetchAgain();
            }
        }
    }
}

void QDeclarativeOrganizerModel::fetchAgain()
{
    d->m_items.clear();
    qt_organizerItemIdHash.clear();

    reset();

    QList<QOrganizerItemSortOrder> sortOrders;
    foreach (QDeclarativeOrganizerItemSortOrder* so, d->m_sortOrders) {
        sortOrders.append(so->sortOrder());
    }
    QOrganizerItemFetchRequest* req = new QOrganizerItemFetchRequest(this);
    req->setManager(d->m_manager);
    req->setSorting(sortOrders);

    req->setFilter(d->m_filter? d->m_filter->filter() : QOrganizerItemFilter());
    req->setFetchHint(d->m_fetchHint ? d->m_fetchHint->fetchHint() : QOrganizerItemFetchHint());

    connect(req,SIGNAL(stateChanged(QOrganizerAbstractRequest::State)), this, SLOT(itemFetched()));

    req->start();
    emit itemsChanged();
}

void QDeclarativeOrganizerModel::itemFetched()
{
    QOrganizerItemFetchRequest* req = qobject_cast<QOrganizerItemFetchRequest*>(QObject::sender());
    if (req && req->isFinished()) {
        QList<QOrganizerItem> items = req->items();

        QList<QDeclarativeOrganizerItem*> dis;
        foreach(QOrganizerItem item, items) {
            dis.append(new QDeclarativeOrganizerItem(item, d->m_manager->detailDefinitions(item.type()), this));
            if (!item.id().isNull())
              qt_organizerItemIdHash.insert(qHash(item.id()), item.id());
        }

        reset();
        beginInsertRows(QModelIndex(), 0, req->items().count());
        d->m_items = dis;
        endInsertRows();

        req->deleteLater();
    }
}


void QDeclarativeOrganizerModel::saveItem(QDeclarativeOrganizerItem* di)
{
    if (di) {
        QOrganizerItem c = di->item();
        QOrganizerItemSaveRequest* req = new QOrganizerItemSaveRequest(this);
        req->setManager(d->m_manager);
        req->setItem(c);

        connect(req,SIGNAL(stateChanged(QOrganizerAbstractRequest::State)), this, SLOT(itemSaved()));

        req->start();
    }
}

void QDeclarativeOrganizerModel::saveItem()
{
    QDeclarativeOrganizerItem* di = qobject_cast<QDeclarativeOrganizerItem*>(QObject::sender());
    saveItem(di);
}

void QDeclarativeOrganizerModel::itemSaved()
{
    QOrganizerItemSaveRequest* req = qobject_cast<QOrganizerItemSaveRequest*>(QObject::sender());
    if (req->isFinished()) {
        QList<QOrganizerItem> items = req->items();
        foreach(const QOrganizerItem& item, items) {
             if (!item.id().isNull())
               qt_organizerItemIdHash.insert(qHash(item.id()), item.id());
        }

        req->deleteLater();
    }
}


void QDeclarativeOrganizerModel::removeItem(uint id)
{
    QList<uint> ids;
    ids << id;
    removeItems(ids);
}

void QDeclarativeOrganizerModel::removeItems(const QList<uint>& ids)
{
    QOrganizerItemRemoveRequest* req = new QOrganizerItemRemoveRequest(this);
    req->setManager(d->m_manager);
    QList<QOrganizerItemId> oids;

    foreach (uint id, ids) {
        QOrganizerItemId itemId = itemIdFromHash(id);
        if (!itemId.isNull()) {
             qt_organizerItemIdHash.remove(id);
             oids.append(itemId);
        }
    }

    req->setItemIds(oids);

    connect(req,SIGNAL(stateChanged(QOrganizerAbstractRequest::State)), this, SLOT(itemRemoved()));

    req->start();
}

void QDeclarativeOrganizerModel::removeItem()
{
    QDeclarativeOrganizerItem* di = qobject_cast<QDeclarativeOrganizerItem*>(QObject::sender());
    removeItem(di->itemId());
}

void QDeclarativeOrganizerModel::itemRemoved()
{
    QOrganizerItemRemoveRequest* req = qobject_cast<QOrganizerItemRemoveRequest*>(QObject::sender());
    if (req->isFinished()) {
         if (req->error() == QOrganizerManager::NoError)
            fetchAgain();
         req->deleteLater();
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
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d->m_items);
}

/*!
  \qmlproperty QDeclarativeListProperty OrganizerModel::occurrences

  This property holds a list of event or todo occurrence items in the organizer model.

  \sa Event, Todo, EventOccurrence, TodoOccurrence
  */
QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::occurrences()
{
    //TODO:XXX
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d->m_items);
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
    QString type((const char*)(p->data));
    QDeclarativeOrganizerModel* model = qobject_cast<QDeclarativeOrganizerModel*>(p->object);
    if (model && !type.isEmpty() && item->item().type() == type) {
        item->connect(item, SIGNAL(valueChanged()), model, SIGNAL(itemsChanged()));
        model->d->m_items.append(item);
    }
}

int  QDeclarativeOrganizerModel::item_count(QDeclarativeListProperty<QDeclarativeOrganizerItem> *p)
{
    QString type((const char*)(p->data));
    QDeclarativeOrganizerModel* model = qobject_cast<QDeclarativeOrganizerModel*>(p->object);
    int count = 0;

    if (model && !type.isEmpty()) {
        foreach (const QDeclarativeOrganizerItem* item, model->d->m_items) {
            if (item->item().type() == type)
                count++;
        }
    }
    return count;
}

QDeclarativeOrganizerItem * QDeclarativeOrganizerModel::item_at(QDeclarativeListProperty<QDeclarativeOrganizerItem> *p, int idx)
{
    QString type((const char*)(p->data));
    QDeclarativeOrganizerModel* model = qobject_cast<QDeclarativeOrganizerModel*>(p->object);

    QDeclarativeOrganizerItem* item = 0;
    if (model && !type.isEmpty()) {
        int i = 0;
        foreach(QDeclarativeOrganizerItem* di, model->d->m_items) {
            if (di->item().type() == type) {
                if (i == idx) {
                    item = di;
                    break;
                } else {
                    i++;
                }
            }
        }
    }
    return item;
}

void  QDeclarativeOrganizerModel::item_clear(QDeclarativeListProperty<QDeclarativeOrganizerItem> *p)
{
    QString type((const char*)(p->data));
    QDeclarativeOrganizerModel* model = qobject_cast<QDeclarativeOrganizerModel*>(p->object);

    if (model && !type.isEmpty()) {
        foreach(QDeclarativeOrganizerItem* di, model->d->m_items) {
            if (di->item().type() == type) {
                model->d->m_items.removeAll(di);
                qt_organizerItemIdHash.remove(di->itemId());
            }
        }
    }
}


QOrganizerItemId QDeclarativeOrganizerModel::itemIdFromHash(uint key)
{
    return qt_organizerItemIdHash.value(key, QOrganizerItemId());
}

QOrganizerCollectionId QDeclarativeOrganizerModel::collectionIdFromHash(uint key)
{
    return qt_organizerCollectionIdHash.value(key, QOrganizerCollectionId());
}
