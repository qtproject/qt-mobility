/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "../../../../src/publishsubscribe/qvaluespacesubscriber.h"
#include "../../../../src/publishsubscribe/qvaluespacepublisher.h"

#include <QCoreApplication>
#include <QStringList>
#include <QTimer>
#include <QUuid>

#include <QDebug>

QTM_USE_NAMESPACE

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
            publisher = new QValueSpacePublisher(uuid, "/usr/lackey/subdir", this);
            publisher->setObjectName("original_lackey");
            publisher->setValue("value", 100);
            publisher->sync();
            subscriber = new QValueSpaceSubscriber(uuid, "/usr/lackey/subdir", this);
            connect(subscriber, SIGNAL(contentsChanged()), this, SLOT(changes()));

            QTimer::singleShot(TIMEOUT, this, SLOT(proceed()));
            break;
        case IpcInterestNotification:
            publisher = new QValueSpacePublisher(uuid, "/ipcInterestNotification", this);
            connect(publisher, SIGNAL(interestChanged(QString,bool)),
                    this, SLOT(interestChanged(QString,bool)));
            break;
        case IpcRemoveKey:
            publisher = new QValueSpacePublisher(uuid, "/ipcRemoveKey", this);
            publisher->setValue("value", 100);
            publisher->sync();
            QTimer::singleShot(TIMEOUT, this, SLOT(removeKey()));
            break;
        }
    }

private slots:
    void proceed() {
        switch (index) {
        case 0:
            //qDebug() << "Setting 101";
            publisher->setValue("value", 101);
            break;
        case 1:
            //qDebug() << "Removing";
            publisher->resetValue("value");
            break;
        case 2:
            //qDebug() << "Setting 102";
            publisher->setValue("value", 102);
            break;
        case 3:
            qDebug() << "Removing";
            publisher->resetValue("value");
            break;
        }
        publisher->sync();

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

    void interestChanged(const QString &attribute, bool interested)
    {
        //qDebug() << Q_FUNC_INFO << path << interested;
        if (interested) {
            if (attribute == "/value")
                publisher->setValue(attribute, 5);
        } else {
            publisher->resetValue(attribute);
        }
    }

    void removeKey()
    {
        if (publisher) {
            delete publisher;
            publisher = 0;
        }

        QTimer::singleShot(TIMEOUT, qApp, SLOT(quit()));
    }

private:
    QValueSpacePublisher *publisher;
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
