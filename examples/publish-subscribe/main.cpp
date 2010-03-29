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

#include "publisherdialog.h"
#include "subscriberdialog.h"

#include <QApplication>

#include <qvaluespace.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    bool createDefault = true;
    bool createPublisher = false;
    bool createSubscriber = false;

    for (int i = 1; i < argc; ++i) {
        if (argv[i] == QLatin1String("-server")) {
            QValueSpace::initValueSpaceServer();
        } else if (argv[i] == QLatin1String("-publisher")) {
            createPublisher = true;
            createDefault = false;
        } else if (argv[i] == QLatin1String("-subscriber")) {
            createSubscriber = true;
            createDefault = false;
        }
    }

    PublisherDialog *publisher = 0;
    if (createDefault || createPublisher) {
        publisher = new PublisherDialog;
        QObject::connect(publisher, SIGNAL(rejected()), &app, SLOT(quit()));
#ifndef QTM_EXAMPLES_SMALL_SCREEN
        publisher->show();
#endif
    }

    SubscriberDialog *subscriber = 0;
    if (createDefault || createSubscriber) {
        subscriber = new SubscriberDialog;
        QObject::connect(subscriber, SIGNAL(rejected()), &app, SLOT(quit()));
#ifndef QTM_EXAMPLES_SMALL_SCREEN
        subscriber->show();
#else
        subscriber->showMaximized();
#endif
    }

#ifdef QTM_EXAMPLES_SMALL_SCREEN
    QObject::connect(publisher, SIGNAL(switchRequested()), subscriber, SLOT(showMaximized()));
    QObject::connect(publisher, SIGNAL(switchRequested()), subscriber, SLOT(repaint()));
    QObject::connect(publisher, SIGNAL(switchRequested()), publisher, SLOT(hide()));

    QObject::connect(subscriber, SIGNAL(switchRequested()), publisher, SLOT(showMaximized()));
    QObject::connect(subscriber, SIGNAL(switchRequested()), publisher, SLOT(repaint()));
    QObject::connect(subscriber, SIGNAL(switchRequested()), subscriber, SLOT(hide()));
#endif

    int result = app.exec();

    if (publisher)
        delete publisher;
    if (subscriber)
        delete subscriber;

    return result;
}
