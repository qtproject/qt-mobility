/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qremoteserviceregister.h>
#include "qservicemanager.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>

QTM_USE_NAMESPACE

class EchoService : public QObject
{
    Q_OBJECT

public:
    EchoService(QObject* parent = 0)
        : QObject(parent)
    {
    }

Q_SIGNALS:
    void broadcastMessage(const QString &msg, const QDateTime &timestamp);

public slots:
    void sendMessage(const QString &msg)
    {
        emit broadcastMessage(msg, QDateTime::currentDateTime());
    }

private:

};


void unregisterExampleService()
{
    QServiceManager m;
    m.removeService("EchoService");
}

void registerExampleService()
{
    unregisterExampleService();
    QServiceManager m;
    const QString path = QCoreApplication::applicationDirPath() + "/xmldata/sfwechoservice.xml";
    if (!m.addService(path)) {
        qWarning() << "Cannot register EchoService" << path;
    }
}

Q_DECLARE_METATYPE(QMetaType::Type);

/*
bool check(const void *p)
{
    // Denies clients who have odd processs Ids
    const QRemoteServiceRegisterCredentials *cr = (const struct QRemoteServiceRegisterCredentials *)p;
    if(cr->pid%2) {
           qDebug() << "Failing client: " << cr->pid;
        return false;
    }

    return true;
}
*/

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    registerExampleService();

    QRemoteServiceRegister *serviceRegister = new QRemoteServiceRegister();
    //serviceRegister->setSecurityFilter(check);

    QRemoteServiceRegister::Entry shared = serviceRegister->createEntry<EchoService>(
        "EchoService", "com.nokia.qt.example.sfwecho", "1.1");
    shared.setInstantiationType(QRemoteServiceRegister::GlobalInstance);

    QRemoteServiceRegister::Entry unique = serviceRegister->createEntry<EchoService>(
        "EchoService", "com.nokia.qt.example.sfwecho", "1.0");
    unique.setInstantiationType(QRemoteServiceRegister::PrivateInstance);

    serviceRegister->publishEntries("sfwecho_service");
    serviceRegister->setQuitOnLastInstanceClosed(true);

    int res =  app.exec();

    delete serviceRegister;
    return res;
}



#include "main.moc"
