#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include "qnfctagtestcommon.h"
#include <qnearfieldtagtype3.h>

class tst_QNearFieldTagType3 : public QObject
{
    Q_OBJECT

public:
    tst_QNearFieldTagType3();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCase1_data();
private:
    QNfcTagTestCommon tester;
    // Not Own
    QNearFieldTagType3 * tagType3;
};

tst_QNearFieldTagType3::tst_QNearFieldTagType3()
{
}

void tst_QNearFieldTagType3::initTestCase()
{
    tester.touchTarget(QNearFieldTarget::NfcTagType3);
}

void tst_QNearFieldTagType3::cleanupTestCase()
{
    tester.removeTarget();
}

void tst_QNearFieldTagType3::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

void tst_QNearFieldTagType3::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

QTEST_MAIN(tst_QNearFieldTagType3);

#include "tst_qnearfieldtagtype3.moc"
