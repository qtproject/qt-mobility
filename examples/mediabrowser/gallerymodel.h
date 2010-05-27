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

    QString columnName(int column) const;
    void setColumnName(int column, const QString &name);

    QString displayFieldForColumn(int column) const;
    void setDisplayFieldForColumn(int column, const QString &field);

    QString decorationFieldForColumn(int column) const;
    void setDecorationFieldForColumn(int column, const QString &field);

    QVector<QString> userRoleFields() const;
    void setUserRoleFields(const QVector<QString> &fields);

public slots:
    void setList(QGalleryItemList *list);

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
