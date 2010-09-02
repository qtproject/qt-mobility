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
    telephoycalllist = new QtMobility::QTelephonyCallList(this);
    connect(telephoycalllist
            , SIGNAL(activeCallAdded(QTelephonyCallInfo))
            , SLOT(onActiveCallAdded(QTelephonyCallInfo)));
    connect(telephoycalllist
            , SIGNAL(activeCallRemoved(QTelephonyCallInfo))
            , SLOT(onActiveCallRemoved(QTelephonyCallInfo)));
    connect(telephoycalllist
            , SIGNAL(activeCallStatusChanged(QTelephonyCallInfo))
            , SLOT(onActiveCallStatusChanged(QTelephonyCallInfo)));
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow::~MainWindow()";
    if(telephoycalllist != 0){
        delete telephoycalllist;
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
    else if(call.status() == QTelephony::Incomming)
        val += "-Incomming";
    else if(call.status() == QTelephony::OnHold)
        val += "-OnHold";

    val += " ";

    if(call.type() == QTelephony::Text)
        val += "Text";
    else if(call.type() == QTelephony::Data)
        val += "Data";
    else if(call.type() == QTelephony::Video)
        val += "Video";
    else if(call.type() == QTelephony::Voice)
        val += "Voice";
    else if(call.type() == QTelephony::Other)
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
