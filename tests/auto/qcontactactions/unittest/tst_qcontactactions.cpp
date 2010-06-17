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
    void testDescriptorHash();
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
    //QVERIFY(multiActionOneDescriptor.implementationVersion() == multiActionTwoDescriptor.implementationVersion());// XXX TODO FIXME after QTMOBILITY-316 is fixed
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
