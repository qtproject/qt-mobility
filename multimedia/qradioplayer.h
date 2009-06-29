#ifndef QRADIOPLAYER_H
#define QRADIOPLAYER_H

#include <QtCore/qobject.h>

#include "qabstractmediaobject.h"
#include "qabstractmediaservice.h"
#include "qmediaserviceprovider.h"

class QRadioService;
class QRadioPlayerPrivate;

extern QRadioService* createRadioService(QMediaServiceProvider *provider = defaultServiceProvider("radio"));

class QRadioPlayer : public QAbstractMediaObject
{
    Q_OBJECT

    Q_PROPERTY(Band band READ band WRITE setBand NOTIFY bandChanged)
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(bool stereo READ isStereo WRITE setStereo NOTIFY stereoStatusChanged)
    Q_PROPERTY(int signalStrength READ signalStrength NOTIFY signalStrengthChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutingChanged)

    Q_ENUMS(Band)

public:
    enum Band { AM, FM, SW };

    QRadioPlayer(QRadioService *radioService = createRadioService(), QObject *parent = 0);
    ~QRadioPlayer();

    Band band() const;

    int frequency() const;

    bool isStereo() const;

    int signalStrength() const;

    qint64 duration() const;

    int volume() const;
    bool isMuted() const;

    QAbstractMediaService* service() const;

public Q_SLOTS:
    void searchForward();
    void searchBackward();

    void setBand(Band band);
    void setFrequency(int frequency);
    void setStereo(bool stereo);

    void setVolume(int volume);
    void setMuted(bool muted);

Q_SIGNALS:
    void bandChanged(QRadioPlayer::Band band);
    void frequencyChanged(int frequency);
    void stereoStatusChanged(bool stereo);
    void signalStrengthChanged(int signalStrength);
    void durationChanged(qint64 durattion);
    void volumeChanged(int volume);
    void mutingChanged(bool muted);

private:
    Q_DISABLE_COPY(QRadioPlayer)
    Q_DECLARE_PRIVATE(QRadioPlayer)
};

#endif  // QRADIOPLAYER_H
