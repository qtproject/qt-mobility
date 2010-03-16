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
#include <QtGui>
#include <QmlEngine>
#include <QmlComponent>
#include <QDebug>
#include <QmlGraphicsItem>
#include <QmlView>
#include <QContactManager>
#include "qmlcontactsa.h"
QT_USE_NAMESPACE
QTM_USE_NAMESPACE

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);


    QmlEngine engine;
    QmlComponent component(&engine, ":example.qml");
//    QMLContactManager *qcm = qobject_cast<QMLContactManager *>(component.create());
//    if (qcm) {
//        qWarning() << "Available back ends: " << qcm->availableManagers();
//        qWarning() << "Current Backend: " << qcm->manager();
//        //qWarning() << "They wear a" << person->shoeSize() << "sized shoe";
//    } else {
//
//        qWarning() << "An error occurred";
//        qWarning() << component.errorsString();
//        exit(-1);
//    }
//    QmlGraphicsItem *qcm = qobject_cast<QmlGraphicsItem *>(component.create());
//    if(!qcm){
//                qWarning() << "An error occurred";
//                qWarning() << component.errorsString();
//                exit(-1);
//
//    }
//    qcm->show();

    QWidget *b = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setMargin(0);
    b->setLayout(vbox);

    QmlView *view = new QmlView(b);
    view->setFocusPolicy(Qt::StrongFocus);
    view->setContentResizable(true);
    view->setUrl(QUrl("qrc:/example.qml"));
    view->execute();
    vbox->addWidget(view);
    b->resize(800,480);
    b->show();    

    return app.exec();
}
