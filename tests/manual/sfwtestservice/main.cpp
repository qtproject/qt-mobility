/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>
#include <QCoreApplication>
#include <QRemoteServiceRegister>
#include <QServiceManager>

#include "sfwtestservice.h"

QTM_USE_NAMESPACE

/*
 * This is a test service which provides an interface for testing
 * the OOP service mechanism provided by Qt Mobility Service Framework.
 * This main function of sfwtest service publishes
 * its entires/services.
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
	
    QRemoteServiceRegister *serviceRegisterSfwTest = new QRemoteServiceRegister();
    QRemoteServiceRegister::Entry serviceUriHandler = serviceRegisterSfwTest->createEntry<
        SfwTestService> ("SfwTestService", "com.nokia.qt.SfwTest", "1.0");

    serviceUriHandler.setInstantiationType(QRemoteServiceRegister::GlobalInstance);
    serviceRegisterSfwTest->publishEntries("sfwtest");

    serviceRegisterSfwTest->setQuitOnLastInstanceClosed(false);

    
    return a.exec();
}
