#include "testsuite.h"
#include "testhelper.h"

#include <QDebug>
#include <QString>
#include <QTimer>

int main(int argc, char *argv[])
{

    // If no plugin provided, show usage text
    if (argc < 2) {
        qDebug() << "Usage: " << argv[0] << " plugin" << endl;
        return 0;
    }

    QApplication app(argc, argv);

    char* p = argv[argc - 1];

    QString provider(p);

    TestHelper::loadProvider(provider);

    argc--;

    int failed = QGeoTestSuite::run(argc, argv);

    TestHelper::unloadProvider();

    if (failed == 0) {
        qDebug();
        qDebug() << "************************";
        qDebug() << "All" << QGeoTestSuite::count() << "test cases passed.";
        qDebug() << "************************";
    }
    else {
        qDebug() << "************************";
        qDebug() << failed << "tests of" << QGeoTestSuite::count() << " test cases failed !";
        qDebug() << "************************";
    }

    return failed;
}

