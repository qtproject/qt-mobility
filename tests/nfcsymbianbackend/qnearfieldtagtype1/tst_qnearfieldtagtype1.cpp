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
    void testNdefAccess();
private:
    QNfcTagTestCommon tester;
    // Not Own
    QNearFieldTagType1 * tagType1;
};

tst_qnearfieldtagtype1::tst_qnearfieldtagtype1()
{
    tagType1 = 0;
}

void tst_qnearfieldtagtype1::initTestCase()
{
    tester.touchTarget(QNearFieldTarget::NfcTagType1);
}

void tst_qnearfieldtagtype1::cleanupTestCase()
{
    tester.removeTarget();
}

void tst_qnearfieldtagtype1::testNdefAccess()
{
    tagType1 = qobject_cast<QNearFieldTagType1 *>(tester.getTarget());
    QVERIFY(tagType1);
    QCOMPARE(tagType1->type(), QNearFieldTarget::NfcTagType1);

    tester.NdefCheck();
}

QTEST_MAIN(tst_qnearfieldtagtype1);

#include "tst_qnearfieldtagtype1.moc"
