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

#ifndef GALLERYMODEL_H
#define GALLERYMODEL_H

#include <qmobilityglobal.h>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QVector>

QTM_BEGIN_NAMESPACE
class QGalleryItemList;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class GalleryModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    GalleryModel(QObject *parent = 0);
    ~GalleryModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    void setColumnCount(int count);

    QGalleryItemList *list() const;
    void setList(QGalleryItemList *list);

    QString columnName(int column) const;
    void setColumnName(int column, const QString &name);

    QString displayFieldForColumn(int column) const;
    void setDisplayFieldForColumn(int column, const QString &field);

    QString decorationFieldForColumn(int column) const;
    void setDecorationFieldForColumn(int column, const QString &field);

    QVector<QString> userRoleFields() const;
    void setUserRoleFields(const QVector<QString> &fields);

private slots:
    void removed(int index, int count);
    void inserted(int index, int count);
    void moved(int from, int to, int count);
    void metaDataChanged(int index, int count, const QList<int> &keys);

private:
    QGalleryItemList *mediaList;
    QVector<QString> columnNames;
    QVector<int> displayKeys;
    QVector<QString> displayFields;
    QVector<int> decorationKeys;
    QVector<QString> decorationFields;
    QVector<int> userKeys;
    QVector<QString> userFields;
};

#endif
