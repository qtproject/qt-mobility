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

#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QString>
#include <QUrl>
#include <QtCore>

//! [0]
//Includes for using the QML objects
#include <QmlView>
#include <QmlContext>

//Includes for using the service framework
#include <qserviceinterfacedescriptor.h>
#include <qservicemanager.h>
//! [0]

#include "sfwexample.h"

void usage()
{
    qWarning() << "Usage: sfw-kinetic-example.qml";
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QString qmlFile;
    for (int j = 1; j < argc; j++) {
        QString arg = argv[j];
        if (arg.startsWith(QChar('-')))
            continue;
        else
            qmlFile = arg;
    }

    if (qmlFile.isEmpty()) {
        usage();
        return 1;
    }

    //! [1]
    QUrl url(qmlFile);
    QFileInfo fi(qmlFile);
    if (fi.exists())
        url = QUrl::fromLocalFile(fi.absoluteFilePath());
    else
        return 1;

    QmlView canvas;
    canvas.setSource(url);
    //! [1]

    //! [2]
    //...
    //! [2]

    //! [3]
    canvas.execute();
    canvas.show();
    //! [3]

    return app.exec();}
