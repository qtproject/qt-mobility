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
#include <QtTest/QtTest>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>

#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>

#include <qdebug.h>

#if defined(Q_WS_MAEMO_6) || defined(Q_WS_MAEMO_5)
#include <stdio.h>
#include <iapconf.h>
#endif


QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QNetworkConfiguration);

class tst_bm_bearer : public QObject
{
    Q_OBJECT

public:
    enum platform {
        QMessaging = 0,
        Native = 1
    };
    
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    
//    void tst_createTime_data();
    void tst_createTime();
    void tst_createTime_2nd();

    void tst_capabilities();
    void tst_defaultconfig();
    void tst_allconfig();
    void tst_updateconfig();
    void tst_updateconfig_2nd();
    void tst_isonline();

    void tst_nc_state();
    void tst_nc_type();
    void tst_nc_purpose();
    void tst_nc_bearerName();
    void tst_nc_identifier();
    void tst_nc_isRoamingAvailable();
    void tst_nc_children();
    void tst_nc_name();
    void tst_nc_isValid();

    void tst_ns_isOpen_data();
    void tst_ns_isOpen();
    void tst_ns_configuration_data();
    void tst_ns_configuration();
    void tst_ns_interface_data();
    void tst_ns_interface();
    void tst_ns_state_data();
    void tst_ns_state();
    void tst_ns_error_data();
    void tst_ns_error();
    void tst_ns_errorString_data();
    void tst_ns_errorString();
    void tst_ns_sessionProperty_data();
    void tst_ns_sessionProperty();
    void tst_ns_bytesWritten_data();
    void tst_ns_bytesWritten();
    void tst_ns_bytesReceived_data();
    void tst_ns_bytesReceived();
    void tst_ns_activeTime_data();
    void tst_ns_activeTime();
    void tst_ns_waitForOpened_data();
    void tst_ns_waitForOpened();
    void tst_ns_open_data();
    void tst_ns_open();
    void tst_ns_close_data();
    void tst_ns_close();
    void tst_ns_stop_data();
    void tst_ns_stop();
//    void tst_ns_migrate_data();
//    void tst_ns_migrate();
//    void tst_ns_ignore_data();
//    void tst_ns_ignore();
//    void tst_ns_accept_data();
//    void tst_ns_accept();
//    void tst_ns_reject_data();
//    void tst_ns_reject();

private:    

};


void tst_bm_bearer::initTestCase()
{
    qRegisterMetaType<QNetworkConfiguration>("QNetworkConfiguration");

}

void tst_bm_bearer::cleanupTestCase()
{

}

void tst_bm_bearer::tst_createTime()
{   
    QBENCHMARK {
            QNetworkConfigurationManager manager;
    }

}

void tst_bm_bearer::tst_createTime_2nd()
{
    QBENCHMARK {
            QNetworkConfigurationManager manager;
    }

}

void tst_bm_bearer::tst_capabilities()
{
    QNetworkConfigurationManager manager;

    QBENCHMARK {
        manager.capabilities();
    }
}

void tst_bm_bearer::tst_defaultconfig()
{
    QNetworkConfigurationManager manager;


    QBENCHMARK {
        manager.defaultConfiguration();
    }
}

void tst_bm_bearer::tst_allconfig()
{
    QNetworkConfigurationManager manager;

    QBENCHMARK {
        manager.allConfigurations();
    }
}

void tst_bm_bearer::tst_updateconfig()
{
    QNetworkConfigurationManager manager;

    QBENCHMARK {
        manager.updateConfigurations();
    }
}

void tst_bm_bearer::tst_updateconfig_2nd()
{
    QNetworkConfigurationManager manager;

    QBENCHMARK_ONCE {
        manager.updateConfigurations();
    }
}

void tst_bm_bearer::tst_isonline()
{
    QNetworkConfigurationManager manager;

    QBENCHMARK {
        manager.isOnline();
    }
}

void tst_bm_bearer::tst_nc_state()
{
    QNetworkConfigurationManager manager;

    QNetworkConfiguration cfg = manager.defaultConfiguration();

    QBENCHMARK {
        cfg.state();
    }
}

void tst_bm_bearer::tst_nc_type()
{
    QNetworkConfigurationManager manager;

    QNetworkConfiguration cfg = manager.defaultConfiguration();

    QBENCHMARK {
        cfg.type();
    }
}

void tst_bm_bearer::tst_nc_purpose()
{
    QNetworkConfigurationManager manager;

    QNetworkConfiguration cfg = manager.defaultConfiguration();

    QBENCHMARK {
        cfg.purpose();
    }
}

void tst_bm_bearer::tst_nc_bearerName()
{
    QNetworkConfigurationManager manager;

    QNetworkConfiguration cfg = manager.defaultConfiguration();

    QBENCHMARK {
        cfg.bearerName();
    }
}

void tst_bm_bearer::tst_nc_identifier()
{
    QNetworkConfigurationManager manager;

    QNetworkConfiguration cfg = manager.defaultConfiguration();

    QBENCHMARK {
        cfg.identifier();
    }
}

void tst_bm_bearer::tst_nc_isRoamingAvailable()
{
    QNetworkConfigurationManager manager;

    QNetworkConfiguration cfg = manager.defaultConfiguration();

    QBENCHMARK {
        cfg.isRoamingAvailable();
    }
}

void tst_bm_bearer::tst_nc_children()
{
    QNetworkConfigurationManager manager;

    QNetworkConfiguration cfg = manager.defaultConfiguration();

    QBENCHMARK {
        cfg.children();
    }
}

void tst_bm_bearer::tst_nc_name()
{
    QNetworkConfigurationManager manager;

    QNetworkConfiguration cfg = manager.defaultConfiguration();

    QBENCHMARK {
        cfg.name();
    }
}

void tst_bm_bearer::tst_nc_isValid()
{
    QNetworkConfigurationManager manager;

    QNetworkConfiguration cfg = manager.defaultConfiguration();

    QBENCHMARK {
        cfg.isValid();
    }
}

void tst_bm_bearer::tst_ns_isOpen_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QTest::newRow(c.name().toAscii()) << c;
    }
}

void tst_bm_bearer::tst_ns_isOpen()
{
    QFETCH(QNetworkConfiguration, config);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.isOpen();
    }
}

void tst_bm_bearer::tst_ns_configuration_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QTest::newRow(c.name().toAscii()) << c;
    }
}

void tst_bm_bearer::tst_ns_configuration()
{
    QFETCH(QNetworkConfiguration, config);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.configuration();
    }
}

void tst_bm_bearer::tst_ns_interface_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QTest::newRow(c.name().toAscii()) << c;
    }
}

void tst_bm_bearer::tst_ns_interface()
{
    QFETCH(QNetworkConfiguration, config);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.interface();
    }
}

void tst_bm_bearer::tst_ns_state_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QTest::newRow(c.name().toAscii()) << c;
    }
}

void tst_bm_bearer::tst_ns_state()
{
    QFETCH(QNetworkConfiguration, config);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.state();
    }
}

void tst_bm_bearer::tst_ns_error_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QTest::newRow(c.name().toAscii()) << c;
    }
}

void tst_bm_bearer::tst_ns_error()
{
    QFETCH(QNetworkConfiguration, config);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.error();
    }
}

void tst_bm_bearer::tst_ns_errorString_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QTest::newRow(c.name().toAscii()) << c;
    }
}

void tst_bm_bearer::tst_ns_errorString()
{
    QFETCH(QNetworkConfiguration, config);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.errorString();
    }
}

void tst_bm_bearer::tst_ns_bytesWritten_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QTest::newRow(c.name().toAscii()) << c;
    }
}

void tst_bm_bearer::tst_ns_bytesWritten()
{
    QFETCH(QNetworkConfiguration, config);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.bytesWritten();
    }
}

void tst_bm_bearer::tst_ns_bytesReceived_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QTest::newRow(c.name().toAscii()) << c;
    }
}

void tst_bm_bearer::tst_ns_bytesReceived()
{
    QFETCH(QNetworkConfiguration, config);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.bytesReceived();
    }
}

void tst_bm_bearer::tst_ns_activeTime_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QTest::newRow(c.name().toAscii()) << c;
    }
}

void tst_bm_bearer::tst_ns_activeTime()
{
    QFETCH(QNetworkConfiguration, config);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.activeTime();
    }
}

void tst_bm_bearer::tst_ns_waitForOpened_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QTest::newRow(c.name().toAscii()) << c;
    }
}

void tst_bm_bearer::tst_ns_waitForOpened()
{
    QFETCH(QNetworkConfiguration, config);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.waitForOpened();
    }
}

void tst_bm_bearer::tst_ns_open_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QTest::newRow(c.name().toAscii()) << c;
    }
}

void tst_bm_bearer::tst_ns_open()
{
    QFETCH(QNetworkConfiguration, config);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.open();
    }
}

void tst_bm_bearer::tst_ns_close_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QTest::newRow(c.name().toAscii()) << c;
    }
}

void tst_bm_bearer::tst_ns_close()
{
    QFETCH(QNetworkConfiguration, config);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.close();
    }
}

void tst_bm_bearer::tst_ns_stop_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QTest::newRow(c.name().toAscii()) << c;
    }    
}

void tst_bm_bearer::tst_ns_stop()
{
    QFETCH(QNetworkConfiguration, config);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.stop();
    }    
}

//void tst_bm_bearer::tst_ns_migrate_data()
//{
//  qDebug() << "Start migrate_data";
//    QTest::addColumn<QNetworkConfiguration>("config");
//
//    QNetworkConfigurationManager manager;
//    QList<QNetworkConfiguration> list = manager.allConfigurations();
//
//    while(!list.isEmpty()){
//        QNetworkConfiguration c = list.takeFirst();
//        QTest::newRow(c.name().toAscii()) << c;
//    }
//    qDebug() << "Done migrate_data";
//}
//
//void tst_bm_bearer::tst_ns_migrate()
//{
//    QFETCH(QNetworkConfiguration, config);
//
//    QNetworkConfigurationManager manager;
//    if(!(manager.capabilities() & QNetworkConfigurationManager::ApplicationLevelRoaming)) {
//        return;
//    }
//
//    qDebug() << "Start migrate";
//    
//    QNetworkSession ns(config);
//
//    qDebug() << "Fire!";
//    QBENCHMARK {
//        ns.migrate();
//    }
//    
//    qDebug() << "Done migrate";
//}
//
//void tst_bm_bearer::tst_ns_ignore_data()
//{
//    QTest::addColumn<QNetworkConfiguration>("config");
//    QNetworkConfigurationManager manager;
//
//    QList<QNetworkConfiguration> list = manager.allConfigurations();
//
//    while(!list.isEmpty()){
//        QNetworkConfiguration c = list.takeFirst();
//        QTest::newRow(c.name().toAscii()) << c;
//    }
//}
//
//void tst_bm_bearer::tst_ns_ignore()
//{
//    QFETCH(QNetworkConfiguration, config);
//    QNetworkConfigurationManager manager;
//    if(!(manager.capabilities() & QNetworkConfigurationManager::ApplicationLevelRoaming)) {
//        return;
//    }
//
//    QNetworkSession ns(config);
//
//    QBENCHMARK {
//        ns.ignore();
//    }
//}
//
//void tst_bm_bearer::tst_ns_accept_data()
//{
//    QTest::addColumn<QNetworkConfiguration>("config");
//
//    QNetworkConfigurationManager manager;
//    QList<QNetworkConfiguration> list = manager.allConfigurations();
//
//    while(!list.isEmpty()){
//        QNetworkConfiguration c = list.takeFirst();
//        QTest::newRow(c.name().toAscii()) << c;
//    }
//}
//
//void tst_bm_bearer::tst_ns_accept()
//{
//    QFETCH(QNetworkConfiguration, config);
//    QNetworkConfigurationManager manager;
//    if(!(manager.capabilities() & QNetworkConfigurationManager::ApplicationLevelRoaming)) {
//        return;
//    }
//
//    QNetworkSession ns(config);
//
//    QBENCHMARK {
//        ns.accept();
//    }
// }
//
//void tst_bm_bearer::tst_ns_reject_data()
//{
//    QTest::addColumn<QNetworkConfiguration>("config");
//
//    QNetworkConfigurationManager manager;
//    QList<QNetworkConfiguration> list = manager.allConfigurations();
//
//    while(!list.isEmpty()){
//        QNetworkConfiguration c = list.takeFirst();
//        QTest::newRow(c.name().toAscii()) << c;
//    }
//}
//
//void tst_bm_bearer::tst_ns_reject()
//{
//    QFETCH(QNetworkConfiguration, config);
//    QNetworkConfigurationManager manager;
//    if(!(manager.capabilities() & QNetworkConfigurationManager::ApplicationLevelRoaming)) {
//        return;
//    }
//
//    QNetworkSession ns(config);
//
//    QBENCHMARK {
//        ns.reject();
//    }
//}

void tst_bm_bearer::tst_ns_sessionProperty_data()
{
    QTest::addColumn<QNetworkConfiguration>("config");
    QTest::addColumn<QString>("key");

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> list = manager.allConfigurations();

    while(!list.isEmpty()){
        QNetworkConfiguration c = list.takeFirst();
        QStringList kl;

        kl += "ActiveConfiguration";
        kl += "UserChoiceConfiguration";
        kl += "ConnectInBackground";
        while(!kl.isEmpty()) {
            QString key = kl.takeFirst();
            QString name = c.name() + "_" +  key;
            QTest::newRow(name.toAscii()) << c << key;
        }
    }
}

void tst_bm_bearer::tst_ns_sessionProperty()
{
    QFETCH(QNetworkConfiguration, config);
    QFETCH(QString, key);

    QNetworkSession ns(config);

    QBENCHMARK {
        ns.sessionProperty(key);
    }
}




int main(int argc, char **argv){

    QApplication app(argc, argv);
    
    tst_bm_bearer test1;
    QTest::qExec(&test1, argc, argv);
    
}
//QTEST_MAIN(tst_messaging);


#include "tst_bm_bearer.moc"

