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

#include <qmobilityglobal.h>
#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include "pspathmapperserver.h"
#include "clientservercommon.h"

QTM_USE_NAMESPACE

#include <QFile>
#include <QTextStream>
void debugOutput(QtMsgType type, const char *msg)
{
    QFile file("C:\\Data\\pspathmapperserver_debug.txt");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
      return;

    QTextStream out(&file);
    out << "Debug: ";
    out << msg;
    out << "\n";
}

int main(int argc, char **argv)
{
    qInstallMsgHandler(debugOutput);

    QCoreApplication app(argc, argv);
    
    qDebug("Attempting to create CPSPathMapperServer");
    
    CPSPathMapperServer* server = new CPSPathMapperServer;
    TInt err = server->Start(KPSPathMapperServerName);
    qDebug() << "err" << err;
    if (err != KErrNone)
    {
        CPSPathMapperServer::PanicServer(ESvrStartServer);
    }
    RProcess::Rendezvous(err);

    return app.exec();
}

