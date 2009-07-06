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
    \brief The QMediaSource class provides information
           about one of current session media stream.

    \sa QMediaStreams
*/

QMediaStreamInfo::QMediaStreamInfo()
    :d(qt_sharedMediaStreamInfo())
{
}

QMediaStreamInfo::QMediaStreamInfo(StreamType type, int streamId)
    :d(new QMediaStreamInfoPrivate(type, streamId))
{
}

QMediaStreamInfo::QMediaStreamInfo(const QMediaStreamInfo &other)
    :d(other.d)
{
}

QMediaStreamInfo &QMediaStreamInfo::operator =(const QMediaStreamInfo &other)
{
    d = other.d;
    return *this;
}

QMediaStreamInfo::~QMediaStreamInfo()
{
}

bool QMediaStreamInfo::isNull() const
{
    return d == qt_sharedMediaStreamInfo();
}

int QMediaStreamInfo::streamId() const
{
    return d->streamId;
}

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

QString QMediaStreamInfo::streamName() const
{
    return property(QLatin1String("Name")).toString();
}

void QMediaStreamInfo::setStreamName(const QString &name)
{
    setProperty(QLatin1String("Name"), name);
}

QString QMediaStreamInfo::streamLanguage() const
{
    return property(QLatin1String("Language")).toString();
}

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



class QMediaStreamsPrivate : public QObjectPrivate
{
Q_DECLARE_PUBLIC(QMediaStreams)
public:
    QAbstractMediaService* service;
    QMediaStreamsControl* control;

    QMap<QMediaStreamInfo::StreamType, QList<QMediaStreamInfo> > streams;
    QList<QMediaStreamInfo> allStreams;
    QMap<QMediaStreamInfo::StreamType, QMediaStreamInfo> activeStreams;

    void _q_updateStreams();
    void _q_updateActiveStreams();
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
    :QObject( *new QMediaStreamsPrivate, parent )
{
    Q_D(QMediaStreams);

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

#include "moc_qmediastreams.cpp"
