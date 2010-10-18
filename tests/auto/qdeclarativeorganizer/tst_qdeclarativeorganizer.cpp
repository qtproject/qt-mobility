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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
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


#include "qdeclarativeorganizermodel_p.h"
#include "qdeclarativeorganizeritemsortorder_p.h"
#include "qdeclarativeorganizerrecurrencerule_p.h" 
#include "qdeclarativeorganizeritem_p.h" 
#include "qdeclarativeorganizeritemfilter_p.h" 
#include "qdeclarativeorganizeritemdetail_p.h" 
#include "qdeclarativeorganizeritemfetchhint_p.h" 
#include "qdeclarativeorganizercollection_p.h" 

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

QTM_USE_NAMESPACE


class tst_QDeclarativeOrganizer : public QObject
{
    Q_OBJECT

public:
    tst_QDeclarativeOrganizer();
    virtual ~tst_QDeclarativeOrganizer();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

private slots:

    void construction();
    void construction_data();


private:
    QObject* createComponent(const QString& componentString);

private:
    // Engine is needed for instantiating declarative components
    QDeclarativeEngine m_engine;
};

tst_QDeclarativeOrganizer::tst_QDeclarativeOrganizer() {}
tst_QDeclarativeOrganizer::~tst_QDeclarativeOrganizer() {}

void tst_QDeclarativeOrganizer::initTestCase()
{
}

void tst_QDeclarativeOrganizer::cleanupTestCase()
{
}

void tst_QDeclarativeOrganizer::init()
{

}
void tst_QDeclarativeOrganizer::construction()
{
    QFETCH(QString, componentString);
    QFETCH(QString, expectedClassName);
    QFETCH(bool, shouldSucceed);

    QObject* obj = createComponent(componentString);

    if (shouldSucceed) {
        if (obj == 0) {
            qWarning("--------- ------------- ------------- ---------------------- ------------ ");
            qWarning("--------- could not instantiate components from organizer   -------------- ");
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

void tst_QDeclarativeOrganizer::construction_data()
{
    QTest::addColumn<QString>("expectedClassName");
    QTest::addColumn<QString>("componentString");
    QTest::addColumn<bool>("shouldSucceed");
    // OrganizerModel
    QTest::newRow("OrganizerModel: No properties") <<  "QDeclarativeOrganizerModel" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerModel {}" << true;
    QTest::newRow("OrganizerModel: Only id property") << "QDeclarativeOrganizerModel" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerModel {id: organizerModelId}" << true;
    QTest::newRow("OrganizerModel: Valuetype properties") << "QDeclarativeOrganizerModel" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerModel {id: organizerModelId; manager:'memory'; startPeriod:'2010-08-12T13:22:01'; endPeriod:'2010-09-12T13:22:01'}" << true;
    QTest::newRow("OrganizerModel: With filter") << "QDeclarativeOrganizerModel" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerModel {id: organizerModelId; filter: OrganizerItemDateTimePeriodFilter{id: filter;start:'2010-08-12T13:22:01';end:'2010-09-12T13:22:01'} }" << true;
    QTest::newRow("OrganizerModel: With fetchHint") << "QDeclarativeOrganizerModel" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerModel {id: organizerModelId; fetchHint:OrganizerItemFetchHint {id:hint; optimizationHints:OrganizerItemFetchHint.AllRequired} }" << true;

    // Organizer Items
    QTest::newRow("Base organizer item") << "QDeclarativeOrganizerItem" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerItem {}" << true;
    QTest::newRow("Base organizer item: only id") << "QDeclarativeOrganizerItem" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerItem {id:organizerItem}" << true;
    QTest::newRow("Base organizer item: Valuetype properties") << "QDeclarativeOrganizerItem" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerItem {id:organizerItem; displayLabel:'test item'; description:'item description'; guid:'1112232133'}" << true;
    QTest::newRow("Base organizer item: default property") << "QDeclarativeOrganizerItem" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerItem {id:organizerItem; OrganizerItemDisplayLabel {label:'test item'} OrganizerItemDescription { description:'item description'} OrganizerItemGuid{guid:'1112232133'} }" << true;

    //OrganizerEvent
    QTest::newRow("Organizer event") << "QDeclarativeOrganizerEvent" << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n OrganizerEvent {}" << true;
    QTest::newRow("Organizer event: Valuetype properties") << "QDeclarativeOrganizerEvent"
               << "import Qt 4.7 \n import QtMobility.organizer 1.1 \n"
                  "OrganizerEvent {id:organizerEvent; displayLabel:'meeting'; startDateTime:'2010-08-12T13:00:00'; endDateTime:'2010-08-12T15:00:00'; isAllDay:false; locationName:'office'; locationAddress:'53 Brandl st'; locationGeoCoordinates:'-27.579570, 153.10031'; priority:OrganizerItemPriority.LowPriority;"
                  "recurrence.recurrenceRules:[OrganizerItemRecurrenceRule {}]\n"
                  "recurrence.recurrenceDates:[]\n recurrence.exceptionDates:[]"
                  "}"
               << true;
}

/*
    Helper function to create components from given string.
*/
QObject* tst_QDeclarativeOrganizer::createComponent(const QString& componentString)
{
    QDeclarativeComponent component(&m_engine);
    component.setData(componentString.toLatin1(), QUrl::fromLocalFile(""));
    QObject* source_obj = component.create();
    if (!source_obj)
        qDebug() << component.errorString();
    return source_obj;
}

QTEST_MAIN(tst_QDeclarativeOrganizer)
#include "tst_qdeclarativeorganizer.moc"
