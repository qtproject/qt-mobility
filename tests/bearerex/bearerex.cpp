/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "bearerex.h"

#include <QtNetwork>

Q_DECLARE_METATYPE(QNetworkConfiguration)

BearerEx::BearerEx(QWidget* parent)
     : QMainWindow(parent), m_monitor(false)
{
    setupUi(this);
    
    createMenus();
    
    connect(&m_NetworkConfigurationManager, SIGNAL(updateCompleted()), this, SLOT(configurationsUpdateCompleted()));
    connect(&m_NetworkConfigurationManager, SIGNAL(configurationAdded(const QNetworkConfiguration&)),
            this, SLOT(configurationAdded(const QNetworkConfiguration&)));
    connect(&m_NetworkConfigurationManager, SIGNAL(configurationRemoved(const QNetworkConfiguration&)),
            this, SLOT(configurationRemoved(const QNetworkConfiguration&)));
    connect(&m_NetworkConfigurationManager, SIGNAL(onlineStateChanged(bool)),
            this, SLOT(onlineStateChanged(bool)));
    connect(&m_NetworkConfigurationManager, SIGNAL(configurationChanged(const QNetworkConfiguration&)),
            this, SLOT(configurationChanged(const QNetworkConfiguration&)));
    showConfigurations();
}

void BearerEx::createMenus()
{
    QAction* act1 = new QAction(tr("Show Details"), this);
    menuBar()->addAction(act1);
    connect(act1, SIGNAL(triggered()), this, SLOT(on_showDetailsButton_clicked()));

    m_openAction = new QAction(tr("Open Session"), this);
    menuBar()->addAction(m_openAction);
    connect(m_openAction, SIGNAL(triggered()), this, SLOT(on_openSessionButton_clicked()));
    
    QAction* exitAct = new QAction(tr("Exit"), this);
    menuBar()->addAction(exitAct);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void BearerEx::showConfigurations()
{
    listWidget->clear();
    QListWidgetItem* listItem;
    
    QNetworkConfiguration defaultConfig = m_NetworkConfigurationManager.defaultConfiguration();
    
    QList<QNetworkConfiguration> configurations = m_NetworkConfigurationManager.allConfigurations();
    for (int i=0; i<configurations.count(); i++)
    {
        listItem = new QListWidgetItem();
        QString text;
        if (configurations[i].type() == QNetworkConfiguration::InternetAccessPoint) {
            text.append("(IAP,");
        } else if (configurations[i].type() == QNetworkConfiguration::ServiceNetwork) {
            text.append("(SNAP,");
        }
        
        if ((configurations[i].state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            text.append("Act) ");
        } else if ((configurations[i].state() & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered) {
            text.append("Disc) ");
        } else {
            text.append("Def) ");
        }
        text.append(configurations[i].name());
        
        if (defaultConfig.isValid() && defaultConfig == configurations[i]) {
            QFont font = listItem->font();
            font.setBold(true);
            font.setUnderline(true);
            listItem->setFont(font);        
        }
        listItem->setText(text);
        listItem->setData(Qt::UserRole, qVariantFromValue(configurations[i]));
        listWidget->addItem(listItem);
    }
}

void BearerEx::on_updateConfigurationsButton_clicked()
{
    m_NetworkConfigurationManager.updateConfigurations();
}

void BearerEx::on_updateListButton_clicked()
{
    showConfigurations();
}

void BearerEx::on_showDetailsButton_clicked()
{
    QListWidgetItem* item = listWidget->currentItem();
    if (!item) {
        return;
    }

	QNetworkConfiguration networkConfiguration = qVariantValue<QNetworkConfiguration>(item->data(Qt::UserRole));
	DetailedInfoDialog infoDialog(&networkConfiguration,this);
	infoDialog.exec();
}

void BearerEx::on_createSessionButton_clicked()
{
    QListWidgetItem* item = listWidget->currentItem();
    if (!item) {
        return;
    }    
    QNetworkConfiguration networkConfiguration = qVariantValue<QNetworkConfiguration>(item->data(Qt::UserRole));
    SessionDialog sessionDialog(&networkConfiguration,&m_NetworkConfigurationManager,this);
    sessionDialog.exec();
}

void BearerEx::on_monitorConfigurationsButton_clicked()
{
    if (m_monitor) {
        monitorConfigurationsButton->setText("Start Monitoring");
        m_monitor = false;
    } else {
        monitorConfigurationsButton->setText("Stop Monitoring");
        m_monitor = true;
    }
}


void BearerEx::configurationAdded(const QNetworkConfiguration& config)
{
    if (m_monitor) {
        QMessageBox msgBox;
        msgBox.setText(QString("New configuration added :\n")+config.name());
        msgBox.exec();
    }
}

void BearerEx::configurationRemoved(const QNetworkConfiguration& config)
{
    if (m_monitor) {
        QMessageBox msgBox;
        msgBox.setText(QString("Configuration removed :\n")+config.name());
        msgBox.exec();
    }
}

void BearerEx::onlineStateChanged(bool isOnline)
{
    if (m_monitor) {
        QMessageBox msgBox;
        if (isOnline) {
            msgBox.setText("Device Online");
        } else {
            msgBox.setText("Device Offline");
        }
        msgBox.exec();
    }
}

void BearerEx::configurationChanged(const QNetworkConfiguration & config)
{
    if (m_monitor) {
        QString state;
        switch (config.state())
        {
            case QNetworkConfiguration::Undefined:
                state = "Undefined";
                break;
            case QNetworkConfiguration::Defined:
                state = "Defined";
                break;
            case QNetworkConfiguration::Discovered:
                state = "Discovered";
                break;
            case QNetworkConfiguration::Active:
                state = "Active";
                break;
        }
    
        QMessageBox msgBox;
        msgBox.setText(QString("Configuration state changed :\n")+config.name()+QString("\n")+state);
        msgBox.exec();
    }
}

void BearerEx::configurationsUpdateCompleted()
{
    QMessageBox msgBox;
    msgBox.setText("Configurations update completed.");
    msgBox.exec();
}

DetailedInfoDialog::DetailedInfoDialog(QNetworkConfiguration* apNetworkConfiguration, QWidget * parent)
    : QDialog(parent)
{
    setupUi(this);

    tableWidget->setColumnCount(2);
    int rowCount = 2;
    
    if (apNetworkConfiguration->type() == QNetworkConfiguration::ServiceNetwork) {
        rowCount = rowCount + apNetworkConfiguration->children().count();
    }

	tableWidget->setRowCount(rowCount);
	tableWidget->setColumnWidth(1,250);
	tableWidget->setItem(0, 0, new QTableWidgetItem(tr("Name")));
	tableWidget->setItem(0, 1, new QTableWidgetItem(apNetworkConfiguration->name()));
	tableWidget->setItem(1, 0, new QTableWidgetItem(tr("Id")));
	tableWidget->setItem(1, 1, new QTableWidgetItem(apNetworkConfiguration->identifier()));
    if (apNetworkConfiguration->type() == QNetworkConfiguration::ServiceNetwork) {
        for (int i=0; i<apNetworkConfiguration->children().count(); i++) {
            tableWidget->setItem(i+2, 0, new QTableWidgetItem(QString("IAP")+QString::number(i+1)));
            tableWidget->setItem(i+2, 1, new QTableWidgetItem(apNetworkConfiguration->children()[i].name()));
        }
    }
    
    tableWidget->setFocusPolicy(Qt::NoFocus);

#ifdef Q_OS_SYMBIAN
    this->showMaximized();
#endif
}

SessionDialog::SessionDialog(QNetworkConfiguration* apNetworkConfiguration,
                             QNetworkConfigurationManager* configManager, 
                             QWidget * parent)
    : QDialog(parent), m_http(0), m_httpRequestOngoing(false)
{
    setupUi(this);
    
    m_ConfigManager = configManager;

    m_alrEnabled = false;
    m_NetworkSession = new QNetworkSession(*apNetworkConfiguration);
    stateChanged(m_NetworkSession->state());
    
    connect(m_NetworkSession, SIGNAL(newConfigurationActivated()), this, SLOT(newConfigurationActivated()));
    connect(m_NetworkSession, SIGNAL(stateChanged(QNetworkSession::State)),
            this, SLOT(stateChanged(QNetworkSession::State)));
    connect(m_NetworkSession, SIGNAL(sessionOpened()), this, SLOT(sessionOpened()));
    connect(m_NetworkSession, SIGNAL(sessionClosed()), this, SLOT(sessionClosed()));
    connect(m_NetworkSession, SIGNAL(error(QNetworkSession::SessionError)), this, SLOT(error(QNetworkSession::SessionError)));
    
    if (apNetworkConfiguration->type() == QNetworkConfiguration::InternetAccessPoint) {
        snapLabel->hide();
        snapLineEdit->hide();
        alrButton->hide();
        iapLineEdit->setText(apNetworkConfiguration->name()+" ("+apNetworkConfiguration->identifier()+")");
    } else if (apNetworkConfiguration->type() == QNetworkConfiguration::ServiceNetwork) {
        snapLineEdit->setText(apNetworkConfiguration->name()+" ("+apNetworkConfiguration->identifier()+")");
    }
    bearerLineEdit->setText(m_NetworkSession->bearerName());
    sentRecDataLineEdit->setText(QString::number(m_NetworkSession->sentData())+
                                 QString(" / ")+
                                 QString::number(m_NetworkSession->receivedData()));
    snapLineEdit->setFocusPolicy(Qt::NoFocus);
    iapLineEdit->setFocusPolicy(Qt::NoFocus);
    bearerLineEdit->setFocusPolicy(Qt::NoFocus);
    sentRecDataLineEdit->setFocusPolicy(Qt::NoFocus);
    stateLineEdit->setFocusPolicy(Qt::NoFocus);
    
#ifdef Q_OS_SYMBIAN
    this->showMaximized();
#endif
}

SessionDialog::~SessionDialog()
{
    delete m_NetworkSession;
    delete m_http;
}

void SessionDialog::on_createQHttpButton_clicked()
{
    if (m_httpRequestOngoing) {
        return;
    }

    if (m_http) {
        disconnect(m_http, 0, 0, 0);
        delete m_http;
    }
    m_http = new QHttp(this);
    createQHttpButton->setText("Recreate QHttp");
    connect(m_http, SIGNAL(done(bool)), this, SLOT(done(bool)));    
}

void SessionDialog::on_sendRequestButton_clicked()
{
    if (m_http) {
        QString urlstring("http://www.google.com");
        QUrl url(urlstring);
        m_http->setHost(url.host(), QHttp::ConnectionModeHttp, url.port() == -1 ? 0 : url.port());
        m_http->get(urlstring);
        m_httpRequestOngoing = true;
    } else {
        QMessageBox msgBox;
        msgBox.setText("QHttp not created.\nCreate QHttp First.");
        msgBox.exec();
    }
}

void SessionDialog::on_openSessionButton_clicked()
{
    m_NetworkSession->open();
    if (m_NetworkSession->isActive()) {
        stateChanged(m_NetworkSession->state()); 
    }
}

void SessionDialog::on_closeSessionButton_clicked()
{
    m_NetworkSession->close();
    if (!m_NetworkSession->isActive()) {
        stateChanged(m_NetworkSession->state()); 
    }
}

void SessionDialog::on_stopConnectionButton_clicked()
{
    m_NetworkSession->stop();
}

void SessionDialog::on_alrButton_clicked()
{
    if (!m_alrEnabled) {
        connect(m_NetworkSession, SIGNAL(preferredConfigurationChanged(const QNetworkConfiguration&, bool)),
                this, SLOT(preferredConfigurationChanged(const QNetworkConfiguration&, bool)));
        alrButton->setText("Disable ALR");
        m_alrEnabled = true;
    } else {
        disconnect(m_NetworkSession, SIGNAL(preferredConfigurationChanged(const QNetworkConfiguration&, bool)), 0, 0);
        alrButton->setText("Enable ALR");
        m_alrEnabled = false;
    }
}

void SessionDialog::on_deleteSessionButton_clicked()
{
    setWindowTitle("Bearer Example");
    accept();
}

void SessionDialog::newConfigurationActivated()
{
    QMessageBox msgBox;
    msgBox.setText("New configuration activated.");
    msgBox.setInformativeText("Do you want to accept new configuration?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if (msgBox.exec() == QMessageBox::Yes) {
        m_NetworkSession->accept();
        iapLineEdit->setText(m_config.name()+" ("+m_config.identifier()+")");
    } else {
        m_NetworkSession->reject();
    }
}

void SessionDialog::preferredConfigurationChanged(const QNetworkConfiguration& config, bool /*isSeamless*/)
{
    m_config =  config;
    QMessageBox msgBox;
    msgBox.setText("Roaming to new configuration.");
    msgBox.setInformativeText("Do you want to migrate to "+config.name()+"?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    if (msgBox.exec() == QMessageBox::Yes) {
        m_NetworkSession->migrate();
    } else {
        m_NetworkSession->ignore();
    }
}

void SessionDialog::sessionOpened()
{
    QVariant identifier = m_NetworkSession->property("ActiveConfigurationIdentifier");
    if (!identifier.isNull()) {
        QString configId = identifier.toString();
        QNetworkConfiguration config = m_ConfigManager->configurationFromIdentifier(configId);
        if (config.isValid()) {
            iapLineEdit->setText(config.name()+" ("+config.identifier()+")");
        }
    }

    QMessageBox msgBox;
    msgBox.setText("Session opened.");
    msgBox.exec();
}

void SessionDialog::sessionClosed()
{
    QMessageBox msgBox;
    msgBox.setText("Session closed.");
    msgBox.exec();
}

void SessionDialog::stateChanged(QNetworkSession::State state)    
{
    QString active;
    if (m_NetworkSession->isActive()) {
        active = " (A)";
    }

    switch (state)
    {
        case QNetworkSession::Invalid:
            stateLineEdit->setText(QString("Invalid")+active);
            break;
        case QNetworkSession::NotAvailable:
            stateLineEdit->setText(QString("NotAvailable")+active);
            break;
        case QNetworkSession::Connecting:
            stateLineEdit->setText(QString("Connecting")+active);
            break;
        case QNetworkSession::Connected:
        {
            stateLineEdit->setText(QString("Connected")+active);
            QVariant identifier = m_NetworkSession->property("ActiveConfigurationIdentifier");
            if (!identifier.isNull()) {
                QString configId = identifier.toString();
                QNetworkConfiguration config = m_ConfigManager->configurationFromIdentifier(configId);
                if (config.isValid()) {
                    iapLineEdit->setText(config.name()+" ("+config.identifier()+")");
                }
            }
            break;
        }                
        case QNetworkSession::Closing:
            stateLineEdit->setText(QString("Closing")+active);
            break;
        case QNetworkSession::Disconnected:
            stateLineEdit->setText(QString("Disconnected")+active);
            break;
        case QNetworkSession::Roaming:
            stateLineEdit->setText(QString("Roaming")+active);
            break;
    }

    bearerLineEdit->setText(m_NetworkSession->bearerName());
}

void SessionDialog::error(QNetworkSession::SessionError error)
{
    QMessageBox msgBox;
    switch (error)
    {
        case QNetworkSession::UnknownSessionError:
            msgBox.setText("UnknownSessionError");
            break;
        case QNetworkSession::SessionAbortedError:
            msgBox.setText("SessionAbortedError");
            break;
        case QNetworkSession::RoamingError:
            msgBox.setText("RoamingError");
            break;
        case QNetworkSession::OperationNotSupportedError:
            msgBox.setText("OperationNotSupportedError");
            break;
    }
    msgBox.exec();
}

void SessionDialog::done(bool error)
{
    m_httpRequestOngoing = false;

    QMessageBox msgBox;
    if (error) {
        msgBox.setText("HTTP request failed.");
    } else {
        QString result(m_http->readAll());
        msgBox.setText(QString("HTTP request finished successfully.\nReceived ")+QString::number(result.length())+QString(" bytes."));
    }
    msgBox.exec();
    
    sentRecDataLineEdit->setText(QString::number(m_NetworkSession->sentData())+
                                 QString(" / ")+
                                 QString::number(m_NetworkSession->receivedData()));
}

// End of file

