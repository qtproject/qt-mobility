
#include "qaudiorendererendpoint.h"

#include <private/qobject_p.h>

/*!
    \class QAudioRendererEndpointInterface
    \preliminary
    \internal
    \brief The QAudioRendererEndpointInterface class provides an interface for audio renderer
    media end points.
*/

/*!
*/
QAudioRendererEndpointInterface::~QAudioRendererEndpointInterface()
{
}

class QAudioRendererEndpointPrivate : public QObjectPrivate
{
public:
    QAudioRendererEndpointPrivate()
        : frequency(0)
        , channels(1)
        , sampleSize(8)
        , device(0)
    {
    }

    int frequency;
    int channels;
    int sampleSize;
    QIODevice *device;
};

/*!
    \class QAudioRendererEndpoint
    \preliminary
    \brief The QAudioRendererEndpoint class provides a media end point that renders audio to a
    QIODevice
*/

/*!
    Constructs a new audio renderer end point with the given \a parent.
*/
QAudioRendererEndpoint::QAudioRendererEndpoint(QObject *parent)
    : QObject(*new QAudioRendererEndpointPrivate, parent)
{
}

/*!
    Destroys an audio renderer end point.
*/
QAudioRendererEndpoint::~QAudioRendererEndpoint()
{
}

/*!
    Returns the frequency of the rendered audio.
*/
int QAudioRendererEndpoint::frequency() const
{
    return d_func()->frequency;
}

/*!
    Sets the \a frequency of the rendered audio.
*/
void QAudioRendererEndpoint::setFrequency(int frequency)
{
    d_func()->frequency = frequency;
}

/*!
    \fn QAudioRendererEndpoint::supportedFrequencies() const

    Returns a list of frequencies supported by an audio renderer.
*/

/*!
    Returns the number of channels in the renderered audio.
*/
int QAudioRendererEndpoint::channels() const
{
    return d_func()->channels;
}

/*!
    Sets the number of \a channels in the renderered audio.
*/
void QAudioRendererEndpoint::setChannels(int channels)
{
    d_func()->channels = channels;
}

/*!
    \fn QAudioRendererEndpoint::supportedChannels() const

    Returns a list of the numbers of channels supported by an audio renderer.
*/

/*!
    Returns the sample size of the rendered audio.
*/
int QAudioRendererEndpoint::sampleSize() const
{
    return d_func()->sampleSize;
}

/*!
    Sets the sample \a size of the rendered audio.
*/
void QAudioRendererEndpoint::setSampleSize(int size)
{
    d_func()->sampleSize = size;
}

/*!
    \fn QList<int> QAudioRendererEndpoint::supportedSampleSizes() const

    Returns a list of sample sizes supported by an audio renderer.
*/

/*!
    Returns the I/O device audio is rendered to.
*/
QIODevice *QAudioRendererEndpoint::device() const
{
    return d_func()->device;
}

/*!
    Sets the I/O \a device audio is rendered to.
*/
void QAudioRendererEndpoint::setDevice(QIODevice *device)
{
    d_func()->device = device;
}
