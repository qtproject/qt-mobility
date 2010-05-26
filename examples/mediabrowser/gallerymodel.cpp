/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "gallerymodel.h"

#include "qgalleryitemlist.h"

GalleryModel::GalleryModel(QObject *parent)
    : QAbstractItemModel(parent)
    , mediaList(0)
    , columnNames(1)
    , displayKeys(1, -1)
    , displayFields(1)
    , decorationKeys(1, -1)
    , decorationFields(1)
{
}

GalleryModel::~GalleryModel()
{
}

QVariant GalleryModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        int key = -1;

        if (role == Qt::DisplayRole || role == Qt::EditRole)
            key = displayKeys.at(index.column());
        else if (role == Qt::DecorationRole)
            key = decorationKeys.at(index.column());
        else if (role >= Qt::UserRole)
            key = userKeys.at(role - Qt::UserRole);

        if (key >= 0)
            return mediaList->metaData(index.row(), key);
    }
    return QVariant();
}

bool GalleryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int key = displayKeys.at(index.column());

        if (key >= 0) {
            mediaList->setMetaData(index.row(), key, value);

            return true;
        }
    }
    return false;
}

Qt::ItemFlags GalleryModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = Qt::ItemIsEnabled;

    if (index.isValid()) {
        int key = displayKeys.at(index.column());

        if (key >= 0 && mediaList->propertyAttributes(key) & QGalleryProperty::CanWrite)
            flags |= Qt::ItemIsEditable;
    }

    return flags;
}

QModelIndex GalleryModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid() && mediaList
            && row >= 0 && row < mediaList->count()
            && column >= 0 && column < displayFields.count()) {

        // Ideally we'd use the scroll position of the view to set the cursor position
        if (row < mediaList->count() - 1) {
            const int position = mediaList->cursorPosition();
            const int pageSize = mediaList->minimumPagedItems();

            if (row - 16 < position && position > 0)
                mediaList->setCursorPosition(qMax(0, row - 16));
            else if (row + 16 > position + pageSize)
                mediaList->setCursorPosition(qMax(0, row + 16 - pageSize));
        }

        return createIndex(row, column);
    }
    return QModelIndex();
}

QModelIndex GalleryModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

QVariant GalleryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return orientation == Qt::Horizontal && role == Qt::DisplayRole
            ? columnNames.value(section)
            : QVariant();
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

        columnNames.resize(count);
        displayFields.resize(count);
        displayKeys.resize(count);
        decorationFields.resize(count);
        decorationKeys.resize(count);

        endRemoveColumns();
    } else if (displayFields.count() < count) {
        int index = displayFields.count();

        beginInsertColumns(QModelIndex(), index, count - 1);

        columnNames.resize(count);
        displayFields.resize(count);
        displayKeys.fill(-1, count);
        decorationFields.resize(count);
        decorationKeys.fill(-1, count);

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
        decorationKeys.fill(-1);
        userKeys.fill(-1);
    }

    mediaList = list;

    if (mediaList) {
        for (int i = 0; i < displayFields.count(); ++i)
            displayKeys[i] = mediaList->propertyKey(displayFields.at(i));

        for (int i = 0; i < decorationFields.count(); ++i)
            decorationKeys[i] = mediaList->propertyKey(decorationFields.at(i));

        for (int i = 0; i < userFields.count(); ++i)
            userKeys[i] = mediaList->propertyKey(userFields.at(i));

        connect(mediaList, SIGNAL(inserted(int,int)), this, SLOT(inserted(int,int)));
        connect(mediaList, SIGNAL(removed(int,int)), this, SLOT(removed(int,int)));
        connect(mediaList, SIGNAL(moved(int,int,int)), this, SLOT(moved(int,int,int)));
        connect(mediaList, SIGNAL(metaDataChanged(int,int,QList<int>)),
                   this, SLOT(metaDataChanged(int,int,QList<int>)));
    }
    endResetModel();
}

QString GalleryModel::columnName(int column) const
{
    return columnNames.at(column);
}

void GalleryModel::setColumnName(int column, const QString &name)
{
    columnNames[column] = name;

    emit headerDataChanged(Qt::Horizontal, column, column);
}

QString GalleryModel::displayFieldForColumn(int column) const
{
    return displayFields.at(column);
}

void GalleryModel::setDisplayFieldForColumn(int column, const QString &field)
{
    displayFields[column] = field;

    if (mediaList) {
        displayKeys[column] = mediaList->propertyKey(field);

        emit dataChanged(createIndex(0, column), createIndex(mediaList->count() - 1, column));
    }
}

QString GalleryModel::decorationFieldForColumn(int column) const
{
    return decorationFields.at(column);
}

void GalleryModel::setDecorationFieldForColumn(int column, const QString &field)
{
    decorationFields[column] = field;

    if (mediaList) {
        decorationKeys[column] = mediaList->propertyKey(field);

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
        for (int i = 0; i < userFields.count(); ++i)
            userKeys[i] = mediaList->propertyKey(userFields.at(i));

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

