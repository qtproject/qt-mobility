/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef FLICKRDEMO_H
#define FLICKRDEMO_H

#include <QDialog>
#include <QMainWindow>
#include <QNetworkReply>
#include <QSize>

QT_BEGIN_NAMESPACE
class QAction;
class QDialogButtonBox;
class QLabel;
class QNetworkAccessManager;
class QNetworkRequest;
class QProgressDialog;
class QPushButton;
class QWidget;
QT_END_NAMESPACE

//// QtMobility API headers
#include <qmobilityglobal.h>

#include "xqlistwidget.h"

QTM_BEGIN_NAMESPACE
class QNetworkSession;
class QGeoPositionInfoSource;
class QGeoPositionInfo;
class QGeoSatelliteInfoSource;
class QGeoSatelliteInfo;
QTM_END_NAMESPACE

#include "connectivityhelper.h"

// Use the QtMobility namespace
QTM_USE_NAMESPACE

class FlickrDemo: public QMainWindow
{
    Q_OBJECT

public:
    FlickrDemo(QWidget* parent = 0);
    ~FlickrDemo();

private:
    void createMenus();
    void viewSatelliteInfo();
    bool parsePictureList(QString xmlString);
    void downloadFlickerPictureList();
    void downloadPictureFromFlickr();
    void downloadNextThumbnail();

private Q_SLOTS:
    void delayedInit();
    void networkSessionOpened();

    void downloadButtonClicked();
    void cancelDownload();

    /*
        void httpRequestFinished(int requestId, bool error);
        void readResponseHeader(const QHttpResponseHeader& responseHeader);
        void updateDataReadProgress(int bytesRead, int totalBytes);
    */

    void pictureListDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void pictureListFinished();
    void pictureListError(QNetworkReply::NetworkError code);
    void clearPictureListRequest();

    void thumbnailFinished();
    void thumbnailError(QNetworkReply::NetworkError code);
    void clearThumbnailRequest();

    void pictureDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void pictureFinished();
    void pictureError(QNetworkReply::NetworkError code);
    void clearPictureRequest();

    // QGeoPositionInfoSource
    void positionUpdated(const QGeoPositionInfo &gpsPos);

    // QGeoSatelliteInfoSource
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo>&);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo>&);

private:
    static const QSize gridSize;
    static const QSize thumbnailSize;
    static const QSize imageSize;
    static const QString apikey;
    static const QString savePath;

    void displayImage(const QPixmap &pixmap);

    QLabel *locationLabel;
    QLabel *satellitesLabel;
    XQListWidget *listWidget;
    QPushButton *downloadButton;

    QAction* m_downloadAct;

    bool m_logfileInUse;
    QGeoPositionInfoSource* m_location;
    QGeoSatelliteInfoSource* m_satellite;

    QNetworkSession* m_session;
    ConnectivityHelper *m_connectivityHelper;

    QProgressDialog *m_progressDialog;

    QNetworkAccessManager *m_nam;
    QNetworkReply *m_pictureListReply;
    QNetworkReply *m_thumbnailReply;
    QNetworkReply *m_pictureReply;

    int m_pages;
    int m_page;

    int m_satellitesInView;
    int m_satellitesUsed;
    double m_latitude;
    double m_longitude;

    bool m_downloadPictureList;
    int m_nameCounter;
    QStringList m_names;
    bool m_shuttingDown;
};

class PictureDialog: public QDialog
{
    Q_OBJECT

public:
    PictureDialog(const QPixmap& pixmapd, const QString& pictureName, QWidget* parent = 0);

private:
    QLabel *label;
    QLabel *imageLabel;
    QDialogButtonBox *buttonBox;
};

#endif // FLICKRDEMO_H

