#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include "qnfctagtestcommon.h"

class tst_qnearfieldtagtype4 : public QObject
{
    Q_OBJECT

public:
    tst_qnearfieldtagtype4();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCase1_data();
private:
    QNfcTagTestCommon tester;
    // Not own
    QNearFieldTarget * tagType4;
};

tst_qnearfieldtagtype4::tst_qnearfieldtagtype4()
{
}

void tst_qnearfieldtagtype4::initTestCase()
{
    tester.touchTarget(QNearFieldTarget::NfcTagType4);
}

void tst_qnearfieldtagtype4::cleanupTestCase()
{
    tester.removeTarget();
}

void tst_qnearfieldtagtype4::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

void tst_qnearfieldtagtype4::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

QTEST_MAIN(tst_qnearfieldtagtype4);

#include "tst_qnearfieldtagtype4.moc"
