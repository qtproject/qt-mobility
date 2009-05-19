/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include <qservicemanager.h>
#include <qserviceinterfacedescriptor.h>

#include "mainwindow.h"

Q_DECLARE_METATYPE(QServiceInterfaceDescriptor)

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    serviceManager = new QServiceManager(this);

    QStringList exampleXmlFiles;
    exampleXmlFiles << "filemanagerservice.xml" << "bluetoothtransferservice.xml";
    foreach (const QString &fileName, exampleXmlFiles) {
        QString path = QCoreApplication::applicationDirPath() + "/xmldata/" + fileName;
        serviceManager->addService(path);
    }

    initWidgets();
    reloadServicesList();
}

MainWindow::~MainWindow()
{
/*
    qDebug() << "------------ Removing services ----------";
    serviceManager->removeService("FileManagerService");
    serviceManager->removeService("BluetoothTransferService");
*/
}

void MainWindow::currentServiceChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
    reloadInterfaceImplementationsList();
}

void MainWindow::currentInterfaceImplChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (!current)
        return;

    reloadAttributesList();

    QServiceInterfaceDescriptor descriptor = current->data(Qt::UserRole).value<QServiceInterfaceDescriptor>();
    if (descriptor.isValid()) {
        defaultInterfaceButton->setText(tr("Set as default implementation for %1") 
                .arg(descriptor.interfaceName()));
        defaultInterfaceButton->setEnabled(true);
    }
}

void MainWindow::clickedSetDefaultImpl()
{
    QListWidgetItem *item = interfacesListWidget->currentItem();
    if (!item)
        return;

    QServiceInterfaceDescriptor descriptor = item->data(Qt::UserRole).value<QServiceInterfaceDescriptor>();
    if (descriptor.isValid()) {
        if (serviceManager->setDefaultServiceForInterface(descriptor)) {
            int currentIndex = interfacesListWidget->row(item);
            reloadInterfaceImplementationsList();
            interfacesListWidget->setCurrentRow(currentIndex);
        } else {
            qWarning() << "Unable to set default service for interface:" 
                    << descriptor.interfaceName();
        }
    }
}

void MainWindow::clickedAttributesRadioButton(QAbstractButton *button)
{
    Q_UNUSED(button);
    reloadAttributesList();
}

void MainWindow::reloadServicesList()
{
    servicesListWidget->clear();

    QStringList services = serviceManager->findServices();
    for (int i=0; i<services.count(); i++)
        servicesListWidget->addItem(services[i]);

    servicesListWidget->addItem(showAllServicesItem);
}

void MainWindow::reloadInterfaceImplementationsList()
{
    QString serviceName;
    if (servicesListWidget->currentItem()
            && servicesListWidget->currentItem() != showAllServicesItem) {
        serviceName = servicesListWidget->currentItem()->text();
        interfacesGroup->setTitle(tr("Interfaces implemented by %1").arg(serviceName));
    } else {
        interfacesGroup->setTitle(tr("All interface implementations"));
    }

    interfacesListWidget->clear();

    QList<QServiceInterfaceDescriptor> descriptors = serviceManager->findInterfaces(serviceName);

    for (int i=0; i<descriptors.count(); i++) {
        QString text = QString("%1 %2.%3")
                .arg(descriptors[i].interfaceName())
                .arg(descriptors[i].majorVersion())
                .arg(descriptors[i].minorVersion());

        if (serviceName.isEmpty())
            text += " (" + descriptors[i].serviceName() + ")";

        QServiceInterfaceDescriptor defaultInterfaceImpl = 
                serviceManager->defaultServiceInterface(descriptors[i].interfaceName());
        if (descriptors[i].serviceName() == defaultInterfaceImpl.serviceName())
            text += tr(" (default)");

        QListWidgetItem *item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, qVariantFromValue(descriptors[i]));
        interfacesListWidget->addItem(item);
    }

    defaultInterfaceButton->setEnabled(false);
}

void MainWindow::reloadAttributesList()
{
    QListWidgetItem *item = interfacesListWidget->currentItem();
    if (!item)
        return;

    QServiceInterfaceDescriptor selectedImpl =
            item->data(Qt::UserRole).value<QServiceInterfaceDescriptor>();
    QServiceInterfaceDescriptor defaultImpl = 
            serviceManager->defaultServiceInterface(selectedImpl.interfaceName());

    defaultImplRadioButton->setText(tr("Default implementation for %1\n(currently provided by %2)")
            .arg(defaultImpl.interfaceName())
            .arg(defaultImpl.serviceName()));

    QServiceInterfaceDescriptor descriptor;
    if (selectedImplRadioButton->isChecked())
        descriptor = selectedImpl;
    else
        descriptor = defaultImpl;

    QObject *interfaceImpl = serviceManager->loadInterface(descriptor, 0, 0);

    attributesListWidget->clear();
    if (interfaceImpl) {
        attributesGroup->setTitle(tr("Invokable attributes for %1 class")
                .arg(QString(interfaceImpl->metaObject()->className())));
        for (int i=0; i<interfaceImpl->metaObject()->methodCount(); i++) {
            QMetaMethod method = interfaceImpl->metaObject()->method(i);
            attributesListWidget->addItem("[METHOD] " + QString(method.signature()));
        }
        for (int i=0; i<interfaceImpl->metaObject()->propertyCount(); i++) {
            QMetaProperty property = interfaceImpl->metaObject()->property(i);
            attributesListWidget->addItem("[PROPERTY] " + QString(property.name()));
        }
    } else {
        attributesListWidget->addItem(tr("(Error loading service plugin)"));
    }
}

void MainWindow::initWidgets()
{
    showAllServicesItem = new QListWidgetItem(tr("(All registered services)"));

    servicesListWidget = new QListWidget;
    interfacesListWidget = new QListWidget;
    interfacesListWidget->addItem(tr("(Select a service)"));
    attributesListWidget = new QListWidget;
    attributesListWidget->addItem(tr("(Select an interface implementation)"));

    interfacesListWidget->setMinimumWidth(450);

    connect(servicesListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(currentServiceChanged(QListWidgetItem*,QListWidgetItem*)));

    connect(interfacesListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(currentInterfaceImplChanged(QListWidgetItem*,QListWidgetItem*)));

    defaultInterfaceButton = new QPushButton(tr("Set as default implementation"));
    defaultInterfaceButton->setEnabled(false);
    connect(defaultInterfaceButton, SIGNAL(clicked()),
            this, SLOT(clickedSetDefaultImpl()));

    selectedImplRadioButton = new QRadioButton(tr("Selected interface implementation"));
    defaultImplRadioButton = new QRadioButton(tr("Default implementation"));
    selectedImplRadioButton->setChecked(true);

    QButtonGroup *radioButtons = new QButtonGroup(this);
    radioButtons->addButton(selectedImplRadioButton);
    radioButtons->addButton(defaultImplRadioButton);
    connect(radioButtons, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(clickedAttributesRadioButton(QAbstractButton*)));

    QGroupBox *servicesGroup = new QGroupBox(tr("Show services for:"));
    QVBoxLayout *servicesLayout = new QVBoxLayout;
    servicesLayout->addWidget(servicesListWidget);
    servicesGroup->setLayout(servicesLayout);

    interfacesGroup = new QGroupBox(tr("Interface implementations"));
    QVBoxLayout *interfacesLayout = new QVBoxLayout;
    interfacesLayout->addWidget(interfacesListWidget);
    interfacesLayout->addWidget(defaultInterfaceButton);
    interfacesGroup->setLayout(interfacesLayout);

    attributesGroup = new QGroupBox(tr("Invokable attributes"));
    QVBoxLayout *attributesLayout = new QVBoxLayout;
    attributesLayout->addWidget(attributesListWidget);
    attributesLayout->addWidget(new QLabel(tr("Show attributes for:")));
    attributesLayout->addWidget(selectedImplRadioButton);
    attributesLayout->addWidget(defaultImplRadioButton);
    attributesGroup->setLayout(attributesLayout);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(servicesGroup, 0, 0);
    layout->addWidget(attributesGroup, 0, 1, 2, 1);
    layout->addWidget(interfacesGroup, 1, 0);
    setLayout(layout);
}
