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

#include "artistdelegate.h"

#include <QtGui>

ArtistDelegate::ArtistDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
}

ArtistDelegate::~ArtistDelegate()
{
}

void ArtistDelegate::paint(
        QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QPen oldPen = painter->pen();

    int margin = QApplication::style()->pixelMetric(QStyle::PM_ButtonMargin);

    QRect rect = option.rect;

    if (option.state & QStyle::State_HasFocus) {
        painter->fillRect(rect, option.palette.highlight());

        painter->setPen(option.palette.color(QPalette::HighlightedText));
    }

    rect.adjust(margin, margin, -margin, -margin);

    QString text = index.data(Qt::DisplayRole).toString();

    QRect textRect;
    painter->drawText(rect, Qt::TextWordWrap, text, &textRect);
    rect.setTop(textRect.bottom() + margin);

    int trackCount =  index.data(TrackCount).toInt();

    text = trackCount != 1
            ? tr("%1 Songs").arg(trackCount)
            : tr("1 Song");

    painter->drawText(rect, Qt::TextWordWrap, text, &textRect);

    painter->setPen(oldPen);
}

QSize ArtistDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const
{
    QString placeholder = QLatin1String("This is a longish name\n\n\nW");

    int margin = QApplication::style()->pixelMetric(QStyle::PM_ButtonMargin);

    QSize size = option.fontMetrics.size(0, placeholder);

    size.rheight() += 2 * margin;
    size.rwidth() += 2 * margin;

    return size;
}

