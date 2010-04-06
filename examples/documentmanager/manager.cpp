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

#include "manager.h"

#include "gallerymodel.h"

#include <qgallery.h>

#include <QtGui>

Manager::Manager(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
    , gallery(0)
    , sourceItemRequest(0)
    , destinationItemRequest(0)
    , sourceModel(0)
    , destinationModel(0)
    , sourceView(0)
    , destinationView(0)
    , sourceProgress(0)
    , destinationProgress(0)
    , actionProgress(0)
    , splitter(0)
    , documentsUrl(QUrl::fromLocalFile(
            QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)))
{
    gallery = new QDocumentGallery(this);

    sourceItemRequest = new QGalleryItemRequest(this);
    sourceItemRequest->setGallery(gallery);
    sourceItemRequest->setFields(QStringList() << QDocumentGallery::fileName);
    sourceItemRequest->setItemType(QDocumentGallery::File);
    sourceItemRequest->setContainerUrl(documentsUrl);
    connect(sourceItemRequest, SIGNAL(itemsChanged()), this, SLOT(sourceListChanged()));
    connect(sourceItemRequest, SIGNAL(finished(int)), this, SLOT(sourceRequestFinished(int)));

    sourceModel = new GalleryModel(this);

    QLabel *sourceLabel = new QLabel(tr("Source"));

    sourceView = new QListView;
    sourceView->setModel(sourceModel);
    connect(sourceView, SIGNAL(activated(QModelIndex)),
            this, SLOT(sourceItemActivated(QModelIndex)));

    sourceProgress = new QProgressBar;
    sourceProgress->setOrientation(Qt::Horizontal);
    sourceProgress->setTextVisible(false);

    QBoxLayout *sourceLayout = new QVBoxLayout;
    sourceLayout->setMargin(0);
    sourceLayout->addWidget(sourceLabel);
    sourceLayout->addWidget(sourceView);
    sourceLayout->addWidget(sourceProgress);

    QWidget *sourceWidget = new QWidget;
    sourceWidget->setLayout(sourceLayout);

    destinationItemRequest = new QGalleryItemRequest(this);
    destinationItemRequest->setGallery(gallery);
    destinationItemRequest->setFields(QStringList() << QDocumentGallery::fileName);
    destinationItemRequest->setItemType(QDocumentGallery::Folder);
    destinationItemRequest->setContainerUrl(documentsUrl);
    connect(destinationItemRequest, SIGNAL(itemsChanged()), this, SLOT(destinationListChanged()));
    connect(destinationItemRequest, SIGNAL(finished(int)),
            this, SLOT(destinationRequestFinished(int)));

    destinationModel = new GalleryModel(this);

    QLabel *destinationLabel = new QLabel(tr("Destination"));

    destinationView = new QListView;
    destinationView->setModel(destinationModel);
    connect(destinationView, SIGNAL(activated(QModelIndex)),
            this, SLOT(destinationItemActivated(QModelIndex)));

    destinationProgress = new QProgressBar;
    destinationProgress->setOrientation(Qt::Horizontal);
    destinationProgress->setTextVisible(false);

    QBoxLayout *destinationLayout = new QVBoxLayout;
    destinationLayout->setMargin(0);
    destinationLayout->addWidget(destinationLabel);
    destinationLayout->addWidget(destinationView);
    destinationLayout->addWidget(destinationProgress);

    QWidget *destinationWidget = new QWidget;
    destinationWidget->setLayout(destinationLayout);

    splitter = new QSplitter;
    splitter->setOrientation(Qt::Horizontal);
    splitter->addWidget(sourceWidget);
    splitter->addWidget(destinationWidget);

    QPushButton *copyButton = new QPushButton(tr("Copy"));
    connect(copyButton, SIGNAL(clicked()), this, SLOT(copy()));
    connect(this, SIGNAL(enableButtons(bool)), copyButton, SLOT(setEnabled(bool)));

    QPushButton *moveButton = new QPushButton(tr("Move"));
    connect(moveButton, SIGNAL(clicked()), this, SLOT(move()));
    connect(this, SIGNAL(enableButtons(bool)), moveButton, SLOT(setEnabled(bool)));

    QPushButton *removeButton = new QPushButton(tr("Delete"));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));
    connect(this, SIGNAL(enableButtons(bool)), removeButton, SLOT(setEnabled(bool)));

    QBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setMargin(0);
    buttonLayout->addStretch();
    buttonLayout->addWidget(copyButton);
    buttonLayout->addWidget(moveButton);
    buttonLayout->addWidget(removeButton);
    buttonLayout->addStretch();

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(splitter);
    layout->addLayout(buttonLayout);

    setLayout(layout);

    actionProgress = new QProgressDialog(this);
    actionProgress->setModal(true);

    sourceItemRequest->execute();
    destinationItemRequest->execute();
}

Manager::~Manager()
{
}

void Manager::copy()
{
    QStringList itemIds = selectedSourceIds();

    if (!itemIds.isEmpty()) {
        QGalleryCopyRequest *copyRequest = new QGalleryCopyRequest(gallery, this);
        copyRequest->setItemIds(itemIds);
        copyRequest->setDestinationId(destinationFolderId.last());

        executeActionRequest(copyRequest);
    }
}

void Manager::move()
{
    QStringList itemIds = selectedSourceIds();

    if (!itemIds.isEmpty()) {
        QGalleryMoveRequest *moveRequest = new QGalleryMoveRequest(gallery, this);
        moveRequest->setItemIds(itemIds);
        moveRequest->setDestinationId(destinationFolderId.last());

        executeActionRequest(moveRequest);
    }
}

void Manager::remove()
{
    QStringList itemIds = selectedSourceIds();

    if (!itemIds.isEmpty()) {
        QGalleryRemoveRequest *removeRequest = new QGalleryRemoveRequest(gallery, this);
        removeRequest->setItemIds(itemIds);

        executeActionRequest(removeRequest);
    }
}

QStringList Manager::selectedSourceIds() const
{
    QStringList itemIds;

    foreach (QModelIndex index, sourceView->selectionModel()->selectedIndexes()) {
        QString itemId = index.data(GalleryModel::ItemId).toString();

        if (!itemId.isNull())
            itemIds.append(itemId);
    }

    return itemIds;
}

void Manager::executeActionRequest(QGalleryAbstractRequest *request)
{
    emit enableButtons(false);

    actionRequest = request;

    connect(actionProgress, SIGNAL(canceled()), actionRequest, SLOT(cancel()));

    connect(actionRequest, SIGNAL(progressChanged(int,int)),
            this, SLOT(actionProgressChanged(int,int)));
    connect(actionRequest, SIGNAL(finished(int)), this, SLOT(actionRequestFinished(int)));

    actionProgress->show();

    actionRequest->execute();
}

void Manager::sourceListChanged()
{
    sourceModel->setList(sourceItemRequest->items());
}

void Manager::destinationListChanged()
{
    destinationModel->setList(destinationItemRequest->items());
}

void Manager::sourceProgressChanged(int current, int maximum)
{
    sourceProgress->setRange(0, maximum);
    sourceProgress->setValue(current);
}

void Manager::destinationProgressChanged(int current, int maximum)
{
    destinationProgress->setRange(0, maximum);
    destinationProgress->setValue(current);
}

void Manager::actionProgressChanged(int current, int maximum)
{
    actionProgress->setRange(0, maximum);
    actionProgress->setValue(current);
}


void Manager::sourceRequestFinished(int)
{

}

void Manager::destinationRequestFinished(int)
{

}

void Manager::actionRequestFinished(int)
{
    actionProgress->accept();

    actionRequest->deleteLater();
    actionRequest = 0;

    emit enableButtons(!(sourceFolderId.isEmpty() || destinationFolderId.isEmpty()));
}

void Manager::sourceItemActivated(const QModelIndex &index)
{
    QString type = index.data(GalleryModel::Type).toString();

    if (type == QDocumentGallery::Folder) {
        QString itemId = index.data(GalleryModel::ItemId).toString();

        sourceFolderId.append(itemId);

        sourceItemRequest->setContainerId(itemId);
        sourceItemRequest->execute();

        sourceModel->setListMode(GalleryModel::FileList);
    } else if (type == QLatin1String("Up")) {
        sourceFolderId.removeLast();

        if (sourceFolderId.isEmpty()) {
            sourceItemRequest->setContainerUrl(documentsUrl);
            sourceItemRequest->execute();

            sourceModel->setListMode(GalleryModel::LocationList);

        } else {
            sourceItemRequest->setContainerId(sourceFolderId.last());
            sourceItemRequest->execute();
        }
    }
}

void Manager::destinationItemActivated(const QModelIndex &index)
{
    QString type = index.data(GalleryModel::Type).toString();

    if (type == QDocumentGallery::Folder) {
        QString itemId = index.data(GalleryModel::ItemId).toString();

        destinationFolderId.append(itemId);

        destinationItemRequest->setContainerId(itemId);
        destinationItemRequest->execute();

        destinationModel->setListMode(GalleryModel::FolderList);
    } else if (type == QLatin1String("Up")) {
        destinationFolderId.removeLast();

        if (destinationFolderId.isEmpty()) {
            destinationItemRequest->setContainerUrl(documentsUrl);
            destinationItemRequest->execute();

            destinationModel->setListMode(GalleryModel::LocationList);
        } else {
            destinationItemRequest->setContainerId(destinationFolderId.last());
            destinationItemRequest->execute();
        }
    }
}
