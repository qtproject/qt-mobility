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

#include <QtTest/QtTest>
#include <QApplication>

#include "qtcontacts.h"
#include "qcontactaction.h"
#include "qcontactactiondescriptor.h"

#include "qservicemanager.h"

QTM_USE_NAMESPACE 

class tst_QContactActions : public QObject
{
Q_OBJECT

public:
    tst_QContactActions();
    virtual ~tst_QContactActions();

public slots:
    void init();
    void cleanup();
private slots:
    void testSendEmail();
    void testDescriptor();
    void testDescriptorLessThan();
    void testDescriptorHash();
    void traits();
};

tst_QContactActions::tst_QContactActions()
{
    // set the correct path to look for plugins
    QString path = QApplication::applicationDirPath() + "/dummyplugin/plugins";
    QApplication::addLibraryPath(path);

    // and add the sendemail action to the service framework
    QServiceManager sm;
    qDebug() << sm.findServices();
    if (!sm.removeService("tst_qcontactactions:sendemailaction"))
        qDebug() << " tst_qca: ctor: cleaning up test services failed:" << sm.error();
    if (!sm.addService(QCoreApplication::applicationDirPath() + "/plugins/contacts/xmldata/sendemailactionservice.xml"))
        qDebug() << " tst_qca: ctor: unable to add SendEmail service:" << sm.error();
}

tst_QContactActions::~tst_QContactActions()
{
    QString path = QApplication::applicationDirPath() + "/dummyplugin/plugins";
    QApplication::removeLibraryPath(path);

    QServiceManager sm;
    if (!sm.removeService("tst_qcontactactions:sendemailaction"))
        qDebug() << " tst_qca: ctor: unable to remove service:" << sm.error();
}

void tst_QContactActions::init()
{
}

void tst_QContactActions::cleanup()
{
}

void tst_QContactActions::testSendEmail()
{
    QContact c;
    QContactEmailAddress e;
    e.setEmailAddress("test@nokia.com");
    c.saveDetail(&e);

    QVERIFY(QContactAction::availableActions().contains("SendEmail"));
    QVERIFY(QContactAction::availableActions("tst_qcontactactions:sendemailaction").contains("SendEmail"));

    QList<QContactActionDescriptor> descrs = QContactAction::actionDescriptors();
    bool foundSendEmail = false;
    for (int i = 0; i < descrs.size(); i++) {
        if (descrs.at(i).actionName() == QString("SendEmail")) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(foundSendEmail);

    descrs = QContactAction::actionDescriptors(QString());
    foundSendEmail = false;
    for (int i = 0; i < descrs.size(); i++) {
        QCOMPARE(descrs.at(i).serviceName(), QString("tst_qcontactactions:sendemailaction"));
        if (descrs.at(i).actionName() == QString("SendEmail")) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(foundSendEmail);

    descrs = QContactAction::actionDescriptors(QString());
    foundSendEmail = false;
    for (int i = 0; i < descrs.size(); i++) {
        QCOMPARE(descrs.at(i).serviceName(), QString("tst_qcontactactions:sendemailaction"));
        QCOMPARE(descrs.at(i).implementationVersion(), 1);
        if (descrs.at(i).actionName() == QString("SendEmail")
                && descrs.at(i).serviceName() == QString("tst_qcontactactions:sendemailaction")
                && descrs.at(i).implementationVersion() == 1) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(foundSendEmail);

    descrs = QContactAction::actionDescriptors("SendEmail");
    foundSendEmail = false;
    for (int i = 0; i < descrs.size(); i++) {
        QCOMPARE(descrs.at(i).actionName(), QString("SendEmail"));
        if (descrs.at(i).actionName() == QString("SendEmail")
                && descrs.at(i).serviceName() == QString("tst_qcontactactions:sendemailaction")
                && descrs.at(i).implementationVersion() == 1) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(foundSendEmail);

    descrs = QContactAction::actionDescriptors();
    QContactAction* sendEmail = 0;
    for (int i = 0; i < descrs.size(); i++) {
        if (descrs.at(i).actionName() == QString("SendEmail")
                && descrs.at(i).serviceName() == QString("tst_qcontactactions:sendemailaction")
                && descrs.at(i).implementationVersion() == 1) {
            sendEmail = QContactAction::action(descrs.at(i));
            break;
        }
    }
    QVERIFY(sendEmail);

    QVERIFY(!sendEmail->actionDescriptor().actionName().isEmpty());
    //QVERIFY(!sendEmail->metadata().isNull());
    QVERIFY(!sendEmail->actionDescriptor().serviceName().isEmpty());
    QVERIFY(sendEmail->actionDescriptor().implementationVersion() != -1);
    QVERIFY(sendEmail->actionDescriptor().implementationVersion() != 0);
    //QVERIFY(!sendEmail->contactFilter().isEmpty());
    QVERIFY(sendEmail->isTargetSupported(QContactActionTarget(c,e)));
    QVERIFY(sendEmail->supportedDetails(c).contains(e));
    //QVERIFY(sendEmail->performAction(c, e));
    //QVERIFY(sendEmail->performAction(c));
}

void tst_QContactActions::testDescriptor()
{
    // first, test retrieving an action when no factories are loaded
    QContactAction* invalidAction = QContactAction::action(QContactActionDescriptor());
    QVERIFY(invalidAction == 0); // should be null.

    QContact c;
    QContactEmailAddress e;
    e.setEmailAddress("test@nokia.com");
    c.saveDetail(&e);

    QVERIFY(QContactAction::availableActions().contains("SendEmail"));
    QVERIFY(QContactAction::availableActions("tst_qcontactactions:sendemailaction").contains("SendEmail"));

    QList<QContactActionDescriptor> descrs = QContactAction::actionDescriptors();
    QContactAction* sendEmailAction = 0;
    bool foundSendEmail = false;
    for (int i = 0; i < descrs.size(); i++) {
        if (descrs.at(i).actionName() == QString("SendEmail")) {
            sendEmailAction = QContactAction::action(descrs.at(i));
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(foundSendEmail);
    QVERIFY(sendEmailAction != 0);

    // first, ensure that the descriptor identifies the correct action
    QContactActionDescriptor sendEmailDescriptor;
    sendEmailDescriptor.setActionName(sendEmailAction->actionDescriptor().actionName());
    sendEmailDescriptor.setServiceName(sendEmailAction->actionDescriptor().serviceName());
    sendEmailDescriptor.setImplementationVersion(sendEmailAction->actionDescriptor().implementationVersion());

    // secondly, test operator= and operator==, and copy constructor
    QContactActionDescriptor sendEmailDescriptor2 = sendEmailDescriptor;
    QContactActionDescriptor sendEmailDescriptor3(sendEmailDescriptor2);
    QContactActionDescriptor sendEmailDescriptor4 = sendEmailAction->actionDescriptor();
    QContactActionDescriptor sendEmailDescriptor5 = QContactAction::actionDescriptors(sendEmailDescriptor.actionName()).at(0);
    QContactActionDescriptor sendEmailDescriptor6(sendEmailDescriptor);
    sendEmailDescriptor6 = sendEmailDescriptor5;
    QVERIFY(sendEmailDescriptor2.actionName() == sendEmailDescriptor.actionName());
    QVERIFY(sendEmailDescriptor2.serviceName() == sendEmailDescriptor.serviceName());
    QVERIFY(sendEmailDescriptor2.implementationVersion() == sendEmailDescriptor.implementationVersion());
    QVERIFY(sendEmailDescriptor == sendEmailDescriptor2);
    QVERIFY(sendEmailDescriptor == sendEmailDescriptor3);
    QVERIFY(sendEmailDescriptor == sendEmailDescriptor4);
    QVERIFY(sendEmailDescriptor == sendEmailDescriptor5);
    QVERIFY(sendEmailDescriptor == sendEmailDescriptor6);

    QVERIFY(!sendEmailDescriptor2.isEmpty());
    sendEmailDescriptor2.setActionName("");
    QVERIFY(sendEmailDescriptor2 != sendEmailDescriptor);
    QVERIFY(sendEmailDescriptor2.isEmpty()); // without an action name, cannot uniquely identify an action impl
    sendEmailDescriptor2.setActionName(sendEmailDescriptor.actionName());

    QVERIFY(!sendEmailDescriptor2.isEmpty());
    sendEmailDescriptor2.setServiceName("");
    QVERIFY(sendEmailDescriptor2 != sendEmailDescriptor);
    QVERIFY(sendEmailDescriptor2.isEmpty()); // without a vendor name, cannot uniquely identify an action impl
    sendEmailDescriptor2.setServiceName(sendEmailDescriptor.serviceName());

    QVERIFY(!sendEmailDescriptor2.isEmpty());
    sendEmailDescriptor2.setImplementationVersion(0);
    QVERIFY(sendEmailDescriptor2 != sendEmailDescriptor);
    QVERIFY(sendEmailDescriptor2.isEmpty()); // without a version, cannot uniquely identify an action impl
    sendEmailDescriptor2.setImplementationVersion(sendEmailDescriptor.implementationVersion());

    QVERIFY(sendEmailDescriptor2 == sendEmailDescriptor);

    // XXX TODO: ensure that the caller does not take ownership of the action
    // old test: used to check that the caller DID take ownership...
    //QContactAction *sendEmailAction2 = QContactAction::action(sendEmailAction->actionDescriptor());
    //QContactAction *sendEmailAction3 = QContactAction::action(sendEmailAction->actionDescriptor());
    //QVERIFY(sendEmailAction != sendEmailAction2);
    //QVERIFY(sendEmailAction != sendEmailAction3);
    //QVERIFY(sendEmailAction2 != sendEmailAction3);
    //delete sendEmailAction;
    //delete sendEmailAction2;
    //QVERIFY(sendEmailAction3->actionDescriptor() == sendEmailDescriptor);
    //delete sendEmailAction3;
}

void tst_QContactActions::testDescriptorLessThan()
{
    QContactActionDescriptor qcad1;
    qcad1.setServiceName("a");
    qcad1.setActionName("a");
    qcad1.setImplementationVersion(1);

    QContactActionDescriptor qcad2;
    qcad2.setServiceName("a");
    qcad2.setActionName("a");
    qcad2.setImplementationVersion(2);

    QContactActionDescriptor qcad3;
    qcad3.setServiceName("a");
    qcad3.setActionName("b");
    qcad3.setImplementationVersion(1);

    QContactActionDescriptor qcad4;
    qcad4.setServiceName("b");
    qcad4.setActionName("a");
    qcad4.setImplementationVersion(1);

    QVERIFY(qcad1 < qcad2);
    QVERIFY(qcad2 < qcad3);
    QVERIFY(qcad3 < qcad4);
}

void tst_QContactActions::testDescriptorHash()
{
    QContactActionDescriptor qcad1;
    qcad1.setServiceName("a");
    qcad1.setActionName("a");
    qcad1.setImplementationVersion(1);

    QContactActionDescriptor qcad2;
    qcad2.setServiceName("a");
    qcad2.setActionName("a");
    qcad2.setImplementationVersion(1);

    QContactActionDescriptor qcad3;
    qcad3.setServiceName("a");
    qcad3.setActionName("a");
    qcad3.setImplementationVersion(2);

    QVERIFY(qHash(qcad1) == qHash(qcad2));
    QVERIFY(qHash(qcad1) != qHash(qcad3));
}

void tst_QContactActions::traits()
{
    QCOMPARE(sizeof(QContactActionDescriptor), sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QContactActionDescriptor)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

QTEST_MAIN(tst_QContactActions)
#include "tst_qcontactactions.moc"
