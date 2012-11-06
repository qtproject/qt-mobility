/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>
#include <QServiceManager>
#include <QFile>
#include <QFileInfo>

#include "sfwtestclient.h"

QTM_USE_NAMESPACE

//This is the service XML which will be imported to this path by sfwtest service.
const QString sfwtestservicefile = "/resource/apps/xmldata/sfwtestservice.xml";

sfwtestclient::sfwtestclient(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);
	
    QObject::connect(ui.addServiceButton, SIGNAL(clicked()),
                     this, SLOT(addServiceButtonClicked()));

    QObject::connect(ui.invokeButton, SIGNAL(clicked()),
                     this, SLOT(invokeButtonClicked()));

}

sfwtestclient::~sfwtestclient()
{

}
/*
 * This is a slot for Add Service Button, it adds the OOP sfwtest service.
 */
void sfwtestclient::addServiceButtonClicked()
{
    ui.textEditBox->clear();
    
    QFileInfo fileInfo(sfwtestservicefile);
    QString filePath = fileInfo.absoluteFilePath();
    QServiceManager serviceManager;
    if (QFile::exists(filePath)) {
        if (serviceManager.addService(filePath)) {
            ui.textEditBox->setText("Add Service: Success\n");
        }
        else {
            ui.textEditBox->setText("Add Service: Service Exists Already\n");
        }
    }
    else {
        ui.textEditBox->setText("Removing Service\n");
        serviceManager.removeService("SfwTestService\n");
    }

}

/*
 * This is the slot for Invoke Button, which loads the OOP service
 * and then invokes the Remote Method provided by the service.
 */
void sfwtestclient::invokeButtonClicked()
{
    ui.textEditBox->clear();
    
    QServiceManager serviceManager;
    QServiceFilter serviceFilter;
    serviceFilter.setServiceName("SfwTestService");
    serviceFilter.setInterface("com.nokia.qt.SfwTest");
    
    QList<QServiceInterfaceDescriptor> interfaceList;
    interfaceList = serviceManager.findInterfaces(serviceFilter);
    
    if(!interfaceList.count()) {
        ui.textEditBox->setText("No Interfaces Found\n");
        return;
    }
    
    if(interfaceList.count() > 1) {
        ui.textEditBox->setText("Multiple Interfaces Found\n");
        return;
    }
    ui.textEditBox->setText("Loading Interface...\n");
    
    QObject *sfwTestService = NULL;

    sfwTestService = serviceManager.loadInterface(interfaceList.at(0));

    if (sfwTestService) {
        ui.textEditBox->setText("Loading Interface: Success\n");
        ui.textEditBox->setText("Invoking Interface Method: getServiceCount...\n");
        int result = 0;
        
        QMetaObject::invokeMethod(sfwTestService, "getServiceCount", Q_RETURN_ARG(int, result));
    
        if (!result) {
            ui.textEditBox->setText("Invoke Interface Method getServiceCount: No Services Found\n");
        }
        else {
            QString fmtString;
            ui.textEditBox->setText(fmtString.sprintf("Invoke Interface Method getServiceCount: Found %d Services\n",result));
        }
        delete sfwTestService;
        sfwTestService = NULL;

    }
    else {
        ui.textEditBox->setText("Loading Interface: Failed\n");
    }

}


