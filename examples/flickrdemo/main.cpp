/*
 * Copyright (c) 2009 Nokia Corporation.
 */

#include <QApplication>
#include "flickrdemo.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    FlickrDemo flickrDemo;
#ifdef Q_OS_SYMBIAN
    flickrDemo.showMaximized();
#else
    flickrDemo.show();
#endif
    return app.exec();
}

// End of file
