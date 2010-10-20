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

#include "documentpropertieswidget.h"

#include <qdocumentgallery.h>
#include <qgalleryresultset.h>
#include <qgalleryqueryrequest.h>

#include <QtGui>
#include <qaction.h>

DocumentPropertiesWidget::DocumentPropertiesWidget(
        const QFileInfo &file, QDocumentGallery *gallery, QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , request(0)
    , resultSet(0)
{
    setLayout(new QFormLayout);
    // draw softkeys on symbian to be able to close dialog
    QAction* doneAction = new QAction(tr("Done"), this);
    doneAction->setSoftKeyRole(QAction::PositiveSoftKey);
    connect(doneAction, SIGNAL(triggered()), parent, SLOT(close()));
    addAction(doneAction);
    
    request = new QGalleryQueryRequest(gallery, this);
    request->setFilter(QDocumentGallery::filePath == file.absoluteFilePath());

    QStringList propertyNames = QStringList()
            << QDocumentGallery::fileName
            << QDocumentGallery::mimeType
            << QDocumentGallery::path
            << QDocumentGallery::fileSize
            << QDocumentGallery::lastModified
            << QDocumentGallery::lastAccessed;

    QStringList labels = QStringList()
             << tr("File Name")
             << tr("Type")
             << tr("Path")
             << tr("Size")
             << tr("Modified")
             << tr("Accessed");

    requestProperties(QDocumentGallery::File, propertyNames, labels);
}

void DocumentPropertiesWidget::itemsInserted(int index, int count)
{
    resultSet->fetch(0);

    metaDataChanged(index, count, QList<int>());

    if (index == 0 && request->rootType() == QDocumentGallery::File) {
        QString itemType = resultSet->itemType();

        if (itemType == QDocumentGallery::Audio)
            QTimer::singleShot(0, this, SLOT(requestAudioProperties()));
        else if (itemType == QDocumentGallery::Document)
            QTimer::singleShot(0, this, SLOT(requestDocumentProperties()));
        else if (itemType == QDocumentGallery::Image)
            QTimer::singleShot(0, this, SLOT(requestImageProperties()));
        else if (itemType == QDocumentGallery::Video)
            QTimer::singleShot(0, this, SLOT(requestVideoProperties()));
    }
}

void DocumentPropertiesWidget::requestProperties(
        const QString &itemType, const QStringList &propertyNames, const QStringList &labels)
{
    QStringList currentPropertyNames = request->propertyNames();

    request->setRootType(itemType);
    request->setPropertyNames(propertyNames + currentPropertyNames);
    request->execute();

    resultSet = request->resultSet();

    if (resultSet) {
        connect(resultSet, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        connect(resultSet, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        connect(resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(metaDataChanged(int,int,QList<int>)));

        for (int i = 0; i < currentPropertyNames.count(); ++i)
            propertyKeys[i] = resultSet->propertyKey(currentPropertyNames.at(i));

        for (int i = 0; i < propertyNames.count(); ++i)
            insertRow(i, propertyNames.at(i), labels.at(i));

        if (resultSet->itemCount() > 0)
            itemsInserted(0, resultSet->itemCount());
    }
}

void DocumentPropertiesWidget::requestAudioProperties()
{
    QStringList propertyNames = QStringList()
            << QDocumentGallery::title
            << QDocumentGallery::artist
            << QDocumentGallery::albumTitle
            << QDocumentGallery::albumArtist
            << QDocumentGallery::genre
            << QDocumentGallery::duration;

    QStringList labels = QStringList()
            << tr("Title")
            << tr("Artist")
            << tr("Album")
            << tr("Album Artist")
            << tr("Genre")
            << tr("Duration");

    requestProperties(QDocumentGallery::Audio, propertyNames, labels);
}

void DocumentPropertiesWidget::requestDocumentProperties()
{
    QStringList propertyNames = QStringList()
            << QDocumentGallery::title
            << QDocumentGallery::author
            << QDocumentGallery::pageCount;

    QStringList labels = QStringList()
            << tr("Title")
            << tr("Author")
            << tr("Page Count");

    requestProperties(QDocumentGallery::Document, propertyNames, labels);
}

void DocumentPropertiesWidget::requestImageProperties()
{
    QStringList propertyNames = QStringList()
            << QDocumentGallery::title
            << QDocumentGallery::width
            << QDocumentGallery::height
            << QDocumentGallery::keywords;

    QStringList labels = QStringList()
            << tr("Title")
            << tr("Width")
            << tr("Height")
            << tr("Keywords");

    requestProperties(QDocumentGallery::Image, propertyNames, labels);
}

void DocumentPropertiesWidget::requestVideoProperties()
{
    QStringList propertyNames = QStringList()
            << QDocumentGallery::title
            << QDocumentGallery::width
            << QDocumentGallery::height
            << QDocumentGallery::duration;

    QStringList labels = QStringList()
            << tr("Title")
            << tr("Width")
            << tr("Height")
            << tr("Duration");

    requestProperties(QDocumentGallery::Video, propertyNames, labels);
}

void DocumentPropertiesWidget::itemsRemoved(int index, int count)
{
    metaDataChanged(index, count, QList<int>());
}

void DocumentPropertiesWidget::metaDataChanged(int index, int, const QList<int> &keys)
{
    if (index == 0) {
        if (keys.isEmpty()) {
            for (int i = 0; i < propertyKeys.count(); ++i)
                updateValue(i, propertyKeys.at(i));
        } else {
            foreach (int propertyKey, keys) {
                int i = propertyKeys.indexOf(propertyKey);
                if (i >= 0)
                    updateValue(i, propertyKey);
            }
        }
    }
}

void DocumentPropertiesWidget::insertRow(
        int index, const QString &propertyName, const QString &label)
{
    int propertyKey = resultSet->propertyKey(propertyName);

    QVariant::Type propertyType = resultSet->propertyType(propertyKey);
    QGalleryProperty::Attributes propertyAttributes = resultSet->propertyAttributes(propertyKey);

    QWidget *widget = 0;

    if (propertyAttributes & QGalleryProperty::CanWrite) {
        switch (propertyType) {
        case QVariant::String:
            widget = new QLineEdit;
            break;
        case QVariant::Double:
            widget = new QDoubleSpinBox;
            break;
        case QVariant::Int:
            widget = new QSpinBox;
            break;
        case QVariant::DateTime:
            widget = new QDateTimeEdit;
        default:
            widget = new QLabel;
        }
    } else if (propertyAttributes & QGalleryProperty::CanRead) {
        widget = new QLabel;
    }

    propertyKeys.insert(index, propertyKey);
    widgets.insert(index, widget);

    static_cast<QFormLayout *>(layout())->insertRow(index, label, widget);
}

void DocumentPropertiesWidget::updateValue(int widgetIndex, int propertyKey)
{
    QGalleryProperty::Attributes propertyAttributes = resultSet->propertyAttributes(propertyKey);

    QWidget *widget = widgets.at(widgetIndex);

    QVariant value = resultSet->metaData(propertyKey);

    if (propertyAttributes & QGalleryProperty::CanWrite) {
        switch (value.type()) {
        case QVariant::String:
            if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(widget))
                lineEdit->setText(value.toString());
            break;
        case QVariant::Double:
            if (QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>(widget))
                spinBox->setValue(value.toDouble());
            break;
        case QVariant::Int:
            if (QSpinBox *spinBox = qobject_cast<QSpinBox *>(widget))
                spinBox->setValue(value.toInt());
            break;
        case QVariant::Date:
            if (QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit *>(widget))
                dateTimeEdit->setDate(value.toDate());
            break;
        case QVariant::Time:
            if (QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit *>(widget))
                dateTimeEdit->setTime(value.toTime());
            break;
        case QVariant::DateTime:
            if (QDateTimeEdit *dateTimeEdit = qobject_cast<QDateTimeEdit *>(widget))
                dateTimeEdit->setDateTime(value.toDateTime());
            break;
        case QVariant::Image:
        case QVariant::Pixmap:
            if (QLabel *label = qobject_cast<QLabel *>(widget))
                label->setPixmap(value.value<QPixmap>());
            break;
        case QVariant::StringList:
            if (QLabel *label = qobject_cast<QLabel *>(widget))
                label->setText(value.toStringList().join(QLatin1String("; ")));
        default:
            if (QLabel *label = qobject_cast<QLabel *>(widget))
                label->setText(value.toString());
        }
    } else if (propertyAttributes & QGalleryProperty::CanRead) {
        if (QLabel *label = qobject_cast<QLabel *>(widget)) {
            switch (value.type()) {
            case QVariant::StringList:
                label->setText(value.toStringList().join(QLatin1String("; ")));
            default:
                label->setText(value.toString());
            }
        }
    }
}
