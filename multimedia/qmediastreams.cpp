#include "qmediastreams.h"
#include "qmediastreamscontrol.h"
#include "qmediaplayerservice.h"
#include <QtCore/qmap.h>
#include <QtCore/qstringlist.h>
#include <QtCore/private/qobject_p.h>

class QMediaStreamInfoPrivate : public QSharedData
{
public:
    QMediaStreamInfoPrivate()
        :QSharedData(),
         streamId(-1),
         type(QMediaStreamInfo::UnknownStream)
    {
        ref.ref();
    }

    QMediaStreamInfoPrivate(QMediaStreamInfo::StreamType type, int streamId)
        :QSharedData(),
         streamId(streamId),
         type(type)
    {}

    QMediaStreamInfoPrivate(const QMediaStreamInfoPrivate &other)
        :QSharedData(other),
         streamId(other.streamId),
         type(other.type),
         properties(other.properties)
    {}

    ~QMediaStreamInfoPrivate() {}

    int streamId;
    QMediaStreamInfo::StreamType type;
    QMap<QString,QVariant> properties;
};

Q_GLOBAL_STATIC(QMediaStreamInfoPrivate, qt_sharedMediaStreamInfo)

/*!
    \class QMediaStreamInfo
    \ingroup multimedia

    \preliminary
    \brief The QMediaStreamInfo class provides information
           about one of current session media stream.

    \sa QMediaStreams
*/


/*!
Constructs an invalid stream information object.
*/
QMediaStreamInfo::QMediaStreamInfo()
    :d(qt_sharedMediaStreamInfo())
{
}

/*!
Constructs a stream information object of type \a streamType and id \a streamId.
*/
QMediaStreamInfo::QMediaStreamInfo(StreamType streamType, int streamId)
    :d(new QMediaStreamInfoPrivate(streamType, streamId))
{
}

/*!
  Constructs a copy of the QMediaStreamInfo \a other, passed as the argument to this constructor.
*/
QMediaStreamInfo::QMediaStreamInfo(const QMediaStreamInfo &other)
    :d(other.d)
{
}

/*!
  Assigns the value of the stream information object \a to this object.
*/
QMediaStreamInfo &QMediaStreamInfo::operator =(const QMediaStreamInfo &other)
{
    d = other.d;
    return *this;
}

/*!
  Destroys the stream information object.
*/
QMediaStreamInfo::~QMediaStreamInfo()
{
}

/*!
  Returns true if this is a NULL tream information object, false otherwise.
*/
bool QMediaStreamInfo::isNull() const
{
    return d == qt_sharedMediaStreamInfo();
}

/*!
  Returns identifier of stream.
*/
int QMediaStreamInfo::streamId() const
{
    return d->streamId;
}

/*!
  Returns stream type.
*/
QMediaStreamInfo::StreamType QMediaStreamInfo::type() const
{
    return d->type;
}

QVariant QMediaStreamInfo::property(const QString &name) const
{
    return d->properties.value(name);
}

QStringList QMediaStreamInfo::propertyNames() const
{
    return d->properties.keys();
}

void QMediaStreamInfo::setProperty(const QString &name, const QVariant &value)
{
    d->properties[name] = value;
}

QMap<QString,QVariant> QMediaStreamInfo::properties() const
{
    return d->properties;
}

void QMediaStreamInfo::setProperties(const QMap<QString,QVariant> &properties)
{
    d->properties = properties;
}

/*!
  Returns the stream name, if available.
*/
QString QMediaStreamInfo::streamName() const
{
    return property(QLatin1String("Name")).toString();
}

/*!
  Set the stream name to \a name.
*/
void QMediaStreamInfo::setStreamName(const QString &name)
{
    setProperty(QLatin1String("Name"), name);
}

/*!
  Returns the ISO 639 stream language code, if available.
*/
QString QMediaStreamInfo::streamLanguage() const
{
    return property(QLatin1String("Language")).toString();
}

/*!
  Set the ISO 639 stream language code to \a language.
*/
void QMediaStreamInfo::setStreamLanguage(const QString &language)
{
    setProperty(QLatin1String("Language"), language);
}


/*!
    \class QMediaStreams
    \ingroup multimedia

    \preliminary
    \brief The QMediaStreams class provides
           access to media sessions streams.

    It allows to inspect available video, audio, subpicture
    and data streams and switch between them.

    \sa QMediaStreamInfo
*/



class QMediaStreamsPrivate
{
    Q_DECLARE_PUBLIC(QMediaStreams)
public:
    QAbstractMediaService* service;
    QMediaStreamsControl* control;

    QMap<QMediaStreamInfo::StreamType, QList<QMediaStreamInfo> > streams;
    QList<QMediaStreamInfo> allStreams;
    QMap<QMediaStreamInfo::StreamType, QMediaStreamInfo> activeStreams;
    QMap<QMediaStreamInfo::StreamType, QString> defaultLanguages;

    void _q_updateStreams();
    void _q_updateActiveStreams();

    QMediaStreams *q_ptr;
};

void QMediaStreamsPrivate::_q_updateStreams()
{
    streams.clear();
    allStreams.clear();

    for (int id=0; id<control->streamCount(); id++) {
        QMediaStreamInfo streamInfo(QMediaStreamInfo::StreamType(control->streamType(id)), id);
        streamInfo.setProperties(control->streamProperties(id));

        streams[streamInfo.type()].append(streamInfo);
        allStreams.append(streamInfo);
    }

    emit q_func()->streamsChanged();

    QMapIterator<QMediaStreamInfo::StreamType, QString> languages(defaultLanguages);
    while (languages.hasNext()) {
        languages.next();
        QMediaStreamInfo::StreamType streamType = languages.key();
        const QString language = languages.value();

        if (activeStreams[streamType].streamLanguage() != language) {
            foreach ( const QMediaStreamInfo &streamInfo, allStreams ) {
                if (streamInfo.type() == streamType && streamInfo.streamLanguage() == language)
                    control->setActive(streamInfo.streamId(), true);
            }
        }
    }
}

void QMediaStreamsPrivate::_q_updateActiveStreams()
{
    activeStreams.clear();
    for (int i=0; i<allStreams.count(); i++) {
        if (control->isActive(i)) {
            const QMediaStreamInfo stream = allStreams[i];
            activeStreams[stream.type()] = stream;
        }
    }

    emit q_func()->activeStreamsChanged();
}

/*!
  Constructs the QMediaStreams object for \a mediaObject.
 */
QMediaStreams::QMediaStreams(QAbstractMediaObject *mediaObject, QObject *parent)
    : QObject(parent)
    , d_ptr(new QMediaStreamsPrivate)
{
    Q_D(QMediaStreams);

    d->q_ptr = this;

    d->service = mediaObject->service();
    d->control = qobject_cast<QMediaStreamsControl*>(d->service->control("com.nokia.qt.MediaStreamsControl"));
    if (d->control != 0) {
        connect(d->control, SIGNAL(streamsChanged()), this, SLOT(_q_updateStreams()));
        connect(d->control, SIGNAL(activeStreamsChanged()), this, SLOT(_q_updateActiveStreams()));

        d->_q_updateStreams();
        d->_q_updateActiveStreams();
    }
}

/*!
  Destroys the media streams object.
*/
QMediaStreams::~QMediaStreams()
{
    delete d_ptr;
}

/*!
  Returns true if the service provides information about media streams.
*/
bool QMediaStreams::isStreamsInformationAvailable() const
{
    return d_func()->control != 0;
}

/*!
  Returns the list of streams with type of \a streamType.
*/
QList<QMediaStreamInfo> QMediaStreams::streams(QMediaStreamInfo::StreamType streamType) const
{
    return d_func()->streams.value(streamType);
}

/*!
  Returns the active stream of type \a streamType.
*/
QMediaStreamInfo QMediaStreams::activeStream(QMediaStreamInfo::StreamType streamType) const
{
    return d_func()->activeStreams.value(streamType, QMediaStreamInfo());
}

/*!
  Activate the \a stream.
*/
void QMediaStreams::setActiveStream(const QMediaStreamInfo &stream)
{
    Q_D(QMediaStreams);

    if (d->control) {
        d->control->setActive(stream.streamId(), true);
    }
}

/*!
  Returns the default \a language code for stream of type \a streamType
  or null string if no default language was set.

  \sa QMediaStreamInfo::streamLanguage()
*/
QString QMediaStreams::defaultLanguage(QMediaStreamInfo::StreamType streamType) const
{
    return d_func()->defaultLanguages.value(streamType);
}

/*!
  Set the default \a language code for stream of type \a streamType.
  Next time the set of streams is changed, the stream with the same language code
  will be automaticaly activated.
*/
void QMediaStreams::setDefaultLanguage(QMediaStreamInfo::StreamType streamType, const QString &language)
{
    Q_D(QMediaStreams);
    if (language.isEmpty())
        d->defaultLanguages.remove(streamType);
    else
        d->defaultLanguages.insert(streamType, language);
}

#include "moc_qmediastreams.cpp"
