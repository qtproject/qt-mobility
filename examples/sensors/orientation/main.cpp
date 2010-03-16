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

#include <QtCore>
#include <QtGui>
#include <QtDeclarative>
#include <qaccelerometer.h>
#include <qorientationsensor.h>

QTM_USE_NAMESPACE

QML_DECLARE_TYPE(QOrientationSensor)
QML_DEFINE_TYPE(Qt, 4, 6, OrientationSensor, QOrientationSensor)

QML_DECLARE_TYPE(QSensorReading)
QML_DEFINE_NOCREATE_TYPE(QSensorReading)

QML_DECLARE_TYPE(QOrientationReading)
//### while we don't want to explicitly create this type in QML, we need to define it so
//    we can specify enum values like OrientationReading.LeftUp
QML_DEFINE_TYPE(Qt, 4, 6, OrientationReading, QOrientationReading)

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget();

private:
    QmlView *view;
};

MainWidget::MainWidget()
{
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setMargin(0);
    setLayout(vbox);

    view = new QmlView(this);
    vbox->addWidget(view);

    view->setUrl(QUrl("qrc:/orientation.qml"));
    view->execute();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWidget mainWidget;
    mainWidget.show();

    return app.exec();
}

#include "main.moc"

