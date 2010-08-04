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

#include "downloadmodel.h"

#include "download.h"

DownloadModel::DownloadModel(QDocumentGallery *gallery, QObject *parent)
    : QAbstractListModel(parent)
    , gallery(gallery)
{
}

DownloadModel::~DownloadModel()
{
    qDeleteAll(downloads);
}

int DownloadModel::rowCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? downloads.count() : 0;
}

QModelIndex DownloadModel::index(int row, int column, const QModelIndex &parent) const
{
    return !parent.isValid() && row >= 0 && row < downloads.count() && column == 0
            ? createIndex(row, 0, downloads.at(row))
            : QModelIndex();
}

QVariant DownloadModel::data(const QModelIndex &index, int role) const
{
    Download *download = reinterpret_cast<Download *>(index.internalPointer());

    switch (role) {
    case Qt::DisplayRole:
        return download->displayName();
    case ItemIdRole:
        return download->itemId();
    case ItemTypeRole:
        return download->itemType();
    case StateRole:
        return int(download->state());
    case CurrentProgressRole:
        return download->currentProgress();
    case MaximumProgressRole:
        return download->maximumProgress();
    default:
        return QVariant();
    }
}

void DownloadModel::addDownload(QNetworkReply *networkReply)
{
    Download *download = new Download(networkReply, gallery, this);
    connect(download, SIGNAL(progressChanged(Download*)), this, SLOT(downloadChanged(Download*)));
    connect(download, SIGNAL(stateChanged(Download*)), this, SLOT(downloadChanged(Download*)));

    beginInsertRows(QModelIndex(), downloads.count(), downloads.count());
    downloads.append(download);
    endInsertRows();
}

void DownloadModel::downloadChanged(Download *download)
{
    QModelIndex index = createIndex(downloads.indexOf(download), 0, download);

    emit dataChanged(index, index);
}
