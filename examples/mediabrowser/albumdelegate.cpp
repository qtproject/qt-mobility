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

#include "albumdelegate.h"

#include "gallerymodel.h"

#include <QtGui>

AlbumDelegate::AlbumDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{

}

AlbumDelegate::~AlbumDelegate()
{
}

void AlbumDelegate::paint(
        QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    layout(option);

    QBrush oldBrush = painter->brush();

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
        painter->setBrush(option.palette.highlightedText());
    }

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

    QTextOption textOption;
    textOption.setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    QString title = index.data(Qt::DisplayRole).toString();
    title = option.fontMetrics.elidedText(title, Qt::ElideRight, size.width());
    painter->drawText(titleRect.translated(option.rect.topLeft()), title, textOption);

    QString artist = index.data(ArtistRole).toString();
    artist = option.fontMetrics.elidedText(artist, Qt::ElideRight, size.width());
    painter->drawText(artistRect.translated(option.rect.topLeft()), artist, textOption);
    
    painter->setBrush(oldBrush);
}

QSize AlbumDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const
{
    layout(option);

    return size;
}


void AlbumDelegate::layout(const QStyleOptionViewItem &option) const
{
    AlbumDelegate *delegate = const_cast<AlbumDelegate *>(this);

    int height = option.decorationSize.height()
            + option.fontMetrics.height()
            + option.fontMetrics.lineSpacing();

    int width = qMax(height, option.decorationSize.width());

    delegate->thumbnailRect = QRect(
            QPoint((width - option.decorationSize.width()) / 2, 0), option.decorationSize);

    QSize textSize(width, option.fontMetrics.height());

    delegate->titleRect = QRect(QPoint(0, delegate->thumbnailRect.bottom()), textSize);

    delegate->artistRect = QRect(
            QPoint(0, delegate->titleRect.top() + option.fontMetrics.lineSpacing()), textSize);

    delegate->size = QSize(width, height);
}
