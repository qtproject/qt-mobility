/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Item: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
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

QString QDeclarativeOrganizerModel::manager() const
{
    return d->m_manager->managerName();
}

QStringList QDeclarativeOrganizerModel::availableManagers() const
{
    return QOrganizerManager::availableManagers();
}

QDateTime QDeclarativeOrganizerModel::startPeriod() const
{
    return d->m_startPeriod;
}
void QDeclarativeOrganizerModel::setStartPeriod(const QDateTime& start)
{
    d->m_startPeriod = start;
}

QDateTime QDeclarativeOrganizerModel::endPeriod() const
{
    return d->m_endPeriod;
}
void QDeclarativeOrganizerModel::setEndPeriod(const QDateTime& end)
{
    d->m_endPeriod = end;
}


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

    qWarning() << "Changed backend to: " << managerName;
    connect(d->m_manager, SIGNAL(dataChanged()), this, SLOT(fetchAgain()));
    emit managerChanged();
}

QDeclarativeOrganizerItemFilter* QDeclarativeOrganizerModel::filter() const
{
    return d->m_filter;
}

//void QDeclarativeOrganizerModel::setFilter(QDeclarativeOrganizerItemFilter* filter)
//{
//    if (filter && filter != d->m_filter) {
//        if (d->m_filter)
//            delete d->m_filter;
//        d->m_filter = filter;
//        emit filterChanged();
//    }
//}

QDeclarativeOrganizerItemFetchHint* QDeclarativeOrganizerModel::fetchHint() const
{
    return d->m_fetchHint;
}
//void QDeclarativeOrganizerModel::setFetchHint(QDeclarativeOrganizerItemFetchHint* fetchHint)
//{
//    if (fetchHint && fetchHint != d->m_fetchHint) {
//        if (d->m_fetchHint)
//            delete d->m_fetchHint;
//        d->m_fetchHint = fetchHint;
//        emit fetchHintChanged();
//    }
//}

QDeclarativeOrganizerModel::Error QDeclarativeOrganizerModel::error() const
{
    return static_cast<QDeclarativeOrganizerModel::Error>(d->m_manager->error());
}


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


QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::items()
{
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d->m_items);
}

QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::occurrences()
{
    //TODO:XXX
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d->m_items);
}

QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::events()
{
    void* d = const_cast<char*>(QOrganizerItemType::TypeEvent.latin1());
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d, item_append, item_count, item_at, item_clear);
}

QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::eventOccurrences()
{
    void* d = const_cast<char*>(QOrganizerItemType::TypeEventOccurrence.latin1());
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d, item_append, item_count, item_at, item_clear);
}

QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::todos()
{
    void* d = const_cast<char*>(QOrganizerItemType::TypeTodo.latin1());
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d, item_append, item_count, item_at, item_clear);
}

QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::todoOccurrences()
{
    void* d = const_cast<char*>(QOrganizerItemType::TypeTodoOccurrence.latin1());
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d, item_append, item_count, item_at, item_clear);
}

QDeclarativeListProperty<QDeclarativeOrganizerItem> QDeclarativeOrganizerModel::journals()
{
    void* d = const_cast<char*>(QOrganizerItemType::TypeJournal.latin1());
    return QDeclarativeListProperty<QDeclarativeOrganizerItem>(this, d, item_append, item_count, item_at, item_clear);
}

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
