
#include <QtCore/private/qobject_p.h>
#include <QtCore/qtimer.h>

#include "qabstractmediaservice.h"


class QAbstractMediaServicePrivate : public QObjectPrivate
{
public:
};

QAbstractMediaService::QAbstractMediaService(QObject *parent):
    QObject(*new QAbstractMediaServicePrivate, parent)
{
}

QMediaAudioOutput QAbstractMediaService::audioOutput() const
{
    return QMediaAudioOutput();
}

void QAbstractMediaService::setAudioOutput(QMediaAudioOutput const &audioOutput)
{
}

QList<QMediaAudioOutput> QAbstractMediaService::availableAudioOutputs() const
{
    return QList<QMediaAudioOutout>();
}

QMediaAudioOutput QAbstractMediaService::videoOutput() const
{
    return QMediaAudioOutut();
}

void QAbstractMediaService::setVideoOutput(QMediaVideoOutput const &videoOutput)
{
}

QList<QMediaVideoOutput> QAbstractMediaService::availableVideoOutputs() const
{
    return QList<QMediaVideoOutout>();
}
