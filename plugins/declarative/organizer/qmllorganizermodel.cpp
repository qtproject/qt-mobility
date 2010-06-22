/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
#include <qorganizeritemrequests.h>
#include "qmlorganizermodel.h"
#include "qmlorganizeritem.h"
#include <QDebug>
#include <QSharedPointer>

QMLOrganizerModel::QMLOrganizerModel(QObject *parent)
    :QAbstractListModel(parent),
    m_manager(0),
    m_start(QDateTime::currentDateTime()),
    m_end(QDateTime::currentDateTime())
{

}

QMLOrganizerModel::QMLOrganizerModel(QOrganizerItemManager* manager, const QDateTime& start, const QDateTime& end, QObject *parent) :
    QAbstractListModel(parent),
    m_start(start),
    m_end(end)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(OrganizerItemIdRole, "itemId");
    roleNames.insert(OrganizerItemRole, "item");
    setRoleNames(roleNames);


    connect(&m_request, SIGNAL(resultsAvailable()), this, SLOT(itemsReceived()));

    m_filter.setStartPeriod(start);
    m_filter.setEndPeriod(end);
    m_request.setFilter(m_filter);

    setManager(manager);
}

QString QMLOrganizerModel::manager()
{
    if (m_manager)
        return m_manager->managerName();
    return QString();
}

QDateTime QMLOrganizerModel::startPeriod() const
{
    return m_start;
}

QDateTime QMLOrganizerModel::endPeriod() const
{
    return m_end;
}

void QMLOrganizerModel::setStartPeriod(const QDateTime& start)
{
    m_start = start;
    m_filter.setStartPeriod(m_start);
    m_request.setFilter(m_filter);
}

void QMLOrganizerModel::setEndPeriod(const QDateTime& end)
{
    m_end = end;
    m_filter.setEndPeriod(m_end);
    m_request.setFilter(m_filter);
}


int QMLOrganizerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_items.count();
}

void QMLOrganizerModel::setManager(QOrganizerItemManager* manager)
{
    disconnect(this, SLOT(reloadData()));
    m_manager = manager;
    connect(m_manager, SIGNAL(dataChanged()), this, SLOT(reloadData()));
    m_request.setManager(m_manager);
    reloadData();
}

void QMLOrganizerModel::reloadData()
{
    m_request.start();
}

void QMLOrganizerModel::itemsReceived()
{
    reset();
    foreach (QMLOrganizerItem* qitem, m_items) {
       delete qitem;
    }
    m_items.clear();

    QList<QOrganizerItem> items = m_request.items();
    beginInsertRows(QModelIndex(), 0, items.count());
    foreach (const QOrganizerItem& item, items) {
        QMLOrganizerItem* qitem = new QMLOrganizerItem(this);
        qitem->setItem(item);
        m_items.append(qitem);
    }
    endInsertRows();
}



QVariant QMLOrganizerModel::data(const QModelIndex &index, int role) const
{
    QMLOrganizerItem* item  = m_items.at(index.row());
    switch(role) {
        case OrganizerItemRole:
            return QVariant::fromValue(item);
    }
    return QVariant();
}

