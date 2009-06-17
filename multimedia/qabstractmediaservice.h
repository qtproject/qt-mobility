

class QAbstractMediaService : public QObject
{
    Q_OBJECT
    Q_PROPERTIES(int notifyInterval READ notifyInterval WRITE setNotifyInterval NOTIFY notifyIntervalChanged)

public:
    QMediaAudioOutput audioOutput() const;
    void setAudioOutput(QMediaAudioOutput const &audioOutput);
    QList<QMediaAudioOutput> availableAudioOutputs() const;

    QMediaAudioOutput videoOutput() const;
    void setVideoOutput(QMediaVideoOutput const &videoOutput);
    QList<QMediaAudioOutput> availableVideoOutputs() const;

    int notifyInterval() const;
    void setNotifyInterval(int milliSeconds);

    virtual QAbstractMediaControl* control() const = 0;

Q_SIGNALS:
    void notifyIntervalChanged(int milliSeconds);
};
