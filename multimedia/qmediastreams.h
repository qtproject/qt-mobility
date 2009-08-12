#ifndef QMEDIASTREAMS_H
#define QMEDIASTREAMS_H

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtCore/qshareddata.h>

#include "qabstractmediaobject.h"

class QMediaStreams;
class QMediaStreamInfoPrivate;
class QMediaStreamInfo
{
public:
    enum StreamType { UnknownStream, VideoStream, AudioStream, SubPictureStream, DataStream };
    QMediaStreamInfo();
    QMediaStreamInfo(StreamType type, int streamId);
    QMediaStreamInfo(const QMediaStreamInfo &other);
    QMediaStreamInfo &operator =(const QMediaStreamInfo &other);
    ~QMediaStreamInfo();

    bool isNull() const;

    StreamType type() const;
    int streamId() const;    

    QString streamName() const;
    void setStreamName(const QString &name);

    QString streamLanguage() const;
    void setStreamLanguage(const QString &language);

    QVariant property(const QString &name) const;
    QStringList propertyNames() const;
    void setProperty(const QString &name, const QVariant &value);
    void clearProperty(const QString &name);

    QMap<QString,QVariant> properties() const;
    void setProperties(const QMap<QString,QVariant> &properties);

private:
    QSharedDataPointer<QMediaStreamInfoPrivate> d;
};


class QMediaStreamsPrivate;
class QMediaStreams : public QObject
{
Q_OBJECT
public:
    QMediaStreams(QAbstractMediaObject *mediaObject, QObject *parent = 0);
    virtual ~QMediaStreams();

    bool isStreamsInformationAvailable() const;

    QList<QMediaStreamInfo> streams(QMediaStreamInfo::StreamType streamType) const;    

    QMediaStreamInfo activeStream(QMediaStreamInfo::StreamType streamType) const;
    void setActiveStream(const QMediaStreamInfo &stream);

Q_SIGNALS:
    void streamsChanged();
    void activeStreamsChanged();

protected:
    QMediaStreamsPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QMediaStreams)
    Q_DISABLE_COPY(QMediaStreams)
    Q_PRIVATE_SLOT(d_func(), void _q_updateStreams());
    Q_PRIVATE_SLOT(d_func(), void _q_updateActiveStreams());
};


#endif // QMEDIASTREAMS_H
