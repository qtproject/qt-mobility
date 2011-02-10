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
    setLayout(layout);

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

    resultSet = queryRequest->resultSet();
    if (resultSet) {
        addTextPropertyRow("File Name", QDocumentGallery::fileName);
        addTextPropertyRow("Type", QDocumentGallery::mimeType);
        addTextPropertyRow("Path", QDocumentGallery::path);
        addTextPropertyRow("Size", QDocumentGallery::fileSize);

        addTimePropertyRow("Modified", QDocumentGallery::lastModified);
        addTimePropertyRow("Accessed", QDocumentGallery::lastAccessed);

        if (queryRequest->state() == QGalleryAbstractRequest::Active)
            connect(queryRequest, SIGNAL(finished()), this, SLOT(queryRequestFinished()));
        else if (queryRequest->state() == QGalleryAbstractRequest::Finished)
            queryRequestFinished();
    }
}

void DocumentPropertiesWidget::addTextPropertyRow(const char *rowTitle,
    const QString &propertyName)
{
    QLineEdit *propertyValue = new QLineEdit;
    propertyValue->setReadOnly(true);
    addRowWithTitleAndValue(rowTitle, propertyValue);
    new MetaDataBinding(propertyValue, "text", resultSet, propertyName, this);
}

void DocumentPropertiesWidget::addRowWithTitleAndValue(
    const char *rowTitle, QWidget *propertyValue)
{
    QLineEdit *title = new QLineEdit(tr(rowTitle));
    title->setReadOnly(true);

    QFormLayout *layout = static_cast<QFormLayout *> (this->layout());
    layout->addRow(title, propertyValue);
}

void DocumentPropertiesWidget::addTimePropertyRow(
    const char *rowTitle, const QString &propertyName)
{
    QDateTimeEdit *propertyValue = new QDateTimeEdit;
    propertyValue->setReadOnly(true);
    addRowWithTitleAndValue(rowTitle, propertyValue);
    new MetaDataBinding(propertyValue, "dateTime", resultSet, propertyName, this);
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
    makeItemRequestForProperties(QStringList()
            << QDocumentGallery::title
            << QDocumentGallery::artist
            << QDocumentGallery::albumTitle
            << QDocumentGallery::albumArtist
            << QDocumentGallery::genre
            << QDocumentGallery::duration);

    resultSet = itemRequest->resultSet();
    if (resultSet) {
        addTextPropertyRow("Title", QDocumentGallery::title);
        addTextPropertyRow("Artist", QDocumentGallery::artist);
        addTextPropertyRow("Album", QDocumentGallery::albumTitle);
        addTextPropertyRow("Album Artist", QDocumentGallery::albumArtist);
        addTextPropertyRow("Genre", QDocumentGallery::genre);
        addTextPropertyRow("Duration", QDocumentGallery::duration);
    }
}

void DocumentPropertiesWidget::makeItemRequestForProperties(const QStringList &propertyNames)
{
    itemRequest = new QGalleryItemRequest(documentGallery, this);
    itemRequest->setItemId(queryRequest->itemId());
    itemRequest->setPropertyNames(propertyNames);
    itemRequest->execute();
}

void DocumentPropertiesWidget::requestDocumentProperties()
{
    makeItemRequestForProperties(QStringList()
                << QDocumentGallery::title
                << QDocumentGallery::author
                << QDocumentGallery::pageCount);

    resultSet = itemRequest->resultSet();
    if (resultSet) {
        addTextPropertyRow("Title", QDocumentGallery::title);
        addTextPropertyRow("Author", QDocumentGallery::author);
        addTextPropertyRow("Page Count", QDocumentGallery::pageCount);
    }
}

void DocumentPropertiesWidget::requestImageProperties()
{
    makeItemRequestForProperties(QStringList()
            << QDocumentGallery::title
            << QDocumentGallery::width
            << QDocumentGallery::height
            << QDocumentGallery::keywords);

    resultSet = itemRequest->resultSet();
    if (resultSet) {
        addTextPropertyRow("Title", QDocumentGallery::title);
        addTextPropertyRow("Width", QDocumentGallery::width);
        addTextPropertyRow("Height", QDocumentGallery::height);
        addTextPropertyRow("Keywords", QDocumentGallery::keywords);
    }
}

void DocumentPropertiesWidget::requestVideoProperties()
{
    makeItemRequestForProperties(QStringList()
            << QDocumentGallery::title
            << QDocumentGallery::width
            << QDocumentGallery::height
            << QDocumentGallery::duration);

    resultSet = itemRequest->resultSet();
    if (resultSet) {
        addTextPropertyRow("Title", QDocumentGallery::title);
        addTextPropertyRow("Width", QDocumentGallery::width);
        addTextPropertyRow("Height", QDocumentGallery::height);
        addTextPropertyRow("Duration", QDocumentGallery::duration);
    }
}
