/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
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

#include "mainwindow.h"
#include "mapbox.h"

#include <qgeocoordinate.h>
#include <qgraphicsgeomap.h>
#include <qgeomaprectangleobject.h>

#include <QApplication>
#include <QMessageBox>
#include <QGridLayout>
#include <QPushButton>
#include <QTimer>
#include <QLabel>

#include <qnetworksession.h>
#include <qnetworkconfigmanager.h>

MainWindow::MainWindow(const QVariantHash &programOptions, QWidget *parent)
    : QMainWindow(parent),
    m_programOptions(programOptions)
{
    // Set Internet Access Point
    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities()
                              & QNetworkConfigurationManager::CanStartAndStopInterfaces);
    // Is there default access point, use it
    QNetworkConfiguration cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || (!canStartIAP && cfg.state() != QNetworkConfiguration::Active)) {
        QMessageBox::information(0, tr("Map Viewer Demo"), tr(
                                     "Available Access Points not found."));
        qApp->quit();
        return;
    }

    bool open_session = true;

    if (m_programOptions.contains("session"))
        open_session = m_programOptions["session"].toBool();
    else if (m_programOptions.contains("nosession"))
        open_session = !m_programOptions["nosession"].toBool();

    if (open_session) {
        m_session = new QNetworkSession(cfg, this);

        connect(m_session, SIGNAL(opened()), this, SLOT(networkSessionOpened()));
        connect(m_session,
                SIGNAL(error(QNetworkSession::SessionError)),
                this,
                SLOT(networkSessionError(QNetworkSession::SessionError)));

        m_session->open();
    }
    else {
        qDebug("Bypassing session...");
        networkSessionOpened();
    }

    setWindowTitle("MapViewer");

    resize(640, 480);
}

MainWindow::~MainWindow()
{
}

void MainWindow::networkSessionOpened()
{
    QString provider = "nokia";

    if (m_programOptions.contains("plugin"))
        provider = m_programOptions["plugin"].toString();
    else if (m_programOptions.contains("provider"))
        provider = m_programOptions["provider"].toString();
    else if (m_programOptions.contains("online"))
        provider = m_programOptions["online"].toBool() ? "nokia" : "nokia_mos";
    else if (m_programOptions.contains("offline"))
        provider = m_programOptions["offline"].toBool() ? "nokia_mos" : "nokia";

    if (provider == "online")
        provider = "nokia";
    else if (provider == "offline" || provider == "mos")
        provider = "nokia_mos";

    m_box = new MapBox(this);

    if (m_programOptions.contains("mode")) {
        QString modeString = m_programOptions["mode"].toString().toLower();
        if ((modeString == "noconnectivity") || (modeString == "none"))
            m_box->setConnectivityMode(QGraphicsGeoMap::NoConnectivity);
        else if (modeString == "offline")
            m_box->setConnectivityMode(QGraphicsGeoMap::OfflineMode);
        else if (modeString == "online")
            m_box->setConnectivityMode(QGraphicsGeoMap::OnlineMode);
        else if (modeString == "hybrid")
            m_box->setConnectivityMode(QGraphicsGeoMap::HybridMode);
    }

    m_box->setProvider(provider);

    setCentralWidget(m_box);

    m_box->addRectangle(55.1977, 5.87586, 47.5395, 15.2802); // TODO: remove
}

void MainWindow::networkSessionError(QNetworkSession::SessionError error){
    if (error == QNetworkSession::UnknownSessionError) {
        if (m_session->state() == QNetworkSession::Connecting) {
            QMessageBox msgBox(qobject_cast<QWidget *>(parent()));
            msgBox.setText("This application requires network access to function.");
            msgBox.setInformativeText("Press Cancel to quit the application.");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Ignore | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Retry);

            switch (msgBox.exec()) {
            case QMessageBox::Retry:
                QTimer::singleShot(0, m_session, SLOT(open()));
                break;

            case QMessageBox::Ignore:
                break;

            case QMessageBox::Cancel:
                close();
                break;
            }
        }
    } else if (error == QNetworkSession::SessionAbortedError) {
        QMessageBox msgBox(qobject_cast<QWidget *>(parent()));
        msgBox.setText("Out of range of network.");
        msgBox.setInformativeText("Move back into range and press Retry, or press Cancel to quit the application.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Retry);

        switch (msgBox.exec()) {
        case QMessageBox::Retry:
            QTimer::singleShot(0, m_session, SLOT(open()));
            break;

        case QMessageBox::Ignore:
            break;

        case QMessageBox::Cancel:
            close();
            break;
        }
    }

}
