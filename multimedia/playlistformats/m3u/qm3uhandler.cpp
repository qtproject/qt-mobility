#include "qm3uhandler.h"
#include "qmediasource.h"
#include <QtCore/qiodevice.h>
#include <QtCore/qfileinfo.h>
#include <QFile>

QT_BEGIN_NAMESPACE

class QM3uPlaylistReader : public QMediaPlaylistReader
{
public:
    QM3uPlaylistReader(QIODevice *device)
        :m_ownDevice(false), m_device(device)
    {
    }

    QM3uPlaylistReader(const QString& location)
        :m_ownDevice(true)
    {
        QFile *f = new QFile(location);
        if (f->open(QIODevice::ReadOnly | QIODevice::Text)) {
            m_device = f;
        } else {
            delete f;
            m_device = 0;
        }
    }

    virtual ~QM3uPlaylistReader()
    {
        if (m_ownDevice) {
            delete m_device;
        }
    }

    virtual bool atEnd() const
    {
        return m_device == 0 || m_device->atEnd();
    }

    virtual QMediaSource readItem()
    {
        while (!atEnd()) {
            QString line = QString::fromLocal8Bit(m_device->readLine());
            if (line.isEmpty() || line[0] == '#')
                continue;

            return QMediaSource(QString(), line);
        }

        return QMediaSource();
    }

    virtual void close()
    {
    }

private:
    bool m_ownDevice;
    QIODevice *m_device;
};

class QM3uPlaylistWritter : public QMediaPlaylistWritter
{
public:
    QM3uPlaylistWritter(QIODevice *device)
        :m_device(device)
    {
    }

    virtual ~QM3uPlaylistWritter()
    {
    }

    virtual bool writeItem(const QMediaSource& item)
    {
        QString location = item.dataLocation().toString()+QChar::Separator_Line;
        return m_device->write(location.toLocal8Bit()) > 0;
    }

    virtual void close()
    {
    }

private:
    QIODevice *m_device;
};


QM3uPlaylistPlugin::QM3uPlaylistPlugin(QObject *parent)
    :QMediaPlaylistIOPlugin(parent)
{
}

QM3uPlaylistPlugin::~QM3uPlaylistPlugin()
{
}

bool QM3uPlaylistPlugin::canRead(QIODevice *device, const QByteArray &format) const
{
    return device->isReadable() && (format == "m3u" || format.isEmpty());
}

bool QM3uPlaylistPlugin::canRead(const QString& location, const QByteArray &format) const
{
    if (!QFileInfo(location).isReadable())
        return false;

    if (format == "m3u")
        return true;

    if (!format.isEmpty())
        return false;
    else
        return location.toLower().endsWith(QLatin1String("m3u"));
}

bool QM3uPlaylistPlugin::canWrite(QIODevice *device, const QByteArray &format) const
{
    return device->isOpen() && device->isWritable() && format == "m3u";
}

QStringList QM3uPlaylistPlugin::keys() const
{
    return QStringList() << QLatin1String("m3u");
}

QMediaPlaylistReader *QM3uPlaylistPlugin::createReader(QIODevice *device, const QByteArray &format)
{
    Q_UNUSED(format);
    return new QM3uPlaylistReader(device);
}

QMediaPlaylistReader *QM3uPlaylistPlugin::createReader(const QString& location, const QByteArray &format)
{
    Q_UNUSED(format);
    return new QM3uPlaylistReader(location);
}

QMediaPlaylistWritter *QM3uPlaylistPlugin::createWritter(QIODevice *device, const QByteArray &format)
{
    Q_UNUSED(format);
    return new QM3uPlaylistWritter(device);
}


QT_END_NAMESPACE
