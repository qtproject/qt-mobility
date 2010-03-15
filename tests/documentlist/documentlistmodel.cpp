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

#include "documentlistmodel.h"

DocumentListModel::DocumentListModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_list(0)
{
}

DocumentListModel::~DocumentListModel()
{
}

QVariant DocumentListModel::data(const QModelIndex &index, int role) const
{
    return index.isValid() && role == Qt::DisplayRole && m_keys.at(index.column()) >= 0
            ? m_list->metaData(index.row(), m_keys.at(index.column()))
            : QVariant();
}

QModelIndex DocumentListModel::index(int row, int column, const QModelIndex &parent) const
{
    return !parent.isValid() && m_list
            && row >= 0 && row < m_list->count()
            && column >= 0 && column < m_fields.count()
            ? createIndex(row, column)
            : QModelIndex();
}

QModelIndex DocumentListModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int DocumentListModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() && m_list
            ? m_list->count()
            : 0;
}

int DocumentListModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? m_fields.count() : 0;
}

void DocumentListModel::setColumnCount(int count)
{
    if (m_fields.count() > count) {
        beginRemoveColumns(QModelIndex(), count, m_fields.count() - 1);

        m_fields.resize(count);
        m_keys.resize(count);

        endRemoveColumns();
    } else if (m_fields.count() < count) {
        int index = m_fields.count();

        beginInsertColumns(QModelIndex(), index, count - 1);

        m_fields.resize(count);
        m_keys.resize(count);

        for (int i = index; i < count; ++i)
            m_keys[i] = -1;

        endInsertColumns();
    }
}

void DocumentListModel::setList(QGalleryDocumentList *list)
{
    beginResetModel();

    if (m_list) {
        disconnect(m_list, SIGNAL(inserted(int,int)), this, SLOT(documentsInserted(int,int)));
        disconnect(m_list, SIGNAL(removed(int,int)), this, SLOT(documentsRemoved(int,int)));
        disconnect(m_list, SIGNAL(moved(int,int,int)), this, SLOT(documentsMoved(int,int,int)));
        disconnect(m_list, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(documentsChanged(int,int,QList<int>)));
    }

    m_list = list;

    if (m_list) {
        foreach (int key, m_list->keys()) {
            QString field = m_list->toString(key);

            for (int i = m_fields.indexOf(field); i >= 0; i = m_fields.indexOf(field, i + 1))
                m_keys[i] = key;
        }

        connect(m_list, SIGNAL(inserted(int,int)), this, SLOT(documentsInserted(int,int)));
        connect(m_list, SIGNAL(removed(int,int)), this, SLOT(documentsRemoved(int,int)));
        connect(m_list, SIGNAL(moved(int,int,int)), this, SLOT(documentsMoved(int,int,int)));
        connect(m_list, SIGNAL(metaDataChanged(int,int,QList<int>)),
                   this, SLOT(documentsChanged(int,int,QList<int>)));
    }
    endResetModel();
}

QString DocumentListModel::columnField(int column) const
{
    return m_fields.at(column);
}

void DocumentListModel::setColumnField(int column, const QString &field)
{
    m_fields[column] = field;

    if (m_list) {
        m_keys[column] = -1;

        foreach (int key, m_list->keys()) {
            if (m_list->toString(key) == field)
                m_keys[column] = key;
                break;
        }

        emit dataChanged(createIndex(0, column), createIndex(m_list->count() - 1, column));
    }
}

void DocumentListModel::documentsRemoved(int index, int count)
{
    beginRemoveRows(QModelIndex(), index, index + count - 1);
    endRemoveRows();
}

void DocumentListModel::documentsInserted(int index, int count)
{
    beginInsertRows(QModelIndex(), index, index + count - 1);
    endInsertRows();
}

void DocumentListModel::documentsMoved(int from, int to, int count)
{
    beginMoveRows(QModelIndex(), from, from + count - 1, QModelIndex(), to);
}

void DocumentListModel::documentsChanged(int index, int count, const QList<int> &)
{
    emit dataChanged(createIndex(index, 0), createIndex(index + count -1, m_fields.count() - 1));
}

