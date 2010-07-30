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
#include <QTimer>
#include <QMetaObject>
#include <QMetaMethod>
#include <QVBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QProcess>
#include <QDateTime>
#include <QLabel>
#include <qservice.h>
#include <qservicemanager.h>

QTM_USE_NAMESPACE

bool verbose = true;

bool requiresLackey()
{
#ifdef Q_OS_SYMBIAN
    return false; //service is started when requested
#else
    return true;
#endif
}

class TopExample : public QWidget
{
    Q_OBJECT
public:
    TopExample(QWidget *parent = 0x0) : QWidget(parent), lackey(0)
    {
        QLineEdit *editrw = new QLineEdit();
        QLineEdit *editro = new QLineEdit();

        editro->setReadOnly(true);
        editro->setFocusPolicy(Qt::NoFocus);

        QPushButton *buttonrw = new QPushButton();
        buttonro = new QPushButton();
        buttonro->setFocusPolicy(Qt::NoFocus);

        label = new QLabel();

        QLabel *name = new QLabel();

        QVBoxLayout *l = new QVBoxLayout();
        l->addWidget(editrw);
        l->addWidget(editro);
        l->addWidget(buttonrw);
        l->addWidget(buttonro);
        l->addWidget(label);
        l->addWidget(name);

        this->setLayout(l);

        if(startIPC() < 0){            
            startService();            
            for(int i = 0; i<100; i++){                
                if(startIPC() == 0)
                    break;
            }
        }

        connect(editrw, SIGNAL(textChanged(QString)), echo, SLOT(inputSignal(QString)));
        connect(echo, SIGNAL(echoSignal(QString)), editro, SLOT(setText(QString)));

        connect(buttonrw, SIGNAL(pressed()), echo, SLOT(pressed()));
        connect(buttonrw, SIGNAL(released()), echo, SLOT(released()));
        connect(echo, SIGNAL(down(bool)), this, SLOT(down(bool)));

        connect(echo, SIGNAL(tick(QDateTime)), this, SLOT(tick(QDateTime)));

        label->setText("Waiting for signal");
        name->setText(echo->metaObject()->className());

    }

    virtual ~TopExample()
    {
        lackey->terminate();
        lackey->waitForFinished();
    }

public slots:
    void tick(QDateTime d)
    {
        label->setText(d.toString());
    }

    void down(bool b)
    {
        buttonro->setDown(b);
    }

private:
    QObject *echo;
    QProcess *lackey;
    QLabel *label;
    QPushButton *buttonro;

    int startIPC(){

        QServiceManager manager;

        // should work.
        // QServiceInterfaceDescriptor d = manager->interfaceDefault("EchoService");
        QList<QServiceInterfaceDescriptor> ld = manager.findInterfaces("EchoService");

        if(ld.count() == 0){            
            return -1;
        }

        echo = manager.loadInterface(ld[0]);
        if(!ld[0].isValid() || echo == 0x0){            
            return -2;
        }
        return 0;
    }

    int startService() {        
        if (requiresLackey()) {
            lackey = new QProcess(this);
            if (verbose)
                lackey->setProcessChannelMode(QProcess::ForwardedChannels);
            lackey->start("./sfwecho_service");
            lackey->waitForStarted();
            if(lackey->error() != QProcess::UnknownError || lackey->state() != QProcess::Running) {
                qDebug() << lackey->error() << lackey->errorString();            
                return -1;
            }
        }
        return 0;
    }


};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    TopExample t;

    t.show();

    return app.exec();
    
}


#include "main.moc"
