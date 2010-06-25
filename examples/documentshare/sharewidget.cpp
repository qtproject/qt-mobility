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
#include "downloaddelegate.h"
#include "downloadmodel.h"
#include "iteminformationdialog.h"

#include <qdocumentgallery.h>

#include <QtGui>
#include <QtWebKit>

ShareWidget::ShareWidget(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , documentGallery(0)
    , downloadModel(0)
    , webView(0)
    , downloadView(0)
{
    documentGallery = new QDocumentGallery(this);

    downloadModel = new DownloadModel(documentGallery, this);

    webView = new QWebView;
    webView->page()->setForwardUnsupportedContent(true);

    connect(webView, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished()));
    connect(webView, SIGNAL(urlChanged(QUrl)), this, SLOT(urlChanged(QUrl)));
    connect(webView->page(), SIGNAL(downloadRequested(QNetworkRequest)),
            this, SLOT(downloadRequested(QNetworkRequest)));
    connect(webView->page(), SIGNAL(unsupportedContent(QNetworkReply*)),
            this, SLOT(unsupportedContent(QNetworkReply*)));

    downloadView = new QListView;
    downloadView->setVisible(false);
    downloadView->setItemDelegate(new DownloadDelegate(this));
    downloadView->setModel(downloadModel);
    connect(downloadView, SIGNAL(activated(QModelIndex)),
            this, SLOT(downloadActivated(QModelIndex)));

    splitter = new QSplitter;
    splitter->addWidget(webView);
    splitter->addWidget(downloadView);

    urlCombo = new QComboBox;
    urlCombo->setEditable(true);
    urlCombo->addItem(QLatin1String("http://www.flickr.com/explore"));
    urlCombo->addItem(QLatin1String("http://picasaweb.google.com/explore"));
    urlCombo->addItem(QLatin1String("http://share.ovi.com"));
    connect(urlCombo, SIGNAL(activated(QString)), this, SLOT(loadUrl(QString)));

    QWidgetAction *urlAction = new QWidgetAction(this);
    urlAction->setDefaultWidget(urlCombo);

    QToolBar *toolBar = new QToolBar;
    toolBar->addAction(urlAction);

    reloadAction = toolBar->addAction(style()->standardIcon(QStyle::SP_BrowserReload), tr("Reload"));
    connect(reloadAction, SIGNAL(triggered()), this, SLOT(load()));

    stopAction = toolBar->addAction(style()->standardIcon(QStyle::SP_BrowserStop), tr("Stop"));
    stopAction->setEnabled(false);
    connect(stopAction, SIGNAL(triggered()), webView, SLOT(reload()));

    addToolBar(toolBar);
    setCentralWidget(splitter);

    QUrl url;
    QStringList arguments = QCoreApplication::arguments();
    if (arguments.count() > 1)
        url = QUrl(arguments.at(1));

    if (url.isValid()) {
        urlCombo->setEditText(arguments.at(1));
        webView->load(url);
    } else {
        urlCombo->setEditText(QLatin1String("http://"));
    }
}

void ShareWidget::downloadRequested(const QNetworkRequest &request)
{
    downloadModel->addDownload(webView->page()->networkAccessManager()->get(request));
    downloadView->setVisible(true);
}

void ShareWidget::unsupportedContent(QNetworkReply *reply)
{
    downloadModel->addDownload(reply);
    downloadView->setVisible(true);
}

void ShareWidget::loadStarted()
{
    stopAction->setEnabled(true);
}

void ShareWidget::loadFinished()
{
    stopAction->setEnabled(false);
}

void ShareWidget::load()
{
    if (urlCombo->currentText() != webView->url().toString())
        webView->load(urlCombo->currentText());
    else
        webView->reload();
}

void ShareWidget::loadUrl(const QString &url)
{
    webView->load(QUrl(url));
}

void ShareWidget::urlChanged(const QUrl &url)
{
    urlCombo->setEditText(url.toString());
}

void ShareWidget::downloadActivated(const QModelIndex &index)
{
    QVariant itemId = index.data(DownloadModel::ItemIdRole);
    QString itemType = index.data(DownloadModel::ItemTypeRole).toString();

    if (itemId.isValid()) {
        ItemInformationDialog *dialog = new ItemInformationDialog(documentGallery, this);
        dialog->setModal(true);
        dialog->setAttribute(Qt::WA_DeleteOnClose);

        dialog->addProperty(QDocumentGallery::fileName, tr("File Name"));
        dialog->addProperty(QDocumentGallery::fileSize, tr("File Size"));
        dialog->addProperty(QDocumentGallery::mimeType, tr("MIME Type"));

        if (itemType == QDocumentGallery::Image) {
            dialog->addProperty(QDocumentGallery::thumbnailPixmap, tr("Preview"));
            dialog->addProperty(QDocumentGallery::keywords, tr("Keywords"));
        } else if (itemType == QDocumentGallery::Audio) {
            dialog->addProperty(QDocumentGallery::albumTitle, tr("Album"));
            dialog->addProperty(QDocumentGallery::artist, tr("Artist"));
            dialog->addProperty(QDocumentGallery::genre, tr("Genre"));
            dialog->addProperty(QDocumentGallery::duration, tr("Duration"));
        }
        dialog->showItem(itemId);
    }
}

