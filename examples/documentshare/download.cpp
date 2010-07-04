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

#include "download.h"

#include "sharewidget.h"

#include <qdocumentgallery.h>
#include <qgalleryurlrequest.h>

#include <QtGui>
#include <QtNetwork>

Download::Download(QNetworkReply *networkReply, QDocumentGallery *gallery, QObject *parent)
    : QObject(parent)
    , downloadState(Downloading)
    , currentDownloadProgress(0)
    , maximumDownloadProgress(0)
    , networkReply(networkReply)
    , urlRequest(0)
{
    connect(networkReply, SIGNAL(metaDataChanged()), this, SLOT(networkMetaDataChanged()));
    connect(networkReply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(networkProgress(qint64,qint64)));
    connect(networkReply, SIGNAL(readyRead()), this, SLOT(networkReadyRead()));
    connect(networkReply, SIGNAL(finished()), this, SLOT(networkFinished()));

    urlRequest = new QGalleryUrlRequest(gallery, this);
    urlRequest->setCreate(true);
    connect(urlRequest, SIGNAL(progressChanged(int,int)), this, SLOT(urlRequestProgress(int,int)));
    connect(urlRequest, SIGNAL(finished(int)), this, SLOT(urlRequestFinished(int)));
}

Download::~Download()
{
    delete networkReply;
}

Download::State Download::state() const
{
    return downloadState;
}

QVariant Download::itemId() const
{
    return urlRequest->itemId();
}

QString Download::itemType() const
{
    return urlRequest->itemType();
}

QString Download::displayName() const
{
    return fileName;
}

int Download::currentProgress() const
{
    return currentDownloadProgress;
}

int Download::maximumProgress() const
{
    return maximumDownloadProgress;
}

void Download::networkMetaDataChanged()
{
    if (!file.isOpen()) {
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

        QString filePath = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)
                + QLatin1Char('/')
                + fileName;

        filePath = QFileDialog::getSaveFileName(0, tr("Save File"), filePath);

        if (!filePath.isEmpty()) {
            file.setFileName(filePath);
            if (file.open(QIODevice::WriteOnly))
                networkReadyRead();
            else
                networkReply->abort();
        }
    }
}

void Download::networkProgress(qint64 current, qint64 maximum)
{
    currentDownloadProgress = current;
    maximumDownloadProgress = maximum;

    emit progressChanged(this);
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

        downloadState = DownloadError;
    } else {
        networkMetaDataChanged();
        networkReadyRead();

        file.close();

        downloadState = Finalizing;

        urlRequest->setItemUrl(QUrl::fromLocalFile(file.fileName()));
        urlRequest->execute();
    }

    emit stateChanged(this);
}

void Download::urlRequestProgress(int current, int maximum)
{
    currentDownloadProgress = current;
    maximumDownloadProgress = maximum;

    emit progressChanged(this);
}

void Download::urlRequestFinished(int result)
{
    if (result == QGalleryAbstractRequest::Succeeded) {
        downloadState = Finished;
    } else {
        qWarning("URL request failed %d", result);

        downloadState = ItemError;
    }

    emit stateChanged(this);
}
