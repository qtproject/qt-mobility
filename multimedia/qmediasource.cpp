#include "qmediasource.h"

class QMediaSourcePrivate : public QSharedData
{
public:
    QMediaSourcePrivate()
        :QSharedData() { ref.ref(); }

    QMediaSourcePrivate(const QString &mimeType, const QVariant &url)
        :QSharedData(),
         mimeType(mimeType),
         url(url),
         duration(-1)
    {}

    QMediaSourcePrivate(const QMediaSourcePrivate &other)
        :QSharedData(other),
         mimeType(other.mimeType),
         url(other.url),
         duration(other.duration)
    {}

    ~QMediaSourcePrivate() {}

    QString mimeType;
    QVariant url;
    int duration;
};

Q_GLOBAL_STATIC(QMediaSourcePrivate, qt_sharedMediaSource)

/*!
    \class QMediaSource
    \ingroup multimedia

    \preliminary
    \brief The QMediaSource class provides an media source location.

    \sa
*/

/*!
*/
QMediaSource::QMediaSource()
    :d(qt_sharedMediaSource())
{
}

/*!
*/
QMediaSource::QMediaSource(const QString &mimeType, const QVariant &url)
    :d(new QMediaSourcePrivate(mimeType, url))
{
}

/*!
*/
QMediaSource::QMediaSource(const QMediaSource &other)
    :d(other.d)
{
}

/*!
*/
QMediaSource &QMediaSource::operator =(const QMediaSource &other)
{
    d = other.d;
    return *this;
}

/*!
*/
QMediaSource::~QMediaSource()
{
}

/*!
*/
bool QMediaSource::isNull() const
{
    return d == qt_sharedMediaSource();
}

/*!
*/
QString QMediaSource::mimeType() const
{
    return d->mimeType;
}

/*!
*/
void QMediaSource::setMimeType(QString const& mimeType)
{
    d->mimeType = mimeType;
}

/*!
*/
QVariant QMediaSource::dataLocation() const
{
    return d->url;
}

/*!
*/
void QMediaSource::setDataLocation(QVariant const& url)
{
    d->url = url;
}

/*!
  Return the duration of media in miliseconds
  or -1 if duration is not available.
*/
int QMediaSource::duration() const
{
    return d->duration;
}

/*!
  Set the duration of media in miliseconds.  
*/
void QMediaSource::setDuration(int ms)
{
    d->duration = ms;
}

/*!
*/
bool QMediaSource::operator ==(const QMediaSource& other) const
{
    return d->url == other.d->url;
}

/*!
*/
bool QMediaSource::operator !=(const QMediaSource& other) const
{
    return d->url != other.d->url;
}
