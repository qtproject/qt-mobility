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

#include "photodelegate.h"

#include "gallerymodel.h"

#include <QtGui>

PhotoDelegate::PhotoDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
}

PhotoDelegate::~PhotoDelegate()
{
}

void PhotoDelegate::paint(
        QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    layout(option);

    QRect imageRect = thumbnailRect.translated(option.rect.topLeft());
    QPixmap thumbnail = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
    if (!thumbnail.isNull()) {
        painter->drawPixmap(imageRect, thumbnail);

        if (option.state & QStyle::State_HasFocus) {
            imageRect.adjust(0, 0, -1, -1);
            painter->drawRect(imageRect);
        }
    } else {
        if (option.state & QStyle::State_HasFocus)
            painter->fillRect(imageRect, option.palette.highlight());

        imageRect.adjust(0, 0, -1, -1);
        painter->drawRect(imageRect);
    }
}

QSize PhotoDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const
{
    layout(option);

    return size;
}


void PhotoDelegate::layout(const QStyleOptionViewItem &option) const
{
    PhotoDelegate *delegate = const_cast<PhotoDelegate *>(this);

    int height = option.decorationSize.height();

    int width = qMax(height, option.decorationSize.width());

    delegate->thumbnailRect = QRect(
            QPoint((width - option.decorationSize.width()) / 2, 0), option.decorationSize);

    delegate->size = QSize(width, height);
}
