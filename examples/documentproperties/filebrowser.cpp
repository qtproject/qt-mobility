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

#include "filebrowser.h"

#include "documentpropertieswidget.h"

#include <qdocumentgallery.h>

#include <QtGui>

FileBrowser::FileBrowser(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , gallery(new QDocumentGallery(this))
    , fileSystemModel(0)
    , view(0)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow);
#endif

    fileSystemModel = new QFileSystemModel(this);
    fileSystemModel->setRootPath(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));

    view = new QListView;
    view->setModel(fileSystemModel);
    connect(view, SIGNAL(activated(QModelIndex)), this, SLOT(activated(QModelIndex)));

    setCentralWidget(view);

    menuBar()->addAction(tr("Documents"), this, SLOT(browseDocuments()));
    menuBar()->addAction(tr("Audio"), this, SLOT(browseAudio()));
    menuBar()->addAction(tr("Images"), this, SLOT(browseImages()));
    menuBar()->addAction(tr("Videos"), this, SLOT(browseVideos()));

    browseDocuments();
}

void FileBrowser::browseAudio()
{
    rootPath = QDesktopServices::storageLocation(QDesktopServices::MusicLocation);

    fileSystemModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs);

    view->setRootIndex(fileSystemModel->index(rootPath));

    setWindowTitle(tr("Audio"));
}

void FileBrowser::browseDocuments()
{
    rootPath = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);

    fileSystemModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs);

    view->setRootIndex(fileSystemModel->index(rootPath));

    setWindowTitle(tr("Documents"));
}

void FileBrowser::browseImages()
{
    rootPath = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);

    fileSystemModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs);

    view->setRootIndex(fileSystemModel->index(rootPath));

    setWindowTitle(tr("Images"));
}

void FileBrowser::browseVideos()
{
    rootPath = QDesktopServices::storageLocation(QDesktopServices::MoviesLocation);

    fileSystemModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs);

    view->setRootIndex(fileSystemModel->index(rootPath));

    setWindowTitle(tr("Videos"));
}

void FileBrowser::activated(const QModelIndex &index)
{
    QFileInfo fileInfo = fileSystemModel->fileInfo(index);

    if (fileInfo.isDir() && fileInfo.fileName() != QLatin1String(".")) {
        if (fileInfo.fileName() == QLatin1String("..")) {
            QModelIndex parent = view->rootIndex().parent();

            fileInfo = fileSystemModel->fileInfo(parent);

            if (fileInfo.absoluteFilePath() == rootPath)
                fileSystemModel->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs);

            view->setRootIndex(parent);
        } else {
            fileSystemModel->setFilter(QDir::AllEntries | QDir::AllDirs);

            view->setRootIndex(index);
        }

        setWindowTitle(fileInfo.fileName());
    } else {
        if (fileInfo.fileName() == QLatin1String("."))
            fileInfo = fileSystemModel->fileInfo(view->rootIndex());


#if defined(Q_WS_MAEMO_5)
        DocumentPropertiesWidget *widget = new DocumentPropertiesWidget(fileInfo, gallery, this);
        widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

        QScrollArea *window = new QScrollArea(this);
        window->setWindowFlags(window->windowFlags() | Qt::Window);
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setAttribute(Qt::WA_Maemo5StackedWindow);
        window->setWidgetResizable(true);
        window->setWidget(widget);
        window->show();
#elif defined (Q_OS_SYMBIAN)
        QScrollArea *window = new QScrollArea(this);
        DocumentPropertiesWidget *widget = new DocumentPropertiesWidget(fileInfo, gallery, window);
        widget->setWindowModality(Qt::WindowModal);
        window->setWindowFlags(window->windowFlags() | Qt::Dialog);
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->setWidgetResizable(true);
        window->setWidget(widget);
        window->showMaximized();
#else
        DocumentPropertiesWidget *widget = new DocumentPropertiesWidget(fileInfo, gallery, this);
        widget->setWindowFlags(widget->windowFlags() | Qt::Dialog);
        widget->setAttribute(Qt::WA_DeleteOnClose);
        widget->setWindowModality(Qt::WindowModal);
        widget->show();
#endif
    }
}
