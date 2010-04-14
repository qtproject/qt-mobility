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

#include "qdocumentgallery.h"
#include "qgalleryitemlist.h"

GalleryModel::GalleryModel(QObject *parent)
    : QAbstractItemModel(parent)
    , mediaList(0)
    , mode(LocationList)
    , titleKey(-1)
{
}

GalleryModel::~GalleryModel()
{
}

QVariant GalleryModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        int row = index.row();

        if (mode != LocationList) {
            if (row == 0) {
                if (role == Qt::DisplayRole)
                    return tr("Up...");
                else if (role == Type)
                    return QLatin1String("Up");
                else
                    return QVariant();
            } else {
                row -= 1;
            }
        }

        if (role == Qt::DisplayRole && titleKey >= 0)
            return mediaList->metaData(row, titleKey);
        else if (role == ItemId)
            return mediaList->id(row);
        else if (role == Type)
            return mediaList->type(row);
    }
    return QVariant();
}

QModelIndex GalleryModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid() && column == 0) {
        int count = mediaList ? mediaList->count() : 0;

        if (mode != LocationList)
            count += 1;

        if (row >= 0 && row < count)
            return createIndex(row, column);
    }
    return QModelIndex();
}

QModelIndex GalleryModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int GalleryModel::rowCount(const QModelIndex &parent) const
{
    int count = 0;
    if (!parent.isValid()) {
        if (mediaList)
            count = mediaList->count();

        if (mode != LocationList)
            count += 1;
    }
    return count;
}

int GalleryModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? 1 : 0;
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
    }

    mediaList = list;

    if (mediaList) {
        titleKey = mediaList->propertyKey(QDocumentGallery::fileName);

        connect(mediaList, SIGNAL(inserted(int,int)), this, SLOT(inserted(int,int)));
        connect(mediaList, SIGNAL(removed(int,int)), this, SLOT(removed(int,int)));
        connect(mediaList, SIGNAL(moved(int,int,int)), this, SLOT(moved(int,int,int)));
        connect(mediaList, SIGNAL(metaDataChanged(int,int,QList<int>)),
                   this, SLOT(metaDataChanged(int,int,QList<int>)));
    }
    endResetModel();
}

GalleryModel::ListMode GalleryModel::listMode() const
{
    return mode;
}

void GalleryModel::setListMode(ListMode listMode)
{
    if (mode != listMode) {
        if (mode == LocationList) {
            beginRemoveRows(QModelIndex(), 0, 0);
            mode = listMode;
            endRemoveRows();
        } else if (listMode == LocationList) {
            beginInsertRows(QModelIndex(), 0, 0);
            mode = listMode;
            endInsertRows();
        } else {
            mode = listMode;
        }
    }
}

void GalleryModel::removed(int index, int count)
{
    if (mode != LocationList)
        index += 1;

    beginRemoveRows(QModelIndex(), index, index + count - 1);
    endRemoveRows();
}

void GalleryModel::inserted(int index, int count)
{
    if (mode != LocationList)
        index += 1;

    beginInsertRows(QModelIndex(), index, index + count - 1);
    endInsertRows();
}

void GalleryModel::moved(int from, int to, int count)
{
    if (mode != LocationList) {
        from += 1;
        to += 1;
    }

    beginMoveRows(QModelIndex(), from, from + count - 1, QModelIndex(), to);
}

void GalleryModel::metaDataChanged(int index, int count, const QList<int> &)
{
    if (mode != LocationList)
        index += 1;

    emit dataChanged(createIndex(index, 0), createIndex(index + count -1, 0));
}

