
#ifndef QMEDIASINK_H
#define QMEDIASINK_H

#include <QString>
#include <QVariant>


class QMediaSinkPrivate;
class QMediaSink : public QObject
{
public:
    QMediaSink();
    ~QMediaSink();

    QString mimeType() const;
    void setMimeType(QString const& mimeType);

    QVariant dataLocation() const;
    void setDataLocation(QVariant const& url);

private:
    Q_DECLARE_PRIVATE(QMediaSink)
};

#endif  // QMEDIASINK_H



