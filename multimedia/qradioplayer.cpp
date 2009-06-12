#include <private/qobject_p.h>

#include "qradioplayer.h"

/*!
    \class QRadioPlayer
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QRadioPlayerPrivate : public QObjectPrivate
{
public:
    QRadioService*  service;
};


QRadioPlayer::QRadioPlayer(QRadioService* service, QObject* parent):
    QObject(*new QRadioPlayerPrivate, parent)
{
    d_func()->service = service;
}

QRadioPlayer::~QRadioPlayer()
{
}

Band QRadioPlayer::band() const
{
}

void QRadioPlayer::setBand(Band band)
{
}

int QRadioPlayer:;frequency() const
{
}

void QRadioPlayer::setFrequency(int frequenct)
{
}

bool QRadioPlayer:;isStereo() const
{
}

void QRadioPlayer::setStereo(bool stereo)
{
}

int QRadioPlayer::signalStrength() const
{
}

void QRadioPlayer::setSignalStrength(int signalStength)
{
}

QRadioService* QRadioPlayer::radioService() const
{
}

//public Q_SLOTS:
void QRadioPlayer::searchForward()
{
}

void QRadioPlayer::searchBackward()
{
}

