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

#include "imageviewer.h"
#include "ui_imageviewer.h"

#include <QtGui>

ImageViewer::ImageViewer(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::ImageViewer)
{
    ui->setupUi(this);
    playlist = new QMediaPlaylist(0, this);
    model = new QMediaPlaylistModel(playlist,this);
    navigator = new QMediaPlaylistNavigator(playlist,this);

    connect(navigator, SIGNAL(activated(QMediaResourceList)), SLOT(display(QMediaResourceList)));
    connect(navigator, SIGNAL(playbackModeChanged(QMediaPlaylistNavigator::PlaybackMode)), SLOT(updatePlaybackMode(QMediaPlaylistNavigator::PlaybackMode)));
    connect(ui->listView, SIGNAL(activated(QModelIndex)), SLOT(jump(QModelIndex)));
    connect(navigator, SIGNAL(surroundingItemsChanged()), SLOT(updateNavigationActions()));

    navigator->setPlaybackMode(QMediaPlaylistNavigator::Linear);

    ui->listView->setModel(model);
    updateNavigationActions();
}

ImageViewer::~ImageViewer()
{
    delete ui;
}

void ImageViewer::addImage()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Add Image", "/usr/share/doc/packages/libqt4/src/images", "*.png");
    if (1 || !fileNames.isEmpty()) {
        int firstPos = playlist->size();
        foreach(const QString &fileName, fileNames) {
            playlist->append(QMediaResource(QLatin1String("file://") + fileName, "image/png"));
        }

        navigator->jump(firstPos);        
    }
}

void ImageViewer::advance()
{
    navigator->advance();
}

void ImageViewer::back()
{
    navigator->back();
}

void ImageViewer::shuffle()
{
    playlist->shuffle();
}

void ImageViewer::display(const QMediaResourceList &resources)
{
    if (resources.isEmpty()) {
        ui->listView->clearSelection();
        ui->label->clear();
    } else {
        ui->listView->setCurrentIndex(model->index(navigator->currentPosition()));
        ui->label->setPixmap(resources.first().uri().path());
    }
}

void ImageViewer::jump(const QModelIndex &index)
{
    navigator->jump(index.row());
}

void ImageViewer::setPlaybackMode(int mode)
{
    navigator->setPlaybackMode(QMediaPlaylistNavigator::PlaybackMode(mode));
}

void ImageViewer::updatePlaybackMode(QMediaPlaylistNavigator::PlaybackMode mode)
{
    ui->playbackMode->setCurrentIndex(int(mode));    
}

void ImageViewer::updateNavigationActions()
{
    ui->buttonBack->setEnabled(!navigator->previousItem().isNull());
    ui->buttonAdvance->setEnabled(!navigator->nextItem().isNull());
}

void ImageViewer::loadPlaylist()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Load playlist", ".", "*.m3u");
    if (!fileName.isEmpty()) {
        if (!playlist->load(fileName, "m3u")) {
            QMessageBox::warning(this, "Playlist loading failed", "Playlist loading failed");
        }
    }
}

void ImageViewer::savePlaylist()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save playlist", ".", "*.m3u");
    if (!fileName.isEmpty()) {
        if (!playlist->save(fileName, "m3u")) {
            QMessageBox::warning(this, "Playlist saving failed", "Playlist saving failed");
        }
    }
}
