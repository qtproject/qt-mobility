/*
 * Copyright (c) 2009 Nokia Corporation.
 */

#ifndef FLICKRDEMO_H
#define FLICKRDEMO_H

#include <QtGui>
#include <QtNetwork>

//// QtMobility API headers
#include <qgeopositioninfosource.h>
#include <qgeopositioninfo.h>
#include <qgeosatelliteinfosource.h>
#include <qgeosatelliteinfo.h>
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>

#include "xqlistwidget.h"

// Use the QtMobility namespace
using namespace QtMobility;

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

    void downloadButtonClicked();
    void cancelDownload();

    void httpRequestFinished(int requestId, bool error);
    void readResponseHeader(const QHttpResponseHeader& responseHeader);
    void updateDataReadProgress(int bytesRead, int totalBytes);

    // QGeoPositionInfoSource
    void positionUpdated(QGeoPositionInfo);

    // QGeoSatelliteInfoSource
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo>&);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo>&);

private:
    QLabel *locationLabel;
    QLabel *satellitesLabel;
    XQListWidget *listWidget;
    QPushButton *downloadButton;

    QAction* m_downloadAct;

    bool m_logfileInUse;
    QGeoPositionInfoSource* m_location;
    QGeoSatelliteInfoSource* m_satellite;
    QNetworkSession* m_session;

    QProgressDialog m_progressDialog;
    QHttp m_http;
    QString m_filePath;
    QFile* m_file;
    int m_httpGetId;
    int m_httpThumbnailGetId;
    bool m_httpRequestAborted;

    int m_pages;
    int m_page;

    int m_satellitesInView;
    int m_satellitesUsed;
    double m_latitude;
    double m_longitude;

    bool m_downloadPictureList;

    bool m_downloadingThumbnails;
    int m_nameCounter;
    QStringList m_names;
};

class PictureDialog: public QDialog
{
Q_OBJECT

public:
    PictureDialog(const QString& filePath, const QString& pictureName, QWidget* parent = 0);

private:
    QLabel *label;
    QLabel *imageLabel;
    QDialogButtonBox *buttonBox;
};

#endif // FLICKRDEMO_H

