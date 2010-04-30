/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include <qvaluespacesubscriber.h>

#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QUrl>
#include <QDeclarativeView>
#include <qdeclarative.h>
#include <QtDeclarative>

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
    //! [1]
    qmlRegisterType<QValueSpaceSubscriber>("Qt", 4, 6, "ValueSpaceSubscriber");
    //! [1]

    QApplication app(argc, argv);

    MainWidget mainWidget;
    mainWidget.show();

    return app.exec();
}

#include "main.moc"
