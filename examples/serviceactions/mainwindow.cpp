/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "qmessageserviceaction.h"
#include <QComboBox>
#include <QListView>
#include <QVBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QPointer>
#include <QPushButton>
#include <QDebug>

typedef QPointer<QMessageServiceAction> QMessageServiceActionPtr;

static const QSize WindowGeometry(400,300);
static const QString WindowTitle("QMessageServiceAction Example");

class ComposeSendWidget : public QWidget
{
    Q_OBJECT

public:
    ComposeSendWidget(QMessageServiceAction* service, QWidget* parent = 0);

private slots:
    void composeButtonClicked();

private:
    QMessageServiceAction* m_service;
    QPushButton* m_composeButton;
};

ComposeSendWidget::ComposeSendWidget(QMessageServiceAction* service, QWidget* parent)
:
QWidget(parent),
m_service(service)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* infoLabel = new QLabel("Click to compose a message using the platform messaging client",this);
    infoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(infoLabel);

    m_composeButton = new QPushButton("Compose",this);
    connect(m_composeButton,SIGNAL(clicked(bool)),this,SLOT(composeButtonClicked()));
    layout->addWidget(m_composeButton);
}

void ComposeSendWidget::composeButtonClicked()
{
    qWarning() << "COMPOSE BUTTON CLICKED";
    m_service->compose(QMessage());
}

class RetrieveWidget : public QWidget
{
public:
    RetrieveWidget(QMessageServiceAction* service, QWidget* parent = 0);

private:
    QMessageServiceAction* m_service;
};

RetrieveWidget::RetrieveWidget(QMessageServiceAction* service, QWidget* parent)
:
QWidget(parent),
m_service(service)
{
}

class ShowWidget : public QWidget
{
public:
    ShowWidget(QMessageServiceAction* service, QWidget* parent = 0);

private:
    QMessageServiceAction* m_service;
};

ShowWidget::ShowWidget(QMessageServiceAction* service, QWidget* parent)
:
QWidget(parent),
m_service(service)
{
}

class QueryWidget : public QWidget
{
public:
    QueryWidget(QMessageServiceAction* service, QWidget* parent = 0);

private:
    QMessageServiceAction* m_service;
};

QueryWidget::QueryWidget(QMessageServiceAction* service, QWidget* parent)
:
QWidget(parent),
m_service(service)
{
}

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags f)
:
QMainWindow(parent,f),
m_tabWidget(0)
{
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);

    m_serviceAction = new QMessageServiceAction(this);

    m_tabWidget->addTab(new ComposeSendWidget(m_serviceAction,this),"Compose and Send");
    m_tabWidget->addTab(new RetrieveWidget(m_serviceAction,this),"Retrieve");
    m_tabWidget->addTab(new ShowWidget(m_serviceAction,this),"Show");
    m_tabWidget->addTab(new QueryWidget(m_serviceAction,this),"Query");

    setWindowTitle(WindowTitle);
    resize(WindowGeometry);
}

#include <mainwindow.moc>
