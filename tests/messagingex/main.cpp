#include <QApplication>

#include "messagingex.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MessagingEx messagingEx;
    messagingEx.showMaximized();
    return app.exec();
}

// End of file
