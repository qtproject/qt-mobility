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

        sharedEcho = 0;
        uniqueEcho = 0;
        echo = 0;

        // Set default connection to the echo shared server
        on_sharedChat_toggled(true);
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

    void on_sharedChat_toggled(bool checked)
    {
        uniqueChat->setChecked(!checked);
        if (checked) {
            if (!connectToChat())
                echoBox->append("**Unable to connect to shared Echo Chat server**");
            else 
                echoBox->append("**Connected to shared Echo Chat server**");
        }
    }
    
    void on_uniqueChat_toggled(bool checked)
    {
        sharedChat->setChecked(!checked);
        if (checked) {
            if (!connectToChat())
                echoBox->append("**Unable to connect to unique Echo Chat server**");
            else 
                echoBox->append("**Connected to unique Echo Chat server**");
        }
    }

    void receivedMessage(const QString &msg, const QDateTime &ts)
    {
        QString newMsg = "[" + ts.toString("hh:mm") + "]" + " " + msg;
        echoBox->append(newMsg);
    }

    void errorIPC()
    {
      QDateTime ts = QDateTime::currentDateTime();
      QString newMsg = "[" + ts.toString("hh:mm") + "]" + " " + "IPC Error";
      echoBox->append(newMsg);
    }

private:
    QObject *sharedEcho;
    QObject *uniqueEcho;
    QObject *echo;

    bool connectToChat()
    {
        // 0 for unique and 1 for shared
        int version = 0;
        if (sharedChat->isChecked())
            version = 1;

        // Set to unique server if it previously existed
        if (uniqueEcho && version == 0) {
            echo = uniqueEcho;
            return true;
        }
        
        // Set to shared server if it previously existed
        if (sharedEcho && version == 1) {
            echo = sharedEcho;
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
            uniqueEcho = service;
            echo = uniqueEcho;
        } else {
            sharedEcho = service;
            echo = sharedEcho;
        }

        echo->setParent(this);

        // Connect echo message broadcasts
        QObject::connect(echo, SIGNAL(broadcastMessage(QString,QDateTime)),
                         this, SLOT(receivedMessage(QString,QDateTime)));

        QObject::connect(echo, SIGNAL(errorUnrecoverableIPCFault(QService::UnrecoverableIPCError)),
                         this, SLOT(errorIPC()));

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
