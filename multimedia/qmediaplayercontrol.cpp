
#include "qabstractmediacontrol.h"


QMediaPlayerControl::QMediaPlayerControl(QObject *parent):
    QAbstractMediaControl(parent)
{
    addPropertyWatch("position");
}
