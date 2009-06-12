#ifndef QMEDIAPLAYERSESSION_H
#define QMEDIAPLAYERSESSION_H


#include <QObject>

class QMediaPlayerSession : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QMediaSource mediaSource READ mediaSource WRITE setMediaSource)
    Q_PROPERTY(<> videoOutput READ videoOutput WRITE setVideoOutput);
    Q_PROPERTY(<> audioOutput READ audioOutput WRITE setAudioOutput);

    friend class QMediaPlayer;

public:
    <> videoOutput() const;
    void setVideoOutput(<>);
    QList<> availableVideoOutputs() const;

    <> audioOutput() const;
    void setAudioOutput(<>);
    QList<> availableAudioOutputs() const;

protected:
    QMediaPlayerSession(QMediaPlayerSessionPrivate& dd, QObject* parent);

    QMediaPlayerImpl* playerObject() const = 0;

private:
    Q_DECLARE_PRIVATE(QMediaPlayerSession)
};


class QMediaPlayerImpl : public QAbstractMediaObject
{
    Q_OBJECT

public:
    QMediaSource mediaSource() const;
    void setMediaSource(QMediaSource mediaSource);

    virtual void play() = 0;
    virtual void pause() = 0e
    virtual void stop() = 0;

protected:
    virtual bool propertyValueChanged(QByteArray const& name, QVariant const& value) = 0;
};


#endif  // QMEDIAPLAYERSESSION_H
