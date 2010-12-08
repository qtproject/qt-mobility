/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
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

#include "documentpropertieswidget.h"

#include "metadatabinding.h"

#include <qdocumentgallery.h>
#include <qgalleryitemrequest.h>
#include <qgalleryqueryrequest.h>
#include <qgalleryresultset.h>

#include <QtGui>
#include <qaction.h>

DocumentPropertiesWidget::DocumentPropertiesWidget(
        const QFileInfo &file, QDocumentGallery *gallery, QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , documentGallery(gallery)
    , queryRequest(0)
    , itemRequest(0)
{
    // draw softkeys on symbian to be able to close dialog
    QAction* doneAction = new QAction(tr("Done"), this);
    doneAction->setSoftKeyRole(QAction::PositiveSoftKey);
    connect(doneAction, SIGNAL(triggered()), parent, SLOT(close()));
    addAction(doneAction);

    QFormLayout *layout = new QFormLayout;

    queryRequest = new QGalleryQueryRequest(documentGallery, this);
    queryRequest->setRootType(QDocumentGallery::File);
    queryRequest->setFilter(QDocumentGallery::filePath == file.absoluteFilePath());
    queryRequest->setPropertyNames(QStringList()
            << QDocumentGallery::fileName
            << QDocumentGallery::mimeType
            << QDocumentGallery::path
            << QDocumentGallery::fileSize
            << QDocumentGallery::lastModified
            << QDocumentGallery::lastAccessed);
    queryRequest->execute();

    if (QGalleryResultSet *resultSet = queryRequest->resultSet()) {
        QLabel *fileName = new QLabel;
        layout->addRow(tr("File Name"), fileName);
        new MetaDataBinding(fileName, "text", resultSet, QDocumentGallery::fileName, this);

        QLabel *mimeType = new QLabel;
        layout->addRow(tr("Type"), mimeType);
        new MetaDataBinding(mimeType, "text", resultSet, QDocumentGallery::mimeType, this);

        QLabel *path = new QLabel;
        layout->addRow(tr("Path"), path);
        new MetaDataBinding(path, "text", resultSet, QDocumentGallery::path, this);

        QLabel *size = new QLabel;
        layout->addRow(tr("Size"), size);
        new MetaDataBinding(size, "text", resultSet, QDocumentGallery::fileSize, this);

        QDateTimeEdit *lastModified = new QDateTimeEdit;
        lastModified->setReadOnly(true);
        layout->addRow(tr("Modified"), lastModified);
        new MetaDataBinding(
                lastModified, "dateTime", resultSet, QDocumentGallery::lastModified, this);

        QDateTimeEdit *lastAccessed = new QDateTimeEdit;
        lastAccessed->setReadOnly(true);
        layout->addRow(tr("Accessed"), lastAccessed);
        new MetaDataBinding(
                lastAccessed, "dateTime", resultSet, QDocumentGallery::lastAccessed, this);

        setLayout(layout);

        if (queryRequest->state() == QGalleryAbstractRequest::Active)
            connect(queryRequest, SIGNAL(finished()), this, SLOT(queryRequestFinished()));
        else if (queryRequest->state() == QGalleryAbstractRequest::Finished)
            queryRequestFinished();
    }
}

void DocumentPropertiesWidget::queryRequestFinished()
{
    if (queryRequest->seek(0)) {
        if (queryRequest->itemType() == QDocumentGallery::Audio)
            requestAudioProperties();
        else if (queryRequest->itemType() == QDocumentGallery::Video)
           requestVideoProperties();
        else if (queryRequest->itemType() == QDocumentGallery::Image)
            requestImageProperties();
        else if (queryRequest->itemType() == QDocumentGallery::Document)
            requestDocumentProperties();
    }
}

void DocumentPropertiesWidget::requestAudioProperties()
{
    itemRequest = new QGalleryItemRequest(documentGallery, this);
    itemRequest->setItemId(queryRequest->itemId());
    itemRequest->setPropertyNames(QStringList()
            << QDocumentGallery::title
            << QDocumentGallery::artist
            << QDocumentGallery::albumTitle
            << QDocumentGallery::albumArtist
            << QDocumentGallery::genre
            << QDocumentGallery::duration);
    itemRequest->execute();

    if (QGalleryResultSet *resultSet = itemRequest->resultSet()) {
        QFormLayout *layout = static_cast<QFormLayout *>(QWidget::layout());

        QLabel *title = new QLabel;
        layout->addRow(tr("Title"), title);
        new MetaDataBinding(title, "text", resultSet, QDocumentGallery::title, this);

        QLabel *artist = new QLabel;
        layout->addRow(tr("Artist"), artist);
        new MetaDataBinding(artist, "text", resultSet, QDocumentGallery::artist, this);

        QLabel *album = new QLabel;
        layout->addRow(tr("Album"), album);
        new MetaDataBinding(album, "text", resultSet, QDocumentGallery::albumTitle, this);

        QLabel *albumArtist = new QLabel;
        layout->addRow(tr("Album Artist"), albumArtist);
        new MetaDataBinding(albumArtist, "text", resultSet, QDocumentGallery::albumArtist, this);

        QLabel *genre = new QLabel;
        layout->addRow(tr("Genre"), genre);
        new MetaDataBinding(genre, "text", resultSet, QDocumentGallery::genre, this);

        QLabel *duration = new QLabel;
        layout->addRow(tr("Duration"), duration);
        new MetaDataBinding(duration, "text", resultSet, QDocumentGallery::duration, this);
    }
}

void DocumentPropertiesWidget::requestDocumentProperties()
{
    itemRequest = new QGalleryItemRequest(documentGallery, this);
    itemRequest->setItemId(queryRequest->itemId());
    itemRequest->setPropertyNames(QStringList()
                << QDocumentGallery::title
                << QDocumentGallery::author
                << QDocumentGallery::pageCount);
    itemRequest->execute();

    if (QGalleryResultSet *resultSet = itemRequest->resultSet()) {
        QFormLayout *layout = static_cast<QFormLayout *>(QWidget::layout());

        QLabel *title = new QLabel;
        layout->addRow(tr("Title"), title);
        new MetaDataBinding(title, "text", resultSet, QDocumentGallery::title, this);

        QLabel *author = new QLabel;
        layout->addRow(tr("Author"), author);
        new MetaDataBinding(author, "text", resultSet, QDocumentGallery::author, this);

        QLabel *pageCount = new QLabel;
        layout->addRow(tr("Page Count"), pageCount);
        new MetaDataBinding(pageCount, "text", resultSet, QDocumentGallery::pageCount, this);
    }
}

void DocumentPropertiesWidget::requestImageProperties()
{
    itemRequest = new QGalleryItemRequest(documentGallery, this);
    itemRequest->setItemId(queryRequest->itemId());
    itemRequest->setPropertyNames(QStringList()
            << QDocumentGallery::title
            << QDocumentGallery::width
            << QDocumentGallery::height
            << QDocumentGallery::keywords);
    itemRequest->execute();

    if (QGalleryResultSet *resultSet = itemRequest->resultSet()) {
        QFormLayout *layout = static_cast<QFormLayout *>(QWidget::layout());

        QLabel *title = new QLabel;
        layout->addRow(tr("Title"), title);
        new MetaDataBinding(title, "text", resultSet, QDocumentGallery::title, this);

        QLabel *width = new QLabel;
        layout->addRow(tr("Width"), width);
        new MetaDataBinding(width, "text", resultSet, QDocumentGallery::width, this);

        QLabel *height = new QLabel;
        layout->addRow(tr("Height"), height);
        new MetaDataBinding(height, "text", resultSet, QDocumentGallery::height, this);

        QLabel *keywords = new QLabel;
        layout->addRow(tr("Keywords"), keywords);
        new MetaDataBinding(keywords, "text", resultSet, QDocumentGallery::keywords, this);
    }
}

void DocumentPropertiesWidget::requestVideoProperties()
{
    itemRequest = new QGalleryItemRequest(documentGallery, this);
    itemRequest->setItemId(queryRequest->itemId());
    itemRequest->setPropertyNames(QStringList()
            << QDocumentGallery::title
            << QDocumentGallery::width
            << QDocumentGallery::height
            << QDocumentGallery::duration);
    itemRequest->execute();

    if (QGalleryResultSet *resultSet = itemRequest->resultSet()) {
        QFormLayout *layout = static_cast<QFormLayout *>(QWidget::layout());

        QLabel *title = new QLabel;
        layout->addRow(tr("Title"), title);
        new MetaDataBinding(title, "text", resultSet, QDocumentGallery::title, this);

        QLabel *width = new QLabel;
        layout->addRow(tr("Width"), width);
        new MetaDataBinding(width, "text", resultSet, QDocumentGallery::width, this);

        QLabel *height = new QLabel;
        layout->addRow(tr("Height"), height);
        new MetaDataBinding(height, "text", resultSet, QDocumentGallery::height, this);

        QLabel *duration = new QLabel;
        layout->addRow(tr("Duration"), duration);
        new MetaDataBinding(duration, "text", resultSet, QDocumentGallery::duration, this);
    }
}
