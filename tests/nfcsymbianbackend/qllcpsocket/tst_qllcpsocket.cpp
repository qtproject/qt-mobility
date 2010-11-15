#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

class tst_qllcpsocket : public QObject
{
    Q_OBJECT

public:
    tst_qllcpsocket();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCase1_data();
};

tst_qllcpsocket::tst_qllcpsocket()
{
}

void tst_qllcpsocket::initTestCase()
{
}

void tst_qllcpsocket::cleanupTestCase()
{
}

void tst_qllcpsocket::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

void tst_qllcpsocket::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

QTEST_MAIN(tst_qllcpsocket);

#include "tst_qllcpsocket.moc"
