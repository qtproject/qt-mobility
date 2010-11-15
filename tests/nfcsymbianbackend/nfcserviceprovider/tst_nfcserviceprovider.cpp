#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

class tst_nfcServiceProvider : public QObject
{
    Q_OBJECT

public:
    tst_nfcServiceProvider();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCase1_data();
};

tst_nfcServiceProvider::tst_nfcServiceProvider()
{
}

void tst_nfcServiceProvider::initTestCase()
{
}

void tst_nfcServiceProvider::cleanupTestCase()
{
}

void tst_nfcServiceProvider::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

void tst_nfcServiceProvider::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

QTEST_MAIN(tst_nfcServiceProvider);

#include "tst_nfcserviceprovider.moc"
