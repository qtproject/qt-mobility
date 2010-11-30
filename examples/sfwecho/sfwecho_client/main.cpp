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

#include <QApplication>
#include <QMetaObject>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QProcess>
#include <QDateTime>
#include <QLabel>
#include <qservice.h>
#include <qservicemanager.h>

#if defined (Q_OS_SYMBIAN) || defined(Q_OS_WINCE) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#include "ui_sfwecho_client_mobile.h"
#else
#include "ui_sfwecho_client.h"
#endif



QTM_USE_NAMESPACE

class EchoClient : public QMainWindow, public Ui_EchoClient
{
    Q_OBJECT
public:
    EchoClient(QWidget *parent = 0, Qt::WindowFlags flags = 0)
        : QMainWindow(parent, flags)
    {
        setupUi(this);

        globalEcho = 0;
        privateEcho = 0;
        echo = 0;

        // Set default connection to the echo global server
        on_globalChat_toggled(true);
    }

    ~EchoClient()
    {
    }

public slots:
    void on_sendButton_clicked()
    {
        QString message = userName->text() + ": " + messageEdit->text();
        messageEdit->setText("");

        // Send the instant message to the echo server
        QMetaObject::invokeMethod(echo, "sendMessage", Q_ARG(QString, message));
    }

    void on_globalChat_toggled(bool checked)
    {
        privateChat->setChecked(!checked);
        if (checked) {
            if (!connectToChat())
                echoBox->append("**Unable to connect to global Echo Chat server**");
            else 
                echoBox->append("**Connected to global Echo Chat server**");
        }
    }
    
    void on_privateChat_toggled(bool checked)
    {
        globalChat->setChecked(!checked);
        if (checked) {
            if (!connectToChat())
                echoBox->append("**Unable to connect to private Echo Chat server**");
            else 
                echoBox->append("**Connected to private Echo Chat server**");
        }
    }

    void receivedMessage(const QString &msg, const QDateTime &ts)
    {
        QString newMsg = "[" + ts.toString("hh:mm") + "]" + " " + msg;
        echoBox->append(newMsg);
    }

    void errorIPC(QService::UnrecoverableIPCError error)
    {
      QDateTime ts = QDateTime::currentDateTime();
      QString newMsg = "[" + ts.toString("hh:mm") + "]" + " " + "IPC Error! ";

      if (error == QService::ErrorServiceNoLongerAvailable)
          newMsg += "Service no longer available";
      
      echoBox->append(newMsg);
    }

private:
    QObject *globalEcho;
    QObject *privateEcho;
    QObject *echo;

    bool connectToChat()
    {
        // 0 for private and 1 for global
        int version = 0;
        if (globalChat->isChecked())
            version = 1;

        // Set to private server if it previously existed
        if (privateEcho && version == 0) {
            echo = privateEcho;
            return true;
        }
        
        // Set to global server if it previously existed
        if (globalEcho && version == 1) {
            echo = globalEcho;
            return true;
        }

        // No connection established yet
        QServiceManager manager;
        QList<QServiceInterfaceDescriptor> list = manager.findInterfaces("EchoService");
        if(list.count() < version+1){
            echoBox->append("**Unable to find a registered service**");
            return false;
        }

        QServiceInterfaceDescriptor desc = list[version];
        if (!desc.isValid()) {            
            qWarning() << "EchoService interface not found";
            return false;
        }

        QObject *service = manager.loadInterface(desc);
        if (!service) {  
            qWarning() << "EchoService unable to connect";
            return false;
        }

        if (version == 0) {
            privateEcho = service;
            echo = privateEcho;
        } else {
            globalEcho = service;
            echo = globalEcho;
        }

        echo->setParent(this);

        // Connect echo message broadcasts
        QObject::connect(echo, SIGNAL(broadcastMessage(QString,QDateTime)),
                         this, SLOT(receivedMessage(QString,QDateTime)));

        // Connect IPC errors
        QObject::connect(echo, SIGNAL(errorUnrecoverableIPCFault(QService::UnrecoverableIPCError)),
                         this, SLOT(errorIPC(QService::UnrecoverableIPCError)));

        return true;
    }
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    EchoClient dialog;

#ifdef Q_OS_SYMBIAN
    dialog.showMaximized();
#else
    dialog.show();
#endif

    return app.exec();
    
}


#include "main.moc"
