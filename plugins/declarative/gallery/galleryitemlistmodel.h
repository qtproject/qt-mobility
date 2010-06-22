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

#ifndef GALLERYITEMLIST_H
#define GALLERYITEMLIST_H

#include <qmobilityglobal.h>

#include <QtCore/qabstractitemmodel.h>

QTM_BEGIN_NAMESPACE

class QGalleryItemList;

class GalleryItemListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        ItemId = 0,
        ItemType,
        ItemUrl,
        Reading,
        Writing,
        Available,
        MetaDataOffset
    };

    GalleryItemListModel(QObject *parent = 0);
    ~GalleryItemListModel();

    int rowCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QModelIndex index(int row, int column, const QModelIndex &parent) const;

    bool autoUpdateCursorPosition() const { return m_updateCursor; }
    void setAutoUpdateCursorPosition(bool enabled) { m_updateCursor = enabled; }

public Q_SLOTS:
    void setItemList(QGalleryItemList *list);

Q_SIGNALS:
    void cursorPositionChanged();

private Q_SLOTS:
    void _q_itemsInserted(int index, int count);
    void _q_itemsRemoved(int index, int count);
    void _q_itemsMoved(int from, int to, int count);
    void _q_itemsChanged(int index, int count);

protected:
    QGalleryItemList *m_itemList;
    int m_rowCount;
    int m_lowerOffset;
    int m_upperOffset;
    bool m_updateCursor;
};

QTM_END_NAMESPACE

#endif
