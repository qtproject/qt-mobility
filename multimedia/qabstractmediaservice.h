#ifndef QABSTRACTMEDIASERVICE_H
#define QABSTRACTMEDIASERVICE_H

class QAbstractMediaService : public QObject
{
    Q_OBJECT

public:
    QMediaAudioOutput audioOutput() const;
    void setAudioOutput(QMediaAudioOutput const &audioOutput);
    QList<QMediaAudioOutput> availableAudioOutputs() const;

    QMediaAudioOutput videoOutput() const;
    void setVideoOutput(QMediaVideoOutput const &videoOutput);
    QList<QMediaAudioOutput> availableVideoOutputs() const;

    virtual QAbstractMediaControl* control() const = 0;

protected:
    QAbstractMediaService(QObject* parent);
};


#endif  // QABSTRACTMEDIASERVICE_H

