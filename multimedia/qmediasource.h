#ifndef QMEDIASOURCE_H
#define QMEDIASOURCE_H

#include <QString>
#include <QVariant>
#include <QSharedDataPointer>

class QMediaSourcePrivate;
class QMediaSource
{
public:
    QMediaSource();
    QMediaSource(const QString &mimeType, const QVariant &url);
    QMediaSource(const QMediaSource &other);
    QMediaSource &operator =(const QMediaSource &other);
    ~QMediaSource();

    bool isNull() const;

    QString mimeType() const;
    void setMimeType(const QString &mimeType);

    QVariant dataLocation() const;
    void setDataLocation(const QVariant &url);

    int duration() const;
    void setDuration(int ms);

    bool operator ==(const QMediaSource& other) const;
    bool operator !=(const QMediaSource& other) const;

private:
    QSharedDataPointer<QMediaSourcePrivate> d;
};

Q_DECLARE_METATYPE(QMediaSource);

#endif  // QMEDIASOURCE_H
