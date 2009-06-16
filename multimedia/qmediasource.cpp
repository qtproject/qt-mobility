#include "qmediasource.h"

class QMediaSourcePrivate : public QSharedData
{
public:
    QMediaSourcePrivate()
        :QSharedData() {}

    QMediaSourcePrivate(const QString &mimeType, const QVariant &url)
        :QSharedData(),
         mimeType(mimeType),
         url(url)
    {}

    QMediaSourcePrivate(const QMediaSourcePrivate &other)
        :QSharedData(other),
         mimeType(other.mimeType),
         url(other.url)
    {}

    ~QMediaSourcePrivate() {}

    QString mimeType;
    QVariant url;
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
