#ifndef QRADIOPLAYER_H
#define QRADIOPLAYER_H

#include <QObject>

class QRadioService;
class QRadioPlayerPrivate;

class QRadioPlayer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Band band READ band WRITE setBand NOTIFY bandChanged)
    Q_PROPERTY(int frequency READ frequemcy WRITE setFrequnecy NOTIFY frequencyChanged)
    Q_PROPERTY(bool stereo READ isStereo WRITE setStereo NOTIFY stereoStatusChanged)
    Q_PROPERTY(int signalStrength READ signalStrength  NOTIFY signalStrengthChanged)

    Q_ENUM(Band)

public:
    enum Band { AM, FM, SW };

    QRadioPlayer(QRadioService* radioService = createRadioService(), QObject* parent = 0);
    ~QRadioPlayer();

    Band band() const;
    void setBand(Band band);

    int frequency() const;
    void setFrequency(int frequenct);

    bool isStereo() const;
    void setStereo(bool stereo);

    int signalStrength() const;
    void setSignalStrength(int signalStength);

    QRadioService* radioService() const;

public Q_SLOTS:
    void searchForward();
    void searchBackward();

Q_SIGNALS:
    void bandChanged(QRadioPlayer::Band band);
    void frequencyChanged(int frequency);
    void stereoStatusChanged(bool stereo);
    void signalStrengthChanged(int signalStrength);

private:
    Q_DELCARE_PRIVATE(QRadioPlayer)
    Q_DISABLE_COPY(QRadioPlayer)
};


#endif  // QRADIOPLAYER_H
