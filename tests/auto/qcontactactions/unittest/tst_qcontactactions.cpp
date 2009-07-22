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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#define QT_STATICPLUGIN
#include <QtTest/QtTest>
#include <QApplication>

#include "qtcontacts.h"
#include "qcontactmanager_p.h"
#include "qcontactaction.h"
#include "qcontactactionfactory.h"


//TESTED_CLASS=
//TESTED_FILES=

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
};

/* Test a static factory as well */
class DummyStaticActionFactory : public QContactActionFactory
{
    Q_OBJECT
    Q_INTERFACES(QContactActionFactory)

public:
    DummyStaticActionFactory() {}
    ~DummyStaticActionFactory() {}

    QString name() const
    {
        return QString("dummystaticactionfactory");
    }

    QList<QContactActionFactory::ActionDescriptor> actionDescriptors() const
    {
        return QList<QContactActionFactory::ActionDescriptor>();
    }

    QContactAction* instance(const QContactActionFactory::ActionDescriptor&) const
    {
        return 0;
    }
};

/* Statically import it (and a duplicate copy of it, purely for testing purposes) */
Q_EXPORT_PLUGIN2(contacts_testdummystaticactionfactory, DummyStaticActionFactory);
Q_IMPORT_PLUGIN(contacts_testdummystaticactionfactory);
Q_EXPORT_PLUGIN2(contacts_testdummystaticactionfactorycopy, DummyStaticActionFactory);
Q_IMPORT_PLUGIN(contacts_testdummystaticactionfactorycopy);

tst_QContactActions::tst_QContactActions()
{
}

tst_QContactActions::~tst_QContactActions()
{
}

void tst_QContactActions::init()
{
}

void tst_QContactActions::cleanup()
{
}

void tst_QContactActions::testSendEmail()
{
    // set the correct path to look for plugins
    QString path = QApplication::applicationDirPath() + "/dummyplugin/plugins/";
    QApplication::addLibraryPath(path);

    QContact c;
    QContactEmailAddress e;
    e.setEmailAddress("test@nokia.com");
    c.saveDetail(&e);

    QVERIFY(QContactManager::availableActions().contains("SendEmail"));
    QVERIFY(QContactManager::availableActions("Test").contains("SendEmail"));
    QVERIFY(QContactManager::availableActions("Test", 1).contains("SendEmail"));
    QVERIFY(QContactManager::availableActions(QString(), -200).contains("SendEmail"));

    QList<QContactAction*> impls = QContactManager::actions();
    bool foundSendEmail = false;
    for (int i = 0; i < impls.size(); i++) {
        if (impls.at(i)->actionName() == QString("SendEmail")) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(foundSendEmail);

    impls = QContactManager::actions(QString(), "Test");
    foundSendEmail = false;
    for (int i = 0; i < impls.size(); i++) {
        QCOMPARE(impls.at(i)->vendor(), QString("Test"));
        if (impls.at(i)->actionName() == QString("SendEmail")) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(foundSendEmail);

    impls = QContactManager::actions(QString(), "Test", 1);
    foundSendEmail = false;
    for (int i = 0; i < impls.size(); i++) {
        QCOMPARE(impls.at(i)->vendor(), QString("Test"));
        QCOMPARE(impls.at(i)->implementationVersion(), 1);
        if (impls.at(i)->actionName() == QString("SendEmail")
                && impls.at(i)->vendor() == QString("Test")
                && impls.at(i)->implementationVersion() == 1) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(foundSendEmail);

    impls = QContactManager::actions(QString(), "Test", -200); // shouldn't find any with this impl.version.
    foundSendEmail = false;
    for (int i = 0; i < impls.size(); i++) {
        QCOMPARE(impls.at(i)->vendor(), QString("Test"));
        if (impls.at(i)->actionName() == QString("SendEmail")
                && impls.at(i)->vendor() == QString("Test")
                && impls.at(i)->implementationVersion() == 1) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(!foundSendEmail);

    impls = QContactManager::actions(QString(), QString(), -200); // ignores implementation Version if empty vendor
    foundSendEmail = false;
    for (int i = 0; i < impls.size(); i++) {
        if (impls.at(i)->actionName() == QString("SendEmail")
                && impls.at(i)->vendor() == QString("Test")
                && impls.at(i)->implementationVersion() == 1) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(foundSendEmail);

    impls = QContactManager::actions(QString(), "Nonexistent");
    foundSendEmail = false;
    for (int i = 0; i < impls.size(); i++) {
        QCOMPARE(impls.at(i)->vendor(), QString("Nonexistent"));
        if (impls.at(i)->actionName() == QString("SendEmail")
                && impls.at(i)->vendor() == QString("Test")
                && impls.at(i)->implementationVersion() == 1) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(!foundSendEmail);

    impls = QContactManager::actions("SendEmail");
    foundSendEmail = false;
    for (int i = 0; i < impls.size(); i++) {
        QCOMPARE(impls.at(i)->actionName(), QString("SendEmail"));
        if (impls.at(i)->actionName() == QString("SendEmail")
                && impls.at(i)->vendor() == QString("Test")
                && impls.at(i)->implementationVersion() == 1) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(foundSendEmail);

    impls = QContactManager::actions("SendEmail", "Test");
    foundSendEmail = false;
    for (int i = 0; i < impls.size(); i++) {
        QCOMPARE(impls.at(i)->actionName(), QString("SendEmail"));
        QCOMPARE(impls.at(i)->vendor(), QString("Test"));
        if (impls.at(i)->actionName() == QString("SendEmail")
                && impls.at(i)->vendor() == QString("Test")
                && impls.at(i)->implementationVersion() == 1) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(foundSendEmail);

    impls = QContactManager::actions("SendEmail", "Test", 1);
    foundSendEmail = false;
    for (int i = 0; i < impls.size(); i++) {
        QCOMPARE(impls.at(i)->actionName(), QString("SendEmail"));
        QCOMPARE(impls.at(i)->vendor(), QString("Test"));
        QCOMPARE(impls.at(i)->implementationVersion(), 1);
        if (impls.at(i)->actionName() == QString("SendEmail")
                && impls.at(i)->vendor() == QString("Test")
                && impls.at(i)->implementationVersion() == 1) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(foundSendEmail);

    impls = QContactManager::actions("SendEmail", "Test", -200); // shouldn't find any with this impl.version.
    foundSendEmail = false;
    for (int i = 0; i < impls.size(); i++) {
        QCOMPARE(impls.at(i)->actionName(), QString("SendEmail"));
        QCOMPARE(impls.at(i)->vendor(), QString("Test"));
        if (impls.at(i)->actionName() == QString("SendEmail")
                && impls.at(i)->vendor() == QString("Test")
                && impls.at(i)->implementationVersion() == 1) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(!foundSendEmail);

    impls = QContactManager::actions("SendEmail", QString(), -200); // ignores implementation Version if empty vendor
    foundSendEmail = false;
    for (int i = 0; i < impls.size(); i++) {
        QCOMPARE(impls.at(i)->actionName(), QString("SendEmail"));
        if (impls.at(i)->actionName() == QString("SendEmail")
                && impls.at(i)->vendor() == QString("Test")
                && impls.at(i)->implementationVersion() == 1) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(foundSendEmail);

    impls = QContactManager::actions("SendEmail", "Nonexistent");
    foundSendEmail = false;
    for (int i = 0; i < impls.size(); i++) {
        QCOMPARE(impls.at(i)->actionName(), QString("SendEmail"));
        QCOMPARE(impls.at(i)->vendor(), QString("Nonexistent"));
        if (impls.at(i)->actionName() == QString("SendEmail")
                && impls.at(i)->vendor() == QString("Test")
                && impls.at(i)->implementationVersion() == 1) {
            foundSendEmail = true;
            break;
        }
    }
    QVERIFY(!foundSendEmail);

    impls = QContactManager::actions();
    QContactAction* sendEmail = 0;
    for (int i = 0; i < impls.size(); i++) {
        if (impls.at(i)->actionName() == QString("SendEmail")
                && impls.at(i)->vendor() == QString("Test")
                && impls.at(i)->implementationVersion() == 1) {
            sendEmail = impls.at(i);
            break;
        }
    }
    QVERIFY(sendEmail);

    QVERIFY(!sendEmail->actionName().isEmpty());
    //QVERIFY(!sendEmail->metadata().isNull());
    QVERIFY(!sendEmail->vendor().isEmpty());
    QVERIFY(sendEmail->implementationVersion() != -1);
    QVERIFY(sendEmail->implementationVersion() != 0);
    //QVERIFY(!sendEmail->contactFilter().isEmpty());
    QVERIFY(sendEmail->supportsDetail(e));
    QVERIFY(sendEmail->supportedDetails(c).contains(e));
    //QVERIFY(sendEmail->performAction(c, e));
    //QVERIFY(sendEmail->performAction(c));
}

QTEST_MAIN(tst_QContactActions)
#include "tst_qcontactactions.moc"
