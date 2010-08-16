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

#ifndef DOWNLOADCOMPLETEDIALOG_H
#define DOWNLOADCOMPLETEDIALOG_H

#include <qmobilityglobal.h>

#include <QtCore/QVariant>
#include <QtGui/QDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QTextEdit;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE
class QDocumentGallery;
class QGalleryItemList;
class QGalleryItemRequest;
QTM_END_NAMESPACE

class Download;

QTM_USE_NAMESPACE;

class ItemInformationDialog : public QDialog
{
    Q_OBJECT
public:
    ItemInformationDialog(
            QDocumentGallery *gallery,
            QWidget *parent = 0,
            Qt::WindowFlags flags = 0);

    void addProperty(const QString &propertyName, const QString &displayName);

    void showItem(const QVariant &itemId);

private slots:
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void metaDataChanged(int index, int count, const QList<int> &keys);

private:
    QGalleryItemRequest *request;
    QGalleryItemList *item;
    QStringList propertyNames;
    QStringList displayNames;
    QList<int> propertyKeys;
    QList<QVariant::Type> propertyTypes;
    QList<QLabel *> propertyWidgets;
};

#endif
