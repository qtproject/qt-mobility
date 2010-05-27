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

#include "downloadcompletedialog.h"

#include <qdocumentgallery.h>
#include <qgalleryitemlist.h>
#include <qgalleryitemrequest.h>

#include <QtGui>

DownloadCompleteDialog::DownloadCompleteDialog(
        QDocumentGallery *gallery, QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , request(0)
    , item(0)
    , fileName(0)
    , mimeType(0)
    , title(0)
    , subject(0)
    , description(0)
    , fileNameKey(-1)
    , mimeTypeKey(-1)
    , titleKey(-1)
    , subjectKey(-1)
    , descriptionKey(-1)
{
    request = new QGalleryItemRequest(gallery, this);
    request->setPropertyNames(QStringList()
            << QDocumentGallery::fileName
            << QDocumentGallery::mimeType
            << QDocumentGallery::title
            << QDocumentGallery::subject
            << QDocumentGallery::description);

    connect(request, SIGNAL(itemChanged(QGalleryItemList*)), this, SLOT(itemChanged()));

    connect(this, SIGNAL(finished(int)), request, SLOT(clear()));

    fileName = new QLabel;
    mimeType = new QLabel;

    title = new QLineEdit;
    title->setReadOnly(true);
    connect(title, SIGNAL(editingFinished()), this, SLOT(titleEdited()));

    subject = new QLineEdit;
    subject->setReadOnly(true);
    connect(subject, SIGNAL(editingFinished()), this, SLOT(subjectEdited()));

    description = new QTextEdit;
    description->setReadOnly(true);

    QFormLayout *layout = new QFormLayout;
    layout->addRow(tr("File Name"), fileName);
    layout->addRow(tr("Mime Type"), mimeType);
    layout->addRow(tr("Title"), title);
    layout->addRow(tr("Subject"), subject);
    layout->addRow(tr("Description"), description);

    setLayout(layout);
    setWindowTitle(tr("Download Complete"));
}

void DownloadCompleteDialog::show(const QVariant &itemId)
{
    request->setItemId(itemId);
    request->execute();

    QDialog::show();
}

void DownloadCompleteDialog::itemChanged()
{
    if (item) {
        disconnect(item, SIGNAL(inserted(int,int)), this, SLOT(itemsInserted(int,int)));
        disconnect(item, SIGNAL(removed(int,int)), this, SLOT(itemsRemoved(int,int)));
        disconnect(item, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(metaDataChanged(int,int,QList<int>)));

        if (item->count() > 0)
            itemsRemoved(0, item->count());
    }

    item = request->item();

    if (item) {
        fileNameKey = item->propertyKey(QDocumentGallery::fileName);
        mimeTypeKey = item->propertyKey(QDocumentGallery::mimeType);
        titleKey = item->propertyKey(QDocumentGallery::title);
        subjectKey = item->propertyKey(QDocumentGallery::subject);
        descriptionKey = item->propertyKey(QDocumentGallery::description);

        connect(item, SIGNAL(inserted(int,int)), this, SLOT(itemsInserted(int,int)));
        connect(item, SIGNAL(removed(int,int)), this, SLOT(itemsRemoved(int,int)));
        connect(item, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(metaDataChanged(int,int,QList<int>)));

        if (item->count() > 0)
            itemsInserted(0, item->count());
    }
}

void DownloadCompleteDialog::itemsInserted(int, int)
{
    fileName->setText(item->metaData(0, fileNameKey).toString());
    mimeType->setText(item->metaData(0, mimeTypeKey).toString());

    title->setText(item->metaData(0, titleKey).toString());
    title->setReadOnly(false);

    subject->setText(item->metaData(0, subjectKey).toString());
    subject->setReadOnly(false);

    description->setText(item->metaData(0, descriptionKey).toString());
}

void DownloadCompleteDialog::itemsRemoved(int, int)
{
    fileName->clear();
    mimeType->clear();
    title->clear();
    title->setReadOnly(true);
    subject->clear();
    subject->setReadOnly(true);
    description->clear();
}

void DownloadCompleteDialog::metaDataChanged(int, int, const QList<int> &keys)
{
    if (keys.contains(fileNameKey) || keys.isEmpty())
        fileName->setText(item->metaData(0, fileNameKey).toString());

    if (keys.contains(mimeTypeKey) || keys.isEmpty())
        mimeType->setText(item->metaData(0, mimeTypeKey).toString());

    if (keys.contains(titleKey) || keys.isEmpty())
        title->setText(item->metaData(0, titleKey).toString());

    if (keys.contains(subjectKey) || keys.isEmpty())
        subject->setText(item->metaData(0, subjectKey).toString());

    if (keys.contains(descriptionKey) || keys.isEmpty())
        description->setText(item->metaData(0, descriptionKey).toString());
}

void DownloadCompleteDialog::titleEdited()
{
    item->setMetaData(0, titleKey, title->text());
}

void DownloadCompleteDialog::subjectEdited()
{
    item->setMetaData(0, subjectKey, subject->text());
}

