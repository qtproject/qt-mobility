#include "testsuite.h"
#include "testhelper.h"

#include <QDebug>
#include <QString>
#include <QTimer>

int main(int argc, char *argv[])
{

    QStringList providers;
    // If no plugin provided, show usage text
    if (argc < 2) {
        qDebug() << "Plugin provider name not found !";
        qDebug() << "Usage: " << argv[0] << " plugin" << endl;
        qDebug() << "Query for providers...";
        providers = QGeoServiceProvider::availableServiceProviders();

    }else {

        char* p = argv[argc - 1];
        providers << QString(p);
        argc--;
    }

    QApplication app(argc, argv);

    int result = 0;

    foreach (QString provider, providers)
        {
            qDebug();
            qDebug() << "************************";
            qDebug() << "Running tests for" << provider;
            qDebug() << "************************";

            TestHelper::loadProvider(provider);

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
                qDebug() << failed << "tests of" << QGeoTestSuite::count()
                    << " test cases failed !";
                qDebug() << "************************";
            }

            result += failed;
        }
    return result;
}

