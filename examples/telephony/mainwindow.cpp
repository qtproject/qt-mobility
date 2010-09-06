#include "mainwindow.h"
#include <qmobilityglobal.h>
#include "ui_mainwindow.h"
#include <QtCore/QDebug>
#include <qtelephony.h>
#include <qtelephonycalllist.h>

using namespace QtMobility;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << "MainWindow::MainWindow(QWidget *parent)";
    ui->setupUi(this);
    this->setWindowTitle("Call Monitor");
    ui->lstRxMsg->setModel(&m_rxDBusMsg);
    telephonycalllist = new QtMobility::QTelephonyCallList(this);
    connect(telephonycalllist
        , SIGNAL(activeCallAdded(QTelephonyCallInfo))
        , SLOT(onActiveCallAdded(QTelephonyCallInfo)));
    connect(telephonycalllist
        , SIGNAL(activeCallStatusChanged(QTelephonyCallInfo))
        , SLOT(onActiveCallStatusChanged(QTelephonyCallInfo)));
    connect(telephonycalllist
        , SIGNAL(activeCallRemoved(QTelephonyCallInfo))
        , SLOT(onActiveCallRemoved(QTelephonyCallInfo)));
    connect(telephonycalllist
        , SIGNAL(activeCallCountChanged())
        , SLOT(onActiveCallCountChanged()));
    connect(ui->pushButtonClearList
        , SIGNAL(released())
        , SLOT(onClearList()));
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow::~MainWindow()";
    if(telephonycalllist != 0){
        delete telephonycalllist;
    }
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::addListEntry(const QString& event, const QTelephonyCallInfo& call)
{
    QString val = call.remotePartyIdentifier();
    val += " " + event;
    val += " ";

    if(call.status() == QTelephony::Idle)
        val += "-Idle";
    else if(call.status() == QTelephony::Dialing)
        val += "-Dialing";
    else if(call.status() == QTelephony::Alerting)
        val += "-Alerting";
    else if(call.status() == QTelephony::Connected)
        val += "-Connected";
    else if(call.status() == QTelephony::Disconnecting)
        val += "-Disconnecting";
    else if(call.status() == QTelephony::Incoming)
        val += "-Incoming";
    else if(call.status() == QTelephony::OnHold)
        val += "-OnHold";

    val += " ";

    unsigned int type = call.type();
    qDebug() << "type : "  << type;
    if(type & QTelephony::Text)
        val += "Text";
    if(type & QTelephony::Data)
        val += "Data";
    if(type & QTelephony::Video)
        val += "Video";
    if(type & QTelephony::Voice)
        val += "Voice";
    if(type & QTelephony::Other)
        val += "Other";

    QStringList vl = m_rxDBusMsg.stringList();
    vl.append(val);
    m_rxDBusMsg.setStringList(vl);
    ui->lstRxMsg->scrollToBottom();

}

void MainWindow::onActiveCallAdded(const QTelephonyCallInfo& call)
{
    addListEntry("call added: ", call);
}

void MainWindow::onActiveCallRemoved(const QTelephonyCallInfo& call)
{
    addListEntry("call removed: ", call);
}

void MainWindow::onActiveCallStatusChanged(const QTelephonyCallInfo& call)
{
    addListEntry("status changed: ", call);
}

void MainWindow::onActiveCallCountChanged()
{
    qDebug() << "active Call count changed";
}

void MainWindow::onClearList()
{
    QStringList vl = m_rxDBusMsg.stringList();
    vl.clear();
    m_rxDBusMsg.setStringList(vl);
}
