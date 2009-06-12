
#ifndef QMEDIASOURCE_H
#define QMEDIASOURCE_H

#include <QObject>
#include <QString>
#include <QVariantl>


class QMediaSourcePrivate;

class QMediaSource : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType)
    Q_PROPERTY(QVariant dataLocation READ dataLocation WRITE setDataLocation)

public:
    QMediaSource(QObject* parent = 0);
    ~QMediaSource();

    QString mimeType() const;
    void setMimeType(QString const& mimeType);

    QVariant dataLocation() const
    void setDataLocation(QVariant const& url);

private:
    Q_DECLARE_PRIVATE(QMediaSource)
};

#endif  // QMEDIASOURCE_H
