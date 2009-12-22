/*
 * Copyright (c) 2009 Nokia Corporation.
 */

#include "flickrdemo.h"
#include <QDomDocument>

#include <qnmeapositioninfosource.h>

static const QSize gridSize(52, 52);
static const QSize thumbnailSize(50, 50);
static const QSize imageSize(150, 150);
static const QString apikey = "e36784df8a03fea04c22ed93318b291c";

#ifdef Q_OS_SYMBIAN
static const QString savePath = "c:\\Data\\Images\\"; // In S60 Download images to Gallery
#else
static const QString savePath = QDir::tempPath();
#endif

FlickrDemo::FlickrDemo(QWidget* parent) :
        QMainWindow(parent),
        m_session(0),
        m_logfileInUse(false),
        m_file(0),
        m_httpGetId(-1),
        m_httpThumbnailGetId(-1),
        m_pages(0),
        m_page(1),
        m_satellitesInView(0),
        m_satellitesUsed(0),
        m_latitude(-1000),
        m_longitude(-1000),
        m_downloadPictureList(true),
        m_downloadingThumbnails(false)
{    
    resize(252, 344);
    QWidget *centralWidget = new QWidget;        
    QVBoxLayout *verticalLayout = new QVBoxLayout();

    locationLabel = new QLabel(tr("Lat: Long:"));        
    verticalLayout->addWidget(locationLabel);

    satellitesLabel = new QLabel(tr("Using 0 of 0 satellites"));        
    verticalLayout->addWidget(satellitesLabel);

    listWidget = new XQListWidget();
    verticalLayout->addWidget(listWidget);

    downloadButton = new QPushButton(tr("Download Picture List"));        
    verticalLayout->addWidget(downloadButton);

    centralWidget->setLayout(verticalLayout);
    setCentralWidget(centralWidget);

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

    // QHttp
    connect(&m_http, SIGNAL(requestFinished(int, bool)),
            this, SLOT(httpRequestFinished(int, bool)));
    connect(&m_http, SIGNAL(dataReadProgress(int, int)),
            this, SLOT(updateDataReadProgress(int, int)));
    connect(&m_http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)),
            this, SLOT(readResponseHeader(const QHttpResponseHeader&)));

#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE)
    createMenus();
#endif
    listWidget->setGridSize(gridSize);
    listWidget->setIconSize(thumbnailSize);
    connect(&m_progressDialog, SIGNAL(canceled()), this, SLOT(cancelDownload()));

    // Start listening GPS position updates
    m_location->startUpdates();

    // Start listening GPS satellite status changes
    if (m_satellite != 0) {
        m_satellite->startUpdates();
    } else {
        satellitesLabel->setText(tr("GPS not detected, replaying coordinates from sample log file."));
    }

    QTimer::singleShot(100, this, SLOT(delayedInit()));
}

FlickrDemo::~FlickrDemo()
{
    m_location->stopUpdates();
    if (m_satellite)
        m_satellite->stopUpdates();
    m_http.abort();
    if (m_session)
        m_session->close();    
}


void FlickrDemo::delayedInit()
{
    if (m_logfileInUse) {
        QMessageBox::information(this, tr("Flickr Demo"), 
            tr("No GPS support detected, using GPS data from a sample log file instead."));
    }
}

void FlickrDemo::createMenus()
{
    m_downloadAct = new QAction(tr("Download Picture List"), this);
    menuBar()->addAction(m_downloadAct);
    connect(m_downloadAct, SIGNAL(triggered()), this, SLOT(on_downloadButton_clicked()));
    QAction* exitAct = new QAction(tr("Exit"), this);
    menuBar()->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void FlickrDemo::positionUpdated(QGeoPositionInfo gpsPos)
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
        satellitesLabel->setText(tr("Using %1 of %2 satellites").arg(
                                 QString::number(m_satellitesUsed), QString::number(m_satellitesInView)));    
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
        if (QMessageBox::question(this, tr("Flickr Demo"), tr("No satellite connection.\n"
                                  "Use hard coded coordinates?"), QMessageBox::Ok | QMessageBox::Cancel,
                                  QMessageBox::Cancel) == QMessageBox::Cancel) {
            return;
        }
        // If GPS signal is not detected, Tampere area is used as default location
        m_latitude = 61.4500;
        m_longitude = 23.8502;
    }

    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities()
                              & QNetworkConfigurationManager::CanStartAndStopInterfaces);
    QNetworkConfiguration cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || (!canStartIAP && cfg.state() != QNetworkConfiguration::Active)) {
        QMessageBox::information(this, tr("Flickr Demo"), tr("Available Access Points not found."));
        return;
    }
    m_session = new QNetworkSession(cfg, this);    
    m_session->open();

    bool ok = m_session->waitForOpened();
    if (ok) {
        // Set IAP name
        satellitesLabel->setText(tr("Access Point: %1").arg(cfg.name()));

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

        m_http.setHost(url.host(), QHttp::ConnectionModeHttp, url.port() == -1 ? 0 : url.port());
        m_httpRequestAborted = false;

        m_httpGetId = m_http.get(urlstring);

        m_progressDialog.setWindowTitle(tr("FlickrDemo"));
        m_progressDialog.setLabelText(tr("Downloading\nPicture List."));
        m_progressDialog.setMaximum(10);
        m_progressDialog.setValue(1);
    }
}

bool FlickrDemo::parsePictureList(QString xmlString)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument domDocument;
    if (!domDocument.setContent(xmlString, true, &errorStr, &errorLine, &errorColumn)) {
        QMessageBox::information(window(), tr("Flickr Demo"), tr(
                                     "XML Parse error at line %1, column %2:\n%3") .arg(errorLine) .arg(errorColumn) .arg(
                                     errorStr));
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
    QFileInfo fileInfo(url.path());
    QString fileName = fileInfo.fileName();
    if (fileName.isEmpty()) {
        fileName = "test.jpg";
    }

    m_filePath = savePath;
    m_filePath.append(fileName);

    if (QFile::exists(m_filePath)) {
        if (QMessageBox::question(this, tr("Flickr Demo"), tr("File %1 is already downloaded."
                                  "Overwrite?").arg(fileName), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel)
                == QMessageBox::Cancel) {
            return;
        }
        QFile::remove(m_filePath);
    }

    m_file = new QFile(m_filePath);
    if (!m_file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Flickr Demo"),
                                 tr("Unable to save the file %1: %2.") .arg(m_filePath).arg(m_file->errorString()));
        delete m_file;
        m_file = 0;
        return;
    }

    m_http.setHost(url.host(), QHttp::ConnectionModeHttp, url.port() == -1 ? 0 : url.port());

    m_httpRequestAborted = false;
    QByteArray encodedUrl = QUrl::toPercentEncoding(url.path(), "!$&'()*+,;=:@/");
    if (encodedUrl.isEmpty()) {
        encodedUrl = "/";
    }
    m_httpGetId = m_http.get(encodedUrl, m_file);

    m_progressDialog.setWindowTitle(tr("Flickr Demo"));
    m_progressDialog.setLabelText(tr("Downloading:\n%1").arg(fileName));
    m_progressDialog.setMaximum(10);
    m_progressDialog.setValue(1);

    downloadButton->setEnabled(false);
}

void FlickrDemo::cancelDownload()
{
    m_httpRequestAborted = true;
    m_downloadingThumbnails = false;
    m_http.abort();
    downloadButton->setEnabled(true);
}

void FlickrDemo::httpRequestFinished(int requestId, bool error)
{
    if (m_downloadingThumbnails && m_httpThumbnailGetId == requestId) {
        if (!error) {
            QByteArray picture = m_http.readAll();
            if (!picture.isNull() && picture.size() > 0) {
                QListWidgetItem* item = listWidget->item(m_nameCounter);
                QImage image;
                if (image.loadFromData(picture, "jpg")) {
                    item->setIcon(QPixmap::fromImage(image.scaled(thumbnailSize,
                                                     Qt::KeepAspectRatio, Qt::SmoothTransformation)));
                    listWidget->update();
                }
            }
        }
        downloadNextThumbnail();
        return;
    }

    if (requestId != m_httpGetId) {
        return;
    }

    m_progressDialog.hide();

    if (m_httpRequestAborted) {
        if (m_file) {
            m_file->close();
            m_file->remove();
            delete m_file;
            m_file = 0;
        }

        return;
    }

    if (!m_downloadPictureList) {
        m_file->close();
    }

    if (error) {
        if (!m_downloadPictureList) {
            m_file->remove();
        }
        QMessageBox::information(this, tr("Flickr Demo"), tr("Download failed: %1.") .arg(
                                     m_http.errorString()));
    }

    if (m_downloadPictureList) {
        if (parsePictureList(QString::fromUtf8(m_http.readAll()))) {
            m_downloadPictureList = false;
            downloadButton->setText(tr("Download Selected Picture"));
            m_downloadAct->setText(tr("Download Selected Picture"));
        }
    } else {
        PictureDialog dialog(m_filePath, listWidget->currentItem()->text(), this);
#if defined(Q_OS_SYMBIAN) || defined (Q_OS_WINCE)
        dialog.showMaximized();
#endif
        if (!dialog.exec()) {
            if (m_file->exists()) {
                m_file->remove();
            }
        }
        delete m_file;
        m_file = 0;
    }

    downloadButton->setEnabled(true);
}

void FlickrDemo::readResponseHeader(const QHttpResponseHeader& responseHeader)
{
    switch (responseHeader.statusCode()) {
        case 200: // Ok
        case 301: // Moved Permanently
        case 302: // Found
        case 303: // See Other
        case 307: // Temporary Redirect
            // these are not error conditions
            break;
        default:
            QMessageBox::information(this, tr("Flickr Demo"), tr("Download failed: %1.") .arg(
                                         responseHeader.reasonPhrase()));
            m_downloadingThumbnails = false;
            m_httpRequestAborted = true;
            m_progressDialog.hide();
            m_http.abort();
    }
}

void FlickrDemo::updateDataReadProgress(int bytesRead, int totalBytes)
{
    if (m_httpRequestAborted) {
        return;
    }

    if (!m_downloadingThumbnails) {
        m_progressDialog.setMaximum(totalBytes);
        m_progressDialog.setValue(bytesRead);
    }
}

void FlickrDemo::downloadNextThumbnail()
{
    m_nameCounter++;
    if (m_nameCounter < m_names.count()) {
        QString pictureUrl = m_names[m_nameCounter];
        pictureUrl.append("_s.jpg");
        QUrl url(pictureUrl);
        m_http.setHost(url.host(), QHttp::ConnectionModeHttp, url.port() == -1 ? 0 : url.port());
        m_downloadingThumbnails = true;
        m_httpThumbnailGetId = m_http.get(pictureUrl);
    } else {
        m_downloadingThumbnails = false;
    }
}

PictureDialog::PictureDialog(const QString& filePath, const QString& pictureName, QWidget* parent) :
        QDialog(parent)
{    
    resize(252, 361);
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(6);
    verticalLayout->setContentsMargins(11, 11, 11, 11);        

    label = new QLabel();
    QString fileName = QFileInfo(filePath).fileName();
    label->setText(tr("Downloaded:\n%1\n%2").arg(pictureName).arg(fileName));

    QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
    label->setSizePolicy(sizePolicy);

    verticalLayout->addWidget(label);

    imageLabel = new QLabel();        
    QImage image;
    image.load(filePath);
    imageLabel->setPixmap(QPixmap::fromImage(image.scaled(imageSize, Qt::KeepAspectRatio,
                          Qt::SmoothTransformation)));

    verticalLayout->addWidget(imageLabel);

    buttonBox = new QDialogButtonBox();        
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    verticalLayout->addWidget(buttonBox);

    setLayout(verticalLayout);

    setWindowTitle(tr("Flickr Demo"));
}

