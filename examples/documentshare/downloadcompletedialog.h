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

#ifndef DOWNLOADCOMPLETEDIALOG_H
#define DOWNLOADCOMPLETEDIALOG_H

#include <qmobilityglobal.h>

#include <QtGui/qdialog.h>

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

class DownloadCompleteDialog : public QDialog
{
    Q_OBJECT
public:
    DownloadCompleteDialog(
            QDocumentGallery *gallery, QWidget *parent = 0, Qt::WindowFlags flags = 0);

    void show(const QVariant &itemId);

private slots:
    void itemChanged();
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void metaDataChanged(int index, int count, const QList<int> &keys);

    void titleEdited();
    void subjectEdited();

private:
    QGalleryItemRequest *request;
    QGalleryItemList *item;
    QLabel *fileName;
    QLabel *mimeType;
    QLineEdit *title;
    QLineEdit *subject;
    QTextEdit *description;
    int fileNameKey;
    int mimeTypeKey;
    int titleKey;
    int subjectKey;
    int descriptionKey;
};

#endif
