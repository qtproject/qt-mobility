#include "qmediastreamscontrol.h"
#include "qmediacontrol_p.h"

QMediaStreamsControl::QMediaStreamsControl(QObject *parent)
    :QMediaControl(*new QMediaControlPrivate, parent)
{
}

QMediaStreamsControl::~QMediaStreamsControl()
{
}
