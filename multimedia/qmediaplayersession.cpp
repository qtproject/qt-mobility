
#include "qmediaplayersession.h"




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
