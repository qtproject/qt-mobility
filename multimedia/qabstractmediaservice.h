#ifndef QABSTRACTMEDIASERVICE_H
#define QABSTRACTMEDIASERVICE_H

#include <QtCore/qobject.h>
#include <QtCore/qlist.h>

class QAbstractMediaControl;

class QAbstractMediaServicePrivate;

class QAbstractMediaService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* audioOutput READ audioOutput WRITE setAudioOutput)
    Q_PROPERTY(QObject* videoOutput READ videoOutput WRITE setVideoOutput)
public:
    QObject *audioOutput() const;
    virtual void setAudioOutput(QObject *object);

    QObject *videoOutput() const;
    virtual void setVideoOutput(QObject *output);

    virtual QList<QByteArray> supportedEndpointInterfaces() const;

    virtual QObject *createEndpoint(const char *interface);

    virtual QAbstractMediaControl* control() const = 0;

protected:
    QAbstractMediaService(QObject* parent);

private:
    Q_DECLARE_PRIVATE(QAbstractMediaService)
};


#endif  // QABSTRACTMEDIASERVICE_H

