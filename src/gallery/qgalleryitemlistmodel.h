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

#ifndef QGALLERYITEMLISTMODEL_H
#define QGALLERYITEMLISTMODEL_H

#include <qmobilityglobal.h>

#include <QtCore/qabstractitemmodel.h>

QTM_BEGIN_NAMESPACE

class QGalleryItemList;

class QGalleryItemListModelPrivate;

class Q_GALLERY_EXPORT QGalleryItemListModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(QGalleryItemList *itemList READ itemList WRITE setItemList)
    Q_PROPERTY(bool autoUpdateCursorPosition READ autoUpdateCursorPosition WRITE setAutoUpdateCursorPosition)
public:
    QGalleryItemListModel(QObject *parent = 0);
    ~QGalleryItemListModel();

    QGalleryItemList *itemList() const;

    bool autoUpdateCursorPosition() const;
    void setAutoUpdateCursorPosition(bool enabled);

    QHash<int, QString> roleProperties(int column) const;
    void setRoleProperties(int column, const QHash<int, QString> &properties);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void addColumn(const QHash<int, QString> &properties);
    void addColumn(const QString &property, int role = Qt::DisplayRole);
    void insertColumn(int index, const QHash<int, QString> &properties);
    void insertColumn(int index, const QString &property, int role = Qt::DisplayRole);
    void removeColumn(int index);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

public Q_SLOTS:
    void setItemList(QGalleryItemList *list);

private:
    QScopedPointer<QGalleryItemListModelPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QGalleryItemListModel)
    Q_PRIVATE_SLOT(d_func(), void _q_inserted(int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_removed(int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_moved(int, int, int))
    Q_PRIVATE_SLOT(d_func(), void _q_metaDataChanged(int, int, const QList<int> &))
};

QTM_END_NAMESPACE

#endif
