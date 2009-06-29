
#ifndef QMEDIASINK_H
#define QMEDIASINK_H

#include <QObject>
#include <QString>
#include <QVariant>


class QMediaSinkPrivate;

class QMediaSink : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType)
    Q_PROPERTY(QVariant dataLocation READ dataLocation WRITE setDataLocation)

public:
    QMediaSink(QObject* parent = 0);
    ~QMediaSink();

    QString mimeType() const;
    void setMimeType(QString const& mimeType);

    QVariant dataLocation() const;
    void setDataLocation(QVariant const& url);

private:
    Q_DECLARE_PRIVATE(QMediaSink)
};

#endif  // QMEDIASINK_H



