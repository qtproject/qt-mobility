#ifndef QM3UHANDLER_H
#define QM3UHANDLER_H

#include "qmediaplaylistioplugin.h"

QT_BEGIN_NAMESPACE

class QM3uPlaylistPlugin : public QMediaPlaylistIOPlugin
{
Q_OBJECT
public:
    explicit QM3uPlaylistPlugin(QObject *parent = 0);
    virtual ~QM3uPlaylistPlugin();

    virtual bool canRead(QIODevice *device, const QByteArray &format = QByteArray() ) const;
    virtual bool canRead(const QString& location, const QByteArray &format = QByteArray()) const;

    virtual bool canWrite(QIODevice *device, const QByteArray &format) const;

    virtual QStringList keys() const;

    virtual QMediaPlaylistReader *createReader(QIODevice *device, const QByteArray &format = QByteArray());
    virtual QMediaPlaylistReader *createReader(const QString& location, const QByteArray &format = QByteArray());

    virtual QMediaPlaylistWritter *createWritter(QIODevice *device, const QByteArray &format);
};

QT_END_NAMESPACE

#endif // QM3UHANDLER_H
