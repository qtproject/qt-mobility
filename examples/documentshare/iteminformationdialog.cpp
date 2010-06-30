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

#include "iteminformationdialog.h"

#include <qdocumentgallery.h>
#include <qgalleryitemlist.h>
#include <qgalleryitemrequest.h>

#include <QtGui>

ItemInformationDialog::ItemInformationDialog(
        QDocumentGallery *gallery, QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , request(0)
    , item(0)
{
    request = new QGalleryItemRequest(gallery, this);
}


void ItemInformationDialog::addProperty(const QString &propertyName, const QString &displayName)
{
    propertyNames.append(propertyName);
    displayNames.append(displayName);
}

void ItemInformationDialog::showItem(const QVariant &itemId)
{
    request->setPropertyNames(propertyNames);
    request->setItemId(itemId);
    request->execute();

    item = request->item();

    if (item) {
        connect(item, SIGNAL(inserted(int,int)), this, SLOT(itemsInserted(int,int)));
        connect(item, SIGNAL(removed(int,int)), this, SLOT(itemsRemoved(int,int)));
        connect(item, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(metaDataChanged(int,int,QList<int>)));

        QFormLayout *layout = new QFormLayout;

        for (int i = 0; i < propertyNames.count(); ++i) {
            QString propertyName = propertyNames.at(i);

            int key = item->propertyKey(propertyName);

            if (key >= 0) {
                QVariant::Type type = item->propertyType(key);

                QLabel *label = new QLabel;

                layout->addRow(displayNames.at(i), label);

                propertyKeys.append(key);
                propertyTypes.append(type);
                propertyWidgets.append(label);
            }
        }

        setLayout(layout);

        if (item->count() > 0)
            itemsInserted(0, item->count());

        show();
    }
}


void ItemInformationDialog::itemsInserted(int, int)
{
    for (int i = 0; i < propertyKeys.count(); ++i) {
        QVariant value = item->metaData(0, propertyKeys.at(i));

        switch (propertyTypes.at(i)) {
        case QVariant::Image:
        case QVariant::Pixmap:
            propertyWidgets.at(i)->setPixmap(value.value<QPixmap>());
            break;
        default:
            propertyWidgets.at(i)->setText(value.toString());
        }
    }
}

void ItemInformationDialog::itemsRemoved(int, int)
{
    for (int i = 0; i < propertyWidgets.count(); ++i)
        propertyWidgets.at(i)->clear();
}

void ItemInformationDialog::metaDataChanged(int, int, const QList<int> &keys)
{
    if (keys.isEmpty()) {
        for (int i = 0; i < propertyKeys.count(); ++i) {
            QVariant value = item->metaData(0, propertyKeys.at(i));

            switch (propertyTypes.at(i)) {
            case QVariant::Image:
            case QVariant::Pixmap:
                propertyWidgets.at(i)->setPixmap(value.value<QPixmap>());
                break;
            default:
                propertyWidgets.at(i)->setText(value.toString());
            }
        }
    } else {
        foreach (int propertyKey, keys) {
            int i = propertyKeys.indexOf(propertyKey);

            if (i >= 0) {
                QVariant value = item->metaData(0, propertyKey);

                switch (propertyTypes.at(i)) {
                case QVariant::Image:
                case QVariant::Pixmap:
                    propertyWidgets.at(i)->setPixmap(value.value<QPixmap>());
                    break;
                default:
                    propertyWidgets.at(i)->setText(value.toString());
                }
            }
        }
    }
}
