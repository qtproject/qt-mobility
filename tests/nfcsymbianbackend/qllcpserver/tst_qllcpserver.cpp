#include <QtCore/QString>
#include <QtTest/QtTest>

class tst_QLlcpServer : public QObject
{
    Q_OBJECT

public:
    tst_QLlcpServer();

private Q_SLOTS:
    void testCase1();
};

tst_QLlcpServer::tst_QLlcpServer()
{
}

void tst_QLlcpServer::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(tst_QLlcpServer);

#include "tst_qllcpserver.moc"
