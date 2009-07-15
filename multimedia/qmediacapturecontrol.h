#ifndef QMEDIACAPTURECONTROL_H
#define QMEDIACAPTURECONTROL_H

#include "qabstractmediacontrol.h"

class QMediaSink;

class QMediaCaptureControl : public QAbstractMediaControl
{
Q_OBJECT
public:
    virtual ~QMediaCaptureControl();

    virtual QMediaSink sink() const = 0;
    virtual bool setSink(const QMediaSink &sink) = 0;

    virtual int state() = 0;

    virtual qint64 position() const = 0;
    virtual void setPositionUpdatePeriod(int ms) = 0;

signals:
    void stateChanged(int state);
    void positionChanged(qint64 position);
    void error(int error, const QString &errorString);

public slots:
    virtual void start() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

protected:    
    QMediaCaptureControl(QObject* parent);
};

#endif // QMEDIACAPTURECONTROL_H
