/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//TESTED_COMPONENT=src/contacts

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
    void testDescriptorHash();
    void testTarget();
    void traits();
};

tst_QContactActions::tst_QContactActions()
{
    // set the correct path to look for plugins
    QString path = QApplication::applicationDirPath() + "/dummyplugin/plugins";
    QApplication::addLibraryPath(path);

    // and add the sendemail + call actions to the service framework
    QServiceManager sm;

    // clean up any actions/services.
    QStringList allServices = sm.findServices();
    foreach(const QString& serv, allServices) {
        if (serv.startsWith("tst_qcontact")) {
            if (!sm.removeService(serv)) {
                qDebug() << " tst_qca: ctor: cleaning up test service" << serv << "failed:" << sm.error();
            }
        }
    }

    if (!sm.addService(QCoreApplication::applicationDirPath() + "/plugins/contacts/xmldata/sendemailactionservice.xml"))
        qDebug() << " tst_qca: ctor: unable to add SendEmail service:" << sm.error();
    if (!sm.addService(QCoreApplication::applicationDirPath() + "/plugins/contacts/xmldata/multiactionservice.xml"))
        qDebug() << " tst_qca: ctor: unable to add MultiAction service:" << sm.error();
}

tst_QContactActions::~tst_QContactActions()
{
    QString path = QApplication::applicationDirPath() + "/dummyplugin/plugins";
    QApplication::removeLibraryPath(path);

    // clean up any actions/services.
    QServiceManager sm;
    QStringList allServices = sm.findServices();
    foreach(const QString& serv, allServices) {
        if (serv.startsWith("tst_qcontact")) {
            if (!sm.removeService(serv)) {
                qDebug() << " tst_qca: ctor: cleaning up test service" << serv << "failed:" << sm.error();
            }
        }
    }
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
}

void tst_QContactActions::testDescriptor()
{
    // first, test retrieving an invalid action
    QContactAction* invalidAction = QContactAction::action(QContactActionDescriptor());
    QVERIFY(invalidAction == 0); // should be null.

    QContact c;
    QContactEmailAddress e;
    e.setEmailAddress("test@nokia.com");
    c.saveDetail(&e);
    QContactPhoneNumber p;
    p.setNumber("12345");
    c.saveDetail(&p);

    QVERIFY(QContactAction::availableActions().contains("SendEmail"));
    QVERIFY(QContactAction::availableActions("tst_qcontactactions:sendemailaction").contains("SendEmail"));

    QContactActionDescriptor sendEmailDescriptor;
    QContactActionDescriptor multiActionOneDescriptor;
    QContactActionDescriptor multiActionTwoDescriptor;

    QList<QContactActionDescriptor> descrs = QContactAction::actionDescriptors();
    QContactAction* sendEmailAction = 0;
    for (int i = 0; i < descrs.size(); i++) {
        QContactActionDescriptor temp = descrs.at(i);
        if (temp.actionName() == QString("SendEmail")) {
            sendEmailAction = QContactAction::action(temp);
            QVERIFY(c.availableActions().contains(temp)); // has an email address, so should be available
            QVERIFY(temp.supportsContact(c));
            sendEmailDescriptor = temp;
        } else if (temp.actionName() == QString("call")) {
            if (temp.metaData(QString(QLatin1String("Provider"))) == QString(QLatin1String("sip"))) {
                multiActionOneDescriptor = temp;
            } else {
                multiActionTwoDescriptor = temp;
            }
        }
    }

    QVERIFY(sendEmailDescriptor.isValid());
    QVERIFY(multiActionOneDescriptor.isValid());
    QVERIFY(multiActionTwoDescriptor.isValid());

    QVERIFY(sendEmailAction != 0);
    delete sendEmailAction;

    // now test equivalence.  The send email action descriptor should
    // have a different action name to both multi action one and two.
    QVERIFY(sendEmailDescriptor.actionName() != multiActionOneDescriptor.actionName());
    QVERIFY(sendEmailDescriptor.actionName() != multiActionTwoDescriptor.actionName());
    QVERIFY(sendEmailDescriptor != multiActionOneDescriptor);
    QVERIFY(sendEmailDescriptor != multiActionTwoDescriptor);

    // multi action one and two should have the same action name, service
    // name and implementation (minor) version.  BUT they have different
    // implementations (Provider is different) so they should NOT be equal.
    QVERIFY(multiActionOneDescriptor.actionName() == multiActionTwoDescriptor.actionName());
    QVERIFY(multiActionOneDescriptor.serviceName() == multiActionTwoDescriptor.serviceName());
    QVERIFY(multiActionOneDescriptor.implementationVersion() == multiActionTwoDescriptor.implementationVersion());
    QVERIFY(multiActionOneDescriptor != multiActionTwoDescriptor);

    // verify that the meta data is reported correctly
    QVERIFY(multiActionOneDescriptor.metaData("Provider") != multiActionTwoDescriptor.metaData("Provider"));
}

void tst_QContactActions::testDescriptorHash()
{
    QContactActionDescriptor qcad1, qcad2, qcad3;
    QList<QContactActionDescriptor> alldes = QContactAction::actionDescriptors();

    if (alldes.size() > 1) {
        qcad1 = alldes.at(0);
        qcad2 = alldes.at(1);
        QVERIFY(qHash(qcad1) != qHash(qcad2));
        qcad3 = alldes.at(0);
        QVERIFY(qHash(qcad1) == qHash(qcad3));
    }

    if (alldes.size() > 2) {
        qcad3 = alldes.at(2);
        QVERIFY(qHash(qcad1) != qHash(qcad2));
        QVERIFY(qHash(qcad2) != qHash(qcad3));
        QVERIFY(qHash(qcad1) != qHash(qcad3));
    }
}

void tst_QContactActions::testTarget()
{
    // first, create a contact with some details
    QContact c1;
    QContactName n1;
    n1.setFirstName("test");
    n1.setLastName("contact");
    c1.saveDetail(&n1);
    QContactPhoneNumber p1;
    p1.setNumber("12345");
    c1.saveDetail(&p1);
    QContactEmailAddress e1;
    e1.setEmailAddress("test@example.com");
    c1.saveDetail(&e1);

    QList<QContactDetail> dl1;
    dl1 << p1 << e1;

    QContactActionTarget t1;          // default ctor
    QContactActionTarget t2(c1);      // "whole contact" target
    QContactActionTarget t3(c1, e1);  // "specific detail" target
    QContactActionTarget t4(c1, dl1); // "detail list" target

    QCOMPARE(t2.contact(), c1);
    QCOMPARE(t3.contact(), c1);
    QCOMPARE(t4.contact(), c1);

    QCOMPARE(t2.details(), QList<QContactDetail>());
    QCOMPARE(t3.details(), QList<QContactDetail>() << e1);
    QCOMPARE(t4.details(), dl1);

    QVERIFY(t1 != t2);
    QVERIFY(t1 != t3);
    QVERIFY(t1 != t4);
    QVERIFY(t2 != t3);
    QVERIFY(t2 != t4);
    QVERIFY(t3 != t4);
    QVERIFY(qHash(t2) != qHash(t3));
    QVERIFY(qHash(t2) != qHash(t4));
    QVERIFY(qHash(t3) != qHash(t4));

    QVERIFY(!t1.isValid());
    QVERIFY(t2.isValid());
    QVERIFY(t3.isValid());
    QVERIFY(t4.isValid());

    t1.setContact(c1);
    t1.setDetails(dl1);
    QVERIFY(t1.isValid());
    QVERIFY(t1 == t4);

    t2 = t1;
    QVERIFY(t2.isValid()); // check that assignment operator doesn't destroy validity.
    QVERIFY(t1 == t2);
    QVERIFY(t2 == t4);
    QVERIFY(t2 != t3);
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
