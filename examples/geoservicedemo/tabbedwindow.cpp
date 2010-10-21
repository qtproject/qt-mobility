/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tabbedwindow.h"
#include "servicestab.h"

#include <QApplication>
#include <QTabWidget>
#include <QAction>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QTimer>
#include <QMessageBox>
#include <QNetworkProxyFactory>
#include <qnetworkconfigmanager.h>

TabbedWindow::TabbedWindow(QWidget *parent)
        : QMainWindow(parent), m_serviceProvider(NULL)
{
    setWindowTitle(tr("Geo Service Demo"));

    m_servicesTab = new ServicesTab();

    QObject::connect(m_servicesTab, SIGNAL(serviceProviderChanged(QString)), this,
                     SLOT(setProvider(QString)), Qt::QueuedConnection);

    m_geocodingTab = new GeocodingTab();
    m_reverseTab = new ReverseGeocodingTab();
    m_routingTab = new RouteTab();

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(m_servicesTab, tr("Service Providers"));
    m_tabWidget->addTab(m_routingTab, tr("Route"));
    m_tabWidget->addTab(m_geocodingTab, tr("Geocoding"));
    m_tabWidget->addTab(m_reverseTab, tr("Reverse Geocoding"));

    setCentralWidget(m_tabWidget);

    // setup exit menu for devices

#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    QAction* exitAction = new QAction(tr("Exit"), this);
    menuBar()->addAction(exitAction);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
#endif

    // Set Internet Access Point
    QNetworkConfigurationManager manager;
    const bool canStartIAP = (manager.capabilities()
                              & QNetworkConfigurationManager::CanStartAndStopInterfaces);
    // Is there default access point, use it
    QNetworkConfiguration cfg = manager.defaultConfiguration();
    if (!cfg.isValid() || (!canStartIAP && cfg.state() != QNetworkConfiguration::Active)) {
        QMessageBox::information(this, tr("Geo Service Demo"), tr(
                                     "Available Access Points not found."));
        return;
    }

    m_session = new QNetworkSession(cfg, this);
    connect(m_session, SIGNAL(opened()), this, SLOT(networkSessionOpened()));
    connect(m_session,
            SIGNAL(error(QNetworkSession::SessionError)),
            this,
            SLOT(error(QNetworkSession::SessionError)));

    m_session->open();
    resize(640, 480);
}

TabbedWindow::~TabbedWindow()
{
    delete m_serviceProvider;
}

void TabbedWindow::networkSessionOpened()
{
    QNetworkProxyFactory::setUseSystemConfiguration(true);
    QTimer::singleShot(0, m_servicesTab, SLOT(initialize()));
}

void TabbedWindow::error(QNetworkSession::SessionError error)
{
    if (error == QNetworkSession::UnknownSessionError) {
        if (m_session->state() == QNetworkSession::Connecting) {
            QMessageBox msgBox(qobject_cast<QWidget *>(parent()));
            msgBox.setText("This application requires network access to function.");
            msgBox.setInformativeText("Press Cancel to quit the application.");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Retry);
            int ret = msgBox.exec();
            if (ret == QMessageBox::Retry) {
                QTimer::singleShot(0, m_session, SLOT(open()));
            } else if (ret == QMessageBox::Cancel) {
                close();
            }
        }
    } else if (error == QNetworkSession::SessionAbortedError) {
        QMessageBox msgBox(qobject_cast<QWidget *>(parent()));
        msgBox.setText("Out of range of network.");
        msgBox.setInformativeText("Move back into range and press Retry, or press Cancel to quit the application.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Retry);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Retry) {
            QTimer::singleShot(0, m_session, SLOT(open()));
        } else if (ret == QMessageBox::Cancel) {
            close();
        }
    }
}

void TabbedWindow::setProvider(QString providerId)
{
    if (m_serviceProvider)
        delete m_serviceProvider ;
    m_serviceProvider = new QGeoServiceProvider(providerId);
    if (m_serviceProvider->error() != QGeoServiceProvider::NoError) {
        QMessageBox::information(this, tr("Geo Service Demo"), tr(
                                     "Unable to find the %1 geoservices plugin.").arg(providerId));
        QTimer::singleShot(0, qApp, SLOT(quit()));
        return;
    }

    m_geocodingTab->initialize(m_serviceProvider->searchManager());
    m_reverseTab->initialize(m_serviceProvider->searchManager());
    m_routingTab->initialize(m_serviceProvider->routingManager());
}

