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

#include "sharewidget.h"

#include "download.h"
#include "downloadcompletedialog.h"

#include <qdocumentgallery.h>

#include <QtGui>
#include <QtWebKit>

ShareWidget::ShareWidget(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
    , documentGallery(0)
    , webView(0)
    , downloadCompleteDialog(0)
{
    webView = new QWebView;
    webView->setUrl(QUrl(QLatin1String("http://share.ovi.com")));
    webView->page()->setForwardUnsupportedContent(true);

    connect(webView->page(), SIGNAL(unsupportedContent(QNetworkReply*)),
            this, SLOT(unsupportedContent(QNetworkReply*)));
    connect(webView->page(), SIGNAL(downloadRequested(QNetworkRequest)),
            this, SLOT(downloadRequested(QNetworkRequest)));

    QBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(webView);

    setLayout(layout);

    documentGallery = new QDocumentGallery(this);
}

void ShareWidget::unsupportedContent(QNetworkReply *reply)
{
    Download *download = new Download(reply, documentGallery, this);

    connect(download, SIGNAL(succeeded(Download*)), this, SLOT(downloadSucceeded(Download*)));
    connect(download, SIGNAL(failed(Download*)), this, SLOT(downloadFailed(Download*)));
}

void ShareWidget::downloadRequested(const QNetworkRequest &request)
{
    unsupportedContent(webView->page()->networkAccessManager()->get(request));
}

void ShareWidget::downloadSucceeded(Download *download)
{
    if (!downloadCompleteDialog) {
        downloadCompleteDialog = new DownloadCompleteDialog(documentGallery, this);
        downloadCompleteDialog->setModal(true);
    }

    downloadCompleteDialog->show(download->itemId());

    download->deleteLater();
}

void ShareWidget::downloadFailed(Download *download)
{
    download->deleteLater();
}
