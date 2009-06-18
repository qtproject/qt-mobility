/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "bearerdialog.h"
#include "sessionwidget.h"

#include <QDebug>

#ifdef Q_OS_WIN
#include <winsock2.h>
#undef interface
#endif

BearerDialog::BearerDialog(QWidget *parent)
:   QDialog(parent)
{
    setupUi(this);
    delete tabWidget->currentWidget();
    sessionGroup->hide();

    updateConfigurations();

    onlineStateChanged(!manager.allConfigurations(QNetworkConfiguration::Active).isEmpty());

    QNetworkConfiguration defaultConfiguration = manager.defaultConfiguration();
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);

        if (item->data(0, Qt::UserRole).toString() == defaultConfiguration.identifier()) {
            treeWidget->setCurrentItem(item);
            showConfigurationFor(item);
            break;
        }
    }

    connect(&manager, SIGNAL(configurationAdded(const QNetworkConfiguration&)),
            this, SLOT(configurationAdded(const QNetworkConfiguration&)));
    connect(&manager, SIGNAL(configurationRemoved(const QNetworkConfiguration&)),
            this, SLOT(configurationRemoved(const QNetworkConfiguration&)));
    connect(&manager, SIGNAL(configurationChanged(const QNetworkConfiguration&)),
            this, SLOT(configurationChanged(const QNetworkConfiguration)));
    connect(&manager, SIGNAL(updateCompleted()), this, SLOT(updateConfigurations()));
    connect(&manager, SIGNAL(onlineStateChanged(bool)), this ,SLOT(onlineStateChanged(bool)));

#ifdef Q_OS_WIN
    connect(registerButton, SIGNAL(clicked()), this, SLOT(registerNetwork()));
    connect(unregisterButton, SIGNAL(clicked()), this, SLOT(unregisterNetwork()));
#else
    nlaGroup->hide();
#endif

    connect(treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
            this, SLOT(createSessionFor(QTreeWidgetItem*)));

    connect(treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(showConfigurationFor(QTreeWidgetItem*)));

    connect(newSessionButton, SIGNAL(clicked()),
            this, SLOT(createNewSession()));
    connect(deleteSessionButton, SIGNAL(clicked()),
            this, SLOT(deleteSession()));

    connect(scanButton, SIGNAL(clicked()),
            this, SLOT(performScan()));
}

BearerDialog::~BearerDialog()
{
}

static void updateItem(QTreeWidgetItem *item, const QNetworkConfiguration &config)
{
    item->setText(0, config.name());
    item->setData(0, Qt::UserRole, config.identifier());

    QFont font = item->font(1);
    font.setBold((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active);
    item->setFont(0, font);
}

void BearerDialog::configurationAdded(const QNetworkConfiguration &config, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = new QTreeWidgetItem;
    updateItem(item, config);

    if (parent)
        parent->addChild(item);
    else
        treeWidget->addTopLevelItem(item);

    if (config.type() == QNetworkConfiguration::ServiceNetwork) {
        foreach (const QNetworkConfiguration &child, config.children())
            configurationAdded(child, item);
    }
}

void BearerDialog::configurationAdded(const QNetworkConfiguration &config)
{
    configurationAdded(config, 0);
}

void BearerDialog::configurationRemoved(const QNetworkConfiguration &config)
{
    QList<QTreeWidgetItem *> items = treeWidget->findItems(config.name(), Qt::MatchExactly);

    while (!items.isEmpty())
        delete treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(items.takeFirst()));
}

void BearerDialog::configurationChanged(const QNetworkConfiguration &config)
{
    QList<QTreeWidgetItem *> items = treeWidget->findItems(config.name(), Qt::MatchExactly);

    while (!items.isEmpty()) {
        QTreeWidgetItem *item = items.takeFirst();

        if (item) {
            updateItem(item, config);

            if (config.type() == QNetworkConfiguration::ServiceNetwork)
                updateSnapConfiguration(item, config);

            if (item == treeWidget->currentItem())
                showConfigurationFor(item);
        }
    }
}

void BearerDialog::updateSnapConfiguration(QTreeWidgetItem *parent, const QNetworkConfiguration &snap)
{
    QMap<QString, QTreeWidgetItem *> itemMap;
    for (int i = 0; i < parent->childCount(); ++i) {
        QTreeWidgetItem *item = parent->child(i);
        itemMap.insert(item->data(0, Qt::UserRole).toString(), item);
    }

    QList<QNetworkConfiguration> allConfigurations = snap.children();

    while (!allConfigurations.isEmpty()) {
        QNetworkConfiguration config = allConfigurations.takeFirst();

        QTreeWidgetItem *item = itemMap.take(config.identifier());
        if (item) {
            updateItem(item, config);

            if (config.type() == QNetworkConfiguration::ServiceNetwork)
                updateSnapConfiguration(item, config);
        } else {
            configurationAdded(config, parent);
        }
    }

    foreach (const QString &id, itemMap.keys())
        delete itemMap.value(id);

    itemMap.clear();
}

void BearerDialog::updateConfigurations()
{
    progressBar->hide();
    scanButton->show();

    QList<QTreeWidgetItem *> items = treeWidget->findItems(QLatin1String("*"), Qt::MatchWildcard);
    QMap<QString, QTreeWidgetItem *> itemMap;
    while (!items.isEmpty()) {
        QTreeWidgetItem *item = items.takeFirst();
        itemMap.insert(item->data(0, Qt::UserRole).toString(), item);
    }

    QList<QNetworkConfiguration> allConfigurations = manager.allConfigurations();

    while (!allConfigurations.isEmpty()) {
        QNetworkConfiguration config = allConfigurations.takeFirst();

        QTreeWidgetItem *item = itemMap.take(config.identifier());
        if (item) {
            updateItem(item, config);

            if (config.type() == QNetworkConfiguration::ServiceNetwork)
                updateSnapConfiguration(item, config);
        } else {
            configurationAdded(config);
        }
    }

    foreach (const QString &id, itemMap.keys())
        delete itemMap.value(id);
}

void BearerDialog::onlineStateChanged(bool isOnline)
{
    if (isOnline)
        onlineState->setText(tr("Online"));
    else
        onlineState->setText(tr("Offline"));
}

#ifdef Q_OS_WIN
void BearerDialog::registerNetwork()
{
    QTreeWidgetItem *item = treeWidget->currentItem();

    const QString identifier = item->data(0, Qt::UserRole).toString();

    QNetworkConfiguration configuration = manager.configurationFromIdentifier(identifier);

    qDebug() << "Registering" << configuration.name() << "with system";

    WSAQUERYSET networkInfo;
    memset(&networkInfo, 0, sizeof(networkInfo));
    networkInfo.dwSize = sizeof(networkInfo);
    networkInfo.lpszServiceInstanceName = (LPWSTR)identifier.utf16();
    networkInfo.dwNameSpace = NS_NLA;

    if (WSASetService(&networkInfo, RNRSERVICE_REGISTER, 0) == SOCKET_ERROR)
        qDebug() << "WSASetService(RNRSERVICE_REGISTER) returned" << WSAGetLastError();
}

void BearerDialog::unregisterNetwork()
{
    QTreeWidgetItem *item = treeWidget->currentItem();

    const QString identifier = item->data(0, Qt::UserRole).toString();

    QNetworkConfiguration configuration = manager.configurationFromIdentifier(identifier);

    qDebug() << "Unregistering" << configuration.name() << "with system";

    WSAQUERYSET networkInfo;
    memset(&networkInfo, 0, sizeof(networkInfo));
    networkInfo.dwSize = sizeof(networkInfo);
    networkInfo.lpszServiceInstanceName = (LPWSTR)identifier.utf16();
    networkInfo.dwNameSpace = NS_NLA;

    if (WSASetService(&networkInfo, RNRSERVICE_DELETE, 0) == SOCKET_ERROR)
        qDebug() << "WSASetService(RNRSERVICE_DELETE) returned" << WSAGetLastError();
}
#endif

void BearerDialog::showConfigurationFor(QTreeWidgetItem *item)
{
    QString identifier;

    if (item)
        identifier = item->data(0, Qt::UserRole).toString();

    QNetworkConfiguration conf = manager.configurationFromIdentifier(identifier);

    switch (conf.state()) {
    case QNetworkConfiguration::Active:
        configurationState->setText(tr("Active"));
        break;
    case QNetworkConfiguration::Discovered:
        configurationState->setText(tr("Discovered"));
        break;
    case QNetworkConfiguration::Defined:
        configurationState->setText(tr("Defined"));
        break;
    case QNetworkConfiguration::Undefined:
        configurationState->setText(tr("Undefined"));
        break;
    default:
        configurationState->setText(QString());
    }

    switch (conf.type()) {
    case QNetworkConfiguration::InternetAccessPoint:
        configurationType->setText(tr("Internet Access Point"));
        break;
    case QNetworkConfiguration::ServiceNetwork:
        configurationType->setText(tr("Service Network"));
        break;
    case QNetworkConfiguration::Invalid:
        configurationType->setText(tr("Invalid"));
        break;
    default:
        configurationType->setText(QString());
    }

    switch (conf.purpose()) {
    case QNetworkConfiguration::Unknown:
        configurationPurpose->setText(tr("Unknown"));
        break;
    case QNetworkConfiguration::Public:
        configurationPurpose->setText(tr("Public"));
        break;
    case QNetworkConfiguration::Private:
        configurationPurpose->setText(tr("Private"));
        break;
    case QNetworkConfiguration::ServiceSpecific:
        configurationPurpose->setText(tr("Service Specific"));
        break;
    default:
        configurationPurpose->setText(QString());
    }

    configurationIdentifier->setText(conf.identifier());

    configurationRoaming->setText(conf.roamingAvailable() ? tr("Available") : tr("Not available"));

    configurationChildren->setText(QString::number(conf.children().count()));

    configurationName->setText(conf.name());
}

void BearerDialog::createSessionFor(QTreeWidgetItem *item)
{
    const QString identifier = item->data(0, Qt::UserRole).toString();

    QNetworkConfiguration conf = manager.configurationFromIdentifier(identifier);

    SessionWidget *session = new SessionWidget(conf);

    tabWidget->addTab(session, conf.name());

    sessionGroup->show();

    sessionWidgets.append(session);
}

void BearerDialog::createNewSession()
{
    QTreeWidgetItem *item = treeWidget->currentItem();

    createSessionFor(item);
}

void BearerDialog::deleteSession()
{
    SessionWidget *session = qobject_cast<SessionWidget *>(tabWidget->currentWidget());
    if (session) {
        sessionWidgets.removeAll(session);

        delete session;

        if (tabWidget->count() == 0)
            sessionGroup->hide();
    }
}

void BearerDialog::performScan()
{
    scanButton->hide();
    progressBar->show();
    manager.updateConfigurations();
}

