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

#include "downloaddelegate.h"

#include "download.h"
#include "downloadmodel.h"

#include <QtGui>

DownloadDelegate::DownloadDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
}

DownloadDelegate::~DownloadDelegate()
{
}

void DownloadDelegate::paint(
        QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyle *style = QApplication::style();
    int state = index.data(DownloadModel::StateRole).toInt();

    int hMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, &option);
    int vMargin = style->pixelMetric(QStyle::PM_FocusFrameVMargin, &option);

    QRect rect = option.rect;

    if (option.state & QStyle::State_Selected)
        painter->fillRect(rect, option.palette.highlight());

    rect.adjust(hMargin, vMargin, -hMargin, -vMargin);

    QString text = stateText(state, index.data(Qt::DisplayRole).toString());

    QStyleOptionProgressBarV2 progressOption;
    progressOption.minimum = 0;
    progressOption.maximum = index.data(DownloadModel::MaximumProgressRole).toInt();
    progressOption.progress = index.data(DownloadModel::CurrentProgressRole).toInt();
    progressOption.orientation = Qt::Horizontal;
    progressOption.rect = rect;
    progressOption.palette = option.palette;
    progressOption.palette.setColor(
            QPalette::Highlight, option.palette.color(QPalette::AlternateBase));

    style->drawControl(QStyle::CE_ProgressBar, &progressOption, painter);

    QRect textRect = style->subElementRect(QStyle::SE_ProgressBarContents, &progressOption);

    style->drawItemText(painter, textRect, 0, option.palette, true, text);
}

QSize DownloadDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyle *style = QApplication::style();
    int state = index.data(DownloadModel::StateRole).toInt();

    int hMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, &option);
    int vMargin = style->pixelMetric(QStyle::PM_FocusFrameVMargin, &option);

    QString text = stateText(state, index.data(Qt::DisplayRole).toString());

    QRect textRect = style->itemTextRect(
            option.fontMetrics, option.rect, option.displayAlignment, true, text);
    textRect.adjust(-hMargin, -vMargin, hMargin, vMargin);

    QStyleOptionProgressBarV2 progressOption;
    progressOption.minimum = 0;
    progressOption.maximum = index.data(DownloadModel::MaximumProgressRole).toInt();
    progressOption.progress = index.data(DownloadModel::CurrentProgressRole).toInt();
    progressOption.orientation = Qt::Horizontal;
    progressOption.rect = option.rect;

    QSize size = style->sizeFromContents(
            QStyle::CT_ProgressBar, &progressOption, textRect.size());

    size.rwidth() += 2 * hMargin;
    size.rheight() += 2 * vMargin;

    return size;
}

QString DownloadDelegate::stateText(int state, const QString &displayText) const
{
    switch (state) {
    case Download::Downloading:
        return tr("Downloading:\n  %1").arg(displayText);
    case Download::Finalizing:
        return tr("Registering:\n  %1").arg(displayText);
    case Download::Finished:
        return tr("Download Finished:\n  %1").arg(displayText);
    case Download::DownloadError:
        return tr("Download Failed:\n  %1").arg(displayText);
    case Download::ItemError:
        return tr("Registration Failed:\n  %1").arg(displayText);
    default:
        return QString();
    }
}
