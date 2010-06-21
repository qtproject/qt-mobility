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

GalleryItemListModel::GalleryItemListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_itemList(0)
    , m_lowerOffset(0)
    , m_upperOffset(0)
    , m_updateCursor(true)
{
}

GalleryItemListModel::~GalleryItemListModel()
{
}

int GalleryItemListModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? m_rowCount : 0;
}

QVariant GalleryItemListModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
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
    } else {
        return QVariant();
    }
}

bool GalleryItemListModel::setData(
        const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && m_itemList->propertyAttributes(role) & QGalleryProperty::CanWrite) {
        m_itemList->setMetaData(index.row(), role, value);

        return true;
    } else {
        return false;
    }

}


QModelIndex GalleryItemListModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid() && row >= 0 && row < m_rowCount && column == 0) {
        if (m_updateCursor) {
            if (row < m_itemList->count() - 1) {
                const int position = m_itemList->cursorPosition();

                if (row - m_lowerOffset < position && position > 0) {
                    m_itemList->setCursorPosition(qMax(0, row - m_lowerOffset));

                    emit const_cast<GalleryItemListModel *>(this)->cursorPositionChanged();
                } else if (row + m_upperOffset > position) {
                    m_itemList->setCursorPosition(qMax(0, row + m_upperOffset));

                    emit const_cast<GalleryItemListModel *>(this)->cursorPositionChanged();
                }
            }
        }

        return createIndex(row, column);
    } else {
        return QModelIndex();
    }
}

void GalleryItemListModel::setItemList(QGalleryItemList *list)
{
    m_itemList = list;

    if (m_itemList) {
        QHash<int, QByteArray> roleNames;

        QStringList propertyNames = m_itemList->propertyNames();

        typedef QStringList::const_iterator iterator;
        for (iterator it = propertyNames.constBegin(), end = propertyNames.constEnd(); it != end; ++it)
            roleNames.insert(m_itemList->propertyKey(*it), it->toLatin1());

        roleNames.insert(-1, QByteArray("itemId"));
        roleNames.insert(-2, QByteArray("itemType"));
        roleNames.insert(-3, QByteArray("url"));

        setRoleNames(roleNames);

        connect(m_itemList, SIGNAL(inserted(int,int)), this, SLOT(_q_itemsInserted(int,int)));
        connect(m_itemList, SIGNAL(removed(int,int)), this, SLOT(_q_itemsRemoved(int,int)));
        connect(m_itemList, SIGNAL(moved(int,int,int)), this, SLOT(_q_itemsMoved(int,int,int)));
        connect(m_itemList, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(_q_metaDataChanged(int,int,QList<int>)));

        m_lowerOffset = m_itemList->minimumPagedItems() / 4;
        m_upperOffset = m_lowerOffset - m_itemList->minimumPagedItems();

        m_rowCount = m_itemList->count();
    } else {
        m_rowCount = 0;
    }

    reset();
}

void GalleryItemListModel::_q_itemsInserted(int index, int count)
{
    beginInsertRows(QModelIndex(), index, index + count - 1);
    m_rowCount += count;
    endInsertRows();
}

void GalleryItemListModel::_q_itemsRemoved(int index, int count)
{
    beginRemoveRows(QModelIndex(), index, index + count - 1);
    m_rowCount -= count;
    endRemoveRows();
}

void GalleryItemListModel::_q_itemsMoved(int from, int to, int count)
{
    beginMoveRows(QModelIndex(), from, from + count - 1, QModelIndex(), to);
    endMoveRows();
}

void GalleryItemListModel::_q_metaDataChanged(int index, int count, const QList<int> &)
{
    emit dataChanged(createIndex(index, 0), createIndex(index + count - 1, 0));
}

#include "moc_galleryitemlistmodel.cpp"

QTM_END_NAMESPACE
