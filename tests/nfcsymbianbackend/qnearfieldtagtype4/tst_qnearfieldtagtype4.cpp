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
    void testNdefAccess();
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

void tst_qnearfieldtagtype4::testNdefAccess()
{
    QCOMPARE(tagType4->type(), QNearFieldTarget::NfcTagType4);

    tester.NdefCheck();
}

QTEST_MAIN(tst_qnearfieldtagtype4);

#include "tst_qnearfieldtagtype4.moc"
