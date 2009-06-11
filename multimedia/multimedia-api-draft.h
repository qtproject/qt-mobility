
/*
   Experimental multimedia API for the Qt Mobility Program
   */

class QMediaPlaylist
{
public:
    QMediaPlayList(QString name, QPlayListSource = defaultPlayListSource());    // default == memory?

    void enqueue(QMediaSource);
    void remove(QMediaSource));

    clear();

slots:
    void start();
    void pause();
    void stop();

    void next();
    void prev();

    void jump(int);

signals:
    void current(MediaSource);
};


class QAbstractMediaObject : public QObject
{
public:
    virtual void setUserObject(QObject* userObject);

protected:
    virtual bool propertyValueChanged(QByteArray const& name, QVariant const& value) = 0;

    bool event(QEvent* e)
    {
        if (e->type() == QEvent::DynamicPropertyChange) {
            QByteArray pn = qobject_cast<QDynamicpropertyChangeEvent*>(e)->propertyName();
            QVariant pv = property(pn.constData());
            if (!propertyValueChanged(pn, pv))
                return false;

            const QMetaObject* m = userObject->metaObject();

            int pi = m->indexOfProperty(pn);
            if (pi == -1)
                return false;

            QMetaProperty p = m->property(pi);
            if (!p.hasNotifySignal()) {
                return false

            p->notifySignal().invoke(userObject, QGenericArgument(QMetaType::typeName(p.userType), property(pn).data()));

            e->accept();
            return true;
        }

        return false;
    }

    QObject* userObject;
};


class QMediaMetaData : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QMediaMetaData)

    Q_PROPERTY(bool metaDataAvailable READ metaDataAvailable NOTIFY metaDataAvailabilityChanged)

public:
    QMediaMetaData(QMediaPlayer* mediaPlayer);
    ~QMediaMetaData();

    bool metaDataAvailable() const;

    QList<QString> availableMetaData() const;
    QVariant valueFor(QString const& name) const;

signals:
    void metaDataAvailabilityChanged(bool metaDataAvailable);
};

QMediaPlayerSession* createMediaPlayerSession(QString name = "default")
{
    return new QMediaPlayerSessionImplementation;
}


class QMediaPlayerSessionPrivate;

class QMediaPlayerSession : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QMediaPlayerSession)

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
};

class QMediaPlayerImpl : public QAbstractMediaObject
{
    Q_OBJECT

public:
    QMediaSource mediaSource() const;
    void setMediaSource(QMediaSource mediaSource);

    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

protected:
    virtual bool propertyValueChanged(QByteArray const& name, QVariant const& value) 
    {
        return true if change meaningful;

        return false;
    }
};

class QMediaPlayer : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QMediaPlayer)

    Q_PROPERTY(QMediaSource mediaSource READ mediaSource WRITE setMediaSource NOTIFY mediaSourceChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(qint64 position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERYY(bool muted READ isMuted WRITE setMuted NOTIFY mutingChanged)
    Q_PROPERTY(bool buffering READ isBuffering NOTIFY bufferingChanged)
    Q_PROPERTY(bool video READ isVideoAvailable NOTIFY videoAvailablityChanged)
    Q_PROPERTY(int playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChange)
    Q_PROPERTY(QMediaPlayerSession* mediaSession READ mediaSesssion)

    Q_ENUMS(State)

public:
    enum State { LoadingState, PlayingState, PausedState, StoppedState };

    QMediaPlayer(QMediaPlayerSession* session = createMediaSession(), QObject* parent = 0);
    ~QMediaPlayer();

    State state() const;
    QMediaSource source() const;

    qint64 duration() const;
    qint64 position() const;

    int volume() const;
    bool isMuted() const;

    bool isBuffering() const;
    bool isVideoAvailable() const;

    QMediaPlayerSession* session() const;

slots:
    void setMediaSource(QMediaSource mediaSource);

    void play();
    void pause();
    void stop();

    void setPosition(qint64 position);
    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    void mediaSourceChanged(QMediaSource mediaSource);
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void stateChanged(State newState);
    void volumeChanged(int volume);
    void mutingChanged(bool muted);
    void videoAvailabilityChanged(bool videoAvailable);
    void bufferingChanged(bool buffering);

private:
    Q_DISABLE_COPY(QMediaPlayer)

    QMediaPlayerImpl* d;
};


class QMediaRecorderPrivate;

closs QMediaRecorder : public QObject
{
public:
    QMediaRecorder(QMediaRecorderSession* = createMediaSession());

    State state() const;
    MediaSink sink() const
    MediaSession* session() const;

slots:
    void setMediaSink(MediaSink);

    void record();
    void pause();
    void stop();

    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    void stateChangeed(oldState, newState);
};


class QRadioService : public QObject
{
    Q_OBJECT

    <> audioOutput() const;
    void setAudioOutput(<>);
    QList<> availableAudioOutputs() const;

    friend class QRadioPlayer;

protected:
    QRadioService(QObject* parent);

    QRadioPlayerImpl* playerObject() const = 0;
};



class QRadioPlayerImp : public QAbstractMediaSession
{
public:
    virtual void seekForward() = 0;
    virtual void seekBackward() = 0;
};


class QRadioPlayer : public QObject
{
    Q_OBJECT
    Q_DELCARE_PRIVATE(QRadioPlayer)

    Q_PROPERTY(Band band READ band WRITE setBand NOTIFY bandChanged)
    Q_PROPERTY(int frequency READ frequemcy WRITE setFrequnecy NOTIFY frequencyChanged)
    Q_PROPERTY(bool stereo READ isStereo WRITE setStereo NOTIFY stereoStatusChanged)
    Q_PROPERTY(int signalStrength READ signalStrength  NOTIFY signalStrengthChanged)

    Q_ENUM(Band)

public:
    enum Band { AM, FM, SW };

    QRadioPlayer(QRadioService* radioService = createRadioService(), QObject* parent = 0):
        QObject(parent)
    {
        Q_D(QRadioPlayer);

        d->radioService = radioService;
        d->radioImpl = radioService->impl();
        d->radioImpl->setUserObject(this);
    }

    ~QRadioPlayer();

    Band band() const
    {
        return d_func()->radioImpl->property("Band");
    }

    void setBand(Band band)
    {
        d_func()->radioImpl->setProperty("Band", band);
    }

    int frequency() const;
    void setFrequency(int frequenct);

    bool isStereo() const;
    void setStereo(bool stereo);

    int signalStrength() const;
    void setSignalStrength(int signalStength);

    QRadioService* radioService() const;

slots:
    void searchForward() { d_func()->radioImpl->seekForward(); }
    void searchBackward() { d_func()->radioImpl->seekBackward(); }

signals:
    void bandChanged(Band band);
    void frequencyChanged(int frequency);
    void stereoStatusChanged(bool stereo);
    void signalStrengthChanged(int signalStrength);
};


class QMediaImageViewer : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QMediaSource mediaSource READ mediaSource WRITE setMediaSource NOTIFY mediaSourceChanged)
    Q_PROPERTY(int interval READ interval NOTIFY intervalChanged)

public:
    QMediaImageViewer();
    ~QMediImageViewer();

    void play();
    void pause();
    void stop();

slots:
    void setMediaSource(QMediaSource mediaSource);
};

class MediaType
{
    QVariant type;
};

class MediaLocation
{
    QVariant location;
};


class MediaSource
{
    setMediaType();
    setMediaLocation();
};

class MediaSink
{
    mediaType();
    mediaLocation();
};



class VideoWidget
{
    VideoWidget(QMediaPlayer* playerObject);
};

class MediaSeekControl
{
    MediaSeek(QMediaPlayer* playerObject);

    bool isValid() const;

slots:
    void seek(int);
    void nextChapter();
    void prevChapter();

signals;
     void changeState(bool seekable)
};

