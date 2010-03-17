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

#include "gallerymodel.h"

#include "qgalleryitemlist.h"

GalleryModel::GalleryModel(QObject *parent)
    : QAbstractItemModel(parent)
    , mediaList(0)
    , displayKeys(1, -1)
    , displayFields(1)
{
}

GalleryModel::~GalleryModel()
{
}

QVariant GalleryModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        int key = -1;

        if (role == Qt::DisplayRole)
            key = displayKeys.at(index.column());
        else if (role >= Qt::UserRole)
            key = userKeys.at(role - Qt::UserRole);

        if (key >= 0)
            return mediaList->metaData(index.row(), key);
    }
    return QVariant();
}

QModelIndex GalleryModel::index(int row, int column, const QModelIndex &parent) const
{
    return !parent.isValid() && mediaList
            && row >= 0 && row < mediaList->count()
            && column >= 0 && column < displayFields.count()
            ? createIndex(row, column)
            : QModelIndex();
}

QModelIndex GalleryModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int GalleryModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() && mediaList
            ? mediaList->count()
            : 0;
}

int GalleryModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? displayFields.count() : 0;
}

void GalleryModel::setColumnCount(int count)
{
    if (displayFields.count() > count) {
        beginRemoveColumns(QModelIndex(), count, displayFields.count() - 1);

        displayFields.resize(count);
        displayKeys.resize(count);

        endRemoveColumns();
    } else if (displayFields.count() < count) {
        int index = displayFields.count();

        beginInsertColumns(QModelIndex(), index, count - 1);

        displayFields.resize(count);
        displayKeys.fill(-1, count);

        endInsertColumns();
    }
}

QGalleryItemList *GalleryModel::list() const
{
    return mediaList;
}

void GalleryModel::setList(QGalleryItemList *list)
{
    beginResetModel();

    if (mediaList) {
        disconnect(mediaList, SIGNAL(inserted(int,int)), this, SLOT(inserted(int,int)));
        disconnect(mediaList, SIGNAL(removed(int,int)), this, SLOT(removed(int,int)));
        disconnect(mediaList, SIGNAL(moved(int,int,int)), this, SLOT(moved(int,int,int)));
        disconnect(mediaList, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(metaDataChanged(int,int,QList<int>)));

        displayKeys.fill(-1);
        userKeys.fill(-1);
    }

    mediaList = list;

    if (mediaList) {
        foreach (int key, mediaList->keys()) {
            QString field = mediaList->toString(key);

            for (int i = displayFields.indexOf(field);
                    i >= 0;
                    i = displayFields.indexOf(field, i + 1)) {
                displayKeys[i] = key;
            }

            for (int i = userFields.indexOf(field); i >= 0; i = userFields.indexOf(field, i + 1))
                userKeys[i] = key;
        }

        connect(mediaList, SIGNAL(inserted(int,int)), this, SLOT(inserted(int,int)));
        connect(mediaList, SIGNAL(removed(int,int)), this, SLOT(removed(int,int)));
        connect(mediaList, SIGNAL(moved(int,int,int)), this, SLOT(moved(int,int,int)));
        connect(mediaList, SIGNAL(metaDataChanged(int,int,QList<int>)),
                   this, SLOT(metaDataChanged(int,int,QList<int>)));
    }
    endResetModel();
}

QString GalleryModel::displayFieldForColumn(int column) const
{
    return displayFields.at(column);
}

void GalleryModel::setDisplayFieldForColumn(int column, const QString &field)
{
    displayFields[column] = field;

    if (mediaList) {
        displayKeys[column] = -1;

        foreach (int key, mediaList->keys()) {
            if (mediaList->toString(key) == field)
                displayKeys[column] = key;
                break;
        }

        emit dataChanged(createIndex(0, column), createIndex(mediaList->count() - 1, column));
    }
}

QVector<QString> GalleryModel::userRoleFields() const
{
    return userFields;
}

void GalleryModel::setUserRoleFields(const QVector<QString> &fields)
{
    userFields = fields;
    userKeys.fill(-1, userFields.count());

    if (mediaList) {
        foreach (int key, mediaList->keys()) {
            QString field = mediaList->toString(key);

            for (int i = userFields.indexOf(field); i >= 0; i = userFields.indexOf(field, i + 1))
                userKeys[i] = key;
        }

        emit dataChanged(
                createIndex(0, 0),
                createIndex(mediaList->count() - 1, displayFields.count()));
    }
}

void GalleryModel::removed(int index, int count)
{
    beginRemoveRows(QModelIndex(), index, index + count - 1);
    endRemoveRows();
}

void GalleryModel::inserted(int index, int count)
{
    beginInsertRows(QModelIndex(), index, index + count - 1);
    endInsertRows();
}

void GalleryModel::moved(int from, int to, int count)
{
    beginMoveRows(QModelIndex(), from, from + count - 1, QModelIndex(), to);
}

void GalleryModel::metaDataChanged(int index, int count, const QList<int> &)
{
    emit dataChanged(
            createIndex(index, 0),
            createIndex(index + count -1, displayFields.count() - 1));
}

