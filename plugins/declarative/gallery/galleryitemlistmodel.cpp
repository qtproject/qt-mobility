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

#include "galleryitemlistmodel.h"

#include <qgalleryitemlist.h>

#include <QtCore/qstringlist.h>

#include <QtCore/qdebug.h>

QTM_BEGIN_NAMESPACE

GalleryItemListModel::GalleryItemListModel(
        QGalleryItemList *list, QObject *parent)
    : QAbstractListModel(parent)
    , m_itemList(list)
{
    QHash<int, QByteArray> roleNames;

    QStringList propertyNames = m_itemList->propertyNames();

    typedef QStringList::const_iterator iterator;
    for (iterator it = propertyNames.constBegin(), end = propertyNames.constEnd(); it != end; ++it)
        roleNames.insert(m_itemList->propertyKey(*it), it->toLatin1());

    roleNames.insert(-1, QByteArray("itemId"));
    roleNames.insert(-2, QByteArray("itemType"));
    roleNames.insert(-3, QByteArray("url"));

    setRoleNames(roleNames);

    connect(list, SIGNAL(inserted(int,int)), this, SLOT(_q_itemsInserted(int,int)));
    connect(list, SIGNAL(removed(int,int)), this, SLOT(_q_itemsRemoved(int,int)));
    connect(list, SIGNAL(moved(int,int,int)), this, SLOT(_q_itemsMoved(int,int,int)));
    connect(list, SIGNAL(metaDataChanged(int,int,QList<int>)),
            this, SLOT(_q_metaDataChanged(int,int,QList<int>)));
}

GalleryItemListModel::~GalleryItemListModel()
{
}

int GalleryItemListModel::rowCount(const QModelIndex &parent) const
{
    int count = !parent.isValid() ? m_itemList->count() : 0;

    return count;
}

QVariant GalleryItemListModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case -1:
        return m_itemList->id(index.row());
    case -2:
        return m_itemList->type(index.row());
    case -3:
        return m_itemList->url(index.row());
    default:
        return m_itemList->metaData(index.row(), role);
    }
}

bool GalleryItemListModel::setData(
        const QModelIndex &index, const QVariant &value, int role)
{
    if (m_itemList->propertyAttributes(role) & QGalleryProperty::CanWrite) {
        m_itemList->setMetaData(index.row(), role, value);

        return true;
    } else {
        return false;
    }
}

void GalleryItemListModel::_q_itemsInserted(int index, int count)
{
    beginInsertRows(QModelIndex(), index, index + count - 1);
    endInsertRows();
}

void GalleryItemListModel::_q_itemsRemoved(int index, int count)
{
    beginRemoveRows(QModelIndex(), index, index + count - 1);
    endRemoveRows();
}

void GalleryItemListModel::_q_itemsMoved(int from, int to, int count)
{
    beginMoveRows(QModelIndex(), from, from + count - 1, QModelIndex(), to);
    endMoveRows();
}

void GalleryItemListModel::_q_metaDataChanged(
        int index, int count, const QList<int> &keys)
{
    emit dataChanged(createIndex(index, 0), createIndex(index + count - 1, 0));
}

#include "moc_galleryitemlistmodel.cpp"

QTM_END_NAMESPACE
