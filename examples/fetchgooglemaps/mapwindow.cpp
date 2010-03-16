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

#include <QtGui>
#include <QtWebKit>

#include <qgeopositioninfosource.h>
#include <qnmeapositioninfosource.h>
#include <qgeosatelliteinfosource.h>
#ifndef Q_WS_MAEMO_5
#include <qnetworksession.h>
#include <qnetworkconfigmanager.h>
#endif

#include "satellitedialog.h"

#include "mapwindow.h"

// Use the special 'localhost' key for the Google Maps key
const QString GMAPS_STATICMAP_URL_TEMPLATE =  "http://maps.google.com/staticmap?center=%1,%2&zoom=14&size=%3x%4&map type=mobile&markers=%1,%2&key=ABQIAAAAnfs7bKE82qgb3Zc2YyS-oBT2yXp_ZAY8_ufC3CFXhHIE1NvwkxSySz_REpPq-4WZA27OwgbtyR3VcA&sensor=false";


MapWindow::MapWindow(QWidget *parent, Qt::WFlags flags)
        : QMainWindow(parent, flags),
        webView(new QWebView),
        posLabel(new QLabel),
        headingAndSpeedLabel(new QLabel),
        dateTimeLabel(new QLabel),
        loading(false),
        usingLogFile(false),
        location(0),
        waitingForFix(false)
{
    location = QGeoPositionInfoSource::createDefaultSource(this);
    if (!location) {
        QNmeaPositionInfoSource *nmeaSource = new QNmeaPositionInfoSource(QNmeaPositionInfoSource::SimulationMode, this);
        QFile *logFile = new QFile(QApplication::applicationDirPath()
                                   + QDir::separator() + "nmealog.txt", this);
        nmeaSource->setDevice(logFile);
        location = nmeaSource;

        usingLogFile = true;
    }

    location->setUpdateInterval(5000);

    connect(location, SIGNAL(positionUpdated(QGeoPositionInfo)),
            this, SLOT(positionUpdated(QGeoPositionInfo)));

    connect(webView, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));

    QWidget *mainWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(mainWidget);
    layout->addWidget(webView);
    layout->addWidget(posLabel);
    layout->addWidget(headingAndSpeedLabel);
    layout->addWidget(dateTimeLabel);
    setCentralWidget(mainWidget);

#if !defined(Q_OS_SYMBIAN)
    resize(300, 300);
#endif
    setWindowTitle(tr("Google Maps Demo"));

    QTimer::singleShot(0, this, SLOT(delayedInit()));
}

MapWindow::~MapWindow()
{
    location->stopUpdates();
#ifndef Q_WS_MAEMO_5
    session->close();
#endif
}

void MapWindow::delayedInit()
{
    if (usingLogFile) {
        QMessageBox::information(this, tr("Fetch Google Maps"),
                                 tr("No GPS support detected, using GPS data from a sample log file instead."));
    } else {
        waitForFix();
        location->stopUpdates();
    }

#ifndef Q_WS_MAEMO_5
    // Set Internet Access Point
    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities()
                              & QNetworkConfigurationManager::CanStartAndStopInterfaces);
    // Is there default access point, use it
    QNetworkConfiguration cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || (!canStartIAP && cfg.state() != QNetworkConfiguration::Active)) {
        QMessageBox::information(this, tr("Flickr Demo"), tr(
                                     "Available Access Points not found."));
        return;
    }

    session = new QNetworkSession(cfg, this);
    session->open();
    session->waitForOpened(-1);
#endif
    connect(location, SIGNAL(updateTimeout()), this, SLOT(waitForFix()));

    location->startUpdates();
}

// Brings up a satellite strength dialog box until a position fix is received.
// This will also start the position updates if they are not already started.
void MapWindow::waitForFix() {
    if (waitingForFix)
        return;

    waitingForFix = true;

    QGeoSatelliteInfoSource *satellite = QGeoSatelliteInfoSource::createDefaultSource(this);

    if (satellite) {
        SatelliteDialog *dialog = new SatelliteDialog(this,
                30,
                SatelliteDialog::ExitOnFixOrCancel,
                SatelliteDialog::OrderByPrnNumber,
                SatelliteDialog::ScaleToMaxPossible);

        dialog->connectSources(location, satellite);

        location->startUpdates();
        satellite->startUpdates();

        dialog->exec();

        satellite->stopUpdates();

        delete dialog;
        delete satellite;
    }

    waitingForFix = false;
}

void MapWindow::start()
{
    location->startUpdates();
}

void MapWindow::positionUpdated(const QGeoPositionInfo &info)
{
    QString heading = "?";
    QString speed = "?";
    if (info.hasAttribute(QGeoPositionInfo::Direction))
        heading = QString("%1%2").arg(info.attribute(QGeoPositionInfo::Direction)).arg(QChar(0x00b0));
    if (info.hasAttribute(QGeoPositionInfo::GroundSpeed))
        speed = QString::number(info.attribute(QGeoPositionInfo::GroundSpeed) * 3.6, 'f', 1);
    posLabel->setText(tr("Position: %1").arg(info.coordinate().toString()));
    headingAndSpeedLabel->setText(tr("Bearing %1, travelling at %2 km/h").arg(heading).arg(speed));

    dateTimeLabel->setText(tr("(Last update: %1)").
                           arg(info.dateTime().toLocalTime().time().toString()));

    if (!loading) {
        // Google Maps does not provide maps larger than 640x480
        int width = qMin(webView->width(), 640);
        int height = qMin(webView->height(), 480);
        QString url = GMAPS_STATICMAP_URL_TEMPLATE
                      .arg(QString::number(info.coordinate().latitude()))
                      .arg(QString::number(info.coordinate().longitude()))
                      .arg(QString::number(width))
                      .arg(QString::number(height));
        webView->load(url);
    }
}

void MapWindow::loadStarted()
{
    loading = true;
    webView->setUpdatesEnabled(false);
}

void MapWindow::loadFinished(bool)
{
    loading = false;
    webView->setUpdatesEnabled(true);
}
