#include "wavedecoder.h"

#include <QtCore/qtimer.h>
#include <QtCore/qendian.h>
#include <QDebug>

WaveDecoder::WaveDecoder(QIODevice *s, QObject *parent):
    QIODevice(parent),
    haveFormat(false),
    dataSize(0),
    remaining(0),
    source(s)
{
    open(QIODevice::ReadOnly | QIODevice::Unbuffered);

    if (source->bytesAvailable() >= sizeof(CombinedHeader))
        QTimer::singleShot(0, this, SLOT(handleData()));
    else
        connect(source, SIGNAL(readyRead()), SLOT(handleData()));
}

WaveDecoder::~WaveDecoder()
{
}

QAudioFormat WaveDecoder::audioFormat() const
{
    return format;
}

int WaveDecoder::duration() const
{
    return size() * 1000 / (format.sampleSize() / 8) / format.channels() / format.frequency();
}

qint64 WaveDecoder::size() const
{
    return haveFormat ? dataSize : 0;
}

bool WaveDecoder::isSequential() const
{
    return source->isSequential();
}

qint64 WaveDecoder::bytesAvailable() const
{
    return haveFormat ? source->bytesAvailable() : 0;
}

qint64 WaveDecoder::readData(char *data, qint64 maxlen)
{
    return haveFormat ? source->read(data, maxlen) : 0;
}

qint64 WaveDecoder::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return -1;
}

void WaveDecoder::handleData()
{
    if (source->bytesAvailable() < sizeof(CombinedHeader))
        return;

    source->disconnect(SIGNAL(readyRead()), this, SLOT(handleData()));
    source->read((char*)&header, sizeof(CombinedHeader));

    if (qstrncmp(header.riff.descriptor.id, "RIFF", 4) != 0 ||
        qstrncmp(header.riff.type, "WAVE", 4) != 0 ||
        qstrncmp(header.wave.descriptor.id, "fmt ", 4) != 0 ||
        (header.wave.audioFormat != 0 && header.wave.audioFormat != 1) ||
        qstrncmp(header.data.descriptor.id, "data", 4) != 0) {

        emit invalidFormat();
    }
    else {
        int bps = qFromLittleEndian<quint16>(header.wave.bitsPerSample);

        format.setCodec("audio/pcm");
        format.setSampleType(bps == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setFrequency(qFromLittleEndian<quint32>(header.wave.sampleRate));
        format.setSampleSize(bps);
        format.setChannels(qFromLittleEndian<quint16>(header.wave.numChannels));

        dataSize = qFromLittleEndian<quint32>(header.data.descriptor.size);

        haveFormat = true;
        connect(source, SIGNAL(readyRead()), SIGNAL(readyRead()));
        emit formatKnown();
    }
}
