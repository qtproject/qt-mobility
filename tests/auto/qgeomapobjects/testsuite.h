#ifndef TESTSUITE_H
#define TESTSUITE_H

#include <QString>
#include <QTest>
#include <QList>
#include <QSharedPointer>

#define ADD_TO_TESTSUITE(className) static  QGeoTestSuite::TestCase<className> test(#className);

namespace QGeoTestSuite
{

typedef QList<QObject*> TestSuite;

inline TestSuite& testSuite()
{
    //keep suite
    static TestSuite suite;
    return suite;
}

inline bool findTest(QObject* object)
{
    TestSuite& suite = testSuite();

    if (suite.contains(object)) {
        return true;
    }
    foreach (QObject* test,suite)
        {
            if (test->objectName() == object->objectName()) {
                return true;
            }
        }
    return false;
}

inline void addTest(QObject* object)
{
    TestSuite& suite = testSuite();

    if (!findTest(object)) {
        suite.append(object);
    }
}

inline int count()
{
    TestSuite& suite = testSuite();

    return suite.count();
}

inline int run(int argc, char *argv[])
{
    int ret = 0;

    foreach (QObject* test, testSuite())
        {
            ret += QTest::qExec(test, argc, argv);
        }

    //returns 0 if all test passed
    return ret;
}

template<class T>
class TestCase
{
public:
    QSharedPointer<T> pointer;

    TestCase(const QString& name) :
        pointer(new T)
    {

        pointer->setObjectName(name);
        addTest(pointer.data());
    }
};
}

#endif //TESTSUITE_H
