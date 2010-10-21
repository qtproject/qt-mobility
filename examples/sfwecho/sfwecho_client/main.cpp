/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
