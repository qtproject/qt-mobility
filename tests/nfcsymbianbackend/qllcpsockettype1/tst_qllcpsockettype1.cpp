#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

class tst_qllcpsockettype1 : public QObject
{
    Q_OBJECT

public:
    tst_qllcpsockettype1();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCase1_data();
};

tst_qllcpsockettype1::tst_qllcpsockettype1()
{
}

void tst_qllcpsockettype1::initTestCase()
{
}

void tst_qllcpsockettype1::cleanupTestCase()
{
}

void tst_qllcpsockettype1::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

void tst_qllcpsockettype1::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

QTEST_MAIN(tst_qllcpsockettype1);

#include "tst_qllcpsockettype1.moc"
