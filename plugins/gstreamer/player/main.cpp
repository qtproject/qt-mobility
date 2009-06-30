#include "player.h"

#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Player player;
    player.show();

    return app.exec();
};
