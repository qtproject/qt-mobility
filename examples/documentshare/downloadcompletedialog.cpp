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

    connect(request, SIGNAL(itemChanged()), this, SLOT(itemChanged()));

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

void DownloadCompleteDialog::show(const QString &itemId)
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

