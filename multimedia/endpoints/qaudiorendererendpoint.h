#ifndef QAUDIORENDERERENDPOINT_H
#define QAUDIORENDERERENDPOINT_H

#include <QtCore/qplugin.h>

class QAudioRendererEndpointInterface
{
public:
    virtual ~QAudioRendererEndpointInterface();
};

#define QAudioRendererEndpointInterface_iid "com.nokia.Qt.QAudioRendererEndpointInterface/1.0"

Q_DECLARE_INTERFACE(QAudioRendererEndpointInterface, QAudioRendererEndpointInterface_iid)

class QIODevice;

class QAudioRendererEndpointPrivate;

class QAudioRendererEndpoint : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency)
    Q_PROPERTY(int channels READ channels WRITE setChannels)
    Q_PROPERTY(int sampleSize READ sampleSize WRITE setSampleSize)
    Q_DECLARE_PRIVATE(QAudioRendererEndpoint)
public:
    QAudioRendererEndpoint(QObject *parent = 0);
    ~QAudioRendererEndpoint();

    int frequency() const;
    virtual void setFrequency(int frequency);
    virtual QList<int> supportedFrequencies() const = 0;

    int channels() const;
    virtual void setChannels(int channels);
    virtual QList<int> supportedChannels() const = 0;

    int sampleSize() const;
    virtual void setSampleSize(int size);
    virtual QList<int> supportedSampleSizes() const = 0;

    QIODevice *device() const;
    virtual void setDevice(QIODevice *device);
};

#endif
