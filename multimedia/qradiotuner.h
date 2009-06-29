#ifndef QRADIOTUNER_H
#define QRADIOTUNER_H

#include "qabstractmediacontrol.h"

class QRadioTuner : public QAbstractMediaControl
{
    Q_OBJECT

public:
    QRadioTuner(QObject *parent = 0);
    ~QRadioTuner();

    int band() const;
    virtual void setBand(int b);

    int frequency() const;
    virtual void setFrequency(int frequency);

    bool isStereo() const;
    virtual void setStereo(bool stereo);

    int signalStrength() const;
    virtual void setSignalStrength(int strength);

    qint64 duration() const;
    virtual void setDuration(qint64 duration);

    int volume() const;
    virtual void setVolume(int volume);

    bool isMuted() const;
    virtual void setMuted(bool muted);

    virtual void searchForward() = 0;
    virtual void searchBackward() = 0;
};

#endif  // QRADIOTUNER_H
