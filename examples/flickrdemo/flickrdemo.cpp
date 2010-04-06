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

#include "flickrdemo.h"
#include <QDomDocument>

#include <qnmeapositioninfosource.h>
#include <qgeopositioninfosource.h>
#include <qgeosatelliteinfosource.h>
#include <qgeopositioninfo.h>
#include <qgeosatelliteinfo.h>
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>

#include <QAction>
#include <QApplication>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QProgressDialog>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

// static constant intialization

const QSize FlickrDemo::gridSize = QSize(52, 52);
const QSize FlickrDemo::thumbnailSize = QSize(50, 50);
const QSize FlickrDemo::imageSize = QSize(150, 150);
const QString FlickrDemo::apikey = QString("e36784df8a03fea04c22ed93318b291c");

FlickrDemo::FlickrDemo(QWidget* parent) :
        QMainWindow(parent),
        m_logfileInUse(false),
        m_session(0),
        m_pictureListReply(0),
        m_thumbnailReply(0),
        m_pictureReply(0),
        m_pages(0),
        m_page(1),
        m_satellitesInView(0),
        m_satellitesUsed(0),
        m_latitude(-1000),
        m_longitude(-1000),
        m_downloadPictureList(true),
        m_shuttingDown(false)
{
    resize(252, 344);

    locationLabel = new QLabel(tr("Lat: Long:"));
    satellitesLabel = new QLabel(tr("Using 0 of 0 satellites"));
    listWidget = new XQListWidget();
    downloadButton = new QPushButton(tr("Download Picture List"));

    QVBoxLayout *verticalLayout = new QVBoxLayout();
    verticalLayout->addWidget(locationLabel);
    verticalLayout->addWidget(satellitesLabel);
    verticalLayout->addWidget(listWidget);
    verticalLayout->addWidget(downloadButton);

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(verticalLayout);
    setCentralWidget(centralWidget);

    createMenus();
    listWidget->setGridSize(gridSize);
    listWidget->setIconSize(thumbnailSize);
    m_progressDialog = new QProgressDialog(this);
    m_progressDialog->setModal(true);
    connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));

    setWindowTitle(tr("Flickr Demo"));

    connect(downloadButton, SIGNAL(clicked()), this, SLOT(downloadButtonClicked()));

    // QGeoPositionInfoSource
    m_location = QGeoPositionInfoSource::createDefaultSource(this);

    if (m_location == 0) {
        QNmeaPositionInfoSource *nmeaLocation = new QNmeaPositionInfoSource(QNmeaPositionInfoSource::SimulationMode, this);
        QFile *logFile = new QFile(QApplication::applicationDirPath()
                                   + QDir::separator() + "nmealog.txt", this);
        nmeaLocation->setDevice(logFile);
        m_location = nmeaLocation;
        m_logfileInUse = true;
    }

    // Listen gps position changes
    connect(m_location, SIGNAL(positionUpdated(QGeoPositionInfo)),
            this, SLOT(positionUpdated(QGeoPositionInfo)));

    // QGeoSatelliteInfoSource
    m_satellite = QGeoSatelliteInfoSource::createDefaultSource(this);
    // Listen satellite status changes
    if (m_satellite != 0) {
        connect(m_satellite, SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo>&)),
                this, SLOT(satellitesInViewUpdated(const QList<QGeoSatelliteInfo>&)));
        connect(m_satellite, SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo>&)),
                this, SLOT(satellitesInUseUpdated(const QList<QGeoSatelliteInfo>&)));
    }

    m_nam = new QNetworkAccessManager(this);

    QTimer::singleShot(0, this, SLOT(delayedInit()));
}

FlickrDemo::~FlickrDemo()
{
    m_shuttingDown = true;

    m_location->stopUpdates();
    if (m_satellite)
        m_satellite->stopUpdates();

    if (m_pictureListReply) {
        m_pictureListReply->abort();
        delete m_pictureListReply;
    }
    if (m_thumbnailReply) {
        m_thumbnailReply->abort();
        delete m_thumbnailReply;
    }
    if (m_pictureReply) {
        m_pictureReply->abort();
        delete m_pictureReply;
    }

    if (m_session)
        m_session->close();
}

void FlickrDemo::delayedInit()
{
    if (m_logfileInUse) {
        QMessageBox::information(this, tr("Flickr Demo"),
                                 tr("No GPS support detected, using GPS data from a sample log file instead."));
    }

    QTM_PREPEND_NAMESPACE(QNetworkConfigurationManager) manager;
    const bool canStartIAP = (manager.capabilities()
                              & QTM_PREPEND_NAMESPACE(QNetworkConfigurationManager)::CanStartAndStopInterfaces);
    QTM_PREPEND_NAMESPACE(QNetworkConfiguration) cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || (!canStartIAP && cfg.state() != QTM_PREPEND_NAMESPACE(QNetworkConfiguration)::Active)) {
        QMessageBox::information(this, tr("Flickr Demo"), tr("Available Access Points not found."));
        return;
    }

    m_session = new QNetworkSession(cfg, this);
    m_connectivityHelper = new ConnectivityHelper(m_session, this);
    connect(m_session, SIGNAL(opened()), this, SLOT(networkSessionOpened()));
    connect(m_connectivityHelper, SIGNAL(networkingCancelled()), qApp, SLOT(quit()));

    m_session->open();
}

void FlickrDemo::networkSessionOpened()
{
    // Start listening GPS position updates
    m_location->startUpdates();

    // Start listening GPS satellite status changes
    if (m_satellite != 0) {
        m_satellite->startUpdates();
    } else {
        satellitesLabel->setText(tr("GPS not detected, replaying coordinates from sample log file."));
    }
}

void FlickrDemo::createMenus()
{
    m_downloadAct = new QAction(tr("Download Picture List"), this);
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE)
    menuBar()->addAction(m_downloadAct);
#endif
    connect(m_downloadAct, SIGNAL(triggered()), this, SLOT(downloadButtonClicked()));

    QAction* exitAct = new QAction(tr("Exit"), this);
#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE)
    menuBar()->addAction(exitAct);
#endif
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void FlickrDemo::positionUpdated(const QGeoPositionInfo &gpsPos)
{
    if (gpsPos.isValid()) {
        QGeoCoordinate coord = gpsPos.coordinate();
        m_latitude = coord.latitude();
        m_longitude = coord.longitude();

        locationLabel->setText(tr("Lat: %1 Long: %2").arg(QString::number(m_latitude),
                               QString::number(m_longitude)));
    }
}

void FlickrDemo::satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites)
{
    m_satellitesInView = satellites.count();
    viewSatelliteInfo();
}
void FlickrDemo::satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites)
{
    m_satellitesUsed = satellites.count();
    viewSatelliteInfo();
}

void FlickrDemo::viewSatelliteInfo()
{
    if (m_satellite != 0) {
        satellitesLabel->setText(tr("Using %1 of %2 satellites").arg(QString::number(m_satellitesUsed), QString::number(m_satellitesInView)));
    } else {
        satellitesLabel->setText(tr("GPS not detected, replaying coordinates from sample log file."));
    }
}

void FlickrDemo::downloadButtonClicked()
{
    if (m_downloadPictureList) {
        downloadFlickerPictureList();
    } else {
        downloadPictureFromFlickr();
    }
}

void FlickrDemo::downloadFlickerPictureList()
{
    if (m_latitude == -1000 || m_longitude == -1000) {
        if (QMessageBox::question(this,
                                  tr("Flickr Demo"),
                                  tr("No satellite connection.\n""Use hard coded coordinates?"),
                                  QMessageBox::Ok | QMessageBox::Cancel,
                                  QMessageBox::Cancel)
                == QMessageBox::Cancel) {
            return;
        }
        // If GPS signal is not detected, Tampere area is used as default location
        m_latitude = 61.4500;
        m_longitude = 23.8502;
    }

    // Set IAP name
    satellitesLabel->setText(tr("Access Point: %1").arg(m_session->configuration().name()));
    locationLabel->setText(tr("Lat: %1 Long: %2").arg(QString::number(m_latitude),
                           QString::number(m_longitude)));

    QString
    urlstring =
        "http://api.flickr.com/services/rest/?min_taken_date=2000-01-01+0:00:00&extras=date_taken&method=flickr.photos.search&per_page=30&sort=date-taken-desc";
    urlstring.append("&api_key=");
    urlstring.append(apikey);
    urlstring.append("&lat=");
    urlstring.append(QString::number(m_latitude));
    urlstring.append("&lon=");
    urlstring.append(QString::number(m_longitude));
    urlstring.append("&page=");
    urlstring.append(QString::number(m_page));

    QUrl url(urlstring);

    QNetworkRequest req(url);
    m_pictureListReply = m_nam->get(req);
    connect(m_pictureListReply,
            SIGNAL(downloadProgress(qint64, qint64)),
            this,
            SLOT(pictureListDownloadProgress(qint64, qint64)));
    connect(m_pictureListReply,
            SIGNAL(finished()),
            this,
            SLOT(pictureListFinished()));
    connect(m_pictureListReply,
            SIGNAL(error(QNetworkReply::NetworkError)),
            this,
            SLOT(pictureListError(QNetworkReply::NetworkError)));

    m_progressDialog->setWindowTitle(tr("FlickrDemo"));
    m_progressDialog->setLabelText(tr("Downloading\nPicture List."));
    m_progressDialog->setMaximum(10);
    m_progressDialog->setValue(0);
    m_progressDialog->show();
}

bool FlickrDemo::parsePictureList(QString xmlString)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument domDocument;
    if (!domDocument.setContent(xmlString, true, &errorStr, &errorLine, &errorColumn)) {
        QMessageBox::information(window(),
                                 tr("Flickr Demo"),
                                 tr("XML Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr));
        return false;
    }
    QDomElement root = domDocument.documentElement();

    if (root.tagName() != "rsp") {
        QMessageBox::information(window(), tr("Flickr Demo"), tr("Invalid XML format."));
        return false;
    }
    if (root.attribute("stat") != "ok") {
        QMessageBox::information(window(), tr("Flickr Demo"), tr("Query failed."));
        return false;
    }

    listWidget->clear();
    QListWidgetItem* listItem;
    QDomElement photosElement = root.firstChildElement("photos");
    if (!photosElement.isNull()) {
        m_pages = (photosElement.attribute("pages")).toInt();
        m_page = (photosElement.attribute("page")).toInt();

        QDomElement child = photosElement.firstChildElement("photo");
        while (!child.isNull()) {
            QString url = "http://farm";
            url.append(child.attribute("farm"));
            url.append(".static.flickr.com/");
            url.append(child.attribute("server"));
            url.append("/");
            url.append(child.attribute("id"));
            url.append("_");
            url.append(child.attribute("secret"));

            m_names << url;

            listItem = new QListWidgetItem(child.attribute("title"));
            QImage image(thumbnailSize, QImage::Format_Mono);
            image.fill(1);
            listItem->setIcon(QPixmap::fromImage(image));
            listItem->setData(Qt::UserRole, url);
            listWidget->addItem(listItem);

            child = child.nextSiblingElement("photo");
        }
    } else {
        return false;
    }
    m_nameCounter = -1;
    downloadNextThumbnail();

    return true;
}

void FlickrDemo::downloadPictureFromFlickr()
{
    QListWidgetItem* item = listWidget->currentItem();
    if (!item) {
        return;
    }
    QString pictureUrl = item->data(Qt::UserRole).toString();
    pictureUrl.append("_m.jpg");

    QUrl url(pictureUrl);

    QNetworkRequest req(url);
    m_pictureReply = m_nam->get(req);
    connect(m_pictureReply,
            SIGNAL(downloadProgress(qint64, qint64)),
            this,
            SLOT(pictureDownloadProgress(qint64, qint64)));
    connect(m_pictureReply,
            SIGNAL(finished()),
            this,
            SLOT(pictureFinished()));
    connect(m_pictureReply,
            SIGNAL(error(QNetworkReply::NetworkError)),
            this,
            SLOT(pictureError(QNetworkReply::NetworkError)));

    m_progressDialog->setWindowTitle(tr("Flickr Demo"));
    m_progressDialog->setLabelText(tr("Downloading:\n%1").arg(pictureUrl));
    m_progressDialog->setMaximum(10);
    m_progressDialog->setValue(0);
    m_progressDialog->show();

    downloadButton->setEnabled(false);
}

void FlickrDemo::cancelDownload()
{
    if (m_pictureListReply) {
        m_pictureListReply->abort();
        delete m_pictureListReply;
        m_pictureListReply = 0;
    }

    if (m_pictureReply) {
        m_pictureReply->abort();
        delete m_pictureReply;
        m_pictureReply = 0;
    }

    downloadButton->setEnabled(true);
}

void FlickrDemo::pictureListDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    m_progressDialog->setMaximum(bytesTotal);
    m_progressDialog->setValue(bytesReceived);
}

void FlickrDemo::pictureListFinished()
{
    m_progressDialog->hide();
    if (parsePictureList(QString::fromUtf8(m_pictureListReply->readAll()))) {
        m_downloadPictureList = false;
        downloadButton->setText(tr("Download Selected Picture"));
        m_downloadAct->setText(tr("Download Selected Picture"));
    }

    downloadButton->setEnabled(true);

    QTimer::singleShot(0, this, SLOT(clearPictureListRequest()));
}

void FlickrDemo::pictureListError(QNetworkReply::NetworkError code)
{
    if (m_shuttingDown)
        return;

    m_progressDialog->hide();
    QMessageBox::information(this,
                             tr("Flickr Demo"),
                             tr("Error downloading picture list: %1.").arg(m_pictureListReply->errorString()));

    QTimer::singleShot(0, this, SLOT(clearPictureListRequest()));
}

void FlickrDemo::clearPictureListRequest()
{
    delete m_pictureListReply;
    m_pictureListReply = 0;
}

void FlickrDemo::thumbnailFinished()
{
    QByteArray picture = m_thumbnailReply->readAll();
    if (!picture.isNull() && picture.size() > 0) {
        QListWidgetItem* item = listWidget->item(m_nameCounter);
        QImage image;
        if (image.loadFromData(picture, "jpg")) {
            item->setIcon(QPixmap::fromImage(
                              image.scaled(thumbnailSize,
                                           Qt::KeepAspectRatio,
                                           Qt::SmoothTransformation)));
            listWidget->update();
        }
    }
    downloadNextThumbnail();
}

void FlickrDemo::thumbnailError(QNetworkReply::NetworkError code)
{
    if (m_shuttingDown)
        return;

    QMessageBox::information(this,
                             tr("Flickr Demo"),
                             tr("Error downloading thumbnails: %1.").arg(m_thumbnailReply->errorString()));

    QTimer::singleShot(0, this, SLOT(clearThumbnailRequest()));
}

void FlickrDemo::clearThumbnailRequest()
{
    delete m_thumbnailReply;
    m_thumbnailReply = 0;
}

void FlickrDemo::pictureDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    m_progressDialog->setMaximum(bytesTotal);
    m_progressDialog->setValue(bytesReceived);
}

void FlickrDemo::pictureFinished()
{
    m_progressDialog->hide();
    downloadButton->setEnabled(true);

    QByteArray picture = m_pictureReply->readAll();

    if (picture.isNull() || picture.size() <= 0)
        return;

    QImage image;
    if (!image.loadFromData(picture, "jpg"))
        return;

    QPixmap pixmap = QPixmap::fromImage(
                         image.scaled(imageSize,
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation));

    displayImage(pixmap);

    QTimer::singleShot(0, this, SLOT(clearPictureRequest()));
}

void FlickrDemo::pictureError(QNetworkReply::NetworkError code)
{
    if (m_shuttingDown)
        return;

    m_progressDialog->hide();
    QMessageBox::information(this,
                             tr("Flickr Demo"),
                             tr("Error downloading picture: %1.").arg(m_pictureReply->errorString()));

    downloadButton->setEnabled(true);

    QTimer::singleShot(0, this, SLOT(clearPictureRequest()));
}

void FlickrDemo::clearPictureRequest()
{
    delete m_pictureReply;
    m_pictureReply = 0;
}

void FlickrDemo::displayImage(const QPixmap &pixmap)
{
    PictureDialog dialog(pixmap, listWidget->currentItem()->text(), this);
#if defined(Q_OS_SYMBIAN) || defined (Q_OS_WINCE)
    dialog.showMaximized();
#endif
    dialog.exec();
}

void FlickrDemo::downloadNextThumbnail()
{
    m_nameCounter++;
    if (m_nameCounter < m_names.count()) {
        QString pictureUrl = m_names[m_nameCounter];
        pictureUrl.append("_s.jpg");
        QUrl url(pictureUrl);

        QNetworkRequest req(url);
        m_thumbnailReply = m_nam->get(req);
        connect(m_thumbnailReply,
                SIGNAL(finished()),
                this,
                SLOT(thumbnailFinished()));
        connect(m_thumbnailReply,
                SIGNAL(error(QNetworkReply::NetworkError)),
                this,
                SLOT(thumbnailError(QNetworkReply::NetworkError)));
    } else {
        QTimer::singleShot(0, this, SLOT(clearThumbnailRequest()));
    }
}

PictureDialog::PictureDialog(const QPixmap& pixmap, const QString& pictureName, QWidget* parent) :
        QDialog(parent)
{
    resize(252, 361);
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(6);
    verticalLayout->setContentsMargins(11, 11, 11, 11);

    label = new QLabel();
    label->setText(tr("Downloaded:\n%1").arg(pictureName));

    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
    label->setSizePolicy(sizePolicy);

    verticalLayout->addWidget(label);

    imageLabel = new QLabel();
    imageLabel->setPixmap(pixmap);

    verticalLayout->addWidget(imageLabel);

    buttonBox = new QDialogButtonBox();
    buttonBox->setStandardButtons(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(accept()));

    verticalLayout->addWidget(buttonBox);

    setLayout(verticalLayout);

    setWindowTitle(tr("Flickr Demo"));
}
