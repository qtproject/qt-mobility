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
