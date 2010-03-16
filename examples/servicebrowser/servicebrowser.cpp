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

#include <qservicemanager.h>
#include <qserviceinterfacedescriptor.h>

#include "servicebrowser.h"

Q_DECLARE_METATYPE(QServiceInterfaceDescriptor)

ServiceBrowser::ServiceBrowser(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    serviceManager = new QServiceManager(this);

    registerExampleServices();

    initWidgets();
    reloadServicesList();

    setWindowTitle(tr("Services Browser"));
}

ServiceBrowser::~ServiceBrowser()
{
    unregisterExampleServices();
}

void ServiceBrowser::currentInterfaceImplChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);
    if (!current)
        return;

    reloadAttributesList();
    reloadAttributesRadioButtonText();

    QServiceInterfaceDescriptor descriptor = current->data(Qt::UserRole).value<QServiceInterfaceDescriptor>();
    if (descriptor.isValid()) {
        defaultInterfaceButton->setText(tr("Set as default implementation for %1") 
                .arg(descriptor.interfaceName()));
        defaultInterfaceButton->setEnabled(true);
    }
}

void ServiceBrowser::reloadServicesList()
{
    servicesListWidget->clear();

    QStringList services = serviceManager->findServices();
    for (int i=0; i<services.count(); i++)
        servicesListWidget->addItem(services[i]);

    servicesListWidget->addItem(showAllServicesItem);
}

void ServiceBrowser::reloadInterfaceImplementationsList()
{
    QString serviceName;
    if (servicesListWidget->currentItem()
            && servicesListWidget->currentItem() != showAllServicesItem) {
        serviceName = servicesListWidget->currentItem()->text();
        interfacesGroup->setTitle(tr("Interfaces implemented by %1").arg(serviceName));
    } else {
        interfacesGroup->setTitle(tr("All interface implementations"));
    }

    QList<QServiceInterfaceDescriptor> descriptors = serviceManager->findInterfaces(serviceName);

    attributesListWidget->clear();
    interfacesListWidget->clear();
    for (int i=0; i<descriptors.count(); i++) {
        QString text = QString("%1 %2.%3")
                .arg(descriptors[i].interfaceName())
                .arg(descriptors[i].majorVersion())
                .arg(descriptors[i].minorVersion());

        if (serviceName.isEmpty())
            text += " (" + descriptors[i].serviceName() + ")";

        QServiceInterfaceDescriptor defaultInterfaceImpl = 
                serviceManager->interfaceDefault(descriptors[i].interfaceName());
        if (descriptors[i] == defaultInterfaceImpl)
            text += tr(" (default)");

        QListWidgetItem *item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, qVariantFromValue(descriptors[i]));
        interfacesListWidget->addItem(item);
    }

    defaultInterfaceButton->setEnabled(false);
}

void ServiceBrowser::reloadAttributesList()
{
    QListWidgetItem *item = interfacesListWidget->currentItem();
    if (!item)
        return;

    QServiceInterfaceDescriptor selectedImpl =
            item->data(Qt::UserRole).value<QServiceInterfaceDescriptor>();

    QObject *implementationRef;
    if (selectedImplRadioButton->isChecked())
        implementationRef = serviceManager->loadInterface(selectedImpl, 0, 0);
    else
        implementationRef = serviceManager->loadInterface(selectedImpl.interfaceName(), 0, 0);

    attributesListWidget->clear();
    if (!implementationRef) {
        attributesListWidget->addItem(tr("(Error loading service plugin)"));
        return;
    }

    const QMetaObject *metaObject = implementationRef->metaObject();
    attributesGroup->setTitle(tr("Invokable attributes for %1 class")
            .arg(QString(metaObject->className())));
    for (int i=0; i<metaObject->methodCount(); i++) {
        QMetaMethod method = metaObject->method(i);
        attributesListWidget->addItem("[METHOD] " + QString(method.signature()));
    }
    for (int i=0; i<metaObject->propertyCount(); i++) {
        QMetaProperty property = metaObject->property(i);
        attributesListWidget->addItem("[PROPERTY] " + QString(property.name()));
    }
}

void ServiceBrowser::setDefaultInterfaceImplementation()
{
    QListWidgetItem *item = interfacesListWidget->currentItem();
    if (!item)
        return;

    QServiceInterfaceDescriptor descriptor = item->data(Qt::UserRole).value<QServiceInterfaceDescriptor>();
    if (descriptor.isValid()) {
        if (serviceManager->setInterfaceDefault(descriptor)) {
            int currentIndex = interfacesListWidget->row(item);
            reloadInterfaceImplementationsList();
            interfacesListWidget->setCurrentRow(currentIndex);
        } else {
            qWarning() << "Unable to set default service for interface:" 
                    << descriptor.interfaceName();
        }
    }
}

void ServiceBrowser::registerExampleServices()
{
    QStringList exampleXmlFiles;
    exampleXmlFiles << "filemanagerservice.xml" << "bluetoothtransferservice.xml";
    foreach (const QString &fileName, exampleXmlFiles) {
        QString path = QCoreApplication::applicationDirPath() + "/xmldata/" + fileName;
        serviceManager->addService(path);
    }
}

void ServiceBrowser::unregisterExampleServices()
{
    serviceManager->removeService("FileManagerService");
    serviceManager->removeService("BluetoothTransferService");
}

void ServiceBrowser::reloadAttributesRadioButtonText()
{
    QListWidgetItem *item = interfacesListWidget->currentItem();
    if (!item)
        return;

    QServiceInterfaceDescriptor selectedImpl =
            item->data(Qt::UserRole).value<QServiceInterfaceDescriptor>();
    QServiceInterfaceDescriptor defaultImpl = 
            serviceManager->interfaceDefault(selectedImpl.interfaceName());

    defaultImplRadioButton->setText(tr("Default implementation for %1\n(currently provided by %2)")
            .arg(defaultImpl.interfaceName())
            .arg(defaultImpl.serviceName()));
}

void ServiceBrowser::initWidgets()
{
    showAllServicesItem = new QListWidgetItem(tr("(All registered services)"));

    servicesListWidget = new QListWidget;
    interfacesListWidget = new QListWidget;
    interfacesListWidget->addItem(tr("(Select a service)"));
    attributesListWidget = new QListWidget;
    attributesListWidget->addItem(tr("(Select an interface implementation)"));

#ifndef Q_OS_SYMBIAN
    interfacesListWidget->setMinimumWidth(450);
#endif

    connect(servicesListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(reloadInterfaceImplementationsList()));

    connect(interfacesListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(currentInterfaceImplChanged(QListWidgetItem*,QListWidgetItem*)));

    defaultInterfaceButton = new QPushButton(tr("Set as default implementation"));
    defaultInterfaceButton->setEnabled(false);
    connect(defaultInterfaceButton, SIGNAL(clicked()),
            this, SLOT(setDefaultInterfaceImplementation()));

    selectedImplRadioButton = new QRadioButton(tr("Selected interface implementation"));
    defaultImplRadioButton = new QRadioButton(tr("Default implementation"));
    selectedImplRadioButton->setChecked(true);

    QButtonGroup *radioButtons = new QButtonGroup(this);
    radioButtons->addButton(selectedImplRadioButton);
    radioButtons->addButton(defaultImplRadioButton);
    connect(radioButtons, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(reloadAttributesList()));

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

#ifndef Q_OS_SYMBIAN
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(servicesGroup, 0, 0);
    layout->addWidget(attributesGroup, 0, 1, 2, 1);
    layout->addWidget(interfacesGroup, 1, 0);
#else
    QVBoxLayout *layout = new QVBoxLayout;    
    QStackedLayout *stackedLayout = new QStackedLayout;
    stackedLayout->addWidget(servicesGroup);
    stackedLayout->addWidget(interfacesGroup);
    stackedLayout->addWidget(attributesGroup);
    
    QComboBox *pageComboBox = new QComboBox;
    pageComboBox->addItem(tr("Services"));
    pageComboBox->addItem(tr("Interfaces"));
    pageComboBox->addItem(tr("Attributes"));
    connect(pageComboBox, SIGNAL(activated(int)),
            stackedLayout, SLOT(setCurrentIndex(int)));
    
    layout->addWidget(pageComboBox);
    layout->addLayout(stackedLayout);
#endif

    setLayout(layout);
}
