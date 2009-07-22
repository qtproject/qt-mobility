/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact:  Nokia Corporation (qt-info@nokia.com)**
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
** will be met:  http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.  
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.  
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QCoreApplication>
#include "qvaluespace.h"
#include <QTest>
#include <QFile>
#include <QTimer>
#include <stdio.h>
#include <QDebug>

#define TIMEOUT 700

class Controller : public QObject
{
    Q_OBJECT
public:
    Controller(int function) 
        : QObject(0), index(0)
    {
        if (function == 0) {
            object = new QValueSpaceObject("/usr/lackey/subdir", this);
            connect(object, SIGNAL(itemSetValue(QByteArray,QVariant)),
                    this, SLOT(itemSetValue(QByteArray,QVariant)));
            object->setObjectName("original_lackey");
            object->setAttribute("value", 100);
            object->sync();
            item = new QValueSpaceItem("/usr/lackey/subdir", this);
            connect(item, SIGNAL(contentsChanged()), this, SLOT(changes()));

            QTimer::singleShot(TIMEOUT, this, SLOT(proceed()));
        } else {
            item = new QValueSpaceItem("/usr/lackey", this);
            item->setValue("changeRequests/value", 501);
            QTimer::singleShot(TIMEOUT, this, SLOT(setValueNextStep()));
        }
    }

private slots:
    void setValueNextStep()
    {
        switch(index) {
            case 0:
                item->setValue(QByteArray("changeRequests/value"), 502);
                break;
            case 1:
                item->remove("changeRequests/value");
                break;
            case 2:
                item->setValue(QString("changeRequests/value"), 503);
                break;
            case 3:
                item->remove(QString("changeRequests/value"));
                break;
            case 4:
                item->remove(QByteArray("changeRequests/value"));
                break;
            case 5: 
                item->remove("changeRequests/test");
                break;
            case 6:
                item->remove();
                break;
        }
        index++;
        item->sync();
        if (index == 7) {
            QTimer::singleShot(TIMEOUT, qApp, SLOT(quit()));
        } else
            QTimer::singleShot(TIMEOUT, this, SLOT(setValueNextStep()));
 
    }

    void proceed() {
        switch(index) {

            case 0:
                //qDebug() << "Setting 101";
                object->setAttribute(QByteArray("value"), 101);
                break;
            case 1:
                //qDebug() << "Removing";
                object->removeAttribute("value");
                break;
            case 2:
                //qDebug() << "Setting 102";
                object->setAttribute(QString("value"), 102);
                break;
        }
        index++;
        object->sync();
        if (index == 3)
            QTimer::singleShot(TIMEOUT, qApp, SLOT(quit()));
        else
            QTimer::singleShot(TIMEOUT, this, SLOT(proceed()));
    }

    void itemSetValue(const QByteArray& /*path*/, const QVariant& /*variant*/ )
    {
        //qDebug() << sender()->objectName() << path << variant.toInt();
    }

    void changes()
    {
        qDebug() << "changes:" << item->value("mine", 6).toInt();

    }

private:
    QValueSpaceObject* object;
    QValueSpaceItem *item;
    int index;
};

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QStringList arguments = app.arguments();
    arguments.takeFirst();
    //QValueSpace::initValuespaceManager();

    bool testSetValue = false;
    while (!arguments.isEmpty()) {
        QString arg = arguments.takeFirst();
        if (arg == "-ipcSetValue") {
            testSetValue = true;
            break;
        }
    }

    Controller controler(testSetValue);;
    qDebug() << "Starting lackey";
    return app.exec();
}

#include "main.moc"
