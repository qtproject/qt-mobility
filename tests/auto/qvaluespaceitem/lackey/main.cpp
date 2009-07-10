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
//#include <QDebug>

class Controller : public QObject
{
    Q_OBJECT
public:
    Controller() 
        : QObject(0), index(0)
    {
        object = new QValueSpaceObject("/usr/lackey/subdir", this);
        object->setAttribute("value", 100);
        object->sync();
        QTimer::singleShot(1000, this, SLOT(proceed()));
    }

private slots:
    void proceed() {

        switch(index) {
            case 0:
                //qDebug() << "Setting 101";
                object->setAttribute("value", 101);
                break;
            case 1:
                //qDebug() << "Removing";
                object->removeAttribute("value");
                break;
            case 2:
                //qDebug() << "Setting 102";
                object->setAttribute("value", 102);
                break;
        }
        index++;
        object->sync();
        if (index == 3)
            QTimer::singleShot(1000, qApp, SLOT(quit()));
        else
            QTimer::singleShot(1000, this, SLOT(proceed()));
    }

private:
    QValueSpaceObject* object;
    int index;
};

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    Controller controler;
    //qDebug() << "Starting lackey";
    return app.exec();
}

#include "main.moc"
