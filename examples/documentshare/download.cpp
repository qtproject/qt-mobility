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

#include "download.h"

#include "sharewidget.h"

#include <qdocumentgallery.h>
#include <qgalleryurlrequest.h>

#include <QtGui>
#include <QtNetwork>

Download::Download(QNetworkReply *networkReply, QDocumentGallery *gallery, QObject *parent)
    : QObject(parent)
    , networkReply(networkReply)
    , urlRequest(0)
{
    connect(networkReply, SIGNAL(metaDataChanged()), this, SLOT(networkMetaDataChanged()));
    connect(networkReply, SIGNAL(readyRead()), this, SLOT(networkReadyRead()));
    connect(networkReply, SIGNAL(finished()), this, SLOT(networkFinished()));

    urlRequest = new QGalleryUrlRequest(gallery, this);
    connect(urlRequest, SIGNAL(finished(int)), this, SLOT(urlRequestFinished(int)));
}

Download::~Download()
{
    delete networkReply;
}

QVariant Download::itemId() const
{
    return urlRequest->itemId();
}

void Download::networkMetaDataChanged()
{
    if (!file.isOpen()) {
        QString fileName;

        QByteArray disposition = networkReply->rawHeader("Content-Disposition");

        if (!disposition.isEmpty()) {
            int fileNameIndex = disposition.indexOf("filename=") + 9;

            if (fileNameIndex >= 9) {
                int semiColonIndex = disposition.indexOf(';', fileNameIndex);

                fileName = semiColonIndex >= 0
                        ? disposition.mid(fileNameIndex, semiColonIndex - fileNameIndex)
                        : disposition.mid(fileNameIndex);

                if (fileName.startsWith(QLatin1Char('"'))) {
                    fileName = fileName.endsWith(QLatin1Char('"'))
                        ? fileName.mid(1, fileName.length() - 2)
                        : fileName.mid(1);
                } else if (fileName.endsWith(QLatin1Char('"'))) {
                    fileName.chop(1);
                }
            }
        }

        if (fileName.isEmpty()) {
            fileName = networkReply->url().path().section(QLatin1Char('/'), -1);

            if (fileName.isEmpty())
                fileName = QLatin1String("unnamed_download");
        }

        fileName = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)
                + QLatin1Char('/')
                + fileName;

        fileName = QFileDialog::getSaveFileName(
                qobject_cast<QWidget *>(parent()), tr("Save File"), fileName);

        if (!fileName.isEmpty()) {
            file.setFileName(fileName);
            if (file.open(QIODevice::WriteOnly))
                networkReadyRead();
            else
                networkReply->abort();
        }
    }
}

void Download::networkReadyRead()
{
    if (file.isOpen()) {
        char buffer[4096];

        qint64 bytesRead = networkReply->read(buffer, 4096);

        while (bytesRead > 0) {
            file.write(buffer, bytesRead);

            bytesRead = networkReply->read(buffer, 4096);
        }
    }
}

void Download::networkFinished()
{
    if (networkReply->error() != QNetworkReply::NoError) {
        if (file.isOpen()) {
            file.close();
            file.remove();
        }

        emit failed(this);
    } else {
        networkMetaDataChanged();
        networkReadyRead();

        file.close();

        urlRequest->setItemUrl(QUrl::fromLocalFile(file.fileName()));
        urlRequest->execute();
    }
}

void Download::urlRequestFinished(int result)
{
    if (result == QGalleryAbstractRequest::Succeeded)
        emit succeeded(this);
    else
        emit failed(this);
}

