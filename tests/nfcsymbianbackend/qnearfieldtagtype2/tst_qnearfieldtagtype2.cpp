#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include "qnfctagtestcommon.h"
#include <qnearfieldtagtype2.h>

class tst_QNearFieldTagType2 : public QObject
{
    Q_OBJECT

public:
    tst_QNearFieldTagType2();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCase1_data();
private:
    QNfcTagTestCommon tester;
    // Not Own
    QNearFieldTagType2 * tagType2;
};

tst_QNearFieldTagType2::tst_QNearFieldTagType2()
{
}

void tst_QNearFieldTagType2::initTestCase()
{
    tester.touchTarget(QNearFieldTarget::NfcTagType2);
}

void tst_QNearFieldTagType2::cleanupTestCase()
{
}

void tst_QNearFieldTagType2::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

void tst_QNearFieldTagType2::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

QTEST_MAIN(tst_QNearFieldTagType2);

#include "tst_qnearfieldtagtype2.moc"
