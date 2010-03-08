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

#include <qvaluespacesubscriber.h>

#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QUrl>
#include <QDeclarativeView>
#include <qdeclarative.h>

QTM_USE_NAMESPACE

//! [0]
QML_DECLARE_TYPE(QValueSpaceSubscriber);
//! [0]

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget();

private:
    QDeclarativeView *view;
};

MainWidget::MainWidget()
{
    //! [1]
    QML_REGISTER_TYPE(Qt, 4, 6, ValueSpaceSubscriber, QValueSpaceSubscriber);
    //! [1]

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setMargin(0);
    setLayout(vbox);

    view = new QDeclarativeView(this);
    view->setFixedSize(100, 230);
    vbox->addWidget(view);

    //! [2]
    view->setSource(QUrl("qrc:/battery-meter.qml"));
    view->show();
    //! [2]

    QPushButton *quitButton = new QPushButton("Quit");
    vbox->addWidget(quitButton);
    connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWidget mainWidget;
    mainWidget.show();

    return app.exec();
}

#include "main.moc"
