#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include "qnfctagtestcommon.h"
#include <qnearfieldtagtype1.h>

class tst_qnearfieldtagtype1 : public QObject
{
    Q_OBJECT

public:
    tst_qnearfieldtagtype1();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCase1_data();
private:
    QNfcTagTestCommon tester;
    // Not Own
    QNearFieldTagType1 * tagType1;
};

tst_qnearfieldtagtype1::tst_qnearfieldtagtype1()
{
}

void tst_qnearfieldtagtype1::initTestCase()
{
    tester.touchTarget(QNearFieldTarget::NfcTagType1);
}

void tst_qnearfieldtagtype1::cleanupTestCase()
{
    tester.removeTarget();
}

void tst_qnearfieldtagtype1::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

void tst_qnearfieldtagtype1::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

QTEST_MAIN(tst_qnearfieldtagtype1);

#include "tst_qnearfieldtagtype1.moc"
