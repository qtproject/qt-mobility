/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <QMetaObject>
#include <QDateTime>
#include <QMap>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QDeclarativeListProperty>


#include "qdeclarativecontactmodel_p.h"
#include "qdeclarativecontactsortorder_p.h"
#include "qdeclarativecontact_p.h" 
#include "qdeclarativecontactfilters_p.h" 
#include "qdeclarativecontactdetails_p.h" 
#include "qdeclarativecontactfetchhint_p.h" 
#include "qdeclarativecontactrelationship_p.h" 
#include "qdeclarativecontactrelationshipmodel_p.h" 

// Eventually these will make it into qtestcase.h
// but we might need to tweak the timeout values here.
#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

#ifndef QTRY_VERIFY
#define QTRY_VERIFY(__expr) \
    do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)
#endif

#ifndef QTRY_WAIT
#define QTRY_WAIT(code, __expr) \
    do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            do { code } while(0); \
            QTest::qWait(__step); \
        } \
    } while(0)
#endif


#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif


QTM_USE_NAMESPACE


class tst_QDeclarativeContact : public QObject
{
    Q_OBJECT

public:
    tst_QDeclarativeContact();
    virtual ~tst_QDeclarativeContact();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

private slots:

    void contact();
    void contactModel();
    void filter();
    void details();
    void relationship();
    void relationshipModel();
    void sortOrder();
    void construction();
    void construction_data();


private:
    QObject* createComponent(const QString& componentString);

private:
    // Engine is needed for instantiating declarative components
    QDeclarativeEngine engine;
};

tst_QDeclarativeContact::tst_QDeclarativeContact() {}
tst_QDeclarativeContact::~tst_QDeclarativeContact() {}

void tst_QDeclarativeContact::initTestCase()
{
}

void tst_QDeclarativeContact::cleanupTestCase()
{
}

void tst_QDeclarativeContact::init()
{

}
void tst_QDeclarativeContact::construction()
{
    QFETCH(QString, componentString);
    QFETCH(QString, expectedClassName);
    QFETCH(bool, shouldSucceed);

    QObject* obj = createComponent(componentString);

    if (shouldSucceed) {
        if (obj == 0) {
            qWarning("--------- ------------- ------------- ---------------------- ------------ ");
            qWarning("--------- could not instantiate components from contact   -------------- ");
            qWarning("--------- declarative plugin. make sure it is built and found ------------");
            qWarning(" ---------under {QTDIR}/imports, or c:/sys/bin on Symbian     ----------- ");
            qWarning("--------- ------------- ------------- ---------------------- ------------ ");
        }
        QVERIFY(obj != 0);
        QCOMPARE(obj->metaObject()->className(), expectedClassName.toAscii().constData());
    } else {
        QVERIFY(obj == 0);
    }
    delete obj;
}

void tst_QDeclarativeContact::construction_data()
{
    QTest::addColumn<QString>("expectedClassName");
    QTest::addColumn<QString>("componentString");
    QTest::addColumn<bool>("shouldSucceed");
    // ContactModel
    QTest::newRow("ContactModel: No properties") <<  "QDeclarativeContactModel" << "import Qt 4.7 \n import QtMobility.contacts 1.1 \n ContactModel {}" << true;
    QTest::newRow("ContactModel: Only id property") << "QDeclarativeContactModel" << "import Qt 4.7 \n import QtMobility.contacts 1.1 \n ContactModel {id: contactModelId}" << true;
    QTest::newRow("ContactModel: Valuetype properties") << "QDeclarativeContactModel" << "import Qt 4.7 \n import QtMobility.contacts 1.1 \n ContactModel {id: contactModelId; manager:'memory'}" << true;
    QTest::newRow("ContactModel: With filter") << "QDeclarativeContactModel" << "import Qt 4.7 \n import QtMobility.contacts 1.1 \n ContactModel {id: contactModelId; filter: DetailFilter{} }" << true;
    QTest::newRow("ContactModel: With fetchHint") << "QDeclarativeContactModel" << "import Qt 4.7 \n import QtMobility.contacts 1.1 \n ContactModel {id: contactModelId; fetchHint:FetchHint {id:hint; optimizationHints:FetchHint.AllRequired} }" << true;
}
void tst_QDeclarativeContact::contact()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/contact.qml"));

    QDeclarativeContact *contact = qobject_cast<QDeclarativeContact*>(component.create());
    QVERIFY(contact != 0); 
    //other qverfies
    delete contact;
}
void tst_QDeclarativeContact::contactModel()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/contactmodel.qml"));

    QDeclarativeContactModel *contactModel = qobject_cast<QDeclarativeContactModel*>(component.create());
    QVERIFY(contactModel != 0); 
    //other qverfies
    delete contactModel;
}
void tst_QDeclarativeContact::filter()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/filter.qml"));

    QDeclarativeContactFilter *filter = qobject_cast<QDeclarativeContactFilter*>(component.create());
    QVERIFY(filter != 0); 
    //other qverfies
    delete filter;
}
void tst_QDeclarativeContact::details()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/details.qml"));

    QDeclarativeContactDetail *detail =  qobject_cast<QDeclarativeContactDetail*>(component.create());
    QVERIFY(detail != 0); 
    //other qverfies
    delete detail;
}
void tst_QDeclarativeContact::relationship()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/relationship.qml"));

    QDeclarativeContactRelationship *relationship = qobject_cast<QDeclarativeContactRelationship*>(component.create());
    QVERIFY(relationship != 0); 
    //other qverfies
    delete relationship;
}
void tst_QDeclarativeContact::relationshipModel()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/relationshipmodel.qml"));

    QDeclarativeContactRelationshipModel *model = qobject_cast<QDeclarativeContactRelationshipModel*>(component.create());
    QVERIFY(model != 0); 
    //other qverfies
    delete model;
}
void tst_QDeclarativeContact::sortOrder()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/sortorder.qml"));

    QDeclarativeContactSortOrder *sortOrder = qobject_cast<QDeclarativeContactSortOrder*>(component.create());
    QVERIFY(sortOrder != 0); 
    //other qverfies
    delete sortOrder;
}

/*
    Helper function to create components from given string.
*/
QObject* tst_QDeclarativeContact::createComponent(const QString& componentString)
{
    QDeclarativeComponent component(&engine);
    component.setData(componentString.toLatin1(), QUrl::fromLocalFile(""));
    QObject* source_obj = component.create();
    if (!source_obj)
        qDebug() << component.errorString();
    return source_obj;
}

QTEST_MAIN(tst_QDeclarativeContact)
#include "tst_qdeclarativecontact.moc"
