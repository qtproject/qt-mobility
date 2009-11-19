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

#include <qvaluespacesubscriber.h>
#include <qvaluespaceprovider.h>

#include <QCoreApplication>
#include <QStringList>
#include <QTimer>
#include <QUuid>

#include <QDebug>

#define TIMEOUT 700

#define ERROR_SETVALUE_NOT_SUPPORTED 1

class Controller : public QObject
{
    Q_OBJECT

public:
    enum Function { Invalid, IpcTests, IpcInterestNotification, IpcRemoveKey };

    Controller(Function function, const QUuid uuid)
        : QObject(0), index(0), abortCode(0)
    {
        switch (function) {
        case Invalid:
            QTimer::singleShot(0, this, SLOT(quit()));
            break;
        case IpcTests:
            provider = new QValueSpaceProvider("/usr/lackey/subdir", uuid, this);
            provider->setObjectName("original_lackey");
            provider->setAttribute("value", 100);
            provider->sync();
            subscriber = new QValueSpaceSubscriber("/usr/lackey/subdir", uuid, this);
            connect(subscriber, SIGNAL(contentsChanged()), this, SLOT(changes()));

            QTimer::singleShot(TIMEOUT, this, SLOT(proceed()));
            break;
        case IpcInterestNotification:
            provider = new QValueSpaceProvider("/ipcInterestNotification", uuid, this);
            connect(provider, SIGNAL(attributeInterestChanged(QString,bool)),
                    this, SLOT(attributeInterestChanged(QString,bool)));
            break;
        case IpcRemoveKey:
            provider = new QValueSpaceProvider("/ipcRemoveKey", uuid, this);
            provider->setAttribute("value", 100);
            provider->sync();
            QTimer::singleShot(TIMEOUT, this, SLOT(removeKey()));
            break;
        }
    }

private slots:
    void proceed() {
        switch (index) {
        case 0:
            //qDebug() << "Setting 101";
            provider->setAttribute("value", 101);
            break;
        case 1:
            //qDebug() << "Removing";
            provider->removeAttribute("value");
            break;
        case 2:
            //qDebug() << "Setting 102";
            provider->setAttribute("value", 102);
            break;
        case 3:
            qDebug() << "Removing";
            provider->removeAttribute("value");
            break;
        }
        provider->sync();

        index++;
        if (index == 4)
            QTimer::singleShot(TIMEOUT, qApp, SLOT(quit()));
        else
            QTimer::singleShot(TIMEOUT, this, SLOT(proceed()));
    }

    void abort() {
        qApp->exit(abortCode);
    }

    void changes()
    {
        //qDebug() << "changes:" << subscriber->value("mine", 6).toInt();
    }

    void attributeInterestChanged(const QString &attribute, bool interested)
    {
        //qDebug() << Q_FUNC_INFO << path << interested;
        if (interested) {
            if (attribute == "/value")
                provider->setAttribute(attribute, 5);
        } else {
            provider->removeAttribute(attribute);
        }
    }

    void removeKey()
    {
        if (provider) {
            delete provider;
            provider = 0;
        }

        QTimer::singleShot(TIMEOUT, qApp, SLOT(quit()));
    }

private:
    QValueSpaceProvider *provider;
    QValueSpaceSubscriber *subscriber;
    int index;
    int abortCode;
};

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QStringList arguments = app.arguments();
    arguments.takeFirst();

    if (arguments.count() != 2) {
        qWarning("lackey expects 2 arguments.");
        return 1;
    }

    Controller::Function function = Controller::Invalid;
    {
        const QString arg = arguments.takeFirst();

        if (arg == "-ipcTests") {
            function = Controller::IpcTests;
        } else if (arg == "-ipcInterestNotification") {
            function = Controller::IpcInterestNotification;
        } else if (arg == "-ipcRemoveKey") {
            function = Controller::IpcRemoveKey;
        }
    }

    QUuid uuid(arguments.takeFirst());

    Controller controler(function, uuid);
    qDebug() << "Starting lackey";
    return app.exec();
}

#include "main.moc"
