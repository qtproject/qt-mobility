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

#ifndef MANAGER_H
#define MANAGER_H

#include <QtCore/QUrl>
#include <QtGui/QWidget>

class QAbstractButton;
class QButtonGroup;
class QDocumentGallery;
class QGalleryAbstractRequest;
class QGalleryContainerRequest;
class QGalleryUrlRequest;
class QListView;
class QModelIndex;
class QProgressBar;
class QProgressDialog;
class QSplitter;

class GalleryModel;

class Manager : public QWidget
{
    Q_OBJECT
public:
    Manager(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~Manager();

private slots:
    void copy();
    void move();
    void remove();

    void documentsListChanged();
    void documentsInserted(int index, int count);
    void documentsRemoved(int index, int count);


    void sourceListChanged();
    void destinationListChanged();

    void sourceProgressChanged(int current, int maximum);
    void destinationProgressChanged(int current, int maximum);
    void actionProgressChanged(int current, int maximum);

    void sourceRequestFinished(int result);
    void destinationRequestFinished(int result);
    void actionRequestFinished(int result);

    void sourceItemActivated(const QModelIndex &index);
    void destinationItemActivated(const QModelIndex &index);


signals:
    void enableButtons(bool enable);

private:
    QStringList selectedSourceIds() const;
    void executeActionRequest(QGalleryAbstractRequest *request);

    QDocumentGallery *gallery;
    QGalleryUrlRequest *documentsRequest;
    QGalleryContainerRequest *sourceItemRequest;
    QGalleryContainerRequest *destinationItemRequest;
    QGalleryAbstractRequest *actionRequest;
    GalleryModel *sourceModel;
    GalleryModel *destinationModel;
    QListView *sourceView;
    QListView *destinationView;
    QProgressBar *sourceProgress;
    QProgressBar *destinationProgress;
    QProgressDialog *actionProgress;
    QSplitter *splitter;

    QString documentsId;
    QStringList sourceFolderId;
    QStringList destinationFolderId;
};

#endif
