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

#ifndef DOCUMENTLISTMODEL_H
#define DOCUMENTLISTMODEL_H

#include <qgalleryitemlist.h>

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qvector.h>

class DocumentListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    DocumentListModel(QObject *parent = 0);
    ~DocumentListModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    void setColumnCount(int count);

    QGalleryItemList *list() const { return m_list; }
    void setList(QGalleryItemList *list);

    QString columnField(int column) const;
    void setColumnField(int column, const QString &field);

private Q_SLOTS:
    void documentsRemoved(int index, int count);
    void documentsInserted(int index, int count);
    void documentsMoved(int from, int to, int count);
    void documentsChanged(int index, int count, const QList<int> &keys);

private:
    QGalleryItemList *m_list;
    QVector<int> m_keys;
    QVector<QString> m_fields;
};

#endif
