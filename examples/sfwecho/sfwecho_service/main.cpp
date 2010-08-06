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
#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>
#include <qremoteserviceclassregister.h>
#include <qremoteservicecontrol.h>
#include "qservicemanager.h"
#include <QDebug>


#include "qservicefilter.h" //only used to test custom metatype

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QServiceFilter);
Q_DECLARE_METATYPE(QVariant)

class EchoTestService : public QObject
{
    Q_OBJECT
    Q_SERVICE(EchoTestService, "EchoService", "com.nokia.qt.example.sfwecho", "1.0")

public:
    EchoTestService(QObject* parent = 0)
        : QObject(parent)
    {
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
        timer->start(1000);
    }


Q_SIGNALS:
    void echoSignal(QString);
    void echoClicked(bool);
    void tick(QDateTime);
    void down(bool);

public slots:

    void inputSignal(QString str)
    {
        emit echoSignal(str);
    }

    void timeout()
    {
        emit tick(QDateTime::currentDateTime());
    }

    void pressed()
    {
        emit down(true);
    }

    void released()
    {
        emit down(false);
    }


private:

};

Q_DECLARE_METATYPE(QMetaType::Type);


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QServiceManager m;
    const QString path = QCoreApplication::applicationDirPath() + "/xmldata/sfwechoservice.xml";

    bool r = m.addService(path);
    if (!r) {
        qWarning() << "Cannot register EchoService" << path;
    }

    qDebug() << "Registered: " << path;

    //QRemoteServiceClassRegister::registerType<SharedTestService>(QRemoteServiceClassRegister::SharedInstance);
    QRemoteServiceClassRegister::registerType<EchoTestService>(QRemoteServiceClassRegister::SharedInstance);

    //this only works
    QRemoteServiceControl* control = new QRemoteServiceControl();
    control->publishServices("qt_sfw_example_echo");
#ifdef Q_OS_SYMBIAN
    RProcess::Rendezvous(KErrNone);
#endif

    int res =  app.exec();
    delete control;    
    return res;
}



#include "main.moc"
