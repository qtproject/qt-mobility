#include <QtTest/QtTest>
#include <QtCore>
#include "testservice.h"
#include "testserviceiterable.h"
#include "service.h"

class TestServiceTest: public QObject
{
    Q_OBJECT
    
private slots:
    void initTestCase();
    void testProperty();
    void testIterator();


private:
    ServiceSingleton<TestService>* so;
};

void TestServiceTest::initTestCase()
{
    so = new ServiceSingleton<TestService>();
}

void TestServiceTest::testProperty()
{
    int valueInt = 12345;
    so->setProperty("property_int", valueInt);
    QCOMPARE(valueInt, so->property("property_int").toInt());

    QDateTime dateTime(QDateTime::currentDateTime());
    so->setProperty("property_date", dateTime);
    QCOMPARE(dateTime, so->property("property_date").toDateTime());
}

void TestServiceTest::testIterator()
{
    QVariantList list;
    list << "Hello World" << 123456 << true;
    TestServiceIterable* ret = (TestServiceIterable*)so->testIteratorSync(list);
    QVERIFY(ret->hasNext());
    QCOMPARE(list[0], ret->next());
    QCOMPARE(list[1], ret->next());
    QCOMPARE(list[2], ret->next());
}

QTEST_MAIN(TestServiceTest)
#include "testservicetest.moc"
