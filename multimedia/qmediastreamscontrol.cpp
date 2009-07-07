#include "qmediastreamscontrol.h"
#include "qabstractmediacontrol_p.h"

QMediaStreamsControl::QMediaStreamsControl(QObject *parent)
    :QAbstractMediaControl(*new QAbstractMediaControlPrivate, parent)
{
}

QMediaStreamsControl::~QMediaStreamsControl()
{
}
