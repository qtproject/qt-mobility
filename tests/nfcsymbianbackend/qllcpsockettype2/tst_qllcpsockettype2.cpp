#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

class tst_qllcpsockettype2 : public QObject
{
    Q_OBJECT

public:
    tst_qllcpsockettype2();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCase1_data();
};

tst_qllcpsockettype2::tst_qllcpsockettype2()
{
}

void tst_qllcpsockettype2::initTestCase()
{
}

void tst_qllcpsockettype2::cleanupTestCase()
{
}

void tst_qllcpsockettype2::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

void tst_qllcpsockettype2::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

QTEST_MAIN(tst_qllcpsockettype2);

#include "tst_qllcpsockettype2.moc"
