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

#include <QtGui>
#include <QtWebKit>

#include <qgeopositioninfosource.h>
#include <qnmeapositioninfosource.h>
#include <qgeosatelliteinfosource.h>
#include <qnetworksession.h>
#include <qnetworkconfigmanager.h>

#include "satellitedialog.h"

#include "mapwindow.h"

QTM_USE_NAMESPACE

// Use the special 'localhost' key for the Google Maps key
const QString GMAPS_STATICMAP_URL_TEMPLATE =  "http://maps.google.com/staticmap?center=%1,%2&zoom=14&size=%3x%4&map type=mobile&markers=%1,%2&key=ABQIAAAAnfs7bKE82qgb3Zc2YyS-oBT2yXp_ZAY8_ufC3CFXhHIE1NvwkxSySz_REpPq-4WZA27OwgbtyR3VcA&sensor=false";


MapWindow::MapWindow(QWidget *parent, Qt::WFlags flags)
        : QMainWindow(parent, flags),
        loading(false),
        usingLogFile(false),
        location(0),
        waitingForFix(false)
{
    webView = new QWebView(this);
    webView->setMaximumSize(640, 480);

    posLabel = new QLabel(this);
    posLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    headingAndSpeedLabel = new QLabel(this);
    headingAndSpeedLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    dateTimeLabel = new QLabel(this);
    dateTimeLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

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
    layout->addWidget(webView, 1);
    
    QVBoxLayout *labelLayout = new QVBoxLayout();
    labelLayout->addWidget(posLabel);
    labelLayout->addWidget(headingAndSpeedLabel);
    labelLayout->addWidget(dateTimeLabel);

    layout->addLayout(labelLayout, 0);
    layout->setSizeConstraint(QLayout::SetMaximumSize);

    int maxHeight = webView->maximumSize().height();
    maxHeight += posLabel->size().height();
    maxHeight += headingAndSpeedLabel->size().height();
    maxHeight += dateTimeLabel->size().height();

    setMaximumHeight(maxHeight);

    int maxWidth = webView->maximumWidth();
    maxWidth = qMin(maxWidth, posLabel->maximumWidth());
    maxWidth = qMin(maxWidth, headingAndSpeedLabel->maximumWidth());
    maxWidth = qMin(maxWidth, dateTimeLabel->maximumWidth());
    
    setMaximumWidth(maxWidth);

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
    session->close();
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

    // Set Internet Access Point
    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities()
                              & QNetworkConfigurationManager::CanStartAndStopInterfaces);
    // Is there default access point, use it
    QTM_PREPEND_NAMESPACE(QNetworkConfiguration) cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || (!canStartIAP && cfg.state() != QTM_PREPEND_NAMESPACE(QNetworkConfiguration)::Active)) {
        QMessageBox::information(this, tr("Flickr Demo"), tr(
                                     "Available Access Points not found."));
        return;
    }

    session = new QTM_PREPEND_NAMESPACE(QNetworkSession)(cfg, this);
    session->open();
    session->waitForOpened(-1);
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
                           arg(info.timestamp().toLocalTime().time().toString()));

    if (!loading) {
        // Google Maps does not provide maps larger than 640x480
        int width = qMin(webView->width(), 640);
        int height = qMin(webView->height(), 480);

        if ((width == 0) || (height == 0))
            return;

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
