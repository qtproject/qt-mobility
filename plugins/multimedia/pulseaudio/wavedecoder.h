#ifndef WAVEDECODER_H
#define WAVEDECODER_H

#include <QtCore/qiodevice.h>
#include <QtMultimedia/qaudioformat.h>

class WaveDecoder : public QIODevice
{
Q_OBJECT

public:
    explicit WaveDecoder(QIODevice *source, QObject *parent = 0);
    ~WaveDecoder();

    QAudioFormat audioFormat() const;
    int duration() const;

    qint64 size() const;
    bool isSequential() const;
    qint64 bytesAvailable() const;

signals:
    void formatKnown();
    void invalidFormat();

private slots:
    void handleData();

private:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);

    struct chunk
    {
        char        id[4];
        quint32     size;
    };
    struct RIFFHeader
    {
        chunk       descriptor;
        char        type[4];
    };
    struct WAVEHeader
    {
        chunk       descriptor;
        quint16     audioFormat;
        quint16     numChannels;
        quint32     sampleRate;
        quint32     byteRate;
        quint16     blockAlign;
        quint16     bitsPerSample;
    };
    struct DATAHeader
    {
        chunk       descriptor;
    };
    struct CombinedHeader
    {
        RIFFHeader  riff;
        WAVEHeader  wave;
        DATAHeader  data;
    };

    bool haveFormat;
    qint64 dataSize;
    qint64 remaining;
    QAudioFormat format;
    QIODevice *source;
    CombinedHeader header;
};

#endif // WAVEDECODER_H
