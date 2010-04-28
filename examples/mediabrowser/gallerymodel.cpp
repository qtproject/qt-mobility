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

