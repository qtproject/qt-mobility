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
    void testNdefAccess();
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

void tst_QNearFieldTagType3::testNdefAccess()
{
    tagType3 = qobject_cast<QNearFieldTagType3 *>(tester.getTarget());
    QVERIFY(tagType3);
    QCOMPARE(tagType3->type(), QNearFieldTarget::NfcTagType3);

    tester.NdefCheck();
}

QTEST_MAIN(tst_QNearFieldTagType3);

#include "tst_qnearfieldtagtype3.moc"
