#ifndef QABSTRACTMEDIASERVICE_H
#define QABSTRACTMEDIASERVICE_H

#include <QtCore/qobject.h>
#include <QtCore/qlist.h>

class QAbstractMediaControl;
class QMediaAudioOutput;
class QMediaVideoOutput;

class QAbstractMediaServicePrivate;
class QAbstractMediaService : public QObject
{
    Q_OBJECT

public:
    QMediaAudioOutput audioOutput() const;
    void setAudioOutput(QMediaAudioOutput const &audioOutput);
    QList<QMediaAudioOutput> availableAudioOutputs() const;

    QMediaAudioOutput videoOutput() const;
    void setVideoOutput(QMediaVideoOutput const &videoOutput);
    QList<QMediaVideoOutput> availableVideoOutputs() const;

    virtual QAbstractMediaControl* control() const = 0;

protected:
    QAbstractMediaService(QObject* parent);

private:
    Q_DECLARE_PRIVATE(QAbstractMediaService)
};


#endif  // QABSTRACTMEDIASERVICE_H

