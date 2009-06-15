#include <QApplication>

#include "bearerex.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    BearerEx bearerEx;
#ifdef Q_OS_SYMBIAN
    bearerEx.showMaximized();
#else
    bearerEx.show();
#endif
    return app.exec();
}

// End of file

